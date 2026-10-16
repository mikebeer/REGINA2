/*
 *  The Regina Rexx Interpreter - object-oriented extension
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  See oo.h for the overall design.
 */

#include "rexx.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* helpers in variable.c and funcs.c */
var_hashtable *oo_pool_new( const tsd_t *TSD );
const streng *oo_pool_get( const tsd_t *TSD, var_hashtable *pool, const streng *name );
void oo_pool_set( const tsd_t *TSD, var_hashtable *pool, const streng *name, streng *value );
void oo_expose( tsd_t *TSD, var_hashtable *src, const streng *name );
int oo_stem_tails( tsd_t *TSD, const streng *stemname,
                   void (*cb)( tsd_t *, void *, const streng *, const streng * ),
                   void *data );
streng *(*oo_find_bif( tsd_t *TSD, const char *name ))( tsd_t *, cparamboxptr );
int oo_exit_protected( tsd_t *TSD, void (*fn)( tsd_t *, void * ), void *data );

/* ------------------------------------------------------------------ */
/* data structures                                                    */
/* ------------------------------------------------------------------ */

enum {
   OK_PLAIN = 0, OK_CLASS, OK_NIL, OK_ARRAY, OK_DIRECTORY, OK_TABLE,
   OK_QUEUE, OK_BUFFER, OK_SUPER, OK_SUPPLIER, OK_STRING
};

#define MF_CLASS     0x0001   /* class method                    */
#define MF_PRIVATE   0x0002
#define MF_ATTR_GET  0x0004
#define MF_ATTR_SET  0x0008
#define MF_CONSTANT  0x0010
#define MF_ABSTRACT  0x0020

struct oo_object;
struct oo_call;
typedef streng *(*oo_native)( tsd_t *TSD, struct oo_call *c );

typedef struct oo_labels {
   int n;
   nodeptr *v;
} oo_labels;

typedef struct oo_program {
   struct oo_program *next;
   internal_parser_type tree;   /* kept alive for the whole run      */
   streng *file;                /* source file name                  */
   oo_labels labels;            /* labels of the prolog code         */
   int processed;
} oo_program;

typedef struct oo_method {
   struct oo_method *next;
   streng *name;                /* uppercase message name            */
   int flags;
   nodeptr body;                /* statements, may be NULL           */
   oo_labels labels;            /* labels inside the body            */
   oo_program *prog;
   streng *attr;                /* variable of an attribute method   */
   streng *constval;            /* value of a ::CONSTANT             */
   oo_native native;
} oo_method;

typedef struct oo_classdata {
   streng *name;                /* as displayed                      */
   streng *id;                  /* uppercase, for lookups            */
   struct oo_object *super;
   streng *supername;           /* until resolved                    */
   struct oo_object **inherits;
   streng **inheritnames;
   int ninherits;
   struct oo_object **mro;      /* lookup order, mro[0] == the class */
   int nmro;
   oo_method *imethods;         /* instance methods                  */
   oo_method *cmethods;         /* class methods                     */
   int ikind;                   /* kind of the instances             */
   int mixin;
   int resolved;
   oo_program *prog;
} oo_classdata;

typedef struct oo_scope {
   struct oo_scope *next;
   struct oo_object *cls;       /* the class defining the variables  */
   var_hashtable *pool;
   struct oo_object *superproxy;
} oo_scope;

typedef struct oo_entry {        /* Directory/Table entry             */
   struct oo_entry *hnext;      /* hash chain                        */
   struct oo_entry *onext, *oprev; /* insertion order                */
   streng *key;
   streng *value;
   unsigned hash;
} oo_entry;

typedef struct oo_hash {
   oo_entry **tbl;
   unsigned size, count;
   oo_entry *first, *last;
} oo_hash;

typedef struct oo_object {
   unsigned long id;
   struct oo_object *cls;
   int kind;
   int snapshot;                /* internal DO OVER snapshot         */
   streng *handle;
   streng *objname;             /* set by OBJECTNAME=                */
   oo_scope *scopes;
   union {
      oo_classdata *c;
      struct { streng **v; long size, cap; } a;
      oo_hash *h;
      streng *s;                /* MutableBuffer and String values   */
      struct { struct oo_object *self; struct oo_object *start; } sup;
      struct { struct oo_object *items, *indexes; long pos; } sp;
   } u;
} oo_object;

typedef struct oo_frame {        /* stored in proclevel->oo_ctx       */
   oo_object *self;             /* NULL in a routine                 */
   oo_object *scope;
   oo_labels *labels;
} oo_frame;

typedef struct oo_routine {
   struct oo_routine *next;
   streng *name;
   nodeptr body;
   oo_labels labels;
   oo_program *prog;
} oo_routine;

typedef struct oo_call {
   const streng *selfstr;       /* receiver as handle or string      */
   oo_object *self;             /* NULL for a plain string           */
   oo_object *scope;            /* class the method was found in     */
   const streng *name;          /* message name                      */
   int argc;
   const streng **argv;         /* argv[i] may be NULL (omitted)     */
} oo_call;

typedef struct oo_tsd {
   oo_object **objs;
   unsigned long nobjs, cap;
   oo_object *cObject, *cClass, *cString, *cArray, *cDirectory, *cTable,
             *cStringTable, *cQueue, *cList, *cBuffer, *cSupplier, *cRelation;
   oo_object *nil;
   oo_object *environment;      /* a Directory                       */
   oo_program *programs;
   oo_routine *routines;
   oo_object **classes;         /* all classes, for ".NAME" lookups  */
   int nclasses, capclasses;
   streng *s_self, *s_super, *s_string, *s_init, *s_unknown, *s_makearray;
   int overcount;
} oo_tsd;

/* ------------------------------------------------------------------ */
/* basic helpers                                                      */
/* ------------------------------------------------------------------ */

static void init_builtin_classes( tsd_t *TSD, oo_tsd *ot );

static oo_tsd *get_ot( tsd_t *TSD )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;

   if ( ot == NULL )
   {
      ot = (oo_tsd *) MallocTSD( sizeof( oo_tsd ) );
      memset( ot, 0, sizeof( oo_tsd ) );
      TSD->oo_tsd = ot;
      TSD->oo_active = 1;
      ot->s_self = Str_creTSD( "SELF" );
      ot->s_super = Str_creTSD( "SUPER" );
      ot->s_string = Str_creTSD( "STRING" );
      ot->s_init = Str_creTSD( "INIT" );
      ot->s_unknown = Str_creTSD( "UNKNOWN" );
      ot->s_makearray = Str_creTSD( "MAKEARRAY" );
      init_builtin_classes( TSD, ot );
   }
   return ot;
}

static streng *upper_dup( tsd_t *TSD, const streng *s )
{
   streng *r = Str_dupTSD( s );
   mem_upper( r->value, r->len );
   return r;
}

static int str_eq( const streng *a, const streng *b )
{
   return a->len == b->len && memcmp( a->value, b->value, a->len ) == 0;
}

static int str_eqc( const streng *a, const char *b )
{
   int l = (int) strlen( b );
   return a->len == l && memcmp( a->value, b, l ) == 0;
}

static streng *mkstr( tsd_t *TSD, const char *s )
{
   return Str_creTSD( s );
}

static streng *mkint( tsd_t *TSD, long n )
{
   char buf[32];
   sprintf( buf, "%ld", n );
   return Str_creTSD( buf );
}

static streng *mkbool( tsd_t *TSD, int b )
{
   return Str_creTSD( b ? "1" : "0" );
}

static streng *catc( tsd_t *TSD, streng *base, const char *s )
{
   return Str_catstrTSD( base, s );
}

static streng *cats( tsd_t *TSD, streng *base, const streng *s )
{
   return Str_catTSD( base, s );
}

/* ------------------------------------------------------------------ */
/* object table and handles                                           */
/* ------------------------------------------------------------------ */

static oo_object *new_object( tsd_t *TSD, oo_object *cls, int kind )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *o;
   unsigned long id;
   char hex[9];

   if ( ot->nobjs + 1 >= ot->cap )
   {
      unsigned long ncap = ot->cap ? ot->cap * 2 : 256;
      oo_object **n = (oo_object **) MallocTSD( ncap * sizeof( oo_object * ) );
      memset( n, 0, ncap * sizeof( oo_object * ) );
      if ( ot->objs )
      {
         memcpy( n, ot->objs, ot->cap * sizeof( oo_object * ) );
         FreeTSD( ot->objs );
      }
      ot->objs = n;
      ot->cap = ncap;
   }
   id = ++ot->nobjs;

   o = (oo_object *) MallocTSD( sizeof( oo_object ) );
   memset( o, 0, sizeof( oo_object ) );
   o->id = id;
   o->cls = cls;
   o->kind = kind;
   o->handle = Str_makeTSD( OO_HANDLE_LEN );
   o->handle->len = OO_HANDLE_LEN;
   memcpy( o->handle->value, "\000\001OBJ\002", 6 );
   sprintf( hex, "%08lX", id );
   memcpy( o->handle->value + 6, hex, 8 );
   o->handle->value[14] = '\003';
   o->handle->value[15] = '\000';
   ot->objs[id] = o;
   return o;
}

/* returns the object a string refers to or NULL */
static oo_object *obj_of( tsd_t *TSD, const streng *s )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   unsigned long id = 0;
   int i;
   char c;

   if ( ot == NULL || s == NULL || !OO_LOOKS_LIKE_HANDLE( s ) )
      return NULL;
   if ( memcmp( s->value, "\000\001OBJ\002", 6 ) != 0
     || s->value[14] != '\003' || s->value[15] != '\000' )
      return NULL;
   for ( i = 6; i < 14; i++ )
   {
      c = s->value[i];
      id <<= 4;
      if ( c >= '0' && c <= '9' )
         id += c - '0';
      else if ( c >= 'A' && c <= 'F' )
         id += c - 'A' + 10;
      else
         return NULL;
   }
   if ( id == 0 || id > ot->nobjs )
      return NULL;
   return ot->objs[id];
}

static void free_object( tsd_t *TSD, oo_object *o )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   long i;

   ot->objs[o->id] = NULL;
   if ( o->kind == OK_ARRAY || o->kind == OK_QUEUE )
   {
      for ( i = 0; i < o->u.a.size; i++ )
         if ( o->u.a.v[i] )
            Free_stringTSD( o->u.a.v[i] );
      if ( o->u.a.v )
         FreeTSD( o->u.a.v );
   }
   Free_stringTSD( o->handle );
   FreeTSD( o );
}

static streng *hdup( tsd_t *TSD, oo_object *o )
{
   return Str_dupTSD( o->handle );
}

/* ------------------------------------------------------------------ */
/* classes                                                            */
/* ------------------------------------------------------------------ */

static oo_object *class_by_id( oo_tsd *ot, const streng *id )
{
   int i;

   for ( i = ot->nclasses - 1; i >= 0; i-- )
      if ( str_eq( ot->classes[i]->u.c->id, id ) )
         return ot->classes[i];
   return NULL;
}

static oo_object *new_class( tsd_t *TSD, const char *cname, const streng *sname,
                             oo_object *super, int ikind )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *c;
   oo_classdata *cd;

   c = new_object( TSD, ot->cClass, OK_CLASS );
   cd = (oo_classdata *) MallocTSD( sizeof( oo_classdata ) );
   memset( cd, 0, sizeof( oo_classdata ) );
   c->u.c = cd;
   cd->name = sname ? Str_dupTSD( sname ) : Str_creTSD( cname );
   cd->id = upper_dup( TSD, cd->name );
   cd->super = super;
   if ( ikind >= 0 )
      cd->ikind = ikind;
   else
      cd->ikind = super ? super->u.c->ikind : OK_PLAIN;
   cd->resolved = 1;

   if ( ot->nclasses >= ot->capclasses )
   {
      int ncap = ot->capclasses ? ot->capclasses * 2 : 32;
      oo_object **n = (oo_object **) MallocTSD( ncap * sizeof( oo_object * ) );
      if ( ot->classes )
      {
         memcpy( n, ot->classes, ot->nclasses * sizeof( oo_object * ) );
         FreeTSD( ot->classes );
      }
      ot->classes = n;
      ot->capclasses = ncap;
   }
   ot->classes[ot->nclasses++] = c;
   return c;
}

static void mro_add( tsd_t *TSD, oo_object ***list, int *n, int *cap, oo_object *c )
{
   int i;

   for ( i = 0; i < *n; i++ )
      if ( (*list)[i] == c )
         return;
   if ( *n >= *cap )
   {
      int ncap = *cap ? *cap * 2 : 8;
      oo_object **nl = (oo_object **) MallocTSD( ncap * sizeof( oo_object * ) );
      if ( *list )
      {
         memcpy( nl, *list, *n * sizeof( oo_object * ) );
         FreeTSD( *list );
      }
      *list = nl;
      *cap = ncap;
   }
   (*list)[(*n)++] = c;
}

/*
 * The method lookup order of a class: the class itself, then its
 * INHERITed mixins (the last one first, each with its own order), then
 * the superclass with its order.
 */
static void compute_mro( tsd_t *TSD, oo_object *c )
{
   oo_classdata *cd = c->u.c;
   oo_object **list = NULL;
   int n = 0, cap = 0, i, j;

   if ( cd->mro )
      FreeTSD( cd->mro );
   mro_add( TSD, &list, &n, &cap, c );
   for ( i = cd->ninherits - 1; i >= 0; i-- )
   {
      oo_classdata *md = cd->inherits[i]->u.c;
      if ( md->mro == NULL )
         compute_mro( TSD, cd->inherits[i] );
      for ( j = 0; j < md->nmro; j++ )
         mro_add( TSD, &list, &n, &cap, md->mro[j] );
   }
   if ( cd->super )
   {
      oo_classdata *sd = cd->super->u.c;
      if ( sd->mro == NULL )
         compute_mro( TSD, cd->super );
      for ( j = 0; j < sd->nmro; j++ )
         mro_add( TSD, &list, &n, &cap, sd->mro[j] );
   }
   cd->mro = list;
   cd->nmro = n;
}

static oo_method *add_method( tsd_t *TSD, oo_object *c, const char *cname,
                              const streng *sname, oo_native fn, int flags )
{
   oo_method *m = (oo_method *) MallocTSD( sizeof( oo_method ) );

   memset( m, 0, sizeof( oo_method ) );
   m->name = sname ? upper_dup( TSD, sname ) : Str_creTSD( cname );
   m->native = fn;
   m->flags = flags;
   if ( flags & MF_CLASS )
   {
      m->next = c->u.c->cmethods;
      c->u.c->cmethods = m;
   }
   else
   {
      m->next = c->u.c->imethods;
      c->u.c->imethods = m;
   }
   return m;
}

static oo_method *find_in( oo_method *m, const streng *name )
{
   for ( ; m; m = m->next )
      if ( str_eq( m->name, name ) )
         return m;
   return NULL;
}

/*
 * Finds the method for message name sent to recv (NULL: a plain string).
 * If after is given, the search starts after that class (SUPER calls).
 */
static oo_method *lookup( tsd_t *TSD, oo_object *recv, const streng *name,
                          oo_object *after, oo_object **scope )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_classdata *cd;
   oo_method *m;
   int i, skipping = ( after != NULL );

   if ( recv && recv->kind == OK_CLASS )
   {
      cd = recv->u.c;
      if ( cd->mro == NULL )
         compute_mro( TSD, recv );
      for ( i = 0; i < cd->nmro; i++ )
      {
         if ( skipping )
         {
            if ( cd->mro[i] == after )
               skipping = 0;
            continue;
         }
         if ( ( m = find_in( cd->mro[i]->u.c->cmethods, name ) ) != NULL )
         {
            *scope = cd->mro[i];
            return m;
         }
      }
      skipping = 0;   /* class side done, now the methods of .Class */
      recv = NULL;
      cd = ot->cClass->u.c;
   }
   else
      cd = ( recv ? recv->cls : ot->cString )->u.c;

   if ( cd->mro == NULL )
      compute_mro( TSD, ( recv ? recv->cls : ot->cString ) );
   for ( i = 0; i < cd->nmro; i++ )
   {
      if ( skipping )
      {
         if ( cd->mro[i] == after )
            skipping = 0;
         continue;
      }
      if ( ( m = find_in( cd->mro[i]->u.c->imethods, name ) ) != NULL )
      {
         *scope = cd->mro[i];
         return m;
      }
   }
   return NULL;
}

