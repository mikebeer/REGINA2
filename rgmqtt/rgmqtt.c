/*
 * RgMqtt - an MQTT 3.1.1 client for Regina Rexx.
 *
 * An external function package like RegUtil or RxSock: a shared library
 * (librgmqtt.so, rgmqtt.dll) with SAA external functions. It needs no other
 * library than the system's sockets.
 *
 *   call RxFuncAdd 'MqttLoadFuncs', 'rgmqtt', 'MqttLoadFuncs'
 *   call MqttLoadFuncs
 *   h = MqttConnect('localhost', 1883, 'my-client')
 *   call MqttSubscribe h, 'sensors/#', 1
 *   call MqttPublish h, 'sensors/kitchen', '21.5', 1
 *   if MqttReceive(h, 'msg.', 5) then say msg.topic '=' msg.message
 *   call MqttDisconnect h
 *
 * Functions (see rgmqtt/README for details)
 *   MqttLoadFuncs()  MqttDropFuncs()  MqttVersion()
 *   MqttConnect(host [,port] [,clientid] [,options.])   -> handle, 0 on error
 *   MqttPublish(h, topic, message [,qos] [,retain])     -> 0, -1 on error
 *   MqttSubscribe(h, filter [,qos])                     -> granted QoS, 128, -1
 *   MqttUnsubscribe(h, filter)                          -> 0, -1
 *   MqttReceive(h, stem. [,seconds])                    -> 1 message, 0 none, -1
 *   MqttPending(h)                                      -> messages waiting
 *   MqttPing(h)                                         -> 0, -1
 *   MqttDisconnect(h)                                   -> 0, -1
 *   MqttError()                                         -> text of the last error
 *
 * QoS 0, 1 and 2 are supported in both directions. Messages that arrive
 * while the client waits for an acknowledgement are queued and returned by
 * MqttReceive in order. Keep-alive pings are sent while waiting.
 *
 * Copyright (C) 2026 Mike Beer. Distributed under the GNU Library General
 * Public License, like Regina.
 */
#define INCL_RXSHV
#define INCL_RXFUNC
#include "rexxsaa.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
# include <winsock2.h>
# include <ws2tcpip.h>
# include <windows.h>
# define CLOSESOCK closesocket
typedef SOCKET sock_t;
# define BADSOCK INVALID_SOCKET
#else
# include <unistd.h>
# include <errno.h>
# include <netdb.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/select.h>
# include <sys/time.h>
# include <netinet/in.h>
# include <netinet/tcp.h>
# include <signal.h>
# define CLOSESOCK close
typedef int sock_t;
# define BADSOCK ( -1 )
#endif

#define RGMQTT_VERSION "RgMqtt 1.0 (MQTT 3.1.1)"
#define MAXCONN 64
#define MAXPACKET ( 256L * 1024L * 1024L )
#define MAXQOS2 256

#define FN( n ) APIRET APIENTRY n( PCSZ name, ULONG argc, PRXSTRING argv, PCSZ queue, PRXSTRING ret )
#define INCORRECT_CALL 40

/* MQTT control packet types */
enum { CONNECT = 1, CONNACK, PUBLISH, PUBACK, PUBREC, PUBREL, PUBCOMP, SUBSCRIBE,
       SUBACK, UNSUBSCRIBE, UNSUBACK, PINGREQ, PINGRESP, DISCONNECT };

typedef struct message {
   struct message *next;
   char *topic;
   size_t tlen;
   char *data;
   size_t dlen;
   int qos, retain, dup;
} message;

typedef struct {
   int used;
   sock_t fd;
   long keepalive;              /* seconds, 0 = none */
   double last_send;            /* ms */
   long timeout;                /* ms to wait for an acknowledgement */
   unsigned short next_id;
   message *head, *tail;
   long pending;
   unsigned short qos2[MAXQOS2];        /* incoming QoS 2 ids not yet released */
   int nqos2;
} conn;

static conn conns[MAXCONN + 1];
static char lasterr[512] = "";

/* ------------------------------------------------------------------ */
/* helpers                                                            */
/* ------------------------------------------------------------------ */

static void seterr( const char *fmt, const char *arg )
{
   if ( arg )
      snprintf( lasterr, sizeof( lasterr ), fmt, arg );
   else
      snprintf( lasterr, sizeof( lasterr ), "%s", fmt );
}

static double now_ms( void )
{
#ifdef _WIN32
   return (double) GetTickCount64();
#else
   struct timespec t;
   clock_gettime( CLOCK_MONOTONIC, &t );
   return t.tv_sec * 1000.0 + t.tv_nsec / 1e6;
#endif
}

