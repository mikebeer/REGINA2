/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         __reginaparse
#define yylex           __reginalex
#define yyerror         __reginaerror
#define yydebug         __reginadebug
#define yynerrs         __reginanerrs
#define yylval          __reginalval
#define yychar          __reginachar

/* First part of user prologue.  */
#line 1 "yaccsrc.y"


#ifndef lint
static char *RCSid = "$Id: yaccsrc.y,v 1.47 2025/03/17 22:05:55 mark Exp $";
#endif

/*
 *  The Regina Rexx Interpreter
 *  Copyright (C) 1992-1994  Anders Christensen <anders@pvv.unit.no>
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
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "rexx.h"
#include <time.h>

#if defined(HAVE_MALLOC_H)
# include <malloc.h>
#endif

#if defined(HAVE_ALLOCA_H)
# include <alloca.h>
#endif

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#if defined(_MSC_VER) || defined(MAC)
# define __STDC__ 1 /* Hack to allow const since it is not defined */
#endif

#define YYSTYPE nodeptr

/* locals, they are protected by regina_parser (see lexsrc.l) */
static int tmplno,           /* lineno of current instruction             */
           tmpchr,           /* character position of current instruction */
           level,            /* nested do/if/select depth                 */
           start_parendepth; /* see below at parendepth                   */

/*
 * parendepth regulates the action which happens detecting a comma or an
 * empty expression. A negative values indicates an error; both a comma
 * and an empty expression raise an error.
 * We regulate the enumeration of arguments with this semantical flag.
 * Look at "call subroutine args" and "function args". Function itself
 * contains a parentheses pair, so starting with a depth of just allows
 * the enumeration. subroutine starts with either 0 or 1. The latter one
 * is allowed for the support request of "call subroutine(a,b,c)" which
 * isn't allowed by ANSI but can be enabled for backward compatibility.
 */
static int parendepth;

static nodeptr current, with = NULL;

static char *rxnullptr = NULL; /* for C++ compilation */

typedef enum { IS_UNKNOWN,
               IS_A_NUMBER,
               IS_NO_NUMBER,
               IS_SIM_SYMBOL,
               IS_COMP_SYMBOL } node_type;

typedef enum { REDUCE_CALL,
               REDUCE_EXPR,
               REDUCE_RIGHT,
               REDUCE_SUBEXPR } reduce_mode;

static node_type gettypeof( nodeptr thisptr ) ;
static void checkconst( nodeptr thisptr ) ;
static nodeptr reduce_expr_list( nodeptr thisptr, reduce_mode mode );
static void transform( nodeptr thisptr ) ;
static nodeptr create_head( const char *name ) ;
static nodeptr makenode( int type, int numb, ... ) ;
static void checkdosyntax( cnodeptr thisptr ) ;
void newlabel( const tsd_t *TSD, internal_parser_type *ipt, nodeptr thisptr ) ;
static nodeptr optgluelast( nodeptr p1, nodeptr p2 );
static void move_labels( nodeptr front, nodeptr end, int level );
static nodeptr do_over_var( nodeptr rep );
static void finish_do( nodeptr dostat, nodeptr opts );
static nodeptr split_requires( nodeptr *dirs );
extern int __regina_do_is_loop; /* lexsrc.l: the DO token was LOOP */
extern int __regina_oo_keywords( void ); /* lexsrc.l: OO keywords are on */
extern int __regina_oo_off( void );      /* lexsrc.l: --no-oo, classic only */
extern streng *__regina_block_take( int *line );  /* lexsrc.l: source of a { block } */
extern const char *__regina_oo_operator; /* lexsrc.l, OO extension */

#define IS_EXPRLIST(x) ( ( (x) != NULL )                   \
                      && ( ( (x)->type == X_CEXPRLIST )    \
                        || ( (x)->type == X_EXPRLIST ) ) )

/* OO: message terms and array references carry argument lists in p[1] */
#define IS_FUNCTION(x) ( ( (x) != NULL )                  \
                      && ( ( (x)->type == X_EX_FUNC )     \
                        || ( (x)->type == X_IN_FUNC )     \
                        || ( (x)->type == X_MSGSEND )     \
                        || ( (x)->type == X_CASCADE )     \
                        || ( (x)->type == X_INDEX ) ) )

#define AUTO_REDUCE(x,y) { if ( parendepth == 1 )                             \
                           {                                                  \
                             x = reduce_expr_list( x, REDUCE_EXPR );          \
                             /* detect "call s (a,b)<op>" and      */         \
                             /* "call s ()<op>"                    */         \
                             if ( IS_EXPRLIST( x ) )                          \
                             {                                                \
                                if ( (y) != NULL )                            \
                                   exiterror( ERR_INVALID_EXPRESSION, 1, y ); \
                                else if ( (x)->p[0] == NULL )                 \
                                   exiterror( ERR_UNEXPECTED_PARAN, 0 );      \
                                else                                          \
                                   exiterror( ERR_UNEXPECTED_PARAN, 1 );      \
                             }                                                \
                           }                                                  \
                         }
#line 165 "yaccsrc.y"

#ifdef NDEBUG
# define YYDEBUG 0
#else
# define YYDEBUG 1
#endif

