/*
 * pybridge.c - the Python subset interpreter of github.com/mikebeer/python
 * (one C89 file, python/py_all.c) compiled into Regina, with the C
 * interface of pybridge.h on top. The Regina side is in oo.c (.Python,
 * .PyObject).
 *
 * py_all.c keeps all its functions static, so this file includes it and
 * adds the interface in the same translation unit. Build with
 * -DNO_PYTHON to leave Python out; then pyb_available() returns 0.
 *
 * Python values handed to Regina are kept in a list that the Python
 * garbage collector sees as a root (allocated as a permanent object), so
 * they stay alive until pyb_release.
 */
#include "pybridge.h"

#ifdef NO_PYTHON

int pyb_available( void ) { return 0; }
int pyb_init( const char *script, const char *exe ) { (void)script; (void)exe; return -1; }
const char *pyb_error( void ) { return "Python is not available in this build"; }
pyref pyb_none( void ) { return -1; }
pyref pyb_str( const char *s, long len ) { (void)s; (void)len; return -1; }
pyref pyb_bytes( const char *s, long len ) { (void)s; (void)len; return -1; }
pyref pyb_number( const char *s, long len ) { (void)s; (void)len; return -1; }
pyref pyb_list( long n, const pyref *i ) { (void)n; (void)i; return -1; }
pyref pyb_dict( long n, const pyref *k, const pyref *v ) { (void)n; (void)k; (void)v; return -1; }
pyref pyb_kwargs( long n, const pyref *k, const pyref *v ) { (void)n; (void)k; (void)v; return -1; }
void pyb_release( pyref r ) { (void)r; }
pyref pyb_dup( pyref r ) { (void)r; return -1; }
int pyb_kind( pyref r ) { (void)r; return PYK_OTHER; }
char *pyb_text( pyref r, long *len ) { (void)r; *len = 0; return 0; }
char *pyb_repr( pyref r, long *len ) { (void)r; *len = 0; return 0; }
char *pyb_typename( pyref r ) { (void)r; return 0; }
long pyb_items( pyref r, pyref **out ) { (void)r; (void)out; return -1; }
long pyb_dict_items( pyref r, pyref **k, pyref **v ) { (void)r; (void)k; (void)v; return -1; }
pyref pyb_import( const char *name ) { (void)name; return -1; }
pyref pyb_eval( const char *s, long n, pyref d ) { (void)s; (void)n; (void)d; return -1; }
int pyb_exec( const char *s, long n ) { (void)s; (void)n; return -1; }
pyref pyb_global( const char *name ) { (void)name; return -1; }
pyref pyb_send( pyref o, const char *nm, long n, const pyref *a )
   { (void)o; (void)nm; (void)n; (void)a; return -1; }
pyref pyb_call( pyref f, long n, const pyref *a ) { (void)f; (void)n; (void)a; return -1; }
pyref pyb_getattr( pyref o, const char *nm ) { (void)o; (void)nm; return -1; }
int pyb_setattr( pyref o, const char *nm, pyref v ) { (void)o; (void)nm; (void)v; return -1; }
pyref pyb_getitem( pyref o, long n, const pyref *k ) { (void)o; (void)n; (void)k; return -1; }
int pyb_setitem( pyref o, long n, const pyref *k, pyref v )
   { (void)o; (void)n; (void)k; (void)v; return -1; }
pyref pyb_op( const char *op, pyref a, pyref b ) { (void)op; (void)a; (void)b; return -1; }
void pyb_set_callback( pyb_callback cb ) { (void)cb; }
pyref pyb_rexx_callable( long id ) { (void)id; return -1; }
int pyb_truth( pyref r ) { (void)r; return -1; }
int pyb_is_mapping( pyref r ) { (void)r; return 0; }

#else

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#if !defined(PY_LIBDIR) && defined(REGINA_SHARE_DIRECTORY)
# define PY_LIBDIR REGINA_SHARE_DIRECTORY "/pylib"
#endif
#if !defined(_WIN32) && !defined(PY_POSIX)
# define PY_POSIX 1
#endif
#ifdef _WIN32
# define PY_PATHLIST_SEP ";"
#endif

