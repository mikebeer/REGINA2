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
#include <time.h>
#include "pybridge.h"
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
streng *oo_run_level( tsd_t *TSD, nodeptr body, paramboxptr args, void *ctx,
                      void (*setup)( tsd_t *, void * ), void *data );

/* ------------------------------------------------------------------ */
/* OO mode: --oo, --no-oo, REGINA_OO                                  */
/* ------------------------------------------------------------------ */

static int oo_cmdline_mode = 0;

void oo_set_mode( int mode )
{
   oo_cmdline_mode = mode;
}

static int oo_word_is( const char *e, const char *w )
{
   for ( ; *e && *w; e++, w++ )
      if ( rx_toupper( *e ) != *w )
         return 0;
   return *e == '\0' && *w == '\0';
}

int oo_mode( void )
{
   static int env_mode = 2;      /* 2: REGINA_OO not read yet */

   if ( oo_cmdline_mode )
      return oo_cmdline_mode;
   if ( env_mode == 2 )
   {
      const char *e = getenv( "REGINA_OO" );
      env_mode = 0;
      if ( e )
      {
         if ( oo_word_is( e, "1" ) || oo_word_is( e, "ON" ) || oo_word_is( e, "YES" )
           || oo_word_is( e, "OO" ) )
            env_mode = 1;
         else if ( oo_word_is( e, "0" ) || oo_word_is( e, "OFF" ) || oo_word_is( e, "NO" )
                || oo_word_is( e, "CLASSIC" ) )
            env_mode = -1;
      }
   }
   return env_mode;
}

/* ------------------------------------------------------------------ */
/* data structures                                                    */
/* ------------------------------------------------------------------ */

enum {
   OK_PLAIN = 0, OK_CLASS, OK_NIL, OK_ARRAY, OK_DIRECTORY, OK_TABLE,
   OK_QUEUE, OK_BUFFER, OK_SUPER, OK_SUPPLIER, OK_STRING, OK_STEM, OK_CONS,
   OK_PYOBJ, OK_BLOCK, OK_INTERVAL
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
   unsigned char gcmark;        /* reached in this collection        */
   unsigned char gcroot;        /* never collected (held by Python)  */
   unsigned long condemned;     /* unreachable since this clause     */
   streng *handle;
   streng *objname;             /* set by OBJECTNAME=                */
   streng *aux;                 /* SortedCollection: its sort block  */
   oo_scope *scopes;
   union {
      oo_classdata *c;
      /* Array, Queue, List; an Array with ndims > 1 is stored row by row */
      struct { streng **v; long size, cap; long *dims; int ndims; } a;
      oo_hash *h;
      streng *s;                /* MutableBuffer and String values   */
      struct { struct oo_object *self; struct oo_object *start; } sup;
      struct { struct oo_object *items, *indexes; long pos; } sp;
      var_hashtable *pool;      /* Stem: private pool with stem "S." */
      struct { streng *car, *cdr; } cons;   /* Cons: a LISP cell     */
      long pyref;               /* PyObject: see pybridge.h          */
      struct { long from, to, by; } iv;   /* Interval                  */
      struct { struct oo_blockcode *code; proclevel home; unsigned long serial;
               struct oo_frame *homeframe;
               var_hashtable *kept;          /* variables after home ended */
               struct oo_object *nextblock;  /* blocks of the same home */
             } blk;   /* Block */
   } u;
} oo_object;

typedef struct oo_frame {        /* stored in proclevel->oo_ctx       */
   oo_object *self;             /* NULL in a routine                 */
   oo_object *scope;
   oo_labels *labels;
   int block;                   /* a block: labels of outer count too */
   struct oo_frame *outer;      /* frame of the block's creator      */
} oo_frame;

typedef struct oo_blockcode {    /* a parsed block, one per { } node   */
   struct oo_blockcode *next;
   nodeptr node;
   streng *src;
   internal_parser_type tree;
   oo_labels labels;
   nodeptr body;
   int procedure;               /* begins with PROCEDURE             */
   int nparams;                 /* names of its first USE ARG        */
   streng **params;
} oo_blockcode;

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
   streng *s_stem;                   /* "S.", the stem inside a Stem      */
   struct oo_object *cStem;
   struct oo_object *cCons, *cNilList;   /* LISP lists; methods of .nil */
   /* stem references of the arguments of the message being sent */
   const streng **pend_argv;
   paramboxptr pend_plist;
   struct oo_program *loading;  /* ::REQUIRES file being loaded */
   struct oo_object *cPython, *cPyObject;   /* the Python bridge     */
   struct oo_object *cBlock;
   struct oo_object *cInterval, *cSet, *cBag, *cSorted;
   struct oo_object *extending;   /* class of the current ::EXTENSION */
   struct oo_object *cMessage;
   /* garbage collection, see oo_gc() */
#define OO_PINS 4096
   unsigned long pins[OO_PINS];
   int pintop;
   unsigned long nlive, gc_next, gc_runs, gc_freed;
   oo_blockcode *blocks;
} oo_tsd;

/* ------------------------------------------------------------------ */
/* basic helpers                                                      */
/* ------------------------------------------------------------------ */

static void init_builtin_classes( tsd_t *TSD, oo_tsd *ot );
static int ws_nbuiltin = -1;    /* built-in classes, see []NL         */
static streng *m_obj_concat( tsd_t *TSD, struct oo_call *c );

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
      ot->s_stem = Str_creTSD( "S." );
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

static streng *hdup( tsd_t *TSD, oo_object *o );
static void gc_count( tsd_t *TSD, struct oo_tsd *ot );
static int pin_object( tsd_t *TSD, oo_object *o );
static void unpin_object( tsd_t *TSD, int k );

static oo_frame *new_frame( tsd_t *TSD )
{
   oo_frame *f = (oo_frame *) MallocTSD( sizeof( oo_frame ) );
   memset( f, 0, sizeof( oo_frame ) );
   return f;
}

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
   if ( kind == OK_STEM )
      o->u.pool = oo_pool_new( TSD );
   if ( kind == OK_CONS )
   {
      /* a cell is never empty: (nil . nil) until INIT sets it */
      o->u.cons.car = hdup( TSD, ot->nil );
      o->u.cons.cdr = hdup( TSD, ot->nil );
   }
   o->kind = kind;
   o->handle = Str_makeTSD( OO_HANDLE_LEN );
   o->handle->len = OO_HANDLE_LEN;
   memcpy( o->handle->value, "\000\001OBJ\002", 6 );
   sprintf( hex, "%08lX", id );
   memcpy( o->handle->value + 6, hex, 8 );
   o->handle->value[14] = '\003';
   o->handle->value[15] = '\000';
   ot->objs[id] = o;
   gc_count( TSD, ot );
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
      if ( o->u.a.dims )
         FreeTSD( o->u.a.dims );
   }
   if ( o->kind == OK_CONS )
   {
      if ( o->u.cons.car )
         Free_stringTSD( o->u.cons.car );
      if ( o->u.cons.cdr )
         Free_stringTSD( o->u.cons.cdr );
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
   /* .nil is also the empty LISP list: car, cdr, length, ... */
   if ( recv && recv->kind == OK_NIL && after == NULL && ot->cNilList
     && ( m = find_in( ot->cNilList->u.c->imethods, name ) ) != NULL )
   {
      *scope = ot->cNilList;
      return m;
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

static const streng **ot_pend_argv( tsd_t *TSD )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   return ot ? ot->pend_argv : NULL;
}

static paramboxptr ot_pend_box( tsd_t *TSD, int i )
{
   paramboxptr q = ( (oo_tsd *) TSD->oo_tsd )->pend_plist;

   while ( q && i-- > 0 )
      q = q->next;
   return q;
}

static paramboxptr make_plist( tsd_t *TSD, int argc, const streng **argv )
{
   paramboxptr first = NULL, last = NULL, p;
   int i;

   for ( i = 0; i < argc; i++ )
   {
      if ( TSD->par_stack )
      {
         /* deallocplink() keeps boxes on par_stack for reuse */
         p = TSD->par_stack;
         TSD->par_stack = p->next;
      }
      else
         p = (paramboxptr) MallocTSD( sizeof( parambox ) );
      p->next = NULL;
      p->dealloc = 1;
      p->value = argv[i] ? Str_dupTSD( argv[i] ) : NULL;
      p->stempool = NULL;
      p->stemname = NULL;
      if ( argv == ot_pend_argv( TSD ) )
      {
         /* "obj~m(st.)": pass the stem reference on to USE ARG s. */
         paramboxptr q = ot_pend_box( TSD, i );
         if ( q )
         {
            p->stempool = q->stempool;
            p->stemname = q->stemname;
         }
      }
      if ( last )
         last->next = p;
      else
         first = p;
      last = p;
   }
   return first;
}

typedef struct { const streng *selfstr; oo_frame *frame; } self_setup;

static void setup_self( tsd_t *TSD, void *data )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   self_setup *ss = (self_setup *) data;
   oo_scope *sc;

   setvalue( TSD, ot->s_self, Str_dupTSD( ss->selfstr ), -1 );
   sc = get_scope( TSD, ss->frame->self, ss->frame->scope );
   if ( sc->superproxy == NULL )
   {
      sc->superproxy = new_object( TSD, ot->cObject, OK_SUPER );
      sc->superproxy->u.sup.self = ss->frame->self;
      sc->superproxy->u.sup.start = ss->frame->scope;
   }
   setvalue( TSD, ot->s_super, hdup( TSD, sc->superproxy ), -1 );
}

static void setup_selfstr( tsd_t *TSD, void *data )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   self_setup *ss = (self_setup *) data;

   setvalue( TSD, ot->s_self, Str_dupTSD( ss->selfstr ), -1 );
}

/*
 * Runs a method or routine body as a new activation, see oo_run_level()
 * in interprt.c.
 */
static streng *run_body( tsd_t *TSD, nodeptr body, oo_frame *frame,
                         paramboxptr args, const streng *selfstr )
{
   self_setup ss;

   if ( body == NULL )
   {
      if ( args )
         deallocplink( TSD, args );
      return NULL;
   }
   ss.selfstr = selfstr;
   ss.frame = frame;
   if ( frame->self == NULL && selfstr )
      /* a method of String (::EXTENSION String) for a plain string */
      return oo_run_level( TSD, body, args, frame, setup_selfstr, &ss );
   return oo_run_level( TSD, body, args, frame, frame->self ? setup_self : NULL, &ss );
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
      int pin = pin_object( TSD, self );
      c.selfstr = selfstr;
      c.self = self;
      c.scope = scope;
      c.name = name;
      c.argc = argc;
      c.argv = argv;
      result = m->native( TSD, &c );
      unpin_object( TSD, pin );
      return result;
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

   frame = new_frame( TSD );
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
   {
      /* a STRING method that uses its own object as a string */
      static int nesting = 0;
      if ( nesting > 50 )
         return default_name( TSD, o );
      nesting++;
      r = send_msg( TSD, s, ot->s_string, 0, NULL );
      nesting--;
   }
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
   long cnt, i;

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
   for ( i = 0; i < cnt; i++ )   /* copies: the method may change the array */
      items[i] = arr->u.a.v[i] ? Str_dupTSD( arr->u.a.v[i] ) : NULL;
   r = send_msg( TSD, c->selfstr, name, (int) cnt, items );
   for ( i = 0; i < cnt; i++ )
      if ( items[i] )
         Free_stringTSD( (streng *) items[i] );
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

   if ( cd->resolved != 1 )
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

   if ( cls->u.c->resolved != 1 )
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

   if ( cls->u.c->resolved != 1 )
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

   if ( mix->u.c->mro == NULL )
      compute_mro( TSD, mix );
   if ( is_subclass( mix, cls ) )
      exiterror( ERR_EXECUTION_ERROR, 10, tmpstr_of( TSD, cls->u.c->name ) );
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
      boxes[i].stempool = NULL;
      boxes[i].stemname = NULL;
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

#define OO_MAX_INDEX 100000000L

void oo_array_put( tsd_t *TSD, oo_object *a, long idx, streng *v )
{
   long i;

   if ( idx > OO_MAX_INDEX )
   {
      char buf[32];
      sprintf( buf, "%ld", idx );
      exiterror( ERR_INVALID_ARGUMENT, 6, buf );
   }
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

/* ------------------------------------------------------------------ */
/* multi-dimensional arrays                                           */
/* ------------------------------------------------------------------ */

#define ARR_MULTI(arr) ( (arr)->kind == OK_ARRAY && (arr)->u.a.ndims > 1 )

/* for methods that only work on a list: error 98.8 for a 2+ dim array */
static oo_object *seq1_self( tsd_t *TSD, oo_call *c )
{
   oo_object *a = seq_self( TSD, c );

   if ( ARR_MULTI( a ) )
      exiterror( ERR_EXECUTION_ERROR, 8, tmpstr_of( TSD, c->name ) );
   return a;
}

static void arr_dims_error( tsd_t *TSD, oo_object *a, int given )
{
   char d[16], g[16];

   sprintf( d, "%d", ARR_MULTI( a ) ? a->u.a.ndims : 1 );
   sprintf( g, "%d", given );
   exiterror( ERR_EXECUTION_ERROR, 11, d, g );
}

/* the index of linear position i (0-based): a number, or an Array of numbers */
static streng *arr_index_value( tsd_t *TSD, oo_object *a, long i )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *ix;
   int k;
   long rest = i, pos[64];

   if ( !ARR_MULTI( a ) )
      return mkint( TSD, i + 1 );
   for ( k = a->u.a.ndims - 1; k >= 0; k-- )
   {
      pos[k] = rest % a->u.a.dims[k] + 1;
      rest /= a->u.a.dims[k];
   }
   ix = new_object( TSD, ot->cArray, OK_ARRAY );
   for ( k = 0; k < a->u.a.ndims; k++ )
      oo_array_put( TSD, ix, k + 1, mkint( TSD, pos[k] ) );
   return hdup( TSD, ix );
}

/* gives the array the dimensions nd (as many as it has) and moves the items */
static void arr_relayout( tsd_t *TSD, oo_object *a, const long *nd )
{
   long nsize = 1, i, k, rest, off, idx[64];
   streng **nv = NULL;
   int n = a->u.a.ndims;

   for ( k = 0; k < n; k++ )
   {
      nsize *= nd[k];
      if ( nsize > OO_MAX_INDEX )
      {
         char buf[32];
         sprintf( buf, "%ld", nsize );
         exiterror( ERR_INVALID_ARGUMENT, 6, buf );
      }
   }
   if ( nsize > 0 )
   {
      nv = (streng **) MallocTSD( nsize * sizeof( streng * ) );
      memset( nv, 0, nsize * sizeof( streng * ) );
   }
   for ( i = 0; i < a->u.a.size; i++ )
   {
      if ( a->u.a.v[i] == NULL )
         continue;
      for ( rest = i, k = n - 1; k >= 0; k-- )
      {
         idx[k] = rest % a->u.a.dims[k];
         rest /= a->u.a.dims[k];
      }
      for ( off = 0, k = 0; k < n; k++ )
         off = off * nd[k] + idx[k];
      nv[off] = a->u.a.v[i];
   }
   if ( a->u.a.v )
      FreeTSD( a->u.a.v );
   a->u.a.v = nv;
   a->u.a.size = a->u.a.cap = nsize;
   for ( k = 0; k < n; k++ )
      a->u.a.dims[k] = nd[k];
}

/* makes a an array of n dimensions with the given sizes; items are lost */
static void arr_make_multi( tsd_t *TSD, oo_object *a, int n, const long *sizes )
{
   long i;

   for ( i = 0; i < a->u.a.size; i++ )
      if ( a->u.a.v[i] )
         Free_stringTSD( a->u.a.v[i] );
   if ( a->u.a.v )
      FreeTSD( a->u.a.v );
   a->u.a.v = NULL;
   a->u.a.size = a->u.a.cap = 0;
   if ( a->u.a.dims )
      FreeTSD( a->u.a.dims );
   a->u.a.ndims = n;
   a->u.a.dims = (long *) MallocTSD( n * sizeof( long ) );
   memset( a->u.a.dims, 0, n * sizeof( long ) );
   arr_relayout( TSD, a, sizes );
}

/*
 * The linear position (0-based) of the indexes argv[from..] of c in a.
 * With grow, an index beyond a bound extends that dimension, and an empty
 * one-dimensional array takes the number of dimensions of the indexes.
 * Without grow, -1 means "no such position".
 */
static long arr_position( tsd_t *TSD, oo_call *c, oo_object *a, int from, int grow )
{
   int k = c->argc - from, j, beyond = 0;
   long idx[64], nd[64], off = 0;

   if ( k > 64 )
      arr_dims_error( TSD, a, k );
   if ( !ARR_MULTI( a ) )
   {
      if ( a->kind == OK_ARRAY && oo_array_items( TSD, a, NULL ) == 0 && k > 1 )
      {
         if ( !grow )
            return -1;
         for ( j = 0; j < k; j++ )
            nd[j] = 0;
         arr_make_multi( TSD, a, k, nd );
      }
      else
         arr_dims_error( TSD, a, k );
   }
   else if ( k != a->u.a.ndims )
      arr_dims_error( TSD, a, k );
   for ( j = 0; j < k; j++ )
   {
      idx[j] = whole_arg( TSD, c, from + j, 1 );
      nd[j] = a->u.a.dims[j];
      if ( idx[j] > nd[j] )
      {
         beyond = 1;
         nd[j] = idx[j];
      }
   }
   if ( beyond )
   {
      if ( !grow )
         return -1;
      arr_relayout( TSD, a, nd );
   }
   for ( j = 0; j < k; j++ )
      off = off * a->u.a.dims[j] + ( idx[j] - 1 );
   return off;
}

static streng *m_arr_dimension( tsd_t *TSD, oo_call *c )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *a = seq_self( TSD, c ), *r;
   int n = ARR_MULTI( a ) ? a->u.a.ndims : 1, k;

   if ( str_eqc( c->name, "DIMENSIONS" ) )
   {
      r = new_object( TSD, ot->cArray, OK_ARRAY );
      for ( k = 0; k < n; k++ )
         oo_array_put( TSD, r, k + 1, mkint( TSD, ARR_MULTI( a ) ? a->u.a.dims[k] : a->u.a.size ) );
      return hdup( TSD, r );
   }
   if ( opt_arg( c, 0 ) == NULL )
      return mkint( TSD, n );
   k = (int) whole_arg( TSD, c, 0, 1 );
   if ( k > n )
      return mkint( TSD, 0 );
   return mkint( TSD, ARR_MULTI( a ) ? a->u.a.dims[k - 1] : a->u.a.size );
}

static streng *nil_or( tsd_t *TSD, const streng *v )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   return v ? Str_dupTSD( v ) : hdup( TSD, ot->nil );
}

/* the single index of [] and friends */
static long index_arg( tsd_t *TSD, oo_call *c, int i )
{
   if ( c->argc > i + 1 )
      arr_dims_error( TSD, seq_self( TSD, c ), c->argc - i );
   return whole_arg( TSD, c, i, 1 );
}

static streng *m_arr_init( tsd_t *TSD, oo_call *c )
{
   oo_object *a = seq_self( TSD, c );
   long n;

   if ( a->kind == OK_ARRAY && c->argc > 1 )
   {
      /* .array~new(3, 4): a 3 x 4 array */
      long sizes[64];
      int k;
      if ( c->argc > 64 )
         arr_dims_error( TSD, a, c->argc );
      for ( k = 0; k < c->argc; k++ )
         sizes[k] = whole_arg( TSD, c, k, 0 );
      arr_make_multi( TSD, a, c->argc, sizes );
      return NULL;
   }
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

   if ( cls->u.c->resolved != 1 )
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
   long i;

   if ( ARR_MULTI( a ) || ( a->kind == OK_ARRAY && c->argc > 1 ) )
   {
      long p = arr_position( TSD, c, a, 0, 0 );
      return nil_or( TSD, p >= 0 ? a->u.a.v[p] : NULL );
   }
   i = index_arg( TSD, c, 0 );
   return nil_or( TSD, i <= a->u.a.size ? a->u.a.v[i - 1] : NULL );
}

static streng *m_arr_put( tsd_t *TSD, oo_call *c )
{
   oo_object *a = seq_self( TSD, c );
   const streng *v = need_arg( TSD, c, 0 );
   long i;

   if ( ARR_MULTI( a ) || c->argc > 2 )
   {
      long p = arr_position( TSD, c, a, 1, 1 );
      if ( a->u.a.v[p] )
         Free_stringTSD( a->u.a.v[p] );
      a->u.a.v[p] = Str_dupTSD( v );
      return NULL;
   }
   i = whole_arg( TSD, c, 1, 1 );
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
   oo_object *a = seq1_self( TSD, c );
   const streng *v = need_arg( TSD, c, 0 );

   oo_array_put( TSD, a, a->u.a.size + 1, Str_dupTSD( v ) );
   return mkint( TSD, a->u.a.size );
}

static streng *m_arr_firstlast( tsd_t *TSD, oo_call *c )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *a = seq1_self( TSD, c );
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
   oo_object *a = seq1_self( TSD, c );
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
   long i;

   if ( ARR_MULTI( a ) || ( a->kind == OK_ARRAY && c->argc > 1 ) )
   {
      long p = arr_position( TSD, c, a, 0, 0 );
      return mkbool( TSD, p >= 0 && a->u.a.v[p] != NULL );
   }
   i = streng_to_int( TSD, s, &err );
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
   oo_object *a = seq_self( TSD, c );
   long i = find_item( a, need_arg( TSD, c, 0 ) );

   return i ? arr_index_value( TSD, a, i - 1 ) : hdup( TSD, ot->nil );
}

static streng *m_arr_remove( tsd_t *TSD, oo_call *c )
{
   oo_object *a = str_eqc( c->name, "DELETE" ) ? seq1_self( TSD, c ) : seq_self( TSD, c );
   long i;
   streng *v;

   if ( ARR_MULTI( a ) )
   {
      /* remove(i, j): the item, no shift; the size stays */
      long p = arr_position( TSD, c, a, 0, 0 );
      v = NULL;
      if ( p >= 0 )
      {
         v = a->u.a.v[p];
         a->u.a.v[p] = NULL;
      }
      return v ? v : nil_or( TSD, NULL );
   }
   i = whole_arg( TSD, c, 0, 1 );
   v = arr_take( TSD, a, i, a->kind == OK_QUEUE || str_eqc( c->name, "DELETE" ) );

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
   oo_object *a = seq1_self( TSD, c );
   const streng *v = need_arg( TSD, c, 0 );
   const streng *where = opt_arg( c, 1 );
   long idx;

   /*
    * insert(item, index), as in ooRexx for Array and List: after index;
    * .nil means at the start; without index at the end
    */
   {
      oo_object *w = where ? obj_of( TSD, where ) : NULL;
      if ( where == NULL )
         idx = a->u.a.size + 1;
      else if ( w && w->kind == OK_NIL )
         idx = 1;
      else
         idx = whole_arg( TSD, c, 1, 1 ) + 1;
   }
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
         oo_array_put( TSD, arr, ++n, arr_index_value( TSD, a, i ) );
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
   oo_object *a = seq1_self( TSD, c );
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
   oo_object *a = seq1_self( TSD, c );
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

   if ( args && args->kind != OK_ARRAY && args->kind != OK_QUEUE )
      args = NULL;
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
   streng *n, *vk = oo_stringify( TSD, v );

   if ( vk )
      v = vk;
   if ( pos > b->u.s->len )
      pos = b->u.s->len;
   n = Str_makeTSD( b->u.s->len + v->len );
   memcpy( n->value, b->u.s->value, pos );
   memcpy( n->value + pos, v->value, v->len );
   memcpy( n->value + pos + v->len, b->u.s->value + pos, b->u.s->len - pos );
   n->len = b->u.s->len + v->len;
   Free_stringTSD( b->u.s );
   b->u.s = n;
   if ( vk )
      Free_stringTSD( vk );
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
         oo_array_put( TSD, idx, n, arr_index_value( TSD, o, i ) );
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
         if ( ARR_MULTI( o ) )
         {
            /* the size is the product of the dimensions; keep both */
            n->u.a.ndims = o->u.a.ndims;
            n->u.a.dims = (long *) MallocTSD( o->u.a.ndims * sizeof( long ) );
            memcpy( n->u.a.dims, o->u.a.dims, o->u.a.ndims * sizeof( long ) );
         }
         break;
      case OK_CONS:
         n->u.cons.car = o->u.cons.car ? Str_dupTSD( o->u.cons.car ) : NULL;
         n->u.cons.cdr = o->u.cons.cdr ? Str_dupTSD( o->u.cons.cdr ) : NULL;
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
      case OK_PYOBJ:
         n->u.pyref = pyb_dup( o->u.pyref );   /* the same Python object */
         break;
      case OK_INTERVAL:
         n->u.iv = o->u.iv;
         break;
      case OK_BLOCK:
         n->u.blk = o->u.blk;
         n->u.blk.nextblock = NULL;
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
   if ( o->aux )
      n->aux = Str_dupTSD( o->aux );
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
   add_methods( TSD, c, "DIMENSION DIMENSIONS", m_arr_dimension, 0 );
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

/* ------------------------------------------------------------------ */
/* Stem: a stem as an object; "RETURN x." in OO code returns one      */
/* ------------------------------------------------------------------ */

static int cmp_tails( const void *x, const void *y );

static oo_object *stem_self( tsd_t *TSD, oo_call *c )
{
   if ( c->self == NULL || c->self->kind != OK_STEM )
      err_nomethod( TSD, c->selfstr, c->name );
   return c->self;
}

static variableptr stem_box( tsd_t *TSD, oo_object *o )
{
   return oo_stem_box( TSD, o->u.pool, ( (oo_tsd *) TSD->oo_tsd )->s_stem );
}

variableptr oo_stem_object_box( tsd_t *TSD, const streng *s )
{
   oo_object *o;

   if ( TSD->oo_tsd == NULL || !OO_LOOKS_LIKE_HANDLE( s ) )
      return NULL;
   o = obj_of( TSD, s );
   return ( o && o->kind == OK_STEM ) ? stem_box( TSD, o ) : NULL;
}

/* the tail of stem[i, j, ...]: the string values joined with "." */
static streng *stem_tail( tsd_t *TSD, oo_call *c, int from )
{
   streng *t = Str_creTSD( "" ), *v;
   int i;

   for ( i = from; i < c->argc || i == from; i++ )
   {
      const streng *a = need_arg( TSD, c, i );
      if ( i > from )
         t = catc( TSD, t, "." );
      v = oo_stringify( TSD, a );
      t = cats( TSD, t, v ? v : a );
      if ( v )
         Free_stringTSD( v );
   }
   return t;
}

static streng *m_stem_init( tsd_t *TSD, oo_call *c )
{
   const streng *d = opt_arg( c, 0 );

   /* .stem~new(default): the default value of every element */
   if ( d )
   {
      oo_object *o = stem_self( TSD, c );
      stem_box( TSD, o );
      oo_pool_set( TSD, o->u.pool, ( (oo_tsd *) TSD->oo_tsd )->s_stem, Str_dupTSD( d ) );
   }
   return NULL;
}

static streng *m_stem_at( tsd_t *TSD, oo_call *c )
{
   variableptr b = stem_box( TSD, stem_self( TSD, c ) );
   streng *t = stem_tail( TSD, c, 0 );
   const streng *v = oo_stem_get( TSD, b, t );

   Free_stringTSD( t );
   if ( v == NULL )
      v = oo_stem_default( TSD, b );
   return nil_or( TSD, v );
}

static streng *m_stem_put( tsd_t *TSD, oo_call *c )
{
   oo_object *o = stem_self( TSD, c );
   streng *t = stem_tail( TSD, c, 1 );

   oo_stem_put( TSD, o->u.pool, ( (oo_tsd *) TSD->oo_tsd )->s_stem, t,
                Str_dupTSD( need_arg( TSD, c, 0 ) ) );
   Free_stringTSD( t );
   return NULL;
}

static streng *m_stem_remove( tsd_t *TSD, oo_call *c )
{
   oo_object *o = stem_self( TSD, c );
   streng *t = stem_tail( TSD, c, 0 ), *old;
   const streng *v = oo_stem_get( TSD, stem_box( TSD, o ), t );

   old = nil_or( TSD, v );
   if ( v )
      oo_stem_put( TSD, o->u.pool, ( (oo_tsd *) TSD->oo_tsd )->s_stem, t, NULL );
   Free_stringTSD( t );
   return old;
}

static streng *m_stem_hasindex( tsd_t *TSD, oo_call *c )
{
   streng *t = stem_tail( TSD, c, 0 );
   int has = oo_stem_get( TSD, stem_box( TSD, stem_self( TSD, c ) ), t ) != NULL;

   Free_stringTSD( t );
   return mkbool( TSD, has );
}

static streng *m_stem_items( tsd_t *TSD, oo_call *c )
{
   return mkint( TSD, oo_stem_each( TSD, stem_box( TSD, stem_self( TSD, c ) ), NULL, NULL ) );
}

static streng *m_stem_isempty( tsd_t *TSD, oo_call *c )
{
   return mkbool( TSD, oo_stem_each( TSD, stem_box( TSD, stem_self( TSD, c ) ), NULL, NULL ) == 0 );
}

typedef struct { streng **t, **v; long n, cap; } stemlist;

static void stemlist_cb( tsd_t *TSD, void *data, const streng *tail, const streng *value )
{
   stemlist *l = (stemlist *) data;

   if ( l->n >= l->cap )
   {
      long ncap = l->cap ? l->cap * 2 : 16;
      streng **nt = (streng **) MallocTSD( ncap * sizeof( streng * ) );
      if ( l->t )
      {
         memcpy( nt, l->t, l->n * sizeof( streng * ) );
         FreeTSD( l->t );
      }
      l->t = nt;
      l->cap = ncap;
   }
   l->t[l->n++] = Str_dupTSD( tail );
   (void) value;
}

/* the tails of the stem of o, sorted like DO OVER sorts them */
static void stem_sorted_tails( tsd_t *TSD, oo_object *o, stemlist *l )
{
   l->t = l->v = NULL;
   l->n = l->cap = 0;
   oo_stem_each( TSD, stem_box( TSD, o ), stemlist_cb, l );
   if ( l->n )
      qsort( l->t, l->n, sizeof( streng * ), cmp_tails );
}

static streng *m_stem_lists( tsd_t *TSD, oo_call *c )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *o = stem_self( TSD, c ), *idx, *items;
   variableptr b = stem_box( TSD, o );
   stemlist l;
   long i;
   int want = str_eqc( c->name, "ALLITEMS" ) ? 1 : str_eqc( c->name, "SUPPLIER" ) ? 2 : 0;

   stem_sorted_tails( TSD, o, &l );
   idx = new_object( TSD, ot->cArray, OK_ARRAY );
   items = new_object( TSD, ot->cArray, OK_ARRAY );
   for ( i = 0; i < l.n; i++ )
   {
      if ( want != 1 )
         oo_array_put( TSD, idx, i + 1, Str_dupTSD( l.t[i] ) );
      if ( want != 0 )
         oo_array_put( TSD, items, i + 1, Str_dupTSD( oo_stem_get( TSD, b, l.t[i] ) ) );
      Free_stringTSD( l.t[i] );
   }
   if ( l.t )
      FreeTSD( l.t );
   if ( want == 2 )
      return hdup( TSD, make_supplier( TSD, items, idx ) );
   return hdup( TSD, want ? items : idx );
}

static streng *m_stem_empty( tsd_t *TSD, oo_call *c )
{
   oo_object *o = stem_self( TSD, c );
   stemlist l;
   long i;

   stem_sorted_tails( TSD, o, &l );
   for ( i = 0; i < l.n; i++ )
   {
      oo_stem_put( TSD, o->u.pool, ( (oo_tsd *) TSD->oo_tsd )->s_stem, l.t[i], NULL );
      Free_stringTSD( l.t[i] );
   }
   if ( l.t )
      FreeTSD( l.t );
   return NULL;
}

