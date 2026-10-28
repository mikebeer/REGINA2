/*
 * dialect.c - NetRexx and CREXX notation for Regina OO
 *
 * The translation works line by line and keeps the line structure: every
 * output line is the input line with its declarations replaced by Regina
 * OO directives (several on one line, separated by ";") and some symbols
 * rewritten. Comments are kept where they are. See dialect.h and
 * README.OO.md, "Other dialects".
 *
 * NetRexx (options netrexx, *.nrx):
 *   class N [extends S] [implements I, ...] [interface] -> ::class N ...
 *   properties [static|constant] [indirect]     -> ::method __NRXPn [class]
 *     name [= value | = type]                   -> name = value | 0 | .nil
 *   method m[(a, b = 5, c = int)] [static] [abstract] [returns t]
 *                                               -> ::method m [class];
 *                                                  expose props; use arg ...
 *   method N(...)  (the class name)             -> ::method init ...
 *   super(args) / this(args)                    -> self~init:super(args) ...
 *   N(args), N.f(args)                          -> .N(args), .N.f(args)
 *   m(args) for a method of the class           -> self~m(args) / .N~m(args)
 *   loop ...                                    -> do ...
 *   name = type  (a declaration)                -> drop name
 *   type[n]                                     -> .array~new(n)
 *   ask, parse ask                              -> linein(), parse pull
 *   a trailing "-" (continuation)               -> ","
 *   import, package                             -> (removed)
 *
 * CREXX (options levelg, or levelb):
 *   name: class [implements .i, ...]            -> ::class name ...
 *   name: interface                             -> ::class name mixinclass Object
 *   m: method = .type   (in a class)            -> ::method m; expose attributes
 *   m: procedure = .type                        -> ::routine m
 *   *: factory / f: factory                     -> constructor methods
 *   _x = .type  (attribute, in a class)         -> instance variable
 *   arg a = .type, b = 5                        -> use arg a, b = 5 + checks
 *   x = .int  (declaration)                     -> x = 0
 *   import n                                    -> ::requires 'n'
 *   namespace n                                 -> (removed)
 */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "dialect.h"

/* ------------------------------------------------------------------ */
/* small helpers                                                      */
/* ------------------------------------------------------------------ */

typedef struct { char *s; long n, cap; } sbuf;

static void sb_add( sbuf *b, const char *s, long n )
{
   if ( b->n + n + 1 > b->cap )
   {
      long cap = b->cap ? b->cap : 256;
      char *ns;
      while ( b->n + n + 1 > cap )
         cap *= 2;
      ns = (char *) realloc( b->s, cap );
      if ( ns == NULL )
         return;
      b->s = ns;
      b->cap = cap;
   }
   memcpy( b->s + b->n, s, n );
   b->n += n;
   b->s[b->n] = '\0';
}

static void sb_str( sbuf *b, const char *s ) { sb_add( b, s, (long) strlen( s ) ); }
static void sb_ch( sbuf *b, char c ) { sb_add( b, &c, 1 ); }

static int symch( int c )
{
   return isalnum( c ) || c == '_' || c == '.' || c == '!' || c == '?'
       || c == '@' || c == '#' || c == '$';
}

static int symstart( int c )
{
   return isalpha( c ) || c == '_' || c == '!' || c == '?' || c == '@'
       || c == '#' || c == '$';
}

static int eqi( const char *a, long n, const char *b )
{
   long i;
   for ( i = 0; i < n; i++ )
      if ( b[i] == '\0' || tolower( (unsigned char) a[i] ) != tolower( (unsigned char) b[i] ) )
         return 0;
   return b[n] == '\0';
}

static char *dupn( const char *s, long n )
{
   char *r = (char *) malloc( n + 1 );
   memcpy( r, s, n );
   r[n] = '\0';
   return r;
}

/*
 * mask: 0 code, 1 string, 2 comment. Comments nest; "--" runs to the end
 * of the line; strings end at the line end.
 */
static unsigned char *classify( const char *s, long len )
{
   unsigned char *m = (unsigned char *) malloc( len + 1 );
   long i = 0;
   int depth = 0;

   while ( i < len )
   {
      char c = s[i], d = ( i + 1 < len ) ? s[i + 1] : '\0';
      if ( depth )
      {
         m[i] = 2;
         if ( c == '/' && d == '*' )
         {
            m[++i] = 2;
            depth++;
         }
         else if ( c == '*' && d == '/' )
         {
            m[++i] = 2;
            depth--;
         }
         i++;
         continue;
      }
      if ( c == '/' && d == '*' )
      {
         m[i] = m[i + 1] = 2;
         depth = 1;
         i += 2;
         continue;
      }
      if ( c == '-' && d == '-' )
      {
         while ( i < len && s[i] != '\n' && s[i] != '\r' )
            m[i++] = 2;
         continue;
      }
      if ( c == '\'' || c == '"' )
      {
         m[i++] = 1;
         while ( i < len && s[i] != '\n' && s[i] != '\r' )
         {
            m[i] = 1;
            if ( s[i] == c )
            {
               if ( i + 1 < len && s[i + 1] == c )
               {
                  m[++i] = 1;
                  i++;
                  continue;
               }
               i++;
               break;
            }
            i++;
         }
         continue;
      }
      m[i++] = 0;
   }
   return m;
}

/* ------------------------------------------------------------------ */
/* the program structure                                              */
/* ------------------------------------------------------------------ */

typedef struct { char *name; int stat, block, indirect; } dprop;
typedef struct { char *name; int stat, abstract; } dmeth;
typedef struct {
   char *name, *super;
   char **impl; int nimpl;
   int iface, hasctor;
   dprop *props; int nprops;
   dmeth *meths; int nmeths;
   int nblocks; int *blockstatic;
   int line;
   /* CREXX */
   struct lbmem *mem; int nmem;   /* factories, matches, methods        */
   char **attrval;                /* initial value of props[i]          */
} dclass;

/* a member of a CREXX class or interface */
typedef struct lbmem {
   char *name;                    /* "*" for the default factory        */
   int kind;                      /* 'F' factory, 'M' match, 'm' method */
   long line, argline;            /* its line, its "arg" line or -1     */
   int body;                      /* code besides the arg line          */
} lbmem;

typedef struct {
   int dialect;
   const char *src; long len;
   unsigned char *mask;
   long *ls, *le;                 /* line start, end (before CR/LF) */
   long nlines;
   dclass *cls; int ncls;
   /* emit state */
   int cur;                       /* class of the current line, -1 none */
   int inprops, propstatic;       /* in a properties block              */
   int inmeth, methstatic, ctor;  /* in a method                        */
   int pending_props;             /* constructor: init after super(...) */
   int blk;                       /* properties blocks of the class so far */
   int library;                   /* read by ::requires: main isn't run  */
   int cont;                      /* the previous line continues        */
} dctx;

static dclass *find_class( dctx *c, const char *n, long len )
{
   int i;
   for ( i = 0; i < c->ncls; i++ )
      if ( eqi( n, len, c->cls[i].name ) )
         return &c->cls[i];
   return NULL;
}

static dclass *superclass( dctx *c, dclass *k )
{
   if ( k == NULL || k->super == NULL )
      return NULL;
   return find_class( c, k->super, (long) strlen( k->super ) );
}

/* method m of class k or a (local) superclass; *owner gets its class */
static dmeth *find_meth( dctx *c, dclass *k, const char *n, long len, dclass **owner )
{
   int i, j, guard = 0;
   for ( ; k && guard < 64; k = superclass( c, k ), guard++ )
   {
      for ( i = 0; i < k->nmeths; i++ )
         if ( eqi( n, len, k->meths[i].name ) )
         {
            if ( owner )
               *owner = k;
            return &k->meths[i];
         }
      /* the methods of the interfaces it implements */
      for ( j = 0; j < k->nimpl; j++ )
      {
         dclass *ic = find_class( c, k->impl[j], (long) strlen( k->impl[j] ) );
         if ( ic == NULL || ic == k )
            continue;
         for ( i = 0; i < ic->nmeths; i++ )
            if ( eqi( n, len, ic->meths[i].name ) )
            {
               if ( owner )
                  *owner = ic;
               return &ic->meths[i];
            }
      }
   }
   return NULL;
}

static dprop *find_prop( dctx *c, dclass *k, const char *n, long len, dclass **owner )
{
   int i, guard = 0;
   for ( ; k && guard < 64; k = superclass( c, k ), guard++ )
      for ( i = 0; i < k->nprops; i++ )
         if ( eqi( n, len, k->props[i].name ) )
         {
            if ( owner )
               *owner = k;
            return &k->props[i];
         }
   return NULL;
}

/* the code of a line with comments as blanks (strings kept) */
static char *line_code( dctx *c, long ln, unsigned char **m, long *n )
{
   long a = c->ls[ln], e = c->le[ln], i;
   char *r = dupn( c->src + a, e - a );
   if ( m )
      *m = c->mask + a;
   for ( i = a; i < e; i++ )
      if ( c->mask[i] == 2 )
         r[i - a] = ' ';
   if ( n )
      *n = e - a;
   return r;
}