/* py_all.c sets the feature macros it needs */
#undef _XOPEN_SOURCE
#undef _POSIX_SOURCE
#define main pyb_py_main
#include "python/py_all.c"
#undef main

/* ------------------------------------------------------------------ */
/* helpers written in Python                                          */
/* ------------------------------------------------------------------ */

static const char rx_helpers[] =
"class _rx_KW(dict):\n"
"    pass\n"
"def _rx_attr(o, name):\n"
"    for n in (name, name.lower()):\n"
"        try:\n"
"            return getattr(o, n)\n"
"        except AttributeError:\n"
"            pass\n"
"    low = name.lower()\n"
"    for n in dir(o):\n"
"        if n.lower() == low:\n"
"            return getattr(o, n)\n"
"    raise AttributeError(\"'%s' object has no attribute '%s'\" % (type(o).__name__, low))\n"
"def _rx_split(args):\n"
"    if args and type(args[-1]) is _rx_KW:\n"
"        return args[:-1], dict(args[-1])\n"
"    return args, {}\n"
"def _rx_send(o, name, args):\n"
"    a = _rx_attr(o, name)\n"
"    args, kw = _rx_split(args)\n"
"    if callable(a):\n"
"        return a(*args, **kw)\n"
"    if args or kw:\n"
"        raise TypeError(\"'%s' object is not callable\" % type(a).__name__)\n"
"    return a\n"
"def _rx_call(f, args):\n"
"    args, kw = _rx_split(args)\n"
"    return f(*args, **kw)\n"
"def _rx_setattr(o, name, v):\n"
"    low = name.lower()\n"
"    for n in dir(o):\n"
"        if n == name or n.lower() == low:\n"
"            setattr(o, n, v)\n"
"            return\n"
"    setattr(o, low, v)\n"
"def _rx_global(name):\n"
"    g = globals()\n"
"    for n in (name, name.lower()):\n"
"        if n in g:\n"
"            return g[n]\n"
"    low = name.lower()\n"
"    for n in g:\n"
"        if n.lower() == low:\n"
"            return g[n]\n"
"    try:\n"
"        return eval(low)\n"
"    except NameError:\n"
"        raise NameError(\"name '%s' is not defined\" % low)\n"
"def _rx_eval(src, names):\n"
"    if names is None:\n"
"        return eval(src)\n"
"    return eval(src, None, names)\n"
"def _rx_key(keys):\n"
"    return keys[0] if len(keys) == 1 else tuple(keys)\n"
"def _rx_getitem(o, keys):\n"
"    return o[_rx_key(keys)]\n"
"def _rx_setitem(o, keys, v):\n"
"    o[_rx_key(keys)] = v\n"
"_rx_ops2 = {'+': lambda a, b: a + b, '-': lambda a, b: a - b,\n"
"    '*': lambda a, b: a * b, '/': lambda a, b: a / b, '%': lambda a, b: a // b,\n"
"    '//': lambda a, b: a % b, '**': lambda a, b: a ** b,\n"
"    '=': lambda a, b: a == b, '==': lambda a, b: a == b,\n"
"    '\\\\=': lambda a, b: a != b, '\\\\==': lambda a, b: a != b,\n"
"    '>': lambda a, b: a > b, '<': lambda a, b: a < b,\n"
"    '>=': lambda a, b: a >= b, '<=': lambda a, b: a <= b,\n"
"    '>>': lambda a, b: a > b, '<<': lambda a, b: a < b,\n"
"    '>>=': lambda a, b: a >= b, '<<=': lambda a, b: a <= b,\n"
"    '&': lambda a, b: a & b, '|': lambda a, b: a | b, '&&': lambda a, b: a ^ b}\n"
"def _rx_op(op, a, b, unary):\n"
"    if unary:\n"
"        if op == '-':\n"
"            return -a\n"
"        if op == '+':\n"
"            return +a\n"
"        return not a\n"
"    f = _rx_ops2.get(op)\n"
"    if f is None:\n"
"        raise TypeError(\"operator %s is not supported for Python objects\" % op)\n"
"    return f(a, b)\n"
"def _rx_items(o):\n"
"    return list(o)\n"
"def _rx_dict_items(d):\n"
"    k = list(d.keys())\n"
"    return [k, [d[x] for x in k]]\n"
"def _rx_ismap(o):\n"
"    return isinstance(o, dict) or (hasattr(o, 'keys') and hasattr(o, '__getitem__'))\n"
"def _rx_truth(o):\n"
"    return bool(o)\n"
"def _rx_import(name):\n"
"    m = __import__(name)\n"
"    for part in name.split('.')[1:]:\n"
"        m = getattr(m, part)\n"
"    return m\n"
"class _rx_Callback:\n"
"    def __init__(self, ref):\n"
"        self.ref = ref\n"
"    def __call__(self, *args, **kw):\n"
"        a = list(args)\n"
"        if kw:\n"
"            a.append(kw)\n"
"        return _rx_rexxcall(self.ref, a)\n"
"    def __repr__(self):\n"
"        return '<Rexx block>'\n"
"def _rx_type(o):\n"
"    return type(o).__name__\n";