static streng *m_stem_string( tsd_t *TSD, oo_call *c )
{
   oo_object *o = stem_self( TSD, c );
   const streng *d = oo_stem_default( TSD, stem_box( TSD, o ) );

   /* like the value of a stem variable: its default value, or its name */
   if ( d )
      return Str_dupTSD( d );
   return o->objname ? Str_dupTSD( o->objname ) : Str_creTSD( "a Stem" );
}

/* "RETURN x." in OO code: a new Stem object holding a copy of x. */
streng *oo_eval_stemobj( tsd_t *TSD, nodeptr thisptr )
{
   oo_tsd *ot = get_ot( TSD );
   oo_object *o = new_object( TSD, ot->cStem, OK_STEM );

   oo_stem_assign( TSD, o->u.pool, ot->s_stem,
                   oo_stem_box( TSD, TSD->currlevel->vars, thisptr->p[0]->name ) );
   o->objname = Str_dupTSD( thisptr->p[0]->name );
   return hdup( TSD, o );
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


/* ------------------------------------------------------------------ */
/* LISP-style lists: the Cons class; .nil is the empty list           */
/* ------------------------------------------------------------------ */

#define LISP_MAX_CELLS 100000000L   /* also the guard against cycles     */

static oo_object *cons_of( tsd_t *TSD, const streng *s )
{
   oo_object *o = s ? obj_of( TSD, s ) : NULL;
   return ( o && o->kind == OK_CONS ) ? o : NULL;
}

static int is_nil( tsd_t *TSD, const streng *s )
{
   oo_object *o = s ? obj_of( TSD, s ) : NULL;
   return o && o->kind == OK_NIL;
}

static streng *nil_handle( tsd_t *TSD )
{
   return hdup( TSD, ( (oo_tsd *) TSD->oo_tsd )->nil );
}

/* a new cell; takes over car and cdr (NULL means .nil) */
static streng *new_cons( tsd_t *TSD, streng *car, streng *cdr )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *o = new_object( TSD, ot->cCons, OK_CONS );

   Free_stringTSD( o->u.cons.car );
   Free_stringTSD( o->u.cons.cdr );
   o->u.cons.car = car ? car : nil_handle( TSD );
   o->u.cons.cdr = cdr ? cdr : nil_handle( TSD );
   return hdup( TSD, o );
}

/* the receiver as a list: a Cons, or NULL for .nil */
static oo_object *list_self( tsd_t *TSD, oo_call *c )
{
   if ( c->self && c->self->kind == OK_CONS )
      return c->self;
   if ( c->self && c->self->kind == OK_NIL )
      return NULL;
   err_nomethod( TSD, c->selfstr, c->name );
   return NULL;
}

/*
 * Walks the cells starting at o: n is the number of cells, *tail the
 * value after the last cell (.nil for a proper list). A circular list is
 * error 98.12.
 */
static long list_cells( tsd_t *TSD, oo_object *o, const streng **tail )
{
   oo_object *slow = o, *fast = o;
   long n = 0;

   *tail = NULL;
   while ( fast )
   {
      n++;
      *tail = fast->u.cons.cdr;
      fast = cons_of( TSD, fast->u.cons.cdr );
      if ( ( n & 1 ) == 0 )
      {
         slow = cons_of( TSD, slow->u.cons.cdr );
         if ( fast && fast == slow )
            exiterror( ERR_EXECUTION_ERROR, 12 );
      }
      if ( n > LISP_MAX_CELLS )
         exiterror( ERR_EXECUTION_ERROR, 12 );
   }
   return n;
}

/* the items of the list o as an array of borrowed strings, n of them */
static const streng **list_items( tsd_t *TSD, oo_object *o, long *n )
{
   const streng *tail, **v;
   long i;

   *n = o ? list_cells( TSD, o, &tail ) : 0;
   v = (const streng **) MallocTSD( ( *n + 1 ) * sizeof( streng * ) );
   for ( i = 0; i < *n; i++ )
   {
      v[i] = o->u.cons.car;
      o = cons_of( TSD, o->u.cons.cdr );
   }
   return v;
}

/* a new proper list of the strings v[0..n-1] (copied), ending in tail */
static streng *list_from( tsd_t *TSD, const streng **v, long n, const streng *tail )
{
   streng *acc = tail ? Str_dupTSD( tail ) : nil_handle( TSD );
   long i;

   for ( i = n - 1; i >= 0; i-- )
      acc = new_cons( TSD, v[i] ? Str_dupTSD( v[i] ) : NULL, acc );
   return acc;
}

/*
 * Calls the function named fname with the arguments argv: an internal
 * routine of the program, a ::ROUTINE, a built-in function, or else the
 * message fname sent to argv[0] with the other arguments.
 */
static streng *call_block( tsd_t *TSD, oo_object *b, int argc, const streng **argv );
static void collect_labels( tsd_t *TSD, nodeptr n, oo_labels *L, nodeptr owner );

/*
 * Calls the function f: a Block, or the name of an internal routine, a
 * ::routine, a built-in function or a method of the first argument.
 * need: the result is required (error 44 if there is none).
 */
static streng *fn_call( tsd_t *TSD, const streng *fname, int argc, const streng **argv,
                        int need )
{
   streng *u, *r;
   nodeptr entry;
   char bif[64];
   oo_object *bo = obj_of( TSD, fname );

   if ( bo && bo->kind == OK_BLOCK )
   {
      r = call_block( TSD, bo, argc, argv );
      if ( r == NULL && need )
         exiterror( ERR_NO_DATA_RETURNED, 1, "a block" );
      return r;
   }
   u = oo_stringify( TSD, fname );
   if ( u == NULL )
      u = Str_dupTSD( fname );
   mem_upper( u->value, u->len );
   {
      /*
       * An internal routine of the program that named it: the label is
       * looked up in the callers' levels, so mapcar('f', l) called through
       * the lisp library finds F in the main program. The routine runs as
       * if called from that level (its variables, EXPOSE, labels).
       */
      proclevel cur = TSD->currlevel, lev;
      entry = NULL;
      for ( lev = cur; lev && entry == NULL; lev = entry ? lev : lev->prev )
      {
         TSD->currlevel = lev;
         entry = getlabel( TSD, u );
         if ( entry && entry->u.trace_only )
            entry = NULL;
         if ( entry )
            break;
      }
      TSD->currlevel = cur;
      if ( entry )
      {
         proclevel keep = lev->next;
         set_reserved_value( TSD, POOL0_SIGL, NULL, TSD->currentnode->lineno, VFLAG_NUM );
         /* the new level copies lev; it must not take over lev's callees */
         lev->next = NULL;
         TSD->currlevel = lev;
         r = CallInternalFunction( TSD, entry->next, TSD->currentnode,
                                   make_plist( TSD, argc, argv ) );
         TSD->currlevel = cur;
         lev->next = keep;
         goto called;
      }
   }
   if ( 0 )
      ;
   else if ( oo_has_routine( TSD, u ) )
      r = oo_call_routine( TSD, u, make_plist( TSD, argc, argv ) );
   else
   {
      r = NOFUNC;
      if ( u->len < (int) sizeof( bif ) )
      {
         memcpy( bif, u->value, u->len );
         bif[u->len] = '\0';
         r = call_bif( TSD, bif, argc, argv, u );
      }
      if ( r == NOFUNC )
      {
         if ( argc < 1 )
            exiterror( ERR_ROUTINE_NOT_FOUND, 1, tmpstr_of( TSD, u ) );
         r = send_msg( TSD, argv[0], u, argc - 1, argv + 1 );
      }
   }
called:
   if ( r == NULL && need )
      exiterror( ERR_NO_DATA_RETURNED, 1, tmpstr_of( TSD, u ) );
   Free_stringTSD( u );
   return r;
}

static streng *lisp_call( tsd_t *TSD, const streng *fname, int argc, const streng **argv )
{
   return fn_call( TSD, fname, argc, argv, 1 );
}

/* the result of a FILTER function must be 0 or 1, as for IF (error 34) */
static int lisp_bool( tsd_t *TSD, const streng *r )
{
   streng *v = oo_stringify( TSD, r );
   const streng *t = v ? v : r;
   int b = -1, i = 0, j = t->len;

   while ( i < j && rx_isspace( t->value[i] ) )
      i++;
   while ( j > i && rx_isspace( t->value[j - 1] ) )
      j--;
   if ( j - i == 1 && ( t->value[i] == '0' || t->value[i] == '1' ) )
      b = t->value[i] == '1';
   if ( b < 0 )
      exiterror( ERR_UNLOGICAL_VALUE, 1, tmpstr_of( TSD, t ) );
   if ( v )
      Free_stringTSD( v );
   return b;
}

/* deep equality: equal atoms (strict ==) or lists of equal elements */
static int lisp_equal( tsd_t *TSD, const streng *a, const streng *b, int depth )
{
   oo_object *ca, *cb;
   long n = 0;

   if ( depth > 10000 )
      exiterror( ERR_EXECUTION_ERROR, 12 );
   for ( ;; )
   {
      ca = cons_of( TSD, a );
      cb = cons_of( TSD, b );
      if ( ca == NULL || cb == NULL )
         return ca == NULL && cb == NULL && str_eq( a, b );
      if ( ca == cb )
         return 1;
      if ( !lisp_equal( TSD, ca->u.cons.car, cb->u.cons.car, depth + 1 ) )
         return 0;
      a = ca->u.cons.cdr;
      b = cb->u.cons.cdr;
      if ( ++n > LISP_MAX_CELLS )
         exiterror( ERR_EXECUTION_ERROR, 12 );
   }
}

/* the printed form of an atom: quoted if it contains blanks or parens */
static streng *atom_text( tsd_t *TSD, streng *out, const streng *x )
{
   streng *v = oo_stringify( TSD, x );
   const streng *t = v ? v : x;
   int i, quote = ( t->len == 0 );

   for ( i = 0; i < t->len && !quote; i++ )
      if ( rx_isspace( t->value[i] ) || t->value[i] == '(' || t->value[i] == ')'
        || t->value[i] == '"' || t->value[i] == ';' )
         quote = 1;
   if ( quote )
   {
      out = catc( TSD, out, "\"" );
      for ( i = 0; i < t->len; i++ )
      {
         char ch[3] = { t->value[i], '\0', '\0' };
         if ( t->value[i] == '"' )
            ch[1] = '"';
         out = catc( TSD, out, ch );
      }
      out = catc( TSD, out, "\"" );
   }
   else
      out = cats( TSD, out, t );
   if ( v )
      Free_stringTSD( v );
   return out;
}

static streng *lisp_print( tsd_t *TSD, streng *out, const streng *x, int depth )
{
   oo_object *o = cons_of( TSD, x );
   long n = 0;

   if ( o == NULL )
      return is_nil( TSD, x ) ? catc( TSD, out, "NIL" ) : atom_text( TSD, out, x );
   if ( depth > 1000 )
      return catc( TSD, out, "(...)" );
   out = catc( TSD, out, "(" );
   for ( ;; )
   {
      out = lisp_print( TSD, out, o->u.cons.car, depth + 1 );
      x = o->u.cons.cdr;
      if ( is_nil( TSD, x ) )
         break;
      if ( ( o = cons_of( TSD, x ) ) == NULL )
      {
         out = catc( TSD, out, " . " );
         out = atom_text( TSD, out, x );
         break;
      }
      if ( ++n > 100000 )
      {
         out = catc( TSD, out, " ..." );
         break;
      }
      out = catc( TSD, out, " " );
   }
   return catc( TSD, out, ")" );
}

/* reads one s-expression at *p: atoms, "strings", (lists) and (a . b) */
static streng *lisp_read( tsd_t *TSD, const streng *src, int *p, int depth )
{
   const char *s = src->value;
   int len = src->len, start;
   streng *r;

   while ( *p < len && ( rx_isspace( s[*p] ) || s[*p] == ';' ) )
   {
      if ( s[*p] == ';' )       /* a comment up to the end of the line */
         while ( *p < len && s[*p] != '\n' )
            ( *p )++;
      else
         ( *p )++;
   }
   if ( *p >= len )
      exiterror( ERR_EXECUTION_ERROR, 13, "unexpected end" );
   if ( depth > 1000 )
      exiterror( ERR_EXECUTION_ERROR, 13, "lists nested too deeply" );
   if ( s[*p] == ')' )
      exiterror( ERR_EXECUTION_ERROR, 13, "unexpected \")\"" );
   if ( s[*p] == '(' )
   {
      streng **v = NULL, *tail = NULL;
      long n = 0, cap = 0, i;
      ( *p )++;
      for ( ;; )
      {
         while ( *p < len && rx_isspace( s[*p] ) )
            ( *p )++;
         if ( *p >= len )
            exiterror( ERR_EXECUTION_ERROR, 13, "missing \")\"" );
         if ( s[*p] == ')' )
         {
            ( *p )++;
            break;
         }
         if ( s[*p] == '.' && n > 0 && *p + 1 < len
           && ( rx_isspace( s[*p + 1] ) || s[*p + 1] == '(' ) )
         {
            ( *p )++;
            tail = lisp_read( TSD, src, p, depth + 1 );
            while ( *p < len && rx_isspace( s[*p] ) )
               ( *p )++;
            if ( *p >= len || s[*p] != ')' )
               exiterror( ERR_EXECUTION_ERROR, 13, "\")\" expected after a dotted pair" );
            ( *p )++;
            break;
         }
         if ( n >= cap )
         {
            streng **nv;
            cap = cap ? cap * 2 : 8;
            nv = (streng **) MallocTSD( cap * sizeof( streng * ) );
            if ( v )
            {
               memcpy( nv, v, n * sizeof( streng * ) );
               FreeTSD( v );
            }
            v = nv;
         }
         v[n++] = lisp_read( TSD, src, p, depth + 1 );
      }
      r = tail ? tail : nil_handle( TSD );
      for ( i = n - 1; i >= 0; i-- )
         r = new_cons( TSD, v[i], r );
      if ( v )
         FreeTSD( v );
      return r;
   }
   if ( s[*p] == '"' )
   {
      r = Str_creTSD( "" );
      for ( ( *p )++; ; ( *p )++ )
      {
         char ch[2] = { 0, 0 };
         if ( *p >= len )
            exiterror( ERR_EXECUTION_ERROR, 13, "missing closing quote" );
         if ( s[*p] == '"' )
         {
            if ( *p + 1 < len && s[*p + 1] == '"' )
               ( *p )++;
            else
            {
               ( *p )++;
               break;
            }
         }
         ch[0] = s[*p];
         r = Str_catTSD( r, Str_creTSD( ch ) );
      }
      return r;
   }
   start = *p;
   while ( *p < len && !rx_isspace( s[*p] ) && s[*p] != '(' && s[*p] != ')'
        && s[*p] != '"' && s[*p] != ';' )
      ( *p )++;
   r = Str_ncreTSD( s + start, *p - start );
   if ( r->len == 3 && rx_toupper( r->value[0] ) == 'N' && rx_toupper( r->value[1] ) == 'I'
     && rx_toupper( r->value[2] ) == 'L' )
   {
      Free_stringTSD( r );
      return nil_handle( TSD );
   }
   return r;
}

/* .cons~new(car, cdr) */
static streng *m_cons_init( tsd_t *TSD, oo_call *c )
{
   oo_object *o = c->self;
   const streng *a = opt_arg( c, 0 ), *d = opt_arg( c, 1 );

   if ( o == NULL || o->kind != OK_CONS )
      err_nomethod( TSD, c->selfstr, c->name );
   if ( o->u.cons.car )
      Free_stringTSD( o->u.cons.car );
   if ( o->u.cons.cdr )
      Free_stringTSD( o->u.cons.cdr );
   o->u.cons.car = a ? Str_dupTSD( a ) : nil_handle( TSD );
   o->u.cons.cdr = d ? Str_dupTSD( d ) : nil_handle( TSD );
   return NULL;
}

/* .cons~of(a, b, c): the list (a b c); no arguments: .nil */
static streng *m_cons_of( tsd_t *TSD, oo_call *c )
{
   return list_from( TSD, c->argv, c->argc, NULL );
}

/* .cons~read('(1 (2 3) "x y")') */
static streng *m_cons_read( tsd_t *TSD, oo_call *c )
{
   const streng *src = need_arg( TSD, c, 0 ), *t;
   streng *v = oo_stringify( TSD, src ), *r;
   int p = 0;


   t = v ? v : src;
   r = lisp_read( TSD, t, &p, 0 );
   /* only blanks and ; comments may follow */
   while ( p < t->len && ( rx_isspace( t->value[p] ) || t->value[p] == ';' ) )
   {
      if ( t->value[p] == ';' )
         while ( p < t->len && t->value[p] != '\n' )
            p++;
      else
         p++;
   }
   if ( p < t->len )
      exiterror( ERR_EXECUTION_ERROR, 13, "text after the expression" );
   if ( v )
      Free_stringTSD( v );
   return r;
}

/* .cons~apply(f, list): calls f with the items of list as arguments */
static streng *m_cons_apply( tsd_t *TSD, oo_call *c )
{
   const streng *f = need_arg( TSD, c, 0 ), **v;
   const streng *l = opt_arg( c, 1 );
   long n;
   streng *r;

   v = list_items( TSD, l ? cons_of( TSD, l ) : NULL, &n );
   r = lisp_call( TSD, f, (int) n, v );
   FreeTSD( (void *) v );
   return r;
}

static streng *m_cons_car( tsd_t *TSD, oo_call *c )
{
   oo_object *o = list_self( TSD, c );
   return o ? Str_dupTSD( o->u.cons.car ) : nil_handle( TSD );
}

static streng *m_cons_cdr( tsd_t *TSD, oo_call *c )
{
   oo_object *o = list_self( TSD, c );
   return o ? Str_dupTSD( o->u.cons.cdr ) : nil_handle( TSD );
}

static streng *m_cons_set( tsd_t *TSD, oo_call *c )
{
   oo_object *o = list_self( TSD, c );
   streng **slot;

   if ( o == NULL )
      err_nomethod( TSD, c->selfstr, c->name );
   slot = str_eqc( c->name, "SETCAR" ) ? &o->u.cons.car : &o->u.cons.cdr;
   Free_stringTSD( *slot );
   *slot = Str_dupTSD( need_arg( TSD, c, 0 ) );
   return NULL;
}

/* the cell n steps further, or NULL */
static oo_object *list_skip( tsd_t *TSD, oo_object *o, long n )
{
   while ( o && n-- > 0 )
      o = cons_of( TSD, o->u.cons.cdr );
   return o;
}

/* second, third, l[n] (1-based) */
static streng *m_cons_nth( tsd_t *TSD, oo_call *c )
{
   oo_object *o = list_self( TSD, c );
   long n;

   if ( str_eqc( c->name, "SECOND" ) )
      n = 2;
   else if ( str_eqc( c->name, "THIRD" ) )
      n = 3;
   else
      n = whole_arg( TSD, c, 0, 1 );
   o = list_skip( TSD, o, n - 1 );
   return o ? Str_dupTSD( o->u.cons.car ) : nil_handle( TSD );
}

/* nthcdr(n), 0-based as in LISP */
static streng *m_cons_nthcdr( tsd_t *TSD, oo_call *c )
{
   oo_object *o = list_self( TSD, c );
   long n = whole_arg( TSD, c, 0, 0 );

   if ( o == NULL )
      return nil_handle( TSD );
   if ( n == 0 )
      return Str_dupTSD( c->selfstr );
   o = list_skip( TSD, o, n - 1 );
   return o ? Str_dupTSD( o->u.cons.cdr ) : nil_handle( TSD );
}

static streng *m_cons_length( tsd_t *TSD, oo_call *c )
{
   oo_object *o = list_self( TSD, c );
   const streng *tail;

   return mkint( TSD, o ? list_cells( TSD, o, &tail ) : 0 );
}

static streng *m_cons_islist( tsd_t *TSD, oo_call *c )
{
   oo_object *o = list_self( TSD, c );
   const streng *tail;

   if ( o == NULL )
      return mkbool( TSD, 1 );
   list_cells( TSD, o, &tail );
   return mkbool( TSD, is_nil( TSD, tail ) );
}

/* last: the last cell (as LISP); lastItem: its car */
static streng *m_cons_last( tsd_t *TSD, oo_call *c )
{
   oo_object *o = list_self( TSD, c ), *n;
   const streng *tail;

   if ( o == NULL )
      return nil_handle( TSD );
   n = list_skip( TSD, o, list_cells( TSD, o, &tail ) - 1 );
   if ( str_eqc( c->name, "LASTITEM" ) )
      return Str_dupTSD( n->u.cons.car );
   return hdup( TSD, n );
}

static streng *m_cons_reverse( tsd_t *TSD, oo_call *c )
{
   oo_object *o = list_self( TSD, c );
   streng *acc = nil_handle( TSD );
   const streng *tail;
   long n = o ? list_cells( TSD, o, &tail ) : 0;

   for ( ; n > 0; n-- )
   {
      acc = new_cons( TSD, Str_dupTSD( o->u.cons.car ), acc );
      o = cons_of( TSD, o->u.cons.cdr );
   }
   return acc;
}

/* copy: a new list with the same items (the cells are copied) */
static streng *m_cons_copy( tsd_t *TSD, oo_call *c )
{
   oo_object *o = list_self( TSD, c );
   const streng **v, *tail = NULL;
   long n;
   streng *r;

   if ( o == NULL )
      return nil_handle( TSD );
   v = list_items( TSD, o, &n );
   list_cells( TSD, o, &tail );
   r = list_from( TSD, v, n, tail );
   FreeTSD( (void *) v );
   return r;
}

/* append(l2, l3, ...): a new list; the last argument is shared */
static streng *m_cons_append( tsd_t *TSD, oo_call *c )
{
   streng *r;
   int k;
   const streng **v;
   long n;

   if ( c->argc == 0 )
      return Str_dupTSD( c->self && c->self->kind == OK_CONS ? c->selfstr : NULL );
   r = Str_dupTSD( need_arg( TSD, c, c->argc - 1 ) );
   for ( k = c->argc - 2; k >= -1; k-- )
   {
      const streng *l = ( k >= 0 ) ? need_arg( TSD, c, k ) : c->selfstr;
      oo_object *o = cons_of( TSD, l );
      if ( o == NULL && !is_nil( TSD, l ) )
         exiterror( ERR_INVALID_ARGUMENT, 4, tmpstr_of( TSD, c->name ), k + 1,
                    tmpstr_of( TSD, l ) );
      v = list_items( TSD, o, &n );
      {
         streng *nr = list_from( TSD, v, n, r );
         Free_stringTSD( r );
         r = nr;
      }
      FreeTSD( (void *) v );
   }
   return r;
}

/* member(x): the rest of the list starting at x, or .nil */
static streng *m_cons_member( tsd_t *TSD, oo_call *c )
{
   oo_object *o = list_self( TSD, c );
   const streng *x = need_arg( TSD, c, 0 ), *tail;
   long n = o ? list_cells( TSD, o, &tail ) : 0;

   for ( ; n > 0; n-- )
   {
      if ( lisp_equal( TSD, o->u.cons.car, x, 0 ) )
         return hdup( TSD, o );
      o = cons_of( TSD, o->u.cons.cdr );
   }
   return nil_handle( TSD );
}

/* assoc(key): the first item that is a pair whose car equals key */
static streng *m_cons_assoc( tsd_t *TSD, oo_call *c )
{
   oo_object *o = list_self( TSD, c ), *p;
   const streng *k = need_arg( TSD, c, 0 ), *tail;
   long n = o ? list_cells( TSD, o, &tail ) : 0;

   for ( ; n > 0; n-- )
   {
      if ( ( p = cons_of( TSD, o->u.cons.car ) ) != NULL
        && lisp_equal( TSD, p->u.cons.car, k, 0 ) )
         return hdup( TSD, p );
      o = cons_of( TSD, o->u.cons.cdr );
   }
   return nil_handle( TSD );
}

/* map(f), filter(f), reduce(f [, initial]) */
static streng *m_cons_hof( tsd_t *TSD, oo_call *c )
{
   oo_object *o = list_self( TSD, c );
   const streng *f = need_arg( TSD, c, 0 ), **v, *args[2];
   long n, i, k = 0;
   streng *r, *acc;
   int mode = str_eqc( c->name, "MAP" ) ? 0 : str_eqc( c->name, "FILTER" ) ? 1 : 2;

   v = list_items( TSD, o, &n );
   if ( mode == 2 )
   {
      /* reduce: acc = f(acc, item), starting with initial or the first item */
      i = 0;
      if ( opt_arg( c, 1 ) )
         acc = Str_dupTSD( opt_arg( c, 1 ) );
      else if ( n > 0 )
         acc = Str_dupTSD( v[i++] );
      else
         acc = nil_handle( TSD );
      for ( ; i < n; i++ )
      {
         args[0] = acc;
         args[1] = v[i];
         r = lisp_call( TSD, f, 2, args );
         Free_stringTSD( acc );
         acc = r;
      }
      FreeTSD( (void *) v );
      return acc;
   }
   {
      const streng **out = (const streng **) MallocTSD( ( n + 1 ) * sizeof( streng * ) );
      streng **owned = (streng **) MallocTSD( ( n + 1 ) * sizeof( streng * ) );
      long nown = 0;
      for ( i = 0; i < n; i++ )
      {
         args[0] = v[i];
         r = lisp_call( TSD, f, 1, args );
         if ( mode == 0 )
            out[k++] = owned[nown++] = r;
         else
         {
            int b = lisp_bool( TSD, r );
            Free_stringTSD( r );
            if ( b )
               out[k++] = v[i];
         }
      }
      r = list_from( TSD, out, k, NULL );
      for ( i = 0; i < nown; i++ )
         Free_stringTSD( owned[i] );
      FreeTSD( (void *) out );
      FreeTSD( owned );
   }
   FreeTSD( (void *) v );
   return r;
}

static streng *m_cons_equal( tsd_t *TSD, oo_call *c )
{
   list_self( TSD, c );
   return mkbool( TSD, lisp_equal( TSD, c->selfstr, need_arg( TSD, c, 0 ), 0 ) );
}

static streng *m_cons_makearray( tsd_t *TSD, oo_call *c )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *arr = new_object( TSD, ot->cArray, OK_ARRAY );
   const streng **v;
   long n, i;

   v = list_items( TSD, list_self( TSD, c ), &n );
   for ( i = 0; i < n; i++ )
      oo_array_put( TSD, arr, i + 1, Str_dupTSD( v[i] ) );
   FreeTSD( (void *) v );
   return hdup( TSD, arr );
}

static streng *m_cons_string( tsd_t *TSD, oo_call *c )
{
   list_self( TSD, c );
   return lisp_print( TSD, Str_creTSD( "" ), c->selfstr, 0 );
}

static void init_lisp( tsd_t *TSD, oo_tsd *ot )
{
   oo_object *c, *nl;
   int k;

   ot->cCons = c = new_class( TSD, "Cons", NULL, ot->cObject, OK_CONS );
   add_methods( TSD, c, "OF", m_cons_of, MF_CLASS );
   add_methods( TSD, c, "READ", m_cons_read, MF_CLASS );
   add_methods( TSD, c, "APPLY", m_cons_apply, MF_CLASS );
   add_methods( TSD, c, "INIT", m_cons_init, 0 );

   /* .nil answers the list methods as the empty list; hidden class */
   ot->cNilList = nl = new_class( TSD, "NilList", NULL, ot->cObject, OK_PLAIN );
   for ( k = 0; k < 2; k++ )
   {
      oo_object *t = k ? nl : c;
      add_methods( TSD, t, "CAR FIRST", m_cons_car, 0 );
      add_methods( TSD, t, "CDR REST", m_cons_cdr, 0 );
      add_methods( TSD, t, "SECOND THIRD [] AT", m_cons_nth, 0 );
      add_methods( TSD, t, "NTHCDR", m_cons_nthcdr, 0 );
      add_methods( TSD, t, "LENGTH ITEMS", m_cons_length, 0 );
      add_methods( TSD, t, "ISLIST", m_cons_islist, 0 );
      add_methods( TSD, t, "LAST LASTITEM", m_cons_last, 0 );
      add_methods( TSD, t, "REVERSE", m_cons_reverse, 0 );
      add_methods( TSD, t, "APPEND", m_cons_append, 0 );
      add_methods( TSD, t, "MEMBER", m_cons_member, 0 );
      add_methods( TSD, t, "ASSOC", m_cons_assoc, 0 );
      add_methods( TSD, t, "MAP FILTER REDUCE", m_cons_hof, 0 );
      add_methods( TSD, t, "MAKEARRAY", m_cons_makearray, 0 );
   }
   add_methods( TSD, c, "SETCAR SETCDR", m_cons_set, 0 );
   add_methods( TSD, c, "COPY", m_cons_copy, 0 );
   add_methods( TSD, c, "EQUAL", m_cons_equal, 0 );
   add_methods( TSD, c, "STRING", m_cons_string, 0 );
   /* keep the hidden class out of the environment and .classes */
   if ( ot->nclasses > 0 && ot->classes[ot->nclasses - 1] == nl )
      ot->nclasses--;
}

/* ------------------------------------------------------------------ */
/* Interval, Set, Bag and SortedCollection                            */
/* ------------------------------------------------------------------ */

static long signed_arg( tsd_t *TSD, oo_call *c, int i )
{
   const streng *s = need_arg( TSD, c, i );
   int err = 0;
   long v = streng_to_int( TSD, s, &err );

   if ( err )
   {
      streng *d = describe( TSD, s );
      exiterror( ERR_INVALID_ARGUMENT, 7, tmpstr_of( TSD, c->name ), i + 1, tmpstr_of( TSD, d ) );
   }
   return v;
}

/* the number of values of an interval */
static long iv_count( oo_object *o )
{
   long from = o->u.iv.from, to = o->u.iv.to, by = o->u.iv.by;

   if ( ( by > 0 && from > to ) || ( by < 0 && from < to ) )
      return 0;
   return ( to - from ) / by + 1;
}

static oo_object *new_interval( tsd_t *TSD, oo_object *cls, long from, long to, long by )
{
   oo_object *o = new_object( TSD, cls, OK_INTERVAL );

   o->u.iv.from = from;
   o->u.iv.to = to;
   o->u.iv.by = by;
   return o;
}

/* .Interval~new(from, to [, by]) and "from"~to(to [, by]) */
static streng *m_iv_new( tsd_t *TSD, oo_call *c )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *cls = cls_self( TSD, c );
   long from = signed_arg( TSD, c, 0 ), to = signed_arg( TSD, c, 1 ), by = 1;

   if ( opt_arg( c, 2 ) )
      by = signed_arg( TSD, c, 2 );
   if ( by == 0 )
      exiterror( ERR_INVALID_ARGUMENT, 8, tmpstr_of( TSD, c->name ), 3 );
   return hdup( TSD, new_interval( TSD, is_subclass( cls, ot->cInterval ) ? cls : ot->cInterval,
                                   from, to, by ) );
}

static streng *m_string_to( tsd_t *TSD, oo_call *c )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   streng *kill;
   const streng *s = strval( TSD, c, &kill );
   int err = 0;
   long from = streng_to_int( TSD, s, &err ), to = signed_arg( TSD, c, 0 ), by = 1;

   if ( err )
   {
      streng *d = describe( TSD, s );
      exiterror( ERR_INVALID_ARGUMENT, 7, "TO", 0, tmpstr_of( TSD, d ) );
   }
   if ( kill )
      Free_stringTSD( kill );
   if ( opt_arg( c, 1 ) )
      by = signed_arg( TSD, c, 1 );
   if ( by == 0 )
      exiterror( ERR_INVALID_ARGUMENT, 8, "TO", 2 );
   return hdup( TSD, new_interval( TSD, ot->cInterval, from, to, by ) );
}

static oo_object *iv_self( tsd_t *TSD, oo_call *c )
{
   if ( c->self == NULL || c->self->kind != OK_INTERVAL )
      err_nomethod( TSD, c->selfstr, c->name );
   return c->self;
}