static int is_subclass( oo_object *c, oo_object *of )
{
   int i;

   if ( c == of )
      return 1;
   if ( c->u.c->mro == NULL )
      return c->u.c->super ? is_subclass( c->u.c->super, of ) : 0;
   for ( i = 0; i < c->u.c->nmro; i++ )
      if ( c->u.c->mro[i] == of )
         return 1;
   return 0;
}

/* ------------------------------------------------------------------ */
/* object variable pools                                              */
/* ------------------------------------------------------------------ */

static oo_scope *get_scope( tsd_t *TSD, oo_object *o, oo_object *cls )
{
   oo_scope *s;

   for ( s = o->scopes; s; s = s->next )
      if ( s->cls == cls )
         return s;
   s = (oo_scope *) MallocTSD( sizeof( oo_scope ) );
   memset( s, 0, sizeof( oo_scope ) );
   s->cls = cls;
   s->pool = oo_pool_new( TSD );
   s->next = o->scopes;
   o->scopes = s;
   return s;
}

/* ------------------------------------------------------------------ */
/* names used in messages                                             */
/* ------------------------------------------------------------------ */

static streng *class_name_str( tsd_t *TSD, oo_object *c )
{
   return Str_dupTSD( c->u.c->name );
}

/* "an Account", "The Account class", "The NIL object" */
static streng *default_name( tsd_t *TSD, oo_object *o )
{
   streng *r;
   const streng *n;
   char ch;

   if ( o->objname )
      return Str_dupTSD( o->objname );
   if ( o->kind == OK_NIL )
      return mkstr( TSD, "The NIL object" );
   if ( o->kind == OK_CLASS )
   {
      r = mkstr( TSD, "The " );
      r = cats( TSD, r, o->u.c->name );
      return catc( TSD, r, " class" );
   }
   n = o->cls->u.c->name;
   ch = (char) rx_toupper( n->len ? n->value[0] : 'X' );
   r = mkstr( TSD, strchr( "AEIOU", ch ) ? "an " : "a " );
   return cats( TSD, r, n );
}

/* for error messages: a short description without calling methods */
static streng *describe( tsd_t *TSD, const streng *s )
{
   oo_object *o = obj_of( TSD, s );

   if ( o == NULL )
      return Str_dupTSD( s );
   return default_name( TSD, o );
}

/* ------------------------------------------------------------------ */
/* errors                                                             */
/* ------------------------------------------------------------------ */

static void err_nomethod( tsd_t *TSD, const streng *recv, const streng *name )
{
   streng *d = describe( TSD, recv );
   exiterror( ERR_METHOD_NOT_FOUND, 1, tmpstr_of( TSD, d ), tmpstr_of( TSD, name ) );
}

static const streng *need_arg( tsd_t *TSD, oo_call *c, int i )
{
   if ( i >= c->argc || c->argv[i] == NULL )
      exiterror( ERR_INVALID_ARGUMENT, 3, tmpstr_of( TSD, c->name ), i + 1 );
   return c->argv[i];
}

static const streng *opt_arg( oo_call *c, int i )
{
   return ( i < c->argc ) ? c->argv[i] : NULL;
}

static long whole_arg( tsd_t *TSD, oo_call *c, int i, int positive )
{
   const streng *s = need_arg( TSD, c, i );
   int err = 0;
   long v;

   v = streng_to_int( TSD, s, &err );
   if ( err || v < ( positive ? 1 : 0 ) )
   {
      streng *d = describe( TSD, s );
      exiterror( ERR_INVALID_ARGUMENT, positive ? 1 : 5, tmpstr_of( TSD, c->name ), i + 1,
                 tmpstr_of( TSD, d ) );
   }
   return v;
}

static oo_object *class_arg( tsd_t *TSD, oo_call *c, int i )
{
   const streng *s = need_arg( TSD, c, i );
   oo_object *o = obj_of( TSD, s );

   if ( o == NULL || o->kind != OK_CLASS )
   {
      streng *d = describe( TSD, s );
      exiterror( ERR_INVALID_ARGUMENT, 2, tmpstr_of( TSD, c->name ), i + 1,
                 tmpstr_of( TSD, d ) );
   }
   return o;
}

/* ------------------------------------------------------------------ */
/* running method bodies                                              */
/* ------------------------------------------------------------------ */

static paramboxptr make_plist( tsd_t *TSD, int argc, const streng **argv )
{
   paramboxptr first = NULL, last = NULL, p;
   int i;

   for ( i = 0; i < argc; i++ )
   {
      p = (paramboxptr) MallocTSD( sizeof( parambox ) );
      p->next = NULL;
      p->dealloc = 1;
      p->value = argv[i] ? Str_dupTSD( argv[i] ) : NULL;
      if ( last )
         last->next = p;
      else
         first = p;
      last = p;
   }
   return first;
}

/*
 * Runs a method or routine body in a new procedure level with a fresh
 * variable pool, like an internal function call with PROCEDURE.
 */
static streng *run_body( tsd_t *TSD, nodeptr body, oo_frame *frame,
                         paramboxptr args, const streng *selfstr )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   proclevel oldlevel = TSD->currlevel;
   nodeptr savenode = TSD->currentnode;
   int stackmark;
   streng *result;
   oo_scope *sc;

   if ( body == NULL )
   {
      if ( args )
         deallocplink( TSD, args );
      return NULL;
   }

   TSD->currlevel = newlevel( TSD, oldlevel );
   TSD->currlevel->args = args;
   TSD->currlevel->vars = create_new_varpool( TSD, 0 );
   TSD->currlevel->varflag = 1;
   TSD->currlevel->oo_ctx = frame;
   stackmark = pushcallstack( TSD, TSD->currentnode );

   if ( frame->self )
   {
      setvalue( TSD, ot->s_self, Str_dupTSD( selfstr ), -1 );
      sc = get_scope( TSD, frame->self, frame->scope );
      if ( sc->superproxy == NULL )
      {
         sc->superproxy = new_object( TSD, ot->cObject, OK_SUPER );
         sc->superproxy->u.sup.self = frame->self;
         sc->superproxy->u.sup.start = frame->scope;
      }
      setvalue( TSD, ot->s_super, hdup( TSD, sc->superproxy ), -1 );
   }

   result = interpret( TSD, body );

   TSD->currentnode = savenode;
   popcallstack( TSD, stackmark );
   removelevel( TSD, TSD->currlevel );
   TSD->currlevel = oldlevel;
   TSD->currlevel->next = NULL;
   TSD->trace_stat = TSD->currlevel->tracestat;
   return result;
}

static streng *invoke( tsd_t *TSD, oo_method *m, oo_object *scope,
                       const streng *selfstr, oo_object *self,
                       const streng *name, int argc, const streng **argv )
{
   oo_call c;
   oo_frame *frame;
   streng *result;
   const streng *v;

   if ( m->native )
   {
      c.selfstr = selfstr;
      c.self = self;
      c.scope = scope;
      c.name = name;
      c.argc = argc;
      c.argv = argv;
      return m->native( TSD, &c );
   }
   if ( m->flags & MF_CONSTANT )
      return Str_dupTSD( m->constval );
   if ( m->flags & MF_ABSTRACT )
      exiterror( ERR_EXECUTION_ERROR, 6, tmpstr_of( TSD, name ) );
   if ( m->flags & ( MF_ATTR_GET | MF_ATTR_SET ) )
   {
      oo_scope *sc;
      if ( self == NULL )
         err_nomethod( TSD, selfstr, name );
      sc = get_scope( TSD, self, scope );
      if ( m->flags & MF_ATTR_GET )
      {
         v = oo_pool_get( TSD, sc->pool, m->attr );
         return Str_dupTSD( v ? v : m->attr );
      }
      oo_pool_set( TSD, sc->pool, m->attr,
                   ( argc > 0 && argv[0] ) ? Str_dupTSD( argv[0] ) : nullstringptr() );
      return NULL;
   }

   frame = (oo_frame *) MallocTSD( sizeof( oo_frame ) );
   frame->self = self;
   frame->scope = scope;
   frame->labels = &m->labels;
   result = run_body( TSD, m->body, frame, make_plist( TSD, argc, argv ), selfstr );
   FreeTSD( frame );
   return result;
}

/*
 * Sends message name (uppercase) to recv. Returns the result or NULL if
 * the method returned nothing.
 */
static streng *send_from( tsd_t *TSD, const streng *recv, const streng *name,
                          oo_object *after, int argc, const streng **argv )
{
   oo_tsd *ot = get_ot( TSD );
   oo_object *o = obj_of( TSD, recv ), *scope = NULL;
   oo_method *m;
   oo_frame *f;

   if ( o && o->kind == OK_SUPER )
   {
      after = o->u.sup.start;
      o = o->u.sup.self;
      recv = o->handle;
   }

   m = lookup( TSD, o, name, after, &scope );
   if ( m == NULL )
   {
      /* UNKNOWN(name, arguments-array) */
      m = lookup( TSD, o, ot->s_unknown, NULL, &scope );
      if ( m != NULL )
      {
         oo_object *arr = new_object( TSD, ot->cArray, OK_ARRAY );
         const streng *uargv[2];
         streng *nm = Str_dupTSD( name ), *ah = hdup( TSD, arr ), *r;
         int i;
         for ( i = 0; i < argc; i++ )
         {
            extern void oo_array_put( tsd_t *, oo_object *, long, streng * );
            if ( argv[i] )
               oo_array_put( TSD, arr, i + 1, Str_dupTSD( argv[i] ) );
         }
         uargv[0] = nm;
         uargv[1] = ah;
         r = invoke( TSD, m, scope, recv, o, ot->s_unknown, 2, uargv );
         Free_stringTSD( nm );
         Free_stringTSD( ah );
         return r;
      }
      err_nomethod( TSD, recv, name );
   }
   if ( m->flags & MF_PRIVATE )
   {
      f = (oo_frame *) TSD->currlevel->oo_ctx;
      if ( f == NULL || f->self == NULL || f->self != o )
         err_nomethod( TSD, recv, name );
   }
   return invoke( TSD, m, scope, recv, o, name, argc, argv );
}

static streng *send_msg( tsd_t *TSD, const streng *recv, const streng *name,
                         int argc, const streng **argv )
{
   return send_from( TSD, recv, name, NULL, argc, argv );
}


/* ------------------------------------------------------------------ */
/* string conversion                                                  */
/* ------------------------------------------------------------------ */

streng *oo_stringify( tsd_t *TSD, const streng *s )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *o;
   streng *r;
   int depth;

   if ( ot == NULL || s == NULL || !OO_LOOKS_LIKE_HANDLE( s ) )
      return NULL;
   if ( ( o = obj_of( TSD, s ) ) == NULL )
      return NULL;
   if ( o->kind == OK_STRING )
      return Str_dupTSD( o->u.s );
   if ( o->kind == OK_BUFFER && o->cls == ot->cBuffer )
      return Str_dupTSD( o->u.s );
   r = send_msg( TSD, s, ot->s_string, 0, NULL );
   for ( depth = 0; r && obj_of( TSD, r ) && depth < 4; depth++ )
   {
      streng *t = default_name( TSD, obj_of( TSD, r ) );
      Free_stringTSD( r );
      r = t;
   }
   if ( r == NULL )
      r = default_name( TSD, o );
   return r;
}

void oo_stringify_inplace( tsd_t *TSD, streng **s, int *owned )
{
   streng *r;

   if ( *s == NULL || !OO_LOOKS_LIKE_HANDLE( *s ) )
      return;
   if ( ( r = oo_stringify( TSD, *s ) ) == NULL )
      return;
   if ( *owned )
      Free_stringTSD( *s );
   *s = r;
   *owned = 1;
}

/* string value of a receiver for String methods */
static const streng *strval( tsd_t *TSD, oo_call *c, streng **kill )
{
   *kill = NULL;
   if ( c->self == NULL )
      return c->selfstr;
   if ( c->self->kind == OK_STRING || c->self->kind == OK_BUFFER )
      return c->self->u.s;
   *kill = oo_stringify( TSD, c->selfstr );
   return *kill;
}

/* ------------------------------------------------------------------ */
/* Object methods                                                     */
/* ------------------------------------------------------------------ */

static oo_object *class_of( tsd_t *TSD, oo_call *c )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;

   if ( c->self == NULL )
      return ot->cString;
   if ( c->self->kind == OK_CLASS )
      return ot->cClass;
   return c->self->cls;
}

static streng *m_nothing( tsd_t *TSD, oo_call *c )
{
   return NULL;
}

static streng *m_defaultname( tsd_t *TSD, oo_call *c )
{
   if ( c->self == NULL )
      return Str_dupTSD( c->selfstr );
   return default_name( TSD, c->self );
}

static streng *m_setobjectname( tsd_t *TSD, oo_call *c )
{
   const streng *v = need_arg( TSD, c, 0 );
   streng *s;

   if ( c->self == NULL )
      return NULL;
   if ( c->self->objname )
      Free_stringTSD( c->self->objname );
   s = oo_stringify( TSD, v );
   c->self->objname = s ? s : Str_dupTSD( v );
   return NULL;
}

static streng *m_class( tsd_t *TSD, oo_call *c )
{
   return hdup( TSD, class_of( TSD, c ) );
}

static streng *m_isa( tsd_t *TSD, oo_call *c )
{
   oo_object *cls = class_arg( TSD, c, 0 );
   oo_object *mine = class_of( TSD, c );

   if ( mine->u.c->mro == NULL )
      compute_mro( TSD, mine );
   return mkbool( TSD, is_subclass( mine, cls ) );
}

static streng *m_identity_eq( tsd_t *TSD, oo_call *c )
{
   const streng *o = opt_arg( c, 0 );
   int eq = ( o != NULL && str_eq( o, c->selfstr ) );

   if ( str_eqc( c->name, "\\=" ) || str_eqc( c->name, "\\==" )
     || str_eqc( c->name, "<>" ) || str_eqc( c->name, "><" ) )
      eq = !eq;
   return mkbool( TSD, eq );
}

static streng *m_hashcode( tsd_t *TSD, oo_call *c )
{
   if ( c->self == NULL )
      return mkint( TSD, (long) hashvalue( c->selfstr->value, c->selfstr->len ) );
   return mkint( TSD, (long) c->self->id );
}

static streng *m_isnil( tsd_t *TSD, oo_call *c )
{
   return mkbool( TSD, c->self && c->self->kind == OK_NIL );
}

static streng *m_send( tsd_t *TSD, oo_call *c )
{
   streng *name, *r;
   const streng *n = need_arg( TSD, c, 0 );

   name = oo_stringify( TSD, n );
   if ( name == NULL )
      name = Str_dupTSD( n );
   mem_upper( name->value, name->len );
   r = send_msg( TSD, c->selfstr, name, c->argc - 1, c->argv + 1 );
   Free_stringTSD( name );
   return r;
}

extern long oo_array_items( tsd_t *TSD, oo_object *a, const streng ***out );

static streng *m_sendwith( tsd_t *TSD, oo_call *c )
{
   streng *name, *r;
   const streng *n = need_arg( TSD, c, 0 );
   oo_object *arr = obj_of( TSD, need_arg( TSD, c, 1 ) );
   const streng **items = NULL;
   long cnt;

   if ( arr == NULL || ( arr->kind != OK_ARRAY && arr->kind != OK_QUEUE ) )
   {
      streng *d = describe( TSD, c->argv[1] );
      exiterror( ERR_INVALID_ARGUMENT, 4, tmpstr_of( TSD, c->name ), 2, tmpstr_of( TSD, d ) );
   }
   name = oo_stringify( TSD, n );
   if ( name == NULL )
      name = Str_dupTSD( n );
   mem_upper( name->value, name->len );
   /* positional: holes stay omitted arguments */
   cnt = arr->u.a.size;
   items = (const streng **) MallocTSD( ( cnt + 1 ) * sizeof( streng * ) );
   memcpy( items, arr->u.a.v, cnt * sizeof( streng * ) );
   r = send_msg( TSD, c->selfstr, name, (int) cnt, items );
   FreeTSD( (void *) items );
   Free_stringTSD( name );
   return r;
}