static int net_init( void )
{
#ifdef _WIN32
   static int done = 0;
   if ( !done )
   {
      WSADATA w;
      if ( WSAStartup( MAKEWORD( 2, 2 ), &w ) != 0 )
         return -1;
      done = 1;
   }
#else
   static int done = 0;
   if ( !done )
   {
      signal( SIGPIPE, SIG_IGN );       /* a closed socket gives EPIPE instead */
      done = 1;
   }
#endif
   return 0;
}

static void retbuf( PRXSTRING ret, const char *s, size_t n )
{
   if ( ret->strptr == NULL || n > ret->strlength )
      ret->strptr = (char *) RexxAllocateMemory( (ULONG) ( n + 1 ) );
   if ( n )
      memcpy( ret->strptr, s, n );
   ret->strlength = (ULONG) n;
}

static void retstr( PRXSTRING ret, const char *s )
{
   retbuf( ret, s, strlen( s ) );
}

static void retint( PRXSTRING ret, long v )
{
   char b[32];
   sprintf( b, "%ld", v );
   retstr( ret, b );
}

static int given( ULONG argc, PRXSTRING argv, ULONG i )
{
   return i < argc && argv[i].strptr != NULL;
}

/* a copy of argument i with a terminating 0 (empty if omitted) */
static char *argstr( ULONG argc, PRXSTRING argv, ULONG i, size_t *len )
{
   size_t n = given( argc, argv, i ) ? argv[i].strlength : 0;
   char *p = (char *) malloc( n + 1 );
   if ( p == NULL )
      return NULL;
   if ( n )
      memcpy( p, argv[i].strptr, n );
   p[n] = 0;
   if ( len )
      *len = n;
   return p;
}

/* a whole number from argument i; def if omitted, 0 if it isn't a number */
static int argnum( ULONG argc, PRXSTRING argv, ULONG i, long def, long *out )
{
   char b[40], *end;
   size_t n;

   if ( !given( argc, argv, i ) || argv[i].strlength == 0 )
   {
      *out = def;
      return 1;
   }
   n = argv[i].strlength;
   if ( n >= sizeof( b ) )
      return 0;
   memcpy( b, argv[i].strptr, n );
   b[n] = 0;
   *out = strtol( b, &end, 10 );
   while ( *end == ' ' )
      end++;
   return end != b && *end == 0;
}

/* a number of seconds (may have decimals) as ms */
static int argsecs( ULONG argc, PRXSTRING argv, ULONG i, long def, long *out )
{
   char b[40], *end;
   size_t n;
   double d;

   if ( !given( argc, argv, i ) || argv[i].strlength == 0 )
   {
      *out = def;
      return 1;
   }
   n = argv[i].strlength;
   if ( n >= sizeof( b ) )
      return 0;
   memcpy( b, argv[i].strptr, n );
   b[n] = 0;
   d = strtod( b, &end );
   while ( *end == ' ' )
      end++;
   if ( end == b || *end != 0 || d < 0 )
      return 0;
   *out = (long) ( d * 1000.0 + 0.5 );
   return 1;
}

static conn *argconn( ULONG argc, PRXSTRING argv )
{
   long h;
   if ( !argnum( argc, argv, 0, 0, &h ) || h < 1 || h > MAXCONN || !conns[h].used )
   {
      seterr( "not a connection handle", NULL );
      return NULL;
   }
   return &conns[h];
}

/* stem name with a trailing dot */
static char *stemname( ULONG argc, PRXSTRING argv, ULONG i )
{
   size_t n;
   char *s = argstr( argc, argv, i, &n ), *t;
   if ( s == NULL || n == 0 || s[n - 1] == '.' )
      return s;
   t = (char *) malloc( n + 2 );
   if ( t )
   {
      memcpy( t, s, n );
      t[n] = '.';
      t[n + 1] = 0;
   }
   free( s );
   return t;
}

static void setvar( const char *stem, const char *tail, const char *value, size_t len )
{
   SHVBLOCK b;
   char nm[300];

   snprintf( nm, sizeof( nm ), "%s%s", stem, tail );
   memset( &b, 0, sizeof( b ) );
   b.shvcode = RXSHV_SYSET;
   MAKERXSTRING( b.shvname, nm, strlen( nm ) );
   MAKERXSTRING( b.shvvalue, (char *) value, len );
   RexxVariablePool( &b );
}

static void setvarnum( const char *stem, const char *tail, long v )
{
   char b[32];
   sprintf( b, "%ld", v );
   setvar( stem, tail, b, strlen( b ) );
}