enum { H_SEND, H_CALL, H_SETATTR, H_GLOBAL, H_EVAL, H_GETITEM, H_SETITEM, H_OP,
       H_ITEMS, H_DICTITEMS, H_TRUTH, H_TYPE, H_ATTR, H_KW, H_IMPORT, H_ISMAP, H_CALLBACK, H_COUNT };
static const char *helper_names[H_COUNT] = {
   "_rx_send", "_rx_call", "_rx_setattr", "_rx_global", "_rx_eval", "_rx_getitem",
   "_rx_setitem", "_rx_op", "_rx_items", "_rx_dict_items", "_rx_truth", "_rx_type",
   "_rx_attr", "_rx_KW", "_rx_import", "_rx_ismap", "_rx_Callback"
};
static Value helpers[H_COUNT];

/* ------------------------------------------------------------------ */
/* the table of values held by Regina                                 */
/* ------------------------------------------------------------------ */

static List *refs;                /* permanent: a GC root */
static long *freeslots;
static long nfree, capfree;
static int py_state;              /* 0 not started, 1 running, -1 failed */
static char errbuf[1200];
static Value pyb_res;             /* result of the guarded block */

static pyref keep( Value v )
{
   long i;
   if ( nfree > 0 )
   {
      i = freeslots[--nfree];
      refs->a[i] = v;
      return i;
   }
   list_push( refs, v );
   return refs->n - 1;
}

static Value val( pyref r )
{
   if ( r < 0 || r >= refs->n )
      return NONE_V;
   return refs->a[r];
}

void pyb_release( pyref r )
{
   if ( py_state != 1 || r < 0 || r >= refs->n || refs->a[r].t == T_UNSET )
      return;
   refs->a[r] = UNSET_V;
   if ( nfree >= capfree )
   {
      capfree = capfree ? capfree * 2 : 64;
      freeslots = (long *)xrealloc( freeslots, sizeof( long ) * (size_t)capfree );
   }
   freeslots[nfree++] = r;
}

pyref pyb_dup( pyref r )
{
   if ( py_state != 1 )
      return -1;
   return keep( val( r ) );
}

/* ------------------------------------------------------------------ */
/* running Python code safely                                         */
/* ------------------------------------------------------------------ */

static void set_error( void )
{
   Value exc = cur_exc;
   Buf b;
   const char *cn = "Exception";
   if ( exc.t == T_INST )
      cn = AS_INST( exc )->cls->name->s;
   buf_init( &b );
   if ( exc.t == T_INST )
      str_buf( exc, &b );
   if ( b.n > 0 )
      sprintf( errbuf, "%.60s: %.1000s", cn, b.s );
   else
      sprintf( errbuf, "%.60s", cn );
   buf_free( &b );
   cur_exc = NONE_V;
}