static streng *m_hasmethod( tsd_t *TSD, oo_call *c )
{
   streng *name = upper_dup( TSD, need_arg( TSD, c, 0 ) );
   oo_object *scope;
   int has = lookup( TSD, c->self, name, NULL, &scope ) != NULL;

   Free_stringTSD( name );
   return mkbool( TSD, has );
}

static streng *m_request( tsd_t *TSD, oo_call *c )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   streng *name = upper_dup( TSD, need_arg( TSD, c, 0 ) );
   oo_object *scope;
   streng *r;

   if ( str_eqc( name, "STRING" ) )
   {
      Free_stringTSD( name );
      r = oo_stringify( TSD, c->selfstr );
      return r ? r : Str_dupTSD( c->selfstr );
   }
   name = Str_catstrTSD( name, "" );
   {
      streng *mname = Str_creTSD( "MAKE" );
      mname = cats( TSD, mname, name );
      if ( lookup( TSD, c->self, mname, NULL, &scope ) )
      {
         r = send_msg( TSD, c->selfstr, mname, 0, NULL );
         Free_stringTSD( mname );
         Free_stringTSD( name );
         return r;
      }
      Free_stringTSD( mname );
   }
   Free_stringTSD( name );
   return hdup( TSD, ot->nil );
}

extern oo_object *oo_copy_object( tsd_t *TSD, oo_object *o );

static streng *m_copy( tsd_t *TSD, oo_call *c )
{
   if ( c->self == NULL || c->self->kind == OK_CLASS || c->self->kind == OK_NIL )
      return Str_dupTSD( c->selfstr );
   return hdup( TSD, oo_copy_object( TSD, c->self ) );
}

static streng *m_instancemethod_names( tsd_t *TSD, oo_call *c );

/* ------------------------------------------------------------------ */
/* Class methods (sent to class objects)                              */
/* ------------------------------------------------------------------ */

static oo_object *cls_self( tsd_t *TSD, oo_call *c )
{
   if ( c->self == NULL || c->self->kind != OK_CLASS )
      err_nomethod( TSD, c->selfstr, c->name );
   return c->self;
}

static void resolve_class( tsd_t *TSD, oo_object *c );

static streng *m_new( tsd_t *TSD, oo_call *c )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *cls = cls_self( TSD, c ), *o;
   oo_classdata *cd = cls->u.c;
   streng *h, *r;

   if ( !cd->resolved )
      resolve_class( TSD, cls );
   if ( cd->mixin )
      exiterror( ERR_EXECUTION_ERROR, 7, tmpstr_of( TSD, cd->name ) );
   if ( cls == ot->cString )
      return c->argc > 0 && c->argv[0] ? Str_dupTSD( c->argv[0] ) : nullstringptr();
   if ( cls == ot->cClass || cd->ikind == OK_CLASS )
   {
      /* .Class~new(name) creates a new class */
      const streng *n = need_arg( TSD, c, 0 );
      oo_object *nc = new_class( TSD, NULL, n, ot->cObject, -1 );
      compute_mro( TSD, nc );
      return hdup( TSD, nc );
   }
   o = new_object( TSD, cls, cd->ikind );
   if ( cd->ikind == OK_DIRECTORY || cd->ikind == OK_TABLE )
   {
      o->u.h = (oo_hash *) MallocTSD( sizeof( oo_hash ) );
      memset( o->u.h, 0, sizeof( oo_hash ) );
   }
   else if ( cd->ikind == OK_BUFFER || cd->ikind == OK_STRING )
      o->u.s = nullstringptr();
   h = hdup( TSD, o );
   r = send_msg( TSD, h, ot->s_init, c->argc, c->argv );
   if ( r )
      Free_stringTSD( r );
   return h;
}

static streng *m_cls_id( tsd_t *TSD, oo_call *c )
{
   return class_name_str( TSD, cls_self( TSD, c ) );
}

static streng *m_cls_superclass( tsd_t *TSD, oo_call *c )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *cls = cls_self( TSD, c );

   if ( !cls->u.c->resolved )
      resolve_class( TSD, cls );
   return hdup( TSD, cls->u.c->super ? cls->u.c->super : ot->nil );
}

void oo_array_put( tsd_t *TSD, oo_object *a, long idx, streng *v );

static streng *m_cls_superclasses( tsd_t *TSD, oo_call *c )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *cls = cls_self( TSD, c );
   oo_object *arr = new_object( TSD, ot->cArray, OK_ARRAY );
   long n = 0;
   int i;

   if ( !cls->u.c->resolved )
      resolve_class( TSD, cls );
   if ( cls->u.c->super )
      oo_array_put( TSD, arr, ++n, hdup( TSD, cls->u.c->super ) );
   for ( i = 0; i < cls->u.c->ninherits; i++ )
      oo_array_put( TSD, arr, ++n, hdup( TSD, cls->u.c->inherits[i] ) );
   return hdup( TSD, arr );
}

static void invalidate_mros( oo_tsd *ot, tsd_t *TSD )
{
   int i;

   for ( i = 0; i < ot->nclasses; i++ )
      if ( ot->classes[i]->u.c->mro )
      {
         FreeTSD( ot->classes[i]->u.c->mro );
         ot->classes[i]->u.c->mro = NULL;
         ot->classes[i]->u.c->nmro = 0;
      }
}

static streng *m_cls_subclass( tsd_t *TSD, oo_call *c )
{
   oo_object *cls = cls_self( TSD, c ), *nc;
   const streng *n = need_arg( TSD, c, 0 );

   nc = new_class( TSD, NULL, n, cls, -1 );
   nc->u.c->mixin = str_eqc( c->name, "MIXINCLASS" );
   compute_mro( TSD, nc );
   return hdup( TSD, nc );
}

static streng *m_cls_inherit( tsd_t *TSD, oo_call *c )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *cls = cls_self( TSD, c );
   oo_object *mix = class_arg( TSD, c, 0 );
   oo_classdata *cd = cls->u.c;
   oo_object **n;

   n = (oo_object **) MallocTSD( ( cd->ninherits + 1 ) * sizeof( oo_object * ) );
   if ( cd->ninherits )
   {
      memcpy( n, cd->inherits, cd->ninherits * sizeof( oo_object * ) );
      FreeTSD( cd->inherits );
   }
   n[cd->ninherits++] = mix;
   cd->inherits = n;
   invalidate_mros( ot, TSD );
   return NULL;
}

static streng *m_cls_issubclassof( tsd_t *TSD, oo_call *c )
{
   oo_object *cls = cls_self( TSD, c );
   oo_object *of = class_arg( TSD, c, 0 );

   if ( cls->u.c->mro == NULL )
      compute_mro( TSD, cls );
   return mkbool( TSD, is_subclass( cls, of ) );
}

static streng *m_cls_querymixin( tsd_t *TSD, oo_call *c )
{
   return mkbool( TSD, cls_self( TSD, c )->u.c->mixin );
}

static streng *m_instancemethod_names( tsd_t *TSD, oo_call *c )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *cls = cls_self( TSD, c );
   oo_object *arr = new_object( TSD, ot->cArray, OK_ARRAY );
   oo_method *m;
   long n = 0;

   for ( m = cls->u.c->imethods; m; m = m->next )
      oo_array_put( TSD, arr, ++n, Str_dupTSD( m->name ) );
   return hdup( TSD, arr );
}

/* ------------------------------------------------------------------ */
/* String methods                                                     */
/* ------------------------------------------------------------------ */

/* methods where the receiver is the second argument of the BIF */
static const char *const recv_second[] = {
   "POS", "LASTPOS", "COUNTSTR", "CHANGESTR", "WORDPOS", "INSERT",
   "OVERLAY", NULL
};

/* String methods implemented by the built-in function of the same name */
static const char *const string_bifs[] = {
   "ABBREV", "ABS", "B2X", "BITAND", "BITOR", "BITXOR", "C2D", "C2X",
   "CENTER", "CENTRE", "CHANGESTR", "COMPARE", "COPIES", "COUNTSTR", "D2C",
   "D2X", "DATATYPE", "DELSTR", "DELWORD", "FORMAT", "INSERT", "LASTPOS",
   "LEFT", "LENGTH", "MAX", "MIN", "OVERLAY", "POS", "REVERSE", "RIGHT",
   "SIGN", "SPACE", "STRIP", "SUBSTR", "SUBWORD", "TRANSLATE", "TRUNC",
   "VERIFY", "WORD", "WORDINDEX", "WORDLENGTH", "WORDPOS", "WORDS", "X2B",
   "X2C", "X2D", NULL
};

static streng *call_bif( tsd_t *TSD, const char *bifname, int argc,
                         const streng **argv, const streng *name )
{
   streng *(*fn)( tsd_t *, cparamboxptr );
   parambox *boxes;
   streng **kills;
   const char *saveName;
   void *saveFunc;
   streng *r;
   int i, n = argc;

   fn = oo_find_bif( TSD, bifname );
   if ( fn == NULL )
      return NOFUNC;
   while ( n > 0 && argv[n - 1] == NULL )
      n--;
   boxes = (parambox *) MallocTSD( ( n + 1 ) * sizeof( parambox ) );
   kills = (streng **) MallocTSD( ( n + 1 ) * sizeof( streng * ) );
   for ( i = 0; i < n; i++ )
   {
      kills[i] = argv[i] ? oo_stringify( TSD, argv[i] ) : NULL;
      boxes[i].value = kills[i] ? kills[i] : (streng *) argv[i];
      boxes[i].dealloc = 0;
      boxes[i].next = ( i + 1 < n ) ? &boxes[i + 1] : NULL;
   }
   saveName = TSD->BIFname;
   saveFunc = TSD->BIFfunc;
   TSD->BIFname = bifname;
   TSD->BIFfunc = (void *) fn;
   r = fn( TSD, n ? boxes : NULL );
   TSD->BIFname = saveName;
   TSD->BIFfunc = saveFunc;
   for ( i = 0; i < n; i++ )
      if ( kills[i] )
         Free_stringTSD( kills[i] );
   FreeTSD( boxes );
   FreeTSD( kills );
   return r;
}

static streng *m_string_bif( tsd_t *TSD, oo_call *c )
{
   const streng **args;
   streng *kill, *r, *bif;
   const streng *sv = strval( TSD, c, &kill );
   int i, second = 0, n;

   bif = Str_makeTSD( c->name->len + 1 );
   memcpy( bif->value, c->name->value, c->name->len );
   bif->value[c->name->len] = '\0';
   bif->len = c->name->len;
   for ( i = 0; recv_second[i]; i++ )
      if ( strcmp( recv_second[i], bif->value ) == 0 )
         second = 1;

   n = c->argc + 1;
   if ( second && c->argc == 0 )
      n = 2;
   args = (const streng **) MallocTSD( ( n + 1 ) * sizeof( streng * ) );
   if ( second )
   {
      args[0] = c->argc > 0 ? c->argv[0] : NULL;
      args[1] = sv;
      for ( i = 1; i < c->argc; i++ )
         args[i + 1] = c->argv[i];
   }
   else
   {
      args[0] = sv;
      for ( i = 0; i < c->argc; i++ )
         args[i + 1] = c->argv[i];
   }
   r = call_bif( TSD, bif->value, n, args, c->name );
   FreeTSD( (void *) args );
   Free_stringTSD( bif );
   if ( kill )
      Free_stringTSD( kill );
   if ( r == NOFUNC )
      err_nomethod( TSD, c->selfstr, c->name );
   return r;
}

static streng *m_string_value( tsd_t *TSD, oo_call *c )
{
   streng *kill;
   const streng *sv = strval( TSD, c, &kill );

   return kill ? kill : Str_dupTSD( sv );
}

/* INIT of String subclasses: the instance holds a string value */
static streng *m_string_init( tsd_t *TSD, oo_call *c )
{
   const streng *v = opt_arg( c, 0 );
   streng *k;

   if ( c->self && c->self->kind == OK_STRING && v )
   {
      k = oo_stringify( TSD, v );
      Free_stringTSD( c->self->u.s );
      c->self->u.s = k ? k : Str_dupTSD( v );
   }
   return NULL;
}

static streng *m_upperlower( tsd_t *TSD, oo_call *c )
{
   streng *kill, *r;
   const streng *sv = strval( TSD, c, &kill );
   long start = 1, len, i;

   r = Str_dupTSD( sv );
   if ( kill )
      Free_stringTSD( kill );
   if ( opt_arg( c, 0 ) )
      start = whole_arg( TSD, c, 0, 1 );
   len = r->len - start + 1;
   if ( opt_arg( c, 1 ) )
      len = whole_arg( TSD, c, 1, 0 );
   for ( i = start - 1; i < r->len && i < start - 1 + len; i++ )
      r->value[i] = (char) ( c->name->value[0] == 'U' ? rx_toupper( r->value[i] )
                                                       : rx_tolower( r->value[i] ) );
   return r;
}

static int caseless_eq( const streng *a, const streng *b, int off, int len )
{
   int i;

   for ( i = 0; i < len; i++ )
      if ( rx_toupper( a->value[off + i] ) != rx_toupper( b->value[i] ) )
         return 0;
   return 1;
}

static streng *m_string_test( tsd_t *TSD, oo_call *c )
{
   streng *kill, *ak;
   const streng *sv = strval( TSD, c, &kill );
   const streng *a = need_arg( TSD, c, 0 );
   int r = 0, caseless, i;
   const char *n;

   ak = oo_stringify( TSD, a );
   if ( ak )
      a = ak;
   n = c->name->value;
   caseless = ( strncmp( n, "CASELESS", 8 ) == 0 );
   if ( caseless )
      n += 8;
   if ( strncmp( n, "EQUALS", 6 ) == 0 )
      r = sv->len == a->len && ( caseless ? caseless_eq( sv, a, 0, a->len )
                                         : memcmp( sv->value, a->value, a->len ) == 0 );
   else if ( strncmp( n, "STARTSWITH", 10 ) == 0 )
      r = sv->len >= a->len && ( caseless ? caseless_eq( sv, a, 0, a->len )
                                         : memcmp( sv->value, a->value, a->len ) == 0 );
   else if ( strncmp( n, "ENDSWITH", 8 ) == 0 )
      r = sv->len >= a->len && ( caseless ? caseless_eq( sv, a, sv->len - a->len, a->len )
                                         : memcmp( sv->value + sv->len - a->len, a->value, a->len ) == 0 );
   else /* CONTAINS */
   {
      for ( i = 0; i + a->len <= sv->len && !r; i++ )
         r = caseless ? caseless_eq( sv, a, i, a->len )
                      : memcmp( sv->value + i, a->value, a->len ) == 0;
   }
   if ( kill )
      Free_stringTSD( kill );
   if ( ak )
      Free_stringTSD( ak );
   return mkbool( TSD, r );
}

static streng *m_string_makearray( tsd_t *TSD, oo_call *c )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   streng *kill;
   const streng *sv = strval( TSD, c, &kill );
   const streng *sep = opt_arg( c, 0 );
   oo_object *arr = new_object( TSD, ot->cArray, OK_ARRAY );
   long n = 0, i, start = 0;

   for ( i = 0; i <= sv->len; )
   {
      int hit = 0, skip = 0;
      if ( i == sv->len )
         hit = 1;
      else if ( sep && sep->len > 0 )
      {
         if ( i + sep->len <= sv->len && memcmp( sv->value + i, sep->value, sep->len ) == 0 )
         {
            hit = 1;
            skip = sep->len;
         }
      }
      else if ( sv->value[i] == '\n' )
      {
         hit = 1;
         skip = 1;
      }
      if ( hit )
      {
         long end = i;
         if ( !( sep && sep->len > 0 ) && end > start && sv->value[end - 1] == '\r' )
            end--;
         if ( !( i == sv->len && start == sv->len && n > 0 && !( sep && sep->len > 0 ) ) )
            oo_array_put( TSD, arr, ++n, Str_ncreTSD( sv->value + start, end - start ) );
         if ( i == sv->len )
            break;
         i += skip;
         start = i;
      }
      else
         i++;
   }
   if ( kill )
      Free_stringTSD( kill );
   return hdup( TSD, arr );
}