/* the value of stem.tail, malloc'd; NULL if the variable has no value */
static char *getvar( const char *stem, const char *tail, size_t *len )
{
   SHVBLOCK b;
   char nm[300], *v;

   snprintf( nm, sizeof( nm ), "%s%s", stem, tail );
   memset( &b, 0, sizeof( b ) );
   b.shvcode = RXSHV_SYFET;
   MAKERXSTRING( b.shvname, nm, strlen( nm ) );
   b.shvvalue.strptr = NULL;
   b.shvvalue.strlength = 0;
   if ( RexxVariablePool( &b ) & ( RXSHV_NEWV | RXSHV_BADN ) || ( b.shvret & RXSHV_NEWV ) )
   {
      if ( b.shvvalue.strptr )
         RexxFreeMemory( b.shvvalue.strptr );
      return NULL;
   }
   v = (char *) malloc( b.shvvalue.strlength + 1 );
   if ( v )
   {
      if ( b.shvvalue.strlength )
         memcpy( v, b.shvvalue.strptr, b.shvvalue.strlength );
      v[b.shvvalue.strlength] = 0;
   }
   if ( len )
      *len = b.shvvalue.strlength;
   if ( b.shvvalue.strptr )
      RexxFreeMemory( b.shvvalue.strptr );
   return v;
}

/* ------------------------------------------------------------------ */
/* packets                                                            */
/* ------------------------------------------------------------------ */

typedef struct {
   unsigned char *p;
   size_t n, cap;
   int bad;
} buf;

static void put( buf *b, const void *s, size_t n )
{
   if ( b->bad )
      return;
   if ( b->n + n > b->cap )
   {
      size_t nc = ( b->n + n ) * 2 + 64;
      unsigned char *np = (unsigned char *) realloc( b->p, nc );
      if ( np == NULL )
      {
         b->bad = 1;
         return;
      }
      b->p = np;
      b->cap = nc;
   }
   memcpy( b->p + b->n, s, n );
   b->n += n;
}

static void put1( buf *b, int c )
{
   unsigned char u = (unsigned char) c;
   put( b, &u, 1 );
}

static void put2( buf *b, unsigned v )
{
   put1( b, ( v >> 8 ) & 0xff );
   put1( b, v & 0xff );
}

static void putstr( buf *b, const char *s, size_t n )
{
   put2( b, (unsigned) n );
   put( b, s, n );
}

static void conn_close( conn *c )
{
   message *m, *mn;

   if ( c->fd != BADSOCK )
      CLOSESOCK( c->fd );
   for ( m = c->head; m; m = mn )
   {
      mn = m->next;
      free( m->topic );
      free( m->data );
      free( m );
   }
   memset( c, 0, sizeof( *c ) );
   c->fd = BADSOCK;
}

static int send_all( conn *c, const unsigned char *p, size_t n )
{
   while ( n > 0 )
   {
      long k = (long) send( c->fd, (const char *) p, (int) n, 0 );
      if ( k <= 0 )
      {
         seterr( "connection lost while sending", NULL );
         return -1;
      }
      p += k;
      n -= (size_t) k;
   }
   c->last_send = now_ms();
   return 0;
}

/* sends a packet with fixed header byte hdr and body b */
static int send_packet( conn *c, int hdr, const buf *b )
{
   unsigned char h[5];
   size_t len = b ? b->n : 0, k = 0;
   unsigned char *all;
   int rc;

   if ( b && b->bad )
   {
      seterr( "out of memory", NULL );
      return -1;
   }
   if ( (long) len > MAXPACKET )
   {
      seterr( "message too large for MQTT", NULL );
      return -1;
   }
   do
   {
      unsigned char d = (unsigned char) ( len % 128 );
      len /= 128;
      if ( len > 0 )
         d |= 0x80;
      h[k++] = d;
   } while ( len > 0 );
   len = b ? b->n : 0;
   all = (unsigned char *) malloc( 1 + k + len );
   if ( all == NULL )
   {
      seterr( "out of memory", NULL );
      return -1;
   }
   all[0] = (unsigned char) hdr;
   memcpy( all + 1, h, k );
   if ( len )
      memcpy( all + 1 + k, b->p, len );
   rc = send_all( c, all, 1 + k + len );
   free( all );
   return rc;
}

static int send_ack( conn *c, int hdr, unsigned id )
{
   buf b = { NULL, 0, 0, 0 };
   int rc;
   put2( &b, id );
   rc = send_packet( c, hdr, &b );
   free( b.p );
   return rc;
}