/* the next word of code at *p (a symbol, or one character) */
static long next_word( const char *s, long n, long *p, long *ws )
{
   long i = *p, st;
   while ( i < n && isspace( (unsigned char) s[i] ) )
      i++;
   st = i;
   if ( i < n && symch( (unsigned char) s[i] ) )
      while ( i < n && symch( (unsigned char) s[i] ) )
         i++;
   else if ( i < n )
      i++;
   *ws = st;
   *p = i;
   return i - st;
}

static int first_word_is( const char *s, long n, const char *w )
{
   long p = 0, ws, l = next_word( s, n, &p, &ws );
   return l > 0 && eqi( s + ws, l, w )
       && ( p >= n || !symch( (unsigned char) s[p] ) );
}

static int is_primitive( const char *s, long n )
{
   static const char *t[] = { "int", "long", "short", "byte", "float", "double",
                              "boolean", "char", NULL };
   int i;
   for ( i = 0; t[i]; i++ )
      if ( eqi( s, n, t[i] ) )
         return 1;
   return 0;
}

/*
 * A NetRexx type: a primitive, Rexx, String, Object or a local class,
 * optionally followed by [] (an array). Returns the check kind: 'W' whole
 * number, 'N' number, 'B' boolean, 'C' char, 'A' anything, 0 not a type.
 */
static int nrx_type( dctx *c, const char *s, long n )
{
   long a = 0, e = n, i;
   while ( a < e && isspace( (unsigned char) s[a] ) )
      a++;
   while ( e > a && isspace( (unsigned char) s[e - 1] ) )
      e--;
   if ( e - a >= 2 && s[e - 1] == ']' )
   {
      for ( i = e - 2; i > a && isspace( (unsigned char) s[i] ); i-- )
         ;
      if ( s[i] != '[' )
         return 0;
      e = i;
      while ( e > a && isspace( (unsigned char) s[e - 1] ) )
         e--;
      for ( i = a; i < e; i++ )
         if ( !symch( (unsigned char) s[i] ) )
            return 0;
      return e > a ? 'A' : 0;
   }
   for ( i = a; i < e; i++ )
      if ( !symch( (unsigned char) s[i] ) )
         return 0;
   if ( e == a )
      return 0;
   s += a;
   n = e - a;
   if ( eqi( s, n, "int" ) || eqi( s, n, "long" ) || eqi( s, n, "short" ) || eqi( s, n, "byte" ) )
      return 'W';
   if ( eqi( s, n, "float" ) || eqi( s, n, "double" ) )
      return 'N';
   if ( eqi( s, n, "boolean" ) )
      return 'B';
   if ( eqi( s, n, "char" ) )
      return 'C';
   if ( eqi( s, n, "rexx" ) || eqi( s, n, "string" ) || eqi( s, n, "object" )
     || find_class( c, s, n ) )
      return 'A';
   return 0;
}

static const char *type_name( int k )
{
   switch ( k )
   {
      case 'W': return "int";
      case 'N': return "float";
      case 'B': return "boolean";
      case 'C': return "char";
   }
   return NULL;
}

static const char *strstr_n( const char *s, long n, const char *w )
{
   long i, wl = (long) strlen( w );
   for ( i = 0; i + wl <= n; i++ )
      if ( memcmp( s + i, w, wl ) == 0 )
         return s + i;
   return NULL;
}

/*
 * A CREXX type: .int, .float, .string, .myclass, ... optionally with []
 * (an array). Returns 'W', 'N', 'B', 'S' (string), 'R' (array), 'O'
 * (object) or 0.
 */
static int lb_type( dctx *c, const char *s, long n )
{
   long a = 0, e = n, i;
   (void) c;
   while ( a < e && isspace( (unsigned char) s[a] ) )
      a++;
   while ( e > a && isspace( (unsigned char) s[e - 1] ) )
      e--;
   if ( e - a < 2 || s[a] != '.' || !symstart( (unsigned char) s[a + 1] ) )
      return 0;
   if ( e - a > 2 && s[e - 1] == ']' && s[e - 2] == '[' )
   {
      for ( i = a + 1; i < e - 2; i++ )
         if ( !symch( (unsigned char) s[i] ) )
            return 0;
      return 'R';
   }
   for ( i = a + 1; i < e; i++ )
      if ( !symch( (unsigned char) s[i] ) )
         return 0;
   s += a + 1;
   n = e - a - 1;
   if ( eqi( s, n, "int" ) )
      return 'W';
   if ( eqi( s, n, "float" ) || eqi( s, n, "decimal" ) )
      return 'N';
   if ( eqi( s, n, "boolean" ) )
      return 'B';
   if ( eqi( s, n, "string" ) || eqi( s, n, "binary" ) )
      return 'S';
   return 'O';
}

static const char *lb_default( int kind )
{
   switch ( kind )
   {
      case 'W': case 'N': case 'B': return "0";
      case 'S': return "''";
      case 'R': return ".array~new";
   }
   return ".nil";
}

/* ------------------------------------------------------------------ */
/* expression rewriting                                               */
/* ------------------------------------------------------------------ */

/*
 * Rewrites code s (n chars, mask m) into out. stmt: s starts a statement
 * (line mode; the statement rules apply). Returns 1 if the last code
 * character was a continuation "-".
 */