static const struct { const char *name; int type; int unary; } string_ops[] = {
   { "+", X_PLUSS, 0 }, { "-", X_MINUS, 0 }, { "*", X_MULT, 0 },
   { "/", X_DEVIDE, 0 }, { "%", X_INTDIV, 0 }, { "//", X_MODULUS, 0 },
   { "**", X_EXP, 0 }, { "||", X_CONCAT, 0 }, { "", X_CONCAT, 0 },
   { " ", X_SPACE, 0 }, { "=", X_EQUAL, 0 }, { "\\=", X_DIFF, 0 },
   { "<>", X_DIFF, 0 }, { "><", X_DIFF, 0 }, { "==", X_S_EQUAL, 0 },
   { "\\==", X_S_DIFF, 0 }, { ">", X_GT, 0 }, { "<", X_LT, 0 },
   { ">=", X_GTE, 0 }, { "<=", X_LTE, 0 }, { "\\<", X_GTE, 0 },
   { "\\>", X_LTE, 0 }, { ">>", X_S_GT, 0 }, { "<<", X_S_LT, 0 },
   { ">>=", X_S_GTE, 0 }, { "<<=", X_S_LTE, 0 }, { "&", X_LOG_AND, 0 },
   { "|", X_LOG_OR, 0 }, { "&&", X_LOG_XOR, 0 }, { "\\", X_LOG_NOT, 1 },
   { NULL, 0, 0 }
};

static void free_node_number( tsd_t *TSD, treenode *n )
{
   if ( n->u.number )
   {
      FreeTSD( n->u.number->num );
      FreeTSD( n->u.number );
      n->u.number = NULL;
   }
}

static streng *m_string_op( tsd_t *TSD, oo_call *c )
{
   treenode na, nb, op;
   streng *kill, *bk = NULL, *r;
   const streng *sv = strval( TSD, c, &kill ), *b;
   int i, type = -1, unary = 0;

   for ( i = 0; string_ops[i].name; i++ )
      if ( str_eqc( c->name, string_ops[i].name ) )
      {
         type = string_ops[i].type;
         unary = string_ops[i].unary;
         break;
      }
   if ( type == -1 )
      err_nomethod( TSD, c->selfstr, c->name );
   b = opt_arg( c, 0 );
   if ( b == NULL && !unary )
   {
      /* prefix + and - */
      if ( type == X_PLUSS )
         type = X_U_PLUSS, unary = 1;
      else if ( type == X_MINUS )
         type = X_U_MINUS, unary = 1;
      else
         need_arg( TSD, c, 0 );
   }
   if ( b )
   {
      bk = oo_stringify( TSD, b );
      if ( bk )
         b = bk;
   }
   memset( &na, 0, sizeof( na ) );
   memset( &nb, 0, sizeof( nb ) );
   memset( &op, 0, sizeof( op ) );
   na.type = nb.type = X_STRING;
   na.name = (streng *) sv;
   nb.name = (streng *) b;
   op.type = type;
   op.p[0] = &na;
   op.p[1] = unary ? NULL : &nb;
   op.lineno = TSD->currentnode ? TSD->currentnode->lineno : 0;
   r = evaluate( TSD, &op, NULL );
   free_node_number( TSD, &na );
   free_node_number( TSD, &nb );
   if ( kill )
      Free_stringTSD( kill );
   if ( bk )
      Free_stringTSD( bk );
   return r;
}

/* ------------------------------------------------------------------ */
/* collections: Array, Queue, List                                    */
/* ------------------------------------------------------------------ */

void oo_array_put( tsd_t *TSD, oo_object *a, long idx, streng *v )
{
   long i;

   if ( idx > a->u.a.cap )
   {
      long ncap = a->u.a.cap ? a->u.a.cap : 8;
      streng **n;
      while ( ncap < idx )
         ncap *= 2;
      n = (streng **) MallocTSD( ncap * sizeof( streng * ) );
      for ( i = 0; i < ncap; i++ )
         n[i] = ( i < a->u.a.size ) ? a->u.a.v[i] : NULL;
      if ( a->u.a.v )
         FreeTSD( a->u.a.v );
      a->u.a.v = n;
      a->u.a.cap = ncap;
   }
   if ( idx > a->u.a.size )
   {
      for ( i = a->u.a.size; i < idx; i++ )
         a->u.a.v[i] = NULL;
      a->u.a.size = idx;
   }
   if ( a->u.a.v[idx - 1] )
      Free_stringTSD( a->u.a.v[idx - 1] );
   a->u.a.v[idx - 1] = v;
}

static void arr_insert_at( tsd_t *TSD, oo_object *a, long idx, streng *v )
{
   long i;

   oo_array_put( TSD, a, a->u.a.size + 1, NULL );
   for ( i = a->u.a.size - 1; i >= idx; i-- )
      a->u.a.v[i] = a->u.a.v[i - 1];
   a->u.a.v[idx - 1] = v;
}

static streng *arr_take( tsd_t *TSD, oo_object *a, long idx, int shift )
{
   streng *v;
   long i;

   if ( idx < 1 || idx > a->u.a.size )
      return NULL;
   v = a->u.a.v[idx - 1];
   a->u.a.v[idx - 1] = NULL;
   if ( shift )
   {
      for ( i = idx - 1; i + 1 < a->u.a.size; i++ )
         a->u.a.v[i] = a->u.a.v[i + 1];
      a->u.a.size--;
   }
   else
   {
      while ( a->u.a.size > 0 && a->u.a.v[a->u.a.size - 1] == NULL )
         a->u.a.size--;
   }
   return v;
}

long oo_array_items( tsd_t *TSD, oo_object *a, const streng ***out )
{
   long i, n = 0;

   for ( i = 0; i < a->u.a.size; i++ )
      if ( a->u.a.v[i] )
         n++;
   if ( out )
   {
      *out = (const streng **) MallocTSD( ( n + 1 ) * sizeof( streng * ) );
      for ( i = 0, n = 0; i < a->u.a.size; i++ )
         if ( a->u.a.v[i] )
            (*out)[n++] = a->u.a.v[i];
   }
   return n;
}

static oo_object *seq_self( tsd_t *TSD, oo_call *c )
{
   if ( c->self == NULL || ( c->self->kind != OK_ARRAY && c->self->kind != OK_QUEUE ) )
      err_nomethod( TSD, c->selfstr, c->name );
   return c->self;
}

static streng *nil_or( tsd_t *TSD, const streng *v )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   return v ? Str_dupTSD( v ) : hdup( TSD, ot->nil );
}

/* the single index of [] and friends */
static long index_arg( tsd_t *TSD, oo_call *c, int i )
{
   if ( c->argc > i + 1 && seq_self( TSD, c )->kind == OK_ARRAY
     && str_eqc( c->name, "[]" ) )
      exiterror( ERR_EXECUTION_ERROR, 8 );
   return whole_arg( TSD, c, i, 1 );
}

static streng *m_arr_init( tsd_t *TSD, oo_call *c )
{
   oo_object *a = seq_self( TSD, c );
   long n;

   if ( a->kind == OK_ARRAY && opt_arg( c, 0 ) )
   {
      n = whole_arg( TSD, c, 0, 0 );
      if ( n > 0 )
      {
         oo_array_put( TSD, a, n, NULL );
         a->u.a.size = 0;
      }
   }
   return NULL;
}

static streng *m_arr_of( tsd_t *TSD, oo_call *c )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *cls = cls_self( TSD, c ), *a;
   int i;

   if ( !cls->u.c->resolved )
      resolve_class( TSD, cls );
   a = new_object( TSD, cls, cls->u.c->ikind == OK_QUEUE ? OK_QUEUE : OK_ARRAY );
   for ( i = 0; i < c->argc; i++ )
      if ( c->argv[i] )
         oo_array_put( TSD, a, i + 1, Str_dupTSD( c->argv[i] ) );
      else if ( i + 1 > a->u.a.size )
         oo_array_put( TSD, a, i + 1, NULL );
   (void) ot;
   return hdup( TSD, a );
}

static streng *m_arr_at( tsd_t *TSD, oo_call *c )
{
   oo_object *a = seq_self( TSD, c );
   long i = index_arg( TSD, c, 0 );

   return nil_or( TSD, i <= a->u.a.size ? a->u.a.v[i - 1] : NULL );
}

static streng *m_arr_put( tsd_t *TSD, oo_call *c )
{
   oo_object *a = seq_self( TSD, c );
   const streng *v = need_arg( TSD, c, 0 );
   long i = whole_arg( TSD, c, 1, 1 );

   oo_array_put( TSD, a, i, Str_dupTSD( v ) );
   return NULL;
}

static streng *m_arr_size( tsd_t *TSD, oo_call *c )
{
   return mkint( TSD, seq_self( TSD, c )->u.a.size );
}

static streng *m_arr_items( tsd_t *TSD, oo_call *c )
{
   return mkint( TSD, oo_array_items( TSD, seq_self( TSD, c ), NULL ) );
}

static streng *m_arr_append( tsd_t *TSD, oo_call *c )
{
   oo_object *a = seq_self( TSD, c );
   const streng *v = need_arg( TSD, c, 0 );

   oo_array_put( TSD, a, a->u.a.size + 1, Str_dupTSD( v ) );
   return mkint( TSD, a->u.a.size );
}

static streng *m_arr_firstlast( tsd_t *TSD, oo_call *c )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *a = seq_self( TSD, c );
   int last = ( c->name->value[0] == 'L' );
   int item = str_eqc( c->name, "FIRSTITEM" ) || str_eqc( c->name, "LASTITEM" );
   long i;

   if ( last )
   {
      for ( i = a->u.a.size; i >= 1; i-- )
         if ( a->u.a.v[i - 1] )
            break;
   }
   else
   {
      for ( i = 1; i <= a->u.a.size; i++ )
         if ( a->u.a.v[i - 1] )
            break;
      if ( i > a->u.a.size )
         i = 0;
   }
   if ( i < 1 )
      return hdup( TSD, ot->nil );
   return item ? Str_dupTSD( a->u.a.v[i - 1] ) : mkint( TSD, i );
}

static streng *m_arr_nextprev( tsd_t *TSD, oo_call *c )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *a = seq_self( TSD, c );
   long i = whole_arg( TSD, c, 0, 1 );
   int next = ( c->name->value[0] == 'N' );

   for ( i = next ? i + 1 : i - 1; i >= 1 && i <= a->u.a.size; i = next ? i + 1 : i - 1 )
      if ( a->u.a.v[i - 1] )
         return mkint( TSD, i );
   return hdup( TSD, ot->nil );
}

static streng *m_arr_hasindex( tsd_t *TSD, oo_call *c )
{
   oo_object *a = seq_self( TSD, c );
   const streng *s = need_arg( TSD, c, 0 );
   int err = 0;
   long i = streng_to_int( TSD, s, &err );

   return mkbool( TSD, !err && i >= 1 && i <= a->u.a.size && a->u.a.v[i - 1] );
}

static long find_item( oo_object *a, const streng *v )
{
   long i;

   for ( i = 0; i < a->u.a.size; i++ )
      if ( a->u.a.v[i] && str_eq( a->u.a.v[i], v ) )
         return i + 1;
   return 0;
}

static streng *m_arr_hasitem( tsd_t *TSD, oo_call *c )
{
   return mkbool( TSD, find_item( seq_self( TSD, c ), need_arg( TSD, c, 0 ) ) > 0 );
}

static streng *m_arr_index( tsd_t *TSD, oo_call *c )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   long i = find_item( seq_self( TSD, c ), need_arg( TSD, c, 0 ) );

   return i ? mkint( TSD, i ) : hdup( TSD, ot->nil );
}

static streng *m_arr_remove( tsd_t *TSD, oo_call *c )
{
   oo_object *a = seq_self( TSD, c );
   long i = whole_arg( TSD, c, 0, 1 );
   streng *v = arr_take( TSD, a, i, a->kind == OK_QUEUE || str_eqc( c->name, "DELETE" ) );

   if ( v == NULL )
      return nil_or( TSD, NULL );
   return v;
}

static streng *m_arr_removeitem( tsd_t *TSD, oo_call *c )
{
   oo_object *a = seq_self( TSD, c );
   long i = find_item( a, need_arg( TSD, c, 0 ) );
   streng *v;

   if ( i == 0 )
      return nil_or( TSD, NULL );
   v = arr_take( TSD, a, i, a->kind == OK_QUEUE );
   return v;
}

static streng *m_arr_insert( tsd_t *TSD, oo_call *c )
{
   oo_object *a = seq_self( TSD, c );
   const streng *v = need_arg( TSD, c, 0 );
   const streng *where = opt_arg( c, 1 );
   long idx;

   if ( a->kind == OK_QUEUE )
   {
      /* List~insert(item, index): after index; .nil means first; default last */
      oo_object *w = where ? obj_of( TSD, where ) : NULL;
      if ( where == NULL )
         idx = a->u.a.size + 1;
      else if ( w && w->kind == OK_NIL )
         idx = 1;
      else
         idx = whole_arg( TSD, c, 1, 1 ) + 1;
   }
   else
      idx = where ? whole_arg( TSD, c, 1, 1 ) : a->u.a.size + 1;
   if ( idx > a->u.a.size + 1 )
      oo_array_put( TSD, a, idx, Str_dupTSD( v ) );
   else
      arr_insert_at( TSD, a, idx, Str_dupTSD( v ) );
   return mkint( TSD, idx );
}

static oo_object *new_array_from( tsd_t *TSD, long n, const streng **items, int dup )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *arr = new_object( TSD, ot->cArray, OK_ARRAY );
   long i;

   for ( i = 0; i < n; i++ )
      oo_array_put( TSD, arr, i + 1, dup ? Str_dupTSD( items[i] ) : (streng *) items[i] );
   return arr;
}

static streng *m_arr_makearray( tsd_t *TSD, oo_call *c )
{
   const streng **items;
   long n = oo_array_items( TSD, seq_self( TSD, c ), &items );
   oo_object *arr = new_array_from( TSD, n, items, 1 );

   FreeTSD( (void *) items );
   return hdup( TSD, arr );
}

static streng *m_arr_allindexes( tsd_t *TSD, oo_call *c )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *a = seq_self( TSD, c );
   oo_object *arr = new_object( TSD, ot->cArray, OK_ARRAY );
   long i, n = 0;

   for ( i = 0; i < a->u.a.size; i++ )
      if ( a->u.a.v[i] )
         oo_array_put( TSD, arr, ++n, mkint( TSD, i + 1 ) );
   return hdup( TSD, arr );
}

static streng *m_arr_isempty( tsd_t *TSD, oo_call *c )
{
   return mkbool( TSD, oo_array_items( TSD, seq_self( TSD, c ), NULL ) == 0 );
}

static streng *m_arr_empty( tsd_t *TSD, oo_call *c )
{
   oo_object *a = seq_self( TSD, c );
   long i;

   for ( i = 0; i < a->u.a.size; i++ )
      if ( a->u.a.v[i] )
      {
         Free_stringTSD( a->u.a.v[i] );
         a->u.a.v[i] = NULL;
      }
   a->u.a.size = 0;
   return NULL;
}

static streng *m_arr_fill( tsd_t *TSD, oo_call *c )
{
   oo_object *a = seq_self( TSD, c );
   const streng *v = need_arg( TSD, c, 0 );
   long i;

   for ( i = 1; i <= a->u.a.size; i++ )
      oo_array_put( TSD, a, i, Str_dupTSD( v ) );
   return NULL;
}

static int cmp_strings( const void *x, const void *y )
{
   const streng *a = *(const streng * const *) x, *b = *(const streng * const *) y;
   int l = a->len < b->len ? a->len : b->len;
   int r = memcmp( a->value, b->value, l );

   if ( r )
      return r;
   return a->len - b->len;
}