/* at(i): the i-th value, counted from 1 */
static streng *m_iv_at( tsd_t *TSD, oo_call *c )
{
   oo_object *o = iv_self( TSD, c );
   long i = whole_arg( TSD, c, 0, 1 );

   if ( i > iv_count( o ) )
      return nil_handle( TSD );
   return mkint( TSD, o->u.iv.from + ( i - 1 ) * o->u.iv.by );
}

static streng *m_iv_size( tsd_t *TSD, oo_call *c )
{
   return mkint( TSD, iv_count( iv_self( TSD, c ) ) );
}

static streng *m_iv_firstlast( tsd_t *TSD, oo_call *c )
{
   oo_object *o = iv_self( TSD, c );
   long n = iv_count( o );

   if ( n == 0 )
      return nil_handle( TSD );
   if ( str_eqc( c->name, "FIRST" ) )
      return mkint( TSD, o->u.iv.from );
   return mkint( TSD, o->u.iv.from + ( n - 1 ) * o->u.iv.by );
}

static streng *m_iv_parts( tsd_t *TSD, oo_call *c )
{
   oo_object *o = iv_self( TSD, c );

   if ( str_eqc( c->name, "FROM" ) )
      return mkint( TSD, o->u.iv.from );
   if ( str_eqc( c->name, "TO" ) )
      return mkint( TSD, o->u.iv.to );
   return mkint( TSD, o->u.iv.by );
}

static streng *m_iv_hasitem( tsd_t *TSD, oo_call *c )
{
   oo_object *o = iv_self( TSD, c );
   const streng *s = need_arg( TSD, c, 0 );
   int err = 0;
   long v = streng_to_int( TSD, s, &err ), n = iv_count( o );

   if ( err || n == 0 )
      return mkbool( TSD, 0 );
   v -= o->u.iv.from;
   return mkbool( TSD, v % o->u.iv.by == 0 && v / o->u.iv.by >= 0 && v / o->u.iv.by < n );
}

static streng *m_iv_makearray( tsd_t *TSD, oo_call *c )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *o = iv_self( TSD, c ), *a = new_object( TSD, ot->cArray, OK_ARRAY );
   long n = iv_count( o ), i;

   if ( n > 100000000L )
      exiterror( ERR_INVALID_ARGUMENT, 6, tmpstr_of( TSD, mkint( TSD, n ) ) );
   for ( i = 0; i < n; i++ )
      oo_array_put( TSD, a, i + 1, mkint( TSD, o->u.iv.from + i * o->u.iv.by ) );
   return hdup( TSD, a );
}

static streng *m_iv_reverse( tsd_t *TSD, oo_call *c )
{
   oo_object *o = iv_self( TSD, c );
   long n = iv_count( o ), last = o->u.iv.from + ( n - 1 ) * o->u.iv.by;

   if ( n == 0 )
      return hdup( TSD, new_interval( TSD, o->cls, o->u.iv.to, o->u.iv.from, -o->u.iv.by ) );
   return hdup( TSD, new_interval( TSD, o->cls, last, o->u.iv.from, -o->u.iv.by ) );
}

static streng *m_iv_string( tsd_t *TSD, oo_call *c )
{
   oo_object *o = iv_self( TSD, c );
   char buf[100];

   if ( o->u.iv.by == 1 )
      sprintf( buf, "(%ld to %ld)", o->u.iv.from, o->u.iv.to );
   else
      sprintf( buf, "(%ld to %ld by %ld)", o->u.iv.from, o->u.iv.to, o->u.iv.by );
   return Str_creTSD( buf );
}

/*
 * Set: every item once. Bag: items with a count. Both are kept as a
 * Table (identity for objects, value for strings): the key is the item,
 * the value the item (Set) or its count (Bag).
 */
static int is_bag( tsd_t *TSD, oo_object *o )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   return o && o->kind == OK_TABLE && ot->cBag && is_subclass( o->cls, ot->cBag );
}

static int is_set( tsd_t *TSD, oo_object *o )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   return o && o->kind == OK_TABLE && ot->cSet && is_subclass( o->cls, ot->cSet );
}

static oo_object *setbag_self( tsd_t *TSD, oo_call *c )
{
   if ( !is_set( TSD, c->self ) && !is_bag( TSD, c->self ) )
      err_nomethod( TSD, c->selfstr, c->name );
   return c->self;
}

static long bag_count( const streng *v )
{
   long n = 0;
   int i;
   for ( i = 0; i < v->len; i++ )
      n = n * 10 + ( v->value[i] - '0' );
   return n;
}

/* adds one item (copied) to a Set or Bag */
static void setbag_add( tsd_t *TSD, oo_object *o, const streng *item, long times )
{
   unsigned hv;
   oo_entry *e = h_find( o->u.h, item, &hv );

   if ( !is_bag( TSD, o ) )
   {
      if ( e == NULL )
         h_put( TSD, o->u.h, item, Str_dupTSD( item ) );
      return;
   }
   h_put( TSD, o->u.h, item, mkint( TSD, ( e ? bag_count( e->value ) : 0 ) + times ) );
}

/* put(item), add(item), append(item); Bag: add(item [, times]) */
static streng *m_sb_add( tsd_t *TSD, oo_call *c )
{
   oo_object *o = setbag_self( TSD, c );
   long times = 1;

   if ( is_bag( TSD, o ) && opt_arg( c, 1 ) )
      times = whole_arg( TSD, c, 1, 1 );
   setbag_add( TSD, o, need_arg( TSD, c, 0 ), times );
   return NULL;
}

/* remove(item): removes it (one occurrence from a Bag); returns it or .nil */
static streng *m_sb_remove( tsd_t *TSD, oo_call *c )
{
   oo_object *o = setbag_self( TSD, c );
   const streng *item = need_arg( TSD, c, 0 );
   unsigned hv;
   oo_entry *e = h_find( o->u.h, item, &hv );
   streng *old;

   if ( e == NULL )
      return nil_handle( TSD );
   if ( is_bag( TSD, o ) && bag_count( e->value ) > 1 )
   {
      h_put( TSD, o->u.h, item, mkint( TSD, bag_count( e->value ) - 1 ) );
      return Str_dupTSD( item );
   }
   old = h_remove( TSD, o->u.h, item );
   if ( old )
      Free_stringTSD( old );
   return Str_dupTSD( item );
}

static streng *m_sb_has( tsd_t *TSD, oo_call *c )
{
   oo_object *o = setbag_self( TSD, c );
   unsigned hv;
   return mkbool( TSD, h_find( o->u.h, need_arg( TSD, c, 0 ), &hv ) != NULL );
}

/* occurrencesOf(item): how often a Bag (or Set) holds it */
static streng *m_sb_occurrences( tsd_t *TSD, oo_call *c )
{
   oo_object *o = setbag_self( TSD, c );
   unsigned hv;
   oo_entry *e = h_find( o->u.h, need_arg( TSD, c, 0 ), &hv );

   if ( e == NULL )
      return mkint( TSD, 0 );
   return mkint( TSD, is_bag( TSD, o ) ? bag_count( e->value ) : 1 );
}

static long setbag_items( tsd_t *TSD, oo_object *o )
{
   oo_entry *e;
   long n = 0;

   if ( !is_bag( TSD, o ) )
      return o->u.h->count;
   for ( e = o->u.h->first; e; e = e->onext )
      n += bag_count( e->value );
   return n;
}

static streng *m_sb_items( tsd_t *TSD, oo_call *c )
{
   return mkint( TSD, setbag_items( TSD, setbag_self( TSD, c ) ) );
}

/* makeArray: the items in the order they were added (a Bag repeats them) */
static streng *m_sb_makearray( tsd_t *TSD, oo_call *c )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *o = setbag_self( TSD, c ), *a = new_object( TSD, ot->cArray, OK_ARRAY );
   oo_entry *e;
   long n = 0, k, times;

   for ( e = o->u.h->first; e; e = e->onext )
   {
      times = is_bag( TSD, o ) ? bag_count( e->value ) : 1;
      for ( k = 0; k < times; k++ )
         oo_array_put( TSD, a, ++n, Str_dupTSD( e->key ) );
   }
   return hdup( TSD, a );
}

static oo_object *new_setbag( tsd_t *TSD, oo_object *cls )
{
   oo_object *r = new_object( TSD, cls, OK_TABLE );
   r->u.h = (oo_hash *) MallocTSD( sizeof( oo_hash ) );
   memset( r->u.h, 0, sizeof( oo_hash ) );
   return r;
}

/* the items of any collection argument (Set, Bag, Array, ...) as an Array */
static oo_object *items_of( tsd_t *TSD, const streng *arg )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *o = obj_of( TSD, arg );
   streng *h;

   if ( o && ( o->kind == OK_ARRAY || o->kind == OK_QUEUE ) && !ARR_MULTI( o ) )
      return o;
   h = send_msg( TSD, arg, ot->s_makearray, 0, NULL );
   o = obj_of( TSD, h );
   if ( h )
      Free_stringTSD( h );
   if ( o == NULL || ( o->kind != OK_ARRAY && o->kind != OK_QUEUE ) )
      exiterror( ERR_EXECUTION_ERROR, 5, tmpstr_of( TSD, describe( TSD, arg ) ) );
   return o;
}

/* union(c), intersection(c), difference(c): new Sets (Bags) */
static streng *m_sb_setop( tsd_t *TSD, oo_call *c )
{
   oo_object *o = setbag_self( TSD, c ), *other = items_of( TSD, need_arg( TSD, c, 0 ) );
   oo_object *r = new_setbag( TSD, o->cls ), *oset = new_setbag( TSD, o->cls );
   int op = str_eqc( c->name, "UNION" ) ? 0 : ( str_eqc( c->name, "INTERSECTION" ) ? 1 : 2 );
   oo_entry *e;
   unsigned hv;
   long i, k, times;

   /* the other collection as a Set (Bag) of this kind, to look items up */
   for ( i = 0; i < other->u.a.size; i++ )
      if ( other->u.a.v[i] )
         setbag_add( TSD, oset, other->u.a.v[i], 1 );
   for ( e = o->u.h->first; e; e = e->onext )
   {
      oo_entry *f = h_find( oset->u.h, e->key, &hv );
      times = is_bag( TSD, o ) ? bag_count( e->value ) : 1;
      if ( op == 1 )
      {
         if ( f == NULL )
            continue;
         if ( is_bag( TSD, o ) && bag_count( f->value ) < times )
            times = bag_count( f->value );
      }
      else if ( op == 2 && f )
      {
         if ( !is_bag( TSD, o ) )
            continue;
         times -= bag_count( f->value );
         if ( times <= 0 )
            continue;
      }
      for ( k = 0; k < times; k++ )
         setbag_add( TSD, r, e->key, 1 );
   }
   if ( op == 0 )
      for ( e = oset->u.h->first; e; e = e->onext )
      {
         times = is_bag( TSD, o ) ? bag_count( e->value ) : 1;
         for ( k = 0; k < times; k++ )
            setbag_add( TSD, r, e->key, 1 );
      }
   free_object( TSD, oset );
   return hdup( TSD, r );
}

static streng *m_sb_subset( tsd_t *TSD, oo_call *c )
{
   oo_object *o = setbag_self( TSD, c ), *other = items_of( TSD, need_arg( TSD, c, 0 ) );
   oo_object *oset = new_setbag( TSD, o->cls );
   oo_entry *e;
   unsigned hv;
   long i;
   int ok = 1;

   for ( i = 0; i < other->u.a.size; i++ )
      if ( other->u.a.v[i] )
         setbag_add( TSD, oset, other->u.a.v[i], 1 );
   for ( e = o->u.h->first; e && ok; e = e->onext )
   {
      oo_entry *f = h_find( oset->u.h, e->key, &hv );
      if ( f == NULL || ( is_bag( TSD, o ) && bag_count( f->value ) < bag_count( e->value ) ) )
         ok = 0;
   }
   free_object( TSD, oset );
   return mkbool( TSD, ok );
}

/* .Set~of(items...), .Bag~of(items...) */
static streng *m_sb_of( tsd_t *TSD, oo_call *c )
{
   oo_object *r = new_setbag( TSD, cls_self( TSD, c ) );
   int i;

   for ( i = 0; i < c->argc; i++ )
      if ( c->argv[i] )
         setbag_add( TSD, r, c->argv[i], 1 );
   return hdup( TSD, r );
}

static streng *m_sb_string( tsd_t *TSD, oo_call *c )
{
   oo_object *o = setbag_self( TSD, c );
   streng *r = Str_creTSD( is_bag( TSD, o ) ? "a Bag (" : "a Set (" );
   oo_entry *e;
   long k, times;
   int first = 1;

   for ( e = o->u.h->first; e; e = e->onext )
   {
      times = is_bag( TSD, o ) ? bag_count( e->value ) : 1;
      for ( k = 0; k < times; k++ )
      {
         streng *t = oo_stringify( TSD, e->key );
         if ( !first )
            r = catc( TSD, r, " " );
         first = 0;
         r = cats( TSD, r, t ? t : e->key );
         if ( t )
            Free_stringTSD( t );
      }
   }
   return catc( TSD, r, ")" );
}

/*
 * SortedCollection: an Array kept in order. The order is that of `sort`
 * (string values; whole numbers by value), or given by a block
 * f(a, b) that is 1 if a may come before b.
 */
static int sorted_before( tsd_t *TSD, oo_object *o, const streng *a, const streng *b )
{
   if ( o->aux )
   {
      const streng *argv[2];
      streng *r;
      int v;
      argv[0] = a;
      argv[1] = b;
      r = fn_call( TSD, o->aux, 2, argv, 1 );
      v = lisp_bool( TSD, r );
      Free_stringTSD( r );
      return v;
   }
   return cmp_tails( &a, &b ) <= 0;
}

static oo_object *sorted_self( tsd_t *TSD, oo_call *c )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   if ( c->self == NULL || c->self->kind != OK_ARRAY || !is_subclass( c->self->cls, ot->cSorted ) )
      err_nomethod( TSD, c->selfstr, c->name );
   return c->self;
}

/* inserts item (copied) after all items that may come before it */
static void sorted_add( tsd_t *TSD, oo_object *o, const streng *item )
{
   long n = o->u.a.size, lo = 0, hi = n, mid;
   streng *t;

   while ( lo < hi )
   {
      mid = ( lo + hi ) / 2;
      if ( sorted_before( TSD, o, o->u.a.v[mid], item ) )
         lo = mid + 1;
      else
         hi = mid;
   }
   oo_array_put( TSD, o, n + 1, Str_dupTSD( item ) );
   t = o->u.a.v[n];
   memmove( o->u.a.v + lo + 1, o->u.a.v + lo, ( n - lo ) * sizeof( streng * ) );
   o->u.a.v[lo] = t;
}

/* .SortedCollection~new([f]) */
static streng *m_sorted_init( tsd_t *TSD, oo_call *c )
{
   oo_object *o = sorted_self( TSD, c );
   if ( opt_arg( c, 0 ) )
      o->aux = Str_dupTSD( opt_arg( c, 0 ) );
   return NULL;
}

static streng *m_sorted_add( tsd_t *TSD, oo_call *c )
{
   oo_object *o = sorted_self( TSD, c );
   int i;
   for ( i = 0; i < c->argc; i++ )
      if ( c->argv[i] )
         sorted_add( TSD, o, c->argv[i] );
   return NULL;
}

static streng *m_sorted_of( tsd_t *TSD, oo_call *c )
{
   oo_object *o = new_object( TSD, cls_self( TSD, c ), OK_ARRAY );
   int i;
   for ( i = 0; i < c->argc; i++ )
      if ( c->argv[i] )
         sorted_add( TSD, o, c->argv[i] );
   return hdup( TSD, o );
}

static streng *m_sorted_nochange( tsd_t *TSD, oo_call *c )
{
   exiterror( ERR_EXECUTION_ERROR, 17, tmpstr_of( TSD, c->name ) );
   return NULL;
}

static void add_coll_methods( tsd_t *TSD, oo_object *c );
static void init_more_collections( tsd_t *TSD, oo_tsd *ot )
{
   oo_object *c;
   int k;

   ot->cInterval = c = new_class( TSD, "Interval", NULL, ot->cObject, OK_INTERVAL );
   add_methods( TSD, c, "NEW", m_iv_new, MF_CLASS );
   add_methods( TSD, c, "[] AT", m_iv_at, 0 );
   add_methods( TSD, c, "SIZE ITEMS", m_iv_size, 0 );
   add_methods( TSD, c, "FIRST LAST", m_iv_firstlast, 0 );
   add_methods( TSD, c, "FROM TO BY", m_iv_parts, 0 );
   add_methods( TSD, c, "HASITEM INCLUDES", m_iv_hasitem, 0 );
   add_methods( TSD, c, "MAKEARRAY ALLITEMS", m_iv_makearray, 0 );
   add_methods( TSD, c, "REVERSE", m_iv_reverse, 0 );
   add_methods( TSD, c, "STRING", m_iv_string, 0 );
   add_methods( TSD, ot->cString, "TO", m_string_to, 0 );
   add_coll_methods( TSD, ot->cInterval );

   for ( k = 0; k < 2; k++ )
   {
      c = new_class( TSD, k ? "Bag" : "Set", NULL, ot->cObject, OK_TABLE );
      if ( k )
         ot->cBag = c;
      else
         ot->cSet = c;
      add_methods( TSD, c, "INIT", m_nothing, 0 );
      add_methods( TSD, c, "OF", m_sb_of, MF_CLASS );
      add_methods( TSD, c, "PUT ADD APPEND []=", m_sb_add, 0 );
      add_methods( TSD, c, "REMOVE REMOVEITEM", m_sb_remove, 0 );
      add_methods( TSD, c, "HASITEM HASINDEX INCLUDES", m_sb_has, 0 );
      add_methods( TSD, c, "OCCURRENCESOF", m_sb_occurrences, 0 );
      add_methods( TSD, c, "ITEMS SIZE", m_sb_items, 0 );
      add_methods( TSD, c, "ISEMPTY", m_h_isempty, 0 );
      add_methods( TSD, c, "EMPTY", m_h_empty, 0 );
      add_methods( TSD, c, "MAKEARRAY ALLITEMS ALLINDEXES", m_sb_makearray, 0 );
      add_methods( TSD, c, "UNION INTERSECTION DIFFERENCE", m_sb_setop, 0 );
      add_methods( TSD, c, "ISSUBSETOF", m_sb_subset, 0 );
      add_methods( TSD, c, "STRING", m_sb_string, 0 );
      add_coll_methods( TSD, c );
   }

   ot->cSorted = c = new_class( TSD, "SortedCollection", NULL, ot->cArray, OK_ARRAY );
   add_methods( TSD, c, "INIT", m_sorted_init, 0 );
   add_methods( TSD, c, "OF", m_sorted_of, MF_CLASS );
   add_methods( TSD, c, "ADD APPEND", m_sorted_add, 0 );
   add_methods( TSD, c, "PUT []= INSERT SORT SORTWITH FILL", m_sorted_nochange, 0 );
}

/* ------------------------------------------------------------------ */
/* Blocks: { clauses } and the collection methods that use them        */
/* ------------------------------------------------------------------ */

/*
 * A block is an unnamed internal routine written where it is used:
 *
 *    double = { use arg x; return x * 2 }
 *    big = numbers~select({ :x | x > 100 })
 *
 * Like an internal routine without PROCEDURE it sees the variables of
 * the code that created it; it may begin with PROCEDURE [EXPOSE ...].
 * "{ :a :b | expression }" is short for "{ use arg a, b; return expression }",
 * "{ | expression }" for "{ return expression }".
 * The source is parsed once per { } in the program.
 */

/* the first statement of a block, skipping labels */
static nodeptr first_statement( nodeptr n )
{
   while ( n && ( n->type == X_LABEL || n->type == X_NULL ) )
      n = n->next;
   return n;
}

static int blank_char( char c )
{
   return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

/* "{ :x :y | expr }" as "use arg x, y; return expr", "{ | expr }" as
   "return expr"; NULL for other blocks */
static streng *block_shorthand( tsd_t *TSD, const streng *src )
{
   const char *p = src->value, *e = p + src->len, *bar;
   streng *r;

   while ( p < e && blank_char( *p ) )
      p++;
   if ( p < e && *p == '|' )
   {
      /* "{ | expression }": no parameters */
      streng *t = Str_ncreTSD( p + 1, (int) ( e - p - 1 ) );
      r = Str_creTSD( "return " );
      r = cats( TSD, r, t );
      Free_stringTSD( t );
      return r;
   }
   if ( p >= e || *p != ':' )
      return NULL;
   for ( bar = p; bar < e && *bar != '|'; bar++ )
      ;
   if ( bar >= e )
      exiterror( ERR_TRANSLATION_ERROR, 9 );
   r = Str_makeTSD( src->len + 32 );
   r->len = 0;
   r = catc( TSD, r, "use arg " );
   while ( p < bar )
   {
      const char *n;
      while ( p < bar && ( blank_char( *p ) ) )
         p++;
      if ( p >= bar )
         break;
      if ( *p != ':' )
         exiterror( ERR_TRANSLATION_ERROR, 9 );
      p++;
      for ( n = p; p < bar && !blank_char( *p ) && *p != ':'; p++ )
         ;
      if ( p == n )
         exiterror( ERR_TRANSLATION_ERROR, 9 );
      if ( r->len > 8 )
         r = catc( TSD, r, ", " );
      {
         streng *t = Str_ncreTSD( n, (int) ( p - n ) );
         r = cats( TSD, r, t );
         Free_stringTSD( t );
      }
   }
   r = catc( TSD, r, "; return " );
   {
      streng *t = Str_ncreTSD( bar + 1, (int) ( e - bar - 1 ) );
      r = cats( TSD, r, t );
      Free_stringTSD( t );
   }
   return r;
}

/* line numbers of a block's statements as lines of the program */
static void block_lines( nodeptr n, int offset, int depth )
{
   int i;

   if ( depth > 1000 )
      return;
   for ( ; n; n = n->next )
   {
      if ( n->lineno > 0 )
         n->lineno += offset;
      for ( i = 0; i < 4; i++ )
         if ( n->p[i] && !( n->type == X_LABEL && i == 3 ) )
            block_lines( n->p[i], offset, depth + 1 );
   }
}

static oo_blockcode *block_code( tsd_t *TSD, nodeptr node )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_blockcode *b;
   streng *text, *body;
   internal_parser_type parsing;

   for ( b = ot->blocks; b; b = b->next )
      if ( b->node == node )
         return b;
   body = block_shorthand( TSD, node->name );
   /* the marker switches the OO keywords on for the block's own parse */
   text = Str_creTSD( "/* OOREGINA */ " );
   text = cats( TSD, text, body ? body : node->name );
   if ( body )
      Free_stringTSD( body );
   fetch_string( TSD, text, &parsing );
   if ( parsing.result != 0 )
   {
      Free_stringTSD( text );
      TSD->currentnode = node;
      exiterror( ERR_YACC_SYNTAX, 1, parsing.tline );
   }
   if ( node->lineno > 1 )
      block_lines( parsing.root, node->lineno - 1, 0 );
   b = (oo_blockcode *) MallocTSD( sizeof( oo_blockcode ) );
   memset( b, 0, sizeof( oo_blockcode ) );
   b->node = node;
   b->src = text;
   b->tree = parsing;
   b->body = parsing.root;
   collect_labels( TSD, parsing.root, &b->labels, NULL );
   {
      /* the parameters: the variables of a first USE ARG are local */
      nodeptr first = first_statement( b->body ), n;
      if ( first && first->type == X_PROC )
         b->procedure = 1;
      else if ( first && first->type == X_OO_USEARG )
      {
         for ( n = first->p[0]; n; n = n->p[0] )
            b->nparams++;
         b->params = (streng **) MallocTSD( ( b->nparams + 1 ) * sizeof( streng * ) );
         b->nparams = 0;
         for ( n = first->p[0]; n; n = n->p[0] )
            if ( n->name )
               b->params[b->nparams++] = n->name;
      }
   }
   b->next = ot->blocks;
   ot->blocks = b;
   return b;
}

static oo_object *bind_block( tsd_t *TSD, oo_blockcode *code )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *o = new_object( TSD, ot->cBlock, OK_BLOCK );
   proclevel home = TSD->currlevel;

   o->u.blk.code = code;
   o->u.blk.home = home;
   o->u.blk.serial = home->oo_serial;
   o->u.blk.homeframe = (oo_frame *) home->oo_ctx;
   o->u.blk.nextblock = (oo_object *) home->oo_blocks;
   home->oo_blocks = o;
   return o;
}

/* evaluates "{ ... }": a new Block bound to the current activation */
streng *oo_eval_block( tsd_t *TSD, nodeptr thisptr )
{
   get_ot( TSD );
   TSD->oo_active = 1;
   return hdup( TSD, bind_block( TSD, block_code( TSD, thisptr ) ) );
}

/*
 * The activation of level ends (see removelevel). Its blocks keep its
 * variables: a copy of them, shared by all its blocks. If the level used
 * its caller's variables (an internal routine without PROCEDURE), the
 * blocks move to the caller.
 */
void oo_level_ends( tsd_t *TSD, proclevel level )
{
   oo_object *b, *next;
   var_hashtable *copy = NULL;

   for ( b = (oo_object *) level->oo_blocks; b; b = next )
   {
      next = b->u.blk.nextblock;
      if ( b->u.blk.home != level )
         continue;
      if ( level->varflag == 0 && level->prev )
      {
         b->u.blk.home = level->prev;
         b->u.blk.serial = level->prev->oo_serial;
         b->u.blk.nextblock = (oo_object *) level->prev->oo_blocks;
         level->prev->oo_blocks = b;
         continue;
      }
      if ( copy == NULL )
      {
         extern void oo_pool_copy( const tsd_t *TSD, var_hashtable *from, var_hashtable *to );
         copy = create_new_varpool( TSD, 0 );
         oo_pool_copy( TSD, level->vars, copy );
      }
      b->u.blk.home = NULL;
      b->u.blk.homeframe = NULL;
      b->u.blk.kept = copy;
      b->u.blk.nextblock = NULL;
   }
   level->oo_blocks = NULL;
}

/*
 * The variables of a block: those of its creator, except its parameters
 * (the names of a first USE ARG, or of "{ :x | ... }") and RESULT, RC and
 * SIGL, which are its own. New variables are local too.
 */
static int block_keeps_local( const streng *name, void *data )
{
   oo_blockcode *code = (oo_blockcode *) data;
   int i;

   if ( str_eqc( name, "RESULT" ) || str_eqc( name, "RC" ) || str_eqc( name, "SIGL" ) )
      return 1;
   for ( i = 0; i < code->nparams; i++ )
      if ( str_eq( name, code->params[i] ) )
         return 1;
   return 0;
}

static void block_setup( tsd_t *TSD, void *data )
{
   extern void oo_expose_except( tsd_t *TSD, int ( *keep )( const streng *, void * ),
                                 void *data );
   oo_expose_except( TSD, block_keeps_local, data );
}

static streng *call_block( tsd_t *TSD, oo_object *b, int argc, const streng **argv )
{
   proclevel lev;
   oo_frame *frame;
   oo_blockcode *code = b->u.blk.code;
   var_hashtable *share = NULL;
   streng *r;
   int alive = 0;

   for ( lev = TSD->currlevel; lev && b->u.blk.home; lev = lev->prev )
      if ( lev == b->u.blk.home )
      {
         alive = ( lev->oo_serial == b->u.blk.serial );
         break;
      }
   if ( alive )
      share = b->u.blk.home->vars;
   else if ( b->u.blk.kept )
      share = b->u.blk.kept;
   else
      exiterror( ERR_EXECUTION_ERROR, 16 );
   frame = new_frame( TSD );
   frame->block = 1;
   frame->labels = &code->labels;
   frame->outer = alive ? b->u.blk.homeframe : NULL;
   if ( frame->outer )
   {
      frame->self = frame->outer->self;
      frame->scope = frame->outer->scope;
   }
   {
      int pin = pin_object( TSD, b );
      r = oo_run_block( TSD, code->body, make_plist( TSD, argc, argv ), frame, share,
                        code->procedure ? NULL : block_setup, code );
      unpin_object( TSD, pin );
   }
   FreeTSD( frame );
   return r;
}

static oo_object *block_self( tsd_t *TSD, oo_call *c )
{
   if ( c->self == NULL || c->self->kind != OK_BLOCK )
      err_nomethod( TSD, c->selfstr, c->name );
   return c->self;
}

/* b~call(args...), b~value(args...) */
static streng *m_block_call( tsd_t *TSD, oo_call *c )
{
   return call_block( TSD, block_self( TSD, c ), c->argc, c->argv );
}

static streng *m_block_source( tsd_t *TSD, oo_call *c )
{
   return Str_dupTSD( block_self( TSD, c )->u.blk.code->node->name );
}

static streng *m_block_string( tsd_t *TSD, oo_call *c )
{
   streng *r = Str_creTSD( "{" );
   r = cats( TSD, r, block_self( TSD, c )->u.blk.code->node->name );
   return catc( TSD, r, "}" );
}

/* b~whileTrue(body): calls body as long as b returns 1 */
static streng *m_block_while( tsd_t *TSD, oo_call *c )
{
   oo_object *b = block_self( TSD, c );
   const streng *body = opt_arg( c, 0 );
   int want = str_eqc( c->name, "WHILETRUE" );

   for ( ; ; )
   {
      streng *t = call_block( TSD, b, 0, NULL );
      int v;
      if ( t == NULL )
         exiterror( ERR_NO_DATA_RETURNED, 1, "a block" );
      v = lisp_bool( TSD, t );
      Free_stringTSD( t );
      if ( v != want )
         break;
      if ( body )
      {
         streng *x = fn_call( TSD, body, 0, NULL, 0 );
         if ( x )
            Free_stringTSD( x );
      }
   }
   return NULL;
}

/* .Block~new(source): a block from a string, bound to the caller */
static streng *m_block_new( tsd_t *TSD, oo_call *c )
{
   const streng *src = need_arg( TSD, c, 0 );
   oo_blockcode *b;
   oo_object *o;
   nodeptr n;

   /* a node of its own keys the parsed code; it lives until the end */
   n = (nodeptr) MallocTSD( sizeof( treenode ) );
   memset( n, 0, sizeof( treenode ) );
   n->type = X_OO_BLOCK;
   n->name = Str_dupTSD( src );
   n->lineno = TSD->currentnode ? TSD->currentnode->lineno : 0;
   b = block_code( TSD, n );
   o = bind_block( TSD, b );
   return hdup( TSD, o );
}

/*
 * The collection methods. The items of the receiver are visited in order
 * with their indexes; the function (a block or a routine name) gets
 * (item, index).
 */
typedef struct {
   long n;
   streng **item;    /* copies */
   streng **index;   /* copies */
} coll_items;

static void coll_free( tsd_t *TSD, coll_items *ci )
{
   long i;
   for ( i = 0; i < ci->n; i++ )
   {
      Free_stringTSD( ci->item[i] );
      Free_stringTSD( ci->index[i] );
   }
   if ( ci->item )
      FreeTSD( ci->item );
   if ( ci->index )
      FreeTSD( ci->index );
}