/* waits up to ms for data; 1 ready, 0 timeout, -1 error */
static int wait_readable( conn *c, long ms )
{
   fd_set r;
   struct timeval tv;
   int k;

   FD_ZERO( &r );
   FD_SET( c->fd, &r );
   tv.tv_sec = ms / 1000;
   tv.tv_usec = ( ms % 1000 ) * 1000;
   k = select( (int) c->fd + 1, &r, NULL, NULL, &tv );
#ifndef _WIN32
   if ( k < 0 && errno == EINTR )
      return 0;
#endif
   if ( k < 0 )
   {
      seterr( "select failed", NULL );
      return -1;
   }
   return k > 0;
}

static int recv_exact( conn *c, unsigned char *p, size_t n )
{
   while ( n > 0 )
   {
      long k;
      int w = wait_readable( c, c->timeout );
      if ( w <= 0 )
      {
         if ( w == 0 )
            seterr( "timeout while reading a packet", NULL );
         return -1;
      }
      k = (long) recv( c->fd, (char *) p, (int) n, 0 );
      if ( k <= 0 )
      {
         seterr( "connection closed by the broker", NULL );
         return -1;
      }
      p += k;
      n -= (size_t) k;
   }
   return 0;
}

/*
 * Reads one packet, waiting up to ms for it to start.
 * 1: *hdr, *body (malloc'd), *len set; 0: nothing arrived; -1: error.
 */
static int read_packet( conn *c, long ms, int *hdr, unsigned char **body, size_t *len )
{
   unsigned char h, d;
   size_t n = 0, mult = 1;
   int k, w;

   w = wait_readable( c, ms );
   if ( w <= 0 )
      return w;
   if ( recv_exact( c, &h, 1 ) < 0 )
      return -1;
   for ( k = 0; ; k++ )
   {
      if ( k == 4 || recv_exact( c, &d, 1 ) < 0 )
      {
         if ( k == 4 )
            seterr( "malformed packet from the broker", NULL );
         return -1;
      }
      n += ( d & 0x7f ) * mult;
      mult *= 128;
      if ( !( d & 0x80 ) )
         break;
   }
   *body = (unsigned char *) malloc( n + 1 );
   if ( *body == NULL )
   {
      seterr( "out of memory", NULL );
      return -1;
   }
   if ( n && recv_exact( c, *body, n ) < 0 )
   {
      free( *body );
      return -1;
   }
   *hdr = h;
   *len = n;
   return 1;
}

static unsigned get2( const unsigned char *p )
{
   return ( (unsigned) p[0] << 8 ) | p[1];
}

static unsigned new_id( conn *c )
{
   if ( ++c->next_id == 0 )
      c->next_id = 1;
   return c->next_id;
}

static int qos2_find( conn *c, unsigned id )
{
   int i;
   for ( i = 0; i < c->nqos2; i++ )
      if ( c->qos2[i] == id )
         return i;
   return -1;
}

/* an incoming PUBLISH: acknowledges it and queues the message */
static int got_publish( conn *c, int hdr, unsigned char *p, size_t n )
{
   int qos = ( hdr >> 1 ) & 3;
   size_t tl, off;
   unsigned id = 0;
   message *m;

   if ( n < 2 || ( tl = get2( p ) ) + 2 > n || qos == 3 )
   {
      seterr( "malformed PUBLISH from the broker", NULL );
      return -1;
   }
   off = 2 + tl;
   if ( qos > 0 )
   {
      if ( off + 2 > n )
      {
         seterr( "malformed PUBLISH from the broker", NULL );
         return -1;
      }
      id = get2( p + off );
      off += 2;
   }
   if ( qos == 1 && send_ack( c, PUBACK << 4, id ) < 0 )
      return -1;
   if ( qos == 2 )
   {
      if ( send_ack( c, PUBREC << 4, id ) < 0 )
         return -1;
      if ( qos2_find( c, id ) >= 0 )
         return 0;              /* a duplicate: delivered already */
      if ( c->nqos2 < MAXQOS2 )
         c->qos2[c->nqos2++] = (unsigned short) id;
   }
   m = (message *) calloc( 1, sizeof( message ) );
   if ( m == NULL )
      return 0;
   m->topic = (char *) malloc( tl + 1 );
   m->data = (char *) malloc( n - off + 1 );
   if ( m->topic == NULL || m->data == NULL )
   {
      free( m->topic );
      free( m->data );
      free( m );
      return 0;
   }
   memcpy( m->topic, p + 2, tl );
   m->tlen = tl;
   memcpy( m->data, p + off, n - off );
   m->dlen = n - off;
   m->qos = qos;
   m->retain = hdr & 1;
   m->dup = ( hdr >> 3 ) & 1;
   if ( c->tail )
      c->tail->next = m;
   else
      c->head = m;
   c->tail = m;
   c->pending++;
   return 0;
}