static int rewrite( dctx *c, const char *s, const unsigned char *m, long n, int stmt, sbuf *out )
{
   dclass *k = c->cur >= 0 ? &c->cls[c->cur] : NULL;
   long i = 0, lastcode = -1, closeat[32];
   int nclose = 0, at_stmt = stmt, linemode = stmt, j, cont = 0;
   int pending_here = 0;
   char prev = '\0', prev2 = '\0';

   for ( i = n - 1; i >= 0; i-- )
      if ( m[i] == 0 && !isspace( (unsigned char) s[i] ) )
      {
         lastcode = i;
         break;
      }
   if ( linemode && lastcode > 0 && s[lastcode] == '-' && s[lastcode - 1] != '-' )
      cont = 1;

   i = 0;
   while ( i < n )
   {
      unsigned char ch = (unsigned char) s[i];
      if ( m[i] != 0 )
      {
         if ( m[i] == 1 )
         {
            prev2 = prev;
            prev = '\'';
            at_stmt = 0;
         }
         sb_ch( out, s[i++] );
         /*
          * NetRexx: '('x is the string and x, Rexx would read a hex string:
          * an explicit concatenation keeps the meaning.
          */
         if ( c->dialect == DIALECT_NETREXX && m[i - 1] == 1 && i < n && m[i] == 0
           && ( s[i] == 'x' || s[i] == 'X' || s[i] == 'b' || s[i] == 'B' )
           && ( i + 1 >= n || !symch( (unsigned char) s[i + 1] ) ) )
            sb_str( out, "||" );
         continue;
      }
      for ( j = 0; j < nclose; j++ )
         if ( closeat[j] == i )
            break;
      if ( j < nclose )
      {
         closeat[j] = closeat[--nclose];
         sb_ch( out, ')' );
         prev2 = prev;
         prev = ')';
         i++;
         continue;
      }
      if ( cont && i == lastcode )
      {
         sb_ch( out, ',' );
         i++;
         continue;
      }
      if ( ch == ';' )
      {
         if ( pending_here )
         {
            /* after super(...): the property initialisation */
            dclass *kk = k;
            int b;
            for ( b = 0; kk && b < kk->nblocks; b++ )
               if ( !kk->blockstatic[b] )
               {
                  char t[32];
                  sb_str( out, "; self~__NRXP" );
                  sprintf( t, "%d", b + 1 );
                  sb_str( out, t );
               }
            pending_here = 0;
         }
         sb_ch( out, ';' );
         i++;
         at_stmt = linemode;
         prev2 = prev;
         prev = ';';
         continue;
      }
      if ( isspace( ch ) )
      {
         sb_ch( out, s[i++] );
         continue;
      }
      if ( symch( ch ) )
      {
         long st = i, l, seg, nx, p2;
         const char *sym;
         char nc, nn = '\0';
         dclass *kc;
         dmeth *me;
         dprop *pr;
         dclass *owner = NULL;

         while ( i < n && m[i] == 0 && symch( (unsigned char) s[i] ) )
            i++;
         sym = s + st;
         l = i - st;
         nc = i < n ? s[i] : '\0';
         for ( nx = i; nx < n && m[nx] == 0 && isspace( (unsigned char) s[nx] ); nx++ )
            ;
         if ( nx < n )
            nn = s[nx];
         for ( seg = 0; seg < l && sym[seg] != '.'; seg++ )
            ;

         if ( c->dialect == DIALECT_CREXX && prev != '~' && sym[0] == '.' && l > 1 )
         {
            /* .ns..name: the namespace is dropped; .iface(args): a provider */
            const char *nm = sym + 1, *dd;
            long nl = l - 1;
            dclass *ic;
            if ( ( dd = strstr_n( nm, nl, ".." ) ) != NULL )
            {
               nl -= ( dd + 2 ) - nm;
               nm = dd + 2;
            }
            for ( seg = 0; seg < nl && nm[seg] != '.'; seg++ )
               ;
            ic = find_class( c, nm, seg );
            sb_ch( out, '.' );
            if ( ic && ic->iface && seg == nl && nc == '(' )
            {
               sb_add( out, nm, nl );
               sb_str( out, "~__LBPICK" );
            }
            else
               sb_add( out, nm, nl );
            prev2 = prev;
            prev = 'a';
            at_stmt = 0;
            continue;
         }
         if ( prev == '~' || !symstart( (unsigned char) sym[0] ) )
         {
            sb_add( out, sym, l );
            prev2 = prev;
            prev = 'a';
            at_stmt = 0;
            continue;
         }

         if ( at_stmt && c->dialect == DIALECT_NETREXX )
         {
            at_stmt = 0;
            if ( eqi( sym, l, "loop" ) )
            {
               long q = i, ws, wl = next_word( s, n, &q, &ws );
               sb_str( out, "do" );
               if ( wl == 3 && eqi( s + ws, 3, "for" ) && m[ws] == 0 )
                  i = q;           /* loop for n -> do n */
               prev2 = prev;
               prev = 'a';
               continue;
            }
            if ( eqi( sym, l, "parse" ) )
            {
               long q = i, ws, wl = next_word( s, n, &q, &ws );
               if ( wl == 3 && eqi( s + ws, 3, "ask" ) && m[ws] == 0 )
               {
                  sb_str( out, "parse pull" );
                  i = q;
                  prev2 = prev;
                  prev = 'a';
                  continue;
               }
            }
            if ( c->inmeth && c->ctor && nc == '(' && eqi( sym, l, "super" ) )
            {
               sb_str( out, "self~init:super" );
               if ( c->pending_props )
               {
                  pending_here = 1;
                  c->pending_props = 0;
               }
               prev2 = prev;
               prev = 'a';
               continue;
            }
            if ( c->inmeth && c->ctor && nc == '(' && eqi( sym, l, "this" ) )
            {
               sb_str( out, "self~init" );
               c->pending_props = 0;
               prev2 = prev;
               prev = 'a';
               continue;
            }
            if ( nn == '=' && !( nx + 1 < n && s[nx + 1] == '=' ) && seg == l )
            {
               /* name = type: a declaration */
               long e = nx + 1;
               while ( e < n && !( m[e] == 0 && s[e] == ';' ) && !( m[e] == 2 ) )
                  e++;
               if ( nrx_type( c, s + nx + 1, e - nx - 1 ) )
               {
                  int ok = 1;
                  long q;
                  for ( q = nx + 1; q < e; q++ )
                     if ( m[q] != 0 )
                        ok = 0;
                  if ( ok )
                  {
                     sb_str( out, "drop " );
                     sb_add( out, sym, l );
                     /* keep the blanks so that the columns stay similar */
                     i = e;
                     prev2 = prev;
                     prev = 'a';
                     continue;
                  }
               }
            }
         }
         if ( at_stmt && c->dialect == DIALECT_CREXX )
         {
            at_stmt = 0;
            if ( eqi( sym, l, "call" ) && k && c->inmeth )
            {
               /* call m(args) for a method of the class: self~m(args) */
               long q = i, ws, wl = next_word( s, n, &q, &ws );
               if ( wl > 0 && q < n && s[q] == '(' && find_meth( c, k, s + ws, wl, NULL ) )
               {
                  while ( i < ws )
                     i++;
                  continue;          /* the method name is written next */
               }
            }
            if ( nn == '=' && !( nx + 1 < n && s[nx + 1] == '=' ) && seg == l )
            {
               /* name = .type: a declaration with the type's initial value */
               long e = nx + 1;
               int kind;
               while ( e < n && !( m[e] == 0 && s[e] == ';' ) && m[e] != 2 )
                  e++;
               if ( ( kind = lb_type( c, s + nx + 1, e - nx - 1 ) ) != 0 )
               {
                  sb_add( out, sym, l );
                  sb_str( out, " = " );
                  sb_str( out, lb_default( kind ) );
                  i = e;
                  prev2 = prev;
                  prev = 'a';
                  continue;
               }
            }
         }
         at_stmt = 0;

         if ( c->dialect == DIALECT_CREXX && nc == '[' && i + 2 < n && s[i + 1] == '0'
           && s[i + 2] == ']' )
         {
            /* a[0]: the number of items */
            sb_add( out, sym, l );
            sb_str( out, "~items()" );
            i += 3;
            prev2 = prev;
            prev = ')';
            continue;
         }

         /* N(args), N.f(...), x <= N, N >= x: the local class N */
         kc = c->dialect == DIALECT_NETREXX ? find_class( c, sym, seg ) : NULL;
         if ( kc && ( seg < l || nc == '(' || ( prev2 == '<' && prev == '=' )
                   || ( nn == '>' && nx + 1 < n && s[nx + 1] == '=' ) ) )
         {
            /* .N~a~b: the periods after the class name are messages */
            long q;
            sb_ch( out, '.' );
            for ( q = 0; q < l; q++ )
               sb_ch( out, ( q >= seg && sym[q] == '.' ) ? '~' : sym[q] );
            prev2 = prev;
            prev = 'a';
            continue;
         }
         /* type[n] -> .array~new(n) */
         if ( seg == l && nc == '[' && is_primitive( sym, l ) )
         {
            int depth = 0;
            for ( p2 = i; p2 < n; p2++ )
            {
               if ( m[p2] != 0 )
                  continue;
               if ( s[p2] == '[' )
                  depth++;
               else if ( s[p2] == ']' && --depth == 0 )
                  break;
            }
            if ( p2 < n && nclose < 32 )
            {
               closeat[nclose++] = p2;
               sb_str( out, ".array~new(" );
               i++;               /* the "[" */
               prev2 = prev;
               prev = '(';
               continue;
            }
         }
         if ( k && c->inmeth && seg == l )
         {
            if ( nc == '(' && ( me = find_meth( c, k, sym, l, &owner ) ) != NULL )
            {
               /* a method of this class: m(args) -> self~m(args) */
               if ( me->stat )
               {
                  sb_ch( out, '.' );
                  sb_str( out, owner->name );
                  sb_ch( out, '~' );
               }
               else
                  sb_str( out, "self~" );
               sb_add( out, sym, l );
               prev2 = prev;
               prev = 'a';
               continue;
            }
            if ( nc != '(' && ( pr = find_prop( c, k, sym, l, &owner ) ) != NULL
              && ( ( pr->stat && !c->methstatic ) || ( !pr->stat && owner != k ) ) )
            {
               /* a static property in an instance method, an inherited one */
               if ( pr->stat )
               {
                  sb_ch( out, '.' );
                  sb_str( out, owner->name );
                  sb_ch( out, '~' );
               }
               else
                  sb_str( out, "self~" );
               sb_add( out, sym, l );
               prev2 = prev;
               prev = 'a';
               continue;
            }
         }
         if ( c->dialect == DIALECT_NETREXX && eqi( sym, l, "ask" ) && nc != '(' && nn != '=' )
         {
            sb_str( out, "linein()" );
            prev2 = prev;
            prev = ')';
            continue;
         }
         sb_add( out, sym, l );
         prev2 = prev;
         prev = 'a';
         continue;
      }
      if ( c->dialect == DIALECT_CREXX && ch == '<' && i + 8 <= n
        && eqi( s + i, 8, "<typeof>" ) )
      {
         sb_str( out, "typeof" );
         i += 8;
         prev2 = prev;
         prev = 'a';
         continue;
      }
      sb_ch( out, s[i] );
      prev2 = prev;
      prev = s[i];
      i++;
   }
   if ( pending_here )
   {
      dclass *kk = k;
      int b;
      for ( b = 0; kk && b < kk->nblocks; b++ )
         if ( !kk->blockstatic[b] )
         {
            char t[32];
            sb_str( out, "; self~__NRXP" );
            sprintf( t, "%d", b + 1 );
            sb_str( out, t );
         }
   }
   return cont;
}

/* rewrites a fragment of code that contains no comments */
static void rewrite_frag( dctx *c, const char *s, long n, sbuf *out )
{
   unsigned char *m = classify( s, n );
   rewrite( c, s, m, n, 0, out );
   free( m );
}

/* ------------------------------------------------------------------ */
/* NetRexx: the first pass collects classes, properties and methods   */
/* ------------------------------------------------------------------ */

static void add_impl( dclass *k, const char *s, long n )
{
   k->impl = (char **) realloc( k->impl, ( k->nimpl + 1 ) * sizeof( char * ) );
   k->impl[k->nimpl++] = dupn( s, n );
}