/*
 * GUARDED(code) runs code with a Python exception handler; on an
 * exception it restores the interpreter state (the value stack to the
 * variable base of the caller) and returns failed.
 */
#define GUARDED( code, failed ) do { \
      Handler h_; \
      h_.prev = cur_handler; h_.sp = base; h_.depth = call_depth; h_.frame = cur_frame; \
      cur_handler = &h_; \
      if ( setjmp( h_.jb ) == 0 ) { code; cur_handler = h_.prev; } \
      else { \
         cur_handler = h_.prev; vsp = base; call_depth = h_.depth; cur_frame = h_.frame; \
         gc_perm = 0; set_error(); return failed; \
      } \
   } while ( 0 )

/* calls helper k with n arguments, the result in pyb_res */
static void call_helper( int k, int n, Value a0, Value a1, Value a2, Value a3 )
{
   Value *base = vsp;
   vcheck( 8 );
   VPUSH( a0 ); VPUSH( a1 ); VPUSH( a2 ); VPUSH( a3 );
   pyb_res = call_value( helpers[k], n, base, 0, NULL );
   vsp = base;
}

static Value list_of( long n, const pyref *items )
{
   List *l = list_new( n );
   long i;
   for ( i = 0; i < n; i++ )
      list_push( l, val( items[i] ) );
   return mklist( l );
}

int pyb_available( void )
{
   return 1;
}

/* Rexx code called from Python: _rx_rexxcall(id, args) */
static pyb_callback rexx_cb;

void pyb_set_callback( pyb_callback cb )
{
   rexx_cb = cb;
}

static Value bi_rx_rexxcall( Args *x )
{
   long id, n, i;
   pyref *a, r = -1;
   char err[700];
   int rc;
   List *l;
   Value v;

   if ( x->n != 2 || x->a[0].t != T_INT || x->a[1].t != T_LIST || rexx_cb == NULL )
      throw_new( cls_TypeError, "_rx_rexxcall(id, list)" );
   id = x->a[0].u.i;
   l = AS_LIST( x->a[1] );
   n = l->n;
   a = (pyref *) malloc( sizeof( pyref ) * (size_t) ( n ? n : 1 ) );
   for ( i = 0; i < n; i++ )
      a[i] = keep( l->a[i] );
   err[0] = '\0';
   rc = rexx_cb( id, n, a, &r, err, (int) sizeof( err ) );
   for ( i = 0; i < n; i++ )
      pyb_release( a[i] );
   free( a );
   if ( rc != 0 )
      throw_new( cls_RuntimeError, "%s", err );
   v = val( r );
   pyb_release( r );
   return v;
}

const char *pyb_error( void )
{
   return errbuf;
}

static char exe_buf[PY_PATHMAX];

#if defined(_WIN32)
# include <windows.h>
#elif defined(__APPLE__)
# include <mach-o/dyld.h>
#endif

static const char *find_exe( const char *given )
{
#if defined(_WIN32)
   if ( GetModuleFileNameA( NULL, exe_buf, sizeof( exe_buf ) ) > 0 )
      return exe_buf;
#elif defined(__APPLE__)
   uint32_t size = sizeof( exe_buf );
   if ( _NSGetExecutablePath( exe_buf, &size ) == 0 )
      return exe_buf;
#elif defined(PY_POSIX)
   {
      ssize_t n = readlink( "/proc/self/exe", exe_buf, sizeof( exe_buf ) - 1 );
      if ( n > 0 )
      {
         exe_buf[n] = '\0';
         return exe_buf;
      }
   }
#endif
   return given ? given : "";
}