static streng *m_arr_sort( tsd_t *TSD, oo_call *c )
{
   oo_object *a = seq_self( TSD, c );
   long i, n = 0;

   /* ooRexx requires no holes; holes are moved to the end */
   for ( i = 0; i < a->u.a.size; i++ )
      if ( a->u.a.v[i] )
         a->u.a.v[n++] = a->u.a.v[i];
   for ( i = n; i < a->u.a.size; i++ )
      a->u.a.v[i] = NULL;
   qsort( a->u.a.v, n, sizeof( streng * ), cmp_strings );
   return NULL;
}

static streng *m_arr_tostring( tsd_t *TSD, oo_call *c )
{
   oo_object *a = seq_self( TSD, c );
   const streng *fmt = opt_arg( c, 0 ), *sep = opt_arg( c, 1 );
   streng *r = nullstringptr(), *s;
   int chars = ( fmt && fmt->len > 0 && rx_toupper( fmt->value[0] ) == 'C' );
   long i;
   int first = 1;

   for ( i = 0; i < a->u.a.size; i++ )
   {
      if ( a->u.a.v[i] == NULL )
         continue;
      if ( !first )
      {
         if ( sep )
            r = cats( TSD, r, sep );
         else if ( !chars )
            r = catc( TSD, r, "\n" );
      }
      first = 0;
      s = oo_stringify( TSD, a->u.a.v[i] );
      r = cats( TSD, r, s ? s : a->u.a.v[i] );
      if ( s )
         Free_stringTSD( s );
   }
   return r;
}

static streng *m_arr_section( tsd_t *TSD, oo_call *c )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *a = seq_self( TSD, c );
   long start = whole_arg( TSD, c, 0, 1 ), cnt, i;
   oo_object *r = new_object( TSD, a->cls == ot->cArray ? ot->cArray : a->cls, a->kind );

   cnt = opt_arg( c, 1 ) ? whole_arg( TSD, c, 1, 0 ) : a->u.a.size - start + 1;
   for ( i = 0; i < cnt && start + i <= a->u.a.size; i++ )
      if ( a->u.a.v[start + i - 1] )
         oo_array_put( TSD, r, i + 1, Str_dupTSD( a->u.a.v[start + i - 1] ) );
   return hdup( TSD, r );
}

/* Queue */
static streng *m_q_push( tsd_t *TSD, oo_call *c )
{
   arr_insert_at( TSD, seq_self( TSD, c ), 1, Str_dupTSD( need_arg( TSD, c, 0 ) ) );
   return NULL;
}

static streng *m_q_queue( tsd_t *TSD, oo_call *c )
{
   oo_object *a = seq_self( TSD, c );

   oo_array_put( TSD, a, a->u.a.size + 1, Str_dupTSD( need_arg( TSD, c, 0 ) ) );
   return NULL;
}

static streng *m_q_pull( tsd_t *TSD, oo_call *c )
{
   oo_object *a = seq_self( TSD, c );
   streng *v;

   if ( a->u.a.size == 0 || ( v = arr_take( TSD, a, 1, 1 ) ) == NULL )
      return nil_or( TSD, NULL );
   return v;
}

static streng *m_q_peek( tsd_t *TSD, oo_call *c )
{
   oo_object *a = seq_self( TSD, c );

   return nil_or( TSD, a->u.a.size ? a->u.a.v[0] : NULL );
}

/* ------------------------------------------------------------------ */
/* collections: Directory, Table, StringTable                         */
/* ------------------------------------------------------------------ */

static oo_object *hash_self( tsd_t *TSD, oo_call *c )
{
   if ( c->self == NULL || ( c->self->kind != OK_DIRECTORY && c->self->kind != OK_TABLE ) )
      err_nomethod( TSD, c->selfstr, c->name );
   return c->self;
}

static oo_entry *h_find( oo_hash *h, const streng *key, unsigned *hv )
{
   oo_entry *e;

   *hv = hashvalue( key->value, key->len );
   if ( h->size == 0 )
      return NULL;
   for ( e = h->tbl[*hv % h->size]; e; e = e->hnext )
      if ( e->hash == *hv && str_eq( e->key, key ) )
         return e;
   return NULL;
}

static void h_put( tsd_t *TSD, oo_hash *h, const streng *key, streng *value )
{
   unsigned hv, i;
   oo_entry *e = h_find( h, key, &hv );

   if ( e )
   {
      Free_stringTSD( e->value );
      e->value = value;
      return;
   }
   if ( h->count + 1 > h->size * 2 )
   {
      unsigned nsize = h->size ? h->size * 4 : 16;
      oo_entry **n = (oo_entry **) MallocTSD( nsize * sizeof( oo_entry * ) ), *x, *nx;
      memset( n, 0, nsize * sizeof( oo_entry * ) );
      for ( i = 0; i < h->size; i++ )
         for ( x = h->tbl[i]; x; x = nx )
         {
            nx = x->hnext;
            x->hnext = n[x->hash % nsize];
            n[x->hash % nsize] = x;
         }
      if ( h->tbl )
         FreeTSD( h->tbl );
      h->tbl = n;
      h->size = nsize;
   }
   e = (oo_entry *) MallocTSD( sizeof( oo_entry ) );
   e->key = Str_dupTSD( key );
   e->value = value;
   e->hash = hv;
   e->hnext = h->tbl[hv % h->size];
   h->tbl[hv % h->size] = e;
   e->onext = NULL;
   e->oprev = h->last;
   if ( h->last )
      h->last->onext = e;
   else
      h->first = e;
   h->last = e;
   h->count++;
}

static streng *h_remove( tsd_t *TSD, oo_hash *h, const streng *key )
{
   unsigned hv;
   oo_entry *e = h_find( h, key, &hv ), **pp;
   streng *v;

   if ( e == NULL )
      return NULL;
   for ( pp = &h->tbl[hv % h->size]; *pp != e; pp = &(*pp)->hnext )
      ;
   *pp = e->hnext;
   if ( e->oprev )
      e->oprev->onext = e->onext;
   else
      h->first = e->onext;
   if ( e->onext )
      e->onext->oprev = e->oprev;
   else
      h->last = e->oprev;
   h->count--;
   v = e->value;
   Free_stringTSD( e->key );
   FreeTSD( e );
   return v;
}

/* keys of a Directory are strings: objects used as keys are converted */
static streng *key_arg( tsd_t *TSD, oo_call *c, int i, int upper )
{
   const streng *k = need_arg( TSD, c, i );
   streng *r;

   if ( c->self && c->self->kind == OK_TABLE )
      r = Str_dupTSD( k );        /* Table: identity of objects counts */
   else
   {
      r = oo_stringify( TSD, k );
      if ( r == NULL )
         r = Str_dupTSD( k );
   }
   if ( upper )
      mem_upper( r->value, r->len );
   return r;
}

static streng *m_h_at( tsd_t *TSD, oo_call *c )
{
   oo_object *d = hash_self( TSD, c );
   streng *k = key_arg( TSD, c, 0, str_eqc( c->name, "ENTRY" ) );
   unsigned hv;
   oo_entry *e = h_find( d->u.h, k, &hv );

   Free_stringTSD( k );
   return nil_or( TSD, e ? e->value : NULL );
}

static streng *m_h_put( tsd_t *TSD, oo_call *c )
{
   oo_object *d = hash_self( TSD, c );
   int setentry = str_eqc( c->name, "SETENTRY" );
   /* put(item, index) and []=(item, index); setEntry(name, item) */
   streng *k = key_arg( TSD, c, setentry ? 0 : 1, setentry );
   const streng *v = setentry ? opt_arg( c, 1 ) : need_arg( TSD, c, 0 );

   if ( v == NULL )
   {
      streng *old = h_remove( TSD, d->u.h, k );
      if ( old )
         Free_stringTSD( old );
   }
   else
      h_put( TSD, d->u.h, k, Str_dupTSD( v ) );
   Free_stringTSD( k );
   return NULL;
}

static streng *m_h_has( tsd_t *TSD, oo_call *c )
{
   oo_object *d = hash_self( TSD, c );
   streng *k = key_arg( TSD, c, 0, str_eqc( c->name, "HASENTRY" ) );
   unsigned hv;
   int r = h_find( d->u.h, k, &hv ) != NULL;

   Free_stringTSD( k );
   return mkbool( TSD, r );
}

static streng *m_h_remove( tsd_t *TSD, oo_call *c )
{
   oo_object *d = hash_self( TSD, c );
   streng *k = key_arg( TSD, c, 0, str_eqc( c->name, "REMOVEENTRY" ) );
   streng *v = h_remove( TSD, d->u.h, k );

   Free_stringTSD( k );
   return v ? v : nil_or( TSD, NULL );
}

static streng *m_h_items( tsd_t *TSD, oo_call *c )
{
   return mkint( TSD, hash_self( TSD, c )->u.h->count );
}

static streng *m_h_isempty( tsd_t *TSD, oo_call *c )
{
   return mkbool( TSD, hash_self( TSD, c )->u.h->count == 0 );
}

static streng *m_h_empty( tsd_t *TSD, oo_call *c )
{
   oo_object *d = hash_self( TSD, c );

   while ( d->u.h->first )
   {
      streng *v = h_remove( TSD, d->u.h, d->u.h->first->key );
      Free_stringTSD( v );
   }
   return NULL;
}

static oo_object *h_list( tsd_t *TSD, oo_object *d, int values )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *arr = new_object( TSD, ot->cArray, OK_ARRAY );
   oo_entry *e;
   long n = 0;

   for ( e = d->u.h->first; e; e = e->onext )
      oo_array_put( TSD, arr, ++n, Str_dupTSD( values ? e->value : e->key ) );
   return arr;
}

static streng *m_h_indexes( tsd_t *TSD, oo_call *c )
{
   return hdup( TSD, h_list( TSD, hash_self( TSD, c ), 0 ) );
}

static streng *m_h_allitems( tsd_t *TSD, oo_call *c )
{
   return hdup( TSD, h_list( TSD, hash_self( TSD, c ), 1 ) );
}

static streng *m_h_hasitem( tsd_t *TSD, oo_call *c )
{
   oo_object *d = hash_self( TSD, c );
   const streng *v = need_arg( TSD, c, 0 );
   oo_entry *e;

   for ( e = d->u.h->first; e; e = e->onext )
      if ( str_eq( e->value, v ) )
         return mkbool( TSD, 1 );
   return mkbool( TSD, 0 );
}

static streng *m_h_index( tsd_t *TSD, oo_call *c )
{
   oo_object *d = hash_self( TSD, c );
   const streng *v = need_arg( TSD, c, 0 );
   oo_entry *e;

   for ( e = d->u.h->first; e; e = e->onext )
      if ( str_eq( e->value, v ) )
         return Str_dupTSD( e->key );
   return nil_or( TSD, NULL );
}

/* Directory~unknown: d~name returns the entry NAME, d~name = v sets it */
static streng *m_dir_unknown( tsd_t *TSD, oo_call *c )
{
   oo_object *d = hash_self( TSD, c );
   streng *name = upper_dup( TSD, need_arg( TSD, c, 0 ) );
   oo_object *args = obj_of( TSD, opt_arg( c, 1 ) );
   unsigned hv;
   oo_entry *e;

   if ( name->len > 1 && name->value[name->len - 1] == '=' )
   {
      name->len--;
      if ( args && args->u.a.size > 0 && args->u.a.v[0] )
         h_put( TSD, d->u.h, name, Str_dupTSD( args->u.a.v[0] ) );
      else
      {
         streng *old = h_remove( TSD, d->u.h, name );
         if ( old )
            Free_stringTSD( old );
      }
      Free_stringTSD( name );
      return NULL;
   }
   e = h_find( d->u.h, name, &hv );
   Free_stringTSD( name );
   return nil_or( TSD, e ? e->value : NULL );
}

/* ------------------------------------------------------------------ */
/* MutableBuffer                                                      */
/* ------------------------------------------------------------------ */

static oo_object *buf_self( tsd_t *TSD, oo_call *c )
{
   if ( c->self == NULL || c->self->kind != OK_BUFFER )
      err_nomethod( TSD, c->selfstr, c->name );
   return c->self;
}

static streng *m_buf_init( tsd_t *TSD, oo_call *c )
{
   oo_object *b = buf_self( TSD, c );
   const streng *v = opt_arg( c, 0 );

   if ( v )
   {
      streng *s = oo_stringify( TSD, v );
      Free_stringTSD( b->u.s );
      b->u.s = s ? s : Str_dupTSD( v );
   }
   return NULL;
}

static streng *m_buf_append( tsd_t *TSD, oo_call *c )
{
   oo_object *b = buf_self( TSD, c );
   int i;

   for ( i = 0; i < c->argc; i++ )
   {
      streng *s;
      if ( c->argv[i] == NULL )
         continue;
      s = oo_stringify( TSD, c->argv[i] );
      b->u.s = cats( TSD, b->u.s, s ? s : c->argv[i] );
      if ( s )
         Free_stringTSD( s );
   }
   return Str_dupTSD( c->selfstr );
}

static streng *m_buf_insert( tsd_t *TSD, oo_call *c )
{
   oo_object *b = buf_self( TSD, c );
   const streng *v = need_arg( TSD, c, 0 );
   long pos = opt_arg( c, 1 ) ? whole_arg( TSD, c, 1, 0 ) : 0;
   streng *n;

   if ( pos > b->u.s->len )
      pos = b->u.s->len;
   n = Str_makeTSD( b->u.s->len + v->len );
   memcpy( n->value, b->u.s->value, pos );
   memcpy( n->value + pos, v->value, v->len );
   memcpy( n->value + pos + v->len, b->u.s->value + pos, b->u.s->len - pos );
   n->len = b->u.s->len + v->len;
   Free_stringTSD( b->u.s );
   b->u.s = n;
   return Str_dupTSD( c->selfstr );
}

static streng *m_buf_delete( tsd_t *TSD, oo_call *c )
{
   oo_object *b = buf_self( TSD, c );
   long start = whole_arg( TSD, c, 0, 1 ), len, rest;

   len = opt_arg( c, 1 ) ? whole_arg( TSD, c, 1, 0 ) : b->u.s->len;
   if ( start <= b->u.s->len )
   {
      if ( start - 1 + len > b->u.s->len )
         len = b->u.s->len - start + 1;
      rest = b->u.s->len - ( start - 1 + len );
      memmove( b->u.s->value + start - 1, b->u.s->value + start - 1 + len, rest );
      b->u.s->len -= len;
   }
   return Str_dupTSD( c->selfstr );
}

static streng *m_buf_length( tsd_t *TSD, oo_call *c )
{
   return mkint( TSD, buf_self( TSD, c )->u.s->len );
}

/* ------------------------------------------------------------------ */
/* Supplier                                                           */
/* ------------------------------------------------------------------ */

static oo_object *make_supplier( tsd_t *TSD, oo_object *items, oo_object *indexes )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *s = new_object( TSD, ot->cSupplier, OK_SUPPLIER );

   s->u.sp.items = items;
   s->u.sp.indexes = indexes;
   s->u.sp.pos = 1;
   return s;
}

static streng *m_supplier_of( tsd_t *TSD, oo_call *c )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *o = c->self, *items, *idx;
   long i, n = 0;

   if ( o && ( o->kind == OK_DIRECTORY || o->kind == OK_TABLE ) )
      return hdup( TSD, make_supplier( TSD, h_list( TSD, o, 1 ), h_list( TSD, o, 0 ) ) );
   o = seq_self( TSD, c );
   items = new_object( TSD, ot->cArray, OK_ARRAY );
   idx = new_object( TSD, ot->cArray, OK_ARRAY );
   for ( i = 0; i < o->u.a.size; i++ )
      if ( o->u.a.v[i] )
      {
         oo_array_put( TSD, items, ++n, Str_dupTSD( o->u.a.v[i] ) );
         oo_array_put( TSD, idx, n, mkint( TSD, i + 1 ) );
      }
   return hdup( TSD, make_supplier( TSD, items, idx ) );
}