static void nrx_class_line( dctx *c, const char *s, long n, long ln )
{
   long p = 0, ws, l;
   dclass *k;

   c->cls = (dclass *) realloc( c->cls, ( c->ncls + 1 ) * sizeof( dclass ) );
   k = &c->cls[c->ncls++];
   memset( k, 0, sizeof( *k ) );
   k->line = (int) ln;
   next_word( s, n, &p, &ws );                       /* class */
   l = next_word( s, n, &p, &ws );
   k->name = dupn( s + ws, l );
   while ( ( l = next_word( s, n, &p, &ws ) ) > 0 )
   {
      if ( eqi( s + ws, l, "extends" ) )
      {
         l = next_word( s, n, &p, &ws );
         if ( l > 0 )
            k->super = dupn( s + ws, l );
      }
      else if ( eqi( s + ws, l, "implements" ) )
      {
         for ( ;; )
         {
            l = next_word( s, n, &p, &ws );
            if ( l <= 0 )
               break;
            if ( s[ws] == ',' )
               continue;
            if ( !symch( (unsigned char) s[ws] ) )
               break;
            add_impl( k, s + ws, l );
            {
               long q = p, w2, l2 = next_word( s, n, &q, &w2 );
               if ( l2 == 1 && s[w2] == ',' )
                  p = q;
               else
                  break;
            }
         }
      }
      else if ( eqi( s + ws, l, "interface" ) )
         k->iface = 1;
      else if ( eqi( s + ws, l, "uses" ) )
         break;
   }
}

static void nrx_props_line( dctx *c, const char *s, long n )
{
   long p = 0, ws, l;
   dclass *k = &c->cls[c->cur];

   c->inprops = 1;
   c->propstatic = 0;
   next_word( s, n, &p, &ws );                       /* properties */
   while ( ( l = next_word( s, n, &p, &ws ) ) > 0 )
      if ( eqi( s + ws, l, "static" ) || eqi( s + ws, l, "constant" ) )
         c->propstatic = 1;
   k->blockstatic = (int *) realloc( k->blockstatic, ( k->nblocks + 1 ) * sizeof( int ) );
   k->blockstatic[k->nblocks++] = c->propstatic;
   /* indirect is remembered per block with a negative static marker */
   {
      long q = 0;
      int ind = 0;
      next_word( s, n, &q, &ws );
      while ( ( l = next_word( s, n, &q, &ws ) ) > 0 )
         if ( eqi( s + ws, l, "indirect" ) )
            ind = 1;
      c->inmeth = ind;          /* borrowed until the block's lines are read */
   }
}

static void nrx_prop_stmt( dctx *c, const char *s, long n )
{
   long p = 0, ws, l = next_word( s, n, &p, &ws );
   dclass *k = &c->cls[c->cur];
   dprop *pr;

   if ( l <= 0 || !symstart( (unsigned char) s[ws] ) )
      return;
   k->props = (dprop *) realloc( k->props, ( k->nprops + 1 ) * sizeof( dprop ) );
   pr = &k->props[k->nprops++];
   pr->name = dupn( s + ws, l );
   pr->stat = c->propstatic;
   pr->block = k->nblocks;
   pr->indirect = c->inmeth;
}

/* the parts of a method line */
typedef struct {
   long namest, namelen;
   long argst, arglen;           /* inside the parentheses, -1 none */
   int stat, abstract;
} mline;

static void nrx_parse_method( const char *s, const unsigned char *m, long n, mline *ml )
{
   long p = 0, ws, l;

   memset( ml, 0, sizeof( *ml ) );
   ml->argst = -1;
   next_word( s, n, &p, &ws );                       /* method */
   l = next_word( s, n, &p, &ws );
   ml->namest = ws;
   ml->namelen = l;
   if ( p < n && s[p] == '(' )
   {
      long q = p + 1;
      int depth = 1;
      for ( ; q < n; q++ )
      {
         if ( m[q] != 0 )
            continue;
         if ( s[q] == '(' )
            depth++;
         else if ( s[q] == ')' && --depth == 0 )
            break;
      }
      ml->argst = p + 1;
      ml->arglen = q - p - 1;
      p = q < n ? q + 1 : n;
   }
   while ( ( l = next_word( s, n, &p, &ws ) ) > 0 )
   {
      if ( eqi( s + ws, l, "static" ) )
         ml->stat = 1;
      else if ( eqi( s + ws, l, "abstract" ) )
         ml->abstract = 1;
   }
}

static void nrx_method_line( dctx *c, const char *s, const unsigned char *m, long n )
{
   dclass *k = &c->cls[c->cur];
   dmeth *me;
   mline ml;

   nrx_parse_method( s, m, n, &ml );
   if ( ml.namelen <= 0 )
      return;
   k->meths = (dmeth *) realloc( k->meths, ( k->nmeths + 1 ) * sizeof( dmeth ) );
   me = &k->meths[k->nmeths++];
   me->name = dupn( s + ml.namest, ml.namelen );
   me->stat = ml.stat;
   me->abstract = ml.abstract || k->iface;
   if ( !ml.stat && eqi( s + ml.namest, ml.namelen, k->name ) )
      k->hasctor = 1;
}

/* ------------------------------------------------------------------ */
/* NetRexx: the second pass writes the translation                    */
/* ------------------------------------------------------------------ */

static void expose_list( dclass *k, int stat, sbuf *out )
{
   int i, any = 0;
   for ( i = 0; i < k->nprops; i++ )
      if ( k->props[i].stat == stat )
      {
         sb_str( out, any ? " " : "; expose " );
         sb_str( out, k->props[i].name );
         any = 1;
      }
}

static void props_calls( dclass *k, sbuf *out )
{
   int b;
   char t[32];
   for ( b = 0; b < k->nblocks; b++ )
      if ( !k->blockstatic[b] )
      {
         sb_str( out, "; self~__NRXP" );
         sprintf( t, "%d", b + 1 );
         sb_str( out, t );
      }
}

static void nrx_gen_class( dctx *c, dclass *k, sbuf *out )
{
   int i, any = 0;

   sb_str( out, "::class " );
   sb_str( out, k->name );
   if ( k->iface )
      sb_str( out, " mixinclass Object" );
   else if ( k->super )
   {
      sb_str( out, " subclass " );
      sb_str( out, k->super );
   }
   for ( i = 0; i < k->nimpl; i++ )
   {
      dclass *ic = find_class( c, k->impl[i], (long) strlen( k->impl[i] ) );
      if ( ic == NULL || !ic->iface )
         continue;               /* a Java interface: not known here */
      sb_str( out, any ? " " : " inherit " );
      sb_str( out, ic->name );
      any = 1;
   }
   sb_str( out, " public" );
   for ( i = 0; i < k->nprops; i++ )
   {
      dprop *p = &k->props[i];
      sb_str( out, "; ::attribute " );
      sb_str( out, p->name );
      if ( p->stat )
         sb_str( out, " class" );
      if ( p->indirect && !p->stat )
      {
         char up = (char) toupper( (unsigned char) p->name[0] );
         sb_str( out, "; ::method get" );
         sb_ch( out, up );
         sb_str( out, p->name + 1 );
         sb_str( out, "; expose " );
         sb_str( out, p->name );
         sb_str( out, "; return " );
         sb_str( out, p->name );
         sb_str( out, "; ::method set" );
         sb_ch( out, up );
         sb_str( out, p->name + 1 );
         sb_str( out, "; expose " );
         sb_str( out, p->name );
         sb_str( out, "; use arg " );
         sb_str( out, p->name );
      }
   }
   if ( !k->iface && !k->hasctor )
   {
      int b, inst = 0;
      for ( b = 0; b < k->nblocks; b++ )
         if ( !k->blockstatic[b] )
            inst = 1;
      if ( inst )
      {
         sb_str( out, "; ::method init; self~init:super" );
         props_calls( k, out );
      }
   }
}

/* is the first statement after line ln super(...) or this(...)? */
static int ctor_delegates( dctx *c, long ln )
{
   long i;
   for ( i = ln + 1; i < c->nlines; i++ )
   {
      long n;
      char *s = line_code( c, i, NULL, &n );
      long p = 0, ws, l = next_word( s, n, &p, &ws );
      int r = -1;
      if ( l > 0 )
         r = ( eqi( s + ws, l, "super" ) || eqi( s + ws, l, "this" ) ) && p < n && s[p] == '(';
      free( s );
      if ( r >= 0 )
         return r;
   }
   return 0;
}