#line 215 "yaccsrc.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "yaccsrc.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_ADDRESS = 3,                    /* ADDRESS  */
  YYSYMBOL_ARG = 4,                        /* ARG  */
  YYSYMBOL_CALL = 5,                       /* CALL  */
  YYSYMBOL_DO = 6,                         /* DO  */
  YYSYMBOL_TO = 7,                         /* TO  */
  YYSYMBOL_BY = 8,                         /* BY  */
  YYSYMBOL_FOR = 9,                        /* FOR  */
  YYSYMBOL_WHILE = 10,                     /* WHILE  */
  YYSYMBOL_UNTIL = 11,                     /* UNTIL  */
  YYSYMBOL_EXIT = 12,                      /* EXIT  */
  YYSYMBOL_IF = 13,                        /* IF  */
  YYSYMBOL_THEN = 14,                      /* THEN  */
  YYSYMBOL_ELSE = 15,                      /* ELSE  */
  YYSYMBOL_ITERATE = 16,                   /* ITERATE  */
  YYSYMBOL_INTERPRET = 17,                 /* INTERPRET  */
  YYSYMBOL_LEAVE = 18,                     /* LEAVE  */
  YYSYMBOL_NOP = 19,                       /* NOP  */
  YYSYMBOL_NUMERIC = 20,                   /* NUMERIC  */
  YYSYMBOL_PARSE = 21,                     /* PARSE  */
  YYSYMBOL_EXTERNAL = 22,                  /* EXTERNAL  */
  YYSYMBOL_SOURCE = 23,                    /* SOURCE  */
  YYSYMBOL_VAR = 24,                       /* VAR  */
  YYSYMBOL_VALUE = 25,                     /* VALUE  */
  YYSYMBOL_WITH = 26,                      /* WITH  */
  YYSYMBOL_PROCEDURE = 27,                 /* PROCEDURE  */
  YYSYMBOL_EXPOSE = 28,                    /* EXPOSE  */
  YYSYMBOL_PULL = 29,                      /* PULL  */
  YYSYMBOL_PUSH = 30,                      /* PUSH  */
  YYSYMBOL_QUEUE = 31,                     /* QUEUE  */
  YYSYMBOL_SAY = 32,                       /* SAY  */
  YYSYMBOL_RETURN = 33,                    /* RETURN  */
  YYSYMBOL_SELECT = 34,                    /* SELECT  */
  YYSYMBOL_WHEN = 35,                      /* WHEN  */
  YYSYMBOL_DROP = 36,                      /* DROP  */
  YYSYMBOL_OTHERWISE = 37,                 /* OTHERWISE  */
  YYSYMBOL_SIGNAL = 38,                    /* SIGNAL  */
  YYSYMBOL_ON = 39,                        /* ON  */
  YYSYMBOL_OFF = 40,                       /* OFF  */
  YYSYMBOL_SYNTAX = 41,                    /* SYNTAX  */
  YYSYMBOL_HALT = 42,                      /* HALT  */
  YYSYMBOL_NOVALUE = 43,                   /* NOVALUE  */
  YYSYMBOL_TRACE = 44,                     /* TRACE  */
  YYSYMBOL_END = 45,                       /* END  */
  YYSYMBOL_UPPER = 46,                     /* UPPER  */
  YYSYMBOL_ASSIGNMENTVARIABLE = 47,        /* ASSIGNMENTVARIABLE  */
  YYSYMBOL_STATSEP = 48,                   /* STATSEP  */
  YYSYMBOL_FOREVER = 49,                   /* FOREVER  */
  YYSYMBOL_DIGITS = 50,                    /* DIGITS  */
  YYSYMBOL_FORM = 51,                      /* FORM  */
  YYSYMBOL_FUZZ = 52,                      /* FUZZ  */
  YYSYMBOL_SCIENTIFIC = 53,                /* SCIENTIFIC  */
  YYSYMBOL_ENGINEERING = 54,               /* ENGINEERING  */
  YYSYMBOL_NOT = 55,                       /* NOT  */
  YYSYMBOL_CONCATENATE = 56,               /* CONCATENATE  */
  YYSYMBOL_MODULUS = 57,                   /* MODULUS  */
  YYSYMBOL_GTE = 58,                       /* GTE  */
  YYSYMBOL_GT = 59,                        /* GT  */
  YYSYMBOL_LTE = 60,                       /* LTE  */
  YYSYMBOL_LT = 61,                        /* LT  */
  YYSYMBOL_DIFFERENT = 62,                 /* DIFFERENT  */
  YYSYMBOL_EQUALEQUAL = 63,                /* EQUALEQUAL  */
  YYSYMBOL_NOTEQUALEQUAL = 64,             /* NOTEQUALEQUAL  */
  YYSYMBOL_OFFSET = 65,                    /* OFFSET  */
  YYSYMBOL_SPACE = 66,                     /* SPACE  */
  YYSYMBOL_EXP = 67,                       /* EXP  */
  YYSYMBOL_XOR = 68,                       /* XOR  */
  YYSYMBOL_PLACEHOLDER = 69,               /* PLACEHOLDER  */
  YYSYMBOL_NOTREADY = 70,                  /* NOTREADY  */
  YYSYMBOL_CONSYMBOL = 71,                 /* CONSYMBOL  */
  YYSYMBOL_SIMSYMBOL = 72,                 /* SIMSYMBOL  */
  YYSYMBOL_EXFUNCNAME = 73,                /* EXFUNCNAME  */
  YYSYMBOL_INFUNCNAME = 74,                /* INFUNCNAME  */
  YYSYMBOL_LABEL = 75,                     /* LABEL  */
  YYSYMBOL_DOVARIABLE = 76,                /* DOVARIABLE  */
  YYSYMBOL_HEXSTRING = 77,                 /* HEXSTRING  */
  YYSYMBOL_STRING = 78,                    /* STRING  */
  YYSYMBOL_VERSION = 79,                   /* VERSION  */
  YYSYMBOL_LINEIN = 80,                    /* LINEIN  */
  YYSYMBOL_WHATEVER = 81,                  /* WHATEVER  */
  YYSYMBOL_NAME = 82,                      /* NAME  */
  YYSYMBOL_FAILURE = 83,                   /* FAILURE  */
  YYSYMBOL_BINSTRING = 84,                 /* BINSTRING  */
  YYSYMBOL_OPTIONS = 85,                   /* OPTIONS  */
  YYSYMBOL_ENVIRONMENT = 86,               /* ENVIRONMENT  */
  YYSYMBOL_LOSTDIGITS = 87,                /* LOSTDIGITS  */
  YYSYMBOL_DOLABEL = 88,                   /* DOLABEL  */
  YYSYMBOL_DOCOUNTER = 89,                 /* DOCOUNTER  */
  YYSYMBOL_DOWITH = 90,                    /* DOWITH  */
  YYSYMBOL_DOINDEX = 91,                   /* DOINDEX  */
  YYSYMBOL_DOITEM = 92,                    /* DOITEM  */
  YYSYMBOL_DOOVER = 93,                    /* DOOVER  */
  YYSYMBOL_GTGT = 94,                      /* GTGT  */
  YYSYMBOL_LTLT = 95,                      /* LTLT  */
  YYSYMBOL_NOTGTGT = 96,                   /* NOTGTGT  */
  YYSYMBOL_NOTLTLT = 97,                   /* NOTLTLT  */
  YYSYMBOL_GTGTE = 98,                     /* GTGTE  */
  YYSYMBOL_LTLTE = 99,                     /* LTLTE  */
  YYSYMBOL_INPUT = 100,                    /* INPUT  */
  YYSYMBOL_OUTPUT = 101,                   /* OUTPUT  */
  YYSYMBOL_ERROR = 102,                    /* ERROR  */
  YYSYMBOL_NORMAL = 103,                   /* NORMAL  */
  YYSYMBOL_APPEND = 104,                   /* APPEND  */
  YYSYMBOL_REPLACE = 105,                  /* REPLACE  */
  YYSYMBOL_STREAM = 106,                   /* STREAM  */
  YYSYMBOL_STEM = 107,                     /* STEM  */
  YYSYMBOL_LIFO = 108,                     /* LIFO  */
  YYSYMBOL_FIFO = 109,                     /* FIFO  */
  YYSYMBOL_NOEOL = 110,                    /* NOEOL  */
  YYSYMBOL_LOWER = 111,                    /* LOWER  */
  YYSYMBOL_CASELESS = 112,                 /* CASELESS  */
  YYSYMBOL_MSGSEND = 113,                  /* MSGSEND  */
  YYSYMBOL_CASCADE = 114,                  /* CASCADE  */
  YYSYMBOL_DCOLON = 115,                   /* DCOLON  */
  YYSYMBOL_USEARG = 116,                   /* USEARG  */
  YYSYMBOL_MSGSUPER = 117,                 /* MSGSUPER  */
  YYSYMBOL_BLOCK = 118,                    /* BLOCK  */
  YYSYMBOL_PLUSASSIGNMENTVARIABLE = 119,   /* PLUSASSIGNMENTVARIABLE  */
  YYSYMBOL_MINUSASSIGNMENTVARIABLE = 120,  /* MINUSASSIGNMENTVARIABLE  */
  YYSYMBOL_MULTASSIGNMENTVARIABLE = 121,   /* MULTASSIGNMENTVARIABLE  */
  YYSYMBOL_DIVASSIGNMENTVARIABLE = 122,    /* DIVASSIGNMENTVARIABLE  */
  YYSYMBOL_MODULUSASSIGNMENTVARIABLE = 123, /* MODULUSASSIGNMENTVARIABLE  */
  YYSYMBOL_INTDIVASSIGNMENTVARIABLE = 124, /* INTDIVASSIGNMENTVARIABLE  */
  YYSYMBOL_ORASSIGNMENTVARIABLE = 125,     /* ORASSIGNMENTVARIABLE  */
  YYSYMBOL_XORASSIGNMENTVARIABLE = 126,    /* XORASSIGNMENTVARIABLE  */
  YYSYMBOL_ANDASSIGNMENTVARIABLE = 127,    /* ANDASSIGNMENTVARIABLE  */
  YYSYMBOL_CONCATASSIGNMENTVARIABLE = 128, /* CONCATASSIGNMENTVARIABLE  */
  YYSYMBOL_129_ = 129,                     /* '|'  */
  YYSYMBOL_130_ = 130,                     /* '&'  */
  YYSYMBOL_131_ = 131,                     /* '='  */
  YYSYMBOL_CCAT = 132,                     /* CCAT  */
  YYSYMBOL_133_ = 133,                     /* '+'  */
  YYSYMBOL_134_ = 134,                     /* '-'  */
  YYSYMBOL_135_ = 135,                     /* '*'  */
  YYSYMBOL_136_ = 136,                     /* '/'  */
  YYSYMBOL_137_ = 137,                     /* '%'  */
  YYSYMBOL_UMINUS = 138,                   /* UMINUS  */
  YYSYMBOL_UPLUS = 139,                    /* UPLUS  */
  YYSYMBOL_140_ = 140,                     /* '['  */
  YYSYMBOL_141_ = 141,                     /* ','  */
  YYSYMBOL_142_ = 142,                     /* '('  */
  YYSYMBOL_143_ = 143,                     /* ')'  */
  YYSYMBOL_144_ = 144,                     /* ']'  */
  YYSYMBOL_YYACCEPT = 145,                 /* $accept  */
  YYSYMBOL_start = 146,                    /* start  */
  YYSYMBOL_147_1 = 147,                    /* $@1  */
  YYSYMBOL_prog = 148,                     /* prog  */
  YYSYMBOL_dirsect = 149,                  /* dirsect  */
  YYSYMBOL_dirblock = 150,                 /* dirblock  */
  YYSYMBOL_directive = 151,                /* directive  */
  YYSYMBOL_152_2 = 152,                    /* @2  */
  YYSYMBOL_dirwords = 153,                 /* dirwords  */
  YYSYMBOL_dirword = 154,                  /* dirword  */
  YYSYMBOL_stats = 155,                    /* stats  */
  YYSYMBOL_xstats = 156,                   /* xstats  */
  YYSYMBOL_ystatement = 157,               /* ystatement  */
  YYSYMBOL_lonely_end = 158,               /* lonely_end  */
  YYSYMBOL_nxstats = 159,                  /* nxstats  */
  YYSYMBOL_ncl = 160,                      /* ncl  */
  YYSYMBOL_nlncl = 161,                    /* nlncl  */
  YYSYMBOL_optLabels = 162,                /* optLabels  */
  YYSYMBOL_statement = 163,                /* statement  */
  YYSYMBOL_gruff = 164,                    /* gruff  */
  YYSYMBOL_mttstatement = 165,             /* mttstatement  */
  YYSYMBOL_mtstatement = 166,              /* mtstatement  */
  YYSYMBOL_nclstatement = 167,             /* nclstatement  */
  YYSYMBOL_oo_expose_stat = 168,           /* oo_expose_stat  */
  YYSYMBOL_oo_usearg_stat = 169,           /* oo_usearg_stat  */
  YYSYMBOL_usevars = 170,                  /* usevars  */
  YYSYMBOL_usevar = 171,                   /* usevar  */
  YYSYMBOL_call = 172,                     /* call  */
  YYSYMBOL_do = 173,                       /* do  */
  YYSYMBOL_exit = 174,                     /* exit  */
  YYSYMBOL_if = 175,                       /* if  */
  YYSYMBOL_iterate = 176,                  /* iterate  */
  YYSYMBOL_leave = 177,                    /* leave  */
  YYSYMBOL_say = 178,                      /* say  */
  YYSYMBOL_return = 179,                   /* return  */
  YYSYMBOL_address = 180,                  /* address  */
  YYSYMBOL_arg = 181,                      /* arg  */
  YYSYMBOL_drop = 182,                     /* drop  */
  YYSYMBOL_interpret = 183,                /* interpret  */
  YYSYMBOL_label = 184,                    /* label  */
  YYSYMBOL_nop = 185,                      /* nop  */
  YYSYMBOL_numeric = 186,                  /* numeric  */
  YYSYMBOL_options = 187,                  /* options  */
  YYSYMBOL_parse = 188,                    /* parse  */
  YYSYMBOL_proc = 189,                     /* proc  */
  YYSYMBOL_pull = 190,                     /* pull  */
  YYSYMBOL_push = 191,                     /* push  */
  YYSYMBOL_queue = 192,                    /* queue  */
  YYSYMBOL_select = 193,                   /* select  */
  YYSYMBOL_signal = 194,                   /* signal  */
  YYSYMBOL_when = 195,                     /* when  */
  YYSYMBOL_otherwise = 196,                /* otherwise  */
  YYSYMBOL_trace = 197,                    /* trace  */
  YYSYMBOL_upper = 198,                    /* upper  */
  YYSYMBOL_address_stat = 199,             /* address_stat  */
  YYSYMBOL_200_3 = 200,                    /* @3  */
  YYSYMBOL_address_stat2 = 201,            /* address_stat2  */
  YYSYMBOL_202_4 = 202,                    /* $@4  */
  YYSYMBOL_arg_stat = 203,                 /* arg_stat  */
  YYSYMBOL_call_stat = 204,                /* call_stat  */
  YYSYMBOL_205_5 = 205,                    /* $@5  */
  YYSYMBOL_206_6 = 206,                    /* $@6  */
  YYSYMBOL_call_name = 207,                /* call_name  */
  YYSYMBOL_call_args = 208,                /* call_args  */
  YYSYMBOL_expr_stat = 209,                /* expr_stat  */
  YYSYMBOL_end_stat = 210,                 /* end_stat  */
  YYSYMBOL_end = 211,                      /* end  */
  YYSYMBOL_do_stat = 212,                  /* do_stat  */
  YYSYMBOL_do_opts = 213,                  /* do_opts  */
  YYSYMBOL_dolabel = 214,                  /* dolabel  */
  YYSYMBOL_docounter = 215,                /* docounter  */
  YYSYMBOL_doindex = 216,                  /* doindex  */
  YYSYMBOL_doitem = 217,                   /* doitem  */
  YYSYMBOL_withvars = 218,                 /* withvars  */
  YYSYMBOL_repetitor = 219,                /* repetitor  */
  YYSYMBOL_nvir = 220,                     /* nvir  */
  YYSYMBOL_naddr_with = 221,               /* naddr_with  */
  YYSYMBOL_222_7 = 222,                    /* @7  */
  YYSYMBOL_addr_with = 223,                /* addr_with  */
  YYSYMBOL_connection = 224,               /* connection  */
  YYSYMBOL_inputstmts = 225,               /* inputstmts  */
  YYSYMBOL_outputstmts = 226,              /* outputstmts  */
  YYSYMBOL_errorstmts = 227,               /* errorstmts  */
  YYSYMBOL_adeo = 228,                     /* adeo  */
  YYSYMBOL_adei = 229,                     /* adei  */
  YYSYMBOL_adio = 230,                     /* adio  */
  YYSYMBOL_inputstmt = 231,                /* inputstmt  */
  YYSYMBOL_outputstmt = 232,               /* outputstmt  */
  YYSYMBOL_errorstmt = 233,                /* errorstmt  */
  YYSYMBOL_resourcei = 234,                /* resourcei  */
  YYSYMBOL_resourceo = 235,                /* resourceo  */
  YYSYMBOL_resources = 236,                /* resources  */
  YYSYMBOL_nsimsymb = 237,                 /* nsimsymb  */
  YYSYMBOL_238_8 = 238,                    /* $@8  */
  YYSYMBOL_nnvir = 239,                    /* nnvir  */
  YYSYMBOL_240_9 = 240,                    /* $@9  */
  YYSYMBOL_nspace = 241,                   /* nspace  */
  YYSYMBOL_addrAll = 242,                  /* addrAll  */
  YYSYMBOL_addrSim = 243,                  /* addrSim  */
  YYSYMBOL_addrString = 244,               /* addrString  */
  YYSYMBOL_dovar = 245,                    /* dovar  */
  YYSYMBOL_tobyfor = 246,                  /* tobyfor  */
  YYSYMBOL_conditional = 247,              /* conditional  */
  YYSYMBOL_drop_stat = 248,                /* drop_stat  */
  YYSYMBOL_upper_stat = 249,               /* upper_stat  */
  YYSYMBOL_exit_stat = 250,                /* exit_stat  */
  YYSYMBOL_if_stat = 251,                  /* if_stat  */
  YYSYMBOL_unexp_then = 252,               /* unexp_then  */
  YYSYMBOL_unexp_else = 253,               /* unexp_else  */
  YYSYMBOL_ipret_stat = 254,               /* ipret_stat  */
  YYSYMBOL_iterate_stat = 255,             /* iterate_stat  */
  YYSYMBOL_label_stat = 256,               /* label_stat  */
  YYSYMBOL_labelname = 257,                /* labelname  */
  YYSYMBOL_leave_stat = 258,               /* leave_stat  */
  YYSYMBOL_nop_stat = 259,                 /* nop_stat  */
  YYSYMBOL_numeric_stat = 260,             /* numeric_stat  */
  YYSYMBOL_form_expr = 261,                /* form_expr  */
  YYSYMBOL_options_stat = 262,             /* options_stat  */
  YYSYMBOL_parse_stat = 263,               /* parse_stat  */
  YYSYMBOL_parse_flags = 264,              /* parse_flags  */
  YYSYMBOL_templs = 265,                   /* templs  */
  YYSYMBOL_parse_param = 266,              /* parse_param  */
  YYSYMBOL_proc_stat = 267,                /* proc_stat  */
  YYSYMBOL_pull_stat = 268,                /* pull_stat  */
  YYSYMBOL_push_stat = 269,                /* push_stat  */
  YYSYMBOL_queue_stat = 270,               /* queue_stat  */
  YYSYMBOL_say_stat = 271,                 /* say_stat  */
  YYSYMBOL_return_stat = 272,              /* return_stat  */
  YYSYMBOL_sel_end = 273,                  /* sel_end  */
  YYSYMBOL_select_stat = 274,              /* select_stat  */
  YYSYMBOL_when_stats = 275,               /* when_stats  */
  YYSYMBOL_when_stat = 276,                /* when_stat  */
  YYSYMBOL_ex_when_stat = 277,             /* ex_when_stat  */
  YYSYMBOL_otherwise_stat = 278,           /* otherwise_stat  */
  YYSYMBOL_signal_stat = 279,              /* signal_stat  */
  YYSYMBOL_signal_name = 280,              /* signal_name  */
  YYSYMBOL_namespec = 281,                 /* namespec  */
  YYSYMBOL_asymbol = 282,                  /* asymbol  */
  YYSYMBOL_on = 283,                       /* on  */
  YYSYMBOL_off = 284,                      /* off  */
  YYSYMBOL_c_action = 285,                 /* c_action  */
  YYSYMBOL_s_action = 286,                 /* s_action  */
  YYSYMBOL_trace_stat = 287,               /* trace_stat  */
  YYSYMBOL_whatever = 288,                 /* whatever  */
  YYSYMBOL_assignment = 289,               /* assignment  */
  YYSYMBOL_ass_part = 290,                 /* ass_part  */
  YYSYMBOL_plusassignment = 291,           /* plusassignment  */
  YYSYMBOL_plus_ass_part = 292,            /* plus_ass_part  */
  YYSYMBOL_minusassignment = 293,          /* minusassignment  */
  YYSYMBOL_minus_ass_part = 294,           /* minus_ass_part  */
  YYSYMBOL_multassignment = 295,           /* multassignment  */
  YYSYMBOL_mult_ass_part = 296,            /* mult_ass_part  */
  YYSYMBOL_divassignment = 297,            /* divassignment  */
  YYSYMBOL_div_ass_part = 298,             /* div_ass_part  */
  YYSYMBOL_intdivassignment = 299,         /* intdivassignment  */
  YYSYMBOL_intdiv_ass_part = 300,          /* intdiv_ass_part  */
  YYSYMBOL_modulusassignment = 301,        /* modulusassignment  */
  YYSYMBOL_modulus_ass_part = 302,         /* modulus_ass_part  */
  YYSYMBOL_andassignment = 303,            /* andassignment  */
  YYSYMBOL_and_ass_part = 304,             /* and_ass_part  */
  YYSYMBOL_xorassignment = 305,            /* xorassignment  */
  YYSYMBOL_xor_ass_part = 306,             /* xor_ass_part  */
  YYSYMBOL_orassignment = 307,             /* orassignment  */
  YYSYMBOL_or_ass_part = 308,              /* or_ass_part  */
  YYSYMBOL_concatassignment = 309,         /* concatassignment  */
  YYSYMBOL_concat_ass_part = 310,          /* concat_ass_part  */
  YYSYMBOL_expr = 311,                     /* expr  */
  YYSYMBOL_312_10 = 312,                   /* $@10  */
  YYSYMBOL_313_11 = 313,                   /* $@11  */
  YYSYMBOL_314_12 = 314,                   /* $@12  */
  YYSYMBOL_315_13 = 315,                   /* $@13  */
  YYSYMBOL_316_14 = 316,                   /* $@14  */
  YYSYMBOL_317_15 = 317,                   /* $@15  */
  YYSYMBOL_318_16 = 318,                   /* $@16  */
  YYSYMBOL_319_17 = 319,                   /* $@17  */
  YYSYMBOL_320_18 = 320,                   /* $@18  */
  YYSYMBOL_321_19 = 321,                   /* $@19  */
  YYSYMBOL_322_20 = 322,                   /* $@20  */
  YYSYMBOL_323_21 = 323,                   /* $@21  */
  YYSYMBOL_324_22 = 324,                   /* $@22  */
  YYSYMBOL_325_23 = 325,                   /* $@23  */
  YYSYMBOL_326_24 = 326,                   /* $@24  */
  YYSYMBOL_327_25 = 327,                   /* $@25  */
  YYSYMBOL_328_26 = 328,                   /* $@26  */
  YYSYMBOL_329_27 = 329,                   /* $@27  */
  YYSYMBOL_330_28 = 330,                   /* $@28  */
  YYSYMBOL_331_29 = 331,                   /* $@29  */
  YYSYMBOL_332_30 = 332,                   /* $@30  */
  YYSYMBOL_333_31 = 333,                   /* $@31  */
  YYSYMBOL_334_32 = 334,                   /* $@32  */
  YYSYMBOL_335_33 = 335,                   /* $@33  */
  YYSYMBOL_336_34 = 336,                   /* $@34  */
  YYSYMBOL_337_35 = 337,                   /* $@35  */
  YYSYMBOL_338_36 = 338,                   /* $@36  */
  YYSYMBOL_exprs_sub = 339,                /* exprs_sub  */
  YYSYMBOL_symbtree = 340,                 /* symbtree  */
  YYSYMBOL_function = 341,                 /* function  */
  YYSYMBOL_func_args = 342,                /* func_args  */
  YYSYMBOL_343_37 = 343,                   /* @37  */
  YYSYMBOL_msgtarget = 344,                /* msgtarget  */
  YYSYMBOL_msgname = 345,                  /* msgname  */
  YYSYMBOL_bracket_args = 346,             /* bracket_args  */
  YYSYMBOL_347_38 = 347,                   /* @38  */
  YYSYMBOL_intfunc = 348,                  /* intfunc  */
  YYSYMBOL_extfunc = 349,                  /* extfunc  */
  YYSYMBOL_template = 350,                 /* template  */
  YYSYMBOL_solid = 351,                    /* solid  */
  YYSYMBOL_offset = 352,                   /* offset  */
  YYSYMBOL_string = 353,                   /* string  */
  YYSYMBOL_pv = 354,                       /* pv  */
  YYSYMBOL_exprs = 355,                    /* exprs  */
  YYSYMBOL_356_39 = 356,                   /* $@39  */
  YYSYMBOL_nexpr = 357,                    /* nexpr  */
  YYSYMBOL_anyvars = 358,                  /* anyvars  */
  YYSYMBOL_xsimsymb = 359,                 /* xsimsymb  */
  YYSYMBOL_simsymb = 360                   /* simsymb  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2533

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  145
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  216
/* YYNRULES -- Number of rules.  */
#define YYNRULES  532
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  727

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   386


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,   137,   130,     2,
     142,   143,   135,   133,   141,   134,     2,   136,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   131,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   140,     2,   144,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   129,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   132,   138,   139
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   175,   175,   175,   185,   188,   192,   199,   212,   213,
     216,   220,   225,   225,   237,   240,   241,   244,   246,   248,
     252,   254,   257,   259,   262,   263,   266,   271,   272,   275,
     276,   279,   280,   283,   284,   287,   288,   291,   295,   298,
     299,   300,   301,   304,   305,   306,   307,   308,   309,   310,
     311,   312,   313,   314,   315,   316,   317,   318,   319,   320,
     321,   322,   323,   324,   325,   326,   327,   328,   329,   330,
     331,   332,   333,   334,   335,   336,   337,   338,   339,   340,
     347,   350,   353,   356,   359,   361,   364,   366,   370,   374,
     381,   385,   390,   394,   398,   402,   406,   410,   414,   418,
     422,   426,   430,   434,   438,   442,   446,   450,   454,   458,
     463,   467,   471,   475,   479,   483,   483,   487,   490,   491,
     492,   492,   494,   498,   504,   510,   510,   515,   515,   521,
     522,   523,   524,   525,   531,   536,   537,   543,   544,   547,
     555,   558,   580,   586,   588,   589,   592,   604,   605,   609,
     615,   617,   619,   621,   625,   626,   627,   628,   631,   635,
     639,   643,   646,   647,   648,   651,   657,   659,   662,   665,
     666,   667,   670,   670,   676,   679,   680,   681,   684,   685,
     686,   687,   690,   694,   698,   702,   703,   704,   705,   706,
     709,   710,   711,   712,   713,   716,   717,   718,   719,   720,
     723,   724,   727,   728,   731,   732,   735,   736,   738,   739,
     744,   745,   747,   748,   749,   750,   755,   763,   764,   777,
     778,   782,   783,   787,   790,   790,   794,   794,   798,   799,
     802,   803,   806,   814,   822,   825,   826,   827,   830,   831,
     832,   835,   836,   840,   841,   845,   849,   857,   867,   869,
     871,   872,   875,   878,   881,   886,   888,   889,   892,   900,
     904,   906,   907,   910,   911,   914,   917,   918,   919,   922,
     924,   926,   927,   928,   931,   934,   935,   938,   941,   947,
     953,   954,   957,   959,   961,   963,   965,   967,   969,   973,
     975,   978,   979,   980,   981,   982,   983,   984,   986,   987,
     990,   991,   992,   993,   994,   998,  1002,  1006,  1010,  1014,
    1022,  1023,  1024,  1027,  1032,  1033,  1035,  1036,  1037,  1041,
    1042,  1043,  1046,  1052,  1054,  1055,  1065,  1066,  1069,  1073,
    1079,  1082,  1083,  1085,  1086,  1087,  1088,  1089,  1095,  1099,
    1100,  1106,  1107,  1108,  1111,  1112,  1113,  1116,  1117,  1120,
    1123,  1126,  1127,  1128,  1129,  1132,  1133,  1134,  1135,  1138,
    1140,  1142,  1143,  1147,  1151,  1167,  1173,  1180,  1186,  1193,
    1199,  1206,  1212,  1219,  1225,  1232,  1238,  1245,  1251,  1258,
    1264,  1271,  1277,  1284,  1290,  1295,  1302,  1302,  1335,  1335,
    1338,  1338,  1341,  1341,  1344,  1345,  1345,  1348,  1349,  1349,
    1352,  1353,  1353,  1356,  1357,  1357,  1360,  1361,  1361,  1364,
    1365,  1365,  1368,  1369,  1369,  1372,  1373,  1373,  1376,  1377,
    1377,  1380,  1381,  1383,  1384,  1384,  1388,  1389,  1389,  1393,
    1394,  1394,  1398,  1399,  1399,  1403,  1404,  1404,  1408,  1409,
    1409,  1413,  1414,  1414,  1417,  1418,  1418,  1421,  1422,  1422,
    1425,  1426,  1426,  1429,  1430,  1430,  1433,  1434,  1434,  1437,
    1438,  1438,  1441,  1442,  1442,  1445,  1446,  1450,  1454,  1455,
    1456,  1458,  1459,  1466,  1470,  1474,  1476,  1481,  1482,  1484,
    1486,  1487,  1490,  1491,  1492,  1495,  1499,  1501,  1505,  1505,
    1517,  1519,  1523,  1527,  1530,  1533,  1538,  1541,  1541,  1548,
    1551,  1554,  1555,  1556,  1559,  1561,  1563,  1565,  1567,  1569,
    1571,  1573,  1575,  1579,  1580,  1589,  1590,  1594,  1600,  1601,
    1602,  1612,  1612,  1692,  1736,  1737,  1740,  1742,  1744,  1746,
    1750,  1753,  1754
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "ADDRESS", "ARG",
  "CALL", "DO", "TO", "BY", "FOR", "WHILE", "UNTIL", "EXIT", "IF", "THEN",
  "ELSE", "ITERATE", "INTERPRET", "LEAVE", "NOP", "NUMERIC", "PARSE",
  "EXTERNAL", "SOURCE", "VAR", "VALUE", "WITH", "PROCEDURE", "EXPOSE",
  "PULL", "PUSH", "QUEUE", "SAY", "RETURN", "SELECT", "WHEN", "DROP",
  "OTHERWISE", "SIGNAL", "ON", "OFF", "SYNTAX", "HALT", "NOVALUE", "TRACE",
  "END", "UPPER", "ASSIGNMENTVARIABLE", "STATSEP", "FOREVER", "DIGITS",
  "FORM", "FUZZ", "SCIENTIFIC", "ENGINEERING", "NOT", "CONCATENATE",
  "MODULUS", "GTE", "GT", "LTE", "LT", "DIFFERENT", "EQUALEQUAL",
  "NOTEQUALEQUAL", "OFFSET", "SPACE", "EXP", "XOR", "PLACEHOLDER",
  "NOTREADY", "CONSYMBOL", "SIMSYMBOL", "EXFUNCNAME", "INFUNCNAME",
  "LABEL", "DOVARIABLE", "HEXSTRING", "STRING", "VERSION", "LINEIN",
  "WHATEVER", "NAME", "FAILURE", "BINSTRING", "OPTIONS", "ENVIRONMENT",
  "LOSTDIGITS", "DOLABEL", "DOCOUNTER", "DOWITH", "DOINDEX", "DOITEM",
  "DOOVER", "GTGT", "LTLT", "NOTGTGT", "NOTLTLT", "GTGTE", "LTLTE",
  "INPUT", "OUTPUT", "ERROR", "NORMAL", "APPEND", "REPLACE", "STREAM",
  "STEM", "LIFO", "FIFO", "NOEOL", "LOWER", "CASELESS", "MSGSEND",
  "CASCADE", "DCOLON", "USEARG", "MSGSUPER", "BLOCK",
  "PLUSASSIGNMENTVARIABLE", "MINUSASSIGNMENTVARIABLE",
  "MULTASSIGNMENTVARIABLE", "DIVASSIGNMENTVARIABLE",
  "MODULUSASSIGNMENTVARIABLE", "INTDIVASSIGNMENTVARIABLE",
  "ORASSIGNMENTVARIABLE", "XORASSIGNMENTVARIABLE", "ANDASSIGNMENTVARIABLE",
  "CONCATASSIGNMENTVARIABLE", "'|'", "'&'", "'='", "CCAT", "'+'", "'-'",
  "'*'", "'/'", "'%'", "UMINUS", "UPLUS", "'['", "','", "'('", "')'",
  "']'", "$accept", "start", "$@1", "prog", "dirsect", "dirblock",
  "directive", "@2", "dirwords", "dirword", "stats", "xstats",
  "ystatement", "lonely_end", "nxstats", "ncl", "nlncl", "optLabels",
  "statement", "gruff", "mttstatement", "mtstatement", "nclstatement",
  "oo_expose_stat", "oo_usearg_stat", "usevars", "usevar", "call", "do",
  "exit", "if", "iterate", "leave", "say", "return", "address", "arg",
  "drop", "interpret", "label", "nop", "numeric", "options", "parse",
  "proc", "pull", "push", "queue", "select", "signal", "when", "otherwise",
  "trace", "upper", "address_stat", "@3", "address_stat2", "$@4",
  "arg_stat", "call_stat", "$@5", "$@6", "call_name", "call_args",
  "expr_stat", "end_stat", "end", "do_stat", "do_opts", "dolabel",
  "docounter", "doindex", "doitem", "withvars", "repetitor", "nvir",
  "naddr_with", "@7", "addr_with", "connection", "inputstmts",
  "outputstmts", "errorstmts", "adeo", "adei", "adio", "inputstmt",
  "outputstmt", "errorstmt", "resourcei", "resourceo", "resources",
  "nsimsymb", "$@8", "nnvir", "$@9", "nspace", "addrAll", "addrSim",
  "addrString", "dovar", "tobyfor", "conditional", "drop_stat",
  "upper_stat", "exit_stat", "if_stat", "unexp_then", "unexp_else",
  "ipret_stat", "iterate_stat", "label_stat", "labelname", "leave_stat",
  "nop_stat", "numeric_stat", "form_expr", "options_stat", "parse_stat",
  "parse_flags", "templs", "parse_param", "proc_stat", "pull_stat",
  "push_stat", "queue_stat", "say_stat", "return_stat", "sel_end",
  "select_stat", "when_stats", "when_stat", "ex_when_stat",
  "otherwise_stat", "signal_stat", "signal_name", "namespec", "asymbol",
  "on", "off", "c_action", "s_action", "trace_stat", "whatever",
  "assignment", "ass_part", "plusassignment", "plus_ass_part",
  "minusassignment", "minus_ass_part", "multassignment", "mult_ass_part",
  "divassignment", "div_ass_part", "intdivassignment", "intdiv_ass_part",
  "modulusassignment", "modulus_ass_part", "andassignment", "and_ass_part",
  "xorassignment", "xor_ass_part", "orassignment", "or_ass_part",
  "concatassignment", "concat_ass_part", "expr", "$@10", "$@11", "$@12",
  "$@13", "$@14", "$@15", "$@16", "$@17", "$@18", "$@19", "$@20", "$@21",
  "$@22", "$@23", "$@24", "$@25", "$@26", "$@27", "$@28", "$@29", "$@30",
  "$@31", "$@32", "$@33", "$@34", "$@35", "$@36", "exprs_sub", "symbtree",
  "function", "func_args", "@37", "msgtarget", "msgname", "bracket_args",
  "@38", "intfunc", "extfunc", "template", "solid", "offset", "string",
  "pv", "exprs", "$@39", "nexpr", "anyvars", "xsimsymb", "simsymb", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-619)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-526)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -619,    63,  -619,  -619,  -619,    23,     0,   278,   -47,  -619,
      30,    31,  -619,  -619,  -619,  1556,  -619,  -619,  -619,  -619,
      26,  -619,  -619,  -619,  -619,   402,  -619,    36,   -47,  -619,
    -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,
    -619,  -619,  -619,    32,  -619,  -619,  -619,  -619,  -619,  -619,
    -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,   475,  -619,
    -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,
    -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,
    -619,  -619,  -619,  -619,  -619,  -619,  -619,     9,  -619,  -619,
    -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,
    -619,  -619,  1245,  1333,  -619,  -619,  -619,  -619,   217,  -619,
      64,  -619,  -619,   585,  -619,  2183,  1916,   237,   253,  2183,
    2183,  -619,    65,    28,  2183,    37,   345,  2183,   180,   262,
      91,  2183,  2183,    77,   417,  -619,  -619,   443,    28,  -619,
    -619,  -619,  -619,   283,    64,  -619,  -619,  -619,  -619,  -619,
    -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,
    -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  2183,
    -619,  2183,  -619,  2183,  -619,  2183,  -619,  2183,  -619,  2183,
    -619,  2183,  -619,  2183,  -619,  2183,  -619,  2183,  -619,  2183,
    2295,  -619,  -619,  -619,  -619,    90,  -619,  -619,  -619,  -619,
      13,  -619,  -619,  -619,     9,  -619,    28,   -56,   163,   -43,
    -619,  -619,   -56,  -619,   -56,  2006,  -619,  -619,    26,  -619,
    -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,   185,
     277,  -619,  2095,  2295,  -619,    26,  2295,  -619,  -619,   171,
     219,  -619,  -619,    46,  -619,   110,    39,   110,  -619,  1465,
     272,  2295,  -619,  -619,  2183,   465,  2183,  -619,  -619,  -619,
    -619,  -619,    76,  1157,  -619,    71,  -619,  -619,    96,     4,
     630,    65,  -619,    34,  -619,  -619,  -619,  -619,   705,  -619,
    2183,  -619,   297,  -619,   281,   442,  2183,  -619,   313,  -619,
     329,   351,    26,  -619,  -619,  -619,  -619,  -619,  -619,  -619,
    -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,
    -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,
    -619,  -619,   419,   419,  -619,  -619,  -619,  -619,  -619,  -619,
    -619,  -619,  -619,  -619,  2006,  -619,    90,   402,    26,    58,
    -619,  -619,     9,  -619,  -619,    12,    80,  2183,  -619,  -619,
    -619,  -619,  -619,    38,  -619,   366,  2183,   724,  -619,  -619,
    -619,   499,  -619,  -619,   588,   105,  2380,    66,  -619,  -619,
    -619,  2183,   227,  -619,  -619,  -619,  2183,  -619,  2183,  -619,
    -619,    65,  -619,  -619,  -619,   -20,   -19,    -1,   189,    65,
    -619,  -619,  -619,  2295,  -619,  2183,  -619,  -619,   438,  2295,
    -619,  -619,   245,  -619,  -619,  -619,  -619,  -619,    65,    39,
    -619,   474,  -619,  -619,   252,  1421,   273,    25,  -619,  2295,
    -619,  -619,  -619,  -619,  -619,  -619,    68,  -619,   480,  2295,
    -619,  -619,  -619,  2183,  2183,  2183,  2183,  2183,  2183,  2183,
    2183,  2183,  2183,  2183,  2183,  2183,  2183,  2183,  2183,  2183,
    2183,  -619,  -619,  -619,   182,  -619,  -619,  -619,  2183,  2183,
    2183,  2183,  2183,  2183,  2183,  2183,  -619,  2183,  -619,  -619,
      28,  -619,  -619,  -619,  -619,  -619,   169,  -619,   211,   486,
    -619,  -619,  -619,  -619,  -619,  -619,  -619,   225,   235,   255,
    2183,  2183,    64,  2183,   629,   349,  -619,  -619,   315,   634,
    -619,   495,  -619,  -619,  -619,   300,   300,   300,   369,  2270,
     315,  -619,   189,  -619,   189,  -619,   189,  -619,   226,  -619,
    2295,  -619,  -619,    39,  -619,  -619,  -619,   276,  -619,   788,
    -619,   331,  -619,   507,  -619,   469,   130,  1212,  1212,  1212,
    1212,  1212,  1212,  1212,   469,   -56,  2393,  1212,  1212,  1212,
    1212,  1212,  1212,   236,  -619,  -619,  2393,  2283,  1212,   327,
     327,   130,   130,   130,   238,  -619,  2183,  -619,  -619,  -619,
    -619,  -619,  -619,  -619,  2183,  2380,  2380,    26,  2380,  2183,
     928,  -619,   359,  -619,  -619,  -619,   300,   300,   517,  -619,
     300,   300,   137,  -619,   300,   300,   526,   107,    86,   199,
     300,  -619,   244,   250,   260,  -619,   394,  -619,  -619,   293,
    -619,  -619,  -619,  -619,  -619,  2380,  -619,  -619,   370,   372,
    -619,  1691,   524,  2380,  -619,   439,  -619,   300,   358,   300,
     377,   300,   358,   300,   382,   300,   377,   300,   382,  -619,
     648,  -619,   771,  -619,   771,   315,  -619,  -619,  -619,  -619,
    -619,   549,  -619,   542,  -619,  1826,  -619,  -619,  2183,  2183,
    2183,   524,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,
    -619,   157,   287,   314,   544,    20,  -619,  -619,  -619,   139,
     143,  -619,  -619,  -619,  -619,  -619,  -619,  2183,  2380,  2380,
    2380,   524,  1068,  -619,  -619,   300,  -619,  -619,   300,  -619,
    -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,   521,
    2380,  -619,  -619,  -619,  -619,  -619,  -619,   381,     9,  -619,
    -619,  -619,  -619,  -619,  -619,  -619,  -619
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       2,     0,    34,     1,     3,    37,    32,     0,     7,     9,
      37,    37,    21,    25,    24,    37,    35,    36,    34,   100,
      31,   259,    33,   258,    14,     0,     8,    37,     6,    20,
      96,    97,    88,    89,    90,    91,    92,    99,    93,   101,
     102,   104,   105,     0,   106,   107,   108,    94,    95,   109,
     111,    98,   112,   110,   113,   142,   114,   365,   423,   421,
     400,   429,   435,   432,   438,   441,   444,   447,   418,   415,
     412,   472,   485,   500,   499,   473,   475,   474,   103,   450,
     453,   456,   459,   462,   465,   468,   469,    87,   476,   367,
     369,   371,   373,   377,   375,   383,   381,   379,   385,   406,
     409,   426,     0,     0,   394,   397,   403,   386,     0,    38,
       0,    78,    79,     0,   147,   525,     0,     0,     0,   525,
     525,   115,     0,     0,     0,     0,     0,   525,     0,     0,
       0,   525,   525,     0,     0,   326,   327,   525,     0,    43,
      45,    46,    44,     0,     0,    47,    48,    66,    49,    40,
      41,    42,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    62,    61,    63,    64,    65,    77,   525,
      67,   525,    68,   525,    69,   525,    70,   525,    71,   525,
      72,   525,    75,   525,    73,   525,    74,   525,    76,   525,
     141,   471,   477,   488,   488,    30,    34,    18,    17,    19,
       0,    16,    81,   530,     0,    80,   527,   422,     0,    85,
      86,   480,   478,   481,   479,   525,   252,   253,    39,   138,
     349,   350,   347,   348,   516,   515,   517,   125,   137,     0,
       0,   127,   168,   524,   245,   250,    34,   532,   531,     0,
       0,   308,   309,     0,   503,   520,   124,   520,   290,   502,
       0,   254,   264,   274,   266,     0,   272,   277,   281,   291,
     293,   296,     0,     0,   295,   282,   294,   292,   285,   288,
       0,     0,   301,     0,   305,   306,   307,   316,     0,   343,
       0,   342,     0,   341,     0,     0,     0,   363,     0,   360,
       0,     0,    26,   364,   366,   368,   370,   372,   374,   376,
     378,   380,   382,   384,   419,   398,   427,   433,   430,   436,
     439,   442,   445,   416,   413,   410,   448,   451,   454,   457,
     460,   463,     0,     0,   404,   407,   424,   388,   390,   392,
     395,   401,   497,   487,   525,   486,    29,     0,    13,     0,
     526,    83,    87,   484,   387,     0,   523,   525,   129,   352,
     353,   354,   351,     0,   130,     0,   525,     0,   234,   150,
     151,     0,   148,   149,   240,     0,   229,     0,   256,   261,
     120,     0,     0,   169,   170,   171,     0,   116,   525,   118,
     518,     0,   519,   513,   514,     0,     0,     0,     0,     0,
     506,   512,   241,   265,   271,     0,   275,   276,     0,   273,
     297,   299,     0,   283,   286,   284,   287,   280,     0,   279,
     302,     0,   321,   317,     0,     0,     0,   329,   320,   330,
     331,   333,   357,   356,   358,   355,     0,   334,     0,   359,
     361,   243,   145,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   496,   493,   466,   490,   488,   488,   467,     0,     0,
       0,     0,     0,     0,     0,     0,   470,   525,   489,    15,
     529,    84,   483,   482,   521,   126,   139,   131,     0,     0,
     135,   128,   163,   228,   162,   152,   153,   154,   155,     0,
       0,     0,     0,     0,   416,   164,   251,    34,   172,   172,
     181,     0,   178,   179,   180,   229,   229,   229,   177,     0,
     172,   289,     0,   507,     0,   505,     0,   504,     0,   501,
     270,   267,   298,   278,   303,   314,   325,    34,   315,     0,
     319,     0,   335,     0,   339,   420,   399,   428,   434,   431,
     437,   440,   443,   446,   417,   414,   411,   449,   452,   455,
     458,   461,   464,   491,   494,   495,   405,   408,   425,   389,
     391,   393,   396,   402,     0,   528,   525,   140,   346,   344,
     345,   132,   156,   157,     0,   229,   229,    37,   229,     0,
       0,   121,     0,   117,   176,   182,   229,   229,   189,   183,
     229,   229,   194,   184,   229,   229,   199,     0,     0,     0,
     229,   122,     0,     0,     0,   508,     0,   318,    37,     0,
     313,   336,   488,   498,   522,   229,   238,   239,    37,     0,
      37,    37,   161,   229,   249,   246,   173,   229,   185,   229,
     187,   229,   190,   229,   192,   229,   195,   229,   197,   201,
       0,   203,     0,   205,     0,   172,   511,   510,   509,    34,
     328,     0,   492,   166,    22,    37,   146,    23,     0,     0,
       0,   160,   165,    34,   186,   188,   191,   193,   196,   198,
     209,     0,     0,     0,     0,     0,   200,   206,   215,     0,
       0,   202,   210,   204,   123,    37,   311,     0,   229,   229,
     229,   159,     0,   217,   216,   229,   219,   218,   229,   221,
     220,   223,   222,   208,   207,   212,   211,   214,   213,   322,
     229,   235,   237,   236,   158,   248,   247,     0,     0,   323,
     167,   227,   230,   231,   233,   232,   225
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -619,  -619,  -619,  -619,   543,    48,  -619,  -619,  -619,   234,
     554,  -619,   -11,  -619,   -33,  -104,  -211,    -9,  -553,    -7,
    -619,  -619,  -619,  -619,  -619,   239,  -619,  -619,  -619,  -619,
    -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,
    -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,
    -264,  -251,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,
    -619,  -619,  -619,   228,  -619,  -619,   -32,  -619,  -619,  -619,
    -619,   104,   113,  -619,  -619,  -619,  -484,  -619,    33,  -619,
    -619,  -619,  -619,  -619,  -619,  -619,  -424,  -473,  -328,  -619,
     -36,  -117,  -619,  -619,   -44,  -619,    89,  -619,  -106,  -619,
    -619,  -618,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,
    -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,
    -254,   350,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,
    -619,   206,  -619,  -619,  -619,  -619,   209,   513,   515,   532,
     413,   373,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,
    -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,
    -619,  -619,  -619,  -619,  -619,  -619,   -14,  -619,  -619,  -619,
    -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,
    -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,  -619,
    -619,  -619,  -619,  -619,   334,  -112,  -619,  -175,  -619,   347,
    -619,  -619,  -619,   328,   338,  -125,  -619,   181,  -245,    50,
    -336,  -619,   -78,  -116,   -85,  -115
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,     2,     4,     8,     9,    10,    25,   200,   201,
      11,   618,    12,    13,   619,    20,     5,     6,    14,    15,
      16,   109,   110,   111,   112,   208,   209,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,    21,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   243,   377,   498,   140,   141,
     347,   356,   227,   475,   142,   143,   144,   145,   232,   362,
     363,   487,   488,   489,   364,   378,   581,   582,   379,   501,
     502,   503,   504,   585,   589,   593,   505,   506,   507,   676,
     681,   682,   697,   698,   694,   695,   484,   721,   722,   723,
     365,   661,   492,   146,   147,   148,   149,   150,   151,   152,
     153,    22,    23,   154,   155,   156,   398,   157,   158,   270,
     246,   271,   159,   160,   161,   162,   163,   164,   610,   165,
     417,   418,    17,   531,   166,   282,   479,   228,   229,   230,
     425,   426,   167,   288,   168,   169,   170,   171,   172,   173,
     174,   175,   176,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   188,   189,   233,   215,   461,   462,
     463,   464,   434,   465,   458,   459,   444,   443,   442,   433,
     460,   435,   437,   436,   438,   439,   440,   441,   445,   446,
     447,   448,   449,   450,   344,   191,   192,   333,   334,   453,
     454,   466,   467,   193,   194,   248,   389,   390,   231,   249,
     345,   566,   346,   205,   206,   239
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      29,   190,   210,   240,   391,   274,   218,   250,   108,   195,
     247,   476,   235,   472,   415,   583,    29,   409,   247,   335,
     476,   703,   290,    -5,   620,   367,   601,   416,   291,   278,
     -11,    -4,   586,   202,   595,   410,   -10,   234,   252,   477,
     292,   241,   242,   691,   207,   383,   383,   370,    18,   257,
     405,   384,   384,   275,   276,   620,    26,   322,   323,   289,
      50,    18,    52,     3,   383,   654,   244,   496,     7,   532,
     384,   371,   372,   714,   196,    19,    26,   237,   277,   337,
     497,   203,   590,   594,   332,  -263,  -134,   641,   212,   214,
     340,   293,   244,   294,  -119,   295,   338,   296,   342,   297,
     203,   298,   236,   299,   203,   300,   203,   301,   639,   302,
     251,   303,    18,  -520,   629,   406,  -338,   373,   374,   339,
     478,   635,   512,   514,   375,    18,   671,   672,   673,   674,
    -520,   564,   709,   247,   245,   247,  -520,    72,     7,  -520,
     705,   516,  -520,  -520,   707,   -11,     7,   400,   238,  -520,
     478,   -10,   483,   415,   523,   473,  -520,   411,   693,   247,
     245,   684,  -520,    72,   341,    19,   529,   633,  -520,  -520,
     204,   637,   368,   483,   204,  -520,   204,   587,   591,   245,
     381,   258,    72,   403,   259,   402,   348,   336,   376,  -229,
    -229,  -229,  -229,  -229,  -229,  -229,  -520,   314,  -520,  -520,
     643,   470,   260,   261,   262,   263,  -520,  -520,   404,   264,
    -229,   -82,   568,  -229,  -229,  -229,  -229,  -229,   366,  -255,
     369,   474,  -520,  -226,  -520,  -520,   265,   349,   500,  -226,
     614,   216,   217,  -520,  -226,  -226,   493,   597,   237,   599,
     393,  -226,   399,   322,   323,   671,   672,   673,   674,   671,
     672,   673,   674,   525,   237,   350,   511,   210,   627,   266,
     267,    72,   631,   272,   519,   483,   419,  -260,   351,   247,
     332,   522,   429,   392,   528,  -229,   518,   247,   354,    24,
     554,   555,   421,   569,   237,  -257,   580,   352,   696,   570,
     273,   268,   269,   483,   237,   380,   247,   382,   420,   553,
     510,  -262,  -229,  -229,  -229,  -229,  -229,  -229,  -229,   238,
    -300,  -324,   567,  -324,   430,   699,   606,   486,   608,   349,
    -242,  -324,   422,   349,   423,   238,   485,  -229,  -229,  -229,
     431,  -144,   304,   305,   306,   307,   308,   309,   310,   311,
     312,  -312,   313,   314,   315,  -332,   253,   350,   574,   -12,
     -12,   350,   432,  -224,   565,   238,   -12,   499,   579,  -224,
     351,  -362,   509,  -174,   351,   238,   483,   480,   424,   605,
     316,   317,   318,   319,   320,   321,   609,  -244,   612,   352,
    -226,   520,   613,   352,   305,   372,  -226,   646,   577,   322,
     323,  -226,  -226,   647,   314,   254,   255,   256,  -226,  -143,
     602,   527,   603,   648,   604,   324,   325,   326,   649,   327,
     328,   329,   330,   331,  -136,   -27,   332,    55,   279,   535,
     536,   537,   538,   539,   540,   541,   542,   543,   544,   545,
     546,   547,   548,   549,   550,   551,   552,   652,   685,   521,
     322,   323,   280,   427,   556,   557,   558,   559,   560,   561,
     562,   563,   692,   203,   663,   495,   220,   221,   224,   225,
     599,   508,   329,   330,   331,   226,   394,   332,   286,   597,
     598,   599,   724,   197,   198,   524,   575,   576,   598,   578,
     199,   534,   597,   422,   349,   423,  -268,   571,   222,   223,
     395,   451,    73,    74,   651,   281,   584,   452,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,   611,    68,
      69,    70,   350,  -269,    71,    72,    73,    74,   396,   397,
      75,    76,  -304,   677,   287,   351,   305,    77,  -340,   424,
      58,   658,   659,   660,  -133,   719,   314,    79,    80,    81,
      82,    83,    84,  -175,   352,   701,    71,    72,    73,    74,
     686,   687,    75,    76,    28,  -337,    85,    86,   704,    77,
     615,    88,   706,   708,    27,   623,   513,   515,   517,   625,
     621,   469,    99,   100,   101,   650,   102,   103,   104,   105,
     106,   471,   322,   323,   481,   107,   219,   656,    85,    86,
     485,   486,   573,    88,   588,   592,   596,  -310,   490,   491,
     572,   621,   327,   328,   329,   330,   331,   190,   683,   332,
    -226,   655,   726,   657,   108,   626,  -226,   107,   598,   599,
     408,  -226,  -226,   530,   220,   221,   597,   598,  -226,   700,
     702,   407,   725,   725,   259,   533,  -228,  -228,  -228,  -228,
    -228,   190,   353,   355,   688,   689,   690,   283,   108,   284,
     455,   455,   260,   261,   262,   263,   222,   223,   428,   264,
     456,   456,   224,   225,   616,   617,   285,   622,   468,   226,
     457,     0,     0,   710,     0,   628,   630,  -228,   655,   632,
     634,   716,  -174,   636,   638,     0,   640,   642,   644,   645,
     304,   305,   306,   307,   308,   309,   310,   311,   312,     0,
     313,   314,   315,     0,   653,     0,   412,     0,     0,   266,
     267,     0,   662,     0,     0,     0,   664,     0,   665,   413,
     666,     0,   667,     0,   668,   482,   669,     0,   316,   317,
     318,   319,   320,   321,  -229,  -229,     0,     0,     0,     0,
      50,     0,    52,     0,     0,     0,     0,   322,   323,     0,
     414,   670,     0,   196,   671,   672,   673,   674,   675,     0,
       0,     0,     0,   324,   325,   326,     0,   327,   328,   329,
     330,   331,  -229,     0,   332,     0,     0,   711,   712,   713,
       0,     0,     0,     0,   717,     0,     0,   718,     0,   607,
     483,   -34,   -34,   -34,   -34,     0,     0,     0,     0,   720,
     -34,   -34,   -34,   -34,   -34,   -34,   -34,   -34,   -34,   -34,
       0,     0,     0,     0,     0,   -34,   -34,   -34,   -34,   -34,
     -34,   -34,   -34,   -34,   -34,   -34,   -34,     0,     0,     0,
       0,     0,   -34,   -34,   -34,   -34,   -34,     0,     0,     0,
       0,     0,     0,   -34,   -34,   -34,   -34,   -34,   -34,   -34,
     -34,   -34,   -34,     0,   -34,   -34,   -34,     0,     0,   -34,
     -34,   -34,   -34,   -34,     0,   -34,   -34,     0,     0,     0,
       0,     0,   -34,   -34,   678,   679,   680,   671,   672,   673,
     674,     0,   -34,   -34,   -34,   -34,   -34,   -34,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   -34,   -34,     0,   -34,     0,   -34,   -34,   -34,   -34,
     -34,   -34,   -34,   -34,   -34,   -34,   -34,   -34,   -34,   -34,
       0,   -34,   -34,   -34,   -34,   -34,     0,     0,     0,   624,
     -34,   -37,   -37,   -37,   -37,     0,     0,     0,     0,     0,
     -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,
       0,     0,     0,     0,     0,   -37,   -37,   -37,   -37,   -37,
     -37,   -37,   -37,   -37,   -37,   -37,   -37,     0,     0,     0,
       0,     0,   -37,   -37,   -37,   -37,     0,     0,     0,     0,
       0,     0,     0,   -37,   -37,   -37,   -37,   -37,   -37,   -37,
     -37,   -37,   -37,     0,   -37,   -37,   -37,     0,     0,   -37,
     -37,   -37,   -37,     0,     0,   -37,   -37,     0,     0,     0,
       0,     0,   -37,   -37,     0,     0,     0,     0,     0,     0,
       0,     0,   -37,   -37,   -37,   -37,   -37,   -37,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   -37,   -37,     0,   -37,     0,   -37,   -37,   -37,   -37,
     -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,
       0,   -37,   -37,   -37,   -37,   -37,     0,     0,     0,   715,
     -37,   -37,   -37,   -37,   -37,     0,     0,     0,     0,     0,
     -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,
       0,     0,     0,     0,     0,   -37,   -37,   -37,   -37,   -37,
     -37,   -37,   -37,   -37,   -37,   -37,   -37,     0,     0,     0,
       0,     0,   -37,   -37,   -37,   -37,     0,     0,     0,     0,
       0,     0,     0,   -37,   -37,   -37,   -37,   -37,   -37,   -37,
     -37,   -37,   -37,     0,   -37,   -37,   -37,     0,     0,   -37,
     -37,   -37,   -37,     0,     0,   -37,   -37,     0,     0,     0,
       0,     0,   -37,   -37,     0,     0,     0,     0,   401,     0,
       0,     0,   -37,   -37,   -37,   -37,   -37,   -37,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   -37,   -37,  -525,   -37,     0,   -37,   -37,   -37,   -37,
     -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,
       0,   -37,   -37,   -37,   -37,   -37,     0,     0,     0,     0,
     -37,     0,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,     0,    68,    69,    70,     0,     0,    71,    72,
      73,    74,     0,     0,    75,    76,     0,     0,     0,     0,
       0,    77,     0,     0,     0,     0,   211,     0,     0,     0,
       0,    79,    80,    81,    82,    83,    84,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   304,   305,
      85,    86,     0,     0,     0,    88,     0,     0,   313,   314,
       0,     0,     0,     0,     0,     0,    99,   100,   101,     0,
     102,   103,   104,   105,   106,     0,     0,     0,     0,   107,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
       0,    68,    69,    70,     0,     0,    71,    72,    73,    74,
       0,     0,    75,    76,     0,   322,   323,     0,     0,    77,
       0,     0,     0,     0,   213,     0,     0,     0,     0,    79,
      80,    81,    82,    83,    84,   327,   328,   329,   330,   331,
       0,     0,   332,     0,     0,     0,     0,     0,    85,    86,
       0,     0,     0,    88,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    99,   100,   101,     0,   102,   103,
     104,   105,   106,     0,     0,     0,     0,   107,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,     0,    68,
      69,    70,     0,     0,    71,    72,    73,    74,     0,     0,
      75,    76,     0,     0,     0,     0,     0,    77,     0,     0,
       0,     0,   526,     0,     0,     0,     0,    79,    80,    81,
      82,    83,    84,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    85,    86,     0,     0,
       0,    88,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    99,   100,   101,     0,   102,   103,   104,   105,
     106,     0,     0,     0,     0,   107,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,     0,    68,    69,    70,
       0,     0,    71,    72,    73,    74,     0,     0,    75,    76,
       0,     0,     0,     0,     0,    77,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    79,    80,    81,    82,    83,
      84,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     383,     0,     0,     0,    85,    86,   384,     0,     0,    88,
       0,     0,   224,   225,     0,     0,     0,     0,     0,   226,
      99,   100,   101,     0,   102,   103,   104,   105,   106,    30,
      31,    32,    33,   107,     0,     0,     0,     0,    34,    35,
       0,     0,    36,    37,    38,    39,    40,    41,     0,     0,
       0,     0,     0,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,     0,   385,     0,   386,   387,
      54,    55,    56,    57,     0,     0,     0,   388,     0,     0,
       0,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,     0,    68,    69,    70,     0,     0,    71,    72,    73,
      74,     0,     0,    75,    76,     0,     0,     0,     0,     0,
      77,    78,     0,     0,     0,     0,     0,     0,     0,     0,
      79,    80,    81,    82,    83,    84,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    85,
      86,     0,    87,     0,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,     0,   102,
     103,   104,   105,   106,    30,    31,    32,    33,   107,     0,
       0,     0,     0,    34,    35,     0,     0,    36,    37,    38,
      39,    40,    41,     0,     0,     0,     0,     0,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
       0,     0,     0,     0,     0,    54,   -28,    56,    57,     0,
       0,     0,     0,     0,     0,     0,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,     0,    68,    69,    70,
       0,     0,    71,    72,    73,    74,     0,     0,    75,    76,
       0,     0,     0,     0,     0,    77,    78,     0,     0,     0,
       0,     0,     0,     0,     0,    79,    80,    81,    82,    83,
      84,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    85,    86,     0,    87,     0,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,     0,   102,   103,   104,   105,   106,    30,
      31,    32,    33,   107,     0,     0,     0,     0,    34,    35,
       0,     0,    36,    37,    38,    39,    40,    41,     0,     0,
       0,     0,     0,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,     0,     0,     0,     0,     0,
      54,     0,    56,    57,     0,     0,     0,     0,     0,     0,
       0,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,     0,    68,    69,    70,     0,     0,    71,    72,    73,
      74,     0,     0,    75,    76,     0,     0,     0,     0,     0,
      77,    78,     0,     0,     0,     0,     0,     0,     0,     0,
      79,    80,    81,    82,    83,    84,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    85,
      86,     0,    87,     0,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,     0,   102,
     103,   104,   105,   106,    18,     0,     0,     0,   107,     0,
       0,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,     0,    68,    69,    70,     0,     0,    71,    72,    73,
      74,     0,     0,    75,    76,     0,     0,     0,     0,     0,
      77,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      79,    80,    81,    82,    83,    84,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    85,
      86,     0,     0,     0,    88,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    99,   100,   101,     0,   102,
     103,   104,   105,   106,   343,     0,     0,     0,   107,     0,
       0,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,     0,    68,    69,    70,     0,     0,    71,    72,    73,
      74,     0,     0,    75,    76,     0,     0,     0,     0,     0,
      77,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      79,    80,    81,    82,    83,    84,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    85,
      86,     0,     0,     0,    88,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    99,   100,   101,     0,   102,
     103,   104,   105,   106,   357,     0,     0,     0,   107,     0,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
       0,    68,    69,    70,     0,     0,    71,    72,    73,    74,
       0,   358,    75,    76,     0,     0,     0,     0,     0,    77,
       0,     0,     0,   359,   360,   361,     0,     0,     0,    79,
      80,    81,    82,    83,    84,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    85,    86,
       0,     0,     0,    88,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    99,   100,   101,     0,   102,   103,
     104,   105,   106,     0,     0,     0,     0,   107,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,     0,    68,
      69,    70,     0,     0,    71,    72,    73,    74,     0,     0,
      75,    76,     0,     0,     0,     0,     0,    77,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    79,    80,    81,
      82,    83,    84,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    85,    86,     0,     0,
       0,    88,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    99,   100,   101,     0,   102,   103,   104,   105,
     106,     0,     0,     0,     0,   107,   304,   305,   306,   307,
     308,   309,   310,   311,   312,     0,   313,   314,   315,   304,
     305,   306,   307,   308,   309,   310,   311,   312,     0,   313,
     314,   304,   305,   306,   307,   308,   309,   310,   311,   312,
       0,   313,   314,   315,   316,   317,   318,   319,   320,   321,
       0,     0,     0,     0,     0,     0,     0,   316,   317,   318,
     319,   320,   321,   322,   323,     0,     0,     0,     0,   316,
     317,   318,   319,   320,   321,     0,   322,   323,     0,   324,
     325,   326,     0,   327,   328,   329,   330,   331,   322,   323,
     332,     0,     0,   600,   326,     0,   327,   328,   329,   330,
     331,     0,     0,   332,   324,   325,   326,     0,   327,   328,
     329,   330,   331,     0,     0,   332,   304,   305,   306,   307,
     308,   309,   310,   311,   312,     0,   494,   314,   315,   304,
     305,   306,   307,   308,   309,   310,   311,   312,     0,   313,
     314,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   316,   317,   318,   319,   320,   321,
       0,     0,     0,     0,     0,     0,     0,   316,   317,   318,
     319,   320,   321,   322,   323,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   322,   323,     0,   324,
     325,   326,     0,   327,   328,   329,   330,   331,     0,     0,
     332,     0,     0,   325,   326,     0,   327,   328,   329,   330,
     331,     0,     0,   332
};