static oo_object *sup_self( tsd_t *TSD, oo_call *c )
{
   if ( c->self == NULL || c->self->kind != OK_SUPPLIER )
      err_nomethod( TSD, c->selfstr, c->name );
   return c->self;
}

static streng *m_sup_available( tsd_t *TSD, oo_call *c )
{
   oo_object *s = sup_self( TSD, c );
   return mkbool( TSD, s->u.sp.pos <= s->u.sp.items->u.a.size );
}

static streng *m_sup_item( tsd_t *TSD, oo_call *c )
{
   oo_object *s = sup_self( TSD, c );
   oo_object *a = ( c->name->value[0] == 'I' && c->name->value[1] == 'N' )
                  ? s->u.sp.indexes : s->u.sp.items;

   if ( s->u.sp.pos > a->u.a.size )
      exiterror( ERR_EXECUTION_ERROR, 9 );
   return nil_or( TSD, a->u.a.v[s->u.sp.pos - 1] );
}

static streng *m_sup_next( tsd_t *TSD, oo_call *c )
{
   sup_self( TSD, c )->u.sp.pos++;
   return NULL;
}

static streng *m_sup_all( tsd_t *TSD, oo_call *c )
{
   oo_object *s = sup_self( TSD, c );
   const streng **items;
   oo_object *src = str_eqc( c->name, "ALLINDEXES" ) ? s->u.sp.indexes : s->u.sp.items;
   long n = oo_array_items( TSD, src, &items );
   oo_object *arr = new_array_from( TSD, n, items, 1 );

   FreeTSD( (void *) items );
   return hdup( TSD, arr );
}

/* ------------------------------------------------------------------ */
/* copying                                                            */
/* ------------------------------------------------------------------ */

oo_object *oo_copy_object( tsd_t *TSD, oo_object *o )
{
   oo_object *n = new_object( TSD, o->cls, o->kind );
   oo_scope *s;
   long i;

   switch ( o->kind )
   {
      case OK_ARRAY:
      case OK_QUEUE:
         for ( i = 0; i < o->u.a.size; i++ )
            oo_array_put( TSD, n, i + 1, o->u.a.v[i] ? Str_dupTSD( o->u.a.v[i] ) : NULL );
         break;
      case OK_DIRECTORY:
      case OK_TABLE:
      {
         oo_entry *e;
         n->u.h = (oo_hash *) MallocTSD( sizeof( oo_hash ) );
         memset( n->u.h, 0, sizeof( oo_hash ) );
         for ( e = o->u.h->first; e; e = e->onext )
            h_put( TSD, n->u.h, e->key, Str_dupTSD( e->value ) );
         break;
      }
      case OK_BUFFER:
      case OK_STRING:
         n->u.s = Str_dupTSD( o->u.s );
         break;
      case OK_SUPPLIER:
         n->u.sp = o->u.sp;
         break;
      default:
         break;
   }
   /* object variables: simple variables are copied, see oo_pool_copy */
   for ( s = o->scopes; s; s = s->next )
   {
      extern void oo_pool_copy( const tsd_t *TSD, var_hashtable *from, var_hashtable *to );
      oo_scope *ns = get_scope( TSD, n, s->cls );
      oo_pool_copy( TSD, s->pool, ns->pool );
   }
   return n;
}

/* ------------------------------------------------------------------ */
/* the built-in class library                                         */
/* ------------------------------------------------------------------ */

static void add_methods( tsd_t *TSD, oo_object *c, const char *names, oo_native fn,
                         int flags )
{
   char buf[64];
   const char *p = names, *e;
   streng *s;

   while ( *p )
   {
      e = strchr( p, ' ' );
      if ( e == NULL )
         e = p + strlen( p );
      memcpy( buf, p, e - p );
      buf[e - p] = '\0';
      s = Str_creTSD( buf );
      add_method( TSD, c, NULL, s, fn, flags );
      Free_stringTSD( s );
      p = *e ? e + 1 : e;
   }
}

static void add_seq_methods( tsd_t *TSD, oo_object *c )
{
   add_methods( TSD, c, "INIT", m_arr_init, 0 );
   add_methods( TSD, c, "[] AT", m_arr_at, 0 );
   add_methods( TSD, c, "[]= PUT", m_arr_put, 0 );
   add_methods( TSD, c, "SIZE", m_arr_size, 0 );
   add_methods( TSD, c, "ITEMS", m_arr_items, 0 );
   add_methods( TSD, c, "APPEND", m_arr_append, 0 );
   add_methods( TSD, c, "FIRST LAST FIRSTITEM LASTITEM", m_arr_firstlast, 0 );
   add_methods( TSD, c, "NEXT PREVIOUS", m_arr_nextprev, 0 );
   add_methods( TSD, c, "HASINDEX", m_arr_hasindex, 0 );
   add_methods( TSD, c, "HASITEM", m_arr_hasitem, 0 );
   add_methods( TSD, c, "INDEX", m_arr_index, 0 );
   add_methods( TSD, c, "REMOVE", m_arr_remove, 0 );
   add_methods( TSD, c, "REMOVEITEM", m_arr_removeitem, 0 );
   add_methods( TSD, c, "INSERT", m_arr_insert, 0 );
   add_methods( TSD, c, "MAKEARRAY ALLITEMS", m_arr_makearray, 0 );
   add_methods( TSD, c, "ALLINDEXES", m_arr_allindexes, 0 );
   add_methods( TSD, c, "ISEMPTY", m_arr_isempty, 0 );
   add_methods( TSD, c, "EMPTY", m_arr_empty, 0 );
   add_methods( TSD, c, "SUPPLIER", m_supplier_of, 0 );
   add_methods( TSD, c, "SECTION", m_arr_section, 0 );
   add_methods( TSD, c, "TOSTRING MAKESTRING", m_arr_tostring, 0 );
}

static void add_hash_methods( tsd_t *TSD, oo_object *c, int directory )
{
   add_methods( TSD, c, "INIT", m_nothing, 0 );
   add_methods( TSD, c, "[] AT", m_h_at, 0 );
   add_methods( TSD, c, "[]= PUT", m_h_put, 0 );
   add_methods( TSD, c, "HASINDEX", m_h_has, 0 );
   add_methods( TSD, c, "REMOVE", m_h_remove, 0 );
   add_methods( TSD, c, "ITEMS", m_h_items, 0 );
   add_methods( TSD, c, "ISEMPTY", m_h_isempty, 0 );
   add_methods( TSD, c, "EMPTY", m_h_empty, 0 );
   add_methods( TSD, c, "MAKEARRAY ALLINDEXES", m_h_indexes, 0 );
   add_methods( TSD, c, "ALLITEMS", m_h_allitems, 0 );
   add_methods( TSD, c, "HASITEM", m_h_hasitem, 0 );
   add_methods( TSD, c, "INDEX", m_h_index, 0 );
   add_methods( TSD, c, "SUPPLIER", m_supplier_of, 0 );
   if ( directory )
   {
      add_methods( TSD, c, "ENTRY", m_h_at, 0 );
      add_methods( TSD, c, "SETENTRY", m_h_put, 0 );
      add_methods( TSD, c, "HASENTRY", m_h_has, 0 );
      add_methods( TSD, c, "REMOVEENTRY", m_h_remove, 0 );
      add_methods( TSD, c, "UNKNOWN", m_dir_unknown, 0 );
   }
}

static void env_put( tsd_t *TSD, oo_tsd *ot, const char *key, streng *value )
{
   streng *k = Str_creTSD( key );
   h_put( TSD, ot->environment->u.h, k, value );
   Free_stringTSD( k );
}

static void init_builtin_classes( tsd_t *TSD, oo_tsd *ot )
{
   int i;
   streng *s;

   /* bootstrap: Class is an instance of itself, Object its superclass */
   ot->cClass = new_class( TSD, "Class", NULL, NULL, OK_CLASS );
   ot->cClass->cls = ot->cClass;
   ot->cObject = new_class( TSD, "Object", NULL, NULL, OK_PLAIN );
   ot->cClass->u.c->super = ot->cObject;

   ot->cString = new_class( TSD, "String", NULL, ot->cObject, OK_STRING );
   ot->cArray = new_class( TSD, "Array", NULL, ot->cObject, OK_ARRAY );
   ot->cQueue = new_class( TSD, "Queue", NULL, ot->cObject, OK_QUEUE );
   ot->cList = new_class( TSD, "List", NULL, ot->cObject, OK_QUEUE );
   ot->cDirectory = new_class( TSD, "Directory", NULL, ot->cObject, OK_DIRECTORY );
   ot->cStringTable = new_class( TSD, "StringTable", NULL, ot->cObject, OK_DIRECTORY );
   ot->cTable = new_class( TSD, "Table", NULL, ot->cObject, OK_TABLE );
   ot->cBuffer = new_class( TSD, "MutableBuffer", NULL, ot->cObject, OK_BUFFER );
   ot->cSupplier = new_class( TSD, "Supplier", NULL, ot->cObject, OK_SUPPLIER );

   /* Object */
   add_methods( TSD, ot->cObject, "INIT", m_nothing, 0 );
   add_methods( TSD, ot->cObject, "STRING DEFAULTNAME OBJECTNAME", m_defaultname, 0 );
   add_methods( TSD, ot->cObject, "OBJECTNAME=", m_setobjectname, 0 );
   add_methods( TSD, ot->cObject, "CLASS", m_class, 0 );
   add_methods( TSD, ot->cObject, "ISA ISINSTANCEOF", m_isa, 0 );
   add_methods( TSD, ot->cObject, "= == \\= \\== <> ><", m_identity_eq, 0 );
   add_methods( TSD, ot->cObject, "HASHCODE IDENTITYHASH", m_hashcode, 0 );
   add_methods( TSD, ot->cObject, "ISNIL", m_isnil, 0 );
   add_methods( TSD, ot->cObject, "SEND", m_send, 0 );
   add_methods( TSD, ot->cObject, "SENDWITH", m_sendwith, 0 );
   add_methods( TSD, ot->cObject, "HASMETHOD", m_hasmethod, 0 );
   add_methods( TSD, ot->cObject, "REQUEST", m_request, 0 );
   add_methods( TSD, ot->cObject, "COPY", m_copy, 0 );

   /* Class: the methods every class object understands */
   add_methods( TSD, ot->cClass, "NEW", m_new, 0 );
   add_methods( TSD, ot->cClass, "ID NAME", m_cls_id, 0 );
   add_methods( TSD, ot->cClass, "SUPERCLASS", m_cls_superclass, 0 );
   add_methods( TSD, ot->cClass, "SUPERCLASSES", m_cls_superclasses, 0 );
   add_methods( TSD, ot->cClass, "SUBCLASS MIXINCLASS", m_cls_subclass, 0 );
   add_methods( TSD, ot->cClass, "INHERIT", m_cls_inherit, 0 );
   add_methods( TSD, ot->cClass, "ISSUBCLASSOF", m_cls_issubclassof, 0 );
   add_methods( TSD, ot->cClass, "QUERYMIXINCLASS", m_cls_querymixin, 0 );
   add_methods( TSD, ot->cClass, "METHODS", m_instancemethod_names, 0 );

   /* String */
   for ( i = 0; string_bifs[i]; i++ )
   {
      s = Str_creTSD( string_bifs[i] );
      add_method( TSD, ot->cString, NULL, s, m_string_bif, 0 );
      Free_stringTSD( s );
   }
   add_methods( TSD, ot->cString, "INIT", m_string_init, 0 );
   add_methods( TSD, ot->cString, "STRING MAKESTRING", m_string_value, 0 );
   add_methods( TSD, ot->cString, "UPPER LOWER", m_upperlower, 0 );
   add_methods( TSD, ot->cString, "EQUALS CASELESSEQUALS STARTSWITH CASELESSSTARTSWITH "
                "ENDSWITH CASELESSENDSWITH CONTAINS CASELESSCONTAINS", m_string_test, 0 );
   add_methods( TSD, ot->cString, "MAKEARRAY", m_string_makearray, 0 );
   for ( i = 0; string_ops[i].name; i++ )
   {
      s = Str_creTSD( string_ops[i].name );
      add_method( TSD, ot->cString, NULL, s, m_string_op, 0 );
      Free_stringTSD( s );
   }

   /* collections */
   add_seq_methods( TSD, ot->cArray );
   add_methods( TSD, ot->cArray, "OF", m_arr_of, MF_CLASS );
   add_methods( TSD, ot->cArray, "FILL", m_arr_fill, 0 );
   add_methods( TSD, ot->cArray, "SORT", m_arr_sort, 0 );
   add_methods( TSD, ot->cArray, "DELETE", m_arr_remove, 0 );
   add_seq_methods( TSD, ot->cQueue );
   add_methods( TSD, ot->cQueue, "PUSH", m_q_push, 0 );
   add_methods( TSD, ot->cQueue, "QUEUE", m_q_queue, 0 );
   add_methods( TSD, ot->cQueue, "PULL", m_q_pull, 0 );
   add_methods( TSD, ot->cQueue, "PEEK", m_q_peek, 0 );
   add_seq_methods( TSD, ot->cList );
   add_methods( TSD, ot->cList, "OF", m_arr_of, MF_CLASS );
   add_hash_methods( TSD, ot->cDirectory, 1 );
   add_hash_methods( TSD, ot->cStringTable, 1 );
   add_hash_methods( TSD, ot->cTable, 0 );

   add_methods( TSD, ot->cBuffer, "INIT", m_buf_init, 0 );
   add_methods( TSD, ot->cBuffer, "APPEND", m_buf_append, 0 );
   add_methods( TSD, ot->cBuffer, "INSERT", m_buf_insert, 0 );
   add_methods( TSD, ot->cBuffer, "DELETE", m_buf_delete, 0 );
   add_methods( TSD, ot->cBuffer, "LENGTH", m_buf_length, 0 );
   add_methods( TSD, ot->cBuffer, "STRING", m_string_value, 0 );
   add_methods( TSD, ot->cBuffer, "SUBSTR POS LASTPOS LEFT RIGHT WORDS WORD COUNTSTR VERIFY",
                m_string_bif, 0 );

   add_methods( TSD, ot->cSupplier, "AVAILABLE", m_sup_available, 0 );
   add_methods( TSD, ot->cSupplier, "ITEM INDEX", m_sup_item, 0 );
   add_methods( TSD, ot->cSupplier, "NEXT", m_sup_next, 0 );
   add_methods( TSD, ot->cSupplier, "ALLITEMS ALLINDEXES", m_sup_all, 0 );

   for ( i = 0; i < ot->nclasses; i++ )
      compute_mro( TSD, ot->classes[i] );

   ot->nil = new_object( TSD, ot->cObject, OK_NIL );

   ot->environment = new_object( TSD, ot->cDirectory, OK_DIRECTORY );
   ot->environment->u.h = (oo_hash *) MallocTSD( sizeof( oo_hash ) );
   memset( ot->environment->u.h, 0, sizeof( oo_hash ) );
   for ( i = 0; i < ot->nclasses; i++ )
   {
      oo_object *c = ot->classes[i];
      h_put( TSD, ot->environment->u.h, c->u.c->id, hdup( TSD, c ) );
   }
   env_put( TSD, ot, "NIL", hdup( TSD, ot->nil ) );
   env_put( TSD, ot, "TRUE", mkstr( TSD, "1" ) );
   env_put( TSD, ot, "FALSE", mkstr( TSD, "0" ) );
}

/* ------------------------------------------------------------------ */
/* directives                                                         */
/* ------------------------------------------------------------------ */

static void labels_add( tsd_t *TSD, oo_labels *L, nodeptr n )
{
   nodeptr *nv = (nodeptr *) MallocTSD( ( L->n + 1 ) * sizeof( nodeptr ) );

   if ( L->n )
   {
      memcpy( nv, L->v, L->n * sizeof( nodeptr ) );
      FreeTSD( L->v );
   }
   nv[L->n++] = n;
   L->v = nv;
}

/*
 * Collects the labels of a statement list (including nested DO/IF/SELECT
 * bodies). If owner is given, the labels are marked as belonging to a
 * directive so that the main program doesn't see them (see getlabel()).
 */