static void nrx_gen_method( dctx *c, dclass *k, const char *s, const unsigned char *m, long n,
                            long ln, sbuf *out )
{
   mline ml;
   int abstract, ctor;
   sbuf checks = { NULL, 0, 0 };

   nrx_parse_method( s, m, n, &ml );
   abstract = ml.abstract || k->iface;
   ctor = !ml.stat && eqi( s + ml.namest, ml.namelen, k->name );
   c->inmeth = 1;
   c->methstatic = ml.stat;
   c->ctor = ctor;
   c->pending_props = 0;
   sb_str( out, "::method " );
   if ( ctor )
      sb_str( out, "init" );
   else
      sb_add( out, s + ml.namest, ml.namelen );
   if ( ml.stat )
      sb_str( out, " class" );
   if ( abstract )
   {
      sb_str( out, " abstract" );
      return;
   }
   expose_list( k, ml.stat, out );
   if ( ml.argst >= 0 )
   {
      /* the arguments: name, name = default, name = type */
      long p = ml.argst, e = ml.argst + ml.arglen;
      int first = 1;
      while ( p < e )
      {
         long a = p, q, eq = -1;
         int depth = 0;
         for ( q = p; q < e; q++ )
         {
            if ( m[q] != 0 )
               continue;
            if ( s[q] == '(' || s[q] == '[' )
               depth++;
            else if ( s[q] == ')' || s[q] == ']' )
               depth--;
            else if ( depth == 0 && s[q] == ',' )
               break;
            else if ( depth == 0 && s[q] == '=' && eq < 0 )
               eq = q;
         }
         p = q + 1;
         while ( a < q && isspace( (unsigned char) s[a] ) )
            a++;
         {
            long ne = ( eq >= 0 ? eq : q );
            long nb = a;
            int kind = 0;
            while ( ne > nb && isspace( (unsigned char) s[ne - 1] ) )
               ne--;
            if ( ne <= nb )
               continue;
            sb_str( out, first ? "; use arg " : ", " );
            first = 0;
            sb_add( out, s + nb, ne - nb );
            if ( eq >= 0 )
            {
               long ds = eq + 1, tw;
               kind = nrx_type( c, s + ds, q - ds );
               if ( !kind )
               {
                  /* "by = int 1": a type and a default */
                  while ( ds < q && isspace( (unsigned char) s[ds] ) )
                     ds++;
                  for ( tw = ds; tw < q && symch( (unsigned char) s[tw] ); tw++ )
                     ;
                  if ( tw > ds && tw < q && isspace( (unsigned char) s[tw] )
                    && ( kind = nrx_type( c, s + ds, tw - ds ) ) != 0 )
                     ds = tw;
                  else
                  {
                     kind = 0;
                     ds = eq + 1;
                  }
                  sb_str( out, " = " );
                  rewrite_frag( c, s + ds, q - ds, out );
               }
               if ( kind && type_name( kind ) )
               {
                  sb_str( &checks, "; " );
                  sb_add( &checks, s + nb, ne - nb );
                  sb_str( &checks, " = __OOTYPE(" );
                  sb_add( &checks, s + nb, ne - nb );
                  sb_str( &checks, ", '" );
                  sb_str( &checks, type_name( kind ) );
                  sb_str( &checks, "', '" );
                  sb_add( &checks, s + nb, ne - nb );
                  sb_str( &checks, "')" );
               }
            }
         }
      }
   }
   if ( checks.n )
      sb_add( out, checks.s, checks.n );
   free( checks.s );
   if ( ctor )
   {
      if ( ctor_delegates( c, ln ) )
         c->pending_props = 1;
      else
      {
         sb_str( out, "; self~init:super" );
         props_calls( k, out );
      }
   }
}

static void nrx_gen_props( dctx *c, dclass *k, sbuf *out )
{
   char t[32];
   int i, any = 0, b = c->blk;          /* the number of this block */

   sb_str( out, "::method __NRXP" );
   sprintf( t, "%d", b );
   sb_str( out, t );
   if ( c->propstatic )
      sb_str( out, " class" );
   for ( i = 0; i < k->nprops; i++ )
      if ( k->props[i].block == b )
      {
         sb_str( out, any ? " " : "; expose " );
         sb_str( out, k->props[i].name );
         any = 1;
      }
}

/* a line of a properties block: name [= value | = type] */
static void nrx_gen_prop_line( dctx *c, const char *s, const unsigned char *m, long n, sbuf *out )
{
   long p = 0;
   int any = 0;
   while ( p < n )
   {
      long a = p, q, eq = -1;
      for ( q = p; q < n && !( m[q] == 0 && s[q] == ';' ); q++ )
         if ( m[q] == 0 && s[q] == '=' && eq < 0 )
            eq = q;
      p = q + 1;
      while ( a < q && isspace( (unsigned char) s[a] ) )
         a++;
      if ( a >= q )
         continue;
      {
         long ne = eq >= 0 ? eq : q;
         while ( ne > a && isspace( (unsigned char) s[ne - 1] ) )
            ne--;
         if ( any )
            sb_str( out, "; " );
         any = 1;
         sb_add( out, s + a, ne - a );
         sb_str( out, " = " );
         if ( eq < 0 )
            sb_str( out, ".nil" );
         else
         {
            int kind = nrx_type( c, s + eq + 1, q - eq - 1 );
            if ( kind == 'W' || kind == 'N' || kind == 'B' )
               sb_str( out, "0" );
            else if ( kind == 'C' )
               sb_str( out, "''" );
            else if ( kind )
               sb_str( out, ".nil" );
            else
               rewrite_frag( c, s + eq + 1, q - eq - 1, out );
         }
      }
   }
}

/* the line with its code blanked: the comments stay where they were */
static void blanked( dctx *c, long ln, long from, sbuf *out )
{
   long a = c->ls[ln] + from, e = c->le[ln], i, last = a;
   for ( i = a; i < e; i++ )
      if ( c->mask[i] == 2 )
         last = i + 1;
   for ( i = a; i < last; i++ )
      sb_ch( out, c->mask[i] == 2 ? c->src[i] : ' ' );
}

/* writes generated text for line ln: comments before the first code char,
   the text, then the rest of the line blanked */
static void gen_line( dctx *c, long ln, const char *gen, long glen, sbuf *out )
{
   long a = c->ls[ln], e = c->le[ln], i;
   for ( i = a; i < e && c->mask[i] == 2; i++ )
      sb_ch( out, c->src[i] );
   sb_add( out, gen, glen );
   blanked( c, ln, i - a, out );
}

static void nrx_prologue( dctx *c, int toplevel, sbuf *out )
{
   int i, b;
   char t[32];
   for ( i = 0; i < c->ncls; i++ )
      for ( b = 0; b < c->cls[i].nblocks; b++ )
         if ( c->cls[i].blockstatic[b] )
         {
            sb_ch( out, '.' );
            sb_str( out, c->cls[i].name );
            sb_str( out, "~__NRXP" );
            sprintf( t, "%d", b + 1 );
            sb_str( out, t );
            sb_str( out, "; " );
         }
   if ( !toplevel && !c->library )
      for ( i = 0; i < c->ncls; i++ )
      {
         int j;
         for ( j = 0; j < c->cls[i].nmeths; j++ )
            if ( c->cls[i].meths[j].stat && eqi( c->cls[i].meths[j].name,
                    (long) strlen( c->cls[i].meths[j].name ), "main" ) )
            {
               sb_ch( out, '.' );
               sb_str( out, c->cls[i].name );
               sb_str( out, "~main(arg(1)); exit; " );
               return;
            }
      }
}

static void nrx_translate( dctx *c, sbuf *out )
{
   long ln;
   int toplevel = 0, seen_class = 0;
   long first = 0;

   /* pass 1 */
   c->cur = -1;
   for ( ln = 0; ln < c->nlines; ln++ )
   {
      long n;
      unsigned char *m;
      char *s = line_code( c, ln, &m, &n );
      long p = 0, ws, l = next_word( s, n, &p, &ws );
      if ( l <= 0 )
      {
         free( s );
         continue;
      }
      if ( first_word_is( s, n, "class" ) )
      {
         nrx_class_line( c, s, n, ln );
         c->cur = c->ncls - 1;
         c->inprops = 0;
         seen_class = 1;
      }
      else if ( c->cur >= 0 && first_word_is( s, n, "properties" ) )
         nrx_props_line( c, s, n );
      else if ( c->cur >= 0 && first_word_is( s, n, "method" ) )
      {
         c->inprops = 0;
         nrx_method_line( c, s, m, n );
      }
      else if ( c->cur >= 0 && c->inprops )
      {
         long q = 0;
         while ( q < n )
         {
            long e = q;
            while ( e < n && !( m[e] == 0 && s[e] == ';' ) )
               e++;
            nrx_prop_stmt( c, s + q, e - q );
            q = e + 1;
         }
      }
      else if ( !seen_class && !first_word_is( s, n, "options" )
             && !first_word_is( s, n, "import" ) && !first_word_is( s, n, "package" ) )
         toplevel = 1;
      free( s );
   }

   /* pass 2 */
   c->cur = -1;
   c->inprops = c->inmeth = 0;
   c->cont = 0;
   if ( c->nlines > 0 && c->len >= 2 && c->src[0] == '#' && c->src[1] == '!' )
      first = 1;
   for ( ln = 0; ln < c->nlines; ln++ )
   {
      long n, a = c->ls[ln];
      unsigned char *m;
      char *s = line_code( c, ln, &m, &n );
      sbuf g = { NULL, 0, 0 };

      if ( ln == first )
         nrx_prologue( c, toplevel, out );
      if ( first_word_is( s, n, "class" ) )
      {
         c->cur++;
         c->inprops = c->inmeth = 0;
         nrx_gen_class( c, &c->cls[c->cur], &g );
         gen_line( c, ln, g.s, g.n, out );
         c->blk = 0;
      }
      else if ( c->cur >= 0 && first_word_is( s, n, "properties" ) )
      {
         dclass *k = &c->cls[c->cur];
         long p = 0, ws, l;
         int stat = 0;
         next_word( s, n, &p, &ws );
         while ( ( l = next_word( s, n, &p, &ws ) ) > 0 )
            if ( eqi( s + ws, l, "static" ) || eqi( s + ws, l, "constant" ) )
               stat = 1;
         c->blk++;
         c->propstatic = stat;
         c->inprops = 1;
         c->inmeth = 0;
         nrx_gen_props( c, k, &g );
         gen_line( c, ln, g.s, g.n, out );
      }
      else if ( c->cur >= 0 && first_word_is( s, n, "method" ) )
      {
         c->inprops = 0;
         nrx_gen_method( c, &c->cls[c->cur], s, m, n, ln, &g );
         gen_line( c, ln, g.s, g.n, out );
      }
      else if ( first_word_is( s, n, "import" ) || first_word_is( s, n, "package" ) )
         gen_line( c, ln, "", 0, out );
      else if ( c->cur >= 0 && c->inprops )
      {
         long i;
         int code = 0;
         for ( i = 0; i < n; i++ )
            if ( m[i] != 2 && !isspace( (unsigned char) s[i] ) )
               code = 1;
         if ( code )
         {
            nrx_gen_prop_line( c, s, m, n, &g );
            gen_line( c, ln, g.s, g.n, out );
         }
         else
            sb_add( out, c->src + a, c->le[ln] - a );
      }
      else
      {
         /* code: rewrite it, keeping comments (mask from the source) */
         c->cont = rewrite( c, c->src + a, c->mask + a, c->le[ln] - a, !c->cont, out );
      }
      free( g.s );
      free( s );
      /* the line end as it was */
      {
         long e = c->le[ln], nx = ( ln + 1 < c->nlines ) ? c->ls[ln + 1] : c->len;
         sb_add( out, c->src + e, nx - e );
      }
   }
}

