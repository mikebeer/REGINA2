/*
 * pybridge.h - the embedded Python interpreter as seen from Regina.
 *
 * pybridge.c compiles the Python subset interpreter (python/py_all.c, from
 * github.com/mikebeer/python) into Regina and offers this small C interface.
 * Python values are kept in a table and named by a reference number
 * (pyref); a reference stays valid until pyb_release.
 *
 * Every function that runs Python code returns -1 (or NULL) when Python
 * raised an exception; pyb_error() then gives "TypeError: message".
 */
#ifndef PYBRIDGE_H
#define PYBRIDGE_H

typedef long pyref;

enum {
   PYK_NONE = 0, PYK_BOOL, PYK_INT, PYK_FLOAT, PYK_STR, PYK_BYTES,
   PYK_LIST, PYK_TUPLE, PYK_DICT, PYK_SET, PYK_OTHER
};

int pyb_available( void );
int pyb_init( const char *script, const char *exe );
const char *pyb_error( void );

/* new values */
pyref pyb_none( void );
pyref pyb_str( const char *s, long len );
pyref pyb_bytes( const char *s, long len );
pyref pyb_number( const char *s, long len );   /* int or float, -1 if no number */
pyref pyb_list( long n, const pyref *items );
pyref pyb_dict( long n, const pyref *keys, const pyref *values );
pyref pyb_kwargs( long n, const pyref *keys, const pyref *values );
void pyb_release( pyref r );
pyref pyb_dup( pyref r );

/* inspecting values */
int pyb_kind( pyref r );
char *pyb_text( pyref r, long *len );    /* str(), malloc'd; NULL on error */
char *pyb_repr( pyref r, long *len );
char *pyb_typename( pyref r );
long pyb_items( pyref r, pyref **out );  /* list(r); *out malloc'd; -1 on error */
long pyb_dict_items( pyref r, pyref **keys, pyref **values );

/* running Python */
pyref pyb_import( const char *name );
pyref pyb_eval( const char *src, long len, pyref names );   /* names: dict or -1 */
int pyb_exec( const char *src, long len );
pyref pyb_global( const char *name );                       /* caseless, builtins too */
pyref pyb_send( pyref obj, const char *name, long n, const pyref *args );
pyref pyb_call( pyref fn, long n, const pyref *args );
pyref pyb_getattr( pyref obj, const char *name );
int pyb_setattr( pyref obj, const char *name, pyref value );
pyref pyb_getitem( pyref obj, long nkeys, const pyref *keys );
int pyb_setitem( pyref obj, long nkeys, const pyref *keys, pyref value );
pyref pyb_op( const char *op, pyref a, pyref b );           /* b == -1: unary */
int pyb_truth( pyref r );                                   /* -1 on error */
int pyb_is_mapping( pyref r );                              /* dict, Counter, ... */

#endif
