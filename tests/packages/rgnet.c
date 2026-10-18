/*
 * rgnet: a small test package written the way RxSock is written (a shared
 * library with SAA external functions that sets stem variables through
 * RexxVariablePool). It is NOT RxSock and has its own function names:
 *
 *   NetLoadFuncs NetHostByName NetSocket NetConnect NetSend NetRecv
 *   NetClose NetDropFuncs
 *
 * tests/run-regression.sh builds it to check that external function
 * packages work with Regina OO (objects, blocks, garbage collection).
 */
#define INCL_RXSHV
#define INCL_RXFUNC
#include "rexxsaa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define FN(n) APIRET APIENTRY n( PCSZ name, ULONG argc, PRXSTRING argv, PCSZ queue, PRXSTRING ret )

static void retstr( PRXSTRING ret, const char *s )
{
   size_t n = strlen( s );
   if ( n >= ret->strlength )
      ret->strptr = (char *) RexxAllocateMemory( n + 1 );
   memcpy( ret->strptr, s, n );
   ret->strlength = n;
}

static void retint( PRXSTRING ret, long v )
{
   char b[32];
   sprintf( b, "%ld", v );
   retstr( ret, b );
}

static char *cstr( RXSTRING r )
{
   char *p = malloc( r.strlength + 1 );
   memcpy( p, r.strptr, r.strlength );
   p[r.strlength] = 0;
   return p;
}

static void setvar( const char *stem, const char *tail, const char *value, size_t len )
{
   SHVBLOCK b;
   char name[256];
   snprintf( name, sizeof( name ), "%s%s", stem, tail );
   memset( &b, 0, sizeof( b ) );
   b.shvcode = RXSHV_SYSET;
   MAKERXSTRING( b.shvname, name, strlen( name ) );
   MAKERXSTRING( b.shvvalue, (char *) value, len );
   RexxVariablePool( &b );
}

static char *getvar( const char *stem, const char *tail )
{
   SHVBLOCK b;
   char name[256];
   char *v;
   snprintf( name, sizeof( name ), "%s%s", stem, tail );
   memset( &b, 0, sizeof( b ) );
   b.shvcode = RXSHV_SYFET;
   MAKERXSTRING( b.shvname, name, strlen( name ) );
   b.shvvalue.strptr = NULL;
   RexxVariablePool( &b );
   v = malloc( b.shvvalue.strlength + 1 );
   memcpy( v, b.shvvalue.strptr, b.shvvalue.strlength );
   v[b.shvvalue.strlength] = 0;
   RexxFreeMemory( b.shvvalue.strptr );
   return v;
}

/* NetHostByName(host, stem): stem||NAME, stem||ADDR, stem||FAMILY */
FN( NetHostByName )
{
   char *host, *stem;
   struct hostent *h;
   if ( argc != 2 ) return 40;
   host = cstr( argv[0] );
   stem = cstr( argv[1] );
   h = gethostbyname( host );
   if ( h == NULL ) { retint( ret, 0 ); free( host ); free( stem ); return 0; }
   setvar( stem, "NAME", h->h_name, strlen( h->h_name ) );
   {
      const char *a = inet_ntoa( *(struct in_addr *) h->h_addr_list[0] );
      setvar( stem, "ADDR", a, strlen( a ) );
   }
   setvar( stem, "FAMILY", "AF_INET", 7 );
   retint( ret, 1 );
   free( host ); free( stem );
   return 0;
}

FN( NetSocket )
{
   retint( ret, socket( AF_INET, SOCK_STREAM, 0 ) );
   return 0;
}

/* NetConnect(s, stem): reads stem||PORT and stem||ADDR */
FN( NetConnect )
{
   struct sockaddr_in a;
   char *s, *stem, *port, *addr;
   if ( argc != 2 ) return 40;
   s = cstr( argv[0] );
   stem = cstr( argv[1] );
   port = getvar( stem, "PORT" );
   addr = getvar( stem, "ADDR" );
   memset( &a, 0, sizeof( a ) );
   a.sin_family = AF_INET;
   a.sin_port = htons( (unsigned short) atoi( port ) );
   a.sin_addr.s_addr = inet_addr( addr );
   retint( ret, connect( atoi( s ), (struct sockaddr *) &a, sizeof( a ) ) );
   free( s ); free( stem ); free( port ); free( addr );
   return 0;
}

FN( NetSend )
{
   char *s;
   if ( argc < 2 ) return 40;
   s = cstr( argv[0] );
   retint( ret, (long) send( atoi( s ), argv[1].strptr, argv[1].strlength, 0 ) );
   free( s );
   return 0;
}

/* NetRecv(s, var, len) */
FN( NetRecv )
{
   char *s, *var, *buf;
   long n, len;
   SHVBLOCK b;
   if ( argc < 3 ) return 40;
   s = cstr( argv[0] );
   var = cstr( argv[1] );
   len = atol( argv[2].strptr );
   buf = malloc( len + 1 );
   n = recv( atoi( s ), buf, len, 0 );
   memset( &b, 0, sizeof( b ) );
   b.shvcode = RXSHV_SYSET;
   MAKERXSTRING( b.shvname, var, strlen( var ) );
   MAKERXSTRING( b.shvvalue, buf, n > 0 ? n : 0 );
   RexxVariablePool( &b );
   retint( ret, n );
   free( s ); free( var ); free( buf );
   return 0;
}

FN( NetClose )
{
   char *s = cstr( argv[0] );
   retint( ret, close( atoi( s ) ) );
   free( s );
   return 0;
}

static const char *fns[] = { "NetHostByName", "NetSocket", "NetConnect", "NetSend",
                             "NetRecv", "NetClose", NULL };

FN( NetDropFuncs )
{
   int i;
   for ( i = 0; fns[i]; i++ )
      RexxDeregisterFunction( fns[i] );
   retstr( ret, "" );
   return 0;
}

FN( NetLoadFuncs )
{
   int i;
   for ( i = 0; fns[i]; i++ )
      RexxRegisterFunctionDll( fns[i], "rgnet", fns[i] );
   RexxRegisterFunctionDll( "NetDropFuncs", "rgnet", "NetDropFuncs" );
   retstr( ret, "" );
   return 0;
}