static void collect_labels( tsd_t *TSD, nodeptr n, oo_labels *L, nodeptr owner )
{
   int i;

   for ( ; n; n = n->next )
   {
      if ( n->type == X_DIRECTIVE )
         return;
      if ( n->type == X_LABEL )
      {
         labels_add( TSD, L, n );
         if ( owner )
            n->p[3] = owner;
         continue;
      }
      for ( i = 0; i < 4; i++ )
         if ( n->p[i] && n->p[i]->type != X_DIRECTIVE )
            collect_labels( TSD, n->p[i], L, owner );
   }
}

static int has_statements( nodeptr n )
{
   for ( ; n; n = n->next )
      if ( n->type != X_LABEL && n->type != X_NULL )
         return 1;
   return 0;
}

/* directive words as an array of strings; words[0] is the first word */
static int dir_words( nodeptr d, const streng **w, int max )
{
   nodeptr n;
   int k = 0;

   for ( n = d->p[0]; n && k < max; n = n->next )
      w[k++] = n->name;
   return k;
}

static int opt_is( const streng *w, const char *s )
{
   int l = (int) strlen( s ), i;

   if ( w->len != l )
      return 0;
   for ( i = 0; i < l; i++ )
      if ( rx_toupper( w->value[i] ) != s[i] )
         return 0;
   return 1;
}

static void bad_option( tsd_t *TSD, nodeptr d, const streng *w )
{
   TSD->currentnode = d;
   exiterror( ERR_TRANSLATION_ERROR, 6, tmpstr_of( TSD, w ), tmpstr_of( TSD, d->name ) );
}

static oo_method *new_user_method( tsd_t *TSD, oo_object *cls, const streng *name,
                                   int flags, nodeptr d, nodeptr body, oo_program *prog )
{
   oo_method *m, *list;

   list = ( flags & MF_CLASS ) ? cls->u.c->cmethods : cls->u.c->imethods;
   {
      streng *u = upper_dup( TSD, name );
      if ( find_in( list, u ) )
      {
         TSD->currentnode = d;
         exiterror( ERR_TRANSLATION_ERROR, 3, tmpstr_of( TSD, u ),
                    tmpstr_of( TSD, cls->u.c->name ) );
      }
      m = add_method( TSD, cls, NULL, u, NULL, flags );
      Free_stringTSD( u );
   }
   m->body = body;
   m->prog = prog;
   if ( body )
      collect_labels( TSD, body, &m->labels, d );
   return m;
}

static void add_attribute( tsd_t *TSD, oo_object *cls, const streng *name, int flags,
                           int get, int set, nodeptr d, oo_program *prog )
{
   oo_method *m;
   streng *setname;

   if ( get )
   {
      m = new_user_method( TSD, cls, name, flags | MF_ATTR_GET, d, NULL, prog );
      m->attr = upper_dup( TSD, name );
   }
   if ( set )
   {
      setname = Str_dupTSD( name );
      setname = catc( TSD, setname, "=" );
      m = new_user_method( TSD, cls, setname, flags | MF_ATTR_SET, d, NULL, prog );
      m->attr = upper_dup( TSD, name );
      Free_stringTSD( setname );
   }
}

static streng *find_required( tsd_t *TSD, const streng *name, oo_program *from );
static void load_required( tsd_t *TSD, const streng *name, oo_program *from, nodeptr d );

static void resolve_class( tsd_t *TSD, oo_object *c )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_classdata *cd = c->u.c;
   oo_object *s;
   int i;

   if ( cd->resolved )
      return;
   cd->resolved = 1;    /* also protects against cycles */
   if ( cd->supername )
   {
      s = class_by_id( ot, cd->supername );
      if ( s == NULL || s == c )
         exiterror( ERR_EXECUTION_ERROR, 1, tmpstr_of( TSD, cd->supername ) );
      if ( !s->u.c->resolved )
         resolve_class( TSD, s );
      cd->super = s;
   }
   else if ( cd->super == NULL )
      cd->super = ot->cObject;
   cd->ikind = cd->super->u.c->ikind;
   if ( cd->ninherits )
   {
      cd->inherits = (oo_object **) MallocTSD( cd->ninherits * sizeof( oo_object * ) );
      for ( i = 0; i < cd->ninherits; i++ )
      {
         s = class_by_id( ot, cd->inheritnames[i] );
         if ( s == NULL )
            exiterror( ERR_EXECUTION_ERROR, 1, tmpstr_of( TSD, cd->inheritnames[i] ) );
         if ( !s->u.c->resolved )
            resolve_class( TSD, s );
         cd->inherits[i] = s;
      }
   }
   invalidate_mros( ot, TSD );
}

static void process_directives( tsd_t *TSD, oo_program *prog, nodeptr first )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *cls = NULL, **mine = NULL;
   int nmine = 0, i, k, nw;
   nodeptr d;
   const streng *w[64];
   nodeptr save = TSD->currentnode;

   /* ::REQUIRES first, so that their classes are known */
   for ( d = first; d; d = d->next )
   {
      if ( d->type != X_DIRECTIVE || !str_eqc( d->name, "REQUIRES" ) )
         continue;
      nw = dir_words( d, w, 64 );
      TSD->currentnode = d;
      if ( nw < 1 )
         exiterror( ERR_TRANSLATION_ERROR, 5, "REQUIRES" );
      load_required( TSD, w[0], prog, d );
   }

   mine = (oo_object **) MallocTSD( 64 * sizeof( oo_object * ) );
   for ( d = first; d; d = d->next )
   {
      if ( d->type != X_DIRECTIVE )
         continue;
      TSD->currentnode = d;
      nw = dir_words( d, w, 64 );
      if ( str_eqc( d->name, "REQUIRES" ) || str_eqc( d->name, "OPTIONS" ) )
         continue;
      if ( nw < 1 )
         exiterror( ERR_TRANSLATION_ERROR, 5, tmpstr_of( TSD, d->name ) );

      if ( str_eqc( d->name, "CLASS" ) )
      {
         oo_classdata *cd;
         streng *id = upper_dup( TSD, w[0] );
         for ( i = 0; i < nmine; i++ )
            if ( str_eq( mine[i]->u.c->id, id ) )
               exiterror( ERR_TRANSLATION_ERROR, 2, tmpstr_of( TSD, w[0] ) );
         Free_stringTSD( id );
         cls = new_class( TSD, NULL, w[0], NULL, -1 );
         cd = cls->u.c;
         cd->resolved = 0;
         cd->prog = prog;
         if ( nmine % 64 == 63 )
         {
            oo_object **n = (oo_object **) MallocTSD( ( nmine + 65 ) * sizeof( oo_object * ) );
            memcpy( n, mine, nmine * sizeof( oo_object * ) );
            FreeTSD( mine );
            mine = n;
         }
         mine[nmine++] = cls;
         for ( k = 1; k < nw; k++ )
         {
            if ( ( opt_is( w[k], "SUBCLASS" ) || opt_is( w[k], "MIXINCLASS" ) ) && k + 1 < nw )
            {
               cd->mixin = opt_is( w[k], "MIXINCLASS" );
               cd->supername = upper_dup( TSD, w[++k] );
            }
            else if ( opt_is( w[k], "METACLASS" ) && k + 1 < nw )
               k++;
            else if ( opt_is( w[k], "INHERIT" ) )
            {
               int j;
               cd->inheritnames = (streng **) MallocTSD( nw * sizeof( streng * ) );
               for ( j = k + 1; j < nw && !opt_is( w[j], "PUBLIC" ) && !opt_is( w[j], "PRIVATE" ); j++ )
                  cd->inheritnames[cd->ninherits++] = upper_dup( TSD, w[j] );
               k = j - 1;
            }
            else if ( opt_is( w[k], "PUBLIC" ) || opt_is( w[k], "PRIVATE" ) )
               ;
            else
               bad_option( TSD, d, w[k] );
         }
         /* environment entry, so that .NAME finds it */
         h_put( TSD, ot->environment->u.h, cd->id, hdup( TSD, cls ) );
      }
      else if ( str_eqc( d->name, "METHOD" ) || str_eqc( d->name, "ATTRIBUTE" ) )
      {
         int flags = 0, attr = 0, get = 0, set = 0, isattrdir = str_eqc( d->name, "ATTRIBUTE" );
         if ( cls == NULL )
            exiterror( ERR_TRANSLATION_ERROR, 4, tmpstr_of( TSD, d->name ) );
         for ( k = 1; k < nw; k++ )
         {
            if ( opt_is( w[k], "CLASS" ) )
               flags |= MF_CLASS;
            else if ( opt_is( w[k], "PRIVATE" ) )
               flags |= MF_PRIVATE;
            else if ( opt_is( w[k], "ABSTRACT" ) )
               flags |= MF_ABSTRACT;
            else if ( opt_is( w[k], "ATTRIBUTE" ) && !isattrdir )
               attr = 1;
            else if ( opt_is( w[k], "GET" ) && isattrdir )
               get = 1;
            else if ( opt_is( w[k], "SET" ) && isattrdir )
               set = 1;
            else if ( opt_is( w[k], "PUBLIC" ) || opt_is( w[k], "GUARDED" )
                   || opt_is( w[k], "UNGUARDED" ) || opt_is( w[k], "PROTECTED" )
                   || opt_is( w[k], "UNPROTECTED" ) )
               ;
            else
               bad_option( TSD, d, w[k] );
         }
         if ( isattrdir )
         {
            if ( !get && !set )
               get = set = 1;
            if ( has_statements( d->p[1] ) )
            {
               /* ::ATTRIBUTE name GET/SET with an explicit body */
               streng *n = Str_dupTSD( w[0] );
               if ( set && !get )
                  n = catc( TSD, n, "=" );
               new_user_method( TSD, cls, n, flags, d, d->p[1], prog );
               Free_stringTSD( n );
            }
            else
               add_attribute( TSD, cls, w[0], flags, get, set, d, prog );
         }
         else if ( attr )
            add_attribute( TSD, cls, w[0], flags, 1, 1, d, prog );
         else
            new_user_method( TSD, cls, w[0], flags, d, d->p[1], prog );
      }
      else if ( str_eqc( d->name, "CONSTANT" ) )
      {
         oo_method *m;
         streng *v;
         if ( cls == NULL )
            exiterror( ERR_TRANSLATION_ERROR, 4, "CONSTANT" );
         v = nw > 1 ? Str_dupTSD( w[1] ) : nullstringptr();
         m = new_user_method( TSD, cls, w[0], MF_CONSTANT, d, NULL, prog );
         m->constval = v;
         m = new_user_method( TSD, cls, w[0], MF_CONSTANT | MF_CLASS, d, NULL, prog );
         m->constval = Str_dupTSD( v );
      }
      else if ( str_eqc( d->name, "ROUTINE" ) )
      {
         oo_routine *r;
         streng *id = upper_dup( TSD, w[0] );
         for ( r = ot->routines; r; r = r->next )
            if ( r->prog == prog && str_eq( r->name, id ) )
               exiterror( ERR_TRANSLATION_ERROR, 7, tmpstr_of( TSD, w[0] ) );
         for ( k = 1; k < nw; k++ )
            if ( !opt_is( w[k], "PUBLIC" ) && !opt_is( w[k], "PRIVATE" ) )
               bad_option( TSD, d, w[k] );
         r = (oo_routine *) MallocTSD( sizeof( oo_routine ) );
         memset( r, 0, sizeof( oo_routine ) );
         r->name = id;
         r->body = d->p[1];
         r->prog = prog;
         collect_labels( TSD, d->p[1], &r->labels, d );
         r->next = ot->routines;
         ot->routines = r;
         cls = NULL;
      }
      else
         exiterror( ERR_TRANSLATION_ERROR, 1, tmpstr_of( TSD, d->name ) );
   }

   for ( i = 0; i < nmine; i++ )
      resolve_class( TSD, mine[i] );
   for ( i = 0; i < ot->nclasses; i++ )
      if ( ot->classes[i]->u.c->mro == NULL )
         compute_mro( TSD, ot->classes[i] );
   FreeTSD( mine );
   TSD->currentnode = save;
}

/*
 * X_OO_INIT is the first statement of every program with directives. It
 * sets up the classes, methods and routines of that program.
 */
static oo_program *loading_prog = NULL;

void oo_exec_init( tsd_t *TSD, nodeptr thisptr )
{
   oo_tsd *ot = get_ot( TSD );
   oo_program *prog;

   if ( thisptr->u.nonansi )      /* already done for this tree */
      return;
   thisptr->u.nonansi = 1;

   if ( loading_prog )
   {
      prog = loading_prog;
      loading_prog = NULL;
   }
   else
   {
      prog = (oo_program *) MallocTSD( sizeof( oo_program ) );
      memset( prog, 0, sizeof( oo_program ) );
      prog->tree = TSD->systeminfo->tree;
      prog->file = TSD->systeminfo->input_file ? Str_dupTSD( TSD->systeminfo->input_file ) : NULL;
      /* the parse tree must survive the program: methods live in it */
      TSD->systeminfo->oo_keep_tree = 1;
      collect_labels( TSD, thisptr->next, &prog->labels, NULL );
      prog->next = ot->programs;
      ot->programs = prog;
   }
   prog->processed = 1;
   process_directives( TSD, prog, thisptr->next );
}

/* ------------------------------------------------------------------ */
/* ::REQUIRES                                                         */
/* ------------------------------------------------------------------ */

static streng *try_file( tsd_t *TSD, const char *dir, int dlen, const streng *name )
{
   static const char *const exts[] = { "", ".cls", ".rex", ".rexx", NULL };
   char *buf;
   int i, n;
   FILE *f;
   streng *r = NULL;

   buf = (char *) MallocTSD( dlen + name->len + 16 );
   for ( i = 0; exts[i] && r == NULL; i++ )
   {
      n = 0;
      if ( dlen > 0 )
      {
         memcpy( buf, dir, dlen );
         n = dlen;
         if ( buf[n - 1] != '/' && buf[n - 1] != '\\' )
            buf[n++] = FILE_SEPARATOR;
      }
      memcpy( buf + n, name->value, name->len );
      n += name->len;
      strcpy( buf + n, exts[i] );
      if ( ( f = fopen( buf, "r" ) ) != NULL )
      {
         fclose( f );
         r = Str_crestrTSD( buf );   /* 0-terminated for fopen() */
      }
   }
   FreeTSD( buf );
   return r;
}

static streng *find_required( tsd_t *TSD, const streng *name, oo_program *from )
{
   streng *r;
   const char *env, *p, *e;
   int i;

   if ( from && from->file )
   {
      for ( i = from->file->len - 1; i >= 0; i-- )
         if ( from->file->value[i] == '/' || from->file->value[i] == '\\' )
            break;
      if ( i > 0 && ( r = try_file( TSD, from->file->value, i, name ) ) != NULL )
         return r;
   }
   if ( ( r = try_file( TSD, "", 0, name ) ) != NULL )
      return r;
   env = getenv( "REGINA_MACROS" );
   for ( p = env; p && *p; p = *e ? e + 1 : e )
   {
      for ( e = p; *e && *e != PATH_SEPARATOR; e++ )
         ;
      if ( e > p && ( r = try_file( TSD, p, (int) ( e - p ), name ) ) != NULL )
         return r;
   }
   return NULL;
}

typedef struct { oo_program *p; oo_frame *frame; } prolog_data;

static void run_prolog( tsd_t *TSD, void *data )
{
   prolog_data *pd = (prolog_data *) data;
   streng *r;

   r = run_body( TSD, pd->p->tree.root, pd->frame, NULL, NULL );
   if ( r )
      Free_stringTSD( r );
}

