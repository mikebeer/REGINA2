/*
 * dialect.h - NetRexx and CREXX notation for Regina OO
 *
 * A program that starts with "options netrexx" (or is read from a file
 * ending in .nrx) or with "options levelg" (CREXX; also "levelb") is
 * translated to Regina OO source before it is parsed. Every line of the
 * translation is the same line of the original, so line numbers in
 * messages and traces stay right.
 */
#ifndef DIALECT_H_INCLUDED
#define DIALECT_H_INCLUDED

#define DIALECT_NONE    0
#define DIALECT_NETREXX 1
#define DIALECT_CREXX  2

/* flags of __regina_dialect_translate */
#define DIALECT_NRX_NAME 1  /* the file name ends in .nrx               */
#define DIALECT_LIBRARY  2  /* read by ::requires: a main method isn't run */

/*
 * Returns the dialect of src (DIALECT_NONE if it's ordinary Rexx). For a
 * dialect *out is the translation (malloc'ed, *outlen bytes, the caller
 * frees it). nrx_name holds the DIALECT_ flags above.
 */
int __regina_dialect_translate( const char *src, long len, int nrx_name,
                                char **out, long *outlen );

#endif