static const yytype_int16 yycheck[] =
{
      11,    15,    87,   118,   249,   130,   110,   123,    15,    18,
     122,   347,   116,     1,   278,   499,    27,   271,   130,   194,
     356,     1,   138,     0,   577,   236,   510,   278,   143,   133,
       0,     0,   505,     1,   507,     1,     0,   115,     1,     1,
     144,   119,   120,   661,    58,    65,    65,     1,    48,   127,
      46,    71,    71,   131,   132,   608,     8,   113,   114,   137,
      35,    48,    37,     0,    65,   618,     1,     1,   115,     1,
      71,    25,    26,   691,    48,    75,    28,     1,     1,    66,
      14,    72,   506,   507,   140,    48,    48,     1,   102,   103,
     206,   169,     1,   171,    48,   173,   200,   175,   141,   177,
      72,   179,   116,   181,    72,   183,    72,   185,     1,   187,
     124,   189,    48,    48,   587,   111,    48,    71,    72,   204,
      82,   594,   142,   142,    78,    48,   106,   107,   108,   109,
      65,   467,   685,   245,    69,   247,    71,    72,   115,    48,
       1,   142,    77,    78,     1,   115,   115,   262,    72,    84,
      82,   115,    66,   417,   408,   143,    65,   273,     1,   271,
      69,   645,    71,    72,     1,    75,   417,   591,    77,    78,
     142,   595,     1,    66,   142,    84,   142,   505,   506,    69,
     141,     1,    72,   112,     4,   263,     1,   196,   142,   103,
     104,   105,   106,   107,   108,   109,   131,    67,   133,   134,
       1,   143,    22,    23,    24,    25,   141,   142,   112,    29,
     103,    48,     1,   106,   107,   108,   109,   110,   232,    48,
       1,   141,   131,    66,   133,   134,    46,    42,     1,    72,
     566,    14,    15,   142,    77,    78,   131,   100,     1,   102,
     254,    84,   256,   113,   114,   106,   107,   108,   109,   106,
     107,   108,   109,     1,     1,    70,   381,   342,   586,    79,
      80,    72,   590,     1,   389,    66,   280,    48,    83,   381,
     140,    26,   286,     1,     1,    48,   388,   389,     1,     1,
     455,   456,     1,    72,     1,    48,   497,   102,     1,    78,
      28,   111,   112,    66,     1,   245,   408,   247,     1,   117,
     378,    48,   103,   104,   105,   106,   107,   108,   109,    72,
      48,    35,   143,    37,     1,     1,   527,    92,   529,    42,
      48,    45,    41,    42,    43,    72,    91,   100,   101,   102,
       1,    48,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    48,    66,    67,    68,    48,     1,    70,    93,    71,
      72,    70,     1,    66,   470,    72,    78,   371,     9,    72,
      83,    48,   376,    48,    83,    72,    66,     1,    87,   143,
      94,    95,    96,    97,    98,    99,    45,    48,   142,   102,
      66,   395,   144,   102,    57,    26,    72,   143,   492,   113,
     114,    77,    78,   143,    67,    50,    51,    52,    84,    48,
     512,   415,   514,   143,   516,   129,   130,   131,    14,   133,
     134,   135,   136,   137,    48,    45,   140,    45,     1,   433,
     434,   435,   436,   437,   438,   439,   440,   441,   442,   443,
     444,   445,   446,   447,   448,   449,   450,   612,   649,     1,
     113,   114,    25,     1,   458,   459,   460,   461,   462,   463,
     464,   465,   663,    72,    15,   366,    39,    40,    77,    78,
     102,   372,   135,   136,   137,    84,     1,   140,    25,   100,
     101,   102,   717,    71,    72,     1,   490,   491,   101,   493,
      78,     1,   100,    41,    42,    43,    48,     1,    71,    72,
      25,    72,    73,    74,   609,    78,     1,    78,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,     1,    66,
      67,    68,    70,    48,    71,    72,    73,    74,    53,    54,
      77,    78,    48,   640,    81,    83,    57,    84,    48,    87,
      55,     7,     8,     9,    48,    14,    67,    94,    95,    96,
      97,    98,    99,    48,   102,     1,    71,    72,    73,    74,
       1,     9,    77,    78,    11,    48,   113,   114,   675,    84,
     574,   118,   679,   680,    10,   579,   385,   386,   387,   580,
     577,   337,   129,   130,   131,   608,   133,   134,   135,   136,
     137,   342,   113,   114,   356,   142,     1,   619,   113,   114,
      91,    92,   488,   118,   505,   506,   507,    48,    10,    11,
     487,   608,   133,   134,   135,   136,   137,   621,   644,   140,
      66,   618,   718,   620,   621,   582,    72,   142,   101,   102,
     270,    77,    78,   417,    39,    40,   100,   101,    84,   673,
     674,     1,   717,   718,     4,   426,     7,     8,     9,    10,
      11,   655,   229,   230,   658,   659,   660,   134,   655,   134,
     322,   323,    22,    23,    24,    25,    71,    72,   285,    29,
     322,   323,    77,    78,   575,   576,   134,   578,   334,    84,
     323,    -1,    -1,   687,    -1,   586,   587,    48,   685,   590,
     591,   692,    48,   594,   595,    -1,   597,   598,   599,   600,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    -1,
      66,    67,    68,    -1,   615,    -1,     1,    -1,    -1,    79,
      80,    -1,   623,    -1,    -1,    -1,   627,    -1,   629,    14,
     631,    -1,   633,    -1,   635,     1,   637,    -1,    94,    95,
      96,    97,    98,    99,    10,    11,    -1,    -1,    -1,    -1,
      35,    -1,    37,    -1,    -1,    -1,    -1,   113,   114,    -1,
      45,   103,    -1,    48,   106,   107,   108,   109,   110,    -1,
      -1,    -1,    -1,   129,   130,   131,    -1,   133,   134,   135,
     136,   137,    48,    -1,   140,    -1,    -1,   688,   689,   690,
      -1,    -1,    -1,    -1,   695,    -1,    -1,   698,    -1,     1,
      66,     3,     4,     5,     6,    -1,    -1,    -1,    -1,   710,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      -1,    -1,    -1,    -1,    -1,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    -1,    -1,    -1,
      -1,    -1,    44,    45,    46,    47,    48,    -1,    -1,    -1,
      -1,    -1,    -1,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    -1,    66,    67,    68,    -1,    -1,    71,
      72,    73,    74,    75,    -1,    77,    78,    -1,    -1,    -1,
      -1,    -1,    84,    85,   103,   104,   105,   106,   107,   108,
     109,    -1,    94,    95,    96,    97,    98,    99,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   113,   114,    -1,   116,    -1,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
      -1,   133,   134,   135,   136,   137,    -1,    -1,    -1,     1,
     142,     3,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      -1,    -1,    -1,    -1,    -1,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    -1,    -1,    -1,
      -1,    -1,    44,    45,    46,    47,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    -1,    66,    67,    68,    -1,    -1,    71,
      72,    73,    74,    -1,    -1,    77,    78,    -1,    -1,    -1,
      -1,    -1,    84,    85,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    94,    95,    96,    97,    98,    99,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   113,   114,    -1,   116,    -1,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
      -1,   133,   134,   135,   136,   137,    -1,    -1,    -1,     1,
     142,     3,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      -1,    -1,    -1,    -1,    -1,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    -1,    -1,    -1,
      -1,    -1,    44,    45,    46,    47,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    -1,    66,    67,    68,    -1,    -1,    71,
      72,    73,    74,    -1,    -1,    77,    78,    -1,    -1,    -1,
      -1,    -1,    84,    85,    -1,    -1,    -1,    -1,     1,    -1,
      -1,    -1,    94,    95,    96,    97,    98,    99,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   113,   114,    26,   116,    -1,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
      -1,   133,   134,   135,   136,   137,    -1,    -1,    -1,    -1,
     142,    -1,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    -1,    66,    67,    68,    -1,    -1,    71,    72,
      73,    74,    -1,    -1,    77,    78,    -1,    -1,    -1,    -1,
      -1,    84,    -1,    -1,    -1,    -1,     1,    -1,    -1,    -1,
      -1,    94,    95,    96,    97,    98,    99,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    56,    57,
     113,   114,    -1,    -1,    -1,   118,    -1,    -1,    66,    67,
      -1,    -1,    -1,    -1,    -1,    -1,   129,   130,   131,    -1,
     133,   134,   135,   136,   137,    -1,    -1,    -1,    -1,   142,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      -1,    66,    67,    68,    -1,    -1,    71,    72,    73,    74,
      -1,    -1,    77,    78,    -1,   113,   114,    -1,    -1,    84,
      -1,    -1,    -1,    -1,     1,    -1,    -1,    -1,    -1,    94,
      95,    96,    97,    98,    99,   133,   134,   135,   136,   137,
      -1,    -1,   140,    -1,    -1,    -1,    -1,    -1,   113,   114,
      -1,    -1,    -1,   118,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   129,   130,   131,    -1,   133,   134,
     135,   136,   137,    -1,    -1,    -1,    -1,   142,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    -1,    66,
      67,    68,    -1,    -1,    71,    72,    73,    74,    -1,    -1,
      77,    78,    -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,
      -1,    -1,     1,    -1,    -1,    -1,    -1,    94,    95,    96,
      97,    98,    99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   113,   114,    -1,    -1,
      -1,   118,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   129,   130,   131,    -1,   133,   134,   135,   136,
     137,    -1,    -1,    -1,    -1,   142,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    -1,    66,    67,    68,
      -1,    -1,    71,    72,    73,    74,    -1,    -1,    77,    78,
      -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    94,    95,    96,    97,    98,
      99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      65,    -1,    -1,    -1,   113,   114,    71,    -1,    -1,   118,
      -1,    -1,    77,    78,    -1,    -1,    -1,    -1,    -1,    84,
     129,   130,   131,    -1,   133,   134,   135,   136,   137,     3,
       4,     5,     6,   142,    -1,    -1,    -1,    -1,    12,    13,
      -1,    -1,    16,    17,    18,    19,    20,    21,    -1,    -1,
      -1,    -1,    -1,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    -1,   131,    -1,   133,   134,
      44,    45,    46,    47,    -1,    -1,    -1,   142,    -1,    -1,
      -1,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    -1,    66,    67,    68,    -1,    -1,    71,    72,    73,
      74,    -1,    -1,    77,    78,    -1,    -1,    -1,    -1,    -1,
      84,    85,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      94,    95,    96,    97,    98,    99,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   113,
     114,    -1,   116,    -1,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,    -1,   133,
     134,   135,   136,   137,     3,     4,     5,     6,   142,    -1,
      -1,    -1,    -1,    12,    13,    -1,    -1,    16,    17,    18,
      19,    20,    21,    -1,    -1,    -1,    -1,    -1,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      -1,    -1,    -1,    -1,    -1,    44,    45,    46,    47,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    -1,    66,    67,    68,
      -1,    -1,    71,    72,    73,    74,    -1,    -1,    77,    78,
      -1,    -1,    -1,    -1,    -1,    84,    85,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    94,    95,    96,    97,    98,
      99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   113,   114,    -1,   116,    -1,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,    -1,   133,   134,   135,   136,   137,     3,
       4,     5,     6,   142,    -1,    -1,    -1,    -1,    12,    13,
      -1,    -1,    16,    17,    18,    19,    20,    21,    -1,    -1,
      -1,    -1,    -1,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    -1,    -1,    -1,    -1,    -1,
      44,    -1,    46,    47,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    -1,    66,    67,    68,    -1,    -1,    71,    72,    73,
      74,    -1,    -1,    77,    78,    -1,    -1,    -1,    -1,    -1,
      84,    85,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      94,    95,    96,    97,    98,    99,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   113,
     114,    -1,   116,    -1,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,    -1,   133,
     134,   135,   136,   137,    48,    -1,    -1,    -1,   142,    -1,
      -1,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    -1,    66,    67,    68,    -1,    -1,    71,    72,    73,
      74,    -1,    -1,    77,    78,    -1,    -1,    -1,    -1,    -1,
      84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      94,    95,    96,    97,    98,    99,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   113,
     114,    -1,    -1,    -1,   118,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   129,   130,   131,    -1,   133,
     134,   135,   136,   137,    48,    -1,    -1,    -1,   142,    -1,
      -1,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    -1,    66,    67,    68,    -1,    -1,    71,    72,    73,
      74,    -1,    -1,    77,    78,    -1,    -1,    -1,    -1,    -1,
      84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      94,    95,    96,    97,    98,    99,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   113,
     114,    -1,    -1,    -1,   118,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   129,   130,   131,    -1,   133,
     134,   135,   136,   137,    49,    -1,    -1,    -1,   142,    -1,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      -1,    66,    67,    68,    -1,    -1,    71,    72,    73,    74,
      -1,    76,    77,    78,    -1,    -1,    -1,    -1,    -1,    84,
      -1,    -1,    -1,    88,    89,    90,    -1,    -1,    -1,    94,
      95,    96,    97,    98,    99,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   113,   114,
      -1,    -1,    -1,   118,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   129,   130,   131,    -1,   133,   134,
     135,   136,   137,    -1,    -1,    -1,    -1,   142,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    -1,    66,
      67,    68,    -1,    -1,    71,    72,    73,    74,    -1,    -1,
      77,    78,    -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,    95,    96,
      97,    98,    99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   113,   114,    -1,    -1,
      -1,   118,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   129,   130,   131,    -1,   133,   134,   135,   136,
     137,    -1,    -1,    -1,    -1,   142,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    -1,    66,    67,    68,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    -1,    66,
      67,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      -1,    66,    67,    68,    94,    95,    96,    97,    98,    99,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,    95,    96,
      97,    98,    99,   113,   114,    -1,    -1,    -1,    -1,    94,
      95,    96,    97,    98,    99,    -1,   113,   114,    -1,   129,
     130,   131,    -1,   133,   134,   135,   136,   137,   113,   114,
     140,    -1,    -1,   143,   131,    -1,   133,   134,   135,   136,
     137,    -1,    -1,   140,   129,   130,   131,    -1,   133,   134,
     135,   136,   137,    -1,    -1,   140,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    -1,    66,    67,    68,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    -1,    66,
      67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    94,    95,    96,    97,    98,    99,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,    95,    96,
      97,    98,    99,   113,   114,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   113,   114,    -1,   129,
     130,   131,    -1,   133,   134,   135,   136,   137,    -1,    -1,
     140,    -1,    -1,   130,   131,    -1,   133,   134,   135,   136,
     137,    -1,    -1,   140
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,   146,   147,     0,   148,   161,   162,   115,   149,   150,
     151,   155,   157,   158,   163,   164,   165,   277,    48,    75,
     160,   184,   256,   257,     1,   152,   150,   155,   149,   157,
       3,     4,     5,     6,    12,    13,    16,    17,    18,    19,
      20,    21,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    44,    45,    46,    47,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    66,    67,
      68,    71,    72,    73,    74,    77,    78,    84,    85,    94,
      95,    96,    97,    98,    99,   113,   114,   116,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   133,   134,   135,   136,   137,   142,   164,   166,
     167,   168,   169,   172,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   182,   183,   185,   186,   187,   188,   189,
     190,   191,   192,   193,   194,   195,   196,   197,   198,   199,
     203,   204,   209,   210,   211,   212,   248,   249,   250,   251,
     252,   253,   254,   255,   258,   259,   260,   262,   263,   267,
     268,   269,   270,   271,   272,   274,   279,   287,   289,   290,
     291,   292,   293,   294,   295,   296,   297,   298,   299,   300,
     301,   302,   303,   304,   305,   306,   307,   308,   309,   310,
     311,   340,   341,   348,   349,   162,    48,    71,    72,    78,
     153,   154,     1,    72,   142,   358,   359,   311,   170,   171,
     359,     1,   311,     1,   311,   312,    14,    15,   160,     1,
      39,    40,    71,    72,    77,    78,    84,   207,   282,   283,
     284,   353,   213,   311,   357,   160,   311,     1,    72,   360,
     360,   357,   357,   200,     1,    69,   265,   340,   350,   354,
     358,   311,     1,     1,    50,    51,    52,   357,     1,     4,
      22,    23,    24,    25,    29,    46,    79,    80,   111,   112,
     264,   266,     1,    28,   350,   357,   357,     1,   160,     1,
      25,    78,   280,   282,   283,   284,    25,    81,   288,   357,
     358,   360,   160,   357,   357,   357,   357,   357,   357,   357,
     357,   357,   357,   357,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    66,    67,    68,    94,    95,    96,    97,
      98,    99,   113,   114,   129,   130,   131,   133,   134,   135,
     136,   137,   140,   342,   343,   342,   162,    66,   160,   359,
     358,     1,   141,    48,   339,   355,   357,   205,     1,    42,
      70,    83,   102,   285,     1,   285,   206,    49,    76,    88,
      89,    90,   214,   215,   219,   245,   311,   161,     1,     1,
       1,    25,    26,    71,    72,    78,   142,   201,   220,   223,
     354,   141,   354,    65,    71,   131,   133,   134,   142,   351,
     352,   353,     1,   311,     1,    25,    53,    54,   261,   311,
     360,     1,   357,   112,   112,    46,   111,     1,   266,   265,
       1,   358,     1,    14,    45,   195,   196,   275,   276,   311,
       1,     1,    41,    43,    87,   285,   286,     1,   286,   311,
       1,     1,     1,   324,   317,   326,   328,   327,   329,   330,
     331,   332,   323,   322,   321,   333,   334,   335,   336,   337,
     338,    72,    78,   344,   345,   348,   349,   344,   319,   320,
     325,   313,   314,   315,   316,   318,   346,   347,   339,   154,
     143,   170,     1,   143,   141,   208,   355,     1,    82,   281,
       1,   208,     1,    66,   241,    91,    92,   216,   217,   218,
      10,    11,   247,   131,    66,   241,     1,    14,   202,   311,
       1,   224,   225,   226,   227,   231,   232,   233,   241,   311,
     357,   350,   142,   352,   142,   352,   142,   352,   340,   350,
     311,     1,    26,   265,     1,     1,     1,   311,     1,   196,
     276,   278,     1,   281,     1,   311,   311,   311,   311,   311,
     311,   311,   311,   311,   311,   311,   311,   311,   311,   311,
     311,   311,   311,   117,   342,   342,   311,   311,   311,   311,
     311,   311,   311,   311,   355,   358,   356,   143,     1,    72,
      78,     1,   217,   216,    93,   311,   311,   160,   311,     9,
     161,   221,   222,   221,     1,   228,   232,   233,   241,   229,
     231,   233,   241,   230,   231,   232,   241,   100,   101,   102,
     143,   221,   340,   340,   340,   143,   161,     1,   161,    45,
     273,     1,   142,   144,   355,   311,   241,   241,   156,   159,
     163,   164,   241,   311,     1,   157,   223,   233,   241,   232,
     241,   233,   241,   231,   241,   232,   241,   231,   241,     1,
     241,     1,   241,     1,   241,   241,   143,   143,   143,    14,
     159,   360,   342,   241,   163,   164,   211,   164,     7,     8,
       9,   246,   241,    15,   241,   241,   241,   241,   241,   241,
     103,   106,   107,   108,   109,   110,   234,   236,   103,   104,
     105,   235,   236,   235,   221,   161,     1,     9,   311,   311,
     311,   246,   161,     1,   239,   240,     1,   237,   238,     1,
     239,     1,   239,     1,   236,     1,   236,     1,   236,   163,
     311,   241,   241,   241,   246,     1,   157,   241,   241,    14,
     241,   242,   243,   244,   353,   359,   243
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   145,   147,   146,   148,   148,   148,   148,   149,   149,
     150,   150,   152,   151,   151,   153,   153,   154,   154,   154,
     155,   155,   156,   156,   157,   157,   158,   159,   159,   160,
     160,   161,   161,   162,   162,   163,   163,   164,   165,   166,
     166,   166,   166,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     168,   168,   169,   169,   170,   170,   171,   171,   172,   173,
     174,   175,   176,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   188,   189,   190,   191,   192,   193,
     194,   195,   196,   197,   198,   200,   199,   201,   201,   201,
     202,   201,   201,   201,   203,   205,   204,   206,   204,   204,
     204,   204,   204,   204,   204,   204,   204,   207,   207,   208,
     208,   209,   210,   211,   211,   211,   212,   213,   213,   213,
     214,   215,   216,   217,   218,   218,   218,   218,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   220,
     220,   220,   222,   221,   221,   223,   223,   223,   224,   224,
     224,   224,   225,   226,   227,   228,   228,   228,   228,   228,
     229,   229,   229,   229,   229,   230,   230,   230,   230,   230,
     231,   231,   232,   232,   233,   233,   234,   234,   234,   234,
     235,   235,   235,   235,   235,   235,   236,   236,   236,   236,
     236,   236,   236,   236,   238,   237,   240,   239,   241,   241,
     242,   242,   243,   244,   245,   246,   246,   246,   247,   247,
     247,   248,   248,   249,   249,   250,   251,   251,   251,   251,
     251,   251,   252,   253,   254,   255,   255,   255,   256,   257,
     258,   258,   258,   259,   259,   260,   260,   260,   260,   260,
     260,   260,   260,   260,   260,   261,   261,   262,   263,   263,
     263,   263,   264,   264,   264,   264,   264,   264,   264,   265,
     265,   266,   266,   266,   266,   266,   266,   266,   266,   266,
     267,   267,   267,   267,   267,   268,   269,   270,   271,   272,
     273,   273,   273,   274,   274,   274,   274,   274,   274,   275,
     275,   275,   276,   276,   276,   276,   277,   277,   278,   278,
     279,   279,   279,   279,   279,   279,   279,   279,   279,   279,
     279,   280,   280,   280,   281,   281,   281,   282,   282,   283,
     284,   285,   285,   285,   285,   286,   286,   286,   286,   287,
     287,   287,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   312,   311,   313,   311,
     314,   311,   315,   311,   311,   316,   311,   311,   317,   311,
     311,   318,   311,   311,   319,   311,   311,   320,   311,   311,
     321,   311,   311,   322,   311,   311,   323,   311,   311,   324,
     311,   311,   311,   311,   325,   311,   311,   326,   311,   311,
     327,   311,   311,   328,   311,   311,   329,   311,   311,   330,
     311,   311,   331,   311,   311,   332,   311,   311,   333,   311,
     311,   334,   311,   311,   335,   311,   311,   336,   311,   311,
     337,   311,   311,   338,   311,   311,   311,   311,   311,   311,
     311,   311,   311,   311,   311,   311,   311,   311,   311,   311,
     311,   311,   339,   339,   339,   340,   341,   341,   343,   342,
     344,   344,   344,   344,   344,   344,   345,   347,   346,   348,
     349,   350,   350,   350,   351,   351,   351,   351,   351,   351,
     351,   351,   351,   352,   352,   353,   353,   353,   354,   354,
     354,   356,   355,   355,   357,   357,   358,   358,   358,   358,
     359,   360,   360
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     2,     1,     3,     2,     2,     1,
       2,     1,     0,     4,     2,     3,     1,     1,     1,     1,
       2,     1,     2,     2,     1,     1,     3,     1,     1,     3,
       2,     2,     1,     2,     0,     1,     1,     0,     2,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     2,     2,     3,     3,     1,     1,     0,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     0,     3,     3,     1,     0,
       0,     3,     3,     5,     2,     0,     4,     0,     4,     3,
       3,     4,     5,     4,     3,     4,     3,     1,     1,     1,
       2,     1,     1,     2,     1,     3,     7,     0,     2,     2,
       1,     1,     1,     1,     1,     1,     2,     2,     7,     6,
       5,     4,     2,     2,     2,     5,     5,     8,     0,     1,
       1,     1,     0,     2,     0,     2,     3,     2,     1,     1,
       1,     1,     2,     2,     2,     2,     3,     2,     3,     1,
       2,     3,     2,     3,     1,     2,     3,     2,     3,     1,
       4,     3,     4,     3,     4,     3,     1,     2,     2,     1,
       1,     2,     2,     2,     2,     1,     2,     2,     2,     2,
       2,     2,     2,     2,     0,     3,     0,     3,     1,     0,
       1,     1,     1,     1,     1,     3,     3,     3,     3,     3,
       0,     3,     2,     3,     2,     2,     6,     9,     9,     6,
       2,     4,     2,     2,     2,     2,     3,     1,     1,     1,
       2,     3,     1,     1,     2,     3,     2,     4,     3,     2,
       4,     3,     2,     3,     2,     1,     1,     2,     4,     3,
       3,     2,     1,     2,     2,     1,     2,     2,     1,     3,
       1,     1,     1,     1,     1,     1,     1,     2,     3,     2,
       1,     2,     3,     4,     3,     2,     2,     2,     2,     2,
       2,     3,     1,     5,     4,     4,     2,     3,     5,     2,
       1,     1,     6,     7,     2,     2,     2,     2,     3,     0,
       3,     3,     2,     3,     3,     4,     5,     4,     3,     4,
       3,     1,     1,     1,     2,     2,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       2,     3,     2,     1,     2,     1,     2,     1,     2,     1,
       2,     1,     2,     1,     2,     1,     2,     1,     2,     1,
       2,     1,     2,     1,     2,     1,     0,     3,     0,     4,
       0,     4,     0,     4,     1,     0,     4,     1,     0,     4,
       1,     0,     4,     1,     0,     4,     1,     0,     4,     1,
       0,     4,     1,     0,     4,     1,     0,     4,     1,     0,
       4,     1,     2,     1,     0,     4,     1,     0,     4,     1,
       0,     4,     1,     0,     4,     1,     0,     4,     1,     0,
       4,     1,     0,     4,     1,     0,     4,     1,     0,     4,
       1,     0,     4,     1,     0,     4,     1,     0,     4,     1,
       0,     4,     1,     0,     4,     1,     3,     3,     1,     1,
       3,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       2,     2,     2,     2,     1,     1,     2,     2,     0,     2,
       1,     2,     4,     1,     2,     2,     1,     0,     3,     1,
       1,     3,     1,     1,     2,     2,     1,     2,     3,     4,
       4,     4,     1,     1,     1,     1,     1,     1,     2,     2,
       0,     0,     4,     1,     1,     0,     2,     1,     4,     3,
       1,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* $@1: %empty  */
#line 175 "yaccsrc.y"
                                       { level = 0;
                                         if ( get_options_flag( parser_data.TSD->currlevel, EXT_CALLS_AS_FUNCS )
                                           && !get_options_flag( parser_data.TSD->currlevel, EXT_STRICT_ANSI ) )
                                            start_parendepth = 1;
                                         else
                                            start_parendepth = 0;
                                         parendepth = 0; }
#line 2549 "yaccsrc.c"
    break;

  case 4: /* prog: nlncl stats  */
#line 185 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-1], yyvsp[0] );
                                         yyval->o.last = NULL;
                                         EndProg( yyval ) ; }