int pyb_init( const char *script, const char *exe )
{
   static char *argv[2];
   static char sbuf[PY_PATHMAX];
   int k;

   if ( py_state )
      return py_state == 1 ? 0 : -1;
   strncpy( sbuf, script ? script : "", sizeof( sbuf ) - 1 );
   argv[0] = sbuf;
   argv[1] = NULL;
   exe_path = find_exe( exe );
#ifdef _WIN32
   {
      /* the interpreter composes paths with '/', which Windows accepts too */
      char *p;
      for ( p = sbuf; *p; p++ ) if ( *p == '\\' ) *p = '/';
      if ( exe_path == exe_buf )
         for ( p = exe_buf; *p; p++ ) if ( *p == '\\' ) *p = '/';
   }
#endif
   init_runtime( 1, argv );
   gc_perm = 1;
   refs = list_new( 64 );
   def_bi( NULL, "_rx_rexxcall", bi_rx_rexxcall );
   gc_perm = 0;
   script_name = "<rexx>";
   py_state = -1;
   if ( run_source( rx_helpers, (long)strlen( rx_helpers ) ) != 0 )
   {
      strcpy( errbuf, "the Python helpers did not start" );
      return -1;
   }
   for ( k = 0; k < H_COUNT; k++ )
   {
      long gi = global_index( prefixed( main_gp, intern_c( helper_names[k] ) ) );
      helpers[k] = G[gi];
      keep( helpers[k] );   /* keeps them alive */
   }
   py_state = 1;
   return 0;
}

/* ------------------------------------------------------------------ */
/* new values                                                         */
/* ------------------------------------------------------------------ */

pyref pyb_none( void )
{
   return keep( NONE_V );
}

pyref pyb_str( const char *s, long len )
{
   return keep( mkstr( s, len ) );
}

pyref pyb_bytes( const char *s, long len )
{
   return keep( mkbytes( s, len ) );
}

/* a Rexx number as Python int or float; -1 if s is no number */
pyref pyb_number( const char *s, long len )
{
   long i = 0, digits = 0;
   int isfloat = 0;
   char tmp[128];
   Value v;

   if ( len <= 0 || len >= (long)sizeof( tmp ) )
      return -1;
   if ( s[i] == '+' || s[i] == '-' )
      i++;
   while ( i < len && s[i] >= '0' && s[i] <= '9' ) { i++; digits++; }
   if ( i < len && s[i] == '.' )
   {
      isfloat = 1;
      i++;
      while ( i < len && s[i] >= '0' && s[i] <= '9' ) { i++; digits++; }
   }
   if ( digits == 0 )
      return -1;
   if ( i < len && ( s[i] == 'e' || s[i] == 'E' ) )
   {
      long e = 0;
      isfloat = 1;
      i++;
      if ( i < len && ( s[i] == '+' || s[i] == '-' ) )
         i++;
      while ( i < len && s[i] >= '0' && s[i] <= '9' ) { i++; e++; }
      if ( e == 0 )
         return -1;
   }
   if ( i != len )
      return -1;
   memcpy( tmp, s, (size_t)len );
   tmp[len] = '\0';
   if ( isfloat )
      v = mkfloat( strtod( tmp, NULL ) );
   else if ( !parse_int( tmp, len, 10, &v ) )
      return -1;
   return keep( v );
}

pyref pyb_list( long n, const pyref *items )
{
   return keep( list_of( n, items ) );
}

pyref pyb_dict( long n, const pyref *keys, const pyref *values )
{
   Value *base = vsp;
   Dict *d = dict_new();
   Value dv = mkobj( d, T_DICT );
   long i;
   VPUSH( dv );
   GUARDED( for ( i = 0; i < n; i++ ) dict_set( d, val( keys[i] ), val( values[i] ) ), -1 );
   vsp = base;
   return keep( dv );
}

pyref pyb_kwargs( long n, const pyref *keys, const pyref *values )
{
   Value *base = vsp;
   pyref d = pyb_dict( n, keys, values ), r;
   if ( d < 0 )
      return -1;
   GUARDED( call_helper( H_KW, 1, val( d ), NONE_V, NONE_V, NONE_V ), -1 );
   pyb_release( d );
   r = keep( pyb_res );
   return r;
}