/* handles a packet nobody is waiting for */
static int got_other( conn *c, int hdr, unsigned char *p, size_t n )
{
   switch ( hdr >> 4 )
   {
      case PUBLISH:
         return got_publish( c, hdr, p, n );
      case PUBREL:
         if ( n >= 2 )
         {
            int i = qos2_find( c, get2( p ) );
            if ( i >= 0 )
               c->qos2[i] = c->qos2[--c->nqos2];
            return send_ack( c, ( PUBCOMP << 4 ), get2( p ) );
         }
         return 0;
      default:                  /* PINGRESP, late acknowledgements */
         return 0;
   }
}

/* sends a ping when the keep-alive interval is nearly over */
static int keep_alive( conn *c )
{
   if ( c->keepalive > 0 && now_ms() - c->last_send >= c->keepalive * 750.0 )
      return send_packet( c, PINGREQ << 4, NULL );
   return 0;
}

/* time to wait before the next keep-alive check, at most ms */
static long slice( conn *c, long ms )
{
   if ( c->keepalive > 0 )
   {
      long due = (long) ( c->last_send + c->keepalive * 750.0 - now_ms() );
      if ( due < 0 )
         due = 0;
      if ( due < ms )
         return due;
   }
   return ms;
}

/*
 * Waits for a packet of type want (with packet id id if id >= 0) and returns
 * its body; everything else that arrives meanwhile is handled. NULL on error
 * or timeout.
 */
static unsigned char *wait_for( conn *c, int want, long id, size_t *len )
{
   double end = now_ms() + c->timeout;

   for ( ;; )
   {
      int hdr, k;
      unsigned char *body;
      size_t n;
      long left = (long) ( end - now_ms() );

      if ( left <= 0 )
      {
         seterr( "no answer from the broker", NULL );
         return NULL;
      }
      if ( keep_alive( c ) < 0 )
         return NULL;
      k = read_packet( c, slice( c, left ), &hdr, &body, &n );
      if ( k < 0 )
         return NULL;
      if ( k == 0 )
         continue;
      if ( ( hdr >> 4 ) == want && ( id < 0 || ( n >= 2 && (long) get2( body ) == id ) ) )
      {
         *len = n;
         return body;
      }
      k = got_other( c, hdr, body, n );
      free( body );
      if ( k < 0 )
         return NULL;
   }
}

/* a failed connection is closed: later calls with its handle fail cleanly */
static long lost( conn *c )
{
   conn_close( c );
   return -1;
}

/* ------------------------------------------------------------------ */
/* the functions                                                      */
/* ------------------------------------------------------------------ */

FN( MqttVersion )
{
   retstr( ret, RGMQTT_VERSION );
   return 0;
}

FN( MqttError )
{
   retstr( ret, lasterr );
   return 0;
}