#line 2557 "yaccsrc.c"
    break;

  case 5: /* prog: nlncl  */
#line 188 "yaccsrc.y"
                                       { yyval = yyvsp[0];
                                         if ( yyval != NULL )
                                            yyval->o.last = NULL;
                                         EndProg( yyval ); }
#line 2566 "yaccsrc.c"
    break;

  case 6: /* prog: nlncl stats dirsect  */
#line 192 "yaccsrc.y"
                                       { nodeptr more = split_requires( &yyvsp[0] );
                                         yyval = optgluelast( makenode( X_OO_INIT, 0 ), yyvsp[-2] );
                                         yyval = optgluelast( yyval, yyvsp[-1] );
                                         yyval = optgluelast( yyval, more );
                                         yyval = optgluelast( yyval, yyvsp[0] );
                                         yyval->o.last = NULL;
                                         EndProg( yyval ) ; }
#line 2578 "yaccsrc.c"
    break;

  case 7: /* prog: nlncl dirsect  */
#line 199 "yaccsrc.y"
                                       { nodeptr more = split_requires( &yyvsp[0] );
                                         yyval = optgluelast( makenode( X_OO_INIT, 0 ), yyvsp[-1] );
                                         yyval = optgluelast( yyval, more );
                                         yyval = optgluelast( yyval, yyvsp[0] );
                                         yyval->o.last = NULL;
                                         EndProg( yyval ) ; }
#line 2589 "yaccsrc.c"
    break;

  case 8: /* dirsect: dirsect dirblock  */
#line 212 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 2595 "yaccsrc.c"
    break;

  case 9: /* dirsect: dirblock  */
#line 213 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2601 "yaccsrc.c"
    break;

  case 10: /* dirblock: directive stats  */
#line 216 "yaccsrc.y"
                                       { yyval = yyvsp[-1];
                                         yyval->p[1] = optgluelast( yyval->p[1], yyvsp[0] );
                                         if ( yyval->p[1] != NULL )
                                            yyval->p[1]->o.last = NULL; }
#line 2610 "yaccsrc.c"
    break;

  case 11: /* dirblock: directive  */
#line 220 "yaccsrc.y"
                                       { yyval = yyvsp[0];
                                         if ( yyval->p[1] != NULL )
                                            yyval->p[1]->o.last = NULL; }
#line 2618 "yaccsrc.c"
    break;

  case 12: /* @2: %empty  */
#line 225 "yaccsrc.y"
                                       { yyval = makenode( X_DIRECTIVE, 0 );
                                         yyval->lineno = parser_data.tline;
                                         yyval->charnr = parser_data.tstart; }
#line 2626 "yaccsrc.c"
    break;

  case 13: /* directive: DCOLON @2 dirwords ncl  */
#line 228 "yaccsrc.y"
                                       { yyval = yyvsp[-2];
                                         /* take over the keyword; the
                                          * node must not own it twice */
                                         yyval->name = yyvsp[-1]->name;
                                         yyvsp[-1]->name = NULL;
                                         yyval->p[0] = yyvsp[-1]->next;
                                         if ( yyval->p[0] != NULL )
                                            yyval->p[0]->o.last = NULL;
                                         yyval->p[1] = yyvsp[0]; /* labels */ }
#line 2640 "yaccsrc.c"
    break;

  case 14: /* directive: DCOLON error  */
#line 237 "yaccsrc.y"
                                       { exiterror( ERR_YACC_SYNTAX, 1, parser_data.tline ); }
#line 2646 "yaccsrc.c"
    break;

  case 15: /* dirwords: dirwords SPACE dirword  */
#line 240 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-2], yyvsp[0] ); }
#line 2652 "yaccsrc.c"
    break;

  case 16: /* dirwords: dirword  */
#line 241 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2658 "yaccsrc.c"
    break;

  case 17: /* dirword: SIMSYMBOL  */
#line 244 "yaccsrc.y"
                                       { yyval = makenode( X_STRING, 0 );
                                         yyval->name = Str_cre_TSD( parser_data.TSD, retvalue ); }
#line 2665 "yaccsrc.c"
    break;

  case 18: /* dirword: CONSYMBOL  */
#line 246 "yaccsrc.y"
                                       { yyval = makenode( X_STRING, 0 );
                                         yyval->name = Str_cre_TSD( parser_data.TSD, retvalue ); }
#line 2672 "yaccsrc.c"
    break;

  case 19: /* dirword: STRING  */
#line 248 "yaccsrc.y"
                                       { yyval = makenode( X_STRING, 0 );
                                         yyval->name = Str_cre_TSD( parser_data.TSD, retvalue ); }
#line 2679 "yaccsrc.c"
    break;

  case 20: /* stats: stats ystatement  */
#line 252 "yaccsrc.y"
                                       { /* fixes bug 579711 */
                                         yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 2686 "yaccsrc.c"
    break;

  case 21: /* stats: ystatement  */
#line 254 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2692 "yaccsrc.c"
    break;

  case 22: /* xstats: xstats statement  */
#line 257 "yaccsrc.y"
                                       { /* fixes bug 579711 */
                                         yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 2699 "yaccsrc.c"
    break;

  case 23: /* xstats: statement gruff  */
#line 259 "yaccsrc.y"
                                       { yyval = yyvsp[-1]; }
#line 2705 "yaccsrc.c"
    break;

  case 24: /* ystatement: statement  */
#line 262 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 2711 "yaccsrc.c"
    break;

  case 25: /* ystatement: lonely_end  */
#line 263 "yaccsrc.y"
                                       { exiterror( ERR_UNMATCHED_END, 1 ); }
#line 2717 "yaccsrc.c"
    break;

  case 27: /* nxstats: xstats  */
#line 271 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2723 "yaccsrc.c"
    break;

  case 28: /* nxstats: gruff  */
#line 272 "yaccsrc.y"
                                       { yyval = NULL; }
#line 2729 "yaccsrc.c"
    break;

  case 29: /* ncl: ncl STATSEP optLabels  */
#line 275 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-2], yyvsp[0] ); }
#line 2735 "yaccsrc.c"
    break;

  case 30: /* ncl: STATSEP optLabels  */
#line 276 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2741 "yaccsrc.c"
    break;

  case 31: /* nlncl: optLabels ncl  */
#line 279 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 2747 "yaccsrc.c"
    break;

  case 32: /* nlncl: optLabels  */
#line 280 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2753 "yaccsrc.c"
    break;

  case 33: /* optLabels: optLabels label_stat  */
#line 283 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 2759 "yaccsrc.c"
    break;

  case 34: /* optLabels: %empty  */
#line 284 "yaccsrc.y"
                                       { yyval = NULL; }
#line 2765 "yaccsrc.c"
    break;

  case 37: /* gruff: %empty  */
#line 291 "yaccsrc.y"
                                       { tmpchr=parser_data.tstart;
                                         tmplno=parser_data.tline; }
#line 2772 "yaccsrc.c"
    break;

  case 38: /* mttstatement: gruff mtstatement  */
#line 295 "yaccsrc.y"
                                       { yyval=yyvsp[0]; }
#line 2778 "yaccsrc.c"
    break;

  case 39: /* mtstatement: nclstatement ncl  */
#line 298 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 2784 "yaccsrc.c"
    break;

  case 80: /* oo_expose_stat: EXPOSE anyvars  */
#line 347 "yaccsrc.y"
                                       { yyval = makenode( X_OO_EXPOSE, 1, yyvsp[0] );
                                         yyval->lineno = tmplno;
                                         yyval->charnr = tmpchr; }
#line 2792 "yaccsrc.c"
    break;

  case 81: /* oo_expose_stat: EXPOSE error  */
#line 350 "yaccsrc.y"
                                       { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ); }
#line 2798 "yaccsrc.c"
    break;

  case 82: /* oo_usearg_stat: USEARG usevars  */
#line 353 "yaccsrc.y"
                                       { yyval = makenode( X_OO_USEARG, 1, yyvsp[0] );
                                         yyval->lineno = tmplno;
                                         yyval->charnr = tmpchr; }
#line 2806 "yaccsrc.c"
    break;

  case 83: /* oo_usearg_stat: USEARG usevars error  */
#line 356 "yaccsrc.y"
                                       { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ); }
#line 2812 "yaccsrc.c"
    break;

  case 84: /* usevars: usevar ',' usevars  */
#line 359 "yaccsrc.y"
                                       { yyval = yyvsp[-2];
                                         yyval->p[0] = yyvsp[0]; }
#line 2819 "yaccsrc.c"
    break;

  case 85: /* usevars: usevar  */
#line 361 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2825 "yaccsrc.c"
    break;

  case 86: /* usevar: xsimsymb  */
#line 364 "yaccsrc.y"
                                       { yyval = makenode( X_SIM_SYMBOL, 0 );
                                         yyval->name = (streng *) yyvsp[0]; }
#line 2832 "yaccsrc.c"
    break;

  case 87: /* usevar: %empty  */
#line 366 "yaccsrc.y"
                                       { yyval = makenode( X_SIM_SYMBOL, 0 );
                                         yyval->name = NULL; }
#line 2839 "yaccsrc.c"
    break;

  case 88: /* call: CALL  */