static oo_object *coll_self( tsd_t *TSD, oo_call *c, coll_items *ci )
{
   oo_object *o = c->self;
   long i, n = 0;

   ci->n = 0;
   ci->item = ci->index = NULL;
   if ( o == NULL )
      err_nomethod( TSD, c->selfstr, c->name );
   switch ( o->kind )
   {
      case OK_ARRAY:
      case OK_QUEUE:
         ci->item = (streng **) MallocTSD( ( o->u.a.size + 1 ) * sizeof( streng * ) );
         ci->index = (streng **) MallocTSD( ( o->u.a.size + 1 ) * sizeof( streng * ) );
         for ( i = 0; i < o->u.a.size; i++ )
            if ( o->u.a.v[i] )
            {
               ci->item[n] = Str_dupTSD( o->u.a.v[i] );
               ci->index[n++] = ARR_MULTI( o ) ? arr_index_value( TSD, o, i ) : mkint( TSD, i + 1 );
            }
         break;
      case OK_DIRECTORY:
      case OK_TABLE:
      {
         oo_entry *e;
         long cnt = is_bag( TSD, o ) ? setbag_items( TSD, o ) : (long) o->u.h->count, k;
         ci->item = (streng **) MallocTSD( ( cnt + 1 ) * sizeof( streng * ) );
         ci->index = (streng **) MallocTSD( ( cnt + 1 ) * sizeof( streng * ) );
         for ( e = o->u.h->first; e; e = e->onext )
         {
            if ( is_bag( TSD, o ) || is_set( TSD, o ) )
            {
               /* a Set or Bag: the item is its own index */
               long times = is_bag( TSD, o ) ? bag_count( e->value ) : 1;
               for ( k = 0; k < times; k++ )
               {
                  ci->item[n] = Str_dupTSD( e->key );
                  ci->index[n++] = Str_dupTSD( e->key );
               }
               continue;
            }
            ci->item[n] = Str_dupTSD( e->value );
            ci->index[n++] = Str_dupTSD( e->key );
         }
         break;
      }
      case OK_STEM:
      {
         stemlist l;
         variableptr b = stem_box( TSD, o );
         stem_sorted_tails( TSD, o, &l );
         ci->item = (streng **) MallocTSD( ( l.n + 1 ) * sizeof( streng * ) );
         ci->index = l.t ? l.t : (streng **) MallocTSD( sizeof( streng * ) );
         for ( i = 0; i < l.n; i++ )
         {
            const streng *v = oo_stem_get( TSD, b, l.t[i] );
            ci->item[i] = v ? Str_dupTSD( v ) : nullstringptr();
         }
         n = l.n;
         break;
      }
      case OK_INTERVAL:
      {
         long cnt = iv_count( o );
         ci->item = (streng **) MallocTSD( ( cnt + 1 ) * sizeof( streng * ) );
         ci->index = (streng **) MallocTSD( ( cnt + 1 ) * sizeof( streng * ) );
         for ( i = 0; i < cnt; i++ )
         {
            ci->item[i] = mkint( TSD, o->u.iv.from + i * o->u.iv.by );
            ci->index[i] = mkint( TSD, i + 1 );
         }
         n = cnt;
         break;
      }
      case OK_CONS:
      case OK_NIL:
      {
         const streng **v = list_items( TSD, o->kind == OK_CONS ? o : NULL, &n );
         ci->item = (streng **) MallocTSD( ( n + 1 ) * sizeof( streng * ) );
         ci->index = (streng **) MallocTSD( ( n + 1 ) * sizeof( streng * ) );
         for ( i = 0; i < n; i++ )
         {
            ci->item[i] = Str_dupTSD( v[i] );
            ci->index[i] = mkint( TSD, i + 1 );
         }
         FreeTSD( (void *) v );
         break;
      }
      default:
         err_nomethod( TSD, c->selfstr, c->name );
   }
   ci->n = n;
   return o;
}

/*
 * A new, empty collection of the receiver's class. collect gives an Array
 * for an Interval and a SortedCollection (its order would not hold).
 */
static oo_object *coll_like( tsd_t *TSD, oo_object *o, int collect )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *r;

   switch ( o->kind )
   {
      case OK_INTERVAL:
         return new_object( TSD, ot->cArray, OK_ARRAY );
      case OK_ARRAY:
      case OK_QUEUE:
         if ( ARR_MULTI( o ) || ( collect && is_subclass( o->cls, ot->cSorted ) ) )
            return new_object( TSD, ot->cArray, OK_ARRAY );
         r = new_object( TSD, o->cls, o->kind );
         if ( o->aux )
            r->aux = Str_dupTSD( o->aux );
         return r;
      case OK_DIRECTORY:
      case OK_TABLE:
         r = new_object( TSD, o->cls, o->kind );
         r->u.h = (oo_hash *) MallocTSD( sizeof( oo_hash ) );
         memset( r->u.h, 0, sizeof( oo_hash ) );
         return r;
      case OK_STEM:
         return new_object( TSD, ot->cStem, OK_STEM );
      default:
         return NULL;   /* a LISP list: built with list_from */
   }
}

/* adds item (taken over) to r, a collection made by coll_like */
static void coll_add( tsd_t *TSD, oo_object *r, const streng *index, streng *item )
{
   switch ( r->kind )
   {
      case OK_ARRAY:
      case OK_QUEUE:
         oo_array_put( TSD, r, r->u.a.size + 1, item );
         break;
      case OK_DIRECTORY:
      case OK_TABLE:
         if ( is_set( TSD, r ) || is_bag( TSD, r ) )
         {
            setbag_add( TSD, r, item, 1 );
            Free_stringTSD( item );
         }
         else
            h_put( TSD, r->u.h, index, item );
         break;
      case OK_STEM:
         oo_stem_put( TSD, r->u.pool, ( (oo_tsd *) TSD->oo_tsd )->s_stem, index, item );
         break;
   }
}

static streng *coll_call( tsd_t *TSD, const streng *f, coll_items *ci, long i, int need )
{
   const streng *argv[2];
   argv[0] = ci->item[i];
   argv[1] = ci->index[i];
   return fn_call( TSD, f, 2, argv, need );
}

/* do(f), each(f): f(item, index) for every item */
static streng *m_coll_do( tsd_t *TSD, oo_call *c )
{
   coll_items ci;
   const streng *f = need_arg( TSD, c, 0 );
   long i;

   coll_self( TSD, c, &ci );
   for ( i = 0; i < ci.n; i++ )
   {
      streng *r = coll_call( TSD, f, &ci, i, 0 );
      if ( r )
         Free_stringTSD( r );
   }
   coll_free( TSD, &ci );
   return NULL;
}

/* select(f), reject(f), collect(f): a new collection of the same class */
static streng *m_coll_select( tsd_t *TSD, oo_call *c )
{
   coll_items ci;
   const streng *f = need_arg( TSD, c, 0 );
   int mode = str_eqc( c->name, "SELECT" ) ? 1 : ( str_eqc( c->name, "REJECT" ) ? 0 : 2 );
   oo_object *o = coll_self( TSD, c, &ci ), *res = coll_like( TSD, o, mode == 2 );
   const streng **lv = NULL;
   long i, nl = 0;
   streng *out;

   if ( res == NULL )
      lv = (const streng **) MallocTSD( ( ci.n + 1 ) * sizeof( streng * ) );
   for ( i = 0; i < ci.n; i++ )
   {
      streng *r = coll_call( TSD, f, &ci, i, 1 ), *v;
      if ( mode == 2 )
         v = r;
      else
      {
         int b = lisp_bool( TSD, r );
         Free_stringTSD( r );
         if ( b != mode )
            continue;
         v = Str_dupTSD( ci.item[i] );
      }
      if ( res )
         coll_add( TSD, res, ci.index[i], v );
      else
         lv[nl++] = v;
   }
   if ( res )
      out = hdup( TSD, res );
   else
   {
      out = list_from( TSD, lv, nl, NULL );
      for ( i = 0; i < nl; i++ )
         Free_stringTSD( (streng *) lv[i] );
      FreeTSD( (void *) lv );
   }
   coll_free( TSD, &ci );
   return out;
}

/* detect(f [, none]): the first item for which f is 1, else none (.nil) */
static streng *m_coll_detect( tsd_t *TSD, oo_call *c )
{
   coll_items ci;
   const streng *f = need_arg( TSD, c, 0 ), *none = opt_arg( c, 1 );
   streng *found = NULL;
   long i;

   coll_self( TSD, c, &ci );
   for ( i = 0; i < ci.n && found == NULL; i++ )
   {
      streng *r = coll_call( TSD, f, &ci, i, 1 );
      if ( lisp_bool( TSD, r ) )
         found = Str_dupTSD( ci.item[i] );
      Free_stringTSD( r );
   }
   coll_free( TSD, &ci );
   if ( found )
      return found;
   if ( none )
   {
      oo_object *bo = obj_of( TSD, none );
      if ( bo && bo->kind == OK_BLOCK )
         return fn_call( TSD, none, 0, NULL, 1 );
      return Str_dupTSD( none );
   }
   return nil_handle( TSD );
}

/* inject(initial, f): f(f(initial, item1, index1), item2, index2) ... */
static streng *m_coll_inject( tsd_t *TSD, oo_call *c )
{
   coll_items ci;
   const streng *f = need_arg( TSD, c, 1 );
   streng *acc = Str_dupTSD( need_arg( TSD, c, 0 ) );
   long i;

   coll_self( TSD, c, &ci );
   for ( i = 0; i < ci.n; i++ )
   {
      const streng *argv[3];
      streng *r;
      argv[0] = acc;
      argv[1] = ci.item[i];
      argv[2] = ci.index[i];
      r = fn_call( TSD, f, 3, argv, 1 );
      Free_stringTSD( acc );
      acc = r;
   }
   coll_free( TSD, &ci );
   return acc;
}

/* anySatisfy(f), allSatisfy(f), count(f) */
static streng *m_coll_test( tsd_t *TSD, oo_call *c )
{
   coll_items ci;
   const streng *f = need_arg( TSD, c, 0 );
   int any = str_eqc( c->name, "ANYSATISFY" ), all = str_eqc( c->name, "ALLSATISFY" );
   long i, count = 0;

   coll_self( TSD, c, &ci );
   for ( i = 0; i < ci.n; i++ )
   {
      streng *r = coll_call( TSD, f, &ci, i, 1 );
      int b = lisp_bool( TSD, r );
      Free_stringTSD( r );
      count += b;
      if ( ( any && b ) || ( all && !b ) )
         break;
   }
   coll_free( TSD, &ci );
   if ( any )
      return mkbool( TSD, count > 0 );
   if ( all )
      return mkbool( TSD, i == ci.n );
   return mkint( TSD, count );
}

/* sortWith(f): sorts an Array or List in place; f(a, b) is 1 if a goes first */
static const streng *sort_fn;

static void merge_sort( tsd_t *TSD, streng **v, streng **tmp, long n )
{
   long h = n / 2, i = 0, j = h, k = 0;

   if ( n < 2 )
      return;
   merge_sort( TSD, v, tmp, h );
   merge_sort( TSD, v + h, tmp, n - h );
   while ( i < h && j < n )
   {
      const streng *argv[2];
      streng *r;
      int left;
      argv[0] = v[i];
      argv[1] = v[j];
      r = fn_call( TSD, sort_fn, 2, argv, 1 );
      left = lisp_bool( TSD, r );
      Free_stringTSD( r );
      tmp[k++] = left ? v[i++] : v[j++];
   }
   while ( i < h )
      tmp[k++] = v[i++];
   while ( j < n )
      tmp[k++] = v[j++];
   memcpy( v, tmp, n * sizeof( streng * ) );
}

static streng *m_coll_sortwith( tsd_t *TSD, oo_call *c )
{
   oo_object *o = c->self;
   const streng *f = need_arg( TSD, c, 0 ), *save = sort_fn;
   streng **v, **tmp;
   long n = 0, i;

   if ( o == NULL || ( o->kind != OK_ARRAY && o->kind != OK_QUEUE ) )
      err_nomethod( TSD, c->selfstr, c->name );
   if ( ARR_MULTI( o ) )
      exiterror( ERR_EXECUTION_ERROR, 8, tmpstr_of( TSD, c->name ) );
   /* items without holes, sorted, then stored back from 1 */
   v = (streng **) MallocTSD( ( o->u.a.size + 1 ) * sizeof( streng * ) );
   tmp = (streng **) MallocTSD( ( o->u.a.size + 1 ) * sizeof( streng * ) );
   for ( i = 0; i < o->u.a.size; i++ )
      if ( o->u.a.v[i] )
         v[n++] = Str_dupTSD( o->u.a.v[i] );
   sort_fn = f;
   merge_sort( TSD, v, tmp, n );
   sort_fn = save;
   for ( i = 0; i < o->u.a.size; i++ )
      if ( o->u.a.v[i] )
      {
         Free_stringTSD( o->u.a.v[i] );
         o->u.a.v[i] = NULL;
      }
   o->u.a.size = 0;
   for ( i = 0; i < n; i++ )
      oo_array_put( TSD, o, i + 1, v[i] );
   FreeTSD( v );
   FreeTSD( tmp );
   return Str_dupTSD( c->selfstr );
}

static void add_coll_methods( tsd_t *TSD, oo_object *c )
{
   add_methods( TSD, c, "DO EACH", m_coll_do, 0 );
   add_methods( TSD, c, "SELECT REJECT COLLECT", m_coll_select, 0 );
   add_methods( TSD, c, "DETECT", m_coll_detect, 0 );
   add_methods( TSD, c, "INJECT", m_coll_inject, 0 );
   add_methods( TSD, c, "ANYSATISFY ALLSATISFY COUNT", m_coll_test, 0 );
}

static void init_blocks( tsd_t *TSD, oo_tsd *ot )
{
   oo_object *c;

   ot->cBlock = c = new_class( TSD, "Block", NULL, ot->cObject, OK_BLOCK );
   add_methods( TSD, c, "NEW", m_block_new, MF_CLASS );
   add_methods( TSD, c, "CALL VALUE", m_block_call, 0 );
   add_methods( TSD, c, "SOURCE", m_block_source, 0 );
   add_methods( TSD, c, "STRING", m_block_string, 0 );
   add_methods( TSD, c, "WHILETRUE WHILEFALSE", m_block_while, 0 );

   add_coll_methods( TSD, ot->cArray );
   add_coll_methods( TSD, ot->cQueue );
   add_coll_methods( TSD, ot->cList );
   add_coll_methods( TSD, ot->cDirectory );
   add_coll_methods( TSD, ot->cStringTable );
   add_coll_methods( TSD, ot->cTable );
   add_coll_methods( TSD, ot->cStem );
   add_coll_methods( TSD, ot->cCons );
   add_coll_methods( TSD, ot->cNilList );
   add_methods( TSD, ot->cArray, "SORTWITH", m_coll_sortwith, 0 );
   add_methods( TSD, ot->cList, "SORTWITH", m_coll_sortwith, 0 );
   add_methods( TSD, ot->cQueue, "SORTWITH", m_coll_sortwith, 0 );
}

/* ------------------------------------------------------------------ */
/* condition('O'), Block~ensure, Message                               */
/* ------------------------------------------------------------------ */

/* condition('O'): the current condition as a Directory, or .nil */
streng *oo_condition_object( tsd_t *TSD, const sigtype *sig )
{
   oo_tsd *ot = get_ot( TSD );
   oo_object *d;
   char buf[40];
   const streng *argv[1];
   streng *t;

   TSD->oo_active = 1;
   if ( sig == NULL )
      return hdup( TSD, ot->nil );
   d = new_object( TSD, ot->cDirectory, OK_DIRECTORY );
   d->u.h = (oo_hash *) MallocTSD( sizeof( oo_hash ) );
   memset( d->u.h, 0, sizeof( oo_hash ) );
#define DPUT( k, v ) do { streng *k_ = Str_creTSD( k ); h_put( TSD, d->u.h, k_, v ); \
                          Free_stringTSD( k_ ); } while ( 0 )
   DPUT( "CONDITION", Str_creTSD( signalnames[sig->type] ) );
   DPUT( "INSTRUCTION", Str_creTSD( sig->invoke ? "SIGNAL" : "CALL" ) );
   DPUT( "DESCRIPTION", sig->descr ? Str_dupTSD( sig->descr ) : nullstringptr() );
   if ( sig->type == SIGNAL_SYNTAX )
   {
      if ( sig->subrc )
         sprintf( buf, "%d.%d", sig->rc, sig->subrc );
      else
         sprintf( buf, "%d", sig->rc );
      DPUT( "CODE", Str_creTSD( buf ) );
      DPUT( "RC", mkint( TSD, sig->rc ) );
      sprintf( buf, "%d", sig->rc );
      t = Str_creTSD( buf );
      argv[0] = t;
      DPUT( "ERRORTEXT", call_bif( TSD, "ERRORTEXT", 1, argv, NULL ) );
      Free_stringTSD( t );
   }
   else if ( sig->rc )
      DPUT( "RC", mkint( TSD, sig->rc ) );
   if ( sig->lineno > 0 )
      DPUT( "POSITION", mkint( TSD, sig->lineno ) );
#undef DPUT
   return hdup( TSD, d );
}

typedef struct { oo_object *block; streng *result; } ensure_data;

static void ensure_run( tsd_t *TSD, void *data )
{
   ensure_data *d = (ensure_data *) data;
   d->result = call_block( TSD, d->block, 0, NULL );
}

/*
 * b~ensure(cleanup): runs b, then cleanup (a block or routine), also when
 * b ends with an error or EXIT; the error or EXIT then goes on.
 * b~ifCurtailed(cleanup): cleanup only in that case.
 */
static streng *m_block_ensure( tsd_t *TSD, oo_call *c )
{
   extern int oo_try_call( tsd_t *TSD, void (*fn)( tsd_t *, void * ), void *data );
   extern const char *oo_captured_error( int *no, int *sub );
   extern void oo_reraise_error( int no, int sub, const char *text );
   extern void jump_script_exit( tsd_t *TSD, streng *result );
   ensure_data d;
   const streng *cleanup = need_arg( TSD, c, 0 );
   int status, always = str_eqc( c->name, "ENSURE" ), no = 0, sub = 0;
   char text[600];
   streng *exitval = NULL, *r;

   d.block = block_self( TSD, c );
   d.result = NULL;
   status = oo_try_call( TSD, ensure_run, &d );
   if ( status == 1 )
   {
      exitval = TSD->systeminfo->result;
      TSD->systeminfo->result = NULL;
   }
   else if ( status == 2 )
   {
      strncpy( text, oo_captured_error( &no, &sub ), sizeof( text ) - 1 );
      text[sizeof( text ) - 1] = '\0';
   }
   if ( always || status )
   {
      r = fn_call( TSD, cleanup, 0, NULL, 0 );
      if ( r )
         Free_stringTSD( r );
   }
   if ( status == 1 )
   {
      TSD->instore_is_errorfree = 1;   /* as the EXIT instruction does */
      jump_script_exit( TSD, exitval );
   }
   if ( status == 2 )
      oo_reraise_error( no, sub, text );
   return d.result;
}

/*
 * Message: a message as an object. .message~new(target, name, args...);
 * send([target]) sends it, result is its last result.
 */
static oo_object *msg_self( tsd_t *TSD, oo_call *c )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   if ( c->self == NULL || c->self->kind != OK_ARRAY || !is_subclass( c->self->cls, ot->cMessage ) )
      err_nomethod( TSD, c->selfstr, c->name );
   return c->self;
}

static streng *m_msg_init( tsd_t *TSD, oo_call *c )
{
   oo_object *o = msg_self( TSD, c );
   streng *name;
   int i;

   oo_array_put( TSD, o, 1, Str_dupTSD( need_arg( TSD, c, 0 ) ) );
   name = oo_stringify( TSD, need_arg( TSD, c, 1 ) );
   if ( name == NULL )
      name = Str_dupTSD( need_arg( TSD, c, 1 ) );
   mem_upper( name->value, name->len );
   oo_array_put( TSD, o, 2, name );
   for ( i = 2; i < c->argc; i++ )
      oo_array_put( TSD, o, i + 1, c->argv[i] ? Str_dupTSD( c->argv[i] ) : NULL );
   /* the number of arguments, so that omitted ones stay omitted */
   o->snapshot = 0;
   if ( o->aux )
      Free_stringTSD( o->aux );
   o->aux = NULL;
   o->u.a.dims = NULL;
   return NULL;
}

static streng *m_msg_send( tsd_t *TSD, oo_call *c )
{
   oo_object *o = msg_self( TSD, c );
   const streng *target = opt_arg( c, 0 ) ? opt_arg( c, 0 ) : o->u.a.v[0];
   long argc = o->u.a.size - 2, i;
   const streng **argv = (const streng **) MallocTSD( ( argc + 1 ) * sizeof( streng * ) );
   streng *r;

   for ( i = 0; i < argc; i++ )
      argv[i] = o->u.a.v[i + 2];
   r = send_msg( TSD, target, o->u.a.v[1], (int) argc, argv );
   FreeTSD( (void *) argv );
   if ( o->aux )
      Free_stringTSD( o->aux );
   o->aux = r ? Str_dupTSD( r ) : NULL;
   return r;
}

static streng *m_msg_parts( tsd_t *TSD, oo_call *c )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *o = msg_self( TSD, c );

   if ( str_eqc( c->name, "TARGET" ) )
      return Str_dupTSD( o->u.a.v[0] );
   if ( str_eqc( c->name, "MESSAGENAME" ) )
      return Str_dupTSD( o->u.a.v[1] );
   if ( str_eqc( c->name, "RESULT" ) )
      return o->aux ? Str_dupTSD( o->aux ) : hdup( TSD, ot->nil );
   if ( str_eqc( c->name, "STRING" ) )
   {
      streng *r = Str_creTSD( "a Message " );
      return cats( TSD, r, o->u.a.v[1] );
   }
   {
      /* ARGUMENTS */
      oo_object *a = new_object( TSD, ot->cArray, OK_ARRAY );
      long i;
      for ( i = 2; i < o->u.a.size; i++ )
         oo_array_put( TSD, a, i - 1, o->u.a.v[i] ? Str_dupTSD( o->u.a.v[i] ) : NULL );
      return hdup( TSD, a );
   }
}

static void init_condition_classes( tsd_t *TSD, oo_tsd *ot )
{
   oo_object *c;

   add_methods( TSD, ot->cBlock, "ENSURE IFCURTAILED", m_block_ensure, 0 );
   ot->cMessage = c = new_class( TSD, "Message", NULL, ot->cObject, OK_ARRAY );
   add_methods( TSD, c, "INIT", m_msg_init, 0 );
   add_methods( TSD, c, "SEND VALUE", m_msg_send, 0 );
   add_methods( TSD, c, "TARGET MESSAGENAME RESULT ARGUMENTS STRING", m_msg_parts, 0 );
}

/* ------------------------------------------------------------------ */
/* Garbage collection                                                 */
/* ------------------------------------------------------------------ */

/*
 * Objects are referenced by handle strings that are copied freely, so
 * they can't be counted. oo_gc() marks every object whose handle is
 * stored somewhere reachable and frees the others. It runs only between
 * clauses (see interpret()), when the number of objects has doubled since
 * the last collection.
 *
 * Roots: the variables of all active procedure levels (with stems and
 * arguments), the reserved variables, the environment, all classes, the
 * receivers of native methods that are running (pinned), and the objects
 * a suspended clause may still hold: for every level that is waiting for
 * a call it made, the objects created between the start of its clause and
 * that call (oo_mark .. oo_callmark).
 *
 * Handles of freed objects are never reused: a stale handle (in the
 * external queue, say) is just a string that names no object.
 *
 * Modes: 1 on (default), 0 off, 2 stress (collect before every clause,
 * for testing). --gc, --no-gc and REGINA_GC=on/off/stress set it.
 */
static int gc_mode = -1;                /* -1: not yet decided */

void oo_gc_set_mode( int mode )
{
   gc_mode = mode;
}

static int gc_get_mode( void )
{
   if ( gc_mode < 0 )
   {
      const char *e = getenv( "REGINA_GC" );
      gc_mode = 1;
      if ( e )
      {
         if ( oo_word_is( e, "0" ) || oo_word_is( e, "OFF" ) || oo_word_is( e, "NO" ) )
            gc_mode = 0;
         else if ( oo_word_is( e, "STRESS" ) )
            gc_mode = 2;
      }
   }
   return gc_mode;
}

#define GC_MIN_OBJECTS 10000

typedef struct {
   tsd_t *TSD;
   oo_tsd *ot;
   oo_object **stack;
   long n, cap;
} gc_state;

static void gc_push( gc_state *g, oo_object *o )
{
   tsd_t *TSD = g->TSD;

   if ( o == NULL || o->gcmark )
      return;
   o->gcmark = 1;
   if ( g->n >= g->cap )
   {
      long nc = g->cap ? g->cap * 2 : 1024;
      oo_object **ns = (oo_object **) MallocTSD( nc * sizeof( oo_object * ) );
      if ( g->n )
         memcpy( ns, g->stack, g->n * sizeof( oo_object * ) );
      if ( g->stack )
         FreeTSD( g->stack );
      g->stack = ns;
      g->cap = nc;
   }
   g->stack[g->n++] = o;
}

static void gc_value( void *data, const streng *s )
{
   gc_state *g = (gc_state *) data;
   if ( s && OO_LOOKS_LIKE_HANDLE( s ) )
      gc_push( g, obj_of( g->TSD, s ) );
}

static void gc_scan_pool( gc_state *g, var_hashtable *pool )
{
   extern void oo_scan_pool( const tsd_t *TSD, var_hashtable *pool,
                             void ( *cb )( void *data, const streng *s ), void *data );
   oo_scan_pool( g->TSD, pool, gc_value, g );
}

/* marks what object o refers to */
static void gc_children( gc_state *g, oo_object *o )
{
   oo_scope *sc;
   long i;

   for ( sc = o->scopes; sc; sc = sc->next )
   {
      gc_scan_pool( g, sc->pool );
      gc_push( g, sc->superproxy );
   }
   if ( o->aux )
      gc_value( g, o->aux );
   switch ( o->kind )
   {
      case OK_ARRAY:
      case OK_QUEUE:
         for ( i = 0; i < o->u.a.size; i++ )
            gc_value( g, o->u.a.v[i] );
         break;
      case OK_DIRECTORY:
      case OK_TABLE:
      {
         oo_entry *e;
         if ( o->u.h )
            for ( e = o->u.h->first; e; e = e->onext )
            {
               gc_value( g, e->key );
               gc_value( g, e->value );
            }
         break;
      }
      case OK_STEM:
         gc_scan_pool( g, o->u.pool );
         break;
      case OK_CONS:
         gc_value( g, o->u.cons.car );
         gc_value( g, o->u.cons.cdr );
         break;
      case OK_SUPPLIER:
         gc_push( g, o->u.sp.items );
         gc_push( g, o->u.sp.indexes );
         break;
      case OK_SUPER:
         gc_push( g, o->u.sup.self );
         gc_push( g, o->u.sup.start );
         break;
      case OK_BLOCK:
         gc_scan_pool( g, o->u.blk.kept );
         break;
      case OK_STRING:
      case OK_BUFFER:
         gc_value( g, o->u.s );
         break;
      default:
         break;
   }
}

/* unlinks block b from the list of blocks of its creator */
static void gc_unlink_block( oo_object *b )
{
   proclevel home = b->u.blk.home;
   oo_object **pp;

   if ( home == NULL )
      return;
   for ( pp = (oo_object **) &home->oo_blocks; *pp; pp = &( *pp )->u.blk.nextblock )
      if ( *pp == b )
      {
         *pp = b->u.blk.nextblock;
         return;
      }
}

/* frees everything an object owns, then the object */
static void gc_free( tsd_t *TSD, oo_object *o )
{
   oo_scope *s, *sn;

   switch ( o->kind )
   {
      case OK_DIRECTORY:
      case OK_TABLE:
         if ( o->u.h )
         {
            while ( o->u.h->first )
            {
               streng *v = h_remove( TSD, o->u.h, o->u.h->first->key );
               Free_stringTSD( v );
            }
            if ( o->u.h->tbl )
               FreeTSD( o->u.h->tbl );
            FreeTSD( o->u.h );
         }
         break;
      case OK_STRING:
      case OK_BUFFER:
         if ( o->u.s )
            Free_stringTSD( o->u.s );
         break;
      case OK_STEM:
         if ( o->u.pool )
            kill_variables( TSD, o->u.pool );
         break;
      case OK_PYOBJ:
         pyb_release( o->u.pyref );
         break;
      case OK_BLOCK:
         gc_unlink_block( o );
         break;
      default:
         break;
   }
   for ( s = o->scopes; s; s = sn )
   {
      sn = s->next;
      if ( s->pool )
         kill_variables( TSD, s->pool );
      FreeTSD( s );
   }
   if ( o->objname )
      Free_stringTSD( o->objname );
   if ( o->aux )
      Free_stringTSD( o->aux );
   free_object( TSD, o );
}

/*
 * Collects. forced: also when the mode is off ([]GC()). Returns the number
 * of objects freed, or -1 if a collection is not possible now.
 */
long oo_gc( tsd_t *TSD, int forced )
{
   extern var_hashtable *oo_pool0( const tsd_t *TSD );
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   gc_state g;
   proclevel lev;
   paramboxptr a;
   unsigned long i, oldest = (unsigned long) -1, now = TSD->oo_clause;
   long freed = 0;
   int k;

   TSD->oo_gc_wanted = 0;
   if ( ot == NULL || ( !forced && gc_get_mode() == 0 ) )
      return -1;
   /* an external Rexx program is running: the caller's levels are hidden */
   if ( TSD->systeminfo == NULL || TSD->systeminfo->previous )
      return -1;

   g.TSD = TSD;
   g.ot = ot;
   g.stack = NULL;
   g.n = g.cap = 0;
   for ( i = 1; i <= ot->nobjs; i++ )
      if ( ot->objs[i] )
         ot->objs[i]->gcmark = 0;

   /* roots */
   gc_push( &g, ot->nil );
   gc_push( &g, ot->environment );
   for ( k = 0; k < ot->nclasses; k++ )
      gc_push( &g, ot->classes[k] );
   for ( k = 0; k < OO_PINS; k++ )
      if ( ot->pins[k] && ot->pins[k] <= ot->nobjs )
         gc_push( &g, ot->objs[ot->pins[k]] );
   for ( i = 1; i <= ot->nobjs; i++ )
      if ( ot->objs[i] && ot->objs[i]->gcroot )
         gc_push( &g, ot->objs[i] );
   gc_scan_pool( &g, oo_pool0( TSD ) );
   for ( lev = TSD->currlevel; lev; lev = lev->prev )
   {
      oo_frame *f = (oo_frame *) lev->oo_ctx;
      gc_scan_pool( &g, lev->vars );
      for ( a = lev->args; a; a = a->next )
      {
         gc_value( &g, a->value );
         if ( a->stempool )
            gc_scan_pool( &g, a->stempool );
      }
      if ( f )
         gc_push( &g, f->self );
      if ( lev != TSD->currlevel )
      {
         if ( lev->oo_clause < oldest )
            oldest = lev->oo_clause;
         /* what this level's unfinished clause may hold */
         for ( i = lev->oo_mark + 1; i <= lev->oo_callmark && i <= ot->nobjs; i++ )
            gc_push( &g, ot->objs[i] );
      }
   }

   /* mark */
   while ( g.n > 0 )
      gc_children( &g, g.stack[--g.n] );
   if ( g.stack )
      FreeTSD( g.stack );

   /*
    * sweep. An unreachable object may still be held by a clause that is
    * waiting for a call: in "a + f()" f may drop the last variable that
    * refers to a. Such an object is condemned and freed only by a later
    * collection, when every clause that was waiting has ended.
    */
   for ( i = 1; i <= ot->nobjs; i++ )
   {
      oo_object *o = ot->objs[i];
      if ( o == NULL || o->kind == OK_CLASS )
         continue;
      if ( o->gcmark )
         o->condemned = 0;
      else if ( o->condemned && o->condemned < oldest )
      {
         gc_free( TSD, o );
         freed++;
      }
      else if ( !o->condemned )
         o->condemned = now ? now : 1;
   }
   ot->nlive -= freed;
   ot->gc_next = ot->nlive * 2 > GC_MIN_OBJECTS ? ot->nlive * 2 : GC_MIN_OBJECTS;
   ot->gc_runs++;
   ot->gc_freed += freed;
   if ( gc_get_mode() == 2 )
      TSD->oo_gc_wanted = 1;
   return freed;
}