/* MqttConnect(host [,port] [,clientid] [,options.]) */
FN( MqttConnect )
{
   char *host = NULL, *client = NULL, *stem = NULL, *port = NULL;
   char *user = NULL, *pass = NULL, *wtopic = NULL, *wmsg = NULL, *v;
   size_t clen = 0, ulen = 0, plen = 0, wtlen = 0, wmlen = 0;
   long portnum, keepalive = 60, clean = 1, wqos = 0, wretain = 0, timeout = 10000;
   struct addrinfo hints, *res = NULL, *ai;
   conn *c = NULL;
   buf b = { NULL, 0, 0, 0 };
   unsigned char *ack;
   size_t alen;
   int h, flags;
   char gen[64];

   if ( argc < 1 || argc > 4 || !given( argc, argv, 0 ) )
      return INCORRECT_CALL;
   if ( !argnum( argc, argv, 1, 1883, &portnum ) || portnum < 1 || portnum > 65535 )
      return INCORRECT_CALL;
   retint( ret, 0 );
   host = argstr( argc, argv, 0, NULL );
   client = argstr( argc, argv, 2, &clen );
   if ( given( argc, argv, 3 ) )
   {
      stem = stemname( argc, argv, 3 );
      if ( stem && ( v = getvar( stem, "KEEPALIVE", NULL ) ) )
      {
         keepalive = atol( v );
         free( v );
      }
      if ( stem && ( v = getvar( stem, "CLEAN", NULL ) ) )
      {
         clean = atol( v ) != 0;
         free( v );
      }
      if ( stem && ( v = getvar( stem, "TIMEOUT", NULL ) ) )
      {
         timeout = (long) ( atof( v ) * 1000 );
         free( v );
      }
      if ( stem )
      {
         user = getvar( stem, "USER", &ulen );
         pass = getvar( stem, "PASSWORD", &plen );
         wtopic = getvar( stem, "WILLTOPIC", &wtlen );
         wmsg = getvar( stem, "WILLMESSAGE", &wmlen );
      }
      if ( stem && ( v = getvar( stem, "WILLQOS", NULL ) ) )
      {
         wqos = atol( v );
         free( v );
      }
      if ( stem && ( v = getvar( stem, "WILLRETAIN", NULL ) ) )
      {
         wretain = atol( v ) != 0;
         free( v );
      }
   }
   if ( keepalive < 0 || keepalive > 65535 || wqos < 0 || wqos > 2 || timeout <= 0 )
   {
      seterr( "bad connection option", NULL );
      goto done;
   }
   if ( clen == 0 )
   {
      /* the broker could assign one, but only for clean sessions */
      sprintf( gen, "rgmqtt-%lx-%lx", (unsigned long) time( NULL ),
               (unsigned long) ( now_ms() * 1000.0 ) & 0xffffffUL );
      free( client );
      client = (char *) malloc( strlen( gen ) + 1 );
      if ( client )
         strcpy( client, gen );
      clen = strlen( gen );
   }
   for ( h = 1; h <= MAXCONN; h++ )
      if ( !conns[h].used )
         break;
   if ( h > MAXCONN )
   {
      seterr( "too many connections", NULL );
      goto done;
   }
   if ( net_init() < 0 )
   {
      seterr( "the network can't be initialised", NULL );
      goto done;
   }
   port = (char *) malloc( 8 );
   sprintf( port, "%ld", portnum );
   memset( &hints, 0, sizeof( hints ) );
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   if ( getaddrinfo( host, port, &hints, &res ) != 0 || res == NULL )
   {
      seterr( "unknown host %s", host );
      goto done;
   }
   c = &conns[h];
   memset( c, 0, sizeof( *c ) );
   c->fd = BADSOCK;
   for ( ai = res; ai; ai = ai->ai_next )
   {
      c->fd = socket( ai->ai_family, ai->ai_socktype, ai->ai_protocol );
      if ( c->fd == BADSOCK )
         continue;
      if ( connect( c->fd, ai->ai_addr, (int) ai->ai_addrlen ) == 0 )
         break;
      CLOSESOCK( c->fd );
      c->fd = BADSOCK;
   }
   freeaddrinfo( res );
   if ( c->fd == BADSOCK )
   {
      seterr( "can't connect to %s", host );
      c = NULL;
      goto done;
   }
   {
      int one = 1;
      setsockopt( c->fd, IPPROTO_TCP, TCP_NODELAY, (const char *) &one, sizeof( one ) );
   }
   c->used = 1;
   c->keepalive = keepalive;
   c->timeout = timeout;

   /* CONNECT */
   putstr( &b, "MQTT", 4 );
   put1( &b, 4 );                       /* protocol level 3.1.1 */
   flags = clean ? 0x02 : 0;
   if ( wtopic )
      flags |= 0x04 | (int) ( wqos << 3 ) | ( wretain ? 0x20 : 0 );
   if ( user )
      flags |= 0x80;
   if ( pass )
      flags |= 0x40;
   put1( &b, flags );
   put2( &b, (unsigned) keepalive );
   putstr( &b, client, clen );
   if ( wtopic )
   {
      putstr( &b, wtopic, wtlen );
      putstr( &b, wmsg ? wmsg : "", wmsg ? wmlen : 0 );
   }
   if ( user )
      putstr( &b, user, ulen );
   if ( pass )
      putstr( &b, pass, plen );
   if ( send_packet( c, CONNECT << 4, &b ) < 0 )
   {
      lost( c );
      goto done;
   }
   ack = wait_for( c, CONNACK, -1, &alen );
   if ( ack == NULL )
   {
      lost( c );
      goto done;
   }
   if ( alen < 2 || ack[1] != 0 )
   {
      static const char *why[] = { "", "unacceptable protocol version", "client id rejected",
                                   "server unavailable", "bad user name or password",
                                   "not authorized" };
      int rc = alen < 2 ? 99 : ack[1];
      seterr( "connection refused: %s", rc >= 1 && rc <= 5 ? why[rc] : "unknown reason" );
      free( ack );
      lost( c );
      goto done;
   }
   free( ack );
   retint( ret, h );
   lasterr[0] = 0;

done:
   free( b.p );
   free( host );
   free( client );
   free( stem );
   free( port );
   free( user );
   free( pass );
   free( wtopic );
   free( wmsg );
   return 0;
}