#line 370 "yaccsrc.y"
                                       { yyval = makenode(X_CALL,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2847 "yaccsrc.c"
    break;

  case 89: /* do: DO  */
#line 374 "yaccsrc.y"
                                       { yyval = makenode(X_DO,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ;
                                         /* OO: LOOP, see finish_do() */
                                         yyval->u.nonansi = __regina_do_is_loop ;
                                         level++; }
#line 2858 "yaccsrc.c"
    break;

  case 90: /* exit: EXIT  */
#line 381 "yaccsrc.y"
                                       { yyval = makenode(X_EXIT,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2866 "yaccsrc.c"
    break;

  case 91: /* if: IF  */
#line 385 "yaccsrc.y"
                                       { yyval = makenode(X_IF,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ;
                                         level++; }
#line 2875 "yaccsrc.c"
    break;

  case 92: /* iterate: ITERATE  */
#line 390 "yaccsrc.y"
                                       { yyval = makenode(X_ITERATE,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2883 "yaccsrc.c"
    break;

  case 93: /* leave: LEAVE  */
#line 394 "yaccsrc.y"
                                       { yyval = makenode(X_LEAVE,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2891 "yaccsrc.c"
    break;

  case 94: /* say: SAY  */
#line 398 "yaccsrc.y"
                                       { yyval = makenode(X_SAY,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2899 "yaccsrc.c"
    break;

  case 95: /* return: RETURN  */
#line 402 "yaccsrc.y"
                                       { yyval = makenode(X_RETURN,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2907 "yaccsrc.c"
    break;

  case 96: /* address: ADDRESS  */
#line 406 "yaccsrc.y"
                                       { yyval = makenode(X_ADDR_N,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2915 "yaccsrc.c"
    break;

  case 97: /* arg: ARG  */
#line 410 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_ARG,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2923 "yaccsrc.c"
    break;

  case 98: /* drop: DROP  */
#line 414 "yaccsrc.y"
                                       { yyval = makenode(X_DROP,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2931 "yaccsrc.c"
    break;

  case 99: /* interpret: INTERPRET  */
#line 418 "yaccsrc.y"
                                       { yyval = makenode(X_IPRET,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2939 "yaccsrc.c"
    break;

  case 100: /* label: LABEL  */
#line 422 "yaccsrc.y"
                                       { yyval = makenode(X_LABEL,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2947 "yaccsrc.c"
    break;

  case 101: /* nop: NOP  */
#line 426 "yaccsrc.y"
                                       { yyval = makenode(X_NULL,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2955 "yaccsrc.c"
    break;

  case 102: /* numeric: NUMERIC  */
#line 430 "yaccsrc.y"
                                       { yyval = makenode(0,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2963 "yaccsrc.c"
    break;

  case 103: /* options: OPTIONS  */
#line 434 "yaccsrc.y"
                                       { yyval = makenode(X_OPTIONS,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2971 "yaccsrc.c"
    break;

  case 104: /* parse: PARSE  */
#line 438 "yaccsrc.y"
                                       { yyval = makenode(0,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2979 "yaccsrc.c"
    break;

  case 105: /* proc: PROCEDURE  */
#line 442 "yaccsrc.y"
                                       { yyval = makenode(X_PROC,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2987 "yaccsrc.c"
    break;

  case 106: /* pull: PULL  */
#line 446 "yaccsrc.y"
                                       { yyval = makenode(X_PULL,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2995 "yaccsrc.c"
    break;

  case 107: /* push: PUSH  */
#line 450 "yaccsrc.y"
                                       { yyval = makenode(X_PUSH,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3003 "yaccsrc.c"
    break;

  case 108: /* queue: QUEUE  */
#line 454 "yaccsrc.y"
                                       { yyval = makenode(X_QUEUE,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3011 "yaccsrc.c"
    break;

  case 109: /* select: SELECT  */
#line 458 "yaccsrc.y"
                                       { yyval = makenode(X_SELECT,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ;
                                         level++; }
#line 3020 "yaccsrc.c"
    break;

  case 110: /* signal: SIGNAL  */
#line 463 "yaccsrc.y"
                                       { yyval = makenode(X_SIG_LAB,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3028 "yaccsrc.c"
    break;

  case 111: /* when: WHEN  */
#line 467 "yaccsrc.y"
                                       { yyval = makenode(X_WHEN,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3036 "yaccsrc.c"
    break;

  case 112: /* otherwise: OTHERWISE  */
#line 471 "yaccsrc.y"
                                       { yyval = makenode(X_OTHERWISE,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3044 "yaccsrc.c"
    break;

  case 113: /* trace: TRACE  */
#line 475 "yaccsrc.y"
                                       { yyval = makenode(X_TRACE,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3052 "yaccsrc.c"
    break;

  case 114: /* upper: UPPER  */
#line 479 "yaccsrc.y"
                                       { yyval = makenode(X_UPPER_VAR,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3060 "yaccsrc.c"
    break;

  case 115: /* @3: %empty  */
#line 483 "yaccsrc.y"
                                       { yyval = current = yyvsp[0] ; }
#line 3066 "yaccsrc.c"
    break;

  case 117: /* address_stat2: VALUE expr naddr_with  */
#line 487 "yaccsrc.y"
                                       { current->type = X_ADDR_V ;
                                         current->p[0] = yyvsp[-1] ;
                                         current->p[1] = yyvsp[0] ; }
#line 3074 "yaccsrc.c"
    break;

  case 118: /* address_stat2: addr_with  */
#line 490 "yaccsrc.y"
                                       { exiterror( ERR_STRING_EXPECTED, 1, __reginatext ) ;}
#line 3080 "yaccsrc.c"
    break;

  case 119: /* address_stat2: %empty  */
#line 491 "yaccsrc.y"
                                       { current->type = X_ADDR_S ; }
#line 3086 "yaccsrc.c"
    break;

  case 120: /* $@4: %empty  */
#line 492 "yaccsrc.y"
                                       { exiterror( ERR_STRING_EXPECTED, 1, __reginatext ) ;}
#line 3092 "yaccsrc.c"
    break;

  case 122: /* address_stat2: nvir nexpr naddr_with  */
#line 494 "yaccsrc.y"
                                       { current->name = (streng *)yyvsp[-2] ;
                                         current->type = X_ADDR_N ;
                                         current->p[0] = yyvsp[-1] ;
                                         current->p[1] = yyvsp[0] ; }
#line 3101 "yaccsrc.c"
    break;

  case 123: /* address_stat2: '(' expr ')' nspace naddr_with  */
#line 498 "yaccsrc.y"
                                              { current->type = X_ADDR_V ;
                                         current->p[0] = yyvsp[-3] ;
                                         current->p[1] = yyvsp[0] ;
                                         current->u.nonansi = 1 ; }
#line 3110 "yaccsrc.c"
    break;

  case 124: /* arg_stat: arg templs  */
#line 504 "yaccsrc.y"
                                       { yyval = makenode( X_PARSE, 2, yyvsp[-1], yyvsp[0] );
                                         yyval->u.parseflags = PARSE_UPPER;
                                         yyval->lineno = yyvsp[-1]->lineno;
                                         yyval->charnr = yyvsp[-1]->charnr; }
#line 3119 "yaccsrc.c"
    break;

  case 125: /* $@5: %empty  */
#line 510 "yaccsrc.y"
                                       { parendepth = start_parendepth; }
#line 3125 "yaccsrc.c"
    break;

  case 126: /* call_stat: call call_name $@5 call_args  */
#line 511 "yaccsrc.y"
                                       { yyval = yyvsp[-3];
                                         yyval->p[0] = yyvsp[0];
                                         yyval->name = (streng *) yyvsp[-2];
                                         parendepth = 0; }
#line 3134 "yaccsrc.c"
    break;

  case 127: /* $@6: %empty  */
#line 515 "yaccsrc.y"
                                       { parendepth = start_parendepth; }
#line 3140 "yaccsrc.c"
    break;

  case 128: /* call_stat: call string $@6 call_args  */
#line 516 "yaccsrc.y"
                                       { yyval = yyvsp[-3];
                                         yyval->type = X_EX_FUNC;
                                         yyval->p[0] = yyvsp[0];
                                         yyval->name = (streng *) yyvsp[-2];
                                         parendepth = 0; }
#line 3150 "yaccsrc.c"
    break;

  case 129: /* call_stat: call on error  */
#line 521 "yaccsrc.y"
                             { exiterror( ERR_INV_SUBKEYWORD, 1, "ERROR FAILURE HALT NOTREADY", __reginatext ) ;}
#line 3156 "yaccsrc.c"
    break;

  case 130: /* call_stat: call off error  */
#line 522 "yaccsrc.y"
                              { exiterror( ERR_INV_SUBKEYWORD, 2, "ERROR FAILURE HALT NOTREADY", __reginatext ) ;}
#line 3162 "yaccsrc.c"
    break;

  case 131: /* call_stat: call on c_action error  */
#line 523 "yaccsrc.y"
                                      { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 3168 "yaccsrc.c"
    break;

  case 132: /* call_stat: call on c_action namespec error  */
#line 524 "yaccsrc.y"
                                               { exiterror( ERR_STRING_EXPECTED, 3, __reginatext ) ;}
#line 3174 "yaccsrc.c"
    break;

  case 133: /* call_stat: call on c_action namespec  */
#line 526 "yaccsrc.y"
                                       { yyval = yyvsp[-3] ;
                                         yyval->type = X_CALL_SET ;
                                         yyval->p[0] = yyvsp[-2] ;
                                         yyval->name = (streng *)yyvsp[0] ;
                                         yyval->p[1] = yyvsp[-1] ; }
#line 3184 "yaccsrc.c"
    break;

  case 134: /* call_stat: call on c_action  */
#line 531 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_CALL_SET ;
                                         yyval->p[0] = yyvsp[-1] ;
                                         yyval->name = NULL ;
                                         yyval->p[1] = yyvsp[0] ; }
#line 3194 "yaccsrc.c"
    break;

  case 135: /* call_stat: call off c_action error  */
#line 536 "yaccsrc.y"
                                       { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 3200 "yaccsrc.c"
    break;

  case 136: /* call_stat: call off c_action  */
#line 537 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_CALL_SET ;
                                         yyval->p[0] = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ; }
#line 3209 "yaccsrc.c"
    break;

  case 137: /* call_name: asymbol  */
#line 543 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 3215 "yaccsrc.c"
    break;

  case 138: /* call_name: error  */
#line 544 "yaccsrc.y"
                                       { exiterror( ERR_STRING_EXPECTED, 2, __reginatext );}
#line 3221 "yaccsrc.c"
    break;

  case 139: /* call_args: exprs  */
#line 547 "yaccsrc.y"
                                       {
                                         /*
                                          * "call_args" accepted probably with
                                          * surrounding parentheses. Strip them.
                                          */
                                         yyval = reduce_expr_list( yyvsp[0],
                                                                REDUCE_CALL );
                                       }
#line 3234 "yaccsrc.c"
    break;

  case 140: /* call_args: exprs ')'  */
#line 555 "yaccsrc.y"
                                       { exiterror(ERR_UNEXPECTED_PARAN, 2); }
#line 3240 "yaccsrc.c"
    break;

  case 141: /* expr_stat: expr  */
#line 558 "yaccsrc.y"
                                       { nodeptr t = yyvsp[0]->p[0];
                                         if ( yyvsp[0]->type == X_MSGSEND
                                           || yyvsp[0]->type == X_CASCADE )
                                            /* OO: message instruction */
                                            yyval = makenode( X_OO_MSGSTAT, 1, yyvsp[0] );
                                         else if ( ( yyvsp[0]->type == X_EQUAL
                                                  || yyvsp[0]->type == X_SEQUAL
                                                  || yyvsp[0]->type == X_NEQUAL )
                                                && t != NULL
                                                && ( t->type == X_MSGSEND
                                                  || t->type == X_INDEX ) )
                                            /* OO: obj~attr = v, a[i] = v */
                                            yyval = makenode( X_OO_ASSIGN, 2, t, yyvsp[0]->p[1] );
                                         else
                                         {
                                            yyval = makenode(X_COMMAND,0) ;
                                            yyval->p[0] = yyvsp[0] ;
                                         }
                                         yyval->charnr = tmpchr ;
                                         yyval->lineno = tmplno; }
#line 3265 "yaccsrc.c"
    break;

  case 142: /* end_stat: END  */
#line 580 "yaccsrc.y"
                                       { yyval = makenode(X_END,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ;
                                         level--; }
#line 3274 "yaccsrc.c"
    break;

  case 143: /* end: end_stat simsymb  */
#line 586 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->name = (streng*)(yyvsp[0]) ; }
#line 3281 "yaccsrc.c"
    break;

  case 144: /* end: end_stat  */
#line 588 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3287 "yaccsrc.c"
    break;

  case 145: /* end: end_stat simsymb error  */
#line 589 "yaccsrc.y"
                                       {  exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 3293 "yaccsrc.c"
    break;

  case 146: /* do_stat: do do_opts repetitor conditional ncl nxstats end  */
#line 593 "yaccsrc.y"
                                       { yyval = yyvsp[-6];
                                         yyval->p[0] = yyvsp[-4];
                                         yyval->p[1] = yyvsp[-3];
                                         yyval->p[2] = optgluelast( yyvsp[-2], yyvsp[-1] );
                                         if ( yyval->p[2] )
                                            yyval->p[2]->o.last = NULL;
                                         yyval->p[3] = yyvsp[0];
                                         finish_do( yyval, yyvsp[-5] ); }
#line 3306 "yaccsrc.c"
    break;

  case 147: /* do_opts: %empty  */
#line 604 "yaccsrc.y"
                                       { yyval = NULL ; }
#line 3312 "yaccsrc.c"
    break;

  case 148: /* do_opts: do_opts dolabel  */
#line 605 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ? yyvsp[-1] : makenode(X_OO_DOOPTS,0) ;
                                         if ( yyval->name )
                                            exiterror( ERR_INVALID_DO_SYNTAX, 1, "LABEL" ) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 3321 "yaccsrc.c"
    break;

  case 149: /* do_opts: do_opts docounter  */
#line 609 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ? yyvsp[-1] : makenode(X_OO_DOOPTS,0) ;
                                         if ( yyval->p[0] )
                                            exiterror( ERR_INVALID_DO_SYNTAX, 1, "COUNTER" ) ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3330 "yaccsrc.c"
    break;

  case 150: /* dolabel: DOLABEL  */
#line 615 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 3336 "yaccsrc.c"
    break;

  case 151: /* docounter: DOCOUNTER  */
#line 617 "yaccsrc.y"
                                       { yyval = create_head( retvalue ) ; }
#line 3342 "yaccsrc.c"
    break;

  case 152: /* doindex: DOINDEX  */
#line 619 "yaccsrc.y"
                                       { yyval = create_head( retvalue ) ; }
#line 3348 "yaccsrc.c"
    break;

  case 153: /* doitem: DOITEM  */
#line 621 "yaccsrc.y"
                                       { yyval = create_head( retvalue ) ; }
#line 3354 "yaccsrc.c"
    break;

  case 154: /* withvars: doindex  */
#line 625 "yaccsrc.y"
                                       { yyval = makenode(X_OO_DOOPTS,2,yyvsp[0],NULL) ; }
#line 3360 "yaccsrc.c"
    break;

  case 155: /* withvars: doitem  */
#line 626 "yaccsrc.y"
                                       { yyval = makenode(X_OO_DOOPTS,2,NULL,yyvsp[0]) ; }
#line 3366 "yaccsrc.c"
    break;

  case 156: /* withvars: doindex doitem  */
#line 627 "yaccsrc.y"
                                       { yyval = makenode(X_OO_DOOPTS,2,yyvsp[-1],yyvsp[0]) ; }
#line 3372 "yaccsrc.c"
    break;

  case 157: /* withvars: doitem doindex  */
#line 628 "yaccsrc.y"
                                       { yyval = makenode(X_OO_DOOPTS,2,yyvsp[0],yyvsp[-1]) ; }
#line 3378 "yaccsrc.c"
    break;

  case 158: /* repetitor: dovar '=' expr nspace tobyfor tobyfor tobyfor  */
#line 632 "yaccsrc.y"
                                       { yyval =makenode(X_REP,4,yyvsp[-4],yyvsp[-2],yyvsp[-1],yyvsp[0]) ;
                                         yyval->name = (streng *)yyvsp[-6] ;
                                         checkdosyntax(yyval) ; }
#line 3386 "yaccsrc.c"
    break;

  case 159: /* repetitor: dovar '=' expr nspace tobyfor tobyfor  */
#line 636 "yaccsrc.y"
                                       { yyval =makenode(X_REP,3,yyvsp[-3],yyvsp[-1],yyvsp[0]) ;
                                         yyval->name = (streng *)yyvsp[-5] ;
                                         checkdosyntax(yyval) ; }
#line 3394 "yaccsrc.c"
    break;

  case 160: /* repetitor: dovar '=' expr nspace tobyfor  */
#line 640 "yaccsrc.y"
                                       { yyval = makenode(X_REP,2,yyvsp[-2],yyvsp[0]) ;
                                         yyval->name = (streng *)yyvsp[-4] ;
                                         checkdosyntax(yyval) ; }
#line 3402 "yaccsrc.c"
    break;

  case 161: /* repetitor: dovar '=' expr nspace  */
#line 643 "yaccsrc.y"
                                       { yyval = makenode(X_REP,1,yyvsp[-1]) ;
                                         yyval->name = (streng *)yyvsp[-3] ;
                                         checkdosyntax(yyval) ; }
#line 3410 "yaccsrc.c"
    break;

  case 162: /* repetitor: FOREVER nspace  */
#line 646 "yaccsrc.y"
                                       { yyval = makenode(X_REP_FOREVER,0) ; }
#line 3416 "yaccsrc.c"
    break;

  case 163: /* repetitor: FOREVER error  */
#line 647 "yaccsrc.y"
                             { exiterror( ERR_INV_SUBKEYWORD, 16, "WHILE UNTIL", __reginatext ) ; }
#line 3422 "yaccsrc.c"
    break;

  case 164: /* repetitor: expr nspace  */
#line 648 "yaccsrc.y"
                                       { yyvsp[-1] = makenode(X_DO_EXPR,1,yyvsp[-1]) ;
                                         yyval = makenode(X_REP,2,NULL,yyvsp[-1]) ; }
#line 3429 "yaccsrc.c"
    break;

  case 165: /* repetitor: expr nspace FOR expr nspace  */
#line 652 "yaccsrc.y"
                                       { yyvsp[-4] = makenode(X_DO_EXPR,1,yyvsp[-4]) ;
                                         yyval = makenode(X_REP,3,NULL,yyvsp[-4],
                                                       makenode(X_DO_FOR,1,yyvsp[-1])) ;
                                         if ( do_over_var( yyval ) == NULL )
                                            exiterror( ERR_INVALID_DO_SYNTAX, 1, "FOR" ) ; }
#line 3439 "yaccsrc.c"
    break;

  case 166: /* repetitor: DOWITH withvars DOOVER expr nspace  */
#line 658 "yaccsrc.y"
                                       { yyval = makenode(X_OO_DOWITH,4,yyvsp[-1],yyvsp[-3]->p[0],yyvsp[-3]->p[1],NULL) ; }
#line 3445 "yaccsrc.c"
    break;

  case 167: /* repetitor: DOWITH withvars DOOVER expr nspace FOR expr nspace  */
#line 660 "yaccsrc.y"
                                       { yyval = makenode(X_OO_DOWITH,4,yyvsp[-4],yyvsp[-6]->p[0],yyvsp[-6]->p[1],
                                                       makenode(X_DO_FOR,1,yyvsp[-1])) ; }
#line 3452 "yaccsrc.c"
    break;

  case 168: /* repetitor: %empty  */
#line 662 "yaccsrc.y"
                                       { yyval = NULL ; }
#line 3458 "yaccsrc.c"
    break;

  case 169: /* nvir: CONSYMBOL  */
#line 665 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 3464 "yaccsrc.c"
    break;

  case 170: /* nvir: SIMSYMBOL  */
#line 666 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 3470 "yaccsrc.c"
    break;

  case 171: /* nvir: STRING  */
#line 667 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 3476 "yaccsrc.c"
    break;

  case 172: /* @7: %empty  */
#line 670 "yaccsrc.y"
                                       { SymbolDetect |= SD_ADDRWITH ;
                                         yyval = with = makenode(X_ADDR_WITH,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3485 "yaccsrc.c"
    break;

  case 173: /* naddr_with: @7 addr_with  */
#line 674 "yaccsrc.y"
                                       { with = NULL ;
                                         SymbolDetect &= ~SD_ADDRWITH ; }
#line 3492 "yaccsrc.c"
    break;

  case 174: /* naddr_with: %empty  */
#line 676 "yaccsrc.y"
                                       { yyval = NULL ; }
#line 3498 "yaccsrc.c"
    break;

  case 175: /* addr_with: WITH connection  */
#line 679 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 3504 "yaccsrc.c"
    break;

  case 176: /* addr_with: WITH connection error  */
#line 680 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 5, __reginatext ) ; }
#line 3510 "yaccsrc.c"
    break;

  case 177: /* addr_with: WITH nspace  */
#line 681 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 5, __reginatext ) ; }
#line 3516 "yaccsrc.c"
    break;

  case 181: /* connection: error  */
#line 687 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 5, __reginatext ) ; }
#line 3522 "yaccsrc.c"
    break;

  case 200: /* inputstmt: nspace INPUT nspace resourcei  */
#line 723 "yaccsrc.y"
                                             { with->p[0] = yyvsp[0]; }
#line 3528 "yaccsrc.c"
    break;

  case 201: /* inputstmt: nspace INPUT error  */
#line 724 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 6, __reginatext ) ; }
#line 3534 "yaccsrc.c"
    break;

  case 202: /* outputstmt: nspace OUTPUT nspace resourceo  */
#line 727 "yaccsrc.y"
                                              { with->p[1] = yyvsp[0]; }
#line 3540 "yaccsrc.c"
    break;

  case 203: /* outputstmt: nspace OUTPUT error  */
#line 728 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 7, __reginatext ) ; }
#line 3546 "yaccsrc.c"
    break;

  case 204: /* errorstmt: nspace ERROR nspace resourceo  */
#line 731 "yaccsrc.y"
                                             { with->p[2] = yyvsp[0]; }
#line 3552 "yaccsrc.c"
    break;

  case 205: /* errorstmt: nspace ERROR error  */
#line 732 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 14, __reginatext ) ; }
#line 3558 "yaccsrc.c"
    break;

  case 206: /* resourcei: resources  */
#line 735 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3564 "yaccsrc.c"
    break;

  case 207: /* resourcei: NOEOL resources  */
#line 736 "yaccsrc.y"
                                       { yyval = yyvsp[0] ;
                                         yyval->u.of.noeol = 1 ; }
#line 3571 "yaccsrc.c"
    break;

  case 208: /* resourcei: NOEOL error  */
#line 738 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 8, __reginatext ) ; }
#line 3577 "yaccsrc.c"
    break;

  case 209: /* resourcei: NORMAL  */
#line 739 "yaccsrc.y"
                                       { yyval = makenode(X_ADDR_WITH, 0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3585 "yaccsrc.c"
    break;

  case 210: /* resourceo: resources  */
#line 744 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3591 "yaccsrc.c"
    break;

  case 211: /* resourceo: APPEND resources  */
#line 745 "yaccsrc.y"
                                       { yyval = yyvsp[0] ;
                                         yyval->u.of.append = 1 ; }
#line 3598 "yaccsrc.c"
    break;

  case 212: /* resourceo: APPEND error  */
#line 747 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 8, __reginatext ) ; }
#line 3604 "yaccsrc.c"
    break;

  case 213: /* resourceo: REPLACE resources  */
#line 748 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3610 "yaccsrc.c"
    break;

  case 214: /* resourceo: REPLACE error  */
#line 749 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 9, __reginatext ) ; }
#line 3616 "yaccsrc.c"
    break;

  case 215: /* resourceo: NORMAL  */
#line 750 "yaccsrc.y"
                                       { yyval = makenode(X_ADDR_WITH, 0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3624 "yaccsrc.c"
    break;

  case 216: /* resources: STREAM nnvir  */
#line 755 "yaccsrc.y"
                                       { /* ANSI extension: nsimsymb is
                                          * used by the standard, but I think
                                          * there are no reasons why using
                                          * it here as a must. FGC
                                          */
                                         yyval = yyvsp[0] ;
                                         yyval->u.of.awt = awtSTREAM;
                                         SymbolDetect |= SD_ADDRWITH ; }
#line 3637 "yaccsrc.c"
    break;

  case 217: /* resources: STREAM error  */
#line 763 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_OPTION, 1, __reginatext ) ; }
#line 3643 "yaccsrc.c"
    break;

  case 218: /* resources: STEM nsimsymb  */
#line 764 "yaccsrc.y"
                                       {
                                         streng *tmp = yyvsp[0]->name;
                                         char *p;

                                         /*
                                          * expect a single dot as the last character
                                          */
                                         p = (char *)memchr( tmp->value, '.', tmp->len );
                                         if ( p != tmp->value + tmp->len - 1 )
                                            exiterror( ERR_INVALID_OPTION, 3, __reginatext );
                                         yyval = yyvsp[0] ;
                                         yyval->u.of.awt = awtSTEM ;
                                         SymbolDetect |= SD_ADDRWITH ; }
#line 3661 "yaccsrc.c"
    break;

  case 219: /* resources: STEM error  */
#line 777 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_OPTION, 2, __reginatext ) ; }
#line 3667 "yaccsrc.c"
    break;

  case 220: /* resources: LIFO nnvir  */
#line 778 "yaccsrc.y"
                                       {
                                         yyval = yyvsp[0] ;
                                         yyval->u.of.awt = awtLIFO ;
                                         SymbolDetect |= SD_ADDRWITH ; }
#line 3676 "yaccsrc.c"
    break;

  case 221: /* resources: LIFO error  */
#line 782 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_OPTION, 100, __reginatext ) ; }
#line 3682 "yaccsrc.c"
    break;

  case 222: /* resources: FIFO nnvir  */
#line 783 "yaccsrc.y"
                                       {
                                         yyval = yyvsp[0] ;
                                         yyval->u.of.awt = awtFIFO ;
                                         SymbolDetect |= SD_ADDRWITH ; }
#line 3691 "yaccsrc.c"
    break;

  case 223: /* resources: FIFO error  */
#line 787 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_OPTION, 101, __reginatext ) ; }
#line 3697 "yaccsrc.c"
    break;

  case 224: /* $@8: %empty  */
#line 790 "yaccsrc.y"
                                       { SymbolDetect &= ~SD_ADDRWITH ; }
#line 3703 "yaccsrc.c"
    break;

  case 225: /* nsimsymb: $@8 nspace addrSim  */
#line 791 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3709 "yaccsrc.c"
    break;

  case 226: /* $@9: %empty  */
#line 794 "yaccsrc.y"
                                       { SymbolDetect &= ~SD_ADDRWITH ; }
#line 3715 "yaccsrc.c"
    break;

  case 227: /* nnvir: $@9 nspace addrAll  */
#line 795 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3721 "yaccsrc.c"
    break;

  case 230: /* addrAll: addrSim  */
#line 802 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3727 "yaccsrc.c"
    break;

  case 231: /* addrAll: addrString  */
#line 803 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3733 "yaccsrc.c"
    break;

  case 232: /* addrSim: xsimsymb  */
#line 806 "yaccsrc.y"
                                       { yyval = makenode(X_ADDR_WITH, 0) ;
                                         yyval->name = (streng *) yyvsp[0] ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->u.of.ant = antSIMSYMBOL;
                                       }
#line 3744 "yaccsrc.c"
    break;

  case 233: /* addrString: string  */
#line 814 "yaccsrc.y"
                                       { yyval = makenode(X_ADDR_WITH, 0) ;
                                         yyval->name = (streng *) yyvsp[0] ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->u.of.ant = antSTRING;
                                       }
#line 3755 "yaccsrc.c"
    break;

  case 234: /* dovar: DOVARIABLE  */
#line 822 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 3761 "yaccsrc.c"
    break;

  case 235: /* tobyfor: TO expr nspace  */
#line 825 "yaccsrc.y"
                                       { yyval = makenode(X_DO_TO,1,yyvsp[-1]) ; }
#line 3767 "yaccsrc.c"
    break;

  case 236: /* tobyfor: FOR expr nspace  */
#line 826 "yaccsrc.y"
                                       { yyval = makenode(X_DO_FOR,1,yyvsp[-1]) ; }
#line 3773 "yaccsrc.c"
    break;

  case 237: /* tobyfor: BY expr nspace  */
#line 827 "yaccsrc.y"
                                       { yyval = makenode(X_DO_BY,1,yyvsp[-1]) ; }
#line 3779 "yaccsrc.c"
    break;

  case 238: /* conditional: WHILE expr nspace  */
#line 830 "yaccsrc.y"
                                       { yyval = makenode(X_WHILE,1,yyvsp[-1]) ; }
#line 3785 "yaccsrc.c"
    break;

  case 239: /* conditional: UNTIL expr nspace  */
#line 831 "yaccsrc.y"
                                       { yyval = makenode(X_UNTIL,1,yyvsp[-1]) ; }
#line 3791 "yaccsrc.c"
    break;

  case 240: /* conditional: %empty  */
#line 832 "yaccsrc.y"
                                       { yyval = NULL ; }
#line 3797 "yaccsrc.c"
    break;

  case 241: /* drop_stat: drop anyvars error  */
#line 835 "yaccsrc.y"
                                  { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ) ;}
#line 3803 "yaccsrc.c"
    break;

  case 242: /* drop_stat: drop anyvars  */
#line 836 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3810 "yaccsrc.c"
    break;

  case 243: /* upper_stat: upper anyvars error  */
#line 840 "yaccsrc.y"
                                   { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ) ;}
#line 3816 "yaccsrc.c"
    break;

  case 244: /* upper_stat: upper anyvars  */
#line 841 "yaccsrc.y"
                                        { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3823 "yaccsrc.c"
    break;

  case 245: /* exit_stat: exit nexpr  */
#line 845 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3830 "yaccsrc.c"
    break;

  case 246: /* if_stat: if expr nlncl THEN nlncl ystatement  */
#line 850 "yaccsrc.y"
                                       { move_labels( yyvsp[-5], yyvsp[0], level - 1 );
                                         yyval = yyvsp[-5];
                                         yyval->p[0] = optgluelast( yyvsp[-4], yyvsp[-3] );
                                         yyval->p[0]->o.last = NULL;
                                         yyval->p[1] = optgluelast( yyvsp[-1], yyvsp[0] );
                                         yyval->p[1]->o.last = NULL;
                                         level--; }
#line 3842 "yaccsrc.c"
    break;

  case 247: /* if_stat: if expr nlncl THEN nlncl ystatement ELSE nlncl ystatement  */
#line 858 "yaccsrc.y"
                                       { move_labels( yyvsp[-8], yyvsp[0], level - 1 );
                                         yyval = yyvsp[-8];
                                         yyval->p[0] = optgluelast( yyvsp[-7], yyvsp[-6] );
                                         yyval->p[0]->o.last = NULL;
                                         yyval->p[1] = optgluelast( yyvsp[-4], yyvsp[-3] );
                                         yyval->p[1]->o.last = NULL;
                                         yyval->p[2] = optgluelast( yyvsp[-1], yyvsp[0] );
                                         yyval->p[2]->o.last = NULL;
                                         level--; }
#line 3856 "yaccsrc.c"
    break;

  case 248: /* if_stat: if expr nlncl THEN nlncl ystatement ELSE nlncl error  */
#line 868 "yaccsrc.y"
                                       {  exiterror( ERR_INCOMPLETE_STRUCT, 4 ) ;}
#line 3862 "yaccsrc.c"
    break;

  case 249: /* if_stat: if expr nlncl THEN nlncl error  */
#line 870 "yaccsrc.y"
                                       {  exiterror( ERR_INCOMPLETE_STRUCT, 3 ) ;}
#line 3868 "yaccsrc.c"
    break;

  case 250: /* if_stat: if ncl  */
#line 871 "yaccsrc.y"
                                       {  exiterror( ERR_INCOMPLETE_STRUCT, 0 ) ;}
#line 3874 "yaccsrc.c"
    break;

  case 251: /* if_stat: if expr nlncl error  */
#line 872 "yaccsrc.y"
                                       {  exiterror( ERR_THEN_EXPECTED, 1, parser_data.if_linenr, __reginatext )  ; }
#line 3880 "yaccsrc.c"
    break;

  case 252: /* unexp_then: gruff THEN  */
#line 875 "yaccsrc.y"
                                       {  exiterror( ERR_THEN_UNEXPECTED, 1 )  ; }
#line 3886 "yaccsrc.c"
    break;

  case 253: /* unexp_else: gruff ELSE  */
#line 878 "yaccsrc.y"
                                       {  exiterror( ERR_THEN_UNEXPECTED, 2 )  ; }
#line 3892 "yaccsrc.c"
    break;

  case 254: /* ipret_stat: interpret expr  */
#line 881 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3899 "yaccsrc.c"
    break;

  case 255: /* iterate_stat: iterate simsymb  */
#line 886 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 3906 "yaccsrc.c"
    break;

  case 256: /* iterate_stat: iterate simsymb error  */
#line 888 "yaccsrc.y"
                                     { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 3912 "yaccsrc.c"
    break;

  case 257: /* iterate_stat: iterate  */
#line 889 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3918 "yaccsrc.c"
    break;

  case 258: /* label_stat: labelname  */
#line 892 "yaccsrc.y"
                                       { yyval = yyvsp[0] ;
                                         yyval->u.trace_only =
                                                         (level == 0) ? 0 : 1;
                                         newlabel( (const tsd_t *)parser_data.TSD,
                                                   &parser_data,
                                                   yyvsp[0] ) ; }
#line 3929 "yaccsrc.c"
    break;

  case 259: /* labelname: label  */
#line 900 "yaccsrc.y"
                                       { yyval = yyvsp[0] ;
                                         yyval->name = Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 3936 "yaccsrc.c"
    break;

  case 260: /* leave_stat: leave simsymb  */
#line 904 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 3943 "yaccsrc.c"
    break;

  case 261: /* leave_stat: leave simsymb error  */
#line 906 "yaccsrc.y"
                                   { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 3949 "yaccsrc.c"
    break;

  case 262: /* leave_stat: leave  */
#line 907 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3955 "yaccsrc.c"
    break;

  case 263: /* nop_stat: nop  */
#line 910 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3961 "yaccsrc.c"
    break;

  case 264: /* nop_stat: nop error  */
#line 911 "yaccsrc.y"
                         {  exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 3967 "yaccsrc.c"
    break;

  case 265: /* numeric_stat: numeric DIGITS expr  */
#line 914 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_NUM_D ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3975 "yaccsrc.c"
    break;

  case 266: /* numeric_stat: numeric DIGITS  */
#line 917 "yaccsrc.y"
                                       { yyval = yyvsp[-1]; yyval->type = X_NUM_DDEF ; }
#line 3981 "yaccsrc.c"
    break;

  case 267: /* numeric_stat: numeric FORM form_expr error  */
#line 918 "yaccsrc.y"
                                            { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 3987 "yaccsrc.c"
    break;

  case 268: /* numeric_stat: numeric FORM form_expr  */
#line 919 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_NUM_F ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3995 "yaccsrc.c"
    break;

  case 269: /* numeric_stat: numeric FORM  */
#line 922 "yaccsrc.y"
                                       { /* NOTE! This clashes ANSI! */
                                         yyval = yyvsp[-1] ; yyval->type=X_NUM_FRMDEF ;}
#line 4002 "yaccsrc.c"
    break;

  case 270: /* numeric_stat: numeric FORM VALUE expr  */
#line 924 "yaccsrc.y"
                                       { yyval = yyvsp[-3] ; yyval->type=X_NUM_V ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4009 "yaccsrc.c"
    break;

  case 271: /* numeric_stat: numeric FORM error  */
#line 926 "yaccsrc.y"
                                  { exiterror( ERR_INV_SUBKEYWORD, 11, "ENGINEERING SCIENTIFIC", __reginatext ) ;}
#line 4015 "yaccsrc.c"
    break;

  case 272: /* numeric_stat: numeric FUZZ  */
#line 927 "yaccsrc.y"
                                       { yyval = yyvsp[-1]; yyval->type = X_NUM_FDEF ;}
#line 4021 "yaccsrc.c"
    break;

  case 273: /* numeric_stat: numeric FUZZ expr  */
#line 928 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_NUM_FUZZ ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4029 "yaccsrc.c"
    break;

  case 274: /* numeric_stat: numeric error  */
#line 931 "yaccsrc.y"
                             { exiterror( ERR_INV_SUBKEYWORD, 15, "DIGITS FORM FUZZ", __reginatext ) ;}
#line 4035 "yaccsrc.c"
    break;

  case 275: /* form_expr: SCIENTIFIC  */
#line 934 "yaccsrc.y"
                                       { yyval = makenode(X_NUM_SCI,0) ; }
#line 4041 "yaccsrc.c"
    break;

  case 276: /* form_expr: ENGINEERING  */
#line 935 "yaccsrc.y"
                                       { yyval = makenode(X_NUM_ENG,0) ; }
#line 4047 "yaccsrc.c"
    break;

  case 277: /* options_stat: options nexpr  */
#line 938 "yaccsrc.y"
                                       { (yyval=yyvsp[-1])->p[0]=yyvsp[0] ; }
#line 4053 "yaccsrc.c"
    break;

  case 278: /* parse_stat: parse parse_flags parse_param templs  */
#line 942 "yaccsrc.y"
                                       { yyval = yyvsp[-3] ;
                                         yyval->type = X_PARSE ;
                                         yyval->u.parseflags = (int) yyvsp[-2] ;
                                         yyval->p[0] = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ; }
#line 4063 "yaccsrc.c"
    break;

  case 279: /* parse_stat: parse parse_param templs  */
#line 948 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_PARSE ;
                                         yyval->u.parseflags = 0;
                                         yyval->p[0] = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ; }
#line 4073 "yaccsrc.c"
    break;

  case 280: /* parse_stat: parse parse_flags error  */
#line 953 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 12, "ARG EXTERNAL LINEIN PULL SOURCE VAR VALUE VERSION", __reginatext ) ;}
#line 4079 "yaccsrc.c"
    break;

  case 281: /* parse_stat: parse error  */
#line 954 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 12, "ARG CASELESS EXTERNAL LINEIN LOWER PULL SOURCE UPPER VAR VALUE VERSION", __reginatext ) ;}
#line 4085 "yaccsrc.c"
    break;

  case 282: /* parse_flags: UPPER  */
#line 957 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_UPPER  |
                                                         PARSE_NORMAL); }
#line 4092 "yaccsrc.c"
    break;

  case 283: /* parse_flags: UPPER CASELESS  */
#line 959 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_UPPER  |
                                                         PARSE_CASELESS); }
#line 4099 "yaccsrc.c"
    break;

  case 284: /* parse_flags: CASELESS UPPER  */
#line 961 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_UPPER  |
                                                         PARSE_CASELESS); }
#line 4106 "yaccsrc.c"
    break;

  case 285: /* parse_flags: LOWER  */
#line 963 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_LOWER  |
                                                         PARSE_NORMAL); }
#line 4113 "yaccsrc.c"
    break;

  case 286: /* parse_flags: LOWER CASELESS  */
#line 965 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_LOWER  |
                                                         PARSE_CASELESS); }
#line 4120 "yaccsrc.c"
    break;

  case 287: /* parse_flags: CASELESS LOWER  */
#line 967 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_LOWER  |
                                                         PARSE_CASELESS); }
#line 4127 "yaccsrc.c"
    break;

  case 288: /* parse_flags: CASELESS  */
#line 969 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_NORMAL |
                                                         PARSE_CASELESS); }
#line 4134 "yaccsrc.c"
    break;

  case 289: /* templs: templs ',' template  */
#line 973 "yaccsrc.y"
                                       { /* fixes bugs like bug 579711 */
                                         yyval = optgluelast( yyvsp[-2], yyvsp[0] ); }
#line 4141 "yaccsrc.c"
    break;

  case 290: /* templs: template  */
#line 975 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 4147 "yaccsrc.c"
    break;

  case 291: /* parse_param: ARG  */
#line 978 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_ARG,0) ; }
#line 4153 "yaccsrc.c"
    break;

  case 292: /* parse_param: LINEIN  */
#line 979 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_EXT,0) ; }
#line 4159 "yaccsrc.c"
    break;

  case 293: /* parse_param: EXTERNAL  */
#line 980 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_EXT,0) ; }
#line 4165 "yaccsrc.c"
    break;

  case 294: /* parse_param: VERSION  */
#line 981 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_VER,0) ; }
#line 4171 "yaccsrc.c"
    break;

  case 295: /* parse_param: PULL  */
#line 982 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_PULL,0) ; }
#line 4177 "yaccsrc.c"
    break;

  case 296: /* parse_param: SOURCE  */
#line 983 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_SRC,0) ; }
#line 4183 "yaccsrc.c"
    break;

  case 297: /* parse_param: VAR simsymb  */
#line 984 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_VAR,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 4190 "yaccsrc.c"
    break;

  case 298: /* parse_param: VALUE nexpr WITH  */
#line 986 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_VAL,1,yyvsp[-1]) ; }
#line 4196 "yaccsrc.c"
    break;

  case 299: /* parse_param: VALUE error  */
#line 987 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_TEMPLATE, 3 ) ;}
#line 4202 "yaccsrc.c"
    break;

  case 300: /* proc_stat: proc  */
#line 990 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 4208 "yaccsrc.c"
    break;

  case 301: /* proc_stat: proc error  */
#line 991 "yaccsrc.y"
                          { exiterror( ERR_INV_SUBKEYWORD, 17, __reginatext ) ;}
#line 4214 "yaccsrc.c"
    break;

  case 302: /* proc_stat: proc EXPOSE error  */
#line 992 "yaccsrc.y"
                                 { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ) ;}
#line 4220 "yaccsrc.c"
    break;

  case 303: /* proc_stat: proc EXPOSE anyvars error  */
#line 993 "yaccsrc.y"
                                         { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ) ;}
#line 4226 "yaccsrc.c"
    break;

  case 304: /* proc_stat: proc EXPOSE anyvars  */
#line 994 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4233 "yaccsrc.c"
    break;

  case 305: /* pull_stat: pull template  */
#line 998 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4240 "yaccsrc.c"
    break;

  case 306: /* push_stat: push nexpr  */
#line 1002 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4247 "yaccsrc.c"
    break;

  case 307: /* queue_stat: queue nexpr  */
#line 1006 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4254 "yaccsrc.c"
    break;

  case 308: /* say_stat: say nexpr  */
#line 1010 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4261 "yaccsrc.c"
    break;

  case 309: /* return_stat: return nexpr  */
#line 1014 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         /* OO: "RETURN x." returns a Stem object */
                                         if ( yyvsp[0] && yyvsp[0]->type == X_STEM_SYMBOL
                                           && __regina_oo_keywords() )
                                            yyvsp[0] = makenode( X_OO_STEMOBJ, 1, yyvsp[0] ) ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4272 "yaccsrc.c"
    break;

  case 310: /* sel_end: END simsymb  */
#line 1022 "yaccsrc.y"
                                       { exiterror( ERR_UNMATCHED_END, 0 ) ;}
#line 4278 "yaccsrc.c"
    break;

  case 311: /* sel_end: END simsymb error  */
#line 1023 "yaccsrc.y"
                                       { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 4284 "yaccsrc.c"
    break;

  case 312: /* sel_end: END  */
#line 1024 "yaccsrc.y"
                                       { level--; }
#line 4290 "yaccsrc.c"
    break;

  case 313: /* select_stat: select ncl when_stats otherwise_stat sel_end  */
#line 1028 "yaccsrc.y"
                                       { yyval = yyvsp[-4];
                                         yyval->p[0] = optgluelast( yyvsp[-3], yyvsp[-2] );
                                         yyval->p[0]->o.last = NULL;
                                         yyval->p[1] = yyvsp[-1]; }
#line 4299 "yaccsrc.c"
    break;

  case 314: /* select_stat: select ncl END error  */
#line 1032 "yaccsrc.y"
                                       {  exiterror( ERR_WHEN_EXPECTED, 1, parser_data.select_linenr, __reginatext ) ;}
#line 4305 "yaccsrc.c"
    break;

  case 315: /* select_stat: select ncl otherwise error  */
#line 1034 "yaccsrc.y"
                                       {  exiterror( ERR_WHEN_EXPECTED, 2, parser_data.select_linenr, __reginatext ) ;}
#line 4311 "yaccsrc.c"
    break;

  case 316: /* select_stat: select error  */
#line 1035 "yaccsrc.y"
                                       {  exiterror( ERR_EXTRA_DATA, 1, __reginatext )  ;}
#line 4317 "yaccsrc.c"
    break;

  case 317: /* select_stat: select ncl THEN  */
#line 1036 "yaccsrc.y"
                                       {  exiterror( ERR_THEN_UNEXPECTED, 0 ) ;}
#line 4323 "yaccsrc.c"
    break;

  case 318: /* select_stat: select ncl when_stats otherwise error  */
#line 1038 "yaccsrc.y"
                                       {  exiterror( ERR_INCOMPLETE_STRUCT, 0 ) ;}
#line 4329 "yaccsrc.c"
    break;

  case 319: /* when_stats: when_stats when_stat  */
#line 1041 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 4335 "yaccsrc.c"
    break;

  case 320: /* when_stats: when_stat  */
#line 1042 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 4341 "yaccsrc.c"
    break;

  case 321: /* when_stats: error  */
#line 1043 "yaccsrc.y"
                                       {  exiterror( ERR_WHEN_EXPECTED, 2, parser_data.select_linenr, __reginatext )  ;}
#line 4347 "yaccsrc.c"
    break;

  case 322: /* when_stat: when expr nlncl THEN nlncl statement  */
#line 1047 "yaccsrc.y"
                                       { yyval = yyvsp[-5]; /* fixes bugs like bug 579711 */
                                         yyval->p[0] = optgluelast( yyvsp[-4], yyvsp[-3] );
                                         yyval->p[0]->o.last = NULL;
                                         yyval->p[1] = optgluelast( yyvsp[-1], yyvsp[0] );
                                         yyval->p[1]->o.last = NULL; }
#line 4357 "yaccsrc.c"
    break;

  case 323: /* when_stat: when expr nlncl THEN nlncl statement THEN  */
#line 1053 "yaccsrc.y"
                                       {  exiterror( ERR_THEN_UNEXPECTED, 0 ) ;}
#line 4363 "yaccsrc.c"
    break;

  case 324: /* when_stat: when expr  */
#line 1054 "yaccsrc.y"
                                       {  exiterror( ERR_THEN_EXPECTED, 2, parser_data.select_linenr, __reginatext )  ; }
#line 4369 "yaccsrc.c"
    break;

  case 325: /* when_stat: when error  */
#line 1055 "yaccsrc.y"
                                       {  exiterror( ERR_INVALID_EXPRESSION, 0 ) ;}
#line 4375 "yaccsrc.c"
    break;

  case 326: /* ex_when_stat: gruff when  */
#line 1065 "yaccsrc.y"
                                       { exiterror( ERR_WHEN_UNEXPECTED, 1 ); }
#line 4381 "yaccsrc.c"
    break;

  case 327: /* ex_when_stat: gruff otherwise  */
#line 1066 "yaccsrc.y"
                                       { exiterror( ERR_WHEN_UNEXPECTED, 2 ); }
#line 4387 "yaccsrc.c"
    break;

  case 328: /* otherwise_stat: otherwise nlncl nxstats  */
#line 1069 "yaccsrc.y"
                                         { yyval = yyvsp[-2];
                                         yyval->p[0] = optgluelast( yyvsp[-1], yyvsp[0] );
                                         if ( yyval->p[0] )
                                            yyval->p[0]->o.last = NULL; }
#line 4396 "yaccsrc.c"
    break;

  case 329: /* otherwise_stat: %empty  */
#line 1073 "yaccsrc.y"
                                       { yyval = makenode(X_NO_OTHERWISE,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 4404 "yaccsrc.c"
    break;

  case 330: /* signal_stat: signal VALUE expr  */
#line 1079 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_SIG_VAL ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4412 "yaccsrc.c"
    break;

  case 331: /* signal_stat: signal signal_name error  */
#line 1082 "yaccsrc.y"
                                       { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 4418 "yaccsrc.c"
    break;

  case 332: /* signal_stat: signal signal_name  */
#line 1083 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->name = (streng *)yyvsp[0] ; }
#line 4425 "yaccsrc.c"
    break;

  case 333: /* signal_stat: signal on error  */
#line 1085 "yaccsrc.y"
                              { exiterror( ERR_INV_SUBKEYWORD, 3, "ERROR FAILURE HALT NOTREADY NOVALUE SYNTAX LOSTDIGITS", __reginatext ) ;}
#line 4431 "yaccsrc.c"
    break;

  case 334: /* signal_stat: signal off error  */
#line 1086 "yaccsrc.y"
                               { exiterror( ERR_INV_SUBKEYWORD, 4, "ERROR FAILURE HALT NOTREADY NOVALUE SYNTAX LOSTDIGITS", __reginatext ) ;}
#line 4437 "yaccsrc.c"
    break;

  case 335: /* signal_stat: signal on s_action error  */
#line 1087 "yaccsrc.y"
                                       { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 4443 "yaccsrc.c"
    break;

  case 336: /* signal_stat: signal on s_action namespec error  */
#line 1088 "yaccsrc.y"
                                                { exiterror( ERR_STRING_EXPECTED, 3, __reginatext ) ;}
#line 4449 "yaccsrc.c"
    break;

  case 337: /* signal_stat: signal on s_action namespec  */
#line 1090 "yaccsrc.y"
                                       { yyval = yyvsp[-3] ;
                                         yyval->type = X_SIG_SET ;
                                         yyval->p[0] = yyvsp[-2] ;
                                         yyval->name = (streng *)yyvsp[0] ;
                                         yyval->p[1] = yyvsp[-1] ; }
#line 4459 "yaccsrc.c"
    break;

  case 338: /* signal_stat: signal on s_action  */
#line 1095 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_SIG_SET ;
                                         yyval->p[0] = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ; }
#line 4468 "yaccsrc.c"
    break;

  case 339: /* signal_stat: signal off s_action error  */
#line 1099 "yaccsrc.y"
                                        { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 4474 "yaccsrc.c"
    break;

  case 340: /* signal_stat: signal off s_action  */
#line 1100 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_SIG_SET ;
                                         yyval->p[0] = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ; }
#line 4483 "yaccsrc.c"
    break;

  case 341: /* signal_name: asymbol  */
#line 1106 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 4489 "yaccsrc.c"
    break;

  case 342: /* signal_name: STRING  */
#line 1107 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 4495 "yaccsrc.c"
    break;

  case 343: /* signal_name: error  */
#line 1108 "yaccsrc.y"
                                       { exiterror( ERR_STRING_EXPECTED, 4, __reginatext );}
#line 4501 "yaccsrc.c"
    break;

  case 344: /* namespec: NAME SIMSYMBOL  */
#line 1111 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue);}
#line 4507 "yaccsrc.c"
    break;

  case 345: /* namespec: NAME STRING  */
#line 1112 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 4513 "yaccsrc.c"
    break;

  case 346: /* namespec: NAME error  */
#line 1113 "yaccsrc.y"
                                       { exiterror( ERR_STRING_EXPECTED, 3, __reginatext ) ;}
#line 4519 "yaccsrc.c"
    break;

  case 347: /* asymbol: CONSYMBOL  */
#line 1116 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 4525 "yaccsrc.c"
    break;

  case 348: /* asymbol: SIMSYMBOL  */
#line 1117 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 4531 "yaccsrc.c"
    break;

  case 349: /* on: ON  */
#line 1120 "yaccsrc.y"
                                       { yyval = makenode(X_ON,0) ; }
#line 4537 "yaccsrc.c"
    break;

  case 350: /* off: OFF  */
#line 1123 "yaccsrc.y"
                                       { yyval = makenode(X_OFF,0) ; }
#line 4543 "yaccsrc.c"
    break;

  case 351: /* c_action: ERROR  */
#line 1126 "yaccsrc.y"
                                       { yyval = makenode(X_S_ERROR,0) ; }
#line 4549 "yaccsrc.c"
    break;

  case 352: /* c_action: HALT  */
#line 1127 "yaccsrc.y"
                                       { yyval = makenode(X_S_HALT,0) ; }
#line 4555 "yaccsrc.c"
    break;

  case 353: /* c_action: NOTREADY  */
#line 1128 "yaccsrc.y"
                                       { yyval = makenode(X_S_NOTREADY,0) ; }
#line 4561 "yaccsrc.c"
    break;

  case 354: /* c_action: FAILURE  */
#line 1129 "yaccsrc.y"
                                       { yyval = makenode(X_S_FAILURE,0) ; }
#line 4567 "yaccsrc.c"
    break;

  case 355: /* s_action: c_action  */
#line 1132 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 4573 "yaccsrc.c"
    break;

  case 356: /* s_action: NOVALUE  */
#line 1133 "yaccsrc.y"
                                       { yyval = makenode(X_S_NOVALUE,0) ; }
#line 4579 "yaccsrc.c"
    break;

  case 357: /* s_action: SYNTAX  */
#line 1134 "yaccsrc.y"
                                       { yyval = makenode(X_S_SYNTAX,0) ; }
#line 4585 "yaccsrc.c"
    break;

  case 358: /* s_action: LOSTDIGITS  */
#line 1135 "yaccsrc.y"
                                       { yyval = makenode(X_S_LOSTDIGITS,0) ; }
#line 4591 "yaccsrc.c"
    break;

  case 359: /* trace_stat: trace VALUE expr  */
#line 1138 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4598 "yaccsrc.c"
    break;

  case 360: /* trace_stat: trace nexpr  */
#line 1140 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4605 "yaccsrc.c"
    break;

  case 361: /* trace_stat: trace whatever error  */
#line 1142 "yaccsrc.y"
                                   { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 4611 "yaccsrc.c"
    break;

  case 362: /* trace_stat: trace whatever  */
#line 1143 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 4618 "yaccsrc.c"
    break;

  case 363: /* whatever: WHATEVER  */
#line 1147 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 4624 "yaccsrc.c"
    break;

  case 364: /* assignment: ass_part nexpr  */
#line 1151 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         /*
                                          * An assignment is a numerical
                                          * assignment if and only if we have
                                          * to do a numerical operation, which
                                          * is equivalent to the existence of
                                          * one more argument to $2.
                                          * This fixes bug 720166.
                                          */
                                         if (yyvsp[0] &&
                                             yyvsp[0]->p[0] &&
                                             gettypeof(yyvsp[0]) == IS_A_NUMBER)
                                            yyval->type = X_NASSIGN ; }
#line 4643 "yaccsrc.c"
    break;

  case 365: /* ass_part: ASSIGNMENTVARIABLE  */
#line 1167 "yaccsrc.y"
                                       { yyval = makenode(X_ASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4652 "yaccsrc.c"
    break;

  case 366: /* plusassignment: plus_ass_part nexpr  */
#line 1173 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "+=" );
                                       }
#line 4662 "yaccsrc.c"
    break;

  case 367: /* plus_ass_part: PLUSASSIGNMENTVARIABLE  */
#line 1180 "yaccsrc.y"
                                       { yyval = makenode(X_PLUSASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4671 "yaccsrc.c"
    break;

  case 368: /* minusassignment: minus_ass_part nexpr  */
#line 1186 "yaccsrc.y"
                                         { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "-=" );
                                       }
#line 4681 "yaccsrc.c"
    break;

  case 369: /* minus_ass_part: MINUSASSIGNMENTVARIABLE  */
#line 1193 "yaccsrc.y"
                                         { yyval = makenode(X_MINUSASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4690 "yaccsrc.c"
    break;

  case 370: /* multassignment: mult_ass_part nexpr  */
#line 1199 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "*=" );
                                       }
#line 4700 "yaccsrc.c"
    break;

  case 371: /* mult_ass_part: MULTASSIGNMENTVARIABLE  */
#line 1206 "yaccsrc.y"
                                       { yyval = makenode(X_MULTASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4709 "yaccsrc.c"
    break;

  case 372: /* divassignment: div_ass_part nexpr  */
#line 1212 "yaccsrc.y"
                                     { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "/=" );
                                       }
#line 4719 "yaccsrc.c"
    break;

  case 373: /* div_ass_part: DIVASSIGNMENTVARIABLE  */
#line 1219 "yaccsrc.y"
                                     { yyval = makenode(X_DIVASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4728 "yaccsrc.c"
    break;

  case 374: /* intdivassignment: intdiv_ass_part nexpr  */
#line 1225 "yaccsrc.y"
                                           { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "%=" );
                                       }
#line 4738 "yaccsrc.c"
    break;

  case 375: /* intdiv_ass_part: INTDIVASSIGNMENTVARIABLE  */
#line 1232 "yaccsrc.y"
                                           { yyval = makenode(X_INTDIVASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4747 "yaccsrc.c"
    break;

  case 376: /* modulusassignment: modulus_ass_part nexpr  */
#line 1238 "yaccsrc.y"
                                             { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "//=" );
                                       }
#line 4757 "yaccsrc.c"
    break;

  case 377: /* modulus_ass_part: MODULUSASSIGNMENTVARIABLE  */
#line 1245 "yaccsrc.y"
                                             { yyval = makenode(X_MODULUSASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4766 "yaccsrc.c"
    break;

  case 378: /* andassignment: and_ass_part nexpr  */
#line 1251 "yaccsrc.y"
                                     { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "&=" );
                                       }
#line 4776 "yaccsrc.c"
    break;

  case 379: /* and_ass_part: ANDASSIGNMENTVARIABLE  */
#line 1258 "yaccsrc.y"
                                     { yyval = makenode(X_ANDASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4785 "yaccsrc.c"
    break;

  case 380: /* xorassignment: xor_ass_part nexpr  */
#line 1264 "yaccsrc.y"
                                     { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "&&=" );
                                       }
#line 4795 "yaccsrc.c"
    break;

  case 381: /* xor_ass_part: XORASSIGNMENTVARIABLE  */
#line 1271 "yaccsrc.y"
                                     { yyval = makenode(X_XORASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4804 "yaccsrc.c"
    break;

  case 382: /* orassignment: or_ass_part nexpr  */
#line 1277 "yaccsrc.y"
                                   { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "|=" );
                                       }
#line 4814 "yaccsrc.c"
    break;

  case 383: /* or_ass_part: ORASSIGNMENTVARIABLE  */
#line 1284 "yaccsrc.y"
                                   { yyval = makenode(X_ORASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4823 "yaccsrc.c"
    break;

  case 384: /* concatassignment: concat_ass_part nexpr  */
#line 1290 "yaccsrc.y"
                                           { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                       }
#line 4831 "yaccsrc.c"
    break;

  case 385: /* concat_ass_part: CONCATASSIGNMENTVARIABLE  */
#line 1295 "yaccsrc.y"
                                           { yyval = makenode(X_CONCATASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4840 "yaccsrc.c"
    break;

  case 386: /* $@10: %empty  */
#line 1302 "yaccsrc.y"
                                       { /* We have to accept exprs here even
                                          * if we just want to accept
                                          * '(' expr ')' only. We do this
                                          * because we appect
                                          * "call '(' exprs ')'" too.
                                          * This kann happen only if the
                                          * related control flag parendepth is
                                          * set. But since the parentheses are
                                          * voided just for the outer ones, we
                                          * can reduce the allowness level.
                                          * We don't have to set it back,
                                          * because the outer parentheses
                                          * either is THE one we look for or
                                          * none. This allows a faster error
                                          * detection and that's a good goal.*/
                                         parendepth--; }
#line 4861 "yaccsrc.c"
    break;

  case 387: /* expr: '(' $@10 exprs_sub  */
#line 1318 "yaccsrc.y"
                                       { parendepth++;
                                         if ( parendepth == 1 )
                                         {
                                            /* exprs on as-is basis */
                                            yyval = yyvsp[0];
                                         }
                                         else
                                         {
                                            /* Must already be a plain expr.
                                             * The nexpr part of exprs detects
                                             * mistakes. */
                                            yyval = reduce_expr_list( yyvsp[0],
                                                                REDUCE_EXPR );
                                            if ( yyval == yyvsp[0] )
                                               exiterror( ERR_INTERPRETER_FAILURE, 1, __FILE__, __LINE__, "Reduction of `exprs' not happened." );
                                         }
                                       }
#line 4883 "yaccsrc.c"
    break;

  case 388: /* $@11: %empty  */
#line 1335 "yaccsrc.y"
                                       { parendepth--; }
#line 4889 "yaccsrc.c"
    break;

  case 389: /* expr: expr '+' $@11 expr  */
#line 1336 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "+" );
                                         yyval = makenode( X_PLUSS, 2, yyvsp[-3], yyvsp[0] ); }
#line 4896 "yaccsrc.c"
    break;

  case 390: /* $@12: %empty  */
#line 1338 "yaccsrc.y"
                                       { parendepth--; }
#line 4902 "yaccsrc.c"
    break;

  case 391: /* expr: expr '-' $@12 expr  */
#line 1339 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "-" );
                                         yyval = makenode( X_MINUS, 2, yyvsp[-3], yyvsp[0] ); }
#line 4909 "yaccsrc.c"
    break;

  case 392: /* $@13: %empty  */
#line 1341 "yaccsrc.y"
                                       { parendepth--; }
#line 4915 "yaccsrc.c"
    break;

  case 393: /* expr: expr '*' $@13 expr  */
#line 1342 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "*" );
                                         yyval = makenode( X_MULT, 2, yyvsp[-3], yyvsp[0] ); }
#line 4922 "yaccsrc.c"
    break;

  case 394: /* expr: '*'  */
#line 1344 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "*" ); }
#line 4928 "yaccsrc.c"
    break;

  case 395: /* $@14: %empty  */
#line 1345 "yaccsrc.y"
                                       { parendepth--; }
#line 4934 "yaccsrc.c"
    break;

  case 396: /* expr: expr '/' $@14 expr  */
#line 1346 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "/" );
                                         yyval = makenode( X_DEVIDE, 2, yyvsp[-3], yyvsp[0] ); }
#line 4941 "yaccsrc.c"
    break;

  case 397: /* expr: '/'  */
#line 1348 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "/" ); }
#line 4947 "yaccsrc.c"
    break;

  case 398: /* $@15: %empty  */
#line 1349 "yaccsrc.y"
                                       { parendepth--; }
#line 4953 "yaccsrc.c"
    break;

  case 399: /* expr: expr MODULUS $@15 expr  */
#line 1350 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "//" );
                                         yyval = makenode( X_MODULUS, 2, yyvsp[-3], yyvsp[0] ); }
#line 4960 "yaccsrc.c"
    break;

  case 400: /* expr: MODULUS  */
#line 1352 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "//" ); }
#line 4966 "yaccsrc.c"
    break;

  case 401: /* $@16: %empty  */
#line 1353 "yaccsrc.y"
                                       { parendepth--; }
#line 4972 "yaccsrc.c"
    break;

  case 402: /* expr: expr '%' $@16 expr  */
#line 1354 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "%" );
                                         yyval = makenode( X_INTDIV, 2, yyvsp[-3], yyvsp[0] ); }
#line 4979 "yaccsrc.c"
    break;

  case 403: /* expr: '%'  */
#line 1356 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "%" ); }
#line 4985 "yaccsrc.c"
    break;

  case 404: /* $@17: %empty  */
#line 1357 "yaccsrc.y"
                                       { parendepth--; }
#line 4991 "yaccsrc.c"
    break;

  case 405: /* expr: expr '|' $@17 expr  */
#line 1358 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "|" );
                                         yyval = makenode( X_LOG_OR, 2, yyvsp[-3], yyvsp[0] ); }
#line 4998 "yaccsrc.c"
    break;

  case 406: /* expr: '|'  */
#line 1360 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "|" ); }
#line 5004 "yaccsrc.c"
    break;

  case 407: /* $@18: %empty  */
#line 1361 "yaccsrc.y"
                                       { parendepth--; }
#line 5010 "yaccsrc.c"
    break;

  case 408: /* expr: expr '&' $@18 expr  */
#line 1362 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "&" );
                                         yyval = makenode( X_LOG_AND, 2, yyvsp[-3], yyvsp[0] ); }
#line 5017 "yaccsrc.c"
    break;

  case 409: /* expr: '&'  */
#line 1364 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "&" ); }
#line 5023 "yaccsrc.c"
    break;

  case 410: /* $@19: %empty  */
#line 1365 "yaccsrc.y"
                                       { parendepth--; }
#line 5029 "yaccsrc.c"
    break;

  case 411: /* expr: expr XOR $@19 expr  */
#line 1366 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "&&" );
                                         yyval = makenode( X_LOG_XOR, 2, yyvsp[-3], yyvsp[0] ); }
#line 5036 "yaccsrc.c"
    break;

  case 412: /* expr: XOR  */
#line 1368 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "&&" ); }
#line 5042 "yaccsrc.c"
    break;

  case 413: /* $@20: %empty  */
#line 1369 "yaccsrc.y"
                                       { parendepth--; }
#line 5048 "yaccsrc.c"
    break;

  case 414: /* expr: expr EXP $@20 expr  */
#line 1370 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "**" );
                                         yyval = makenode( X_EXP, 2, yyvsp[-3], yyvsp[0] ); }
#line 5055 "yaccsrc.c"
    break;

  case 415: /* expr: EXP  */
#line 1372 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "**" ); }
#line 5061 "yaccsrc.c"
    break;

  case 416: /* $@21: %empty  */
#line 1373 "yaccsrc.y"
                                       { parendepth--; }
#line 5067 "yaccsrc.c"
    break;

  case 417: /* expr: expr SPACE $@21 expr  */
#line 1374 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], " " );
                                         yyval = makenode( X_SPACE, 2, yyvsp[-3], yyvsp[0] ); }
#line 5074 "yaccsrc.c"
    break;

  case 418: /* expr: SPACE  */
#line 1376 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, " " ); }
#line 5080 "yaccsrc.c"
    break;

  case 419: /* $@22: %empty  */
#line 1377 "yaccsrc.y"
                                       { parendepth--; }
#line 5086 "yaccsrc.c"
    break;

  case 420: /* expr: expr CONCATENATE $@22 expr  */
#line 1378 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "||" );
                                         yyval = makenode( X_CONCAT, 2, yyvsp[-3], yyvsp[0] ); }
#line 5093 "yaccsrc.c"
    break;

  case 421: /* expr: CONCATENATE  */
#line 1380 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "||" ); }
#line 5099 "yaccsrc.c"
    break;

  case 422: /* expr: NOT expr  */
#line 1381 "yaccsrc.y"
                                       { AUTO_REDUCE( yyvsp[0], "\\" );
                                         yyval = makenode( X_LOG_NOT, 1, yyvsp[0] ); }
#line 5106 "yaccsrc.c"
    break;

  case 423: /* expr: NOT  */
#line 1383 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "\\" ); }
#line 5112 "yaccsrc.c"
    break;

  case 424: /* $@23: %empty  */
#line 1384 "yaccsrc.y"
                                       { parendepth--; }
#line 5118 "yaccsrc.c"
    break;

  case 425: /* expr: expr '=' $@23 expr  */
#line 1385 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "=" );
                                         yyval = makenode( X_EQUAL, 2, yyvsp[-3], yyvsp[0] );
                                         transform( yyval ); }
#line 5126 "yaccsrc.c"
    break;

  case 426: /* expr: '='  */
#line 1388 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "=" ); }
#line 5132 "yaccsrc.c"
    break;

  case 427: /* $@24: %empty  */
#line 1389 "yaccsrc.y"
                                       { parendepth--; }
#line 5138 "yaccsrc.c"
    break;

  case 428: /* expr: expr GTE $@24 expr  */
#line 1390 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], ">=" );
                                         yyval = makenode( X_GTE, 2, yyvsp[-3], yyvsp[0] );
                                         transform( yyval ) ; }
#line 5146 "yaccsrc.c"
    break;

  case 429: /* expr: GTE  */
#line 1393 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, ">=" ); }
#line 5152 "yaccsrc.c"
    break;

  case 430: /* $@25: %empty  */
#line 1394 "yaccsrc.y"
                                       { parendepth--; }
#line 5158 "yaccsrc.c"
    break;

  case 431: /* expr: expr LTE $@25 expr  */
#line 1395 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "<=" );
                                         yyval = makenode( X_LTE, 2, yyvsp[-3], yyvsp[0] );
                                         transform( yyval ) ; }
#line 5166 "yaccsrc.c"
    break;

  case 432: /* expr: LTE  */
#line 1398 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "<=" ); }
#line 5172 "yaccsrc.c"
    break;

  case 433: /* $@26: %empty  */
#line 1399 "yaccsrc.y"
                                       { parendepth--; }
#line 5178 "yaccsrc.c"
    break;

  case 434: /* expr: expr GT $@26 expr  */
#line 1400 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], ">" );
                                         yyval = makenode( X_GT, 2, yyvsp[-3], yyvsp[0] );
                                         transform( yyval ) ; }
#line 5186 "yaccsrc.c"
    break;

  case 435: /* expr: GT  */
#line 1403 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, ">" ); }
#line 5192 "yaccsrc.c"
    break;

  case 436: /* $@27: %empty  */
#line 1404 "yaccsrc.y"
                                       { parendepth--; }
#line 5198 "yaccsrc.c"
    break;

  case 437: /* expr: expr LT $@27 expr  */
#line 1405 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "<" );
                                         yyval = makenode( X_LT, 2, yyvsp[-3], yyvsp[0] );
                                         transform( yyval ) ; }
#line 5206 "yaccsrc.c"
    break;

  case 438: /* expr: LT  */
#line 1408 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "<" ); }
#line 5212 "yaccsrc.c"
    break;

  case 439: /* $@28: %empty  */
#line 1409 "yaccsrc.y"
                                       { parendepth--; }
#line 5218 "yaccsrc.c"
    break;

  case 440: /* expr: expr DIFFERENT $@28 expr  */
#line 1410 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "\\=" );
                                         yyval = makenode( X_DIFF, 2, yyvsp[-3], yyvsp[0] );
                                         transform( yyval ) ; }
#line 5226 "yaccsrc.c"
    break;

  case 441: /* expr: DIFFERENT  */
#line 1413 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "\\=" ); }
#line 5232 "yaccsrc.c"
    break;

  case 442: /* $@29: %empty  */
#line 1414 "yaccsrc.y"
                                       { parendepth--; }
#line 5238 "yaccsrc.c"
    break;

  case 443: /* expr: expr EQUALEQUAL $@29 expr  */
#line 1415 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "==" );
                                         yyval = makenode( X_S_EQUAL, 2, yyvsp[-3], yyvsp[0] ); }
#line 5245 "yaccsrc.c"
    break;

  case 444: /* expr: EQUALEQUAL  */
#line 1417 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "==" ); }
#line 5251 "yaccsrc.c"
    break;

  case 445: /* $@30: %empty  */
#line 1418 "yaccsrc.y"
                                       { parendepth--; }
#line 5257 "yaccsrc.c"
    break;

  case 446: /* expr: expr NOTEQUALEQUAL $@30 expr  */
#line 1419 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "\\==" );
                                         yyval = makenode( X_S_DIFF, 2, yyvsp[-3], yyvsp[0] ); }
#line 5264 "yaccsrc.c"
    break;

  case 447: /* expr: NOTEQUALEQUAL  */
#line 1421 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "\\==" ); }
#line 5270 "yaccsrc.c"
    break;

  case 448: /* $@31: %empty  */
#line 1422 "yaccsrc.y"
                                       { parendepth--; }
#line 5276 "yaccsrc.c"
    break;

  case 449: /* expr: expr GTGT $@31 expr  */
#line 1423 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], ">>" );
                                         yyval = makenode( X_S_GT, 2, yyvsp[-3], yyvsp[0] ); }
#line 5283 "yaccsrc.c"
    break;

  case 450: /* expr: GTGT  */
#line 1425 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, ">>" ); }
#line 5289 "yaccsrc.c"
    break;

  case 451: /* $@32: %empty  */
#line 1426 "yaccsrc.y"
                                       { parendepth--; }
#line 5295 "yaccsrc.c"
    break;

  case 452: /* expr: expr LTLT $@32 expr  */
#line 1427 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "<<" );
                                         yyval = makenode( X_S_LT, 2, yyvsp[-3], yyvsp[0] ); }
#line 5302 "yaccsrc.c"
    break;

  case 453: /* expr: LTLT  */
#line 1429 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "<<" ); }
#line 5308 "yaccsrc.c"
    break;

  case 454: /* $@33: %empty  */
#line 1430 "yaccsrc.y"
                                       { parendepth--; }
#line 5314 "yaccsrc.c"
    break;

  case 455: /* expr: expr NOTGTGT $@33 expr  */
#line 1431 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "\\>>" );
                                         yyval = makenode( X_S_NGT, 2, yyvsp[-3], yyvsp[0] ); }
#line 5321 "yaccsrc.c"
    break;

  case 456: /* expr: NOTGTGT  */
#line 1433 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "\\>>" ); }
#line 5327 "yaccsrc.c"
    break;

  case 457: /* $@34: %empty  */
#line 1434 "yaccsrc.y"
                                       { parendepth--; }
#line 5333 "yaccsrc.c"
    break;

  case 458: /* expr: expr NOTLTLT $@34 expr  */
#line 1435 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "\\<<" );
                                         yyval = makenode( X_S_NLT, 2, yyvsp[-3], yyvsp[0] ); }
#line 5340 "yaccsrc.c"
    break;

  case 459: /* expr: NOTLTLT  */
#line 1437 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "\\<<" ); }
#line 5346 "yaccsrc.c"
    break;

  case 460: /* $@35: %empty  */
#line 1438 "yaccsrc.y"
                                       { parendepth--; }
#line 5352 "yaccsrc.c"
    break;

  case 461: /* expr: expr GTGTE $@35 expr  */
#line 1439 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], ">>=" );
                                         yyval = makenode( X_S_GTE, 2, yyvsp[-3], yyvsp[0] ); }
#line 5359 "yaccsrc.c"
    break;

  case 462: /* expr: GTGTE  */
#line 1441 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, ">>=" ); }
#line 5365 "yaccsrc.c"
    break;

  case 463: /* $@36: %empty  */
#line 1442 "yaccsrc.y"
                                       { parendepth--; }
#line 5371 "yaccsrc.c"
    break;

  case 464: /* expr: expr LTLTE $@36 expr  */
#line 1443 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "<<=" );
                                         yyval = makenode( X_S_LTE, 2, yyvsp[-3], yyvsp[0] ); }
#line 5378 "yaccsrc.c"
    break;

  case 465: /* expr: LTLTE  */
#line 1445 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "<<=" ); }
#line 5384 "yaccsrc.c"
    break;

  case 466: /* expr: expr MSGSEND msgtarget  */
#line 1446 "yaccsrc.y"
                                       { AUTO_REDUCE( yyvsp[-2], __regina_oo_operator );
                                         yyval = yyvsp[0];
                                         yyval->type = X_MSGSEND;
                                         yyval->p[0] = yyvsp[-2]; }
#line 5393 "yaccsrc.c"
    break;

  case 467: /* expr: expr CASCADE msgtarget  */
#line 1450 "yaccsrc.y"
                                       { AUTO_REDUCE( yyvsp[-2], __regina_oo_operator );
                                         yyval = yyvsp[0];
                                         yyval->type = X_CASCADE;
                                         yyval->p[0] = yyvsp[-2]; }
#line 5402 "yaccsrc.c"
    break;

  case 468: /* expr: MSGSEND  */
#line 1454 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, __regina_oo_operator ); }
#line 5408 "yaccsrc.c"
    break;

  case 469: /* expr: CASCADE  */
#line 1455 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, __regina_oo_operator ); }
#line 5414 "yaccsrc.c"
    break;

  case 470: /* expr: expr '[' bracket_args  */
#line 1456 "yaccsrc.y"
                                       { AUTO_REDUCE( yyvsp[-2], "[" );
                                         yyval = makenode( X_INDEX, 2, yyvsp[-2], yyvsp[0] ); }
#line 5421 "yaccsrc.c"
    break;

  case 471: /* expr: symbtree  */
#line 1458 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 5427 "yaccsrc.c"
    break;

  case 472: /* expr: CONSYMBOL  */
#line 1459 "yaccsrc.y"
                                       { /* OO: .NAME may be an environment
                                          * symbol, see OO_IS_ENVSYMBOL */
                                         yyval = makenode( ( retvalue[0] == '.'
                                                          && rx_isalpha( retvalue[1] )
                                                          && !__regina_oo_off() )
                                                        ? X_CON_SYMBOL : X_STRING, 0 );
                                         yyval->name = Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 5439 "yaccsrc.c"
    break;

  case 473: /* expr: HEXSTRING  */
#line 1466 "yaccsrc.y"
                                       { yyval = makenode( X_STRING, 0 );
                                         yyval->name = Str_make_TSD(parser_data.TSD,retlength) ;
                                         memcpy(yyval->name->value,retvalue,
                                                    yyval->name->len=retlength); }
#line 5448 "yaccsrc.c"
    break;

  case 474: /* expr: BINSTRING  */
#line 1470 "yaccsrc.y"
                                       { yyval = makenode( X_STRING, 0 );
                                         yyval->name = Str_make_TSD(parser_data.TSD,retlength) ;
                                         memcpy(yyval->name->value,retvalue,
                                                    yyval->name->len=retlength); }
#line 5457 "yaccsrc.c"
    break;

  case 475: /* expr: STRING  */
#line 1474 "yaccsrc.y"
                                       { yyval = makenode( X_STRING, 0 );
                                         yyval->name = Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 5464 "yaccsrc.c"
    break;

  case 476: /* expr: BLOCK  */
#line 1476 "yaccsrc.y"
                                       { /* OO: { clauses }, see oo.c */
                                         int line;
                                         yyval = makenode( X_OO_BLOCK, 0 );
                                         yyval->name = __regina_block_take( &line );
                                         yyval->lineno = line; }
#line 5474 "yaccsrc.c"
    break;

  case 477: /* expr: function  */
#line 1481 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 5480 "yaccsrc.c"
    break;

  case 478: /* expr: '+' expr  */
#line 1482 "yaccsrc.y"
                                       { AUTO_REDUCE( yyvsp[0], rxnullptr );
                                         yyval = makenode( X_U_PLUSS, 1, yyvsp[0] ); }
#line 5487 "yaccsrc.c"
    break;

  case 479: /* expr: '-' expr  */
#line 1484 "yaccsrc.y"
                                       { AUTO_REDUCE( yyvsp[0], rxnullptr );
                                         yyval = makenode( X_U_MINUS, 1, yyvsp[0] ); }
#line 5494 "yaccsrc.c"
    break;

  case 480: /* expr: '+' error  */
#line 1486 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, __reginatext ); }
#line 5500 "yaccsrc.c"
    break;

  case 481: /* expr: '-' error  */
#line 1487 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, __reginatext ); }
#line 5506 "yaccsrc.c"
    break;

  case 482: /* exprs_sub: exprs ')'  */
#line 1490 "yaccsrc.y"
                                       { yyval = yyvsp[-1]; }
#line 5512 "yaccsrc.c"
    break;

  case 483: /* exprs_sub: exprs error  */
#line 1491 "yaccsrc.y"
                                       { exiterror( ERR_UNMATCHED_PARAN, 0 ); }
#line 5518 "yaccsrc.c"
    break;

  case 484: /* exprs_sub: STATSEP  */
#line 1492 "yaccsrc.y"
                                       { exiterror( ERR_UNMATCHED_PARAN, 0 ); }
#line 5524 "yaccsrc.c"
    break;

  case 485: /* symbtree: SIMSYMBOL  */
#line 1495 "yaccsrc.y"
                                       { yyval = (nodeptr)create_head( (const char *)retvalue ) ; }
#line 5530 "yaccsrc.c"
    break;

  case 486: /* function: extfunc func_args  */
#line 1499 "yaccsrc.y"
                                       { yyval = makenode(X_EX_FUNC,1,yyvsp[0]) ;
                                         yyval->name = (streng *)yyvsp[-1] ; }
#line 5537 "yaccsrc.c"
    break;

  case 487: /* function: intfunc func_args  */
#line 1501 "yaccsrc.y"
                                       { yyval = makenode(X_IN_FUNC,1,yyvsp[0]) ;
                                         yyval->name = (streng *)yyvsp[-1] ; }
#line 5544 "yaccsrc.c"
    break;

  case 488: /* @37: %empty  */
#line 1505 "yaccsrc.y"
                                       { /* ugly fake preservs parendepth */
                                         yyval = (YYSTYPE) parendepth;
                                         parendepth = 0; }
#line 5552 "yaccsrc.c"
    break;

  case 489: /* func_args: @37 exprs_sub  */
#line 1508 "yaccsrc.y"
                                       { parendepth = (int) yyval;
                                         yyval = yyvsp[0]; }
#line 5559 "yaccsrc.c"
    break;

  case 490: /* msgtarget: msgname  */
#line 1517 "yaccsrc.y"
                                       { yyval = makenode( X_MSGSEND, 0 );
                                         yyval->name = (streng *) yyvsp[0]; }
#line 5566 "yaccsrc.c"
    break;

  case 491: /* msgtarget: msgname MSGSUPER  */
#line 1519 "yaccsrc.y"
                                       { /* ~name:super, see oo.c */
                                         yyval = makenode( X_MSGSEND, 0 );
                                         yyval->name = (streng *) yyvsp[-1];
                                         yyval->u.nonansi = 1; }
#line 5575 "yaccsrc.c"
    break;

  case 492: /* msgtarget: msgname MSGSUPER '(' func_args  */
#line 1524 "yaccsrc.y"
                                       { yyval = makenode( X_MSGSEND, 2, NULL, yyvsp[0] );
                                         yyval->name = (streng *) yyvsp[-3];
                                         yyval->u.nonansi = 1; }
#line 5583 "yaccsrc.c"
    break;

  case 493: /* msgtarget: STRING  */
#line 1527 "yaccsrc.y"
                                       { yyval = makenode( X_MSGSEND, 0 );
                                         yyval->name = Str_cre_TSD( parser_data.TSD, retvalue );
                                         mem_upper( yyval->name->value, yyval->name->len ); }
#line 5591 "yaccsrc.c"
    break;

  case 494: /* msgtarget: intfunc func_args  */
#line 1530 "yaccsrc.y"
                                       { yyval = makenode( X_MSGSEND, 2, NULL, yyvsp[0] );
                                         yyval->name = (streng *) yyvsp[-1];
                                         mem_upper( yyval->name->value, yyval->name->len ); }
#line 5599 "yaccsrc.c"
    break;

  case 495: /* msgtarget: extfunc func_args  */
#line 1533 "yaccsrc.y"
                                       { yyval = makenode( X_MSGSEND, 2, NULL, yyvsp[0] );
                                         yyval->name = (streng *) yyvsp[-1];
                                         mem_upper( yyval->name->value, yyval->name->len ); }
#line 5607 "yaccsrc.c"
    break;

  case 496: /* msgname: SIMSYMBOL  */
#line 1538 "yaccsrc.y"
                                       { yyval = (nodeptr) Str_cre_TSD( parser_data.TSD, retvalue ); }
#line 5613 "yaccsrc.c"
    break;

  case 497: /* @38: %empty  */
#line 1541 "yaccsrc.y"
                                       { /* like func_args, with "]" */
                                         yyval = (YYSTYPE) parendepth;
                                         parendepth = 0; }
#line 5621 "yaccsrc.c"
    break;

  case 498: /* bracket_args: @38 exprs ']'  */
#line 1544 "yaccsrc.y"
                                       { parendepth = (int) yyval;
                                         yyval = yyvsp[-1]; }
#line 5628 "yaccsrc.c"
    break;

  case 499: /* intfunc: INFUNCNAME  */
#line 1548 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 5634 "yaccsrc.c"
    break;

  case 500: /* extfunc: EXFUNCNAME  */
#line 1551 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 5640 "yaccsrc.c"
    break;

  case 501: /* template: pv solid template  */
#line 1554 "yaccsrc.y"
                                       { yyval =makenode(X_TPL_SOLID,3,yyvsp[-2],yyvsp[-1],yyvsp[0]);}
#line 5646 "yaccsrc.c"
    break;

  case 502: /* template: pv  */
#line 1555 "yaccsrc.y"
                                       { yyval =makenode(X_TPL_SOLID,1,yyvsp[0]) ; }
#line 5652 "yaccsrc.c"
    break;

  case 503: /* template: error  */
#line 1556 "yaccsrc.y"
                    { exiterror( ERR_INVALID_TEMPLATE, 1, __reginatext ) ;}
#line 5658 "yaccsrc.c"
    break;

  case 504: /* solid: '-' offset  */
#line 1559 "yaccsrc.y"
                                       { yyval = makenode(X_NEG_OFFS,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 5665 "yaccsrc.c"
    break;

  case 505: /* solid: '+' offset  */
#line 1561 "yaccsrc.y"
                                       { yyval = makenode(X_POS_OFFS,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 5672 "yaccsrc.c"
    break;

  case 506: /* solid: offset  */
#line 1563 "yaccsrc.y"
                                       { yyval = makenode(X_ABS_OFFS,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 5679 "yaccsrc.c"
    break;

  case 507: /* solid: '=' offset  */
#line 1565 "yaccsrc.y"
                                       { yyval = makenode(X_ABS_OFFS,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 5686 "yaccsrc.c"
    break;

  case 508: /* solid: '(' symbtree ')'  */
#line 1567 "yaccsrc.y"
                                       { yyval = makenode(X_TPL_VAR,0) ;
                                         yyval->p[0] = yyvsp[-1] ; }
#line 5693 "yaccsrc.c"
    break;

  case 509: /* solid: '-' '(' symbtree ')'  */
#line 1569 "yaccsrc.y"
                                       { yyval = makenode(X_NEG_OFFS,0) ;
                                         yyval->p[0] = yyvsp[-1] ; }
#line 5700 "yaccsrc.c"
    break;

  case 510: /* solid: '+' '(' symbtree ')'  */
#line 1571 "yaccsrc.y"
                                       { yyval = makenode(X_POS_OFFS,0) ;
                                         yyval->p[0] = yyvsp[-1] ; }
#line 5707 "yaccsrc.c"
    break;

  case 511: /* solid: '=' '(' symbtree ')'  */
#line 1573 "yaccsrc.y"
                                       { yyval = makenode(X_ABS_OFFS,0) ;
                                         yyval->p[0] = yyvsp[-1] ; }
#line 5714 "yaccsrc.c"
    break;

  case 512: /* solid: string  */
#line 1575 "yaccsrc.y"
                                       { yyval = makenode(X_TPL_MVE,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 5721 "yaccsrc.c"
    break;

  case 513: /* offset: OFFSET  */
#line 1579 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 5727 "yaccsrc.c"
    break;

  case 514: /* offset: CONSYMBOL  */
#line 1580 "yaccsrc.y"
                                       { streng *sptr = Str_cre_TSD(parser_data.TSD,retvalue) ;
                                          if (myisnumber(parser_data.TSD, sptr))
                                          {
                                             exiterror( ERR_INVALID_INTEGER, 4, sptr->value ) ;
                                          }
                                          else
                                             exiterror( ERR_INVALID_TEMPLATE, 0 ) ;}
#line 5739 "yaccsrc.c"
    break;

  case 515: /* string: STRING  */
#line 1589 "yaccsrc.y"
                                       { yyval = (nodeptr) Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 5745 "yaccsrc.c"
    break;

  case 516: /* string: HEXSTRING  */
#line 1590 "yaccsrc.y"
                                       { streng *sptr = Str_make_TSD(parser_data.TSD,retlength) ;
                                         memcpy(sptr->value,retvalue,
                                                 sptr->len=retlength) ;
                                         yyval = (nodeptr) sptr ; }
#line 5754 "yaccsrc.c"
    break;

  case 517: /* string: BINSTRING  */
#line 1594 "yaccsrc.y"
                                       { streng *sptr = Str_make_TSD(parser_data.TSD,retlength) ;
                                         memcpy(sptr->value,retvalue,
                                                 sptr->len=retlength) ;
                                         yyval = (nodeptr) sptr ; }
#line 5763 "yaccsrc.c"
    break;

  case 518: /* pv: PLACEHOLDER pv  */
#line 1600 "yaccsrc.y"
                                       { yyval = makenode(X_TPL_POINT,1,yyvsp[0]) ; }
#line 5769 "yaccsrc.c"
    break;

  case 519: /* pv: symbtree pv  */
#line 1601 "yaccsrc.y"
                                       { yyval = makenode(X_TPL_SYMBOL,2,yyvsp[0],yyvsp[-1]) ; }
#line 5775 "yaccsrc.c"
    break;

  case 520: /* pv: %empty  */
#line 1602 "yaccsrc.y"
                                       { yyval = NULL ; }
#line 5781 "yaccsrc.c"
    break;

  case 521: /* $@39: %empty  */
#line 1612 "yaccsrc.y"
                                       { /* detect
                                          * "x = ( a,. b )",
                                          * "x = ( ,. b )",
                                          * "x = a,. b",
                                          * "x = ,. b",
                                          * "x = f( ( x,. b ) )",
                                          * "x = f( ( ,. b ) )"      */
                                         if ( parendepth < 0 )
                                            exiterror( ERR_UNEXPECTED_PARAN, 1 );

                                         /* With call being the extended kind
                                          * of CALL we may have:
                                          * "x = f( (a),. b )",
                                          * "x = f( a,. b )",
                                          * "x = f( ,. b )",
                                          * "CALL s (a),. b",
                                          * "CALL s a,. b",
                                          * "CALL s ,. b",
                                          * "call s (a),. b",
                                          * "call s a,. b",
                                          * "call s ,. b",
                                          * "call s( (a),. b )",
                                          * "call s( a,. b )",
                                          * "call s( ,. b )",
                                          * "call s (a,a),. b",
                                          * "call s (a),. b",
                                          * "call s (),. b"
                                          *
                                          * detect "(a),." and transform it
                                          * to "a,."                         */
                                         yyvsp[-1] = reduce_expr_list( yyvsp[-1],
                                                                REDUCE_EXPR );

                                         /* detect "call s (a,b),. b" and
                                          * "call s (),. b", but every list on
                                          * the left side of "," is wrong, so
                                          * complain about every exprlist.   */
                                         if ( IS_EXPRLIST( yyvsp[-1] ) )
                                            exiterror( ERR_UNEXPECTED_PARAN, 1 );

                                         yyvsp[-1] = reduce_expr_list( yyvsp[-1],
                                                              REDUCE_SUBEXPR );
                                       }
#line 5829 "yaccsrc.c"
    break;

  case 522: /* exprs: nexpr ',' $@39 exprs  */
#line 1655 "yaccsrc.y"
                                       { /*
                                          * Fixes bug 961301.
                                          */
                                         nodeptr curr;

                                         assert( IS_EXPRLIST( yyvsp[0] ) );

                                         /* detect ",()." */
                                         if ( IS_EXPRLIST( yyvsp[0]->p[0] )
                                           && ( yyvsp[0]->p[1] == NULL )
                                           && ( yyvsp[0]->p[0]->p[0] == NULL ) )
                                            exiterror( ERR_UNEXPECTED_PARAN, 0 );

                                         /* detect ",(a,b)." */
                                         if ( IS_EXPRLIST( yyvsp[0]->p[0] )
                                           && ( yyvsp[0]->p[1] == NULL )
                                           && IS_EXPRLIST( yyvsp[0]->p[0]->p[1] ) )
                                            exiterror( ERR_UNEXPECTED_PARAN, 1 );

                                         /* detect ",(a)." and transform it
                                          * to ",a."                         */
                                         yyvsp[0] = reduce_expr_list( yyvsp[0],
                                                                REDUCE_RIGHT );
                                         assert( IS_EXPRLIST( yyvsp[0] ) );

                                         /* Detect something like
                                          * "call s (a,b)+1"                 */
                                         curr = yyvsp[0]->p[0];
                                         if ( ( curr != NULL )
                                           && !IS_EXPRLIST( curr )
                                           && !IS_FUNCTION( curr )
                                           && ( IS_EXPRLIST( curr->p[0] )
                                             || IS_EXPRLIST( curr->p[1] ) ) )
                                            exiterror( ERR_INVALID_EXPRESSION, 0 );

                                         yyval = makenode( X_EXPRLIST, 2, yyvsp[-3], yyvsp[0] );
                                         checkconst( yyval ); }
#line 5871 "yaccsrc.c"
    break;

  case 523: /* exprs: nexpr  */
#line 1692 "yaccsrc.y"
                                       { /* detect
                                          * "x = ()."
                                          * "x = f(().)"
                                          * "call s (().)"
                                          * "CALL s ()."                     */
                                         if ( ( parendepth < 0 ) && ( yyvsp[0] == NULL ) )
                                            exiterror( ERR_UNMATCHED_PARAN, 901, parser_data.tstart );

                                         /* With call being the extended kind
                                          * of CALL we may have:
                                          * "x = ( a. )",
                                          * "x = f( . )",
                                          * "x = f( ., )",
                                          * "x = f( a. )",
                                          * "x = f( a., )",
                                          * "x = f( a, . )",
                                          * "x = f( a, b. )",
                                          * "CALL s .",
                                          * "CALL s .,",
                                          * "CALL s a.,",
                                          * "CALL s a, .",
                                          * "CALL s a, b.",
                                          * "call s .",
                                          * "call s .,",
                                          * "call s a.,",
                                          * "call s a, .",
                                          * "call s a, b.",
                                          * "call s (a.)",
                                          * "call s (a)+1, .",
                                          * "call s (a), .",
                                          * "call s (a), a.",
                                          * "call s (a), (a).",
                                          * "call s ( ., )",
                                          * "call s ( a., )",
                                          * "call s ( a, . )",
                                          * "call s ( a, b. )"               */

                                         yyvsp[0] = reduce_expr_list( yyvsp[0],
                                                              REDUCE_SUBEXPR );
                                         yyval = makenode( X_EXPRLIST, 1, yyvsp[0] );
                                         checkconst( yyval );
                                        }
#line 5918 "yaccsrc.c"
    break;

  case 524: /* nexpr: expr  */
#line 1736 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 5924 "yaccsrc.c"
    break;

  case 525: /* nexpr: %empty  */
#line 1737 "yaccsrc.y"
                                       { yyval = NULL ; }
#line 5930 "yaccsrc.c"
    break;

  case 526: /* anyvars: xsimsymb anyvars  */
#line 1740 "yaccsrc.y"
                                        { yyval = makenode(X_SIM_SYMBOL,1,yyvsp[0]) ;
                                         yyval->name = (streng *) yyvsp[-1] ; }
#line 5937 "yaccsrc.c"
    break;

  case 527: /* anyvars: xsimsymb  */
#line 1742 "yaccsrc.y"
                                        { yyval = makenode(X_SIM_SYMBOL,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 5944 "yaccsrc.c"
    break;

  case 528: /* anyvars: '(' xsimsymb ')' anyvars  */
#line 1744 "yaccsrc.y"
                                        { yyval = makenode(X_IND_SYMBOL,1,yyvsp[0]) ;
                                         yyval->name = (streng *) yyvsp[-2] ; }
#line 5951 "yaccsrc.c"
    break;

  case 529: /* anyvars: '(' xsimsymb ')'  */
#line 1746 "yaccsrc.y"
                                        { yyval = makenode(X_IND_SYMBOL,0) ;
                                         yyval->name = (streng *) yyvsp[-1] ; }
#line 5958 "yaccsrc.c"
    break;

  case 530: /* xsimsymb: SIMSYMBOL  */
#line 1750 "yaccsrc.y"
                                       { yyval = (treenode *) Str_cre_TSD(parser_data.TSD,retvalue);}
#line 5964 "yaccsrc.c"
    break;

  case 531: /* simsymb: SIMSYMBOL  */
#line 1753 "yaccsrc.y"
                                       { yyval = (treenode *) Str_cre_TSD(parser_data.TSD,retvalue);}
#line 5970 "yaccsrc.c"
    break;

  case 532: /* simsymb: error  */
#line 1754 "yaccsrc.y"
                                       { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ) ;}
#line 5976 "yaccsrc.c"
    break;


#line 5980 "yaccsrc.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 1757 "yaccsrc.y"


static nodeptr makenode( int type, int numb, ... )
{
   nodeptr thisleave ;
   va_list argptr ;
   int i ;

   assert(numb <= (int) (sizeof(thisleave->p)/sizeof(thisleave->p[0])));
#ifdef REXXDEBUG
   printf("makenode: making new node, type: %d\n",type) ;
#endif /* REXXDEBUG */

   thisleave = FreshNode( ) ;
   /* thisleave is initialized to zero except for nodeindex */
   va_start( argptr, numb ) ;
   thisleave->type = type ;
   thisleave->lineno = -1 ;
   thisleave->charnr = -1 ;
   for (i=0;i<numb;i++)
      thisleave->p[i]=va_arg(argptr, nodeptr) ;

   va_end( argptr ) ;

   return( thisleave ) ;
}

static char *getdokeyword( int type )
{
   char *ptr;
   switch( type )
   {
      case X_DO_TO: ptr="TO";break;
      case X_DO_BY: ptr="BY";break;
      case X_DO_FOR: ptr="FOR";break;
      default: ptr="";break;
   }
   return ptr;
}

static void checkdosyntax( cnodeptr thisptr )
{
   if ((thisptr->p[1]!=NULL)&&(thisptr->p[2]!=NULL))
   {
      if ((thisptr->p[1]->type)==(thisptr->p[2]->type))
      {
         exiterror( ERR_INVALID_DO_SYNTAX, 1, getdokeyword(thisptr->p[1]->type) )  ;
      }
   }
   if ((thisptr->p[2]!=NULL)&&(thisptr->p[3]!=NULL))
   {
      if ((thisptr->p[2]->type)==(thisptr->p[3]->type))
      {
         exiterror( ERR_INVALID_DO_SYNTAX, 1, getdokeyword(thisptr->p[2]->type) )  ;
      }
   }
   if ((thisptr->p[1]!=NULL)&&(thisptr->p[3]!=NULL))
   {
      if ((thisptr->p[1]->type)==(thisptr->p[3]->type))
      {
         exiterror( ERR_INVALID_DO_SYNTAX, 1, getdokeyword(thisptr->p[1]->type) )  ;
      }
   }
   return ;
}


void newlabel( const tsd_t *TSD, internal_parser_type *ipt, nodeptr thisptr )
{
   labelboxptr newptr ;

   assert( thisptr ) ;

   newptr = (labelboxptr)MallocTSD(sizeof(labelbox)) ;

   newptr->next = NULL ;
   newptr->entry = thisptr ;
   if (ipt->first_label == NULL)
   {
      ipt->first_label = newptr ;
      ipt->last_label = newptr ; /* must be NULL, too */
   }
   else
   {
      ipt->last_label->next = newptr ;
      ipt->last_label = newptr ;
   }
   ipt->numlabels++;
}

static nodeptr create_tail( const char *name )
{
   const char *cptr ;
   nodeptr node ;
   int constant ;
   streng *tname ;
   tsd_t *TSD = parser_data.TSD;

   if (!*name)
   {
      node = makenode( X_CTAIL_SYMBOL, 0 ) ;
      node->name = Str_make_TSD( parser_data.TSD, 0) ;
      return node ;
   }

   cptr = name ;
   constant = rx_isdigit(*cptr) || *cptr=='.' || (!*cptr) ;
   node = makenode( (constant) ? X_CTAIL_SYMBOL : X_VTAIL_SYMBOL, 0 ) ;

   for (;*cptr && *cptr!='.'; cptr++) ;
   node->name = Str_ncre_TSD( parser_data.TSD, name, cptr-name ) ;

   if (*cptr)
   {
      node->p[0] = create_tail( ++cptr ) ;
      if (constant && node->p[0]->type==X_CTAIL_SYMBOL)
      {
         streng *first, *second ;
         nodeptr tptr ;

         first = node->name ;
         second = node->p[0]->name ;
         node->name = NULL;
         node->p[0]->name = NULL;
         tname = Str_makeTSD( first->len + second->len + 1) ;
         memcpy( tname->value, first->value, first->len ) ;
         tname->value[first->len] = '.' ;
         memcpy( tname->value+first->len+1, second->value, second->len) ;
         tname->len = first->len + second->len + 1 ;

         Free_stringTSD( first ) ;
         Free_stringTSD( second ) ;
         node->name = tname ;
         tptr = node->p[0] ;
         node->p[0] = tptr->p[0] ;
         RejectNode(tptr);
      }
   }

   return node ;
}

static nodeptr create_head( const char *name )
{
   const char *cptr ;
   nodeptr node ;

   /* Bypass reserved variables */
   cptr = ( *name ) ? ( name + 1 ) : name;
   for (; *cptr && *cptr!='.'; cptr++) ;
   node = makenode( X_SIM_SYMBOL, 0 ) ;
   node->name = Str_ncre_TSD( parser_data.TSD, name, cptr-name+(*cptr=='.')) ;

   if (*cptr)
   {
      if (*(++cptr))
         node->p[0] = create_tail( cptr ) ;
      else
         node->p[0] = NULL ;

      node->type = (node->p[0]) ? X_HEAD_SYMBOL : X_STEM_SYMBOL ;
   }

   return node ;
}


static node_type gettypeof( nodeptr thisptr )
{
   tsd_t *TSD = parser_data.TSD;

   switch(thisptr->type)
   {
      case X_PLUSS:
      case X_MINUS:
      case X_MULT:
      case X_U_PLUSS:
      case X_U_MINUS:
      case X_DEVIDE:
      case X_INTDIV:
      case X_MODULUS:
      case X_EQUAL:
      case X_DIFF:
      case X_GTE:
      case X_GT:
      case X_LTE:
      case X_LT:
      case X_SEQUAL:
      case X_SDIFF:
      case X_SGTE:
      case X_SGT:
      case X_SLTE:
      case X_SLT:
      case X_NEQUAL:
      case X_NDIFF:
      case X_NGTE:
      case X_NGT:
      case X_NLTE:
      case X_NLT:
         return IS_A_NUMBER ;


      case X_SIM_SYMBOL:
         return IS_SIM_SYMBOL ;

      case X_HEAD_SYMBOL:
         return IS_COMP_SYMBOL ;

      case X_STRING:
      case X_CON_SYMBOL:
      {
         if (thisptr->u.number)
         {
            fprintf( stderr, "Found an internal spot of investigation of the Regina interpreter.\n"
                             "Please inform Mark Hessling or Florian Grosse-Coosmann about the\n"
                             "circumstances and this script.\n"
                             "\n"
                             "Many thanks!\n"
                             "email addresses:\n"
                             "mark@rexx.org\n"
                             "florian@grosse-coosmann.de\n");
            /* FIXME: When does this happen?
             * It doesn't happen if no feedback is send until end of 2005.
             */
            return IS_A_NUMBER ;
         }

         if ( ( thisptr->u.number = is_a_descr( TSD, thisptr->name ) ) != NULL )
            return IS_A_NUMBER;
         return IS_NO_NUMBER;
      }
   }
   return IS_UNKNOWN ;
}



static void transform( nodeptr thisptr )
{
   int type ;
   node_type left,rght;

   left = gettypeof( thisptr->p[0] ) ;
   rght = gettypeof( thisptr->p[1] ) ;
   type = thisptr->type ;

   if ( ( left == IS_A_NUMBER ) && ( rght == IS_A_NUMBER ) )
   {
      if (type==X_EQUAL)
         thisptr->type = X_NEQUAL ;
      else if (type==X_DIFF)
         thisptr->type = X_NDIFF ;
      else if (type==X_GTE)
         thisptr->type = X_NGTE ;
      else if (type==X_GT)
         thisptr->type = X_NGT ;
      else if (type==X_LTE)
         thisptr->type = X_NLTE ;
      else if (type==X_LT)
         thisptr->type = X_NLT ;
   }
   else if ( ( left == IS_NO_NUMBER ) || ( rght == IS_NO_NUMBER ) )
   {
      if (type==X_EQUAL)
         thisptr->type = X_SEQUAL ;
      else if (type==X_DIFF)
         thisptr->type = X_SDIFF ;
      else if (type==X_GTE)
         thisptr->type = X_SGTE ;
      else if (type==X_GT)
         thisptr->type = X_SGT ;
      else if (type==X_LTE)
         thisptr->type = X_SLTE ;
      else if (type==X_LT)
         thisptr->type = X_SLT ;
   }
   else
   {
      type = thisptr->p[0]->type ;
      if ( ( left == IS_A_NUMBER )
        && ( ( type == X_STRING ) || ( type == X_CON_SYMBOL ) ) )
         thisptr->u.flags.lnum = 1 ;
      else if ( left == IS_SIM_SYMBOL )
         thisptr->u.flags.lsvar = 1 ;
      else if ( left == IS_COMP_SYMBOL )
         thisptr->u.flags.lcvar = 1 ;

      type = thisptr->p[1]->type ;
      if ( ( rght == IS_A_NUMBER )
        && ( ( type == X_STRING ) || ( type == X_CON_SYMBOL ) ) )
         thisptr->u.flags.rnum = 1 ;
      else if ( rght == IS_SIM_SYMBOL )
         thisptr->u.flags.rsvar = 1 ;
      else if ( rght == IS_COMP_SYMBOL )
         thisptr->u.flags.rcvar = 1 ;
   }
}


static int is_const( cnodeptr thisptr )
{
   if (!thisptr)
      return 1 ;

   switch (thisptr->type)
   {
      case X_CON_SYMBOL:
         /* OO: .NAME is resolved at run time */
         if ( thisptr->name && thisptr->name->len > 1
           && thisptr->name->value[0] == '.' && rx_isalpha( thisptr->name->value[1] ) )
            return 0 ;
         return 1 ;
      case X_STRING:
         return 1 ;

#if 0
      Pre-evaluation is not allowed. DIGITS and FUZZ may change within loops
      and the resulting value may or may not be the same. Concatenation with
      or without spaces is the sole allowed operation.

      NEVER ENABLE THIS AGAIN WITHOUT SOLVING THIS PROBLEMS!

      case X_U_PLUSS:
      case X_U_MINUS:
         return is_const( thisptr->p[0] ) ;

      case X_PLUSS:
      case X_MINUS:
      case X_MULT:
/*    case X_DEVIDE: Bug 20000807-41821 */
      case X_INTDIV:
      case X_MODULUS:
      case X_EQUAL:
      case X_DIFF:
      case X_GTE:
      case X_GT:
      case X_LTE:
      case X_LT:
      case X_SEQUAL:
      case X_SDIFF:
      case X_SGTE:
      case X_SGT:
      case X_SLTE:
      case X_SLT:
      case X_NEQUAL:
      case X_NDIFF:
      case X_NGTE:
      case X_NGT:
      case X_NLTE:
      case X_NLT:
#endif

      case X_SPACE:
      case X_CONCAT:
         return is_const( thisptr->p[0] ) && is_const( thisptr->p[1] ) ;
   }
   return 0 ;
}


static void checkconst( nodeptr thisptr )
{
   tsd_t *TSD = parser_data.TSD;

   assert( thisptr->type == X_EXPRLIST ) ;
   if (is_const(thisptr->p[0]))
   {
      if (thisptr->p[0])
         thisptr->u.strng = evaluate( TSD, thisptr->p[0], NULL ) ;
      else
         thisptr->u.strng = NULL ;

      thisptr->type = X_CEXPRLIST ;
   }
}

/*
 * reduce_expr_list will be invoked if the reduction of a list expression for
 * "call" arguments or a plain "(expr)" is needed. The reduction of the
 * outer parentheses of the extended CALL syntax is done with
 * mode==REDUCE_CALL, the reduction of a simple "(expr)" is done with
 * mode==REDUCE_EXPR. REDUCE_RIGHT is a synonym for REDUCE_CALL currently and
 * is intended to be used for reducing the right side of an expression list.
 *
 * REDUCE_SUBEXPR detect "call s (a)+1," and "call s 1+(a)," and reduces it.
 * Furthermore it detects "call s ()+1", "call s 1+()", "call s 1+(a,b)",
 * "call s (a,b)+1" and raises an error in this case.
 */
static nodeptr reduce_expr_list( nodeptr thisptr, reduce_mode mode )
{
   nodeptr h, retval = thisptr;

   if ( !thisptr )
      return retval;

   if ( mode == REDUCE_SUBEXPR )
   {
      if ( ( parendepth == 1 ) && !IS_FUNCTION( thisptr ) && !IS_EXPRLIST( thisptr ) )
      {
         if ( IS_EXPRLIST( thisptr->p[0] ) )
         {
            h = thisptr->p[0];
            if ( ( h->p[0] == NULL ) || ( h->p[1] != NULL ) )
               exiterror( ERR_INVALID_EXPRESSION, 0 );
            thisptr->p[0] = h->p[0];
            RejectNode( h );
         }
         if ( IS_EXPRLIST( thisptr->p[1] ) )
         {
            h = thisptr->p[1];
            if ( ( h->p[0] == NULL ) || ( h->p[1] != NULL ) )
               exiterror( ERR_INVALID_EXPRESSION, 0 );
            thisptr->p[1] = h->p[0];
            RejectNode( h );
         }
      }
      return retval;
   }

   if ( !IS_EXPRLIST( thisptr ) )
      return retval;

   if ( ( mode == REDUCE_CALL ) || ( mode == REDUCE_RIGHT ) )
   {
      if ( IS_EXPRLIST( thisptr->p[0] ) && ( thisptr->p[1] == NULL ) )
      {
         retval = thisptr->p[0];
         RejectNode( thisptr );
      }
   }
   else
   {
      /*
       * mode == REDUCE_EXPR
       */
      if ( ( thisptr->p[0] != NULL ) && ( thisptr->p[1] == NULL ) )
      {
         if ( !IS_EXPRLIST( thisptr->p[0] ) )
         {
            retval = thisptr->p[0];
            RejectNode( thisptr );
         }
      }
   }
   return retval;
}

/*
 * optgluelast connect p2 as the ->next element to p1. Every element may be
 * NULL.
 * If both are non-NULL, the connection is performed using the o.last elements.
 * Just the o.last element of p1 remains non-NULL.
 *
 * Returns: NULL if p1 and p2 are NULL.
 *          The non-NULL element if one argumet is NULL.
 *          p1 otherwise.
 */
static nodeptr optgluelast( nodeptr p1, nodeptr p2 )
{
   nodeptr p2last;

   if ( p1 == NULL )
      return p2;
   if ( p2 == NULL )
      return p1;

   /*
    * This is performed very often, so keep the code fast.
    *
    * p2last is the "o.last"-element of p2 or just p2 if p2 has no next
    * elements. We set p1's o.last further down, but we have to ensure that
    * p2->o.last is NULL first. Therefore every element in the ->next chain
    * of p1 will have NULL as its o.last field.
    */
   if ( ( p2last = p2->o.last ) == NULL )
      p2last = p2;
   else
      p2->o.last = NULL;

   if ( p1->o.last == NULL )
      p1->next = p2;
   else
      p1->o.last->next = p2;
   p1->o.last = p2last;

   return p1;
}

/*
 * justlabels returns 1, if n consists of a sequence of labels. The return
 * value is 0 otherwise.
 */
static int justlabels( nodeptr n )
{
   while ( n != NULL )
   {
      if ( n->type != X_LABEL )
         return 0;
      n = n->next;
   }

   return 1;
}

/*
 * move_labels move the labels from the end of "end" to the end of "front".
 * The labels are marked "read_only" if level is nonnull, the read-only flag
 * is removed if level is 0.
 * NOTE: At least one element of the "end" chain must contain a non-label
 * element.
 */
static void move_labels( nodeptr front, nodeptr end, int level )
{
   nodeptr oend = end;
   nodeptr labels;

   assert( front != NULL );
   assert( !justlabels( end ) );

   while ( !justlabels( end->next ) )
      end = end->next;

   if ( ( labels = end->next ) == NULL )
      return;

   /*
    * extract the labels.
    */
   labels->o.last = oend->o.last;
   end->next = NULL;
   if ( end == oend )
      oend->o.last = NULL;
   else
      oend->o.last = end;

   if ( labels->next == NULL )
      labels->o.last = NULL;

   /*
    * add the labels to the end of front and then re-mark the labels.
    */
   optgluelast( front, labels );
   while ( labels ) {
      labels->u.trace_only = ( level == 0 ) ? 0 : 1;
      labels = labels->next;
   }
}

/* sets the o.last field of the head of a statement chain, see optgluelast */
static nodeptr fix_last( nodeptr chain )
{
   nodeptr t;

   if ( chain == NULL )
      return NULL;
   for ( t = chain; t->next; t = t->next )
      t->o.last = NULL;
   t->o.last = NULL;
   chain->o.last = ( t != chain ) ? t : NULL;
   return chain;
}

static int is_transparent_directive( cnodeptr d )
{
   return d->name != NULL
       && ( ( d->name->len == 8 && memcmp( d->name->value, "REQUIRES", 8 ) == 0 )
         || ( d->name->len == 7 && memcmp( d->name->value, "OPTIONS", 7 ) == 0 ) );
}

/*
 * OO extension: ::REQUIRES and ::OPTIONS may stand anywhere, like an
 * #include in C. They never own statements: the statements after them
 * belong to what came before, the main program as long as no other
 * directive has appeared, otherwise the body of the last directive.
 * *dirs is the chain of directives; it is rebuilt with empty bodies for
 * these two. Returns the statements that continue the main program.
 */
static nodeptr split_requires( nodeptr *dirs )
{
   nodeptr d, next, main_more = NULL, owner = NULL, chain = NULL, body;

   for ( d = *dirs; d; d = next )
   {
      next = d->next;
      d->next = NULL;
      d->o.last = NULL;
      if ( is_transparent_directive( d ) && d->p[1] != NULL )
      {
         body = fix_last( d->p[1] );
         d->p[1] = NULL;
         if ( owner == NULL )
            main_more = optgluelast( main_more, body );
         else
         {
            owner->p[1] = optgluelast( fix_last( owner->p[1] ), body );
            fix_last( owner->p[1] );
            owner->p[1]->o.last = NULL;
         }
      }
      else if ( !is_transparent_directive( d ) )
         owner = d;
      chain = optgluelast( chain, d );
   }
   *dirs = chain;
   return main_more;
}

/*
 * OO extension: "DO name OVER collection" is parsed as "DO expr" with the
 * expression "name OVER collection". do_over_var returns the variable node
 * if rep has this form, else NULL.
 */
static nodeptr do_over_var( nodeptr rep )
{
   nodeptr e, l;

   if ( rep == NULL || rep->type != X_REP || rep->p[0] != NULL
     || rep->p[1] == NULL || rep->p[1]->type != X_DO_EXPR )
      return NULL;
   e = rep->p[1]->p[0];
   if ( e == NULL || e->type != X_SPACE || ( l = e->p[0] ) == NULL
     || l->type != X_SPACE || l->p[0] == NULL || l->p[1] == NULL
     || l->p[1]->type != X_SIM_SYMBOL
     || l->p[1]->name->len != 4 || memcmp( l->p[1]->name->value, "OVER", 4 ) != 0 )
      return NULL;
   if ( l->p[0]->type != X_SIM_SYMBOL && l->p[0]->type != X_HEAD_SYMBOL )
      return NULL;
   return l->p[0];
}

/* one "var = item" step of an OVER loop; mode is NULL, "INDEX" or "ITEM" */
static nodeptr over_item( const char *abuf, const char *ibuf, const char *mode )
{
   nodeptr m = NULL;

   if ( mode )
   {
      m = makenode( X_STRING, 0 );
      m->name = Str_cre_TSD( parser_data.TSD, mode );
   }
   return makenode( X_OO_OVERITEM, 3, create_head( abuf ), create_head( ibuf ), m );
}

static nodeptr over_step( nodeptr dostat, nodeptr var, nodeptr item )
{
   nodeptr asg = makenode( X_OO_OVERSTEP, 2, var, item );

   asg->lineno = dostat->lineno;
   asg->charnr = dostat->charnr;
   return asg;
}

/*
 * Rewrites the loop into
 *    DO <idx> = 1 TO <snapshot of the collection, its item count> [FOR n]
 *       name = <item idx of the snapshot>
 *       ...
 * with two hidden variables that a program cannot name.
 *
 * "DO name OVER coll": var is name, oversym the symbol OVER (for the
 * classic meaning). "DO WITH INDEX i ITEM x OVER coll": var is NULL and
 * idxvar/itemvar are set (one may be NULL); the snapshot holds pairs.
 */
static void transform_over( nodeptr dostat, nodeptr var, nodeptr coll, nodeptr oversym,
                            nodeptr idxvar, nodeptr itemvar, nodeptr fornode )
{
   static unsigned long counter = 0;
   char abuf[32], ibuf[32];
   nodeptr rep, init, over, to, asg, item, iitem = NULL, xitem = NULL, marker = NULL;
   streng *arrname, *idxname;

   counter++;
   sprintf( abuf, "\001OVERA%lu", counter );
   arrname = Str_cre_TSD( parser_data.TSD, abuf );
   sprintf( ibuf, "\001OVERI%lu", counter );
   idxname = Str_cre_TSD( parser_data.TSD, ibuf );

   init = makenode( X_STRING, 0 );
   init->name = Str_cre_TSD( parser_data.TSD, "1" );
   if ( var == NULL )
   {
      marker = makenode( X_STRING, 0 );
      marker->name = Str_cre_TSD( parser_data.TSD, "WITH" );
   }
   /*
    * p[1] and p[2] let the run time recognize the classic meaning
    * "DO expr" of "DO x OVER y" (x and OVER empty, y a whole number).
    * p[3] marks DO WITH.
    */
   over = makenode( X_OO_OVERINIT, 4, coll, var, oversym, marker );
   over->name = Str_dup_TSD( parser_data.TSD, arrname );
   Free_string_TSD( parser_data.TSD, arrname );
   to = makenode( X_DO_TO, 1, over );
   if ( fornode )
      rep = makenode( X_REP, 3, init, to, fornode );
   else
      rep = makenode( X_REP, 2, init, to );
   rep->name = idxname;

   if ( var )
   {
      item = over_item( abuf, ibuf, NULL );
      asg = over_step( dostat, var, item );
   }
   else
   {
      asg = NULL;
      if ( idxvar )
      {
         iitem = over_item( abuf, ibuf, "INDEX" );
         asg = over_step( dostat, idxvar, iitem );
      }
      if ( itemvar )
      {
         xitem = over_item( abuf, ibuf, "ITEM" );
         asg = optgluelast( asg, over_step( dostat, itemvar, xitem ) );
      }
      item = NULL;
   }

   /*
    * WHILE is tested after the control variables are set: the condition
    * assigns them itself before it is evaluated. OVERSTEP and OVERWHILE
    * share the item nodes.
    */
   if ( dostat->p[1] && dostat->p[1]->type == X_WHILE )
   {
      if ( var )
         dostat->p[1]->p[0] = makenode( X_OO_OVERWHILE, 3, var, item,
                                        dostat->p[1]->p[0] );
      else
      {
         if ( itemvar )
            dostat->p[1]->p[0] = makenode( X_OO_OVERWHILE, 3, itemvar, xitem,
                                           dostat->p[1]->p[0] );
         if ( idxvar )
            dostat->p[1]->p[0] = makenode( X_OO_OVERWHILE, 3, idxvar, iitem,
                                           dostat->p[1]->p[0] );
      }
   }

   dostat->p[0] = rep;
   dostat->p[2] = optgluelast( asg, dostat->p[2] );
   dostat->p[2]->o.last = NULL;
   /* LEAVE name and ITERATE name use the visible name */
   if ( var == NULL )
      var = itemvar ? itemvar : idxvar;
   dostat->name = Str_dup_TSD( parser_data.TSD, var->name );
}

static int same_name( const streng *a, const streng *b )
{
   return a && b && Str_cmp( a, b ) == 0;
}

/*
 * Checks the END name of a DO loop and applies the OO forms: DO name OVER,
 * DO WITH, LABEL, COUNTER and LOOP. Classic loops are checked as before.
 */
static void finish_do( nodeptr dostat, nodeptr opts )
{
   nodeptr rep = dostat->p[0], overvar;
   streng *endname = dostat->p[3]->name;
   streng *label = opts ? opts->name : NULL;
   nodeptr counter = opts ? opts->p[0] : NULL;
   int isloop = dostat->u.nonansi;

   dostat->u.nonansi = 0;
   if ( rep && rep->type == X_OO_DOWITH )
   {
      if ( endname && !same_name( endname, label )
        && !( rep->p[1] && same_name( endname, rep->p[1]->name ) )
        && !( rep->p[2] && same_name( endname, rep->p[2]->name ) ) )
         exiterror( ERR_UNMATCHED_END, 0 );
      transform_over( dostat, NULL, rep->p[0], NULL, rep->p[1], rep->p[2], rep->p[3] );
   }
   else if ( !__regina_oo_off() && ( overvar = do_over_var( rep ) ) != NULL )
   {
      /* OO: DO name OVER collection [FOR n] */
      if ( endname && !same_name( endname, label )
        && !same_name( endname, overvar->name ) )
         exiterror( ERR_UNMATCHED_END, 0 );
      transform_over( dostat, overvar, rep->p[1]->p[0]->p[1],
                      rep->p[1]->p[0]->p[0]->p[1], NULL, NULL, rep->p[2] );
   }
   else
   {
      if ( endname && !same_name( endname, label ) )
      {
         if ( rep == NULL || rep->name == NULL )
            exiterror( ERR_UNMATCHED_END, 0 );
         if ( ( endname->len != rep->name->len )
           || strncmp( endname->value, rep->name->value, rep->name->len ) )
            exiterror( ERR_UNMATCHED_END, 0 );
      }
      /*
       * LOOP without a repetitor loops forever. A plain DO group with
       * LABEL or COUNTER becomes a loop running once, so LEAVE works.
       */
      if ( rep == NULL && dostat->p[1] == NULL && isloop )
         dostat->p[0] = makenode( X_REP_FOREVER, 0 );
      else if ( rep == NULL && dostat->p[1] == NULL && ( label || counter ) )
      {
         nodeptr one = makenode( X_STRING, 0 );
         one->name = Str_cre_TSD( parser_data.TSD, "1" );
         dostat->p[0] = makenode( X_REP, 2, NULL, makenode( X_DO_EXPR, 1, one ) );
      }
   }
   if ( label )
   {
      /* the DO node owns the label now, the tree frees every name once */
      opts->name = NULL;
      if ( dostat->name )
      {
         /* OVER loop: its variable stays usable, see LEAVE in interprt.c */
         nodeptr alias = makenode( X_SIM_SYMBOL, 0 );
         alias->name = dostat->name;
         dostat->p[3]->p[1] = alias;
      }
      dostat->name = label;
   }
   if ( counter )
      dostat->p[3]->p[0] = counter;
}