/* ------------------------------------------------------------------ */
/* inspecting values                                                  */
/* ------------------------------------------------------------------ */

int pyb_kind( pyref r )
{
   switch ( val( r ).t )
   {
      case T_NONE: return PYK_NONE;
      case T_BOOL: return PYK_BOOL;
      case T_INT: case T_BIG: return PYK_INT;
      case T_FLOAT: return PYK_FLOAT;
      case T_STR: return PYK_STR;
      case T_BYTES: return PYK_BYTES;
      case T_LIST: return PYK_LIST;
      case T_TUPLE: return PYK_TUPLE;
      case T_DICT: return PYK_DICT;
      case T_SET: return PYK_SET;
      default: return PYK_OTHER;
   }
}

static char *copy_out( const char *s, long n, long *len )
{
   char *p = (char *)malloc( (size_t)n + 1 );
   if ( p == NULL )
      return NULL;
   memcpy( p, s, (size_t)n );
   p[n] = '\0';
   *len = n;
   return p;
}

char *pyb_text( pyref r, long *len )
{
   Value *base = vsp;
   Value v = val( r );
   if ( v.t == T_STR || v.t == T_BYTES )
      return copy_out( AS_STR( v )->s, AS_STR( v )->len, len );
   GUARDED( pyb_res = py_str( v ), NULL );
   return copy_out( AS_STR( pyb_res )->s, AS_STR( pyb_res )->len, len );
}

char *pyb_repr( pyref r, long *len )
{
   Value *base = vsp;
   GUARDED( pyb_res = py_repr( val( r ) ), NULL );
   return copy_out( AS_STR( pyb_res )->s, AS_STR( pyb_res )->len, len );
}

char *pyb_typename( pyref r )
{
   Value *base = vsp;
   long len;
   GUARDED( call_helper( H_TYPE, 1, val( r ), NONE_V, NONE_V, NONE_V ), NULL );
   return copy_out( AS_STR( pyb_res )->s, AS_STR( pyb_res )->len, &len );
}

static long refs_of_list( Value lv, pyref **out )
{
   List *l = AS_LIST( lv );
   long i, n = l->n;
   VPUSH( lv );
   *out = (pyref *)malloc( sizeof( pyref ) * (size_t)( n ? n : 1 ) );
   for ( i = 0; i < n; i++ )
      ( *out )[i] = keep( l->a[i] );
   vsp--;
   return n;
}

long pyb_items( pyref r, pyref **out )
{
   Value *base = vsp;
   GUARDED( call_helper( H_ITEMS, 1, val( r ), NONE_V, NONE_V, NONE_V ), -1 );
   return refs_of_list( pyb_res, out );
}

long pyb_dict_items( pyref r, pyref **keys, pyref **values )
{
   Value *base = vsp;
   Value two;
   long n;
   GUARDED( call_helper( H_DICTITEMS, 1, val( r ), NONE_V, NONE_V, NONE_V ), -1 );
   two = pyb_res;
   VPUSH( two );
   n = refs_of_list( AS_LIST( two )->a[0], keys );
   refs_of_list( AS_LIST( two )->a[1], values );
   vsp = base;
   return n;
}

int pyb_is_mapping( pyref r )
{
   Value *base = vsp;
   if ( val( r ).t == T_DICT )
      return 1;
   GUARDED( call_helper( H_ISMAP, 1, val( r ), NONE_V, NONE_V, NONE_V ), -1 );
   return pyb_res.t == T_BOOL && pyb_res.u.i;
}

int pyb_truth( pyref r )
{
   Value *base = vsp;
   GUARDED( call_helper( H_TRUTH, 1, val( r ), NONE_V, NONE_V, NONE_V ), -1 );
   return pyb_res.t == T_BOOL && pyb_res.u.i;
}

/* ------------------------------------------------------------------ */
/* running Python                                                     */
/* ------------------------------------------------------------------ */