/* ------------------------------------------------------------------ */
/* entry point                                                        */
/* ------------------------------------------------------------------ */

static int detect( const char *s, long len, const unsigned char *m )
{
   long i = 0;
   if ( len >= 2 && s[0] == '#' && s[1] == '!' )
      while ( i < len && s[i] != '\n' )
         i++;
   for ( ; i < len; i++ )
   {
      if ( m[i] != 0 || isspace( (unsigned char) s[i] ) || s[i] == ';' )
         continue;
      if ( i + 7 <= len && eqi( s + i, 7, "options" ) && ( i + 7 == len || !symch( (unsigned char) s[i + 7] ) ) )
      {
         long p = i + 7;
         while ( p < len && s[p] != '\n' && s[p] != ';' && m[p] != 2 )
         {
            long a;
            while ( p < len && ( s[p] == ' ' || s[p] == '\t' ) )
               p++;
            a = p;
            while ( p < len && symch( (unsigned char) s[p] ) )
               p++;
            if ( p - a == 7 && eqi( s + a, 7, "netrexx" ) )
               return DIALECT_NETREXX;
            if ( p - a == 6 && ( eqi( s + a, 6, "levelg" ) || eqi( s + a, 6, "levelb" ) ) )
               return DIALECT_CREXX;
            if ( p == a )
               p++;
         }
      }
      return DIALECT_NONE;
   }
   return DIALECT_NONE;
}

static void lb_translate( dctx *c, sbuf *out );

int __regina_dialect_translate( const char *src, long len, int nrx_name,
                                char **out, long *outlen )
{
   dctx c;
   sbuf o = { NULL, 0, 0 };
   long i, n;
   int d, k;

   *out = NULL;
   *outlen = 0;
   memset( &c, 0, sizeof( c ) );
   c.mask = classify( src, len );
   d = detect( src, len, c.mask );
   if ( d == DIALECT_NONE && ( nrx_name & DIALECT_NRX_NAME ) )
      d = DIALECT_NETREXX;
   c.library = ( nrx_name & DIALECT_LIBRARY ) != 0;
   if ( d == DIALECT_NONE )
   {
      free( c.mask );
      return DIALECT_NONE;
   }
   c.dialect = d;
   c.src = src;
   c.len = len;
   for ( i = 0, n = 1; i < len; i++ )
      if ( src[i] == '\n' )
         n++;
   c.ls = (long *) malloc( ( n + 1 ) * sizeof( long ) );
   c.le = (long *) malloc( ( n + 1 ) * sizeof( long ) );
   c.nlines = 0;
   for ( i = 0; i <= len; )
   {
      long a = i, e;
      while ( i < len && src[i] != '\n' )
         i++;
      e = i;
      if ( e > a && src[e - 1] == '\r' )
         e--;
      if ( a == len && c.nlines > 0 )
         break;
      c.ls[c.nlines] = a;
      c.le[c.nlines] = e;
      c.nlines++;
      i++;
   }
   if ( d == DIALECT_NETREXX )
      nrx_translate( &c, &o );
   else
      lb_translate( &c, &o );
   if ( o.s == NULL )
      sb_add( &o, "", 0 );

   for ( k = 0; k < c.ncls; k++ )
   {
      dclass *kc = &c.cls[k];
      int j;
      free( kc->name );
      free( kc->super );
      for ( j = 0; j < kc->nimpl; j++ )
         free( kc->impl[j] );
      free( kc->impl );
      for ( j = 0; j < kc->nprops; j++ )
         free( kc->props[j].name );
      free( kc->props );
      for ( j = 0; j < kc->nmeths; j++ )
         free( kc->meths[j].name );
      free( kc->meths );
      free( kc->blockstatic );
      for ( j = 0; j < kc->nmem; j++ )
         free( kc->mem[j].name );
      free( kc->mem );
      if ( kc->attrval )
      {
         for ( j = 0; j < kc->nprops; j++ )
            free( kc->attrval[j] );
         free( kc->attrval );
      }
   }
   free( c.cls );
   free( c.ls );
   free( c.le );
   free( c.mask );
   *out = o.s;
   *outlen = o.n;
   return d;
}

/* ------------------------------------------------------------------ */
/* CREXX                                                      */
/* ------------------------------------------------------------------ */

/*
 * "name: kind ..." at the start of a line. Returns 'C' class, 'I'
 * interface, 'F' factory, 'M' match, 'm' method, 'P' procedure or 0; the
 * name in s[*ns], *nl chars ("*" for the default factory) and the
 * position after the keyword in *rest.
 */
static int lb_decl( const char *s, long n, long *ns, long *nl, long *rest )
{
   long i = 0, a, e, p, ws, l;
   int k = 0;

   while ( i < n && isspace( (unsigned char) s[i] ) )
      i++;
   a = i;
   if ( i < n && s[i] == '*' )
      i++;
   else
   {
      if ( i >= n || !symstart( (unsigned char) s[i] ) )
         return 0;
      while ( i < n && symch( (unsigned char) s[i] ) )
         i++;
   }
   e = i;
   while ( i < n && ( s[i] == ' ' || s[i] == '\t' ) )
      i++;
   if ( i >= n || s[i] != ':' || ( i + 1 < n && s[i + 1] == ':' ) )
      return 0;
   p = i + 1;
   l = next_word( s, n, &p, &ws );
   if ( l <= 0 )
      return 0;
   if ( eqi( s + ws, l, "class" ) )
      k = 'C';
   else if ( eqi( s + ws, l, "interface" ) )
      k = 'I';
   else if ( eqi( s + ws, l, "factory" ) )
      k = 'F';
   else if ( eqi( s + ws, l, "match" ) )
      k = 'M';
   else if ( eqi( s + ws, l, "method" ) )
      k = 'm';
   else if ( eqi( s + ws, l, "procedure" ) )
      k = 'P';
   if ( k == 0 || ( s[a] == '*' && k != 'F' && k != 'M' ) )
      return 0;
   if ( p < n && symch( (unsigned char) s[p] ) )
      return 0;
   *ns = a;
   *nl = e - a;
   *rest = p;
   return k;
}

static lbmem *lb_member( dclass *k, int kind, const char *name, long len )
{
   int i;
   for ( i = 0; k && i < k->nmem; i++ )
      if ( k->mem[i].kind == kind && eqi( name, len, k->mem[i].name ) )
         return &k->mem[i];
   return NULL;
}

/* the interface of class k that declares factory f */
static lbmem *lb_iface_factory( dctx *c, dclass *k, const char *f, dclass **ifc )
{
   int j;
   for ( j = 0; j < k->nimpl; j++ )
   {
      dclass *ic = find_class( c, k->impl[j], (long) strlen( k->impl[j] ) );
      lbmem *m;
      if ( ic && ic->iface && ( m = lb_member( ic, 'F', f, (long) strlen( f ) ) ) != NULL )
      {
         if ( ifc )
            *ifc = ic;
         return m;
      }
   }
   return NULL;
}

/*
 * The arguments of an "arg" line: "use arg a, b = 5" (and checks) when
 * full, else just the names "a, b".
 */