/* MqttPublish(h, topic, message [,qos] [,retain]) */
FN( MqttPublish )
{
   conn *c;
   long qos, retain;
   unsigned id = 0;
   buf b = { NULL, 0, 0, 0 };
   unsigned char *ack;
   size_t n;
   long rc = -1;

   if ( argc < 3 || argc > 5 || !given( argc, argv, 1 ) )
      return INCORRECT_CALL;
   if ( !argnum( argc, argv, 3, 0, &qos ) || qos < 0 || qos > 2 )
      return INCORRECT_CALL;
   if ( !argnum( argc, argv, 4, 0, &retain ) || ( retain != 0 && retain != 1 ) )
      return INCORRECT_CALL;
   if ( ( c = argconn( argc, argv ) ) == NULL )
   {
      retint( ret, -1 );
      return 0;
   }
   if ( argv[1].strlength == 0 || memchr( argv[1].strptr, '+', argv[1].strlength )
        || memchr( argv[1].strptr, '#', argv[1].strlength ) )
   {
      seterr( "a topic name must not be empty or contain wildcards", NULL );
      retint( ret, -1 );
      return 0;
   }
   putstr( &b, argv[1].strptr, argv[1].strlength );
   if ( qos > 0 )
      put2( &b, id = new_id( c ) );
   if ( given( argc, argv, 2 ) )
      put( &b, argv[2].strptr, argv[2].strlength );
   if ( send_packet( c, ( PUBLISH << 4 ) | (int) ( qos << 1 ) | (int) retain, &b ) < 0 )
      rc = lost( c );
   else if ( qos == 0 )
      rc = 0;
   else if ( qos == 1 )
   {
      if ( ( ack = wait_for( c, PUBACK, id, &n ) ) != NULL )
      {
         free( ack );
         rc = 0;
      }
      else
         rc = lost( c );
   }
   else
   {
      if ( ( ack = wait_for( c, PUBREC, id, &n ) ) != NULL )
      {
         free( ack );
         if ( send_ack( c, ( PUBREL << 4 ) | 2, id ) == 0
              && ( ack = wait_for( c, PUBCOMP, id, &n ) ) != NULL )
         {
            free( ack );
            rc = 0;
         }
         else
            rc = lost( c );
      }
      else
         rc = lost( c );
   }
   free( b.p );
   retint( ret, rc );
   return 0;
}

/* MqttSubscribe(h, filter [,qos]) */
FN( MqttSubscribe )
{
   conn *c;
   long qos, rc = -1;
   unsigned id;
   buf b = { NULL, 0, 0, 0 };
   unsigned char *ack;
   size_t n;

   if ( argc < 2 || argc > 3 || !given( argc, argv, 1 ) || argv[1].strlength == 0 )
      return INCORRECT_CALL;
   if ( !argnum( argc, argv, 2, 0, &qos ) || qos < 0 || qos > 2 )
      return INCORRECT_CALL;
   if ( ( c = argconn( argc, argv ) ) == NULL )
   {
      retint( ret, -1 );
      return 0;
   }
   put2( &b, id = new_id( c ) );
   putstr( &b, argv[1].strptr, argv[1].strlength );
   put1( &b, (int) qos );
   if ( send_packet( c, ( SUBSCRIBE << 4 ) | 2, &b ) < 0 )
      rc = lost( c );
   else if ( ( ack = wait_for( c, SUBACK, id, &n ) ) == NULL )
      rc = lost( c );
   else
   {
      rc = n >= 3 ? ack[2] : 128;
      if ( rc == 128 )
         seterr( "the broker refused the subscription", NULL );
      free( ack );
   }
   free( b.p );
   retint( ret, rc );
   return 0;
}

/* MqttUnsubscribe(h, filter) */
FN( MqttUnsubscribe )
{
   conn *c;
   long rc = -1;
   unsigned id;
   buf b = { NULL, 0, 0, 0 };
   unsigned char *ack;
   size_t n;

   if ( argc != 2 || !given( argc, argv, 1 ) || argv[1].strlength == 0 )
      return INCORRECT_CALL;
   if ( ( c = argconn( argc, argv ) ) == NULL )
   {
      retint( ret, -1 );
      return 0;
   }
   put2( &b, id = new_id( c ) );
   putstr( &b, argv[1].strptr, argv[1].strlength );
   if ( send_packet( c, ( UNSUBSCRIBE << 4 ) | 2, &b ) < 0 )
      rc = lost( c );
   else if ( ( ack = wait_for( c, UNSUBACK, id, &n ) ) == NULL )
      rc = lost( c );
   else
   {
      free( ack );
      rc = 0;
   }
   free( b.p );
   retint( ret, rc );
   return 0;
}