/* called by new_object */
static void gc_count( tsd_t *TSD, oo_tsd *ot )
{
   int mode = gc_get_mode();

   ot->nlive++;
   TSD->oo_nobjs = ot->nobjs;
   if ( ot->gc_next == 0 )
      ot->gc_next = GC_MIN_OBJECTS;
   if ( ( mode == 1 && ot->nlive >= ot->gc_next ) || mode == 2 )
      TSD->oo_gc_wanted = 1;
}

/*
 * The receivers of native methods that are running are roots: their C
 * code may hold the object after the last handle is gone. A ring of ids:
 * an error that jumps out of a method leaves its pin behind, which only
 * keeps an object a little longer.
 */
static int pin_object( tsd_t *TSD, oo_object *o )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   int k = ot->pintop;

   ot->pins[k] = o ? o->id : 0;
   ot->pintop = ( k + 1 ) % OO_PINS;
   return k;
}

static void unpin_object( tsd_t *TSD, int k )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;

   ot->pins[k] = 0;
   ot->pintop = k;
}

/* ------------------------------------------------------------------ */
/* Python: .Python and .PyObject on top of pybridge.c                 */
/* ------------------------------------------------------------------ */

/*
 * Values crossing to Python: a plain string that is a number becomes an
 * int or float, any other string a str; .nil becomes None; Array, List,
 * Queue and Cons lists become lists; Directory, Table and Stem become
 * dicts; a PyObject is the Python object itself. Results coming back:
 * None is .nil, True/False are 1/0, numbers, str and bytes are strings,
 * everything else is a PyObject.
 */

static int py_callback( long id, long n, const pyref *args, pyref *result,
                        char *err, int errlen );
static tsd_t *py_tsd;               /* the thread that uses Python      */
static streng *py_pending_exit;     /* EXIT in a block called by Python */
static int py_exit_pending = 0;

static void py_start( tsd_t *TSD )
{
   static int started = 0;
   const char *script = NULL;

   if ( started )
      return;
   if ( !pyb_available() )
      exiterror( ERR_EXECUTION_ERROR, 15 );
   if ( TSD->systeminfo && TSD->systeminfo->input_file )
      script = TSD->systeminfo->input_file->value;
   fflush( stdout );
   if ( pyb_init( script, NULL ) != 0 )
      exiterror( ERR_EXECUTION_ERROR, 14, pyb_error() );
   py_tsd = TSD;
   pyb_set_callback( py_callback );
   started = 1;
}

static void py_fail( tsd_t *TSD )
{
   if ( py_exit_pending )
   {
      /* EXIT in a block that Python called: now that Python is left */
      extern void jump_script_exit( tsd_t *TSD, streng *result );
      py_exit_pending = 0;
      TSD->instore_is_errorfree = 1;
      jump_script_exit( TSD, py_pending_exit );
   }
   fflush( stdout );
   exiterror( ERR_EXECUTION_ERROR, 14, pyb_error() );
}

static pyref py_check( tsd_t *TSD, pyref r )
{
   if ( r < 0 )
      py_fail( TSD );
   return r;
}

static streng *py_wrap( tsd_t *TSD, pyref r )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *o = new_object( TSD, ot->cPyObject, OK_PYOBJ );
   o->u.pyref = r;
   return hdup( TSD, o );
}

static pyref to_py( tsd_t *TSD, const streng *s, int depth );

static pyref py_list_of( tsd_t *TSD, const streng **v, long n, int depth )
{
   pyref *items = (pyref *) MallocTSD( ( n + 1 ) * sizeof( pyref ) ), r;
   long i;

   for ( i = 0; i < n; i++ )
      items[i] = v[i] ? to_py( TSD, v[i], depth + 1 ) : pyb_none();
   r = pyb_list( n, items );
   for ( i = 0; i < n; i++ )
      pyb_release( items[i] );
   FreeTSD( items );
   return py_check( TSD, r );
}

/* rows of a multi-dimensional array as nested lists */
static pyref py_array_dim( tsd_t *TSD, oo_object *a, int dim, long *pos, int depth )
{
   long n = a->u.a.dims[dim], i;
   pyref *items = (pyref *) MallocTSD( ( n + 1 ) * sizeof( pyref ) ), r;

   for ( i = 0; i < n; i++ )
   {
      if ( dim + 1 < a->u.a.ndims )
         items[i] = py_array_dim( TSD, a, dim + 1, pos, depth + 1 );
      else
      {
         const streng *v = *pos < a->u.a.size ? a->u.a.v[*pos] : NULL;
         items[i] = v ? to_py( TSD, v, depth + 1 ) : pyb_none();
         ( *pos )++;
      }
   }
   r = pyb_list( n, items );
   for ( i = 0; i < n; i++ )
      pyb_release( items[i] );
   FreeTSD( items );
   return py_check( TSD, r );
}

static pyref py_dict_of( tsd_t *TSD, streng **keys, const streng **vals, long n, int depth )
{
   pyref *k = (pyref *) MallocTSD( ( n + 1 ) * sizeof( pyref ) );
   pyref *v = (pyref *) MallocTSD( ( n + 1 ) * sizeof( pyref ) ), r;
   long i;

   for ( i = 0; i < n; i++ )
   {
      k[i] = pyb_str( keys[i]->value, keys[i]->len );
      v[i] = vals[i] ? to_py( TSD, vals[i], depth + 1 ) : pyb_none();
   }
   r = pyb_dict( n, k, v );
   for ( i = 0; i < n; i++ )
   {
      pyb_release( k[i] );
      pyb_release( v[i] );
   }
   FreeTSD( k );
   FreeTSD( v );
   return py_check( TSD, r );
}

/* the stem in box b as dict, tails in DO OVER order */
static pyref py_stem_dict( tsd_t *TSD, variableptr b, int depth )
{
   stemlist l;
   const streng **v;
   long i;
   pyref r;

   l.t = l.v = NULL;
   l.n = l.cap = 0;
   oo_stem_each( TSD, b, stemlist_cb, &l );
   if ( l.n )
      qsort( l.t, l.n, sizeof( streng * ), cmp_tails );
   v = (const streng **) MallocTSD( ( l.n + 1 ) * sizeof( streng * ) );
   for ( i = 0; i < l.n; i++ )
      v[i] = oo_stem_get( TSD, b, l.t[i] );
   r = py_dict_of( TSD, l.t, v, l.n, depth );
   for ( i = 0; i < l.n; i++ )
      Free_stringTSD( l.t[i] );
   if ( l.t )
      FreeTSD( l.t );
   FreeTSD( (void *) v );
   return r;
}

/* an argument; q is its parameter box when the caller passed a stem ("s.") */
static pyref py_arg( tsd_t *TSD, const streng *v, paramboxptr q )
{
   if ( q && q->stempool && q->stemname )
      return py_stem_dict( TSD, oo_stem_box( TSD, q->stempool, q->stemname ), 0 );
   return v ? to_py( TSD, v, 0 ) : pyb_none();
}

static pyref to_py( tsd_t *TSD, const streng *s, int depth )
{
   oo_object *o;
   pyref r;

   if ( depth > 100 )
      exiterror( ERR_EXECUTION_ERROR, 14, "Rexx value nested too deeply for Python" );
   if ( s == NULL )
      return pyb_none();
   o = obj_of( TSD, s );
   if ( o == NULL )
   {
      if ( ( r = pyb_number( s->value, s->len ) ) >= 0 )
         return r;
      return pyb_str( s->value, s->len );
   }
   switch ( o->kind )
   {
      case OK_PYOBJ:
         return pyb_dup( o->u.pyref );
      case OK_BLOCK:
         /* a Python callable that runs the block; Python may keep it */
         o->gcroot = 1;
         return pyb_rexx_callable( (long) o->id );
      case OK_NIL:
         return pyb_none();
      case OK_ARRAY:
      case OK_QUEUE:
         if ( ARR_MULTI( o ) )
         {
            long pos = 0;
            return py_array_dim( TSD, o, 0, &pos, depth );
         }
         return py_list_of( TSD, (const streng **) o->u.a.v, o->u.a.size, depth );
      case OK_CONS:
      {
         long n;
         const streng **v = list_items( TSD, o, &n );
         r = py_list_of( TSD, v, n, depth );
         FreeTSD( (void *) v );
         return r;
      }
      case OK_DIRECTORY:
      case OK_TABLE:
      {
         long n = o->u.h->count, i = 0;
         streng **k = (streng **) MallocTSD( ( n + 1 ) * sizeof( streng * ) );
         const streng **v = (const streng **) MallocTSD( ( n + 1 ) * sizeof( streng * ) );
         oo_entry *e;
         for ( e = o->u.h->first; e && i < n; e = e->onext, i++ )
         {
            k[i] = oo_stringify( TSD, e->key );
            if ( k[i] == NULL )
               k[i] = Str_dupTSD( e->key );
            v[i] = e->value;
         }
         r = py_dict_of( TSD, k, v, i, depth );
         while ( i-- > 0 )
            Free_stringTSD( k[i] );
         FreeTSD( k );
         FreeTSD( (void *) v );
         return r;
      }
      case OK_STEM:
         return py_stem_dict( TSD, stem_box( TSD, o ), depth );
      default:
      {
         streng *t = oo_stringify( TSD, s );
         if ( t == NULL )
            t = Str_dupTSD( s );
         r = pyb_str( t->value, t->len );
         Free_stringTSD( t );
         return r;
      }
   }
}

/* a Python result as Rexx value; takes over r */
static streng *from_py( tsd_t *TSD, pyref r )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   streng *s;
   char *t;
   long len;

   py_check( TSD, r );
   switch ( pyb_kind( r ) )
   {
      case PYK_NONE:
         pyb_release( r );
         return hdup( TSD, ot->nil );
      case PYK_BOOL:
         s = mkbool( TSD, pyb_truth( r ) == 1 );
         pyb_release( r );
         return s;
      case PYK_INT:
      case PYK_FLOAT:
      case PYK_STR:
      case PYK_BYTES:
         t = pyb_text( r, &len );
         pyb_release( r );
         if ( t == NULL )
            py_fail( TSD );
         s = Str_makeTSD( len );
         memcpy( s->value, t, len );
         s->len = len;
         free( t );
         return s;
      default:
         return py_wrap( TSD, r );
   }
}

/* deep conversion: lists, tuples and sets to Arrays, dicts to Directories */
static streng *py_to_rexx( tsd_t *TSD, pyref r, int depth )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   int kind = pyb_kind( r );
   pyref *k, *v;
   long n, i;

   if ( depth > 100 )
      exiterror( ERR_EXECUTION_ERROR, 14, "Python value nested too deeply for Rexx" );
   if ( kind == PYK_LIST || kind == PYK_TUPLE || kind == PYK_SET )
   {
      oo_object *arr = new_object( TSD, ot->cArray, OK_ARRAY );
      n = pyb_items( r, &v );
      pyb_release( r );
      if ( n < 0 )
         py_fail( TSD );
      for ( i = 0; i < n; i++ )
         oo_array_put( TSD, arr, i + 1, py_to_rexx( TSD, v[i], depth + 1 ) );
      free( v );
      return hdup( TSD, arr );
   }
   if ( kind == PYK_DICT || ( kind == PYK_OTHER && pyb_is_mapping( r ) == 1 ) )
   {
      oo_object *d = new_object( TSD, ot->cDirectory, OK_DIRECTORY );
      d->u.h = (oo_hash *) MallocTSD( sizeof( oo_hash ) );
      memset( d->u.h, 0, sizeof( oo_hash ) );
      n = pyb_dict_items( r, &k, &v );
      pyb_release( r );
      if ( n < 0 )
         py_fail( TSD );
      for ( i = 0; i < n; i++ )
      {
         long len;
         char *t = pyb_text( k[i], &len );
         streng *key;
         pyb_release( k[i] );
         if ( t == NULL )
            py_fail( TSD );
         key = Str_makeTSD( len );
         memcpy( key->value, t, len );
         key->len = len;
         free( t );
         h_put( TSD, d->u.h, key, py_to_rexx( TSD, v[i], depth + 1 ) );
         Free_stringTSD( key );
      }
      free( k );
      free( v );
      return hdup( TSD, d );
   }
   return from_py( TSD, r );
}

static oo_object *py_self( tsd_t *TSD, oo_call *c )
{
   if ( c->self == NULL || c->self->kind != OK_PYOBJ )
      err_nomethod( TSD, c->selfstr, c->name );
   py_start( TSD );
   return c->self;
}

/* the arguments from..argc-1 as new Python references */
static pyref *py_args( tsd_t *TSD, oo_call *c, int from, long *n )
{
   pyref *a = (pyref *) MallocTSD( ( c->argc + 1 ) * sizeof( pyref ) );
   int i;
   int pend = ( c->argv == ot_pend_argv( TSD ) );

   *n = 0;
   for ( i = from; i < c->argc; i++ )
      a[( *n )++] = py_arg( TSD, c->argv[i], pend ? ot_pend_box( TSD, i ) : NULL );
   return a;
}

/* the items of an Array (UNKNOWN's second argument) as Python references */
static pyref *py_array_args( tsd_t *TSD, const streng *arr, long *n )
{
   oo_object *a = obj_of( TSD, arr );
   pyref *v;
   long i;

   *n = 0;
   if ( a == NULL || ( a->kind != OK_ARRAY && a->kind != OK_QUEUE ) )
      return (pyref *) MallocTSD( sizeof( pyref ) );
   v = (pyref *) MallocTSD( ( a->u.a.size + 1 ) * sizeof( pyref ) );
   for ( i = 0; i < a->u.a.size; i++ )
   {
      /* UNKNOWN gets the arguments in an array; stems come from the send */
      paramboxptr q = ( (oo_tsd *) TSD->oo_tsd )->pend_argv ? ot_pend_box( TSD, i ) : NULL;
      if ( q && !( q->value && a->u.a.v[i] && str_eq( q->value, a->u.a.v[i] ) ) )
         q = NULL;
      v[i] = py_arg( TSD, a->u.a.v[i], q );
   }
   *n = a->u.a.size;
   return v;
}

static void py_free_args( pyref *a, long n )
{
   long i;
   for ( i = 0; i < n; i++ )
      pyb_release( a[i] );
}

static char *py_cname( tsd_t *TSD, const streng *s )
{
   char *p = (char *) MallocTSD( s->len + 1 );
   memcpy( p, s->value, s->len );
   p[s->len] = '\0';
   return p;
}

/* obj~name(args): the attribute name, called if it is callable */
static streng *m_py_unknown( tsd_t *TSD, oo_call *c )
{
   oo_object *o = py_self( TSD, c );
   const streng *nm = need_arg( TSD, c, 0 );
   char *name = py_cname( TSD, nm );
   pyref *a, r;
   long n;

   a = py_array_args( TSD, opt_arg( c, 1 ), &n );
   if ( nm->len > 1 && name[nm->len - 1] == '=' )
   {
      int rc;
      name[nm->len - 1] = '\0';
      if ( n == 0 )
         a[n++] = pyb_none();
      rc = pyb_setattr( o->u.pyref, name, a[0] );
      py_free_args( a, n );
      FreeTSD( a );
      FreeTSD( name );
      if ( rc != 0 )
         py_fail( TSD );
      return NULL;
   }
   r = pyb_send( o->u.pyref, name, n, a );
   py_free_args( a, n );
   FreeTSD( a );
   FreeTSD( name );
   return from_py( TSD, r );
}

static streng *m_py_string( tsd_t *TSD, oo_call *c )
{
   oo_object *o = py_self( TSD, c );
   long len;
   char *t = pyb_text( o->u.pyref, &len );
   streng *s;

   if ( t == NULL )
      py_fail( TSD );
   s = Str_makeTSD( len );
   memcpy( s->value, t, len );
   s->len = len;
   free( t );
   return s;
}

static streng *m_py_type( tsd_t *TSD, oo_call *c )
{
   oo_object *o = py_self( TSD, c );
   char *t = pyb_typename( o->u.pyref );
   streng *s;

   if ( t == NULL )
      py_fail( TSD );
   s = Str_creTSD( t );
   free( t );
   return s;
}

/* at(key...) and []: o[key] or o[k1, k2, ...] */
static streng *m_py_at( tsd_t *TSD, oo_call *c )
{
   oo_object *o = py_self( TSD, c );
   long n;
   pyref *k = py_args( TSD, c, 0, &n ), r;

   r = pyb_getitem( o->u.pyref, n, k );
   py_free_args( k, n );
   FreeTSD( k );
   return from_py( TSD, r );
}

/* put(value, key...) and []= */
static streng *m_py_put( tsd_t *TSD, oo_call *c )
{
   oo_object *o = py_self( TSD, c );
   long n;
   pyref v = to_py( TSD, need_arg( TSD, c, 0 ), 0 );
   pyref *k = py_args( TSD, c, 1, &n );
   int rc = pyb_setitem( o->u.pyref, n, k, v );

   py_free_args( k, n );
   FreeTSD( k );
   pyb_release( v );
   if ( rc != 0 )
      py_fail( TSD );
   return NULL;
}

static streng *m_py_makearray( tsd_t *TSD, oo_call *c )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *o = py_self( TSD, c );
   oo_object *arr = new_object( TSD, ot->cArray, OK_ARRAY );
   pyref *v;
   long n = pyb_items( o->u.pyref, &v ), i;

   if ( n < 0 )
      py_fail( TSD );
   for ( i = 0; i < n; i++ )
      oo_array_put( TSD, arr, i + 1, from_py( TSD, v[i] ) );
   free( v );
   return hdup( TSD, arr );
}

static streng *m_py_torexx( tsd_t *TSD, oo_call *c )
{
   oo_object *o = py_self( TSD, c );
   return py_to_rexx( TSD, pyb_dup( o->u.pyref ), 0 );
}

/* toStem(s.): a dict fills s.KEY (uppercased like a tail), anything else s.0 and s.1..s.n */
static streng *m_py_tostem( tsd_t *TSD, oo_call *c )
{
   oo_object *o = py_self( TSD, c );
   paramboxptr q = ( c->argc > 0 && c->argv == ot_pend_argv( TSD ) ) ? ot_pend_box( TSD, 0 ) : NULL;
   pyref *k = NULL, *v = NULL;
   long n, i;

   if ( q == NULL || q->stempool == NULL || q->stemname == NULL )
      exiterror( ERR_EXECUTION_ERROR, 14, "TypeError: toStem needs a stem as argument, like s." );
   if ( pyb_is_mapping( o->u.pyref ) == 1 )
   {
      n = pyb_dict_items( o->u.pyref, &k, &v );
      if ( n < 0 )
         py_fail( TSD );
      for ( i = 0; i < n; i++ )
      {
         streng *t = from_py( TSD, k[i] );
         streng *t2 = oo_stringify( TSD, t );
         if ( t2 )
         {
            Free_stringTSD( t );
            t = t2;
         }
         mem_upper( t->value, t->len );   /* u.name finds the key "name" */
         oo_stem_put( TSD, q->stempool, q->stemname, t, from_py( TSD, v[i] ) );
         Free_stringTSD( t );
      }
   }
   else
   {
      n = pyb_items( o->u.pyref, &v );
      if ( n < 0 )
         py_fail( TSD );
      for ( i = 0; i < n; i++ )
      {
         streng *t = mkint( TSD, i + 1 );
         oo_stem_put( TSD, q->stempool, q->stemname, t, from_py( TSD, v[i] ) );
         Free_stringTSD( t );
      }
      {
         streng *t = mkint( TSD, 0 );
         oo_stem_put( TSD, q->stempool, q->stemname, t, mkint( TSD, n ) );
         Free_stringTSD( t );
      }
   }
   if ( k )
      free( k );
   if ( v )
      free( v );
   return mkint( TSD, n );
}

static streng *m_py_getattr( tsd_t *TSD, oo_call *c )
{
   oo_object *o = py_self( TSD, c );
   char *name = py_cname( TSD, need_arg( TSD, c, 0 ) );
   pyref r = pyb_getattr( o->u.pyref, name );

   FreeTSD( name );
   return from_py( TSD, r );
}

static streng *m_py_setattr( tsd_t *TSD, oo_call *c )
{
   oo_object *o = py_self( TSD, c );
   char *name = py_cname( TSD, need_arg( TSD, c, 0 ) );
   pyref v = to_py( TSD, need_arg( TSD, c, 1 ), 0 );
   int rc = pyb_setattr( o->u.pyref, name, v );

   FreeTSD( name );
   pyb_release( v );
   if ( rc != 0 )
      py_fail( TSD );
   return NULL;
}

/* call(args...): the object itself called */
static streng *m_py_call( tsd_t *TSD, oo_call *c )
{
   oo_object *o = py_self( TSD, c );
   long n;
   pyref *a = py_args( TSD, c, 0, &n ), r;

   r = pyb_call( o->u.pyref, n, a );
   py_free_args( a, n );
   FreeTSD( a );
   return from_py( TSD, r );
}

/* invoke(name, args...): a method by its exact name (copy, items, ...) */
static streng *m_py_invoke( tsd_t *TSD, oo_call *c )
{
   oo_object *o = py_self( TSD, c );
   char *name = py_cname( TSD, need_arg( TSD, c, 0 ) );
   long n;
   pyref *a = py_args( TSD, c, 1, &n ), r;

   r = pyb_send( o->u.pyref, name, n, a );
   py_free_args( a, n );
   FreeTSD( a );
   FreeTSD( name );
   return from_py( TSD, r );
}

/* operators: a + b, a = b, -a, ... with Python's meaning */
static streng *m_py_operator( tsd_t *TSD, oo_call *c )
{
   oo_object *o = py_self( TSD, c );
   char *op = py_cname( TSD, c->name );
   pyref b = -1, r;

   if ( c->argc > 0 && c->argv[0] )
      b = to_py( TSD, c->argv[0], 0 );
   r = pyb_op( op, o->u.pyref, b );
   FreeTSD( op );
   if ( b >= 0 )
      pyb_release( b );
   return from_py( TSD, r );
}

/* .PyObject~new(value): value converted to Python */
static streng *m_py_new( tsd_t *TSD, oo_call *c )
{
   py_start( TSD );
   return py_wrap( TSD, py_check( TSD, to_py( TSD, opt_arg( c, 0 ), 0 ) ) );
}

/* .Python~import(name) */
static streng *m_python_import( tsd_t *TSD, oo_call *c )
{
   char *name;
   pyref r;

   py_start( TSD );
   name = py_cname( TSD, need_arg( TSD, c, 0 ) );
   r = pyb_import( name );
   FreeTSD( name );
   return from_py( TSD, r );
}

/* .Python~eval(expression [, names]) */
static streng *m_python_eval( tsd_t *TSD, oo_call *c )
{
   const streng *src;
   pyref names = -1, r;

   py_start( TSD );
   src = need_arg( TSD, c, 0 );
   if ( opt_arg( c, 1 ) )
   {
      names = to_py( TSD, opt_arg( c, 1 ), 0 );
      if ( pyb_kind( names ) != PYK_DICT )
         exiterror( ERR_EXECUTION_ERROR, 14,
                    "TypeError: the names for eval must be a Directory or Stem" );
   }
   r = pyb_eval( src->value, src->len, names );
   if ( names >= 0 )
      pyb_release( names );
   return from_py( TSD, r );
}

/* .Python~exec(statements) */
static streng *m_python_exec( tsd_t *TSD, oo_call *c )
{
   const streng *src;

   py_start( TSD );
   src = need_arg( TSD, c, 0 );
   if ( pyb_exec( src->value, src->len ) != 0 )
      py_fail( TSD );
   return NULL;
}

/* .Python~str(s): s as Python str, even when it looks like a number */
static streng *m_python_str( tsd_t *TSD, oo_call *c )
{
   const streng *s;
   streng *t;
   pyref r;

   py_start( TSD );
   s = need_arg( TSD, c, 0 );
   t = oo_stringify( TSD, s );
   r = t ? pyb_str( t->value, t->len ) : pyb_str( s->value, s->len );
   if ( t )
      Free_stringTSD( t );
   return py_wrap( TSD, py_check( TSD, r ) );
}

/* .Python~bytes(s): s as Python bytes */
static streng *m_python_bytes( tsd_t *TSD, oo_call *c )
{
   const streng *s;

   py_start( TSD );
   s = need_arg( TSD, c, 0 );
   return py_wrap( TSD, py_check( TSD, pyb_bytes( s->value, s->len ) ) );
}

/* .Python~kw(name, value, ...): keyword arguments, given last in a call */
static streng *m_python_kw( tsd_t *TSD, oo_call *c )
{
   long n = c->argc / 2, i;
   pyref *k, *v, r;

   py_start( TSD );
   if ( c->argc % 2 )
      exiterror( ERR_EXECUTION_ERROR, 14,
                 "TypeError: kw() needs pairs of name and value" );
   k = (pyref *) MallocTSD( ( n + 1 ) * sizeof( pyref ) );
   v = (pyref *) MallocTSD( ( n + 1 ) * sizeof( pyref ) );
   for ( i = 0; i < n; i++ )
   {
      const streng *nm = need_arg( TSD, c, 2 * i );
      streng *low = Str_dupTSD( nm );
      mem_lower( low->value, low->len );
      k[i] = pyb_str( low->value, low->len );
      Free_stringTSD( low );
      v[i] = to_py( TSD, c->argv[2 * i + 1], 0 );
   }
   r = pyb_kwargs( n, k, v );
   py_free_args( k, n );
   py_free_args( v, n );
   FreeTSD( k );
   FreeTSD( v );
   return py_wrap( TSD, py_check( TSD, r ) );
}

/* .Python~name(args): a global of __main__ or a built-in (len, range, ...) */
static streng *m_python_unknown( tsd_t *TSD, oo_call *c )
{
   char *name;
   pyref *a, f, r;
   long n;

   py_start( TSD );
   name = py_cname( TSD, need_arg( TSD, c, 0 ) );
   f = pyb_global( name );
   FreeTSD( name );
   py_check( TSD, f );
   a = py_array_args( TSD, opt_arg( c, 1 ), &n );
   if ( n == 0 && pyb_kind( f ) != PYK_OTHER )
      r = f;   /* a value, not a function */
   else
   {
      r = pyb_call( f, n, a );
      pyb_release( f );
   }
   py_free_args( a, n );
   FreeTSD( a );
   return from_py( TSD, r );
}

/*
 * Python calls a Rexx block. The block runs protected (oo_try_call): an
 * error or EXIT in it must first leave the Python interpreter. An error
 * becomes a Python RuntimeError, and so error 98.14 in the Rexx code
 * that called Python; EXIT ends the program after that.
 */
typedef struct {
   oo_object *block;
   long n;
   const pyref *args;
   pyref result;
} py_cbdata;

static void py_cb_run( tsd_t *TSD, void *data )
{
   py_cbdata *d = (py_cbdata *) data;
   const streng **argv = (const streng **) MallocTSD( ( d->n + 1 ) * sizeof( streng * ) );
   streng *r, *h = hdup( TSD, d->block );
   long i;

   for ( i = 0; i < d->n; i++ )
      argv[i] = from_py( TSD, pyb_dup( d->args[i] ) );
   r = fn_call( TSD, h, (int) d->n, argv, 0 );
   d->result = r ? to_py( TSD, r, 0 ) : pyb_none();
   if ( r )
      Free_stringTSD( r );
   for ( i = 0; i < d->n; i++ )
      Free_stringTSD( (streng *) argv[i] );
   FreeTSD( (void *) argv );
   Free_stringTSD( h );
}

static int py_callback( long id, long n, const pyref *args, pyref *result,
                        char *err, int errlen )
{
   extern int oo_try_call( tsd_t *TSD, void (*fn)( tsd_t *, void * ), void *data );
   extern const char *oo_captured_error( int *no, int *sub );
   tsd_t *TSD = py_tsd;
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   py_cbdata d;
   int status, no, sub;
   const char *text;

   if ( id <= 0 || (unsigned long) id > ot->nobjs || ot->objs[id] == NULL
     || ot->objs[id]->kind != OK_BLOCK )
   {
      strncpy( err, "the Rexx block no longer exists", errlen - 1 );
      return -1;
   }
   d.block = ot->objs[id];
   d.n = n;
   d.args = args;
   d.result = -1;
   status = oo_try_call( TSD, py_cb_run, &d );
   if ( status == 0 )
   {
      *result = d.result;
      return 0;
   }
   if ( status == 1 )
   {
      py_pending_exit = TSD->systeminfo->result;
      TSD->systeminfo->result = NULL;
      py_exit_pending = 1;
      strncpy( err, "EXIT in a Rexx block", errlen - 1 );
      return -1;
   }
   text = oo_captured_error( &no, &sub );
   if ( strncmp( text, "Error ", 6 ) == 0 )
      sprintf( err, "%.600s", text );      /* "Error 42.3: ..." */
   else if ( sub )
      sprintf( err, "Error %d.%d: %.500s", no, sub, text );
   else
      sprintf( err, "Error %d: %.500s", no, text );
   return -1;
}

static void init_python( tsd_t *TSD, oo_tsd *ot )
{
   oo_object *c;

   ot->cPython = c = new_class( TSD, "Python", NULL, ot->cObject, OK_PLAIN );
   add_methods( TSD, c, "IMPORT", m_python_import, MF_CLASS );
   add_methods( TSD, c, "EVAL", m_python_eval, MF_CLASS );
   add_methods( TSD, c, "EXEC", m_python_exec, MF_CLASS );
   add_methods( TSD, c, "STR", m_python_str, MF_CLASS );
   add_methods( TSD, c, "BYTES", m_python_bytes, MF_CLASS );
   add_methods( TSD, c, "KW", m_python_kw, MF_CLASS );
   add_methods( TSD, c, "UNKNOWN", m_python_unknown, MF_CLASS );

   ot->cPyObject = c = new_class( TSD, "PyObject", NULL, ot->cObject, OK_PYOBJ );
   add_methods( TSD, c, "NEW", m_py_new, MF_CLASS );
   add_methods( TSD, c, "UNKNOWN", m_py_unknown, 0 );
   add_methods( TSD, c, "STRING MAKESTRING", m_py_string, 0 );
   add_methods( TSD, c, "PYTYPE", m_py_type, 0 );
   add_methods( TSD, c, "[] AT", m_py_at, 0 );
   add_methods( TSD, c, "[]= PUT", m_py_put, 0 );
   add_methods( TSD, c, "MAKEARRAY", m_py_makearray, 0 );
   add_methods( TSD, c, "TOREXX", m_py_torexx, 0 );
   add_methods( TSD, c, "TOSTEM", m_py_tostem, 0 );
   add_methods( TSD, c, "GETATTR", m_py_getattr, 0 );
   add_methods( TSD, c, "SETATTR", m_py_setattr, 0 );
   add_methods( TSD, c, "CALL", m_py_call, 0 );
   add_methods( TSD, c, "INVOKE", m_py_invoke, 0 );
   add_methods( TSD, c, "+ - * / % // ** = == \\= \\== > < >= <= >> << >>= <<= & | && \\",
                m_py_operator, 0 );
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
   ot->cStem = new_class( TSD, "Stem", NULL, ot->cObject, OK_STEM );
   init_lisp( TSD, ot );
   init_blocks( TSD, ot );
   init_more_collections( TSD, ot );
   init_condition_classes( TSD, ot );
   init_python( TSD, ot );
   ws_nbuiltin = ot->nclasses;   /* classes after these are the program's */

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
   add_methods( TSD, ot->cObject, "||", m_obj_concat, 0 );
   {
      streng *blank = Str_creTSD( " " ), *none = Str_creTSD( "" );
      add_method( TSD, ot->cObject, NULL, blank, m_obj_concat, 0 );
      add_method( TSD, ot->cObject, NULL, none, m_obj_concat, 0 );
      Free_stringTSD( blank );
      Free_stringTSD( none );
   }

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

   add_methods( TSD, ot->cStem, "INIT", m_stem_init, 0 );
   add_methods( TSD, ot->cStem, "[] AT", m_stem_at, 0 );
   add_methods( TSD, ot->cStem, "[]= PUT", m_stem_put, 0 );
   add_methods( TSD, ot->cStem, "REMOVE", m_stem_remove, 0 );
   add_methods( TSD, ot->cStem, "HASINDEX", m_stem_hasindex, 0 );
   add_methods( TSD, ot->cStem, "ITEMS", m_stem_items, 0 );
   add_methods( TSD, ot->cStem, "ISEMPTY", m_stem_isempty, 0 );
   add_methods( TSD, ot->cStem, "EMPTY", m_stem_empty, 0 );
   add_methods( TSD, ot->cStem, "ALLINDEXES MAKEARRAY ALLITEMS SUPPLIER", m_stem_lists, 0 );
   add_methods( TSD, ot->cStem, "STRING", m_stem_string, 0 );
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
      /* ::EXTENSION may replace a method: the newest is found first */
      if ( find_in( list, u ) && ( (oo_tsd *) TSD->oo_tsd )->extending != cls )
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

   if ( cd->resolved == 1 )
      return;
   if ( cd->resolved == -1 )
      exiterror( ERR_EXECUTION_ERROR, 10, tmpstr_of( TSD, cd->name ) );
   cd->resolved = -1;   /* in progress: detects cycles */
   if ( cd->supername )
   {
      s = class_by_id( ot, cd->supername );
      if ( s == NULL )
         exiterror( ERR_EXECUTION_ERROR, 1, tmpstr_of( TSD, cd->supername ) );
      if ( s == c )
         exiterror( ERR_EXECUTION_ERROR, 10, tmpstr_of( TSD, cd->name ) );
      if ( s->u.c->resolved != 1 )
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
         if ( s == c )
            exiterror( ERR_EXECUTION_ERROR, 10, tmpstr_of( TSD, cd->name ) );
         if ( s->u.c->resolved != 1 )
            resolve_class( TSD, s );
         cd->inherits[i] = s;
      }
   }
   cd->resolved = 1;
   invalidate_mros( ot, TSD );
}