static void lb_args( dctx *c, long ln, int full, sbuf *out )
{
   long n, p = 0, ws, l, e;
   unsigned char *m;
   char *s;
   int first = 1;
   sbuf checks = { NULL, 0, 0 };

   if ( ln < 0 )
      return;
   s = line_code( c, ln, &m, &n );
   next_word( s, n, &p, &ws );                       /* arg */
   {
      long q = p;
      l = next_word( s, n, &q, &ws );
      if ( l == 6 && eqi( s + ws, 6, "expose" ) )
         p = q;
   }
   e = n;
   for ( l = p; l < n; l++ )
      if ( m[l] == 0 && s[l] == ';' )
      {
         e = l;
         break;
      }
   while ( p < e )
   {
      long a = p, q, eq = -1;
      int depth = 0;
      for ( q = p; q < e; q++ )
      {
         if ( m[q] != 0 )
            continue;
         if ( s[q] == '(' || s[q] == '[' )
            depth++;
         else if ( s[q] == ')' || s[q] == ']' )
            depth--;
         else if ( depth == 0 && s[q] == ',' )
            break;
         else if ( depth == 0 && s[q] == '=' && eq < 0 )
            eq = q;
      }
      p = q + 1;
      while ( a < q && isspace( (unsigned char) s[a] ) )
         a++;
      {
         long ne = eq >= 0 ? eq : q;
         int kind;
         while ( ne > a && isspace( (unsigned char) s[ne - 1] ) )
            ne--;
         if ( ne <= a )
            continue;
         sb_str( out, first ? ( full ? "use arg " : "" ) : ", " );
         first = 0;
         sb_add( out, s + a, ne - a );
         if ( !full || eq < 0 )
            continue;
         kind = lb_type( c, s + eq + 1, q - eq - 1 );
         if ( !kind )
         {
            sb_str( out, " = " );
            rewrite_frag( c, s + eq + 1, q - eq - 1, out );
         }
         else if ( kind == 'W' || kind == 'N' || kind == 'B' )
         {
            sb_str( &checks, "; " );
            sb_add( &checks, s + a, ne - a );
            sb_str( &checks, " = __OOTYPE(" );
            sb_add( &checks, s + a, ne - a );
            sb_str( &checks, ", '" );
            sb_str( &checks, kind == 'W' ? "int" : kind == 'N' ? "float" : "boolean" );
            sb_str( &checks, "', '" );
            sb_add( &checks, s + a, ne - a );
            sb_str( &checks, "')" );
         }
      }
   }
   if ( checks.n )
      sb_add( out, checks.s, checks.n );
   free( checks.s );
   free( s );
}

static void lb_expose( dclass *k, sbuf *out )
{
   int i;
   for ( i = 0; i < k->nprops; i++ )
   {
      sb_str( out, i ? " " : "; expose " );
      sb_str( out, k->props[i].name );
   }
}

static int cmp_name( const void *a, const void *b )
{
   const dclass *x = *(const dclass * const *) a, *y = *(const dclass * const *) b;
   const char *p = x->name, *q = y->name;
   for ( ; *p && *q; p++, q++ )
      if ( tolower( (unsigned char) *p ) != tolower( (unsigned char) *q ) )
         return tolower( (unsigned char) *p ) - tolower( (unsigned char) *q );
   return (unsigned char) *p - (unsigned char) *q;
}

/* the class method of an interface that selects the provider of factory f */
static void lb_picker( dctx *c, dclass *ic, lbmem *f, sbuf *out )
{
   dclass **cand = (dclass **) malloc( ( c->ncls + 1 ) * sizeof( dclass * ) );
   int nc = 0, i, j, dflt = ( f->name[0] == '*' );
   sbuf names = { NULL, 0, 0 };

   lb_args( c, f->argline, 0, &names );
   if ( names.s == NULL )
      sb_add( &names, "", 0 );
   for ( i = 0; i < c->ncls; i++ )
   {
      dclass *k = &c->cls[i];
      if ( k->iface )
         continue;
      for ( j = 0; j < k->nimpl; j++ )
         if ( eqi( k->impl[j], (long) strlen( k->impl[j] ), ic->name ) )
            break;
      if ( j == k->nimpl )
         continue;
      if ( !dflt && !lb_member( k, 'F', f->name, (long) strlen( f->name ) ) )
         continue;
      cand[nc++] = k;
   }
   qsort( cand, nc, sizeof( dclass * ), cmp_name );
   sb_str( out, "; ::method " );
   sb_str( out, dflt ? "__LBPICK" : f->name );
   sb_str( out, " class" );
   if ( names.n )
   {
      sb_str( out, "; use arg " );
      sb_str( out, names.s );
   }
   sb_str( out, "; b = .nil; s = 0" );
   for ( i = 0; i < nc; i++ )
   {
      if ( lb_member( cand[i], 'M', f->name, (long) strlen( f->name ) ) )
      {
         sb_str( out, "; t = ." );
         sb_str( out, cand[i]->name );
         sb_str( out, "~__LBM_" );
         if ( !dflt )
            sb_str( out, f->name );
         sb_ch( out, '(' );
         sb_str( out, names.s );
         sb_ch( out, ')' );
      }
      else
         sb_str( out, "; t = 1" );
      sb_str( out, "; if t > s then do; b = ." );
      sb_str( out, cand[i]->name );
      sb_str( out, "; s = t; end" );
   }
   sb_str( out, "; if b == .nil then return .nil; return b~" );
   sb_str( out, dflt ? "new" : f->name );
   sb_ch( out, '(' );
   sb_str( out, names.s );
   sb_ch( out, ')' );
   free( names.s );
   free( cand );
}

static void lb_gen_class( dctx *c, dclass *k, sbuf *out )
{
   int i, any = 0;

   sb_str( out, "::class " );
   sb_str( out, k->name );
   if ( k->iface )
      sb_str( out, " mixinclass Object" );
   for ( i = 0; i < k->nimpl; i++ )
   {
      dclass *ic = find_class( c, k->impl[i], (long) strlen( k->impl[i] ) );
      if ( ic == NULL || !ic->iface || ic == k )
         continue;
      sb_str( out, any ? " " : " inherit " );
      sb_str( out, ic->name );
      any = 1;
   }
   sb_str( out, " public" );
   if ( k->iface )
   {
      for ( i = 0; i < k->nmem; i++ )
         if ( k->mem[i].kind == 'F' )
            lb_picker( c, k, &k->mem[i], out );
      return;
   }
   if ( k->nprops )
   {
      sb_str( out, "; ::method __LBA" );
      lb_expose( k, out );
      for ( i = 0; i < k->nprops; i++ )
      {
         int kind = k->attrval[i] ? lb_type( c, k->attrval[i], (long) strlen( k->attrval[i] ) ) : 0;
         sb_str( out, "; " );
         sb_str( out, k->props[i].name );
         sb_str( out, " = " );
         if ( kind || k->attrval[i] == NULL )
            sb_str( out, lb_default( kind ) );
         else
            rewrite_frag( c, k->attrval[i], (long) strlen( k->attrval[i] ), out );
      }
   }
   if ( !lb_member( k, 'F', "*", 1 ) )
   {
      sb_str( out, "; ::method init" );
      if ( k->nprops )
         sb_str( out, "; self~__LBA" );
   }
}

static void lb_gen_member( dctx *c, dclass *k, lbmem *mb, sbuf *out )
{
   int dflt = ( mb->name[0] == '*' );
   lbmem *im = NULL;

   if ( k->iface )
   {
      if ( mb->kind == 'm' )
      {
         sb_str( out, "::method " );
         sb_str( out, mb->name );
         if ( !mb->body )
            sb_str( out, " abstract" );
      }
      return;                   /* factories: see lb_picker */
   }
   if ( mb->argline < 0 && ( mb->kind == 'F' || mb->kind == 'M' ) )
      im = lb_iface_factory( c, k, mb->name, NULL );
   switch ( mb->kind )
   {
      case 'F':
         if ( dflt )
         {
            sb_str( out, "::method init" );
            lb_expose( k, out );
            if ( k->nprops )
               sb_str( out, "; self~__LBA" );
            sb_str( out, "; if arg() = 1 then if arg(1) == '__LBNOINIT__' then return" );
         }
         else
         {
            sbuf names = { NULL, 0, 0 };
            lb_args( c, mb->argline >= 0 ? mb->argline : ( im ? im->argline : -1 ), 0, &names );
            if ( names.s == NULL )
               sb_add( &names, "", 0 );
            sb_str( out, "::method " );
            sb_str( out, mb->name );
            sb_str( out, " class" );
            if ( names.n )
            {
               sb_str( out, "; use arg " );
               sb_str( out, names.s );
            }
            sb_str( out, "; o = self~new('__LBNOINIT__'); o~__LBF_" );
            sb_str( out, mb->name );
            sb_ch( out, '(' );
            sb_str( out, names.s );
            sb_str( out, "); return o; ::method __LBF_" );
            sb_str( out, mb->name );
            lb_expose( k, out );
            free( names.s );
         }
         break;
      case 'M':
         sb_str( out, "::method __LBM_" );
         if ( !dflt )
            sb_str( out, mb->name );
         sb_str( out, " class" );
         break;
      default:
         sb_str( out, "::method " );
         sb_str( out, mb->name );
         if ( mb->body )
            lb_expose( k, out );
         else
            sb_str( out, " abstract" );
   }
   if ( im && im->argline >= 0 )
   {
      /* the class factory takes the arguments of the interface's */
      sb_str( out, "; " );
      lb_args( c, im->argline, 1, out );
   }
}