static void load_required( tsd_t *TSD, const streng *name, oo_program *from, nodeptr d )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   streng *path = find_required( TSD, name, from );
   oo_program *p;
   FILE *fp;
   internal_parser_type parsing;
   oo_frame *frame;
   prolog_data pd;

   if ( path == NULL )
      exiterror( ERR_EXECUTION_ERROR, 3, tmpstr_of( TSD, name ) );
   for ( p = ot->programs; p; p = p->next )
      if ( p->file && str_eq( p->file, path ) )
      {
         Free_stringTSD( path );
         return;
      }
   if ( ( fp = fopen( path->value, "r" ) ) == NULL )
      exiterror( ERR_EXECUTION_ERROR, 3, tmpstr_of( TSD, name ) );
   fetch_file( TSD, fp, &parsing );
   fclose( fp );
   if ( parsing.result != 0 )
   {
      TSD->currentnode = d;
      exiterror( ERR_YACC_SYNTAX, 1, parsing.tline );
   }

   p = (oo_program *) MallocTSD( sizeof( oo_program ) );
   memset( p, 0, sizeof( oo_program ) );
   p->tree = parsing;
   p->file = path;
   p->next = ot->programs;
   ot->programs = p;
   collect_labels( TSD, parsing.root, &p->labels, NULL );

   /* run the prolog; its X_OO_INIT sets up the classes of the file */
   frame = (oo_frame *) MallocTSD( sizeof( oo_frame ) );
   frame->self = NULL;
   frame->scope = NULL;
   frame->labels = &p->labels;
   loading_prog = ( parsing.root && parsing.root->type == X_OO_INIT ) ? p : NULL;
   pd.p = p;
   pd.frame = frame;
   oo_exit_protected( TSD, run_prolog, &pd );
   loading_prog = NULL;
   FreeTSD( frame );
}

/* ------------------------------------------------------------------ */
/* labels and routines                                                */
/* ------------------------------------------------------------------ */

nodeptr oo_getlabel( const tsd_t *TSD, const streng *name )
{
   oo_frame *f = (oo_frame *) TSD->currlevel->oo_ctx;
   int i;

   if ( f == NULL || f->labels == NULL )
      return NULL;
   for ( i = 0; i < f->labels->n; i++ )
      if ( Str_ccmp( f->labels->v[i]->name, name ) == 0 )
         return f->labels->v[i];
   return NULL;
}

static oo_routine *find_routine( tsd_t *TSD, const streng *name )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_routine *r;

   if ( ot == NULL )
      return NULL;
   for ( r = ot->routines; r; r = r->next )
      if ( Str_ccmp( r->name, name ) == 0 )
         return r;
   return NULL;
}

int oo_has_routine( tsd_t *TSD, const streng *name )
{
   return find_routine( TSD, name ) != NULL;
}

streng *oo_call_routine( tsd_t *TSD, const streng *name, paramboxptr args )
{
   oo_routine *r = find_routine( TSD, name );
   oo_frame *frame;
   streng *res;

   if ( r == NULL )
      return NOFUNC;
   frame = (oo_frame *) MallocTSD( sizeof( oo_frame ) );
   frame->self = NULL;
   frame->scope = NULL;
   frame->labels = &r->labels;
   res = run_body( TSD, r->body, frame, args, NULL );
   FreeTSD( frame );
   return res;
}

/* ------------------------------------------------------------------ */
/* expression and instruction entry points                            */
/* ------------------------------------------------------------------ */

/* evaluates an argument list (X_EXPRLIST chain) into an argv array */
static int eval_args( tsd_t *TSD, nodeptr list, paramboxptr *plist, const streng ***argv )
{
   treenode fake;
   paramboxptr p;
   int n = 0, i;

   memset( &fake, 0, sizeof( fake ) );
   fake.p[0] = list;
   *plist = list ? initplist( TSD, &fake ) : NULL;
   for ( p = *plist; p; p = p->next )
      n++;
   *argv = (const streng **) MallocTSD( ( n + 1 ) * sizeof( streng * ) );
   for ( p = *plist, i = 0; p; p = p->next, i++ )
      (*argv)[i] = p->value;
   return n;
}

streng *oo_eval_message( tsd_t *TSD, nodeptr thisptr, int need_result )
{
   streng *recv, *kill = NULL, *result;
   paramboxptr plist;
   const streng **argv;
   int argc;

   get_ot( TSD );
   recv = evaluate( TSD, thisptr->p[0], &kill );
   if ( kill == NULL )
      kill = recv = Str_dupTSD( recv );
   argc = eval_args( TSD, thisptr->p[1], &plist, &argv );
   if ( thisptr->u.nonansi )
   {
      /* ~name:super -- start the lookup after the current method's class */
      oo_frame *f = (oo_frame *) TSD->currlevel->oo_ctx;
      if ( f == NULL || f->self == NULL )
         exiterror( ERR_EXECUTION_ERROR, 4, ":SUPER" );
      result = send_from( TSD, recv, thisptr->name, f->scope, argc, argv );
   }
   else
      result = send_msg( TSD, recv, thisptr->name, argc, argv );
   if ( plist )
      deallocplink( TSD, plist );
   FreeTSD( (void *) argv );
   if ( thisptr->type == X_CASCADE )
   {
      if ( result )
         Free_stringTSD( result );
      result = kill;
      kill = NULL;
      if ( obj_of( TSD, result ) && obj_of( TSD, result )->kind == OK_SUPER )
      {
         streng *t = hdup( TSD, obj_of( TSD, result )->u.sup.self );
         Free_stringTSD( result );
         result = t;
      }
   }
   if ( kill )
      Free_stringTSD( kill );
   if ( need_result && result == NULL )
      exiterror( ERR_NO_RESULT_OBJECT, 1, tmpstr_of( TSD, thisptr->name ) );
   return result;
}

streng *oo_eval_index( tsd_t *TSD, nodeptr thisptr )
{
   streng *recv, *kill = NULL, *result;
   paramboxptr plist;
   const streng **argv;
   int argc;
   static const streng *idx = NULL;
   streng *name = Str_creTSD( "[]" );

   (void) idx;
   get_ot( TSD );
   recv = evaluate( TSD, thisptr->p[0], &kill );
   if ( kill == NULL )
      kill = recv = Str_dupTSD( recv );
   argc = eval_args( TSD, thisptr->p[1], &plist, &argv );
   result = send_msg( TSD, recv, name, argc, argv );
   if ( plist )
      deallocplink( TSD, plist );
   FreeTSD( (void *) argv );
   Free_stringTSD( kill );
   if ( result == NULL )
      exiterror( ERR_NO_RESULT_OBJECT, 1, "[]" );
   Free_stringTSD( name );
   return result;
}

streng *oo_eval_envsymbol( tsd_t *TSD, nodeptr thisptr )
{
   oo_tsd *ot = get_ot( TSD );
   streng *id;
   oo_object *c;
   unsigned hv;
   oo_entry *e;

   id = Str_ncreTSD( thisptr->name->value + 1, thisptr->name->len - 1 );
   mem_upper( id->value, id->len );
   if ( str_eqc( id, "ENVIRONMENT" ) || str_eqc( id, "LOCAL" ) )
   {
      Free_stringTSD( id );
      return hdup( TSD, ot->environment );
   }
   if ( ( c = class_by_id( ot, id ) ) != NULL )
   {
      Free_stringTSD( id );
      return hdup( TSD, c );
   }
   e = h_find( ot->environment->u.h, id, &hv );
   Free_stringTSD( id );
   if ( e )
      return Str_dupTSD( e->value );
   return Str_dupTSD( thisptr->name );
}

void oo_exec_message( tsd_t *TSD, nodeptr thisptr )
{
   streng *r = oo_eval_message( TSD, thisptr->p[0], 0 );

   if ( r )
      set_reserved_value( TSD, POOL0_RESULT, r, 0, VFLAG_STR );
   else
      set_reserved_value( TSD, POOL0_RESULT, NULL, 0, VFLAG_NONE );
}

void oo_exec_assign( tsd_t *TSD, nodeptr thisptr )
{
   nodeptr target = thisptr->p[0];
   streng *recv, *kill = NULL, *value, *vkill = NULL, *name, *r;
   paramboxptr plist = NULL;
   const streng **argv, **all;
   int argc = 0, i;

   get_ot( TSD );
   recv = evaluate( TSD, target->p[0], &kill );
   if ( kill == NULL )
      kill = recv = Str_dupTSD( recv );
   if ( target->type == X_INDEX )
   {
      argc = eval_args( TSD, target->p[1], &plist, &argv );
      name = Str_creTSD( "[]=" );
   }
   else
   {
      argv = (const streng **) MallocTSD( sizeof( streng * ) );
      name = Str_dupTSD( target->name );
      name = catc( TSD, name, "=" );
   }
   value = evaluate( TSD, thisptr->p[1], &vkill );
   all = (const streng **) MallocTSD( ( argc + 2 ) * sizeof( streng * ) );
   all[0] = value;
   for ( i = 0; i < argc; i++ )
      all[i + 1] = argv[i];
   r = send_msg( TSD, recv, name, argc + 1, all );
   if ( r )
      Free_stringTSD( r );
   FreeTSD( (void *) all );
   FreeTSD( (void *) argv );
   if ( plist )
      deallocplink( TSD, plist );
   if ( vkill )
      Free_stringTSD( vkill );
   Free_stringTSD( kill );
   Free_stringTSD( name );
}

void oo_exec_expose( tsd_t *TSD, nodeptr thisptr )
{
   oo_frame *f = (oo_frame *) TSD->currlevel->oo_ctx;
   oo_scope *sc;
   nodeptr n;

   if ( f == NULL || f->self == NULL )
      exiterror( ERR_EXECUTION_ERROR, 4, "EXPOSE" );
   sc = get_scope( TSD, f->self, f->scope );
   for ( n = thisptr->p[0]; n; n = n->p[0] )
      if ( n->name )
         oo_expose( TSD, sc->pool, n->name );
}

void oo_exec_usearg( tsd_t *TSD, nodeptr thisptr )
{
   paramboxptr a = TSD->currlevel->args;
   nodeptr n;

   for ( n = thisptr->p[0]; n; n = n->p[0] )
   {
      if ( n->name )
      {
         if ( a && a->value )
            setvalue( TSD, n->name, Str_dupTSD( a->value ), -1 );
         else
            drop_var( TSD, n->name );
      }
      if ( a )
         a = a->next;
   }
}

/* ------------------------------------------------------------------ */
/* DO name OVER collection                                            */
/* ------------------------------------------------------------------ */

typedef struct { tsd_t *TSD; streng **v; long n, cap; } tailcol;

static void tail_cb( tsd_t *TSD, void *data, const streng *tail, const streng *value )
{
   tailcol *t = (tailcol *) data;

   if ( t->n >= t->cap )
   {
      long ncap = t->cap ? t->cap * 2 : 16;
      streng **n = (streng **) MallocTSD( ncap * sizeof( streng * ) );
      if ( t->v )
      {
         memcpy( n, t->v, t->n * sizeof( streng * ) );
         FreeTSD( t->v );
      }
      t->v = n;
      t->cap = ncap;
   }
   t->v[t->n++] = Str_dupTSD( tail );
   (void) value;
}

static int is_whole( const streng *s, double *d )
{
   int i;
   char buf[40];

   if ( s->len == 0 || s->len > 30 )
      return 0;
   for ( i = 0; i < s->len; i++ )
      if ( !( s->value[i] >= '0' && s->value[i] <= '9' ) )
         return 0;
   memcpy( buf, s->value, s->len );
   buf[s->len] = '\0';
   *d = atof( buf );
   return 1;
}

static int cmp_tails( const void *x, const void *y )
{
   const streng *a = *(const streng * const *) x, *b = *(const streng * const *) y;
   double da, db;
   int wa = is_whole( a, &da ), wb = is_whole( b, &db );

   if ( wa && wb )
      return da < db ? -1 : ( da > db ? 1 : 0 );
   if ( wa != wb )
      return wa ? -1 : 1;
   return cmp_strings( x, y );
}

streng *oo_eval_over_init( tsd_t *TSD, nodeptr thisptr )
{
   oo_tsd *ot = get_ot( TSD );
   nodeptr coll = thisptr->p[0];
   oo_object *snap, *o, *old;
   const streng *cur;
   long i, n = 0;

   /* release the snapshot of the previous run of this loop */
   cur = isvariable( TSD, thisptr->name ) ? getvalue( TSD, thisptr->name, -1 ) : NULL;
   if ( cur && ( old = obj_of( TSD, cur ) ) != NULL && old->snapshot )
      free_object( TSD, old );

   snap = new_object( TSD, ot->cArray, OK_ARRAY );
   snap->snapshot = 1;

   if ( coll->type == X_STEM_SYMBOL )
   {
      tailcol t;
      t.TSD = TSD;
      t.v = NULL;
      t.n = t.cap = 0;
      oo_stem_tails( TSD, coll->name, tail_cb, &t );
      if ( t.n )
         qsort( t.v, t.n, sizeof( streng * ), cmp_tails );
      for ( i = 0; i < t.n; i++ )
         oo_array_put( TSD, snap, ++n, t.v[i] );
      if ( t.v )
         FreeTSD( t.v );
   }
   else
   {
      streng *kill = NULL, *v = evaluate( TSD, coll, &kill );
      streng *arrh = NULL;

      o = obj_of( TSD, v );
      if ( o && o->kind != OK_ARRAY && o->kind != OK_QUEUE && o->kind != OK_DIRECTORY
        && o->kind != OK_TABLE && o->kind != OK_SUPPLIER )
      {
         /* any object that understands MAKEARRAY */
         arrh = send_msg( TSD, v, ot->s_makearray, 0, NULL );
         o = obj_of( TSD, arrh );
         if ( o == NULL || ( o->kind != OK_ARRAY && o->kind != OK_QUEUE ) )
            o = NULL;
      }
      if ( o == NULL )
      {
         streng *d = describe( TSD, v );
         exiterror( ERR_EXECUTION_ERROR, 5, tmpstr_of( TSD, d ) );
      }
      if ( o->kind == OK_ARRAY || o->kind == OK_QUEUE )
      {
         for ( i = 0; i < o->u.a.size; i++ )
            if ( o->u.a.v[i] )
               oo_array_put( TSD, snap, ++n, Str_dupTSD( o->u.a.v[i] ) );
      }
      else if ( o->kind == OK_SUPPLIER )
      {
         for ( i = o->u.sp.pos; i <= o->u.sp.items->u.a.size; i++ )
            if ( o->u.sp.items->u.a.v[i - 1] )
               oo_array_put( TSD, snap, ++n, Str_dupTSD( o->u.sp.items->u.a.v[i - 1] ) );
      }
      else
      {
         oo_entry *e;
         for ( e = o->u.h->first; e; e = e->onext )
            oo_array_put( TSD, snap, ++n, Str_dupTSD( e->key ) );
      }
      if ( kill )
         Free_stringTSD( kill );
      if ( arrh )
         Free_stringTSD( arrh );
   }
   setvalue( TSD, thisptr->name, hdup( TSD, snap ), -1 );
   return mkint( TSD, n );
}

streng *oo_eval_over_item( tsd_t *TSD, nodeptr thisptr )
{
   streng *k1 = NULL, *k2 = NULL, *r;
   const streng *a = evaluate( TSD, thisptr->p[0], &k1 );
   const streng *i = evaluate( TSD, thisptr->p[1], &k2 );
   oo_object *o = obj_of( TSD, a );
   int err = 0;
   long idx = streng_to_int( TSD, i, &err );

   if ( o == NULL || err || idx < 1 || idx > o->u.a.size )
      exiterror( ERR_INTERPRETER_FAILURE, 1, __FILE__, __LINE__, "DO OVER" );
   r = Str_dupTSD( o->u.a.v[idx - 1] );
   if ( k1 )
      Free_stringTSD( k1 );
   if ( k2 )
      Free_stringTSD( k2 );
   return r;
}

/* DO name OVER c WHILE cond: set name before cond is tested */
void oo_over_assign( tsd_t *TSD, nodeptr thisptr )
{
   streng *v = oo_eval_over_item( TSD, thisptr->p[1] );

   if ( thisptr->p[0]->type == X_HEAD_SYMBOL )
      fix_compound( TSD, thisptr->p[0], v );
   else
      setshortcut( TSD, thisptr->p[0], v );
}

streng *oo_describe( tsd_t *TSD, const streng *s )
{
   oo_object *o = obj_of( TSD, s );

   return o ? default_name( TSD, o ) : NULL;
}