static void process_directives( tsd_t *TSD, oo_program *prog, nodeptr first )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_object *cls = NULL, **mine = NULL, **pend_cls = NULL;
   streng **pend_name = NULL;
   nodeptr *pend_dir = NULL;
   int nmine = 0, i, k, nw, npend = 0;
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
      if ( str_eqc( d->name, "CLASS" ) || str_eqc( d->name, "ROUTINE" ) )
         ot->extending = NULL;

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
      else if ( str_eqc( d->name, "EXTENSION" ) )
      {
         /*
          * ::EXTENSION class [INHERIT mixin ...]: the following ::METHOD,
          * ::ATTRIBUTE and ::CONSTANT directives add to (or replace
          * methods of) an existing class, also a built-in one.
          */
         streng *id = upper_dup( TSD, w[0] );
         cls = NULL;
         for ( i = 0; i < nmine && cls == NULL; i++ )
            if ( str_eq( mine[i]->u.c->id, id ) )
               cls = mine[i];
         if ( cls == NULL )
            cls = class_by_id( ot, id );
         Free_stringTSD( id );
         if ( cls == NULL || cls == ot->cNilList )
            exiterror( ERR_TRANSLATION_ERROR, 10, tmpstr_of( TSD, w[0] ) );
         for ( k = 1; k < nw; k++ )
         {
            if ( opt_is( w[k], "INHERIT" ) )
            {
               /* the mixins are added when all classes of the file exist */
               for ( k++; k < nw; k++ )
               {
                  if ( npend % 16 == 0 )
                  {
                     oo_object **nc = (oo_object **) MallocTSD( ( npend + 16 ) * sizeof( oo_object * ) );
                     streng **nn = (streng **) MallocTSD( ( npend + 16 ) * sizeof( streng * ) );
                     nodeptr *nd = (nodeptr *) MallocTSD( ( npend + 16 ) * sizeof( nodeptr ) );
                     if ( npend )
                     {
                        memcpy( nc, pend_cls, npend * sizeof( oo_object * ) );
                        memcpy( nn, pend_name, npend * sizeof( streng * ) );
                        memcpy( nd, pend_dir, npend * sizeof( nodeptr ) );
                        FreeTSD( pend_cls );
                        FreeTSD( pend_name );
                        FreeTSD( pend_dir );
                     }
                     pend_cls = nc;
                     pend_name = nn;
                     pend_dir = nd;
                  }
                  pend_cls[npend] = cls;
                  pend_dir[npend] = d;
                  pend_name[npend++] = upper_dup( TSD, w[k] );
               }
            }
            else
               bad_option( TSD, d, w[k] );
         }
         ot->extending = cls;
         continue;
      }
      else
         exiterror( ERR_TRANSLATION_ERROR, 1, tmpstr_of( TSD, d->name ) );
   }
   ot->extending = NULL;

   /* ::EXTENSION ... INHERIT mixin */
   for ( k = 0; k < npend; k++ )
   {
      oo_object *mx = class_by_id( ot, pend_name[k] ), *target = pend_cls[k];
      oo_classdata *cd = target->u.c;
      oo_object **ni;

      TSD->currentnode = pend_dir[k];
      if ( mx == NULL )
         exiterror( ERR_TRANSLATION_ERROR, 10, tmpstr_of( TSD, pend_name[k] ) );
      if ( mx->u.c->resolved != 1 )
         resolve_class( TSD, mx );
      ni = (oo_object **) MallocTSD( ( cd->ninherits + 1 ) * sizeof( oo_object * ) );
      if ( cd->ninherits )
      {
         memcpy( ni, cd->inherits, cd->ninherits * sizeof( oo_object * ) );
         FreeTSD( cd->inherits );
      }
      ni[cd->ninherits++] = mx;
      cd->inherits = ni;
      Free_stringTSD( pend_name[k] );
   }
   if ( npend )
   {
      FreeTSD( pend_cls );
      FreeTSD( pend_name );
      FreeTSD( pend_dir );
      invalidate_mros( ot, TSD );
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
void oo_exec_init( tsd_t *TSD, nodeptr thisptr )
{
   oo_tsd *ot = get_ot( TSD );
   oo_program *prog;

   if ( thisptr->u.nonansi )      /* already done for this tree */
      return;
   thisptr->u.nonansi = 1;

   if ( ot->loading )
   {
      prog = ot->loading;
      ot->loading = NULL;
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

/*
 * Libraries built into the interpreter, used by ::REQUIRES when no file of
 * that name is found. The source is lib/lisp.cls of the distribution.
 */
static const char lisp_library[] =
   "/* lisp.cls - LISP-style list functions for Regina Rexx OO\n"
   " *\n"
   " * ::requires 'lisp' loads this library. It is built into the\n"
   " * interpreter, so no file is needed; a file named lisp.cls next to the\n"
   " * program or in REGINA_MACROS takes precedence.\n"
   " *\n"
   " * Lists are made of Cons cells; .nil is the empty list. The functions\n"
   " * call the methods of the Cons class. Names that are Rexx built-in\n"
   " * functions get an L in front: LLENGTH, LREVERSE.\n"
   " */\n"
   "::routine cons public           /* (cons a d) */\n"
   "  use arg a, d\n"
   "  return .cons~new(a, d)\n"
   "::routine car public\n"
   "  use arg x\n"
   "  return x~car\n"
   "::routine cdr public\n"
   "  use arg x\n"
   "  return x~cdr\n"
   "::routine caar public\n"
   "  use arg x\n"
   "  return x~car~car\n"
   "::routine cadr public\n"
   "  use arg x\n"
   "  return x~cdr~car\n"
   "::routine cdar public\n"
   "  use arg x\n"
   "  return x~car~cdr\n"
   "::routine cddr public\n"
   "  use arg x\n"
   "  return x~cdr~cdr\n"
   "::routine caddr public\n"
   "  use arg x\n"
   "  return x~cdr~cdr~car\n"
   "::routine list public           /* (list a b c) */\n"
   "  r = .nil\n"
   "  do i = arg() to 1 by -1\n"
   "    r = .cons~new(arg(i), r)\n"
   "  end\n"
   "  return r\n"
   "::routine null public           /* 1 for the empty list */\n"
   "  use arg x\n"
   "  return x~isNil\n"
   "::routine atom public           /* 1 for anything but a cell */\n"
   "  use arg x\n"
   "  return \\x~isA(.cons)\n"
   "::routine consp public\n"
   "  use arg x\n"
   "  return x~isA(.cons)\n"
   "::routine listp public\n"
   "  use arg x\n"
   "  return x~isNil | x~isA(.cons)\n"
   "::routine nth public            /* (nth n l), 0-based as in LISP */\n"
   "  use arg n, l\n"
   "  return l~nthcdr(n)~car\n"
   "::routine nthcdr public\n"
   "  use arg n, l\n"
   "  return l~nthcdr(n)\n"
   "::routine llength public        /* LENGTH is a Rexx built-in function */\n"
   "  use arg l\n"
   "  return l~length\n"
   "::routine lreverse public       /* REVERSE is a Rexx built-in function */\n"
   "  use arg l\n"
   "  return l~reverse\n"
   "::routine last public           /* the last cell */\n"
   "  use arg l\n"
   "  return l~last\n"
   "::routine append public         /* (append l1 l2 ...) */\n"
   "  if arg() = 0 then return .nil\n"
   "  r = arg(arg())\n"
   "  do i = arg() - 1 to 1 by -1\n"
   "    r = arg(i)~append(r)\n"
   "  end\n"
   "  return r\n"
   "::routine member public         /* (member x l) */\n"
   "  use arg x, l\n"
   "  return l~member(x)\n"
   "::routine assoc public          /* (assoc key alist) */\n"
   "  use arg k, al\n"
   "  return al~assoc(k)\n"
   "::routine mapcar public         /* (mapcar 'f l) */\n"
   "  use arg f, l\n"
   "  return l~map(f)\n"
   "::routine filter public         /* the items for which f returns 1 */\n"
   "  use arg f, l\n"
   "  return l~filter(f)\n"
   "::routine reduce public         /* (reduce 'f l [initial]) */\n"
   "  use arg f, l, init\n"
   "  if arg(3, 'E') then return l~reduce(f, init)\n"
   "  return l~reduce(f)\n"
   "::routine apply public          /* (apply 'f l) */\n"
   "  use arg f, l\n"
   "  return .cons~apply(f, l)\n"
   "::routine equal public          /* the same structure and atoms */\n"
   "  use arg a, b\n"
   "  if a~isA(.cons) then return a~equal(b)\n"
   "  return a == b\n"
   "::routine eq public             /* the same object, or equal atoms */\n"
   "  use arg a, b\n"
   "  return a == b\n"
   "::routine rplaca public         /* changes the car of a cell */\n"
   "  use arg c, x\n"
   "  c~setCar(x)\n"
   "  return c\n"
   "::routine rplacd public         /* changes the cdr of a cell */\n"
   "  use arg c, x\n"
   "  c~setCdr(x)\n"
   "  return c\n"
   "::routine read public           /* the list written in a string */\n"
   "  use arg s\n"
   "  return .cons~read(s)\n"
   "\n";

/* the workspace session, see lib/session.rexx and rexx --ws */
static const char session_program[] =
   "/* OOREGINA */\n"
   "/*\n"
   " * The workspace session of Regina OO, started with \"rexx --ws [wsid]\".\n"
   " * Lines are Rexx clauses; an expression on its own shows its value, as in\n"
   " * APL (and the environment WS shows commands). Lines that begin with \"::\"\n"
   " * define routines and classes up to an empty line ([]FX). System commands\n"
   " * begin with \")\": )SAVE )LOAD )OUT )IN )VARS )FNS )CLASSES )ERASE )WSID\n"
   " * )CLEAR )SHOW )GC )HELP )OFF. All variables of the session are named\n"
   " * __WS.something, which []NL and []SAVE leave out.\n"
   " */\n"
   "parse arg __ws.arg\n"
   "say 'Regina OO workspace session. )HELP lists the commands, )OFF ends it.'\n"
   "address WS\n"
   "if __ws.arg \\= '' then do\n"
   "   signal on syntax name __wsStartErr\n"
   "   call __wsCommand ')LOAD' __ws.arg\n"
   "end\n"
   "__wsLoop:\n"
   "__ws.eof = 0\n"
   "call on notready name __wsEOF\n"
   "signal on syntax name __wsErr\n"
   "signal on halt name __wsHalt\n"
   "do forever\n"
   "   call charout , '      '\n"
   "   __ws.line = linein()\n"
   "   if __ws.eof = 1 then\n"
   "      exit 0\n"
   "   if strip(__ws.line) == '' then\n"
   "      iterate\n"
   "   if left(strip(__ws.line), 1) == ')' then do\n"
   "      call __wsCommand strip(__ws.line)\n"
   "      iterate\n"
   "   end\n"
   "   if left(strip(__ws.line), 2) == '::' then do\n"
   "      /* a definition: up to an empty line */\n"
   "      __ws.def = __ws.line\n"
   "      do forever\n"
   "         call charout , '    : '\n"
   "         __ws.more = linein()\n"
   "         if strip(__ws.more) == '' | __ws.eof = 1 then\n"
   "            leave\n"
   "         __ws.def = __ws.def || '0a'x || __ws.more\n"
   "      end\n"
   "      say []FX(__ws.def)\n"
   "      iterate\n"
   "   end\n"
   "   if __wsIsInstruction(__ws.line) then\n"
   "      interpret __ws.line\n"
   "   else do\n"
   "      /* an expression: show its value; a message without result runs */\n"
   "      __ws.trying = 1\n"
   "      interpret '__ws.value =' __ws.line\n"
   "      __ws.trying = 0\n"
   "      say __ws.value\n"
   "      drop __ws.value\n"
   "   end\n"
   "end\n"
   "\n"
   "__wsErr:\n"
   "   if __ws.trying = 1 & rc = 91 then do\n"
   "      /* \"obj~method\" that returns nothing: an instruction after all */\n"
   "      __ws.trying = 0\n"
   "      signal on syntax name __wsErr\n"
   "      interpret __ws.line\n"
   "      signal __wsLoop\n"
   "   end\n"
   "   __ws.trying = 0\n"
   "   if condition('D') == '' then\n"
   "      say 'Error' rc':' errortext(rc)\n"
   "   else\n"
   "      say condition('D')\n"
   "   signal __wsLoop\n"
   "\n"
   "__wsStartErr:\n"
   "   say condition('D')\n"
   "   signal __wsLoop\n"
   "\n"
   "__wsEOF:   /* the end of the input ends the session */\n"
   "   __ws.eof = 1\n"
   "   return\n"
   "\n"
   "__wsHalt:\n"
   "   say 'INTERRUPT'\n"
   "   signal __wsLoop\n"
   "\n"
   "__wsCommand:   /* no PROCEDURE: the commands work on the session's variables */\n"
   "   parse arg ')' __ws.cmd __ws.rest\n"
   "   __ws.cmd = translate(__ws.cmd)\n"
   "   __ws.rest = strip(__ws.rest)\n"
   "   select\n"
   "      when abbrev('SAVE', __ws.cmd, 2) then do\n"
   "         if __ws.rest == '' then\n"
   "            say 'SAVED' []SAVE()\n"
   "         else\n"
   "            say 'SAVED' []SAVE(__ws.rest)\n"
   "      end\n"
   "      when abbrev('LOAD', __ws.cmd, 2) then do\n"
   "         call []LOAD __ws.rest\n"
   "         say 'LOADED' []WSID()\n"
   "      end\n"
   "      when abbrev('OUT', __ws.cmd, 2) then do\n"
   "         parse var __ws.rest __ws.file __ws.names\n"
   "         if __ws.names == '' then\n"
   "            say 'WRITTEN' []OUT(__ws.file)\n"
   "         else\n"
   "            say 'WRITTEN' []OUT(__ws.file, __ws.names)\n"
   "      end\n"
   "      when abbrev('IN', __ws.cmd, 2) then do\n"
   "         parse var __ws.rest __ws.file __ws.names\n"
   "         if __ws.names == '' then\n"
   "            say []IN(__ws.file)\n"
   "         else\n"
   "            say []IN(__ws.file, __ws.names)\n"
   "      end\n"
   "      when abbrev('VARS', __ws.cmd, 1) then say []NL(2)\n"
   "      when abbrev('FNS', __ws.cmd, 1) then say []NL(3)\n"
   "      when abbrev('CLASSES', __ws.cmd, 2) then say []NL(9)\n"
   "      when abbrev('ERASE', __ws.cmd, 1) then call []EX __ws.rest\n"
   "      when abbrev('WSID', __ws.cmd, 1) then do\n"
   "         if __ws.rest == '' then\n"
   "            say []WSID()\n"
   "         else\n"
   "            say 'WAS' []WSID(__ws.rest)\n"
   "      end\n"
   "      when abbrev('CLEAR', __ws.cmd, 2) then do\n"
   "         call []CLEAR\n"
   "         say 'CLEAR WS'\n"
   "      end\n"
   "      when abbrev('SHOW', __ws.cmd, 2) then say []CR(__ws.rest)\n"
   "      when abbrev('GC', __ws.cmd, 1) then say []GC() 'objects freed'\n"
   "      when abbrev('OFF', __ws.cmd, 1) | abbrev('CONTINUE', __ws.cmd, 4) then exit 0\n"
   "      when abbrev('HELP', __ws.cmd, 1) then do\n"
   "         say ')SAVE [wsid]          save the workspace in wsid.rxws'\n"
   "         say ')LOAD wsid            clear, then load wsid.rxws'\n"
   "         say ')OUT file [names]     write all or some names to file.rxo'\n"
   "         say ')IN file [names]      read all or some names from a file'\n"
   "         say ')VARS )FNS )CLASSES   list variables, routines, classes'\n"
   "         say ')ERASE names          erase variables, routines, classes'\n"
   "         say ')SHOW name            show the source of a routine or class'\n"
   "         say ')WSID [name]          show or set the workspace name'\n"
   "         say ')CLEAR                empty workspace'\n"
   "         say ')GC                   collect unused objects now'\n"
   "         say ')OFF                  end the session'\n"
   "         say 'A line with an expression shows its value. Lines that start'\n"
   "         say 'with :: define routines and classes, up to an empty line.'\n"
   "      end\n"
   "      otherwise say 'INCORRECT COMMAND'\n"
   "   end\n"
   "   return\n"
   "\n"
   "/* 1 if the line is an instruction or an assignment, 0 for an expression */\n"
   "__wsIsInstruction:\n"
   "   parse arg __ws.l\n"
   "   parse upper var __ws.l __ws.w1 .\n"
   "   if wordpos(__ws.w1, 'ADDRESS ARG CALL DO DROP EXIT IF INTERPRET ITERATE LEAVE',\n"
   "      'NOP NUMERIC OPTIONS PARSE PROCEDURE PULL PUSH QUEUE RETURN SAY SELECT',\n"
   "      'SIGNAL TRACE UPPER USE EXPOSE LOOP END WHEN OTHERWISE THEN ELSE') > 0 then\n"
   "      return 1\n"
   "   /* several clauses; a label */\n"
   "   __ws.q = ''\n"
   "   __ws.depth = 0\n"
   "   do __ws.i = 1 to length(__ws.l)\n"
   "      __ws.c = substr(__ws.l, __ws.i, 1)\n"
   "      if __ws.q \\== '' then do\n"
   "         if __ws.c == __ws.q then __ws.q = ''\n"
   "         iterate\n"
   "      end\n"
   "      select\n"
   "         when __ws.c == \"'\" | __ws.c == '\"' then __ws.q = __ws.c\n"
   "         when __ws.c == '(' | __ws.c == '[' | __ws.c == '{' then __ws.depth = __ws.depth + 1\n"
   "         when __ws.c == ')' | __ws.c == ']' | __ws.c == '}' then __ws.depth = __ws.depth - 1\n"
   "         when __ws.depth > 0 then nop\n"
   "         when __ws.c == ';' then return 1\n"
   "         when __ws.c == ':' & __ws.i > 1 & substr(__ws.l, __ws.i + 1, 1) \\== ':' then\n"
   "            if verify(strip(left(__ws.l, __ws.i - 1)), ,\n"
   "               'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.!?') = 0 then\n"
   "               return 1\n"
   "         when __ws.c == '=' then do\n"
   "            /* \"target = value\": the target has no blanks outside brackets */\n"
   "            __ws.prev = substr(__ws.l, max(__ws.i - 1, 1), 1)\n"
   "            if __ws.i = 1 | pos(__ws.prev, '\\<>=') > 0 then return 0\n"
   "            if substr(__ws.l, __ws.i + 1, 1) == '=' then return 0\n"
   "            return __wsNoBlanks(strip(left(__ws.l, __ws.i - 1)))\n"
   "         end\n"
   "         otherwise nop\n"
   "      end\n"
   "   end\n"
   "   return 0\n"
   "\n"
   "__wsNoBlanks:\n"
   "   parse arg __ws.t\n"
   "   __ws.d = 0\n"
   "   __ws.q = ''\n"
   "   do __ws.k = 1 to length(__ws.t)\n"
   "      __ws.c = substr(__ws.t, __ws.k, 1)\n"
   "      if __ws.q \\== '' then do\n"
   "         if __ws.c == __ws.q then __ws.q = ''\n"
   "         iterate\n"
   "      end\n"
   "      if __ws.c == \"'\" | __ws.c == '\"' then __ws.q = __ws.c\n"
   "      else if pos(__ws.c, '([{') > 0 then __ws.d = __ws.d + 1\n"
   "      else if pos(__ws.c, ')]}') > 0 then __ws.d = __ws.d - 1\n"
   "      else if __ws.c == ' ' & __ws.d = 0 then return 0\n"
   "   end\n"
   "   return __ws.t \\== ''\n"
   "\n";

const char *oo_session_source( void )
{
   return session_program;
}

/* the built-in libraries: lisp (LISP routines) and python (.Python is built in) */
static const char python_library[] = "/* .Python and .PyObject are built-in classes */\n";
static const struct { const char *name; const char *src; } builtin_libs[] = {
   { "LISP", lisp_library }, { "PYTHON", python_library }
};

/* the built-in library for name ("lisp", "lisp.cls", ...), or NULL; *base its name */
static const char *builtin_library_named( const streng *name, const char **base )
{
   int i, k, n;

   for ( i = 0; i < (int) ( sizeof( builtin_libs ) / sizeof( builtin_libs[0] ) ); i++ )
   {
      n = (int) strlen( builtin_libs[i].name );
      if ( name->len != n && !( name->len == n + 4 && name->value[n] == '.'
           && rx_toupper( name->value[n + 1] ) == 'C' && rx_toupper( name->value[n + 2] ) == 'L'
           && rx_toupper( name->value[n + 3] ) == 'S' ) )
         continue;
      for ( k = 0; k < n; k++ )
         if ( rx_toupper( name->value[k] ) != builtin_libs[i].name[k] )
            break;
      if ( k == n )
      {
         if ( base )
            *base = builtin_libs[i].name;
         return builtin_libs[i].src;
      }
   }
   return NULL;
}

static void load_required( tsd_t *TSD, const streng *name, oo_program *from, nodeptr d )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   streng *path, *look;
   oo_program *p;
   const char *bname = NULL;
   const char *builtin = builtin_library_named( name, &bname );

   /*
    * A built-in library is replaced only by a file of its name with .cls
    * (lisp.cls), so that a program lisp.rexx doesn't hide it.
    */
   look = Str_dupTSD( name );
   if ( builtin && name->len == (int) strlen( bname ) )
      look = catc( TSD, look, ".cls" );
   path = find_required( TSD, look, from );
   if ( path == NULL )
   {
      mem_lower( look->value, look->len );
      path = find_required( TSD, look, from );
   }
   Free_stringTSD( look );
   if ( path && builtin && path->len > 4
     && memcmp( path->value + path->len - 4, ".cls", 4 ) != 0
     && memcmp( path->value + path->len - 4, ".CLS", 4 ) != 0 )
   {
      Free_stringTSD( path );
      path = NULL;
   }
   /* a program never loads itself */
   if ( path && from && from->file && str_eq( path, from->file ) )
   {
      Free_stringTSD( path );
      if ( builtin == NULL )
         return;
      path = NULL;
   }
   builtin = path ? NULL : builtin;
   FILE *fp;
   internal_parser_type parsing;
   oo_frame *frame;
   prolog_data pd;

   if ( path == NULL && builtin != NULL )
   {
      char lname[40];
      sprintf( lname, "<built-in %s>", bname );
      mem_lower( lname, (int) strlen( lname ) );
      path = Str_creTSD( lname );
   }
   if ( path == NULL )
      exiterror( ERR_EXECUTION_ERROR, 3, tmpstr_of( TSD, name ) );
   for ( p = ot->programs; p; p = p->next )
      if ( p->file && str_eq( p->file, path ) )
      {
         Free_stringTSD( path );
         return;
      }
   if ( builtin )
   {
      streng *src = Str_creTSD( builtin );
      fetch_string( TSD, src, &parsing );
      Free_stringTSD( src );
   }
   else
   {
      if ( ( fp = fopen( path->value, "r" ) ) == NULL )
         exiterror( ERR_EXECUTION_ERROR, 3, tmpstr_of( TSD, name ) );
      fetch_file( TSD, fp, &parsing );
      fclose( fp );
   }
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
   frame = new_frame( TSD );
   frame->self = NULL;
   frame->scope = NULL;
   frame->labels = &p->labels;
   ot->loading = ( parsing.root && parsing.root->type == X_OO_INIT ) ? p : NULL;
   pd.p = p;
   pd.frame = frame;
   oo_exit_protected( TSD, run_prolog, &pd );
   ot->loading = NULL;
   FreeTSD( frame );
}

/* ------------------------------------------------------------------ */
/* Workspaces: []NL, []FX, []SAVE, []LOAD, []OUT, []IN ...            */
/* ------------------------------------------------------------------ */

/*
 * A workspace in the sense of APL: the variables of the current
 * procedure level (with the objects they refer to) and the routines and
 * classes defined with []FX. []SAVE writes it to a file, []LOAD reads it
 * back after []CLEAR, []OUT and []IN do the same for some or all names
 * without clearing. The file is Rexx-like text:
 *
 *   ::fx NAME ...          a definition, up to ::endfx
 *   ::objects              statements that make the objects
 *   ::var NAME             statements that set the variable NAME
 *   ::wsid NAME
 *
 * Variables named __WS.* belong to the workspace machinery (the session
 * and the loader) and are never listed or saved.
 */

typedef struct oo_wsdef {
   struct oo_wsdef *next;
   streng *name;                /* uppercase                          */
   int kind;                    /* 3 routine, 9 class                 */
   streng *src;
} oo_wsdef;

static oo_wsdef *ws_defs;       /* definitions made with []FX         */
static streng *ws_id;           /* the workspace name                 */

static const streng *q_arg( cparamboxptr p, int i )
{
   while ( p && i-- > 0 )
      p = p->next;
   return p ? p->value : NULL;
}

static int q_argc( cparamboxptr p )
{
   int n = 0, last = 0;
   for ( ; p; p = p->next )
   {
      n++;
      if ( p->value )
         last = n;
   }
   return last;
}

static streng *q_need( tsd_t *TSD, cparamboxptr p, int i, const char *bif )
{
   const streng *v = q_arg( p, i );
   streng *s;
   if ( v == NULL )
      exiterror( ERR_INCORRECT_CALL, 3, bif, i + 1 );
   s = oo_stringify( TSD, v );
   return s ? s : Str_dupTSD( v );
}

static int ws_internal_name( const streng *name )
{
   return name->len >= 4 && rx_toupper( name->value[0] ) == '_' && name->value[1] == '_'
       && rx_toupper( name->value[2] ) == 'W' && rx_toupper( name->value[3] ) == 'S';
}

/* blank-separated, sorted list of names */
typedef struct { streng **v; long n, cap; } namelist;

static void nl_add( tsd_t *TSD, namelist *l, const streng *name )
{
   long i;
   for ( i = 0; i < l->n; i++ )
      if ( str_eq( l->v[i], name ) )
         return;
   if ( l->n >= l->cap )
   {
      long nc = l->cap ? l->cap * 2 : 32;
      streng **nv = (streng **) MallocTSD( nc * sizeof( streng * ) );
      if ( l->n )
         memcpy( nv, l->v, l->n * sizeof( streng * ) );
      if ( l->v )
         FreeTSD( l->v );
      l->v = nv;
      l->cap = nc;
   }
   l->v[l->n++] = Str_dupTSD( name );
}

static streng *nl_result( tsd_t *TSD, namelist *l )
{
   streng *r = Str_creTSD( "" );
   long i;

   if ( l->n )
      qsort( l->v, l->n, sizeof( streng * ), cmp_strings );
   for ( i = 0; i < l->n; i++ )
   {
      if ( i )
         r = catc( TSD, r, " " );
      r = cats( TSD, r, l->v[i] );
      Free_stringTSD( l->v[i] );
   }
   if ( l->v )
      FreeTSD( l->v );
   return r;
}

typedef struct { tsd_t *TSD; namelist *l; } nl_walk;

static void nl_var_cb( void *data, const streng *name, const streng *tail, const streng *value )
{
   nl_walk *w = (nl_walk *) data;
   (void) value;
   (void) tail;
   if ( ws_internal_name( name ) )
      return;
   if ( str_eqc( name, "RC" ) || str_eqc( name, "RESULT" ) || str_eqc( name, "SIGL" ) )
      return;
   nl_add( w->TSD, w->l, name );
}

static int ws_user_class( oo_tsd *ot, int i )
{
   return i >= ws_nbuiltin && ot->classes[i] != ot->cNilList;
}

/* []NL([classes]): 2 variables, 3 routines, 9 classes; default 2 3 9 */
streng *oo_q_nl( tsd_t *TSD, cparamboxptr parms )
{
   extern void oo_pool_walk( const tsd_t *TSD, var_hashtable *pool,
                             void ( *cb )( void *, const streng *, const streng *,
                                           const streng * ), void *data );
   oo_tsd *ot = get_ot( TSD );
   const streng *c = q_arg( parms, 0 );
   int want2 = 1, want3 = 1, want9 = 1, i;
   namelist l;
   nl_walk w;
   oo_routine *r;

   TSD->oo_active = 1;
   if ( c && c->len )
   {
      want2 = memchr( c->value, '2', c->len ) != NULL;
      want3 = memchr( c->value, '3', c->len ) != NULL;
      want9 = memchr( c->value, '9', c->len ) != NULL;
   }
   memset( &l, 0, sizeof( l ) );
   w.TSD = TSD;
   w.l = &l;
   if ( want2 )
      oo_pool_walk( TSD, TSD->currlevel->vars, nl_var_cb, &w );
   if ( want3 )
      for ( r = ot->routines; r; r = r->next )
         nl_add( TSD, &l, r->name );
   if ( want9 )
      for ( i = 0; i < ot->nclasses; i++ )
         if ( ws_user_class( ot, i ) )
            nl_add( TSD, &l, ot->classes[i]->u.c->id );
   return nl_result( TSD, &l );
}

/* loads a program from a string: its routines and classes, its prolog */
static void load_source( tsd_t *TSD, const streng *src, const char *pname )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   internal_parser_type parsing;
   streng *text;
   oo_program *p;
   oo_frame *frame;
   prolog_data pd;

   /* the marker: the OO keywords are on in the definition */
   text = Str_creTSD( "/* OOREGINA */ " );
   text = cats( TSD, text, src );
   fetch_string( TSD, text, &parsing );
   Free_stringTSD( text );
   if ( parsing.result != 0 )
      exiterror( ERR_YACC_SYNTAX, 1, parsing.tline );
   p = (oo_program *) MallocTSD( sizeof( oo_program ) );
   memset( p, 0, sizeof( oo_program ) );
   p->tree = parsing;
   p->file = Str_creTSD( pname );
   p->next = ot->programs;
   ot->programs = p;
   collect_labels( TSD, parsing.root, &p->labels, NULL );
   frame = new_frame( TSD );
   frame->labels = &p->labels;
   ot->loading = ( parsing.root && parsing.root->type == X_OO_INIT ) ? p : NULL;
   pd.p = p;
   pd.frame = frame;
   oo_exit_protected( TSD, run_prolog, &pd );
   ot->loading = NULL;
   FreeTSD( frame );
}

static void ws_register( tsd_t *TSD, const streng *name, int kind, const streng *src )
{
   oo_wsdef *d, **pp;

   for ( pp = &ws_defs; *pp; )
      if ( str_eq( ( *pp )->name, name ) )
      {
         d = *pp;
         *pp = d->next;
         Free_stringTSD( d->name );
         Free_stringTSD( d->src );
         FreeTSD( d );
      }
      else
         pp = &( *pp )->next;
   d = (oo_wsdef *) MallocTSD( sizeof( oo_wsdef ) );
   d->name = Str_dupTSD( name );
   d->kind = kind;
   d->src = Str_dupTSD( src );
   d->next = NULL;
   for ( pp = &ws_defs; *pp; pp = &( *pp )->next )
      ;
   *pp = d;
}

/* defines the routines and classes of src; returns their names */
static streng *ws_define( tsd_t *TSD, const streng *src )
{
   oo_tsd *ot = get_ot( TSD );
   oo_routine *oldr = ot->routines, *r;
   int oldc = ot->nclasses, i;
   static int serial = 0;
   char pname[40];
   namelist l;

   TSD->oo_active = 1;
   if ( ws_nbuiltin < 0 )
      ws_nbuiltin = ot->nclasses;
   sprintf( pname, "<workspace %d>", ++serial );
   load_source( TSD, src, pname );
   memset( &l, 0, sizeof( l ) );
   for ( r = ot->routines; r && r != oldr; r = r->next )
   {
      ws_register( TSD, r->name, 3, src );
      nl_add( TSD, &l, r->name );
   }
   for ( i = oldc; i < ot->nclasses; i++ )
   {
      ws_register( TSD, ot->classes[i]->u.c->id, 9, src );
      nl_add( TSD, &l, ot->classes[i]->u.c->id );
   }
   return nl_result( TSD, &l );
}

/* []FX(source [, more lines...]): defines routines and classes */
streng *oo_q_fx( tsd_t *TSD, cparamboxptr parms )
{
   streng *src = Str_creTSD( "" ), *r;
   int i, n = q_argc( parms );

   for ( i = 0; i < n; i++ )
   {
      const streng *v = q_arg( parms, i );
      if ( i )
         src = catc( TSD, src, "\n" );
      if ( v )
         src = cats( TSD, src, v );
   }
   r = ws_define( TSD, src );
   Free_stringTSD( src );
   return r;
}

/* []CR(name): the source of a definition made with []FX */
streng *oo_q_cr( tsd_t *TSD, cparamboxptr parms )
{
   streng *name = q_need( TSD, parms, 0, "[]CR" );
   oo_wsdef *d;

   mem_upper( name->value, name->len );
   for ( d = ws_defs; d; d = d->next )
      if ( str_eq( d->name, name ) )
      {
         Free_stringTSD( name );
         return Str_dupTSD( d->src );
      }
   Free_stringTSD( name );
   return nullstringptr();
}

/* removes the routine or class name; 1 if there was one */
static int ws_erase_def( tsd_t *TSD, const streng *name )
{
   oo_tsd *ot = get_ot( TSD );
   oo_routine **rp;
   oo_wsdef **pp, *d;
   int found = 0, i;

   for ( rp = &ot->routines; *rp; )
      if ( str_eq( ( *rp )->name, name ) )
      {
         *rp = ( *rp )->next;     /* its body stays with its program */
         found = 1;
      }
      else
         rp = &( *rp )->next;
   for ( i = 0; i < ot->nclasses; i++ )
      if ( ws_user_class( ot, i ) && str_eq( ot->classes[i]->u.c->id, name ) )
      {
         streng *old = h_remove( TSD, ot->environment->u.h, name );
         if ( old )
            Free_stringTSD( old );
         /* the class object stays for its instances; it is not listed */
         memmove( ot->classes + i, ot->classes + i + 1,
                  ( ot->nclasses - i - 1 ) * sizeof( oo_object * ) );
         ot->nclasses--;
         i--;
         found = 1;
      }
   for ( pp = &ws_defs; *pp; )
      if ( str_eq( ( *pp )->name, name ) )
      {
         d = *pp;
         *pp = d->next;
         Free_stringTSD( d->name );
         Free_stringTSD( d->src );
         FreeTSD( d );
      }
      else
         pp = &( *pp )->next;
   return found;
}

static int ws_var_exists( tsd_t *TSD, const streng *name )
{
   extern const streng *oo_peek_simple( tsd_t *TSD, const streng *name );
   if ( name->len && name->value[name->len - 1] == '.' )
      return 1;
   return oo_peek_simple( TSD, name ) != NULL;
}

/* []EX(names): erases variables, routines and classes; 1 or 0 per name */
streng *oo_q_ex( tsd_t *TSD, cparamboxptr parms )
{
   streng *names = q_need( TSD, parms, 0, "[]EX" ), *r = Str_creTSD( "" );
   int i = 0, n = names->len, first = 1;

   get_ot( TSD );
   mem_upper( names->value, names->len );
   while ( i < n )
   {
      int s, ok = 0;
      streng *w;
      while ( i < n && names->value[i] == ' ' )
         i++;
      if ( i >= n )
         break;
      for ( s = i; i < n && names->value[i] != ' '; i++ )
         ;
      w = Str_ncreTSD( names->value + s, i - s );
      if ( ws_var_exists( TSD, w ) )
      {
         drop_var( TSD, w );
         ok = 1;
      }
      ok |= ws_erase_def( TSD, w );
      Free_stringTSD( w );
      r = catc( TSD, r, first ? ( ok ? "1" : "0" ) : ( ok ? " 1" : " 0" ) );
      first = 0;
   }
   Free_stringTSD( names );
   return r;
}

/* []WSID([name]): the workspace name; sets it if given */
streng *oo_q_wsid( tsd_t *TSD, cparamboxptr parms )
{
   const streng *v = q_arg( parms, 0 );
   streng *old = ws_id ? Str_dupTSD( ws_id ) : Str_creTSD( "CLEAR WS" );

   if ( v )
   {
      if ( ws_id )
         Free_stringTSD( ws_id );
      ws_id = Str_dupTSD( v );
   }
   return old;
}

/* []CLEAR(): no variables, no definitions, WSID "CLEAR WS" */
streng *oo_q_clear( tsd_t *TSD, cparamboxptr parms )
{
   extern void oo_pool_walk( const tsd_t *TSD, var_hashtable *pool,
                             void ( *cb )( void *, const streng *, const streng *,
                                           const streng * ), void *data );
   namelist l;
   nl_walk w;
   long i;

   (void) parms;
   get_ot( TSD );
   memset( &l, 0, sizeof( l ) );
   w.TSD = TSD;
   w.l = &l;
   oo_pool_walk( TSD, TSD->currlevel->vars, nl_var_cb, &w );
   for ( i = 0; i < l.n; i++ )
   {
      drop_var( TSD, l.v[i] );
      Free_stringTSD( l.v[i] );
   }
   if ( l.v )
      FreeTSD( l.v );
   while ( ws_defs )
   {
      streng *n = Str_dupTSD( ws_defs->name );
      ws_erase_def( TSD, n );
      Free_stringTSD( n );
   }
   if ( ws_id )
      Free_stringTSD( ws_id );
   ws_id = NULL;
   return nullstringptr();
}

/* ------------------------------------------------------------------ */
/* writing a workspace                                                */
/* ------------------------------------------------------------------ */

typedef struct {
   tsd_t *TSD;
   oo_tsd *ot;
   long *num;                   /* object id -> number in the file    */
   oo_object **list;            /* objects to write, in order         */
   long n, cap;
   streng *out;
   namelist *only;              /* names to write, NULL for all       */
} ws_writer;

static void ws_put( ws_writer *w, const char *s )
{
   tsd_t *TSD = w->TSD;
   w->out = catc( TSD, w->out, s );
}

/* a string as a Rexx literal; long ones are split over continued lines */
static void ws_literal( ws_writer *w, const streng *s )
{
   int i, plain = 1, chunk = 0;
   char buf[8];

   for ( i = 0; i < s->len; i++ )
   {
      unsigned char c = (unsigned char) s->value[i];
      if ( c < 0x20 || c > 0x7e )
         plain = 0;
   }
   if ( s->len == 0 )
   {
      ws_put( w, "''" );
      return;
   }
   ws_put( w, "'" );
   for ( i = 0; i < s->len; i++ )
   {
      if ( chunk >= ( plain ? 200 : 100 ) )
      {
         ws_put( w, plain ? "' ||,\n  '" : "'x ||,\n  '" );
         chunk = 0;
      }
      if ( plain )
      {
         buf[0] = s->value[i];
         buf[1] = '\0';
         ws_put( w, buf );
         if ( s->value[i] == '\'' )
            ws_put( w, "'" );
      }
      else
      {
         sprintf( buf, "%02X", (unsigned char) s->value[i] );
         ws_put( w, buf );
      }
      chunk++;
   }
   ws_put( w, plain ? "'" : "'x" );
}

static long ws_objnum( ws_writer *w, oo_object *o );

/* a value: a literal, .nil, .CLASS or __ws.N */
static void ws_value( ws_writer *w, const streng *v )
{
   tsd_t *TSD = w->TSD;
   oo_object *o = v ? obj_of( TSD, v ) : NULL;
   char buf[40];

   if ( v == NULL )
   {
      ws_put( w, ".nil" );
      return;
   }
   if ( o == NULL )
   {
      ws_literal( w, v );
      return;
   }
   if ( o->kind == OK_NIL )
      ws_put( w, ".nil" );
   else if ( o == w->ot->environment )
      ws_put( w, ".environment" );
   else if ( o->kind == OK_CLASS )
   {
      ws_put( w, "." );
      w->out = cats( TSD, w->out, o->u.c->id );
   }
   else
   {
      long n = ws_objnum( w, o );
      if ( n == 0 )
         ws_put( w, ".nil" );          /* not saved: a Python object, say */
      else
      {
         sprintf( buf, "__ws.%ld", n );
         ws_put( w, buf );
      }
   }
}

static int ws_savable( oo_object *o )
{
   switch ( o->kind )
   {
      case OK_PYOBJ: case OK_SUPPLIER: case OK_SUPER: case OK_CLASS: case OK_NIL:
         return 0;
      default:
         return 1;
   }
}

/* the number of an object in the file; it is added to the list */
static long ws_objnum( ws_writer *w, oo_object *o )
{
   tsd_t *TSD = w->TSD;

   if ( !ws_savable( o ) )
      return 0;
   if ( w->num[o->id] == 0 )
   {
      if ( w->n >= w->cap )
      {
         long nc = w->cap ? w->cap * 2 : 64;
         oo_object **nl = (oo_object **) MallocTSD( nc * sizeof( oo_object * ) );
         if ( w->n )
            memcpy( nl, w->list, w->n * sizeof( oo_object * ) );
         if ( w->list )
            FreeTSD( w->list );
         w->list = nl;
         w->cap = nc;
      }
      w->list[w->n++] = o;
      w->num[o->id] = w->n;
   }
   return w->num[o->id];
}

static void ws_obj_ref( ws_writer *w, oo_object *o )
{
   char buf[40];
   sprintf( buf, "__ws.%ld", ws_objnum( w, o ) );
   ws_put( w, buf );
}

/* "call []WSITEM __ws.N, 'OP', v1, v2" */
static void ws_item( ws_writer *w, oo_object *o, const char *op, const streng *a,
                     const streng *b, int b_is_raw )
{
   ws_put( w, "call []WSITEM " );
   ws_obj_ref( w, o );
   ws_put( w, ", '" );
   ws_put( w, op );
   ws_put( w, "'" );
   if ( a || b )
   {
      ws_put( w, ", " );
      ws_value( w, a );
   }
   if ( b )
   {
      ws_put( w, ", " );
      if ( b_is_raw )
         ws_literal( w, b );
      else
         ws_value( w, b );
   }
   ws_put( w, "\n" );
}

typedef struct { ws_writer *w; oo_object *o; streng *cls; } ws_scope_walk;

static void ws_scope_cb( void *data, const streng *name, const streng *tail, const streng *value )
{
   ws_scope_walk *s = (ws_scope_walk *) data;
   ws_writer *w = s->w;
   tsd_t *TSD = w->TSD;
   streng *full = Str_dupTSD( name );

   if ( tail )
      full = cats( TSD, full, tail );
   ws_put( w, "call []WSVAR " );
   ws_obj_ref( w, s->o );
   ws_put( w, ", " );
   ws_literal( w, s->cls );
   ws_put( w, ", " );
   ws_literal( w, full );
   ws_put( w, ", " );
   ws_value( w, value );
   ws_put( w, "\n" );
   Free_stringTSD( full );
}

/* the statements that fill object o */
static void ws_fill( ws_writer *w, oo_object *o )
{
   extern void oo_pool_walk( const tsd_t *TSD, var_hashtable *pool,
                             void ( *cb )( void *, const streng *, const streng *,
                                           const streng * ), void *data );
   tsd_t *TSD = w->TSD;
   oo_scope *sc;
   long i;
   char buf[60];

   switch ( o->kind )
   {
      case OK_ARRAY:
      case OK_QUEUE:
         if ( ARR_MULTI( o ) )
         {
            ws_put( w, "call []WSITEM " );
            ws_obj_ref( w, o );
            ws_put( w, ", 'DIMS'" );
            for ( i = 0; i < o->u.a.ndims; i++ )
            {
               sprintf( buf, ", %ld", o->u.a.dims[i] );
               ws_put( w, buf );
            }
            ws_put( w, "\n" );
         }
         for ( i = 0; i < o->u.a.size; i++ )
            if ( o->u.a.v[i] )
            {
               streng *pos = mkint( TSD, i + 1 );
               ws_item( w, o, "AT", o->u.a.v[i], pos, 1 );
               Free_stringTSD( pos );
            }
         break;
      case OK_DIRECTORY:
      case OK_TABLE:
      {
         oo_entry *e;
         for ( e = o->u.h->first; e; e = e->onext )
            ws_item( w, o, "KEY", e->value, e->key, 0 );
         break;
      }
      case OK_STEM:
      {
         stemlist l;
         variableptr b = stem_box( TSD, o );
         const streng *def = oo_stem_default( TSD, b );
         stem_sorted_tails( TSD, o, &l );
         if ( def )
            ws_item( w, o, "DEFAULT", def, NULL, 0 );
         for ( i = 0; i < l.n; i++ )
         {
            const streng *v = oo_stem_get( TSD, b, l.t[i] );
            if ( v )
               ws_item( w, o, "TAIL", v, l.t[i], 1 );
            Free_stringTSD( l.t[i] );
         }
         if ( l.t )
            FreeTSD( l.t );
         break;
      }
      case OK_CONS:
         ws_item( w, o, "CONS", o->u.cons.car, o->u.cons.cdr, 0 );
         break;
      case OK_STRING:
      case OK_BUFFER:
         if ( o->u.s )
            ws_item( w, o, "STRING", o->u.s, NULL, 0 );
         break;
      case OK_INTERVAL:
         sprintf( buf, "call []WSITEM __ws.%ld, 'INTERVAL', %ld, %ld, %ld\n",
                  ws_objnum( w, o ), o->u.iv.from, o->u.iv.to, o->u.iv.by );
         ws_put( w, buf );
         break;
      case OK_BLOCK:
         ws_put( w, "call []WSITEM " );
         ws_obj_ref( w, o );
         ws_put( w, ", 'BLOCK', " );
         ws_literal( w, o->u.blk.code->node->name );
         ws_put( w, "\n" );
         break;
      default:
         break;
   }
   if ( o->aux )
      ws_item( w, o, "AUX", o->aux, NULL, 0 );
   if ( o->objname )
      ws_item( w, o, "NAME", o->objname, NULL, 0 );
   for ( sc = o->scopes; sc; sc = sc->next )
   {
      ws_scope_walk s;
      s.w = w;
      s.o = o;
      s.cls = sc->cls->u.c->id;
      oo_pool_walk( TSD, sc->pool, ws_scope_cb, &s );
   }
}

typedef struct { ws_writer *w; streng *chunk; streng *lastname; } ws_var_walk;

static int ws_wanted( ws_writer *w, const streng *name )
{
   long i;
   if ( w->only == NULL )
      return 1;
   for ( i = 0; i < w->only->n; i++ )
      if ( str_eq( w->only->v[i], name ) )
         return 1;
   return 0;
}

static int ws_symbol( const streng *s )
{
   int i;
   if ( s->len == 0 || ( s->value[0] >= '0' && s->value[0] <= '9' ) || s->value[0] == '.' )
      return 0;
   for ( i = 0; i < s->len; i++ )
   {
      char c = s->value[i];
      if ( !( ( c >= 'A' && c <= 'Z' ) || ( c >= '0' && c <= '9' ) || c == '_' || c == '!'
           || c == '?' ) )
         return 0;
   }
   return 1;
}

/* variables: collected first (their values may add objects) */
typedef struct { streng *name, *tail, *value; } ws_varent;
typedef struct { tsd_t *TSD; ws_varent *v; long n, cap; ws_writer *w; } ws_vars;

static void ws_var_cb( void *data, const streng *name, const streng *tail, const streng *value )
{
   ws_vars *vs = (ws_vars *) data;
   tsd_t *TSD = vs->TSD;

   if ( ws_internal_name( name ) || str_eqc( name, "RC" ) || str_eqc( name, "RESULT" )
     || str_eqc( name, "SIGL" ) || !ws_wanted( vs->w, name ) )
      return;
   if ( vs->n >= vs->cap )
   {
      long nc = vs->cap ? vs->cap * 2 : 64;
      ws_varent *nv = (ws_varent *) MallocTSD( nc * sizeof( ws_varent ) );
      if ( vs->n )
         memcpy( nv, vs->v, vs->n * sizeof( ws_varent ) );
      if ( vs->v )
         FreeTSD( vs->v );
      vs->v = nv;
      vs->cap = nc;
   }
   vs->v[vs->n].name = Str_dupTSD( name );
   vs->v[vs->n].tail = tail ? Str_dupTSD( tail ) : NULL;
   vs->v[vs->n].value = Str_dupTSD( value );
   vs->n++;
}

static int cmp_varent( const void *x, const void *y )
{
   const ws_varent *a = (const ws_varent *) x, *b = (const ws_varent *) y;
   int r = cmp_strings( &a->name, &b->name );
   if ( r )
      return r;
   if ( a->tail == NULL && b->tail == NULL )
      return 0;
   if ( a->tail == NULL )
      return -1;
   if ( b->tail == NULL )
      return 1;
   return cmp_tails( &a->tail, &b->tail );
}

/* the text of a workspace: definitions, objects, variables */
static streng *ws_write( tsd_t *TSD, namelist *only )
{
   extern void oo_pool_walk( const tsd_t *TSD, var_hashtable *pool,
                             void ( *cb )( void *, const streng *, const streng *,
                                           const streng * ), void *data );
   oo_tsd *ot = get_ot( TSD );
   ws_writer w;
   ws_vars vs;
   streng *vartext, *objtext, *tmp;
   oo_wsdef *d, *e;
   long i, done;
   char buf[200];
   time_t now = time( NULL );

   memset( &w, 0, sizeof( w ) );
   w.TSD = TSD;
   w.ot = ot;
   w.only = only;
   w.num = (long *) MallocTSD( ( ot->nobjs + 2 ) * sizeof( long ) );
   memset( w.num, 0, ( ot->nobjs + 2 ) * sizeof( long ) );

   /* the variables, written into their own text */
   memset( &vs, 0, sizeof( vs ) );
   vs.TSD = TSD;
   vs.w = &w;
   oo_pool_walk( TSD, TSD->currlevel->vars, ws_var_cb, &vs );
   if ( vs.n )
      qsort( vs.v, vs.n, sizeof( ws_varent ), cmp_varent );
   w.out = Str_creTSD( "" );
   for ( i = 0; i < vs.n; i++ )
   {
      ws_varent *v = &vs.v[i];
      if ( i == 0 || !str_eq( v->name, vs.v[i - 1].name ) )
      {
         ws_put( &w, "::var " );
         w.out = cats( TSD, w.out, v->name );
         ws_put( &w, "\n" );
      }
      if ( v->tail == NULL && ws_symbol( v->name ) && v->name->value[v->name->len - 1] != '.' )
      {
         w.out = cats( TSD, w.out, v->name );
         ws_put( &w, " = " );
         ws_value( &w, v->value );
         ws_put( &w, "\n" );
      }
      else
      {
         ws_put( &w, "call []WSITEM '', 'VAR', " );
         ws_value( &w, v->value );
         ws_put( &w, ", " );
         ws_literal( &w, v->name );
         if ( v->tail )
         {
            ws_put( &w, ", " );
            ws_literal( &w, v->tail );
         }
         ws_put( &w, "\n" );
      }
   }
   for ( i = 0; i < vs.n; i++ )
   {
      Free_stringTSD( vs.v[i].name );
      if ( vs.v[i].tail )
         Free_stringTSD( vs.v[i].tail );
      Free_stringTSD( vs.v[i].value );
   }
   if ( vs.v )
      FreeTSD( vs.v );
   vartext = w.out;

   /* the objects: made first, then filled (filling may add more) */
   w.out = Str_creTSD( "" );
   tmp = Str_creTSD( "" );
   for ( done = 0; done < w.n; done++ )
   {
      oo_object *o = w.list[done];
      streng *made = w.out;
      w.out = tmp;
      ws_fill( &w, o );
      tmp = w.out;
      w.out = made;
      sprintf( buf, "__ws.%ld = []WSOBJ(", done + 1 );
      ws_put( &w, buf );
      ws_literal( &w, o->cls->u.c->id );
      ws_put( &w, ")\n" );
   }
   objtext = cats( TSD, w.out, tmp );
   Free_stringTSD( tmp );

   /* the file */
   w.out = Str_creTSD( "" );
   strftime( buf, sizeof( buf ), "%Y-%m-%d %H:%M:%S", localtime( &now ) );
   ws_put( &w, "/* Regina OO workspace " );
   if ( ws_id )
      w.out = cats( TSD, w.out, ws_id );
   ws_put( &w, ", " );
   ws_put( &w, buf );
   ws_put( &w, " */\n" );
   if ( ws_id )
   {
      ws_put( &w, "::wsid " );
      w.out = cats( TSD, w.out, ws_id );
      ws_put( &w, "\n" );
   }
   for ( d = ws_defs; d; d = d->next )
   {
      /* a source that defines several names is written once */
      for ( e = ws_defs; e != d; e = e->next )
         if ( str_eq( e->src, d->src ) && ( only == NULL || ws_wanted( &w, e->name ) ) )
            break;
      if ( e != d || ( only && !ws_wanted( &w, d->name ) ) )
         continue;
      ws_put( &w, "::fx " );
      w.out = cats( TSD, w.out, d->name );
      ws_put( &w, "\n" );
      w.out = cats( TSD, w.out, d->src );
      if ( d->src->len && d->src->value[d->src->len - 1] != '\n' )
         ws_put( &w, "\n" );
      ws_put( &w, "::endfx\n" );
   }
   if ( objtext->len )
   {
      ws_put( &w, "::objects\n" );
      w.out = cats( TSD, w.out, objtext );
   }
   w.out = cats( TSD, w.out, vartext );
   Free_stringTSD( objtext );
   Free_stringTSD( vartext );
   FreeTSD( w.num );
   if ( w.list )
      FreeTSD( w.list );
   return w.out;
}

/* the file name of a workspace: NAME.rxws unless it has an extension */
static char *ws_file( tsd_t *TSD, const streng *name, const char *ext )
{
   char *p = (char *) MallocTSD( name->len + 10 );
   int i, dot = 0;

   memcpy( p, name->value, name->len );
   p[name->len] = '\0';
   for ( i = name->len - 1; i >= 0 && p[i] != '/' && p[i] != '\\'; i-- )
      if ( p[i] == '.' )
         dot = 1;
   if ( !dot )
      strcat( p, ext );
   return p;
}

static namelist *ws_names( tsd_t *TSD, cparamboxptr parms, int from )
{
   namelist *l;
   int i, n = q_argc( parms );

   if ( n <= from )
      return NULL;
   l = (namelist *) MallocTSD( sizeof( namelist ) );
   memset( l, 0, sizeof( namelist ) );
   for ( i = from; i < n; i++ )
   {
      const streng *v = q_arg( parms, i );
      int k = 0, s;
      if ( v == NULL )
         continue;
      while ( k < v->len )
      {
         streng *wd;
         while ( k < v->len && v->value[k] == ' ' )
            k++;
         if ( k >= v->len )
            break;
         for ( s = k; k < v->len && v->value[k] != ' '; k++ )
            ;
         wd = Str_ncreTSD( v->value + s, k - s );
         mem_upper( wd->value, wd->len );
         nl_add( TSD, l, wd );
         Free_stringTSD( wd );
      }
   }
   return l;
}

static void ws_free_names( tsd_t *TSD, namelist *l )
{
   long i;
   if ( l == NULL )
      return;
   for ( i = 0; i < l->n; i++ )
      Free_stringTSD( l->v[i] );
   if ( l->v )
      FreeTSD( l->v );
   FreeTSD( l );
}

static streng *ws_write_file( tsd_t *TSD, const char *file, namelist *only, const char *bif )
{
   streng *text = ws_write( TSD, only );
   FILE *fp = fopen( file, "wb" );

   if ( fp == NULL )
   {
      Free_stringTSD( text );
      exiterror( ERR_EXECUTION_ERROR, 18, bif, file );
   }
   fwrite( text->value, 1, text->len, fp );
   fclose( fp );
   Free_stringTSD( text );
   return Str_creTSD( file );
}

/* []SAVE([wsid]): writes the workspace to WSID.rxws; returns the file */
streng *oo_q_save( tsd_t *TSD, cparamboxptr parms )
{
   const streng *v = q_arg( parms, 0 );
   char *file;
   streng *r;

   get_ot( TSD );
   if ( v )
   {
      if ( ws_id )
         Free_stringTSD( ws_id );
      ws_id = Str_dupTSD( v );
   }
   if ( ws_id == NULL )
      exiterror( ERR_EXECUTION_ERROR, 19 );
   file = ws_file( TSD, ws_id, ".rxws" );
   r = ws_write_file( TSD, file, NULL, "[]SAVE" );
   FreeTSD( file );
   return r;
}

/* []OUT(file [, names]): writes all or the named objects to file (.rxo) */
streng *oo_q_out( tsd_t *TSD, cparamboxptr parms )
{
   streng *name = q_need( TSD, parms, 0, "[]OUT" ), *r;
   namelist *only = ws_names( TSD, parms, 1 );
   char *file = ws_file( TSD, name, ".rxo" );

   get_ot( TSD );
   r = ws_write_file( TSD, file, only, "[]OUT" );
   ws_free_names( TSD, only );
   FreeTSD( file );
   Free_stringTSD( name );
   return r;
}

/* ------------------------------------------------------------------ */
/* reading a workspace                                                */
/* ------------------------------------------------------------------ */

static void ws_run( tsd_t *TSD, const streng *code )
{
   if ( code->len )
      dointerpret( TSD, Str_dupTSD( code ) );
}

/* reads the file; returns the names it set or defined */
static streng *ws_read_file( tsd_t *TSD, const char *file, namelist *only, const char *bif )
{
   FILE *fp = fopen( file, "rb" );
   streng *text, *code = Str_creTSD( "" ), *fx = NULL, *section = NULL;
   namelist done;
   long size, i = 0;
   int mode = 0;   /* 0 header, 1 fx, 2 objects, 3 var (taken), 4 var (skipped) */
   streng *newid = NULL;

   if ( fp == NULL )
      exiterror( ERR_EXECUTION_ERROR, 18, bif, file );
   fseek( fp, 0, SEEK_END );
   size = ftell( fp );
   fseek( fp, 0, SEEK_SET );
   text = Str_makeTSD( size + 1 );
   text->len = (int) fread( text->value, 1, size, fp );
   fclose( fp );
   memset( &done, 0, sizeof( done ) );
   get_ot( TSD );

   while ( i < text->len )
   {
      long s = i;
      streng *line;
      while ( i < text->len && text->value[i] != '\n' )
         i++;
      line = Str_ncreTSD( text->value + s, (int) ( i - s ) );
      if ( i < text->len )
         i++;
      if ( line->len && line->value[line->len - 1] == '\r' )
         line->len--;
      if ( mode == 1 )
      {
         if ( line->len == 7 && memcmp( line->value, "::endfx", 7 ) == 0 )
         {
            int take = only == NULL;
            long k;
            for ( k = 0; only && k < only->n; k++ )
               if ( str_eq( only->v[k], section ) )
                  take = 1;
            if ( take )
            {
               streng *names = ws_define( TSD, fx );
               nl_add( TSD, &done, section );
               Free_stringTSD( names );
            }
            Free_stringTSD( fx );
            fx = NULL;
            mode = 0;
         }
         else
         {
            fx = cats( TSD, fx, line );
            fx = catc( TSD, fx, "\n" );
         }
         Free_stringTSD( line );
         continue;
      }
      if ( line->len > 2 && line->value[0] == ':' && line->value[1] == ':' )
      {
         streng *word, *arg;
         int k = 2, a;
         while ( k < line->len && line->value[k] != ' ' )
            k++;
         word = Str_ncreTSD( line->value + 2, k - 2 );
         mem_upper( word->value, word->len );
         while ( k < line->len && line->value[k] == ' ' )
            k++;
         for ( a = k; a < line->len && line->value[a] != ' '; a++ )
            ;
         arg = Str_ncreTSD( line->value + k, a - k );
         mem_upper( arg->value, arg->len );
         if ( str_eqc( word, "FX" ) )
         {
            if ( section )
               Free_stringTSD( section );
            section = arg;
            arg = NULL;
            fx = Str_creTSD( "" );
            mode = 1;
         }
         else if ( str_eqc( word, "OBJECTS" ) )
            mode = 2;
         else if ( str_eqc( word, "VAR" ) )
         {
            long j;
            mode = 4;
            if ( only == NULL )
               mode = 3;
            for ( j = 0; only && j < only->n; j++ )
               if ( str_eq( only->v[j], arg ) )
                  mode = 3;
            if ( mode == 3 )
               nl_add( TSD, &done, arg );
         }
         else if ( str_eqc( word, "WSID" ) )
         {
            if ( newid )
               Free_stringTSD( newid );
            newid = Str_ncreTSD( line->value + k, line->len - k );
         }
         if ( arg )
            Free_stringTSD( arg );
         Free_stringTSD( word );
         Free_stringTSD( line );
         continue;
      }
      if ( mode == 2 || mode == 3 )
      {
         code = cats( TSD, code, line );
         code = catc( TSD, code, "\n" );
      }
      Free_stringTSD( line );
   }
   if ( fx )
      Free_stringTSD( fx );
   if ( section )
      Free_stringTSD( section );
   Free_stringTSD( text );
   /* the objects and variables, run in the caller's variables */
   code = catc( TSD, code, "drop __ws.\n" );
   ws_run( TSD, code );
   Free_stringTSD( code );
   if ( newid && only == NULL && strcmp( bif, "[]LOAD" ) == 0 )
   {
      if ( ws_id )
         Free_stringTSD( ws_id );
      ws_id = newid;
      newid = NULL;
   }
   if ( newid )
      Free_stringTSD( newid );
   return nl_result( TSD, &done );
}

/* []LOAD(wsid): []CLEAR, then the workspace WSID.rxws */
streng *oo_q_load( tsd_t *TSD, cparamboxptr parms )
{
   streng *name = q_need( TSD, parms, 0, "[]LOAD" ), *r;
   char *file = ws_file( TSD, name, ".rxws" );
   FILE *fp = fopen( file, "rb" );

   if ( fp == NULL )
      exiterror( ERR_EXECUTION_ERROR, 18, "[]LOAD", file );
   fclose( fp );
   oo_q_clear( TSD, NULL );
   r = ws_read_file( TSD, file, NULL, "[]LOAD" );
   if ( ws_id == NULL )
      ws_id = Str_dupTSD( name );
   FreeTSD( file );
   Free_stringTSD( name );
   return r;
}

/* []IN(file [, names]): adds the objects of a file (.rxo or .rxws) */
streng *oo_q_in( tsd_t *TSD, cparamboxptr parms )
{
   streng *name = q_need( TSD, parms, 0, "[]IN" ), *r;
   namelist *only = ws_names( TSD, parms, 1 );
   char *file = ws_file( TSD, name, ".rxo" );
   FILE *fp = fopen( file, "rb" );

   if ( fp == NULL )
   {
      /* a saved workspace can be read too */
      FreeTSD( file );
      file = ws_file( TSD, name, ".rxws" );
   }
   else
      fclose( fp );
   r = ws_read_file( TSD, file, only, "[]IN" );
   ws_free_names( TSD, only );
   FreeTSD( file );
   Free_stringTSD( name );
   return r;
}

/* []GC(): collects now; returns the number of objects freed */
streng *oo_q_gc( tsd_t *TSD, cparamboxptr parms )
{
   long n;
   (void) parms;
   get_ot( TSD );
   n = oo_gc( TSD, 1 );
   return mkint( TSD, n < 0 ? 0 : n );
}

/* ------------------------------------------------------------------ */
/* the helpers used by workspace files                                */
/* ------------------------------------------------------------------ */

/* []WSOBJ(classid): a new object of the class, without INIT */
streng *oo_q_wsobj( tsd_t *TSD, cparamboxptr parms )
{
   oo_tsd *ot = get_ot( TSD );
   streng *id = q_need( TSD, parms, 0, "[]WSOBJ" );
   oo_object *cls, *o;

   mem_upper( id->value, id->len );
   cls = class_by_id( ot, id );
   if ( cls == NULL )
      exiterror( ERR_TRANSLATION_ERROR, 10, tmpstr_of( TSD, id ) );
   Free_stringTSD( id );
   if ( cls->u.c->resolved != 1 )
      resolve_class( TSD, cls );
   o = new_object( TSD, cls, cls->u.c->ikind );
   if ( o->kind == OK_DIRECTORY || o->kind == OK_TABLE )
   {
      o->u.h = (oo_hash *) MallocTSD( sizeof( oo_hash ) );
      memset( o->u.h, 0, sizeof( oo_hash ) );
   }
   else if ( o->kind == OK_BUFFER || o->kind == OK_STRING )
      o->u.s = nullstringptr();
   return hdup( TSD, o );
}

/* []WSITEM(obj, op, ...): stores a part of an object (see ws_fill) */
streng *oo_q_wsitem( tsd_t *TSD, cparamboxptr parms )
{
   oo_tsd *ot = get_ot( TSD );
   const streng *h = q_arg( parms, 0 ), *op = q_arg( parms, 1 );
   const streng *a = q_arg( parms, 2 ), *b = q_arg( parms, 3 );
   oo_object *o = h ? obj_of( TSD, h ) : NULL;

   if ( op == NULL )
      exiterror( ERR_INCORRECT_CALL, 3, "[]WSITEM", 2 );
   if ( str_eqc( op, "VAR" ) )
   {
      /* a variable of the caller: VAR, value, name [, tail] */
      if ( a == NULL || b == NULL )
         exiterror( ERR_INCORRECT_CALL, 3, "[]WSITEM", 4 );
      if ( q_arg( parms, 4 ) )
         oo_stem_put( TSD, TSD->currlevel->vars, b, q_arg( parms, 4 ), Str_dupTSD( a ) );
      else if ( b->len && b->value[b->len - 1] == '.' )
      {
         extern void setvalue( tsd_t *TSD, const streng *name, streng *value, int pool );
         setvalue( TSD, b, Str_dupTSD( a ), -1 );
      }
      else
         setvalue( TSD, b, Str_dupTSD( a ), -1 );
      return nullstringptr();
   }
   if ( o == NULL )
      exiterror( ERR_INCORRECT_CALL, 3, "[]WSITEM", 1 );
   if ( str_eqc( op, "AT" ) && a && b )
   {
      int err = 0;
      long pos = streng_to_int( TSD, b, &err );
      if ( err || pos < 1 )
         exiterror( ERR_INCORRECT_CALL, 3, "[]WSITEM", 4 );
      oo_array_put( TSD, o, pos, Str_dupTSD( a ) );
   }
   else if ( str_eqc( op, "DIMS" ) )
   {
      long dims[64];
      int n = 0, err = 0;
      cparamboxptr p = parms->next->next;
      for ( ; p && n < 64; p = p->next )
         if ( p->value )
            dims[n++] = streng_to_int( TSD, p->value, &err );
      if ( n > 1 )
         arr_make_multi( TSD, o, n, dims );
   }
   else if ( str_eqc( op, "KEY" ) && a && b )
   {
      if ( is_bag( TSD, o ) )
      {
         int err = 0;
         setbag_add( TSD, o, b, streng_to_int( TSD, a, &err ) );
      }
      else
         h_put( TSD, o->u.h, b, Str_dupTSD( a ) );
   }
   else if ( str_eqc( op, "TAIL" ) && a && b && o->kind == OK_STEM )
      oo_stem_put( TSD, o->u.pool, ot->s_stem, b, Str_dupTSD( a ) );
   else if ( str_eqc( op, "DEFAULT" ) && a && o->kind == OK_STEM )
   {
      extern void oo_pool_set_stem( const tsd_t *TSD, var_hashtable *pool,
                                    const streng *name, streng *value );
      oo_pool_set_stem( TSD, o->u.pool, ot->s_stem, Str_dupTSD( a ) );
   }
   else if ( str_eqc( op, "CONS" ) && a && b && o->kind == OK_CONS )
   {
      Free_stringTSD( o->u.cons.car );
      Free_stringTSD( o->u.cons.cdr );
      o->u.cons.car = Str_dupTSD( a );
      o->u.cons.cdr = Str_dupTSD( b );
   }
   else if ( str_eqc( op, "STRING" ) && a )
   {
      if ( o->u.s )
         Free_stringTSD( o->u.s );
      o->u.s = Str_dupTSD( a );
   }
   else if ( str_eqc( op, "INTERVAL" ) && o->kind == OK_INTERVAL && a && b && q_arg( parms, 4 ) )
   {
      int err = 0;
      o->u.iv.from = streng_to_int( TSD, a, &err );
      o->u.iv.to = streng_to_int( TSD, b, &err );
      o->u.iv.by = streng_to_int( TSD, q_arg( parms, 4 ), &err );
   }
   else if ( str_eqc( op, "BLOCK" ) && a && o->kind == OK_BLOCK )
   {
      /* bound to the code that reads the workspace */
      nodeptr n = (nodeptr) MallocTSD( sizeof( treenode ) );
      proclevel home = TSD->currlevel;
      memset( n, 0, sizeof( treenode ) );
      n->type = X_OO_BLOCK;
      n->name = Str_dupTSD( a );
      o->u.blk.code = block_code( TSD, n );
      o->u.blk.home = home;
      o->u.blk.serial = home->oo_serial;
      o->u.blk.homeframe = (oo_frame *) home->oo_ctx;
      o->u.blk.nextblock = (oo_object *) home->oo_blocks;
      home->oo_blocks = o;
   }
   else if ( str_eqc( op, "AUX" ) && a )
   {
      if ( o->aux )
         Free_stringTSD( o->aux );
      o->aux = Str_dupTSD( a );
   }
   else if ( str_eqc( op, "NAME" ) && a )
   {
      if ( o->objname )
         Free_stringTSD( o->objname );
      o->objname = Str_dupTSD( a );
   }
   return nullstringptr();
}

/* []WSVAR(obj, classid, name, value): an object variable of the class */
streng *oo_q_wsvar( tsd_t *TSD, cparamboxptr parms )
{
   oo_tsd *ot = get_ot( TSD );
   const streng *h = q_arg( parms, 0 ), *v = q_arg( parms, 3 );
   streng *cid = q_need( TSD, parms, 1, "[]WSVAR" ), *name = q_need( TSD, parms, 2, "[]WSVAR" );
   oo_object *o = h ? obj_of( TSD, h ) : NULL, *cls;
   oo_scope *sc;
   int i;

   cls = class_by_id( ot, cid );
   if ( o == NULL || cls == NULL || v == NULL )
      exiterror( ERR_INCORRECT_CALL, 3, "[]WSVAR", 1 );
   sc = get_scope( TSD, o, cls );
   for ( i = 0; i < name->len && name->value[i] != '.'; i++ )
      ;
   if ( i < name->len && i + 1 < name->len )
   {
      streng *stem = Str_ncreTSD( name->value, i + 1 );
      streng *tail = Str_ncreTSD( name->value + i + 1, name->len - i - 1 );
      oo_stem_put( TSD, sc->pool, stem, tail, Str_dupTSD( v ) );
      Free_stringTSD( stem );
      Free_stringTSD( tail );
   }
   else if ( i == name->len - 1 )
   {
      extern void oo_pool_set_stem( const tsd_t *TSD, var_hashtable *pool,
                                    const streng *name, streng *value );
      oo_pool_set_stem( TSD, sc->pool, name, Str_dupTSD( v ) );
   }
   else
      oo_pool_set( TSD, sc->pool, name, Str_dupTSD( v ) );
   Free_stringTSD( cid );
   Free_stringTSD( name );
   return nullstringptr();
}

/* "address WS": a command is shown, as the result of an expression in APL */
int oo_ws_echo( tsd_t *TSD, const streng *envir, const streng *cmd )
{
   if ( envir == NULL || envir->len != 2 || rx_toupper( envir->value[0] ) != 'W'
     || rx_toupper( envir->value[1] ) != 'S' )
      return 0;
   fwrite( cmd->value, 1, cmd->len, stdout );
   fputc( '\n', stdout );
   fflush( stdout );
   return 1;
}

/* ------------------------------------------------------------------ */
/* labels and routines                                                */
/* ------------------------------------------------------------------ */

nodeptr oo_getlabel( const tsd_t *TSD, const streng *name )
{
   oo_frame *f = (oo_frame *) TSD->currlevel->oo_ctx;
   int i;

   /* a block sees its own labels, then those of the code that created it */
   for ( ; f; f = f->block ? f->outer : NULL )
   {
      if ( f->labels )
         for ( i = 0; i < f->labels->n; i++ )
            if ( Str_ccmp( f->labels->v[i]->name, name ) == 0 )
               return f->labels->v[i];
      if ( !f->block )
         break;
   }
   return NULL;
}

/* 1 if getlabel() must also search the main program (a block made there) */
int oo_label_main( const tsd_t *TSD )
{
   oo_frame *f = (oo_frame *) TSD->currlevel->oo_ctx;

   while ( f && f->block && f->outer )
      f = f->outer;
   return f && f->block && f->outer == NULL;
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
   frame = new_frame( TSD );
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
   {
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   const streng **save_argv = ot->pend_argv;
   paramboxptr save_plist = ot->pend_plist;
   ot->pend_argv = argv;
   ot->pend_plist = plist;
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
   ot->pend_argv = save_argv;
   ot->pend_plist = save_plist;
   }
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
   /* obj~name(args) = v sends NAME= (v, args), a[i] = v sends []= (v, i) */
   argc = eval_args( TSD, target->p[1], &plist, &argv );
   if ( target->type == X_INDEX )
      name = Str_creTSD( "[]=" );
   else
   {
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
         const streng *nm = n->name;
         int stem = nm->len > 1 && nm->value[nm->len - 1] == '.'
                 && memchr( nm->value, '.', nm->len - 1 ) == NULL;
         variableptr box;

         if ( stem && a && a->stempool )
            /* "call f st." ... "use arg s.": s. is the caller's st. */
            oo_link_stem( TSD, nm, oo_stem_box( TSD, a->stempool, a->stemname ) );
         else if ( stem && a && a->value
                && ( box = oo_stem_object_box( TSD, a->value ) ) != NULL )
            oo_link_stem( TSD, nm, box );
         else if ( a && a->value )
            setvalue( TSD, nm, Str_dupTSD( a->value ), -1 );
         else
            drop_var( TSD, nm );
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

/* DO WITH over a stem: tails with their values */
typedef struct { streng *tail, *value; } tailpair;
typedef struct { tsd_t *TSD; tailpair *v; long n, cap; } pairsave;

static void pair_cb( tsd_t *TSD, void *data, const streng *tail, const streng *value )
{
   pairsave *t = (pairsave *) data;

   if ( t->n >= t->cap )
   {
      long ncap = t->cap ? t->cap * 2 : 16;
      tailpair *n = (tailpair *) MallocTSD( ncap * sizeof( tailpair ) );
      if ( t->v )
      {
         memcpy( n, t->v, t->n * sizeof( tailpair ) );
         FreeTSD( t->v );
      }
      t->v = n;
      t->cap = ncap;
   }
   t->v[t->n].tail = Str_dupTSD( tail );
   t->v[t->n].value = Str_dupTSD( value );
   t->n++;
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

/* the tail is the first member, so cmp_tails works on tailpairs too */
static int cmp_pairs( const void *x, const void *y )
{
   return cmp_tails( &( (const tailpair *) x )->tail, &( (const tailpair *) y )->tail );
}

/* DO WITH: the snapshot holds index n at 2n-1 and item n at 2n */
static void snap_pair( tsd_t *TSD, oo_object *snap, long *n, streng *idx, streng *item )
{
   ( *n )++;
   oo_array_put( TSD, snap, 2 * *n - 1, idx );
   oo_array_put( TSD, snap, 2 * *n, item );
}

static void snap_supplier( tsd_t *TSD, oo_object *snap, long *n, oo_object *sp )
{
   long i;

   for ( i = sp->u.sp.pos; i <= sp->u.sp.items->u.a.size; i++ )
      if ( sp->u.sp.items->u.a.v[i - 1] )
         snap_pair( TSD, snap, n,
                    nil_or( TSD, i <= sp->u.sp.indexes->u.a.size
                                 ? sp->u.sp.indexes->u.a.v[i - 1] : NULL ),
                    Str_dupTSD( sp->u.sp.items->u.a.v[i - 1] ) );
}

/* the classic meaning of "DO x OVER y": count = y if x and OVER are empty */
static int classic_over_count( tsd_t *TSD, nodeptr thisptr, const streng *v, long *count )
{
   nodeptr var = thisptr->p[1], over = thisptr->p[2];
   const streng *vx, *vo;
   streng *t;
   int err = 0, i, j;
   long n;

   if ( var == NULL || over == NULL || var->type != X_SIM_SYMBOL )
      return 0;
   vx = oo_peek_simple( TSD, var->name );
   vo = oo_peek_simple( TSD, over->name );
   if ( vx == NULL || vo == NULL || vx->len != 0 || vo->len != 0 || v == NULL
     || obj_of( TSD, v ) != NULL )
      return 0;
   for ( i = 0; i < v->len && rx_isspace( v->value[i] ); i++ )
      ;
   for ( j = v->len; j > i && rx_isspace( v->value[j - 1] ); j-- )
      ;
   if ( j == i )
      return 0;
   t = Str_ncreTSD( v->value + i, j - i );
   n = streng_to_int( TSD, t, &err );
   Free_stringTSD( t );
   if ( err || n < 0 )
      return 0;
   *count = n;
   return 1;
}

/*
 * A program that doesn't use objects gets the classic error for a "DO x
 * OVER y" that is neither a valid count nor a collection.
 */
static void classic_over_error( tsd_t *TSD, nodeptr thisptr, const streng *v )
{
   nodeptr var = thisptr->p[1], over = thisptr->p[2];
   const streng *vx, *vo;
   streng *s;

   if ( var == NULL || over == NULL || var->type != X_SIM_SYMBOL )
      return;
   vx = oo_peek_simple( TSD, var->name );
   vo = oo_peek_simple( TSD, over->name );
   s = Str_dupTSD( vx ? vx : var->name );
   s = catc( TSD, s, " " );
   s = cats( TSD, s, vo ? vo : over->name );
   s = catc( TSD, s, " " );
   s = cats( TSD, s, v );
   exiterror( ERR_INVALID_INTEGER, 2, tmpstr_of( TSD, s ) );
}

streng *oo_eval_over_init( tsd_t *TSD, nodeptr thisptr )
{
   int was_active = TSD->oo_active;
   oo_tsd *ot;
   nodeptr coll = thisptr->p[0];
   oo_object *snap, *o, *old;
   const streng *cur;
   streng *kill = NULL, *v = NULL, *arrh = NULL;
   long i, n = 0, classic;
   int with = thisptr->p[3] != NULL;   /* DO WITH INDEX/ITEM: pairs */

   /* release the snapshot of the previous run of this loop */
   cur = oo_peek_simple( TSD, thisptr->name );
   if ( cur && ( old = obj_of( TSD, cur ) ) != NULL && old->snapshot )
      free_object( TSD, old );

   if ( coll->type != X_STEM_SYMBOL )
      v = evaluate( TSD, coll, &kill );
   if ( classic_over_count( TSD, thisptr, coll->type == X_STEM_SYMBOL
                              ? isvariable( TSD, coll->name ) : v, &classic ) )
   {
      /*
       * A valid classic program: repeat the body, assign nothing. The
       * snapshot variable gets a plain string, objects stay unused.
       */
      if ( kill )
         Free_stringTSD( kill );
      setvalue( TSD, thisptr->name, Str_creTSD( "\001CLASSIC" ), -1 );
      return mkint( TSD, classic );
   }
   if ( !was_active && !with && coll->type != X_STEM_SYMBOL && !OO_LOOKS_LIKE_HANDLE( v ) )
      classic_over_error( TSD, thisptr, v );

   ot = get_ot( TSD );
   snap = new_object( TSD, ot->cArray, OK_ARRAY );
   snap->snapshot = 1;

   if ( with && coll->type == X_STEM_SYMBOL )
   {
      pairsave t;
      t.TSD = TSD;
      t.v = NULL;
      t.n = t.cap = 0;
      oo_stem_tails( TSD, coll->name, pair_cb, &t );
      if ( t.n )
         qsort( t.v, t.n, sizeof( tailpair ), cmp_pairs );
      for ( i = 0; i < t.n; i++ )
         snap_pair( TSD, snap, &n, t.v[i].tail, t.v[i].value );
      if ( t.v )
         FreeTSD( t.v );
   }
   else if ( with )
   {
      o = obj_of( TSD, v );
      if ( o && ( ( o->kind != OK_ARRAY && o->kind != OK_QUEUE && o->kind != OK_DIRECTORY
        && o->kind != OK_TABLE && o->kind != OK_SUPPLIER ) || is_bag( TSD, o ) || is_set( TSD, o ) ) )
      {
         /* any object that has a SUPPLIER or a MAKEARRAY method */
         streng *sname = Str_creTSD( "SUPPLIER" );
         oo_object *scope;
         if ( lookup( TSD, o, sname, NULL, &scope ) != NULL )
            arrh = send_msg( TSD, v, sname, 0, NULL );
         else
            arrh = send_msg( TSD, v, ot->s_makearray, 0, NULL );
         Free_stringTSD( sname );
         o = obj_of( TSD, arrh );
         if ( o == NULL || ( o->kind != OK_ARRAY && o->kind != OK_QUEUE
                          && o->kind != OK_SUPPLIER ) )
            o = NULL;
      }
      if ( o == NULL )
      {
         streng *d;
         d = describe( TSD, v );
         exiterror( ERR_EXECUTION_ERROR, 5, tmpstr_of( TSD, d ) );
      }
      if ( o->kind == OK_ARRAY || o->kind == OK_QUEUE )
      {
         for ( i = 0; i < o->u.a.size; i++ )
            if ( o->u.a.v[i] )
               snap_pair( TSD, snap, &n, arr_index_value( TSD, o, i ), Str_dupTSD( o->u.a.v[i] ) );
      }
      else if ( o->kind == OK_SUPPLIER )
         snap_supplier( TSD, snap, &n, o );
      else
      {
         oo_entry *e;
         for ( e = o->u.h->first; e; e = e->onext )
            snap_pair( TSD, snap, &n, Str_dupTSD( e->key ), Str_dupTSD( e->value ) );
      }
      if ( kill )
         Free_stringTSD( kill );
      if ( arrh )
         Free_stringTSD( arrh );
   }
   else if ( coll->type == X_STEM_SYMBOL )
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
      o = obj_of( TSD, v );
      if ( o && ( ( o->kind != OK_ARRAY && o->kind != OK_QUEUE && o->kind != OK_DIRECTORY
        && o->kind != OK_TABLE && o->kind != OK_SUPPLIER ) || is_bag( TSD, o ) ) )
      {
         /* any object that understands MAKEARRAY; a Bag repeats its items */
         arrh = send_msg( TSD, v, ot->s_makearray, 0, NULL );
         o = obj_of( TSD, arrh );
         if ( o == NULL || ( o->kind != OK_ARRAY && o->kind != OK_QUEUE ) )
            o = NULL;
      }
      if ( o == NULL )
      {
         streng *d;
         d = describe( TSD, v );
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
   streng *r;
   /* the hidden variables are read without tracing */
   const streng *a = oo_peek_simple( TSD, thisptr->p[0]->name );
   const streng *i = oo_peek_simple( TSD, thisptr->p[1]->name );
   oo_object *o = a ? obj_of( TSD, a ) : NULL;
   int err = 0;
   long idx = i ? streng_to_int( TSD, i, &err ) : 0;

   /* DO WITH: p[2] names the half of the pair, "INDEX" or "ITEM" */
   if ( thisptr->p[2] && !err )
      idx = thisptr->p[2]->name->value[1] == 'N' ? 2 * idx - 1 : 2 * idx;
   if ( o == NULL || err || idx < 1 || idx > o->u.a.size )
      exiterror( ERR_INTERPRETER_FAILURE, 1, __FILE__, __LINE__, "DO OVER" );
   r = Str_dupTSD( o->u.a.v[idx - 1] );
   return r;
}

/* DO name OVER c WHILE cond: set name before cond is tested */
void oo_over_assign( tsd_t *TSD, nodeptr thisptr )
{
   streng *v;
   const streng *h = oo_peek_simple( TSD, thisptr->p[1]->p[0]->name );
   oo_object *snap = h ? obj_of( TSD, h ) : NULL;

   if ( snap == NULL )
      return;      /* classic "DO expr": no control variable */
   v = oo_eval_over_item( TSD, thisptr->p[1] );
   if ( TSD->trace_stat == 'I' )
      tracevalue( TSD, v, 'V' );

   if ( thisptr->p[0]->type == X_HEAD_SYMBOL )
      fix_compound( TSD, thisptr->p[0], v );
   else
      setshortcut( TSD, thisptr->p[0], v );
}

/* DO COUNTER c: assign the iteration count n to the variable node var */
void oo_do_counter( tsd_t *TSD, nodeptr var, long n )
{
   streng *v = mkint( TSD, n );

   if ( var->type == X_HEAD_SYMBOL )
      fix_compound( TSD, var, v );
   else
      setshortcut( TSD, var, v );
}

streng *oo_describe( tsd_t *TSD, const streng *s )
{
   oo_object *o = obj_of( TSD, s );

   return o ? default_name( TSD, o ) : NULL;
}

/* ------------------------------------------------------------------ */
/* operators applied to objects                                       */
/* ------------------------------------------------------------------ */

static const char *operator_name( int type )
{
   switch ( type )
   {
      case X_PLUSS: return "+";
      case X_MINUS: return "-";
      case X_MULT: return "*";
      case X_DEVIDE: return "/";
      case X_INTDIV: return "%";
      case X_MODULUS: return "//";
      case X_EXP: return "**";
      case X_CONCAT: return "||";
      case X_SPACE: return " ";
      case X_EQUAL: case X_SEQUAL: case X_NEQUAL: return "=";
      case X_DIFF: case X_SDIFF: case X_NDIFF: return "\\=";
      case X_GT: case X_SGT: case X_NGT: return ">";
      case X_LT: case X_SLT: case X_NLT: return "<";
      case X_GTE: case X_SGTE: case X_NGTE: return ">=";
      case X_LTE: case X_SLTE: case X_NLTE: return "<=";
      case X_S_EQUAL: return "==";
      case X_S_DIFF: return "\\==";
      case X_S_GT: return ">>";
      case X_S_LT: return "<<";
      case X_S_GTE: return ">>=";
      case X_S_LTE: return "<<=";
      case X_S_NGT: return "\\>>";
      case X_S_NLT: return "\\<<";
      case X_LOG_AND: return "&";
      case X_LOG_OR: return "|";
      case X_LOG_XOR: return "&&";
      case X_U_MINUS: return "-";
      case X_U_PLUSS: return "+";
      case X_LOG_NOT: return "\\";
      default: return NULL;
   }
}

/*
 * oo_operator handles "left op right" when left is an object: the object
 * receives the message "op" with right as argument. *handled is 0 if left
 * is no object; then *leftval may hold the already evaluated left operand
 * (owned by the caller) which must not be evaluated a second time.
 */
streng *oo_operator( tsd_t *TSD, nodeptr thisptr, int *handled, streng **leftval )
{
   nodeptr left = thisptr->p[0];
   const char *op = operator_name( thisptr->type );
   streng *lv = NULL, *rv, *rkill = NULL, *name, *r;
   const streng *lval = NULL, *argv[1];

   int unary = ( thisptr->type == X_U_MINUS || thisptr->type == X_U_PLUSS
                 || thisptr->type == X_LOG_NOT );

   *handled = 0;
   *leftval = NULL;
   if ( op == NULL || left == NULL || ( thisptr->p[1] == NULL && !unary ) )
      return NULL;
   switch ( left->type )
   {
      case X_SIM_SYMBOL:
         /* look without evaluating: no trace line, no NOVALUE condition */
         lval = oo_peek_simple( TSD, left->name );
         if ( lval == NULL || obj_of( TSD, lval ) == NULL )
            return NULL;
         /* the right operand may change the variable: keep a copy */
         lval = lv = Str_dupTSD( lval );
         break;

      case X_STRING:
      case X_STEM_SYMBOL:
         return NULL;

      case X_CON_SYMBOL:
         if ( !OO_IS_ENVSYMBOL( left ) )
            return NULL;
         /* fall through */
      default:
         /*
          * Any other expression may produce an object. It is evaluated
          * once; if it isn't an object the caller continues with the value.
          */
         lv = evaluate( TSD, left, NULL );
         if ( obj_of( TSD, lv ) == NULL )
         {
            *leftval = lv;
            return NULL;
         }
         lval = lv;
         break;
   }

   name = Str_creTSD( op );
   if ( unary )
   {
      rv = NULL;
      r = send_msg( TSD, lval, name, 0, NULL );
   }
   else
   {
      rv = evaluate( TSD, thisptr->p[1], &rkill );
      argv[0] = rv;
      r = send_msg( TSD, lval, name, 1, argv );
   }
   Free_stringTSD( name );
   if ( rkill )
      Free_stringTSD( rkill );
   if ( lv )
      Free_stringTSD( lv );
   if ( r == NULL )
      exiterror( ERR_NO_RESULT_OBJECT, 1, op );
   *handled = 1;
   return r;
}

/* Object's concatenation methods: the string values are concatenated */
static streng *m_obj_concat( tsd_t *TSD, oo_call *c )
{
   streng *a = oo_stringify( TSD, c->selfstr ), *b = NULL, *r;
   const streng *bs = opt_arg( c, 0 );

   if ( a == NULL )
      a = Str_dupTSD( c->selfstr );
   if ( bs )
   {
      b = oo_stringify( TSD, bs );
      if ( c->name->len == 1 && c->name->value[0] == ' ' )
         a = catc( TSD, a, " " );
      a = cats( TSD, a, b ? b : bs );
   }
   r = a;
   if ( b )
      Free_stringTSD( b );
   return r;
}

/*
 * Frees the OO state of the thread: objects, classes, routines and the
 * parse trees kept for them. Variable pools of objects are not freed;
 * they belong to the variable module and are small.
 */
void oo_end_program( tsd_t *TSD )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_program *p, *pn;
   oo_routine *r, *rn;
   unsigned long i;
   int k;

   if ( ot == NULL )
      return;
   for ( i = 1; i <= ot->nobjs; i++ )
   {
      oo_object *o = ot->objs[i];
      oo_scope *s, *sn;
      if ( o == NULL )
         continue;
      if ( ( o->kind == OK_DIRECTORY || o->kind == OK_TABLE ) && o->u.h )
      {
         while ( o->u.h->first )
         {
            streng *v = h_remove( TSD, o->u.h, o->u.h->first->key );
            Free_stringTSD( v );
         }
         if ( o->u.h->tbl )
            FreeTSD( o->u.h->tbl );
         FreeTSD( o->u.h );
         o->u.h = NULL;
      }
      else if ( ( o->kind == OK_BUFFER || o->kind == OK_STRING ) && o->u.s )
         Free_stringTSD( o->u.s );
      else if ( o->kind == OK_CLASS && o->u.c )
      {
         oo_classdata *cd = o->u.c;
         oo_method *m, *mn;
         int pass;
         for ( pass = 0; pass < 2; pass++ )
            for ( m = pass ? cd->cmethods : cd->imethods; m; m = mn )
            {
               mn = m->next;
               Free_stringTSD( m->name );
               if ( m->attr )
                  Free_stringTSD( m->attr );
               if ( m->constval )
                  Free_stringTSD( m->constval );
               if ( m->labels.v )
                  FreeTSD( m->labels.v );
               FreeTSD( m );
            }
         Free_stringTSD( cd->name );
         Free_stringTSD( cd->id );
         if ( cd->supername )
            Free_stringTSD( cd->supername );
         for ( k = 0; k < cd->ninherits && cd->inheritnames; k++ )
            Free_stringTSD( cd->inheritnames[k] );
         if ( cd->inheritnames )
            FreeTSD( cd->inheritnames );
         if ( cd->inherits )
            FreeTSD( cd->inherits );
         if ( cd->mro )
            FreeTSD( cd->mro );
         FreeTSD( cd );
         o->u.c = NULL;
      }
      for ( s = o->scopes; s; s = sn )
      {
         sn = s->next;
         FreeTSD( s );
      }
      if ( o->objname )
         Free_stringTSD( o->objname );
      free_object( TSD, o );
   }
   for ( r = ot->routines; r; r = rn )
   {
      rn = r->next;
      Free_stringTSD( r->name );
      if ( r->labels.v )
         FreeTSD( r->labels.v );
      FreeTSD( r );
   }
   for ( p = ot->programs; p; p = pn )
   {
      pn = p->next;
      DestroyInternalParsingTree( TSD, &p->tree );
      if ( p->file )
         Free_stringTSD( p->file );
      if ( p->labels.v )
         FreeTSD( p->labels.v );
      FreeTSD( p );
   }
   FreeTSD( ot->objs );
   FreeTSD( ot->classes );
   Free_stringTSD( ot->s_self );
   Free_stringTSD( ot->s_super );
   Free_stringTSD( ot->s_string );
   Free_stringTSD( ot->s_init );
   Free_stringTSD( ot->s_unknown );
   Free_stringTSD( ot->s_makearray );
   FreeTSD( ot );
   TSD->oo_tsd = NULL;
   TSD->oo_active = 0;
   TSD->oo_depth = 0;
   TSD->oo_stack_base = NULL;
}

void oo_take_tree( tsd_t *TSD, const internal_parser_type *tree )
{
   oo_tsd *ot = (oo_tsd *) TSD->oo_tsd;
   oo_program *p;

   if ( ot == NULL )
      return;
   for ( p = ot->programs; p; p = p->next )
      if ( p->tree.root == tree->root )
      {
         p->tree = *tree;   /* labels may have been sorted meanwhile */
         return;
      }
}