pyref pyb_import( const char *name )
{
   Value *base = vsp;
   Value s = mkcstr( name );
   VPUSH( s );
   GUARDED( call_helper( H_IMPORT, 1, s, NONE_V, NONE_V, NONE_V ), -1 );
   vsp = base;
   return keep( pyb_res );
}

pyref pyb_eval( const char *src, long len, pyref names )
{
   Value *base = vsp;
   Value s = mkstr( src, len );
   VPUSH( s );
   GUARDED( call_helper( H_EVAL, 2, s, names >= 0 ? val( names ) : NONE_V,
                         NONE_V, NONE_V ), -1 );
   vsp = base;
   return keep( pyb_res );
}

int pyb_exec( const char *src, long len )
{
   Value *base = vsp;
   GUARDED( run_body( src, len ), -1 );
   fflush( stdout );
   return 0;
}

pyref pyb_global( const char *name )
{
   Value *base = vsp;
   Value s = mkcstr( name );
   VPUSH( s );
   GUARDED( call_helper( H_GLOBAL, 1, s, NONE_V, NONE_V, NONE_V ), -1 );
   vsp = base;
   return keep( pyb_res );
}

pyref pyb_send( pyref obj, const char *name, long n, const pyref *args )
{
   Value *base = vsp;
   Value s = mkcstr( name ), a;
   VPUSH( s );
   a = list_of( n, args );
   VPUSH( a );
   GUARDED( call_helper( H_SEND, 3, val( obj ), s, a, NONE_V ), -1 );
   vsp = base;
   fflush( stdout );
   return keep( pyb_res );
}

pyref pyb_call( pyref fn, long n, const pyref *args )
{
   Value *base = vsp;
   Value a = list_of( n, args );
   VPUSH( a );
   GUARDED( call_helper( H_CALL, 2, val( fn ), a, NONE_V, NONE_V ), -1 );
   vsp = base;
   fflush( stdout );
   return keep( pyb_res );
}

pyref pyb_getattr( pyref obj, const char *name )
{
   Value *base = vsp;
   Value s = mkcstr( name );
   VPUSH( s );
   GUARDED( call_helper( H_ATTR, 2, val( obj ), s, NONE_V, NONE_V ), -1 );
   vsp = base;
   return keep( pyb_res );
}

int pyb_setattr( pyref obj, const char *name, pyref value )
{
   Value *base = vsp;
   Value s = mkcstr( name );
   VPUSH( s );
   GUARDED( call_helper( H_SETATTR, 3, val( obj ), s, val( value ), NONE_V ), -1 );
   vsp = base;
   return 0;
}

pyref pyb_getitem( pyref obj, long nkeys, const pyref *keys )
{
   Value *base = vsp;
   Value k = list_of( nkeys, keys );
   VPUSH( k );
   GUARDED( call_helper( H_GETITEM, 2, val( obj ), k, NONE_V, NONE_V ), -1 );
   vsp = base;
   return keep( pyb_res );
}

int pyb_setitem( pyref obj, long nkeys, const pyref *keys, pyref value )
{
   Value *base = vsp;
   Value k = list_of( nkeys, keys );
   VPUSH( k );
   GUARDED( call_helper( H_SETITEM, 3, val( obj ), k, val( value ), NONE_V ), -1 );
   vsp = base;
   return 0;
}

pyref pyb_rexx_callable( long id )
{
   Value *base = vsp;
   GUARDED( call_helper( H_CALLBACK, 1, mkint( id ), NONE_V, NONE_V, NONE_V ), -1 );
   return keep( pyb_res );
}

pyref pyb_op( const char *op, pyref a, pyref b )
{
   Value *base = vsp;
   Value s = mkcstr( op );
   VPUSH( s );
   GUARDED( call_helper( H_OP, 4, s, val( a ), b >= 0 ? val( b ) : NONE_V,
                         b >= 0 ? FALSE_V : TRUE_V ), -1 );
   vsp = base;
   return keep( pyb_res );
}

#endif /* NO_PYTHON */