static void lb_translate( dctx *c, sbuf *out )
{
   long ln, first = 0;
   int cur = -1, curmem = -1, toplevel = 0, seen = 0, mainproc = 0, j;
   char **imports = NULL;
   int nimports = 0;

   /* pass 1 */
   for ( ln = 0; ln < c->nlines; ln++ )
   {
      long n, ns, nl, rest, p = 0, ws, l;
      unsigned char *m;
      char *s = line_code( c, ln, &m, &n );
      int kind;

      l = next_word( s, n, &p, &ws );
      if ( l <= 0 )
      {
         free( s );
         continue;
      }
      kind = lb_decl( s, n, &ns, &nl, &rest );
      if ( kind == 'C' || kind == 'I' )
      {
         dclass *k;
         c->cls = (dclass *) realloc( c->cls, ( c->ncls + 1 ) * sizeof( dclass ) );
         k = &c->cls[c->ncls++];
         memset( k, 0, sizeof( *k ) );
         k->name = dupn( s + ns, nl );
         k->iface = ( kind == 'I' );
         k->line = (int) ln;
         p = rest;
         while ( ( l = next_word( s, n, &p, &ws ) ) > 0 )
         {
            if ( eqi( s + ws, l, "implements" ) || s[ws] == ',' )
               continue;
            if ( s[ws] == '.' && l > 1 )
            {
               const char *nm = s + ws + 1, *dd = strstr_n( nm, l - 1, ".." );
               long nn = l - 1;
               if ( dd )
               {
                  nn -= ( dd + 2 ) - nm;
                  nm = dd + 2;
               }
               add_impl( k, nm, nn );
            }
         }
         cur = c->ncls - 1;
         curmem = -1;
         seen = 1;
      }
      else if ( kind == 'P' )
      {
         cur = -1;
         curmem = -1;
         seen = 1;
         if ( eqi( s + ns, nl, "main" ) )
            mainproc = 1;
      }
      else if ( kind && cur >= 0 )
      {
         dclass *k = &c->cls[cur];
         lbmem *mb;
         k->mem = (lbmem *) realloc( k->mem, ( k->nmem + 1 ) * sizeof( lbmem ) );
         mb = &k->mem[k->nmem];
         curmem = k->nmem++;
         mb->name = dupn( s + ns, nl );
         mb->kind = kind;
         mb->line = ln;
         mb->argline = -1;
         mb->body = 0;
         if ( kind == 'm' )
         {
            k->meths = (dmeth *) realloc( k->meths, ( k->nmeths + 1 ) * sizeof( dmeth ) );
            k->meths[k->nmeths].name = dupn( s + ns, nl );
            k->meths[k->nmeths].stat = 0;
            k->meths[k->nmeths].abstract = 0;
            k->nmeths++;
         }
      }
      else if ( !seen )
      {
         if ( !eqi( s + ws, l, "options" ) && !eqi( s + ws, l, "namespace" )
           && !eqi( s + ws, l, "import" ) )
            toplevel = 1;
      }
      else if ( cur >= 0 && curmem >= 0 )
      {
         lbmem *mb = &c->cls[cur].mem[curmem];
         if ( eqi( s + ws, l, "arg" ) && mb->argline < 0 && !mb->body )
            mb->argline = ln;
         else
            mb->body = 1;
      }
      else if ( cur >= 0 && !c->cls[cur].iface )
      {
         /* an attribute: _name = .type | value */
         dclass *k = &c->cls[cur];
         long q;
         if ( symstart( (unsigned char) s[ws] ) )
         {
            k->props = (dprop *) realloc( k->props, ( k->nprops + 1 ) * sizeof( dprop ) );
            k->attrval = (char **) realloc( k->attrval, ( k->nprops + 1 ) * sizeof( char * ) );
            memset( &k->props[k->nprops], 0, sizeof( dprop ) );
            k->props[k->nprops].name = dupn( s + ws, l );
            k->attrval[k->nprops] = NULL;
            for ( q = p; q < n && isspace( (unsigned char) s[q] ); q++ )
               ;
            if ( q < n && s[q] == '=' )
            {
               long e = q + 1, w;
               while ( e < n && !( m[e] == 0 && s[e] == ';' ) )
                  e++;
               /* "with register.N" is a VM layout: not needed here */
               for ( w = q + 1; w + 4 < e; w++ )
                  if ( m[w] == 0 && eqi( s + w, 4, "with" ) && isspace( (unsigned char) s[w - 1] )
                    && isspace( (unsigned char) s[w + 4] ) )
                  {
                     e = w;
                     break;
                  }
               k->attrval[k->nprops] = dupn( s + q + 1, e - q - 1 );
            }
            k->nprops++;
         }
      }
      free( s );
   }

   /* pass 2 */
   cur = -1;
   curmem = -1;
   if ( c->len >= 2 && c->src[0] == '#' && c->src[1] == '!' )
      first = 1;
   {
      int inproc = 0;
      long procarg = -1;
      for ( ln = 0; ln < c->nlines; ln++ )
      {
         long n, ns, nl, rest, p = 0, ws, l, a = c->ls[ln];
         unsigned char *m;
         char *s = line_code( c, ln, &m, &n );
         sbuf g = { NULL, 0, 0 };
         int kind = lb_decl( s, n, &ns, &nl, &rest );

         l = next_word( s, n, &p, &ws );
         if ( ln == first && mainproc && !toplevel )
            sb_str( out, "call main arg(1); exit; " );
         if ( kind == 'C' || kind == 'I' )
         {
            cur++;
            curmem = -1;
            inproc = 0;
            c->cur = cur;
            c->inmeth = 0;
            lb_gen_class( c, &c->cls[cur], &g );
            gen_line( c, ln, g.s, g.n, out );
         }
         else if ( kind == 'P' )
         {
            cur = -1;
            curmem = -1;
            c->cur = -1;
            c->inmeth = 0;
            inproc = 1;
            procarg = ln;
            sb_str( &g, "::routine " );
            sb_add( &g, s + ns, nl );
            gen_line( c, ln, g.s, g.n, out );
         }
         else if ( kind && cur >= 0 )
         {
            dclass *k = &c->cls[cur];
            lbmem *mb = &k->mem[++curmem];
            c->inmeth = 1;
            c->methstatic = 0;
            lb_gen_member( c, k, mb, &g );
            gen_line( c, ln, g.s, g.n, out );
         }
         else if ( l > 0 && ( eqi( s + ws, l, "namespace" ) || eqi( s + ws, l, "import" ) ) )
         {
            if ( eqi( s + ws, l, "import" ) )
            {
               long q = p, w2, l2 = next_word( s, n, &q, &w2 );
               if ( l2 > 0 && !( l2 > 2 && tolower( (unsigned char) s[w2] ) == 'r'
                                 && tolower( (unsigned char) s[w2 + 1] ) == 'x' ) )
               {
                  imports = (char **) realloc( imports, ( nimports + 1 ) * sizeof( char * ) );
                  imports[nimports++] = dupn( s + w2, l2 );
               }
            }
            gen_line( c, ln, "", 0, out );
         }
         else if ( l > 0 && cur >= 0 && curmem >= 0 && c->cls[cur].mem[curmem].argline == ln )
         {
            dclass *k = &c->cls[cur];
            if ( !k->iface )
               lb_args( c, ln, 1, &g );
            gen_line( c, ln, g.s ? g.s : "", g.n, out );
         }
         else if ( l > 0 && inproc && procarg >= 0 && eqi( s + ws, l, "arg" ) )
         {
            lb_args( c, ln, 1, &g );
            gen_line( c, ln, g.s, g.n, out );
            procarg = -1;
         }
         else if ( l > 0 && cur >= 0 && curmem < 0 )
            gen_line( c, ln, "", 0, out );       /* an attribute */
         else if ( l > 0 && cur >= 0 && c->cls[cur].iface && curmem >= 0
                && c->cls[cur].mem[curmem].kind != 'm' )
            gen_line( c, ln, "", 0, out );       /* an interface factory's lines */
         else
         {
            if ( l > 0 )
               procarg = -1;
            c->cont = rewrite( c, c->src + a, c->mask + a, c->le[ln] - a, !c->cont, out );
         }
         free( g.s );
         free( s );
         {
            long e = c->le[ln], nx = ( ln + 1 < c->nlines ) ? c->ls[ln + 1] : c->len;
            sb_add( out, c->src + e, nx - e );
         }
      }
   }
   if ( nimports )
   {
      if ( c->len > 0 && c->src[c->len - 1] != '\n' )
         sb_ch( out, '\n' );
      for ( j = 0; j < nimports; j++ )
      {
         sb_str( out, "::requires '" );
         sb_str( out, imports[j] );
         sb_str( out, "'\n" );
         free( imports[j] );
      }
      free( imports );
   }
}