/* MqttReceive(h, stem. [,seconds]): stem.TOPIC, MESSAGE, QOS, RETAIN, DUP */
FN( MqttReceive )
{
   conn *c;
   long ms;
   double end;
   char *stem;
   message *m;

   if ( argc < 2 || argc > 3 || !given( argc, argv, 1 ) || argv[1].strlength == 0 )
      return INCORRECT_CALL;
   if ( !argsecs( argc, argv, 2, 0, &ms ) )
      return INCORRECT_CALL;
   if ( ( c = argconn( argc, argv ) ) == NULL )
   {
      retint( ret, -1 );
      return 0;
   }
   end = now_ms() + ms;
   while ( c->head == NULL )
   {
      int hdr, k;
      unsigned char *body;
      size_t n;
      long left = (long) ( end - now_ms() );

      if ( keep_alive( c ) < 0 )
      {
         retint( ret, lost( c ) );
         return 0;
      }
      k = read_packet( c, slice( c, left > 0 ? left : 0 ), &hdr, &body, &n );
      if ( k < 0 )
      {
         retint( ret, lost( c ) );
         return 0;
      }
      if ( k > 0 )
      {
         k = got_other( c, hdr, body, n );
         free( body );
         if ( k < 0 )
         {
            retint( ret, lost( c ) );
            return 0;
         }
      }
      else if ( left <= 0 )
         break;
   }
   if ( ( m = c->head ) == NULL )
   {
      retint( ret, 0 );
      return 0;
   }
   c->head = m->next;
   if ( c->head == NULL )
      c->tail = NULL;
   c->pending--;
   stem = stemname( argc, argv, 1 );
   if ( stem )
   {
      setvar( stem, "TOPIC", m->topic, m->tlen );
      setvar( stem, "MESSAGE", m->data, m->dlen );
      setvarnum( stem, "QOS", m->qos );
      setvarnum( stem, "RETAIN", m->retain );
      setvarnum( stem, "DUP", m->dup );
      free( stem );
   }
   free( m->topic );
   free( m->data );
   free( m );
   retint( ret, 1 );
   return 0;
}

/* MqttPending(h): messages received but not yet returned by MqttReceive */
FN( MqttPending )
{
   conn *c;

   if ( argc != 1 )
      return INCORRECT_CALL;
   if ( ( c = argconn( argc, argv ) ) == NULL )
   {
      retint( ret, -1 );
      return 0;
   }
   /* collect what has arrived already */
   for ( ;; )
   {
      int hdr, k;
      unsigned char *body;
      size_t n;
      k = read_packet( c, 0, &hdr, &body, &n );
      if ( k < 0 )
      {
         retint( ret, c->pending );
         return 0;
      }
      if ( k == 0 )
         break;
      k = got_other( c, hdr, body, n );
      free( body );
      if ( k < 0 )
         break;
   }
   retint( ret, c->pending );
   return 0;
}

/* MqttPing(h): a round trip to the broker */
FN( MqttPing )
{
   conn *c;
   unsigned char *ack;
   size_t n;

   if ( argc != 1 )
      return INCORRECT_CALL;
   if ( ( c = argconn( argc, argv ) ) == NULL )
   {
      retint( ret, -1 );
      return 0;
   }
   if ( send_packet( c, PINGREQ << 4, NULL ) < 0 || ( ack = wait_for( c, PINGRESP, -1, &n ) ) == NULL )
   {
      retint( ret, lost( c ) );
      return 0;
   }
   free( ack );
   retint( ret, 0 );
   return 0;
}

/* MqttDisconnect(h) */
FN( MqttDisconnect )
{
   conn *c;
   int rc;

   if ( argc != 1 )
      return INCORRECT_CALL;
   if ( ( c = argconn( argc, argv ) ) == NULL )
   {
      retint( ret, -1 );
      return 0;
   }
   rc = send_packet( c, DISCONNECT << 4, NULL );
   conn_close( c );
   retint( ret, rc );
   return 0;
}

static const char *functions[] = {
   "MqttVersion", "MqttError", "MqttConnect", "MqttPublish", "MqttSubscribe",
   "MqttUnsubscribe", "MqttReceive", "MqttPending", "MqttPing", "MqttDisconnect",
   "MqttDropFuncs", NULL
};

FN( MqttDropFuncs )
{
   int i;
   for ( i = 0; functions[i]; i++ )
      RexxDeregisterFunction( functions[i] );
   retstr( ret, "" );
   return 0;
}

FN( MqttLoadFuncs )
{
   int i;
   for ( i = 0; functions[i]; i++ )
      RexxRegisterFunctionDll( functions[i], "rgmqtt", functions[i] );
   retstr( ret, "" );
   return 0;
}
