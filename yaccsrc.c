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
  YYSYMBOL_OO_IS = 119,                    /* OO_IS  */
  YYSYMBOL_OO_AS = 120,                    /* OO_AS  */
  YYSYMBOL_PLUSASSIGNMENTVARIABLE = 121,   /* PLUSASSIGNMENTVARIABLE  */
  YYSYMBOL_MINUSASSIGNMENTVARIABLE = 122,  /* MINUSASSIGNMENTVARIABLE  */
  YYSYMBOL_MULTASSIGNMENTVARIABLE = 123,   /* MULTASSIGNMENTVARIABLE  */
  YYSYMBOL_DIVASSIGNMENTVARIABLE = 124,    /* DIVASSIGNMENTVARIABLE  */
  YYSYMBOL_MODULUSASSIGNMENTVARIABLE = 125, /* MODULUSASSIGNMENTVARIABLE  */
  YYSYMBOL_INTDIVASSIGNMENTVARIABLE = 126, /* INTDIVASSIGNMENTVARIABLE  */
  YYSYMBOL_ORASSIGNMENTVARIABLE = 127,     /* ORASSIGNMENTVARIABLE  */
  YYSYMBOL_XORASSIGNMENTVARIABLE = 128,    /* XORASSIGNMENTVARIABLE  */
  YYSYMBOL_ANDASSIGNMENTVARIABLE = 129,    /* ANDASSIGNMENTVARIABLE  */
  YYSYMBOL_CONCATASSIGNMENTVARIABLE = 130, /* CONCATASSIGNMENTVARIABLE  */
  YYSYMBOL_131_ = 131,                     /* '|'  */
  YYSYMBOL_132_ = 132,                     /* '&'  */
  YYSYMBOL_133_ = 133,                     /* '='  */
  YYSYMBOL_CCAT = 134,                     /* CCAT  */
  YYSYMBOL_135_ = 135,                     /* '+'  */
  YYSYMBOL_136_ = 136,                     /* '-'  */
  YYSYMBOL_137_ = 137,                     /* '*'  */
  YYSYMBOL_138_ = 138,                     /* '/'  */
  YYSYMBOL_139_ = 139,                     /* '%'  */
  YYSYMBOL_UMINUS = 140,                   /* UMINUS  */
  YYSYMBOL_UPLUS = 141,                    /* UPLUS  */
  YYSYMBOL_142_ = 142,                     /* '['  */
  YYSYMBOL_143_ = 143,                     /* ','  */
  YYSYMBOL_144_ = 144,                     /* '('  */
  YYSYMBOL_145_ = 145,                     /* ')'  */
  YYSYMBOL_146_ = 146,                     /* ']'  */
  YYSYMBOL_YYACCEPT = 147,                 /* $accept  */
  YYSYMBOL_start = 148,                    /* start  */
  YYSYMBOL_149_1 = 149,                    /* $@1  */
  YYSYMBOL_prog = 150,                     /* prog  */
  YYSYMBOL_dirsect = 151,                  /* dirsect  */
  YYSYMBOL_dirblock = 152,                 /* dirblock  */
  YYSYMBOL_directive = 153,                /* directive  */
  YYSYMBOL_154_2 = 154,                    /* @2  */
  YYSYMBOL_dirwords = 155,                 /* dirwords  */
  YYSYMBOL_dirword = 156,                  /* dirword  */
  YYSYMBOL_stats = 157,                    /* stats  */
  YYSYMBOL_xstats = 158,                   /* xstats  */
  YYSYMBOL_ystatement = 159,               /* ystatement  */
  YYSYMBOL_lonely_end = 160,               /* lonely_end  */
  YYSYMBOL_nxstats = 161,                  /* nxstats  */
  YYSYMBOL_ncl = 162,                      /* ncl  */
  YYSYMBOL_nlncl = 163,                    /* nlncl  */
  YYSYMBOL_optLabels = 164,                /* optLabels  */
  YYSYMBOL_statement = 165,                /* statement  */
  YYSYMBOL_gruff = 166,                    /* gruff  */
  YYSYMBOL_mttstatement = 167,             /* mttstatement  */
  YYSYMBOL_mtstatement = 168,              /* mtstatement  */
  YYSYMBOL_nclstatement = 169,             /* nclstatement  */
  YYSYMBOL_oo_expose_stat = 170,           /* oo_expose_stat  */
  YYSYMBOL_oo_usearg_stat = 171,           /* oo_usearg_stat  */
  YYSYMBOL_usevars = 172,                  /* usevars  */
  YYSYMBOL_usevar = 173,                   /* usevar  */
  YYSYMBOL_call = 174,                     /* call  */
  YYSYMBOL_do = 175,                       /* do  */
  YYSYMBOL_exit = 176,                     /* exit  */
  YYSYMBOL_if = 177,                       /* if  */
  YYSYMBOL_iterate = 178,                  /* iterate  */
  YYSYMBOL_leave = 179,                    /* leave  */
  YYSYMBOL_say = 180,                      /* say  */
  YYSYMBOL_return = 181,                   /* return  */
  YYSYMBOL_address = 182,                  /* address  */
  YYSYMBOL_arg = 183,                      /* arg  */
  YYSYMBOL_drop = 184,                     /* drop  */
  YYSYMBOL_interpret = 185,                /* interpret  */
  YYSYMBOL_label = 186,                    /* label  */
  YYSYMBOL_nop = 187,                      /* nop  */
  YYSYMBOL_numeric = 188,                  /* numeric  */
  YYSYMBOL_options = 189,                  /* options  */
  YYSYMBOL_parse = 190,                    /* parse  */
  YYSYMBOL_proc = 191,                     /* proc  */
  YYSYMBOL_pull = 192,                     /* pull  */
  YYSYMBOL_push = 193,                     /* push  */
  YYSYMBOL_queue = 194,                    /* queue  */
  YYSYMBOL_select = 195,                   /* select  */
  YYSYMBOL_signal = 196,                   /* signal  */
  YYSYMBOL_when = 197,                     /* when  */
  YYSYMBOL_otherwise = 198,                /* otherwise  */
  YYSYMBOL_trace = 199,                    /* trace  */
  YYSYMBOL_upper = 200,                    /* upper  */
  YYSYMBOL_address_stat = 201,             /* address_stat  */
  YYSYMBOL_202_3 = 202,                    /* @3  */
  YYSYMBOL_address_stat2 = 203,            /* address_stat2  */
  YYSYMBOL_204_4 = 204,                    /* $@4  */
  YYSYMBOL_arg_stat = 205,                 /* arg_stat  */
  YYSYMBOL_call_stat = 206,                /* call_stat  */
  YYSYMBOL_207_5 = 207,                    /* $@5  */
  YYSYMBOL_208_6 = 208,                    /* $@6  */
  YYSYMBOL_call_name = 209,                /* call_name  */
  YYSYMBOL_call_args = 210,                /* call_args  */
  YYSYMBOL_expr_stat = 211,                /* expr_stat  */
  YYSYMBOL_end_stat = 212,                 /* end_stat  */
  YYSYMBOL_end = 213,                      /* end  */
  YYSYMBOL_do_stat = 214,                  /* do_stat  */
  YYSYMBOL_do_opts = 215,                  /* do_opts  */
  YYSYMBOL_dolabel = 216,                  /* dolabel  */
  YYSYMBOL_docounter = 217,                /* docounter  */
  YYSYMBOL_doindex = 218,                  /* doindex  */
  YYSYMBOL_doitem = 219,                   /* doitem  */
  YYSYMBOL_withvars = 220,                 /* withvars  */
  YYSYMBOL_repetitor = 221,                /* repetitor  */
  YYSYMBOL_nvir = 222,                     /* nvir  */
  YYSYMBOL_naddr_with = 223,               /* naddr_with  */
  YYSYMBOL_224_7 = 224,                    /* @7  */
  YYSYMBOL_addr_with = 225,                /* addr_with  */
  YYSYMBOL_connection = 226,               /* connection  */
  YYSYMBOL_inputstmts = 227,               /* inputstmts  */
  YYSYMBOL_outputstmts = 228,              /* outputstmts  */
  YYSYMBOL_errorstmts = 229,               /* errorstmts  */
  YYSYMBOL_adeo = 230,                     /* adeo  */
  YYSYMBOL_adei = 231,                     /* adei  */
  YYSYMBOL_adio = 232,                     /* adio  */
  YYSYMBOL_inputstmt = 233,                /* inputstmt  */
  YYSYMBOL_outputstmt = 234,               /* outputstmt  */
  YYSYMBOL_errorstmt = 235,                /* errorstmt  */
  YYSYMBOL_resourcei = 236,                /* resourcei  */
  YYSYMBOL_resourceo = 237,                /* resourceo  */
  YYSYMBOL_resources = 238,                /* resources  */
  YYSYMBOL_nsimsymb = 239,                 /* nsimsymb  */
  YYSYMBOL_240_8 = 240,                    /* $@8  */
  YYSYMBOL_nnvir = 241,                    /* nnvir  */
  YYSYMBOL_242_9 = 242,                    /* $@9  */
  YYSYMBOL_nspace = 243,                   /* nspace  */
  YYSYMBOL_addrAll = 244,                  /* addrAll  */
  YYSYMBOL_addrSim = 245,                  /* addrSim  */
  YYSYMBOL_addrString = 246,               /* addrString  */
  YYSYMBOL_dovar = 247,                    /* dovar  */
  YYSYMBOL_tobyfor = 248,                  /* tobyfor  */
  YYSYMBOL_conditional = 249,              /* conditional  */
  YYSYMBOL_drop_stat = 250,                /* drop_stat  */
  YYSYMBOL_upper_stat = 251,               /* upper_stat  */
  YYSYMBOL_exit_stat = 252,                /* exit_stat  */
  YYSYMBOL_if_stat = 253,                  /* if_stat  */
  YYSYMBOL_unexp_then = 254,               /* unexp_then  */
  YYSYMBOL_unexp_else = 255,               /* unexp_else  */
  YYSYMBOL_ipret_stat = 256,               /* ipret_stat  */
  YYSYMBOL_iterate_stat = 257,             /* iterate_stat  */
  YYSYMBOL_label_stat = 258,               /* label_stat  */
  YYSYMBOL_labelname = 259,                /* labelname  */
  YYSYMBOL_leave_stat = 260,               /* leave_stat  */
  YYSYMBOL_nop_stat = 261,                 /* nop_stat  */
  YYSYMBOL_numeric_stat = 262,             /* numeric_stat  */
  YYSYMBOL_form_expr = 263,                /* form_expr  */
  YYSYMBOL_options_stat = 264,             /* options_stat  */
  YYSYMBOL_parse_stat = 265,               /* parse_stat  */
  YYSYMBOL_parse_flags = 266,              /* parse_flags  */
  YYSYMBOL_templs = 267,                   /* templs  */
  YYSYMBOL_parse_param = 268,              /* parse_param  */
  YYSYMBOL_proc_stat = 269,                /* proc_stat  */
  YYSYMBOL_pull_stat = 270,                /* pull_stat  */
  YYSYMBOL_push_stat = 271,                /* push_stat  */
  YYSYMBOL_queue_stat = 272,               /* queue_stat  */
  YYSYMBOL_say_stat = 273,                 /* say_stat  */
  YYSYMBOL_return_stat = 274,              /* return_stat  */
  YYSYMBOL_sel_end = 275,                  /* sel_end  */
  YYSYMBOL_select_stat = 276,              /* select_stat  */
  YYSYMBOL_when_stats = 277,               /* when_stats  */
  YYSYMBOL_when_stat = 278,                /* when_stat  */
  YYSYMBOL_ex_when_stat = 279,             /* ex_when_stat  */
  YYSYMBOL_otherwise_stat = 280,           /* otherwise_stat  */
  YYSYMBOL_signal_stat = 281,              /* signal_stat  */
  YYSYMBOL_signal_name = 282,              /* signal_name  */
  YYSYMBOL_namespec = 283,                 /* namespec  */
  YYSYMBOL_asymbol = 284,                  /* asymbol  */
  YYSYMBOL_on = 285,                       /* on  */
  YYSYMBOL_off = 286,                      /* off  */
  YYSYMBOL_c_action = 287,                 /* c_action  */
  YYSYMBOL_s_action = 288,                 /* s_action  */
  YYSYMBOL_trace_stat = 289,               /* trace_stat  */
  YYSYMBOL_whatever = 290,                 /* whatever  */
  YYSYMBOL_assignment = 291,               /* assignment  */
  YYSYMBOL_ass_part = 292,                 /* ass_part  */
  YYSYMBOL_plusassignment = 293,           /* plusassignment  */
  YYSYMBOL_plus_ass_part = 294,            /* plus_ass_part  */
  YYSYMBOL_minusassignment = 295,          /* minusassignment  */
  YYSYMBOL_minus_ass_part = 296,           /* minus_ass_part  */
  YYSYMBOL_multassignment = 297,           /* multassignment  */
  YYSYMBOL_mult_ass_part = 298,            /* mult_ass_part  */
  YYSYMBOL_divassignment = 299,            /* divassignment  */
  YYSYMBOL_div_ass_part = 300,             /* div_ass_part  */
  YYSYMBOL_intdivassignment = 301,         /* intdivassignment  */
  YYSYMBOL_intdiv_ass_part = 302,          /* intdiv_ass_part  */
  YYSYMBOL_modulusassignment = 303,        /* modulusassignment  */
  YYSYMBOL_modulus_ass_part = 304,         /* modulus_ass_part  */
  YYSYMBOL_andassignment = 305,            /* andassignment  */
  YYSYMBOL_and_ass_part = 306,             /* and_ass_part  */
  YYSYMBOL_xorassignment = 307,            /* xorassignment  */
  YYSYMBOL_xor_ass_part = 308,             /* xor_ass_part  */
  YYSYMBOL_orassignment = 309,             /* orassignment  */
  YYSYMBOL_or_ass_part = 310,              /* or_ass_part  */
  YYSYMBOL_concatassignment = 311,         /* concatassignment  */
  YYSYMBOL_concat_ass_part = 312,          /* concat_ass_part  */
  YYSYMBOL_expr = 313,                     /* expr  */
  YYSYMBOL_314_10 = 314,                   /* $@10  */
  YYSYMBOL_315_11 = 315,                   /* $@11  */
  YYSYMBOL_316_12 = 316,                   /* $@12  */
  YYSYMBOL_317_13 = 317,                   /* $@13  */
  YYSYMBOL_318_14 = 318,                   /* $@14  */
  YYSYMBOL_319_15 = 319,                   /* $@15  */
  YYSYMBOL_320_16 = 320,                   /* $@16  */
  YYSYMBOL_321_17 = 321,                   /* $@17  */
  YYSYMBOL_322_18 = 322,                   /* $@18  */
  YYSYMBOL_323_19 = 323,                   /* $@19  */
  YYSYMBOL_324_20 = 324,                   /* $@20  */
  YYSYMBOL_325_21 = 325,                   /* $@21  */
  YYSYMBOL_326_22 = 326,                   /* $@22  */
  YYSYMBOL_327_23 = 327,                   /* $@23  */
  YYSYMBOL_328_24 = 328,                   /* $@24  */
  YYSYMBOL_329_25 = 329,                   /* $@25  */
  YYSYMBOL_330_26 = 330,                   /* $@26  */
  YYSYMBOL_331_27 = 331,                   /* $@27  */
  YYSYMBOL_332_28 = 332,                   /* $@28  */
  YYSYMBOL_333_29 = 333,                   /* $@29  */
  YYSYMBOL_334_30 = 334,                   /* $@30  */
  YYSYMBOL_335_31 = 335,                   /* $@31  */
  YYSYMBOL_336_32 = 336,                   /* $@32  */
  YYSYMBOL_337_33 = 337,                   /* $@33  */
  YYSYMBOL_338_34 = 338,                   /* $@34  */
  YYSYMBOL_339_35 = 339,                   /* $@35  */
  YYSYMBOL_340_36 = 340,                   /* $@36  */
  YYSYMBOL_341_37 = 341,                   /* $@37  */
  YYSYMBOL_342_38 = 342,                   /* $@38  */
  YYSYMBOL_exprs_sub = 343,                /* exprs_sub  */
  YYSYMBOL_symbtree = 344,                 /* symbtree  */
  YYSYMBOL_function = 345,                 /* function  */
  YYSYMBOL_func_args = 346,                /* func_args  */
  YYSYMBOL_347_39 = 347,                   /* @39  */
  YYSYMBOL_msgtarget = 348,                /* msgtarget  */
  YYSYMBOL_msgname = 349,                  /* msgname  */
  YYSYMBOL_bracket_args = 350,             /* bracket_args  */
  YYSYMBOL_351_40 = 351,                   /* @40  */
  YYSYMBOL_intfunc = 352,                  /* intfunc  */
  YYSYMBOL_extfunc = 353,                  /* extfunc  */
  YYSYMBOL_template = 354,                 /* template  */
  YYSYMBOL_solid = 355,                    /* solid  */
  YYSYMBOL_offset = 356,                   /* offset  */
  YYSYMBOL_string = 357,                   /* string  */
  YYSYMBOL_pv = 358,                       /* pv  */
  YYSYMBOL_exprs = 359,                    /* exprs  */
  YYSYMBOL_360_41 = 360,                   /* $@41  */
  YYSYMBOL_nexpr = 361,                    /* nexpr  */
  YYSYMBOL_anyvars = 362,                  /* anyvars  */
  YYSYMBOL_xsimsymb = 363,                 /* xsimsymb  */
  YYSYMBOL_simsymb = 364                   /* simsymb  */
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
#define YYLAST   2887

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  147
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  218
/* YYNRULES -- Number of rules.  */
#define YYNRULES  537
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  735

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   388


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
       2,     2,     2,     2,     2,     2,     2,   139,   132,     2,
     144,   145,   137,   135,   143,   136,     2,   138,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   133,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   142,     2,   146,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   131,     2,     2,     2,     2,     2,
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
     125,   126,   127,   128,   129,   130,   134,   140,   141
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
     347,   350,   353,   356,   359,   361,   364,   367,   369,   373,
     377,   384,   388,   393,   397,   401,   405,   409,   413,   417,
     421,   425,   429,   433,   437,   441,   445,   449,   453,   457,
     461,   466,   470,   474,   478,   482,   486,   486,   490,   493,
     494,   495,   495,   497,   501,   507,   513,   513,   518,   518,
     524,   525,   526,   527,   528,   534,   539,   540,   546,   547,
     550,   558,   561,   583,   589,   591,   592,   595,   607,   608,
     612,   618,   620,   622,   624,   628,   629,   630,   631,   634,
     638,   642,   646,   649,   650,   651,   654,   660,   662,   665,
     668,   669,   670,   673,   673,   679,   682,   683,   684,   687,
     688,   689,   690,   693,   697,   701,   705,   706,   707,   708,
     709,   712,   713,   714,   715,   716,   719,   720,   721,   722,
     723,   726,   727,   730,   731,   734,   735,   738,   739,   741,
     742,   747,   748,   750,   751,   752,   753,   758,   766,   767,
     780,   781,   785,   786,   790,   793,   793,   797,   797,   801,
     802,   805,   806,   809,   817,   825,   828,   829,   830,   833,
     834,   835,   838,   839,   843,   844,   848,   852,   860,   870,
     872,   874,   875,   878,   881,   884,   889,   891,   892,   895,
     903,   907,   909,   910,   913,   914,   917,   920,   921,   922,
     925,   927,   929,   930,   931,   934,   937,   938,   941,   944,
     950,   956,   957,   960,   962,   964,   966,   968,   970,   972,
     976,   978,   981,   982,   983,   984,   985,   986,   987,   989,
     990,   993,   994,   995,   996,   997,  1001,  1005,  1009,  1013,
    1017,  1025,  1026,  1027,  1030,  1035,  1036,  1038,  1039,  1040,
    1044,  1045,  1046,  1049,  1055,  1057,  1058,  1068,  1069,  1072,
    1076,  1082,  1085,  1086,  1088,  1089,  1090,  1091,  1092,  1098,
    1102,  1103,  1109,  1110,  1111,  1114,  1115,  1116,  1119,  1120,
    1123,  1126,  1129,  1130,  1131,  1132,  1135,  1136,  1137,  1138,
    1141,  1143,  1145,  1146,  1150,  1154,  1170,  1176,  1183,  1189,
    1196,  1202,  1209,  1215,  1222,  1228,  1235,  1241,  1248,  1254,
    1261,  1267,  1274,  1280,  1287,  1293,  1298,  1305,  1305,  1338,
    1338,  1341,  1341,  1344,  1344,  1347,  1348,  1348,  1351,  1352,
    1352,  1355,  1356,  1356,  1359,  1360,  1360,  1363,  1364,  1364,
    1367,  1368,  1368,  1371,  1372,  1372,  1375,  1376,  1376,  1379,
    1380,  1380,  1383,  1384,  1386,  1387,  1387,  1391,  1391,  1395,
    1395,  1399,  1400,  1400,  1404,  1405,  1405,  1409,  1410,  1410,
    1414,  1415,  1415,  1419,  1420,  1420,  1424,  1425,  1425,  1428,
    1429,  1429,  1432,  1433,  1433,  1436,  1437,  1437,  1440,  1441,
    1441,  1444,  1445,  1445,  1448,  1449,  1449,  1452,  1453,  1453,
    1456,  1457,  1461,  1465,  1466,  1467,  1469,  1470,  1477,  1481,
    1485,  1487,  1492,  1493,  1495,  1497,  1498,  1501,  1502,  1503,
    1506,  1510,  1512,  1516,  1516,  1528,  1530,  1534,  1538,  1541,
    1544,  1549,  1552,  1552,  1559,  1562,  1565,  1566,  1567,  1570,
    1572,  1574,  1576,  1578,  1580,  1582,  1584,  1586,  1590,  1591,
    1600,  1601,  1605,  1611,  1612,  1613,  1623,  1623,  1703,  1747,
    1748,  1751,  1753,  1755,  1757,  1761,  1764,  1765
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
  "CASCADE", "DCOLON", "USEARG", "MSGSUPER", "BLOCK", "OO_IS", "OO_AS",
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
  "$@31", "$@32", "$@33", "$@34", "$@35", "$@36", "$@37", "$@38",
  "exprs_sub", "symbtree", "function", "func_args", "@39", "msgtarget",
  "msgname", "bracket_args", "@40", "intfunc", "extfunc", "template",
  "solid", "offset", "string", "pv", "exprs", "$@41", "nexpr", "anyvars",
  "xsimsymb", "simsymb", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-624)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-531)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -624,    62,  -624,  -624,  -624,    34,     7,   116,     1,  -624,
      35,    36,  -624,  -624,  -624,  1487,  -624,  -624,  -624,  -624,
      25,  -624,  -624,  -624,  -624,   304,  -624,    38,     1,  -624,
    -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,
    -624,  -624,  -624,    22,  -624,  -624,  -624,  -624,  -624,  -624,
    -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,   233,  -624,
    -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,
    -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,
    -624,  -624,  -624,  -624,  -624,  -624,  -624,    84,  -624,  -624,
    -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,
    -624,  -624,  1170,  1260,  -624,  -624,  -624,  -624,    70,  -624,
      60,  -624,  -624,   458,  -624,  2305,  2032,   181,   282,  2305,
    2305,  -624,    64,   -40,  2305,   177,   361,  2305,   197,   237,
     112,  2305,  2305,   179,   593,  -624,  -624,  1851,   -40,  -624,
    -624,  -624,  -624,   300,    60,  -624,  -624,  -624,  -624,  -624,
    -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,
    -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  2305,
    -624,  2305,  -624,  2305,  -624,  2305,  -624,  2305,  -624,  2305,
    -624,  2305,  -624,  2305,  -624,  2305,  -624,  2305,  -624,  2305,
    2484,  -624,  -624,  -624,  -624,    94,  -624,  -624,  -624,  -624,
       0,  -624,  -624,  -624,    84,  -624,   -40,   -42,   222,    52,
      73,  -624,   -42,  -624,   -42,  2124,  -624,  -624,    25,  -624,
    -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,   161,
     216,  -624,  2215,  2484,  -624,    25,  2484,  -624,  -624,   223,
     272,  -624,  -624,   138,  -624,     9,    59,     9,  -624,   439,
     348,  2484,  -624,  -624,  2305,   459,  2305,  -624,  -624,  -624,
    -624,  -624,    50,   492,  -624,   120,  -624,  -624,   129,   -25,
     741,    64,  -624,    26,  -624,  -624,  -624,  -624,    42,  -624,
    2305,  -624,   349,  -624,   283,   286,  2305,  -624,   354,  -624,
     367,   373,    25,  -624,  -624,  -624,  -624,  -624,  -624,  -624,
    -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,
    -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,
    -624,  -624,   596,   596,  -624,  -624,  -624,  -624,  -624,  -624,
    -624,  -624,  -624,  -624,  -624,  -624,  2124,  -624,    94,   304,
      25,    83,  -624,  -624,    84,  2305,  -624,  -624,    10,   103,
    2305,  -624,  -624,  -624,  -624,  -624,    32,  -624,   394,  2305,
     495,  -624,  -624,  -624,   144,  -624,  -624,   127,   131,  2571,
      46,  -624,  -624,  -624,  2305,   191,  -624,  -624,  -624,  2305,
    -624,  2305,  -624,  -624,    64,  -624,  -624,  -624,   -26,   -13,
      -1,   107,    64,  -624,  -624,  -624,  2484,  -624,  2305,  -624,
    -624,   415,  2484,  -624,  -624,   234,  -624,  -624,  -624,  -624,
    -624,    64,    59,  -624,   471,  -624,  -624,   267,  1350,   289,
     263,  -624,  2484,  -624,  -624,  -624,  -624,  -624,  -624,   214,
    -624,   474,  2484,  -624,  -624,  -624,  2305,  2305,  2305,  2305,
    2305,  2305,  2305,  2305,  2305,  2305,  2305,  2305,  2305,  2305,
    2305,  2305,  2305,  2305,  -624,  -624,  -624,   180,  -624,  -624,
    -624,  2305,  2305,  2305,  2305,  2305,  2305,  2305,  2305,  2305,
    2305,  -624,  2305,  -624,  -624,   -40,  -624,  2484,  -624,  -624,
    -624,  -624,   168,  -624,    68,   476,  -624,  -624,  -624,  -624,
    -624,  -624,  -624,   295,   287,   278,  2305,  2305,    60,  2305,
     637,   381,  -624,  -624,   345,   642,  -624,   477,  -624,  -624,
    -624,   334,   334,   334,   399,  2394,   345,  -624,   107,  -624,
     107,  -624,   107,  -624,   265,  -624,  2484,  -624,  -624,    59,
    -624,  -624,  -624,  1940,  -624,   796,  -624,   369,  -624,   480,
    -624,   332,    91,   352,   352,   352,   352,   352,   352,   352,
     332,   -42,  2658,   352,   352,   352,   352,   352,   352,   273,
    -624,  -624,   352,   352,  2658,  2745,   352,   500,   500,    91,
      91,    91,   274,  -624,  2305,  -624,  -624,  -624,  -624,  -624,
    -624,  -624,  2305,  2571,  2571,    25,  2571,  2305,   938,  -624,
     436,  -624,  -624,  -624,   334,   334,   153,  -624,   334,   334,
     279,  -624,   334,   334,   306,    17,   229,   236,   334,  -624,
     296,   328,   331,  -624,   465,  -624,  -624,   311,  -624,  -624,
    -624,  -624,  -624,  2571,  -624,  -624,   447,   450,  -624,  1624,
     532,  2571,  -624,   488,  -624,   334,   406,   334,   410,   334,
     406,   334,   414,   334,   410,   334,   414,  -624,   489,  -624,
     554,  -624,   554,   345,  -624,  -624,  -624,  -624,  -624,   485,
    -624,   522,  -624,  1761,  -624,  -624,  2305,  2305,  2305,   532,
    -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,   314,
      49,   443,   460,    67,  -624,  -624,  -624,   105,   143,  -624,
    -624,  -624,  -624,  -624,  -624,  2305,  2571,  2571,  2571,   532,
    1080,  -624,  -624,   334,  -624,  -624,   334,  -624,  -624,  -624,
    -624,  -624,  -624,  -624,  -624,  -624,  -624,   520,  2571,  -624,
    -624,  -624,  -624,  -624,  -624,   280,    84,  -624,  -624,  -624,
    -624,  -624,  -624,  -624,  -624
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       2,     0,    34,     1,     3,    37,    32,     0,     7,     9,
      37,    37,    21,    25,    24,    37,    35,    36,    34,   101,
      31,   260,    33,   259,    14,     0,     8,    37,     6,    20,
      97,    98,    89,    90,    91,    92,    93,   100,    94,   102,
     103,   105,   106,     0,   107,   108,   109,    95,    96,   110,
     112,    99,   113,   111,   114,   143,   115,   366,   424,   422,
     401,   434,   440,   437,   443,   446,   449,   452,   419,   416,
     413,   477,   490,   505,   504,   478,   480,   479,   104,   455,
     458,   461,   464,   467,   470,   473,   474,    88,   481,   368,
     370,   372,   374,   378,   376,   384,   382,   380,   386,   407,
     410,   431,     0,     0,   395,   398,   404,   387,     0,    38,
       0,    78,    79,     0,   148,   530,     0,     0,     0,   530,
     530,   116,     0,     0,     0,     0,     0,   530,     0,     0,
       0,   530,   530,     0,     0,   327,   328,   530,     0,    43,
      45,    46,    44,     0,     0,    47,    48,    66,    49,    40,
      41,    42,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    62,    61,    63,    64,    65,    77,   530,
      67,   530,    68,   530,    69,   530,    70,   530,    71,   530,
      72,   530,    75,   530,    73,   530,    74,   530,    76,   530,
     142,   476,   482,   493,   493,    30,    34,    18,    17,    19,
       0,    16,    81,   535,     0,    80,   532,   423,     0,    85,
      87,   485,   483,   486,   484,   530,   253,   254,    39,   139,
     350,   351,   348,   349,   521,   520,   522,   126,   138,     0,
       0,   128,   169,   529,   246,   251,    34,   537,   536,     0,
       0,   309,   310,     0,   508,   525,   125,   525,   291,   507,
       0,   255,   265,   275,   267,     0,   273,   278,   282,   292,
     294,   297,     0,     0,   296,   283,   295,   293,   286,   289,
       0,     0,   302,     0,   306,   307,   308,   317,     0,   344,
       0,   343,     0,   342,     0,     0,     0,   364,     0,   361,
       0,     0,    26,   365,   367,   369,   371,   373,   375,   377,
     379,   381,   383,   385,   420,   399,   432,   438,   435,   441,
     444,   447,   450,   417,   414,   411,   453,   456,   459,   462,
     465,   468,     0,     0,   425,   427,   405,   408,   429,   389,
     391,   393,   396,   402,   502,   492,   530,   491,    29,     0,
      13,     0,   531,    83,    88,     0,   489,   388,     0,   528,
     530,   130,   353,   354,   355,   352,     0,   131,     0,   530,
       0,   235,   151,   152,     0,   149,   150,   241,     0,   230,
       0,   257,   262,   121,     0,     0,   170,   171,   172,     0,
     117,   530,   119,   523,     0,   524,   518,   519,     0,     0,
       0,     0,     0,   511,   517,   242,   266,   272,     0,   276,
     277,     0,   274,   298,   300,     0,   284,   287,   285,   288,
     281,     0,   280,   303,     0,   322,   318,     0,     0,     0,
     330,   321,   331,   332,   334,   358,   357,   359,   356,     0,
     335,     0,   360,   362,   244,   146,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   501,   498,   471,   495,   493,   493,
     472,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   475,   530,   494,    15,   534,    84,    86,   488,   487,
     526,   127,   140,   132,     0,     0,   136,   129,   164,   229,
     163,   153,   154,   155,   156,     0,     0,     0,     0,     0,
     417,   165,   252,    34,   173,   173,   182,     0,   179,   180,
     181,   230,   230,   230,   178,     0,   173,   290,     0,   512,
       0,   510,     0,   509,     0,   506,   271,   268,   299,   279,
     304,   315,   326,    34,   316,     0,   320,     0,   336,     0,
     340,   421,   400,   433,   439,   436,   442,   445,   448,   451,
     418,   415,   412,   454,   457,   460,   463,   466,   469,   496,
     499,   500,   426,   428,   406,   409,   430,   390,   392,   394,
     397,   403,     0,   533,   530,   141,   347,   345,   346,   133,
     157,   158,     0,   230,   230,    37,   230,     0,     0,   122,
       0,   118,   177,   183,   230,   230,   190,   184,   230,   230,
     195,   185,   230,   230,   200,     0,     0,     0,   230,   123,
       0,     0,     0,   513,     0,   319,    37,     0,   314,   337,
     493,   503,   527,   230,   239,   240,    37,     0,    37,    37,
     162,   230,   250,   247,   174,   230,   186,   230,   188,   230,
     191,   230,   193,   230,   196,   230,   198,   202,     0,   204,
       0,   206,     0,   173,   516,   515,   514,    34,   329,     0,
     497,   167,    22,    37,   147,    23,     0,     0,     0,   161,
     166,    34,   187,   189,   192,   194,   197,   199,   210,     0,
       0,     0,     0,     0,   201,   207,   216,     0,     0,   203,
     211,   205,   124,    37,   312,     0,   230,   230,   230,   160,
       0,   218,   217,   230,   220,   219,   230,   222,   221,   224,
     223,   209,   208,   213,   212,   215,   214,   323,   230,   236,
     238,   237,   159,   249,   248,     0,     0,   324,   168,   228,
     231,   232,   234,   233,   226
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -624,  -624,  -624,  -624,   535,   163,  -624,  -624,  -624,   232,
     552,  -624,   -11,  -624,   -37,  -104,  -214,    -3,  -559,    -7,
    -624,  -624,  -624,  -624,  -624,   249,  -624,  -624,  -624,  -624,
    -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,
    -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,
    -253,  -248,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,
    -624,  -624,  -624,   252,  -624,  -624,   -15,  -624,  -624,  -624,
    -624,   122,   124,  -624,  -624,  -624,  -485,  -624,    30,  -624,
    -624,  -624,  -624,  -624,  -624,  -624,  -438,  -450,  -231,  -624,
     -18,  -103,  -624,  -624,  -359,  -624,    89,  -624,  -100,  -624,
    -624,  -623,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,
    -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,
    -254,   365,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,
    -624,   230,  -624,  -624,  -624,  -624,   226,   509,   517,   533,
     374,   395,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,
    -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,
    -624,  -624,  -624,  -624,  -624,  -624,   -14,  -624,  -624,  -624,
    -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,
    -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,  -624,
    -624,  -624,  -624,  -624,  -624,  -624,   330,  -117,  -624,  -180,
    -624,   356,  -624,  -624,  -624,   259,   285,  -123,  -624,   288,
    -245,    69,  -340,  -624,   -78,  -114,   -85,  -115
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,     2,     4,     8,     9,    10,    25,   200,   201,
      11,   626,    12,    13,   627,    20,     5,     6,    14,    15,
      16,   109,   110,   111,   112,   208,   209,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,    21,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   243,   380,   504,   140,   141,
     350,   359,   227,   481,   142,   143,   144,   145,   232,   365,
     366,   493,   494,   495,   367,   381,   589,   590,   382,   507,
     508,   509,   510,   593,   597,   601,   511,   512,   513,   684,
     689,   690,   705,   706,   702,   703,   490,   729,   730,   731,
     368,   669,   498,   146,   147,   148,   149,   150,   151,   152,
     153,    22,    23,   154,   155,   156,   401,   157,   158,   270,
     246,   271,   159,   160,   161,   162,   163,   164,   618,   165,
     420,   421,    17,   537,   166,   282,   485,   228,   229,   230,
     428,   429,   167,   288,   168,   169,   170,   171,   172,   173,
     174,   175,   176,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   188,   189,   233,   215,   466,   467,
     468,   469,   437,   470,   463,   464,   447,   446,   445,   436,
     461,   462,   465,   438,   440,   439,   441,   442,   443,   444,
     448,   449,   450,   451,   452,   453,   347,   191,   192,   335,
     336,   456,   457,   471,   472,   193,   194,   248,   392,   393,
     231,   249,   348,   574,   349,   205,   206,   239
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      29,   190,   210,   240,   394,   247,   218,   274,   108,   250,
     482,   478,   235,   247,   337,   195,    29,   412,   647,   482,
     591,   408,   370,   202,   290,   418,   628,   413,   291,   278,
     419,   609,   203,   483,    -5,   -11,    -4,   234,   -10,   386,
     292,   241,   242,   415,   207,   387,   699,   502,    18,   257,
     704,   237,   386,   275,   276,    18,   416,   628,   387,   289,
     503,   594,     3,   603,   386,   244,   339,   662,   711,   576,
     387,   322,   323,   196,   598,   602,   722,    50,   245,    52,
    -135,    72,    19,   489,   216,   217,   409,   417,   212,   214,
     196,   293,   342,   294,   203,   295,   340,   296,   203,   297,
     334,   298,   236,   299,   204,   300,   713,   301,    18,   302,
     251,   303,  -525,   244,   484,  -225,     7,    24,   518,   341,
    -230,  -225,   238,  -230,  -230,  -230,  -230,  -230,   247,  -525,
     247,   520,   572,   245,   717,  -525,    72,   496,   497,   373,
     577,  -525,  -525,   522,   715,   637,   578,   403,  -525,     7,
     -11,     7,   643,   -10,   247,   479,   203,   529,   314,   414,
    -525,   641,   351,   374,   375,   645,   204,   418,   692,    19,
     204,    26,   535,   679,   680,   681,   682,  -525,   252,    72,
     277,   245,   237,  -525,    72,   405,  -120,   -12,   -12,  -525,
    -525,    26,   506,   338,   -12,   344,  -525,  -525,   258,  -525,
    -525,   259,   384,   352,   322,   323,   345,  -525,  -525,   376,
     377,   679,   680,   681,   682,   538,   378,   357,   369,   260,
     261,   262,   263,   343,   371,  -264,   264,    18,   475,  -258,
     649,   353,   406,   334,   622,   491,   492,   651,   272,  -230,
     396,   407,   402,   265,   354,  -525,   480,  -525,  -525,   679,
     680,   681,   682,   238,   606,   607,  -525,   489,   352,   210,
     528,   517,  -339,   355,   499,   273,   422,   247,   531,   525,
     -82,  -256,   432,   372,   524,   247,   266,   267,   560,   561,
     595,   599,   379,   237,   424,  -301,   353,   430,    58,   588,
     534,  -230,  -230,  -230,   247,   489,   484,   559,    50,   354,
      52,   237,   489,   516,    71,    72,    73,    74,   268,   269,
      75,    76,   237,   575,   383,   701,   385,    77,   355,   614,
    -261,   616,   708,   710,   425,   352,   426,   425,   352,   426,
    -263,   477,  -230,  -230,  -230,  -230,  -230,  -230,  -230,  -230,
    -230,  -230,  -230,  -230,  -230,  -230,    85,    86,  -145,   395,
     423,    88,   203,   353,   238,   433,   353,   224,   225,  -313,
     505,   573,   253,   635,   226,   515,   354,   639,   434,   354,
     427,   582,   238,   427,   435,   197,   198,   107,   491,   605,
    -227,   607,   199,   238,   526,   355,  -227,   492,   355,   305,
     587,  -227,  -227,  -175,   585,   486,  -243,  -333,  -227,   314,
     489,   610,  -363,   611,   533,   612,   605,   606,   304,   305,
     613,   254,   255,   256,   617,  -245,   527,   620,   313,   314,
     621,  -144,   541,   542,   543,   544,   545,   546,   547,   548,
     549,   550,   551,   552,   553,   554,   555,   556,   557,   558,
     660,   654,  -137,   693,   707,   322,   323,   562,   563,   564,
     565,   566,   567,   568,   569,   570,   571,   700,   501,   219,
     397,   709,   375,  -269,   514,   322,   323,   329,   330,   331,
     332,   333,   530,   655,   334,   540,   656,   579,   592,   657,
     732,   619,   583,   584,   398,   586,   694,   329,   330,   331,
     332,   333,   -27,   404,   334,    55,   488,   220,   221,   605,
     606,   607,   659,   671,   386,  -230,  -230,  -270,   607,  -227,
     387,   606,   399,   400,   605,  -227,   224,   225,  -530,  -305,
    -227,  -227,  -341,   226,  -134,  -176,  -227,  -227,  -338,   222,
     223,   695,  -227,  -311,   727,   224,   225,  -227,  -227,   666,
     667,   668,   226,  -230,  -227,   685,    28,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,   305,    68,    69,
      70,   489,    27,    71,    72,    73,    74,   314,   623,    75,
      76,   474,   388,   631,   389,   390,    77,   633,   629,   658,
     712,   458,   458,   391,   714,   716,    79,    80,    81,    82,
      83,    84,   678,   476,   279,   679,   680,   681,   682,   683,
     596,   600,   604,   356,   358,    85,    86,   459,   459,   629,
      88,   487,   664,   322,   323,   190,   581,   580,   280,   663,
     634,   665,   108,    99,   100,   101,   734,   102,   103,   104,
     105,   106,   220,   221,   691,   411,   107,   331,   332,   333,
     733,   733,   334,   283,  -229,  -229,  -229,  -229,  -229,   190,
     536,   284,   696,   697,   698,   539,   108,   686,   687,   688,
     679,   680,   681,   682,   222,   223,   473,   285,   454,    73,
      74,   281,   624,   625,   455,   630,   519,   521,   523,   460,
     431,   718,     0,   636,   638,  -229,   663,   640,   642,   724,
    -175,   644,   646,     0,   648,   650,   652,   653,   304,   305,
     306,   307,   308,   309,   310,   311,   312,     0,   313,   314,
     315,     0,   661,     0,     0,     0,     0,     0,     0,     0,
     670,     0,     0,     0,   672,     0,   673,     0,   674,     0,
     675,     0,   676,     0,   677,     0,   316,   317,   318,   319,
     320,   321,   410,     0,     0,   259,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   322,   323,     0,     0,     0,
       0,   324,   325,   260,   261,   262,   263,     0,     0,     0,
     264,     0,     0,   326,   327,   328,     0,   329,   330,   331,
     332,   333,     0,     0,   334,   719,   720,   721,     0,     0,
       0,     0,   725,     0,     0,   726,     0,   615,     0,   -34,
     -34,   -34,   -34,     0,     0,     0,     0,   728,   -34,   -34,
     -34,   -34,   -34,   -34,   -34,   -34,   -34,   -34,     0,     0,
     266,   267,     0,   -34,   -34,   -34,   -34,   -34,   -34,   -34,
     -34,   -34,   -34,   -34,   -34,     0,     0,     0,     0,     0,
     -34,   -34,   -34,   -34,   -34,     0,     0,     0,     0,     0,
       0,   -34,   -34,   -34,   -34,   -34,   -34,   -34,   -34,   -34,
     -34,     0,   -34,   -34,   -34,     0,     0,   -34,   -34,   -34,
     -34,   -34,     0,   -34,   -34,     0,     0,     0,     0,     0,
     -34,   -34,     0,     0,     0,     0,     0,     0,     0,     0,
     -34,   -34,   -34,   -34,   -34,   -34,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   -34,
     -34,     0,   -34,     0,   -34,     0,     0,   -34,   -34,   -34,
     -34,   -34,   -34,   -34,   -34,   -34,   -34,   -34,   -34,   -34,
       0,   -34,   -34,   -34,   -34,   -34,     0,     0,     0,   632,
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
       0,   -37,   -37,     0,   -37,     0,   -37,     0,     0,   -37,
     -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,
     -37,   -37,     0,   -37,   -37,   -37,   -37,   -37,     0,     0,
       0,   723,   -37,   -37,   -37,   -37,   -37,     0,     0,     0,
       0,     0,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,
     -37,   -37,     0,     0,     0,     0,     0,   -37,   -37,   -37,
     -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,     0,
       0,     0,     0,     0,   -37,   -37,   -37,   -37,     0,     0,
       0,     0,     0,     0,     0,   -37,   -37,   -37,   -37,   -37,
     -37,   -37,   -37,   -37,   -37,     0,   -37,   -37,   -37,     0,
       0,   -37,   -37,   -37,   -37,     0,     0,   -37,   -37,     0,
       0,     0,     0,     0,   -37,   -37,     0,     0,     0,     0,
       0,   211,     0,     0,   -37,   -37,   -37,   -37,   -37,   -37,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   -37,   -37,     0,   -37,     0,   -37,     0,
       0,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,
     -37,   -37,   -37,   -37,     0,   -37,   -37,   -37,   -37,   -37,
       0,     0,     0,     0,   -37,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,     0,    68,    69,    70,     0,
       0,    71,    72,    73,    74,     0,     0,    75,    76,     0,
       0,     0,     0,     0,    77,     0,     0,     0,     0,     0,
       0,   213,     0,     0,    79,    80,    81,    82,    83,    84,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    85,    86,     0,     0,     0,    88,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    99,   100,   101,     0,   102,   103,   104,   105,   106,
       0,     0,     0,     0,   107,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,     0,    68,    69,    70,     0,
       0,    71,    72,    73,    74,     0,     0,    75,    76,     0,
       0,     0,     0,     0,    77,     0,     0,     0,     0,     0,
       0,   532,     0,     0,    79,    80,    81,    82,    83,    84,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    85,    86,     0,     0,     0,    88,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    99,   100,   101,     0,   102,   103,   104,   105,   106,
       0,     0,     0,     0,   107,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,     0,    68,    69,    70,     0,
       0,    71,    72,    73,    74,     0,     0,    75,    76,     0,
       0,     0,     0,     0,    77,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    79,    80,    81,    82,    83,    84,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    85,    86,     0,     0,     0,    88,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    99,   100,   101,     0,   102,   103,   104,   105,   106,
      30,    31,    32,    33,   107,     0,     0,     0,     0,    34,
      35,     0,     0,    36,    37,    38,    39,    40,    41,     0,
       0,     0,     0,     0,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,     0,     0,     0,     0,
       0,    54,    55,    56,    57,     0,     0,     0,     0,     0,
       0,     0,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,     0,    68,    69,    70,     0,     0,    71,    72,
      73,    74,     0,     0,    75,    76,     0,     0,     0,     0,
       0,    77,    78,     0,     0,     0,     0,     0,     0,     0,
       0,    79,    80,    81,    82,    83,    84,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      85,    86,     0,    87,     0,    88,     0,     0,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,     0,   102,   103,   104,   105,   106,    30,    31,    32,
      33,   107,     0,     0,     0,     0,    34,    35,     0,     0,
      36,    37,    38,    39,    40,    41,     0,     0,     0,     0,
       0,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,     0,     0,     0,     0,     0,    54,   -28,
      56,    57,     0,     0,     0,     0,     0,     0,     0,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,     0,
      68,    69,    70,     0,     0,    71,    72,    73,    74,     0,
       0,    75,    76,     0,     0,     0,     0,     0,    77,    78,
       0,     0,     0,     0,     0,     0,     0,     0,    79,    80,
      81,    82,    83,    84,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    85,    86,     0,
      87,     0,    88,     0,     0,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,     0,   102,
     103,   104,   105,   106,    30,    31,    32,    33,   107,     0,
       0,     0,     0,    34,    35,     0,     0,    36,    37,    38,
      39,    40,    41,     0,     0,     0,     0,     0,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
       0,     0,     0,     0,     0,    54,     0,    56,    57,     0,
       0,     0,     0,     0,     0,     0,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,     0,    68,    69,    70,
       0,     0,    71,    72,    73,    74,     0,     0,    75,    76,
       0,     0,     0,     0,     0,    77,    78,     0,     0,     0,
       0,     0,     0,     0,     0,    79,    80,    81,    82,    83,
      84,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    85,    86,   286,    87,     0,    88,
       0,     0,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,     0,   102,   103,   104,   105,
     106,     0,     0,     0,     0,   107,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,     0,    68,    69,    70,
       0,     0,    71,    72,    73,    74,     0,     0,    75,    76,
       0,     0,   287,     0,     0,    77,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    79,    80,    81,    82,    83,
      84,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    85,    86,     0,     0,     0,    88,
       0,     0,     0,     0,     0,  -325,     0,  -325,     0,     0,
       0,     0,    99,   100,   101,  -325,   102,   103,   104,   105,
     106,     0,     0,     0,     0,   107,   304,   305,   306,   307,
     308,   309,   310,   311,   312,     0,   313,   314,   315,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   316,   317,   318,   319,   320,   321,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   322,   323,     0,     0,     0,     0,   324,
     325,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   326,   327,   328,     0,   329,   330,   331,   332,   333,
      18,     0,   334,     0,     0,     0,     0,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,     0,    68,    69,
      70,     0,     0,    71,    72,    73,    74,     0,     0,    75,
      76,     0,     0,     0,     0,     0,    77,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    79,    80,    81,    82,
      83,    84,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    85,    86,     0,     0,     0,
      88,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    99,   100,   101,     0,   102,   103,   104,
     105,   106,   346,     0,     0,     0,   107,     0,     0,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,     0,
      68,    69,    70,     0,     0,    71,    72,    73,    74,     0,
       0,    75,    76,     0,     0,     0,     0,     0,    77,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    79,    80,
      81,    82,    83,    84,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    85,    86,     0,
       0,     0,    88,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    99,   100,   101,     0,   102,
     103,   104,   105,   106,   360,     0,     0,     0,   107,     0,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
       0,    68,    69,    70,     0,     0,    71,    72,    73,    74,
       0,   361,    75,    76,     0,     0,     0,     0,     0,    77,
       0,     0,     0,   362,   363,   364,     0,     0,     0,    79,
      80,    81,    82,    83,    84,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    85,    86,
       0,     0,     0,    88,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    99,   100,   101,     0,
     102,   103,   104,   105,   106,     0,     0,     0,     0,   107,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
       0,    68,    69,    70,     0,     0,    71,    72,    73,    74,
       0,     0,    75,    76,     0,     0,     0,     0,     0,    77,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    79,
      80,    81,    82,    83,    84,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    85,    86,
       0,     0,     0,    88,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    99,   100,   101,     0,
     102,   103,   104,   105,   106,     0,     0,     0,     0,   107,
     304,   305,   306,   307,   308,   309,   310,   311,   312,     0,
     313,   314,   315,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   316,   317,
     318,   319,   320,   321,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   322,   323,     0,
       0,     0,     0,   324,   325,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   326,   327,   328,     0,   329,
     330,   331,   332,   333,     0,     0,   334,     0,     0,   608,
     304,   305,   306,   307,   308,   309,   310,   311,   312,     0,
     313,   314,   315,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   316,   317,
     318,   319,   320,   321,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   322,   323,     0,
       0,     0,     0,   324,   325,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   326,   327,   328,     0,   329,
     330,   331,   332,   333,     0,     0,   334,   304,   305,   306,
     307,   308,   309,   310,   311,   312,     0,   500,   314,   315,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   316,   317,   318,   319,   320,
     321,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   322,   323,     0,     0,     0,     0,
     324,   325,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   326,   327,   328,     0,   329,   330,   331,   332,
     333,     0,     0,   334,   304,   305,   306,   307,   308,   309,
     310,   311,   312,     0,   313,   314,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   316,   317,   318,   319,   320,   321,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   322,   323,     0,     0,     0,     0,   324,   325,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     327,   328,     0,   329,   330,   331,   332,   333,     0,     0,
     334,   304,   305,   306,   307,   308,   309,   310,   311,   312,
       0,   313,   314,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   316,
     317,   318,   319,   320,   321,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   322,   323,
       0,     0,     0,     0,   324,   325,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   328,     0,
     329,   330,   331,   332,   333,     0,     0,   334
};

static const yytype_int16 yycheck[] =
{
      11,    15,    87,   118,   249,   122,   110,   130,    15,   123,
     350,     1,   116,   130,   194,    18,    27,   271,     1,   359,
     505,    46,   236,     1,   138,   278,   585,     1,   143,   133,
     278,   516,    72,     1,     0,     0,     0,   115,     0,    65,
     144,   119,   120,     1,    58,    71,   669,     1,    48,   127,
       1,     1,    65,   131,   132,    48,    14,   616,    71,   137,
      14,   511,     0,   513,    65,     1,    66,   626,     1,     1,
      71,   113,   114,    48,   512,   513,   699,    35,    69,    37,
      48,    72,    75,    66,    14,    15,   111,    45,   102,   103,
      48,   169,   206,   171,    72,   173,   200,   175,    72,   177,
     142,   179,   116,   181,   144,   183,     1,   185,    48,   187,
     124,   189,    48,     1,    82,    66,   115,     1,   144,   204,
     103,    72,    72,   106,   107,   108,   109,   110,   245,    65,
     247,   144,   472,    69,   693,    71,    72,    10,    11,     1,
      72,    77,    78,   144,     1,   595,    78,   262,    84,   115,
     115,   115,   602,   115,   271,   145,    72,   411,    67,   273,
      48,   599,     1,    25,    26,   603,   144,   420,   653,    75,
     144,     8,   420,   106,   107,   108,   109,    65,     1,    72,
       1,    69,     1,    71,    72,   263,    48,    71,    72,    77,
      78,    28,     1,   196,    78,   143,    84,   133,     1,   135,
     136,     4,   143,    42,   113,   114,   133,   143,   144,    71,
      72,   106,   107,   108,   109,     1,    78,     1,   232,    22,
      23,    24,    25,     1,     1,    48,    29,    48,   145,    48,
       1,    70,   112,   142,   574,    91,    92,     1,     1,    48,
     254,   112,   256,    46,    83,   133,   143,   135,   136,   106,
     107,   108,   109,    72,   101,   102,   144,    66,    42,   344,
      26,   384,    48,   102,   133,    28,   280,   384,     1,   392,
      48,    48,   286,     1,   391,   392,    79,    80,   458,   459,
     511,   512,   144,     1,     1,    48,    70,     1,    55,   503,
       1,   100,   101,   102,   411,    66,    82,   117,    35,    83,
      37,     1,    66,   381,    71,    72,    73,    74,   111,   112,
      77,    78,     1,   145,   245,     1,   247,    84,   102,   533,
      48,   535,   681,   682,    41,    42,    43,    41,    42,    43,
      48,   345,   103,   104,   105,   106,   107,   108,   109,   103,
     104,   105,   106,   107,   108,   109,   113,   114,    48,     1,
       1,   118,    72,    70,    72,     1,    70,    77,    78,    48,
     374,   475,     1,   594,    84,   379,    83,   598,     1,    83,
      87,    93,    72,    87,     1,    71,    72,   144,    91,   100,
      66,   102,    78,    72,   398,   102,    72,    92,   102,    57,
       9,    77,    78,    48,   498,     1,    48,    48,    84,    67,
      66,   518,    48,   520,   418,   522,   100,   101,    56,    57,
     145,    50,    51,    52,    45,    48,     1,   144,    66,    67,
     146,    48,   436,   437,   438,   439,   440,   441,   442,   443,
     444,   445,   446,   447,   448,   449,   450,   451,   452,   453,
     620,   145,    48,   657,     1,   113,   114,   461,   462,   463,
     464,   465,   466,   467,   468,   469,   470,   671,   369,     1,
       1,     1,    26,    48,   375,   113,   114,   135,   136,   137,
     138,   139,     1,   145,   142,     1,   145,     1,     1,    14,
     725,     1,   496,   497,    25,   499,     1,   135,   136,   137,
     138,   139,    45,     1,   142,    45,     1,    39,    40,   100,
     101,   102,   617,    15,    65,    10,    11,    48,   102,    66,
      71,   101,    53,    54,   100,    72,    77,    78,    26,    48,
      77,    78,    48,    84,    48,    48,    66,    84,    48,    71,
      72,     9,    72,    48,    14,    77,    78,    77,    78,     7,
       8,     9,    84,    48,    84,   648,    11,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    57,    66,    67,
      68,    66,    10,    71,    72,    73,    74,    67,   582,    77,
      78,   339,   133,   587,   135,   136,    84,   588,   585,   616,
     683,   322,   323,   144,   687,   688,    94,    95,    96,    97,
      98,    99,   103,   344,     1,   106,   107,   108,   109,   110,
     511,   512,   513,   229,   230,   113,   114,   322,   323,   616,
     118,   359,   627,   113,   114,   629,   494,   493,    25,   626,
     590,   628,   629,   131,   132,   133,   726,   135,   136,   137,
     138,   139,    39,    40,   652,   270,   144,   137,   138,   139,
     725,   726,   142,   134,     7,     8,     9,    10,    11,   663,
     420,   134,   666,   667,   668,   429,   663,   103,   104,   105,
     106,   107,   108,   109,    71,    72,   336,   134,    72,    73,
      74,    78,   583,   584,    78,   586,   388,   389,   390,   323,
     285,   695,    -1,   594,   595,    48,   693,   598,   599,   700,
      48,   602,   603,    -1,   605,   606,   607,   608,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    -1,    66,    67,
      68,    -1,   623,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     631,    -1,    -1,    -1,   635,    -1,   637,    -1,   639,    -1,
     641,    -1,   643,    -1,   645,    -1,    94,    95,    96,    97,
      98,    99,     1,    -1,    -1,     4,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   113,   114,    -1,    -1,    -1,
      -1,   119,   120,    22,    23,    24,    25,    -1,    -1,    -1,
      29,    -1,    -1,   131,   132,   133,    -1,   135,   136,   137,
     138,   139,    -1,    -1,   142,   696,   697,   698,    -1,    -1,
      -1,    -1,   703,    -1,    -1,   706,    -1,     1,    -1,     3,
       4,     5,     6,    -1,    -1,    -1,    -1,   718,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    -1,    -1,
      79,    80,    -1,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    -1,    -1,    -1,    -1,    -1,
      44,    45,    46,    47,    48,    -1,    -1,    -1,    -1,    -1,
      -1,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    -1,    66,    67,    68,    -1,    -1,    71,    72,    73,
      74,    75,    -1,    77,    78,    -1,    -1,    -1,    -1,    -1,
      84,    85,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      94,    95,    96,    97,    98,    99,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   113,
     114,    -1,   116,    -1,   118,    -1,    -1,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
      -1,   135,   136,   137,   138,   139,    -1,    -1,    -1,     1,
     144,     3,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,
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
      -1,   113,   114,    -1,   116,    -1,   118,    -1,    -1,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,    -1,   135,   136,   137,   138,   139,    -1,    -1,
      -1,     1,   144,     3,     4,     5,     6,    -1,    -1,    -1,
      -1,    -1,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    -1,    -1,    -1,    -1,    -1,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    -1,
      -1,    -1,    -1,    -1,    44,    45,    46,    47,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    -1,    66,    67,    68,    -1,
      -1,    71,    72,    73,    74,    -1,    -1,    77,    78,    -1,
      -1,    -1,    -1,    -1,    84,    85,    -1,    -1,    -1,    -1,
      -1,     1,    -1,    -1,    94,    95,    96,    97,    98,    99,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   113,   114,    -1,   116,    -1,   118,    -1,
      -1,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,    -1,   135,   136,   137,   138,   139,
      -1,    -1,    -1,    -1,   144,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    -1,    66,    67,    68,    -1,
      -1,    71,    72,    73,    74,    -1,    -1,    77,    78,    -1,
      -1,    -1,    -1,    -1,    84,    -1,    -1,    -1,    -1,    -1,
      -1,     1,    -1,    -1,    94,    95,    96,    97,    98,    99,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   113,   114,    -1,    -1,    -1,   118,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   131,   132,   133,    -1,   135,   136,   137,   138,   139,
      -1,    -1,    -1,    -1,   144,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    -1,    66,    67,    68,    -1,
      -1,    71,    72,    73,    74,    -1,    -1,    77,    78,    -1,
      -1,    -1,    -1,    -1,    84,    -1,    -1,    -1,    -1,    -1,
      -1,     1,    -1,    -1,    94,    95,    96,    97,    98,    99,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   113,   114,    -1,    -1,    -1,   118,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   131,   132,   133,    -1,   135,   136,   137,   138,   139,
      -1,    -1,    -1,    -1,   144,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    -1,    66,    67,    68,    -1,
      -1,    71,    72,    73,    74,    -1,    -1,    77,    78,    -1,
      -1,    -1,    -1,    -1,    84,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    94,    95,    96,    97,    98,    99,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   113,   114,    -1,    -1,    -1,   118,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   131,   132,   133,    -1,   135,   136,   137,   138,   139,
       3,     4,     5,     6,   144,    -1,    -1,    -1,    -1,    12,
      13,    -1,    -1,    16,    17,    18,    19,    20,    21,    -1,
      -1,    -1,    -1,    -1,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    -1,    -1,    -1,    -1,
      -1,    44,    45,    46,    47,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    -1,    66,    67,    68,    -1,    -1,    71,    72,
      73,    74,    -1,    -1,    77,    78,    -1,    -1,    -1,    -1,
      -1,    84,    85,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    94,    95,    96,    97,    98,    99,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     113,   114,    -1,   116,    -1,   118,    -1,    -1,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,    -1,   135,   136,   137,   138,   139,     3,     4,     5,
       6,   144,    -1,    -1,    -1,    -1,    12,    13,    -1,    -1,
      16,    17,    18,    19,    20,    21,    -1,    -1,    -1,    -1,
      -1,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    -1,    -1,    -1,    -1,    -1,    44,    45,
      46,    47,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    -1,
      66,    67,    68,    -1,    -1,    71,    72,    73,    74,    -1,
      -1,    77,    78,    -1,    -1,    -1,    -1,    -1,    84,    85,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,    95,
      96,    97,    98,    99,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   113,   114,    -1,
     116,    -1,   118,    -1,    -1,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,    -1,   135,
     136,   137,   138,   139,     3,     4,     5,     6,   144,    -1,
      -1,    -1,    -1,    12,    13,    -1,    -1,    16,    17,    18,
      19,    20,    21,    -1,    -1,    -1,    -1,    -1,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      -1,    -1,    -1,    -1,    -1,    44,    -1,    46,    47,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    -1,    66,    67,    68,
      -1,    -1,    71,    72,    73,    74,    -1,    -1,    77,    78,
      -1,    -1,    -1,    -1,    -1,    84,    85,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    94,    95,    96,    97,    98,
      99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   113,   114,    25,   116,    -1,   118,
      -1,    -1,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,    -1,   135,   136,   137,   138,
     139,    -1,    -1,    -1,    -1,   144,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    -1,    66,    67,    68,
      -1,    -1,    71,    72,    73,    74,    -1,    -1,    77,    78,
      -1,    -1,    81,    -1,    -1,    84,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    94,    95,    96,    97,    98,
      99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   113,   114,    -1,    -1,    -1,   118,
      -1,    -1,    -1,    -1,    -1,    35,    -1,    37,    -1,    -1,
      -1,    -1,   131,   132,   133,    45,   135,   136,   137,   138,
     139,    -1,    -1,    -1,    -1,   144,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    -1,    66,    67,    68,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    94,    95,    96,    97,    98,    99,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   113,   114,    -1,    -1,    -1,    -1,   119,
     120,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   131,   132,   133,    -1,   135,   136,   137,   138,   139,
      48,    -1,   142,    -1,    -1,    -1,    -1,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    -1,    66,    67,
      68,    -1,    -1,    71,    72,    73,    74,    -1,    -1,    77,
      78,    -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    94,    95,    96,    97,
      98,    99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   113,   114,    -1,    -1,    -1,
     118,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   131,   132,   133,    -1,   135,   136,   137,
     138,   139,    48,    -1,    -1,    -1,   144,    -1,    -1,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    -1,
      66,    67,    68,    -1,    -1,    71,    72,    73,    74,    -1,
      -1,    77,    78,    -1,    -1,    -1,    -1,    -1,    84,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,    95,
      96,    97,    98,    99,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   113,   114,    -1,
      -1,    -1,   118,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   131,   132,   133,    -1,   135,
     136,   137,   138,   139,    49,    -1,    -1,    -1,   144,    -1,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      -1,    66,    67,    68,    -1,    -1,    71,    72,    73,    74,
      -1,    76,    77,    78,    -1,    -1,    -1,    -1,    -1,    84,
      -1,    -1,    -1,    88,    89,    90,    -1,    -1,    -1,    94,
      95,    96,    97,    98,    99,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   113,   114,
      -1,    -1,    -1,   118,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   131,   132,   133,    -1,
     135,   136,   137,   138,   139,    -1,    -1,    -1,    -1,   144,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      -1,    66,    67,    68,    -1,    -1,    71,    72,    73,    74,
      -1,    -1,    77,    78,    -1,    -1,    -1,    -1,    -1,    84,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,
      95,    96,    97,    98,    99,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   113,   114,
      -1,    -1,    -1,   118,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   131,   132,   133,    -1,
     135,   136,   137,   138,   139,    -1,    -1,    -1,    -1,   144,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    -1,
      66,    67,    68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,    95,
      96,    97,    98,    99,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   113,   114,    -1,
      -1,    -1,    -1,   119,   120,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   131,   132,   133,    -1,   135,
     136,   137,   138,   139,    -1,    -1,   142,    -1,    -1,   145,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    -1,
      66,    67,    68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,    95,
      96,    97,    98,    99,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   113,   114,    -1,
      -1,    -1,    -1,   119,   120,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   131,   132,   133,    -1,   135,
     136,   137,   138,   139,    -1,    -1,   142,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    -1,    66,    67,    68,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    94,    95,    96,    97,    98,
      99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   113,   114,    -1,    -1,    -1,    -1,
     119,   120,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   131,   132,   133,    -1,   135,   136,   137,   138,
     139,    -1,    -1,   142,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    -1,    66,    67,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    94,    95,    96,    97,    98,    99,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   113,   114,    -1,    -1,    -1,    -1,   119,   120,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     132,   133,    -1,   135,   136,   137,   138,   139,    -1,    -1,
     142,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      -1,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,
      95,    96,    97,    98,    99,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   113,   114,
      -1,    -1,    -1,    -1,   119,   120,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   133,    -1,
     135,   136,   137,   138,   139,    -1,    -1,   142
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,   148,   149,     0,   150,   163,   164,   115,   151,   152,
     153,   157,   159,   160,   165,   166,   167,   279,    48,    75,
     162,   186,   258,   259,     1,   154,   152,   157,   151,   159,
       3,     4,     5,     6,    12,    13,    16,    17,    18,    19,
      20,    21,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    44,    45,    46,    47,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    66,    67,
      68,    71,    72,    73,    74,    77,    78,    84,    85,    94,
      95,    96,    97,    98,    99,   113,   114,   116,   118,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   135,   136,   137,   138,   139,   144,   166,   168,
     169,   170,   171,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   187,   188,   189,   190,   191,
     192,   193,   194,   195,   196,   197,   198,   199,   200,   201,
     205,   206,   211,   212,   213,   214,   250,   251,   252,   253,
     254,   255,   256,   257,   260,   261,   262,   264,   265,   269,
     270,   271,   272,   273,   274,   276,   281,   289,   291,   292,
     293,   294,   295,   296,   297,   298,   299,   300,   301,   302,
     303,   304,   305,   306,   307,   308,   309,   310,   311,   312,
     313,   344,   345,   352,   353,   164,    48,    71,    72,    78,
     155,   156,     1,    72,   144,   362,   363,   313,   172,   173,
     363,     1,   313,     1,   313,   314,    14,    15,   162,     1,
      39,    40,    71,    72,    77,    78,    84,   209,   284,   285,
     286,   357,   215,   313,   361,   162,   313,     1,    72,   364,
     364,   361,   361,   202,     1,    69,   267,   344,   354,   358,
     362,   313,     1,     1,    50,    51,    52,   361,     1,     4,
      22,    23,    24,    25,    29,    46,    79,    80,   111,   112,
     266,   268,     1,    28,   354,   361,   361,     1,   162,     1,
      25,    78,   282,   284,   285,   286,    25,    81,   290,   361,
     362,   364,   162,   361,   361,   361,   361,   361,   361,   361,
     361,   361,   361,   361,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    66,    67,    68,    94,    95,    96,    97,
      98,    99,   113,   114,   119,   120,   131,   132,   133,   135,
     136,   137,   138,   139,   142,   346,   347,   346,   164,    66,
     162,   363,   362,     1,   143,   133,    48,   343,   359,   361,
     207,     1,    42,    70,    83,   102,   287,     1,   287,   208,
      49,    76,    88,    89,    90,   216,   217,   221,   247,   313,
     163,     1,     1,     1,    25,    26,    71,    72,    78,   144,
     203,   222,   225,   358,   143,   358,    65,    71,   133,   135,
     136,   144,   355,   356,   357,     1,   313,     1,    25,    53,
      54,   263,   313,   364,     1,   361,   112,   112,    46,   111,
       1,   268,   267,     1,   362,     1,    14,    45,   197,   198,
     277,   278,   313,     1,     1,    41,    43,    87,   287,   288,
       1,   288,   313,     1,     1,     1,   326,   319,   330,   332,
     331,   333,   334,   335,   336,   325,   324,   323,   337,   338,
     339,   340,   341,   342,    72,    78,   348,   349,   352,   353,
     348,   327,   328,   321,   322,   329,   315,   316,   317,   318,
     320,   350,   351,   343,   156,   145,   172,   313,     1,   145,
     143,   210,   359,     1,    82,   283,     1,   210,     1,    66,
     243,    91,    92,   218,   219,   220,    10,    11,   249,   133,
      66,   243,     1,    14,   204,   313,     1,   226,   227,   228,
     229,   233,   234,   235,   243,   313,   361,   354,   144,   356,
     144,   356,   144,   356,   344,   354,   313,     1,    26,   267,
       1,     1,     1,   313,     1,   198,   278,   280,     1,   283,
       1,   313,   313,   313,   313,   313,   313,   313,   313,   313,
     313,   313,   313,   313,   313,   313,   313,   313,   313,   117,
     346,   346,   313,   313,   313,   313,   313,   313,   313,   313,
     313,   313,   359,   362,   360,   145,     1,    72,    78,     1,
     219,   218,    93,   313,   313,   162,   313,     9,   163,   223,
     224,   223,     1,   230,   234,   235,   243,   231,   233,   235,
     243,   232,   233,   234,   243,   100,   101,   102,   145,   223,
     344,   344,   344,   145,   163,     1,   163,    45,   275,     1,
     144,   146,   359,   313,   243,   243,   158,   161,   165,   166,
     243,   313,     1,   159,   225,   235,   243,   234,   243,   235,
     243,   233,   243,   234,   243,   233,   243,     1,   243,     1,
     243,     1,   243,   243,   145,   145,   145,    14,   161,   364,
     346,   243,   165,   166,   213,   166,     7,     8,     9,   248,
     243,    15,   243,   243,   243,   243,   243,   243,   103,   106,
     107,   108,   109,   110,   236,   238,   103,   104,   105,   237,
     238,   237,   223,   163,     1,     9,   313,   313,   313,   248,
     163,     1,   241,   242,     1,   239,   240,     1,   241,     1,
     241,     1,   238,     1,   238,     1,   238,   165,   313,   243,
     243,   243,   248,     1,   159,   243,   243,    14,   243,   244,
     245,   246,   357,   363,   245
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   147,   149,   148,   150,   150,   150,   150,   151,   151,
     152,   152,   154,   153,   153,   155,   155,   156,   156,   156,
     157,   157,   158,   158,   159,   159,   160,   161,   161,   162,
     162,   163,   163,   164,   164,   165,   165,   166,   167,   168,
     168,   168,   168,   169,   169,   169,   169,   169,   169,   169,
     169,   169,   169,   169,   169,   169,   169,   169,   169,   169,
     169,   169,   169,   169,   169,   169,   169,   169,   169,   169,
     169,   169,   169,   169,   169,   169,   169,   169,   169,   169,
     170,   170,   171,   171,   172,   172,   173,   173,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   202,   201,   203,   203,
     203,   204,   203,   203,   203,   205,   207,   206,   208,   206,
     206,   206,   206,   206,   206,   206,   206,   206,   209,   209,
     210,   210,   211,   212,   213,   213,   213,   214,   215,   215,
     215,   216,   217,   218,   219,   220,   220,   220,   220,   221,
     221,   221,   221,   221,   221,   221,   221,   221,   221,   221,
     222,   222,   222,   224,   223,   223,   225,   225,   225,   226,
     226,   226,   226,   227,   228,   229,   230,   230,   230,   230,
     230,   231,   231,   231,   231,   231,   232,   232,   232,   232,
     232,   233,   233,   234,   234,   235,   235,   236,   236,   236,
     236,   237,   237,   237,   237,   237,   237,   238,   238,   238,
     238,   238,   238,   238,   238,   240,   239,   242,   241,   243,
     243,   244,   244,   245,   246,   247,   248,   248,   248,   249,
     249,   249,   250,   250,   251,   251,   252,   253,   253,   253,
     253,   253,   253,   254,   255,   256,   257,   257,   257,   258,
     259,   260,   260,   260,   261,   261,   262,   262,   262,   262,
     262,   262,   262,   262,   262,   262,   263,   263,   264,   265,
     265,   265,   265,   266,   266,   266,   266,   266,   266,   266,
     267,   267,   268,   268,   268,   268,   268,   268,   268,   268,
     268,   269,   269,   269,   269,   269,   270,   271,   272,   273,
     274,   275,   275,   275,   276,   276,   276,   276,   276,   276,
     277,   277,   277,   278,   278,   278,   278,   279,   279,   280,
     280,   281,   281,   281,   281,   281,   281,   281,   281,   281,
     281,   281,   282,   282,   282,   283,   283,   283,   284,   284,
     285,   286,   287,   287,   287,   287,   288,   288,   288,   288,
     289,   289,   289,   289,   290,   291,   292,   293,   294,   295,
     296,   297,   298,   299,   300,   301,   302,   303,   304,   305,
     306,   307,   308,   309,   310,   311,   312,   314,   313,   315,
     313,   316,   313,   317,   313,   313,   318,   313,   313,   319,
     313,   313,   320,   313,   313,   321,   313,   313,   322,   313,
     313,   323,   313,   313,   324,   313,   313,   325,   313,   313,
     326,   313,   313,   313,   313,   327,   313,   328,   313,   329,
     313,   313,   330,   313,   313,   331,   313,   313,   332,   313,
     313,   333,   313,   313,   334,   313,   313,   335,   313,   313,
     336,   313,   313,   337,   313,   313,   338,   313,   313,   339,
     313,   313,   340,   313,   313,   341,   313,   313,   342,   313,
     313,   313,   313,   313,   313,   313,   313,   313,   313,   313,
     313,   313,   313,   313,   313,   313,   313,   343,   343,   343,
     344,   345,   345,   347,   346,   348,   348,   348,   348,   348,
     348,   349,   351,   350,   352,   353,   354,   354,   354,   355,
     355,   355,   355,   355,   355,   355,   355,   355,   356,   356,
     357,   357,   357,   358,   358,   358,   360,   359,   359,   361,
     361,   362,   362,   362,   362,   363,   364,   364
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
       2,     2,     2,     3,     3,     1,     3,     1,     0,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     0,     3,     3,     1,
       0,     0,     3,     3,     5,     2,     0,     4,     0,     4,
       3,     3,     4,     5,     4,     3,     4,     3,     1,     1,
       1,     2,     1,     1,     2,     1,     3,     7,     0,     2,
       2,     1,     1,     1,     1,     1,     1,     2,     2,     7,
       6,     5,     4,     2,     2,     2,     5,     5,     8,     0,
       1,     1,     1,     0,     2,     0,     2,     3,     2,     1,
       1,     1,     1,     2,     2,     2,     2,     3,     2,     3,
       1,     2,     3,     2,     3,     1,     2,     3,     2,     3,
       1,     4,     3,     4,     3,     4,     3,     1,     2,     2,
       1,     1,     2,     2,     2,     2,     1,     2,     2,     2,
       2,     2,     2,     2,     2,     0,     3,     0,     3,     1,
       0,     1,     1,     1,     1,     1,     3,     3,     3,     3,
       3,     0,     3,     2,     3,     2,     2,     6,     9,     9,
       6,     2,     4,     2,     2,     2,     2,     3,     1,     1,
       1,     2,     3,     1,     1,     2,     3,     2,     4,     3,
       2,     4,     3,     2,     3,     2,     1,     1,     2,     4,
       3,     3,     2,     1,     2,     2,     1,     2,     2,     1,
       3,     1,     1,     1,     1,     1,     1,     1,     2,     3,
       2,     1,     2,     3,     4,     3,     2,     2,     2,     2,
       2,     2,     3,     1,     5,     4,     4,     2,     3,     5,
       2,     1,     1,     6,     7,     2,     2,     2,     2,     3,
       0,     3,     3,     2,     3,     3,     4,     5,     4,     3,
       4,     3,     1,     1,     1,     2,     2,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     2,     3,     2,     1,     2,     1,     2,     1,     2,
       1,     2,     1,     2,     1,     2,     1,     2,     1,     2,
       1,     2,     1,     2,     1,     2,     1,     0,     3,     0,
       4,     0,     4,     0,     4,     1,     0,     4,     1,     0,
       4,     1,     0,     4,     1,     0,     4,     1,     0,     4,
       1,     0,     4,     1,     0,     4,     1,     0,     4,     1,
       0,     4,     1,     2,     1,     0,     4,     0,     4,     0,
       4,     1,     0,     4,     1,     0,     4,     1,     0,     4,
       1,     0,     4,     1,     0,     4,     1,     0,     4,     1,
       0,     4,     1,     0,     4,     1,     0,     4,     1,     0,
       4,     1,     0,     4,     1,     0,     4,     1,     0,     4,
       1,     3,     3,     1,     1,     3,     1,     1,     1,     1,
       1,     1,     1,     2,     2,     2,     2,     2,     2,     1,
       1,     2,     2,     0,     2,     1,     2,     4,     1,     2,
       2,     1,     0,     3,     1,     1,     3,     1,     1,     2,
       2,     1,     2,     3,     4,     4,     4,     1,     1,     1,
       1,     1,     1,     2,     2,     0,     0,     4,     1,     1,
       0,     2,     1,     4,     3,     1,     1,     1
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
#line 2627 "yaccsrc.c"
    break;

  case 4: /* prog: nlncl stats  */
#line 185 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-1], yyvsp[0] );
                                         yyval->o.last = NULL;
                                         EndProg( yyval ) ; }
#line 2635 "yaccsrc.c"
    break;

  case 5: /* prog: nlncl  */
#line 188 "yaccsrc.y"
                                       { yyval = yyvsp[0];
                                         if ( yyval != NULL )
                                            yyval->o.last = NULL;
                                         EndProg( yyval ); }
#line 2644 "yaccsrc.c"
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
#line 2656 "yaccsrc.c"
    break;

  case 7: /* prog: nlncl dirsect  */
#line 199 "yaccsrc.y"
                                       { nodeptr more = split_requires( &yyvsp[0] );
                                         yyval = optgluelast( makenode( X_OO_INIT, 0 ), yyvsp[-1] );
                                         yyval = optgluelast( yyval, more );
                                         yyval = optgluelast( yyval, yyvsp[0] );
                                         yyval->o.last = NULL;
                                         EndProg( yyval ) ; }
#line 2667 "yaccsrc.c"
    break;

  case 8: /* dirsect: dirsect dirblock  */
#line 212 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 2673 "yaccsrc.c"
    break;

  case 9: /* dirsect: dirblock  */
#line 213 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2679 "yaccsrc.c"
    break;

  case 10: /* dirblock: directive stats  */
#line 216 "yaccsrc.y"
                                       { yyval = yyvsp[-1];
                                         yyval->p[1] = optgluelast( yyval->p[1], yyvsp[0] );
                                         if ( yyval->p[1] != NULL )
                                            yyval->p[1]->o.last = NULL; }
#line 2688 "yaccsrc.c"
    break;

  case 11: /* dirblock: directive  */
#line 220 "yaccsrc.y"
                                       { yyval = yyvsp[0];
                                         if ( yyval->p[1] != NULL )
                                            yyval->p[1]->o.last = NULL; }
#line 2696 "yaccsrc.c"
    break;

  case 12: /* @2: %empty  */
#line 225 "yaccsrc.y"
                                       { yyval = makenode( X_DIRECTIVE, 0 );
                                         yyval->lineno = parser_data.tline;
                                         yyval->charnr = parser_data.tstart; }
#line 2704 "yaccsrc.c"
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
#line 2718 "yaccsrc.c"
    break;

  case 14: /* directive: DCOLON error  */
#line 237 "yaccsrc.y"
                                       { exiterror( ERR_YACC_SYNTAX, 1, parser_data.tline ); }
#line 2724 "yaccsrc.c"
    break;

  case 15: /* dirwords: dirwords SPACE dirword  */
#line 240 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-2], yyvsp[0] ); }
#line 2730 "yaccsrc.c"
    break;

  case 16: /* dirwords: dirword  */
#line 241 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2736 "yaccsrc.c"
    break;

  case 17: /* dirword: SIMSYMBOL  */
#line 244 "yaccsrc.y"
                                       { yyval = makenode( X_STRING, 0 );
                                         yyval->name = Str_cre_TSD( parser_data.TSD, retvalue ); }
#line 2743 "yaccsrc.c"
    break;

  case 18: /* dirword: CONSYMBOL  */
#line 246 "yaccsrc.y"
                                       { yyval = makenode( X_STRING, 0 );
                                         yyval->name = Str_cre_TSD( parser_data.TSD, retvalue ); }
#line 2750 "yaccsrc.c"
    break;

  case 19: /* dirword: STRING  */
#line 248 "yaccsrc.y"
                                       { yyval = makenode( X_STRING, 0 );
                                         yyval->name = Str_cre_TSD( parser_data.TSD, retvalue ); }
#line 2757 "yaccsrc.c"
    break;

  case 20: /* stats: stats ystatement  */
#line 252 "yaccsrc.y"
                                       { /* fixes bug 579711 */
                                         yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 2764 "yaccsrc.c"
    break;

  case 21: /* stats: ystatement  */
#line 254 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2770 "yaccsrc.c"
    break;

  case 22: /* xstats: xstats statement  */
#line 257 "yaccsrc.y"
                                       { /* fixes bug 579711 */
                                         yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 2777 "yaccsrc.c"
    break;

  case 23: /* xstats: statement gruff  */
#line 259 "yaccsrc.y"
                                       { yyval = yyvsp[-1]; }
#line 2783 "yaccsrc.c"
    break;

  case 24: /* ystatement: statement  */
#line 262 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 2789 "yaccsrc.c"
    break;

  case 25: /* ystatement: lonely_end  */
#line 263 "yaccsrc.y"
                                       { exiterror( ERR_UNMATCHED_END, 1 ); }
#line 2795 "yaccsrc.c"
    break;

  case 27: /* nxstats: xstats  */
#line 271 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2801 "yaccsrc.c"
    break;

  case 28: /* nxstats: gruff  */
#line 272 "yaccsrc.y"
                                       { yyval = NULL; }
#line 2807 "yaccsrc.c"
    break;

  case 29: /* ncl: ncl STATSEP optLabels  */
#line 275 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-2], yyvsp[0] ); }
#line 2813 "yaccsrc.c"
    break;

  case 30: /* ncl: STATSEP optLabels  */
#line 276 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2819 "yaccsrc.c"
    break;

  case 31: /* nlncl: optLabels ncl  */
#line 279 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 2825 "yaccsrc.c"
    break;

  case 32: /* nlncl: optLabels  */
#line 280 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2831 "yaccsrc.c"
    break;

  case 33: /* optLabels: optLabels label_stat  */
#line 283 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 2837 "yaccsrc.c"
    break;

  case 34: /* optLabels: %empty  */
#line 284 "yaccsrc.y"
                                       { yyval = NULL; }
#line 2843 "yaccsrc.c"
    break;

  case 37: /* gruff: %empty  */
#line 291 "yaccsrc.y"
                                       { tmpchr=parser_data.tstart;
                                         tmplno=parser_data.tline; }
#line 2850 "yaccsrc.c"
    break;

  case 38: /* mttstatement: gruff mtstatement  */
#line 295 "yaccsrc.y"
                                       { yyval=yyvsp[0]; }
#line 2856 "yaccsrc.c"
    break;

  case 39: /* mtstatement: nclstatement ncl  */
#line 298 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 2862 "yaccsrc.c"
    break;

  case 80: /* oo_expose_stat: EXPOSE anyvars  */
#line 347 "yaccsrc.y"
                                       { yyval = makenode( X_OO_EXPOSE, 1, yyvsp[0] );
                                         yyval->lineno = tmplno;
                                         yyval->charnr = tmpchr; }
#line 2870 "yaccsrc.c"
    break;

  case 81: /* oo_expose_stat: EXPOSE error  */
#line 350 "yaccsrc.y"
                                       { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ); }
#line 2876 "yaccsrc.c"
    break;

  case 82: /* oo_usearg_stat: USEARG usevars  */
#line 353 "yaccsrc.y"
                                       { yyval = makenode( X_OO_USEARG, 1, yyvsp[0] );
                                         yyval->lineno = tmplno;
                                         yyval->charnr = tmpchr; }
#line 2884 "yaccsrc.c"
    break;

  case 83: /* oo_usearg_stat: USEARG usevars error  */
#line 356 "yaccsrc.y"
                                       { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ); }
#line 2890 "yaccsrc.c"
    break;

  case 84: /* usevars: usevar ',' usevars  */
#line 359 "yaccsrc.y"
                                       { yyval = yyvsp[-2];
                                         yyval->p[0] = yyvsp[0]; }
#line 2897 "yaccsrc.c"
    break;

  case 85: /* usevars: usevar  */
#line 361 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2903 "yaccsrc.c"
    break;

  case 86: /* usevar: xsimsymb '=' expr  */
#line 364 "yaccsrc.y"
                                       { /* USE ARG name = default */
                                         yyval = makenode( X_SIM_SYMBOL, 2, NULL, yyvsp[0] );
                                         yyval->name = (streng *) yyvsp[-2]; }
#line 2911 "yaccsrc.c"
    break;

  case 87: /* usevar: xsimsymb  */
#line 367 "yaccsrc.y"
                                       { yyval = makenode( X_SIM_SYMBOL, 0 );
                                         yyval->name = (streng *) yyvsp[0]; }
#line 2918 "yaccsrc.c"
    break;

  case 88: /* usevar: %empty  */
#line 369 "yaccsrc.y"
                                       { yyval = makenode( X_SIM_SYMBOL, 0 );
                                         yyval->name = NULL; }
#line 2925 "yaccsrc.c"
    break;

  case 89: /* call: CALL  */
#line 373 "yaccsrc.y"
                                       { yyval = makenode(X_CALL,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2933 "yaccsrc.c"
    break;

  case 90: /* do: DO  */
#line 377 "yaccsrc.y"
                                       { yyval = makenode(X_DO,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ;
                                         /* OO: LOOP, see finish_do() */
                                         yyval->u.nonansi = __regina_do_is_loop ;
                                         level++; }
#line 2944 "yaccsrc.c"
    break;

  case 91: /* exit: EXIT  */
#line 384 "yaccsrc.y"
                                       { yyval = makenode(X_EXIT,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2952 "yaccsrc.c"
    break;

  case 92: /* if: IF  */
#line 388 "yaccsrc.y"
                                       { yyval = makenode(X_IF,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ;
                                         level++; }
#line 2961 "yaccsrc.c"
    break;

  case 93: /* iterate: ITERATE  */
#line 393 "yaccsrc.y"
                                       { yyval = makenode(X_ITERATE,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2969 "yaccsrc.c"
    break;

  case 94: /* leave: LEAVE  */
#line 397 "yaccsrc.y"
                                       { yyval = makenode(X_LEAVE,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2977 "yaccsrc.c"
    break;

  case 95: /* say: SAY  */
#line 401 "yaccsrc.y"
                                       { yyval = makenode(X_SAY,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2985 "yaccsrc.c"
    break;

  case 96: /* return: RETURN  */
#line 405 "yaccsrc.y"
                                       { yyval = makenode(X_RETURN,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2993 "yaccsrc.c"
    break;

  case 97: /* address: ADDRESS  */
#line 409 "yaccsrc.y"
                                       { yyval = makenode(X_ADDR_N,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3001 "yaccsrc.c"
    break;

  case 98: /* arg: ARG  */
#line 413 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_ARG,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3009 "yaccsrc.c"
    break;

  case 99: /* drop: DROP  */
#line 417 "yaccsrc.y"
                                       { yyval = makenode(X_DROP,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3017 "yaccsrc.c"
    break;

  case 100: /* interpret: INTERPRET  */
#line 421 "yaccsrc.y"
                                       { yyval = makenode(X_IPRET,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3025 "yaccsrc.c"
    break;

  case 101: /* label: LABEL  */
#line 425 "yaccsrc.y"
                                       { yyval = makenode(X_LABEL,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3033 "yaccsrc.c"
    break;

  case 102: /* nop: NOP  */
#line 429 "yaccsrc.y"
                                       { yyval = makenode(X_NULL,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3041 "yaccsrc.c"
    break;

  case 103: /* numeric: NUMERIC  */
#line 433 "yaccsrc.y"
                                       { yyval = makenode(0,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3049 "yaccsrc.c"
    break;

  case 104: /* options: OPTIONS  */
#line 437 "yaccsrc.y"
                                       { yyval = makenode(X_OPTIONS,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3057 "yaccsrc.c"
    break;

  case 105: /* parse: PARSE  */
#line 441 "yaccsrc.y"
                                       { yyval = makenode(0,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3065 "yaccsrc.c"
    break;

  case 106: /* proc: PROCEDURE  */
#line 445 "yaccsrc.y"
                                       { yyval = makenode(X_PROC,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3073 "yaccsrc.c"
    break;

  case 107: /* pull: PULL  */
#line 449 "yaccsrc.y"
                                       { yyval = makenode(X_PULL,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3081 "yaccsrc.c"
    break;

  case 108: /* push: PUSH  */
#line 453 "yaccsrc.y"
                                       { yyval = makenode(X_PUSH,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3089 "yaccsrc.c"
    break;

  case 109: /* queue: QUEUE  */
#line 457 "yaccsrc.y"
                                       { yyval = makenode(X_QUEUE,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3097 "yaccsrc.c"
    break;

  case 110: /* select: SELECT  */
#line 461 "yaccsrc.y"
                                       { yyval = makenode(X_SELECT,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ;
                                         level++; }
#line 3106 "yaccsrc.c"
    break;

  case 111: /* signal: SIGNAL  */
#line 466 "yaccsrc.y"
                                       { yyval = makenode(X_SIG_LAB,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3114 "yaccsrc.c"
    break;

  case 112: /* when: WHEN  */
#line 470 "yaccsrc.y"
                                       { yyval = makenode(X_WHEN,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3122 "yaccsrc.c"
    break;

  case 113: /* otherwise: OTHERWISE  */
#line 474 "yaccsrc.y"
                                       { yyval = makenode(X_OTHERWISE,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3130 "yaccsrc.c"
    break;

  case 114: /* trace: TRACE  */
#line 478 "yaccsrc.y"
                                       { yyval = makenode(X_TRACE,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3138 "yaccsrc.c"
    break;

  case 115: /* upper: UPPER  */
#line 482 "yaccsrc.y"
                                       { yyval = makenode(X_UPPER_VAR,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3146 "yaccsrc.c"
    break;

  case 116: /* @3: %empty  */
#line 486 "yaccsrc.y"
                                       { yyval = current = yyvsp[0] ; }
#line 3152 "yaccsrc.c"
    break;

  case 118: /* address_stat2: VALUE expr naddr_with  */
#line 490 "yaccsrc.y"
                                       { current->type = X_ADDR_V ;
                                         current->p[0] = yyvsp[-1] ;
                                         current->p[1] = yyvsp[0] ; }
#line 3160 "yaccsrc.c"
    break;

  case 119: /* address_stat2: addr_with  */
#line 493 "yaccsrc.y"
                                       { exiterror( ERR_STRING_EXPECTED, 1, __reginatext ) ;}
#line 3166 "yaccsrc.c"
    break;

  case 120: /* address_stat2: %empty  */
#line 494 "yaccsrc.y"
                                       { current->type = X_ADDR_S ; }
#line 3172 "yaccsrc.c"
    break;

  case 121: /* $@4: %empty  */
#line 495 "yaccsrc.y"
                                       { exiterror( ERR_STRING_EXPECTED, 1, __reginatext ) ;}
#line 3178 "yaccsrc.c"
    break;

  case 123: /* address_stat2: nvir nexpr naddr_with  */
#line 497 "yaccsrc.y"
                                       { current->name = (streng *)yyvsp[-2] ;
                                         current->type = X_ADDR_N ;
                                         current->p[0] = yyvsp[-1] ;
                                         current->p[1] = yyvsp[0] ; }
#line 3187 "yaccsrc.c"
    break;

  case 124: /* address_stat2: '(' expr ')' nspace naddr_with  */
#line 501 "yaccsrc.y"
                                              { current->type = X_ADDR_V ;
                                         current->p[0] = yyvsp[-3] ;
                                         current->p[1] = yyvsp[0] ;
                                         current->u.nonansi = 1 ; }
#line 3196 "yaccsrc.c"
    break;

  case 125: /* arg_stat: arg templs  */
#line 507 "yaccsrc.y"
                                       { yyval = makenode( X_PARSE, 2, yyvsp[-1], yyvsp[0] );
                                         yyval->u.parseflags = PARSE_UPPER;
                                         yyval->lineno = yyvsp[-1]->lineno;
                                         yyval->charnr = yyvsp[-1]->charnr; }
#line 3205 "yaccsrc.c"
    break;

  case 126: /* $@5: %empty  */
#line 513 "yaccsrc.y"
                                       { parendepth = start_parendepth; }
#line 3211 "yaccsrc.c"
    break;

  case 127: /* call_stat: call call_name $@5 call_args  */
#line 514 "yaccsrc.y"
                                       { yyval = yyvsp[-3];
                                         yyval->p[0] = yyvsp[0];
                                         yyval->name = (streng *) yyvsp[-2];
                                         parendepth = 0; }
#line 3220 "yaccsrc.c"
    break;

  case 128: /* $@6: %empty  */
#line 518 "yaccsrc.y"
                                       { parendepth = start_parendepth; }
#line 3226 "yaccsrc.c"
    break;

  case 129: /* call_stat: call string $@6 call_args  */
#line 519 "yaccsrc.y"
                                       { yyval = yyvsp[-3];
                                         yyval->type = X_EX_FUNC;
                                         yyval->p[0] = yyvsp[0];
                                         yyval->name = (streng *) yyvsp[-2];
                                         parendepth = 0; }
#line 3236 "yaccsrc.c"
    break;

  case 130: /* call_stat: call on error  */
#line 524 "yaccsrc.y"
                             { exiterror( ERR_INV_SUBKEYWORD, 1, "ERROR FAILURE HALT NOTREADY", __reginatext ) ;}
#line 3242 "yaccsrc.c"
    break;

  case 131: /* call_stat: call off error  */
#line 525 "yaccsrc.y"
                              { exiterror( ERR_INV_SUBKEYWORD, 2, "ERROR FAILURE HALT NOTREADY", __reginatext ) ;}
#line 3248 "yaccsrc.c"
    break;

  case 132: /* call_stat: call on c_action error  */
#line 526 "yaccsrc.y"
                                      { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 3254 "yaccsrc.c"
    break;

  case 133: /* call_stat: call on c_action namespec error  */
#line 527 "yaccsrc.y"
                                               { exiterror( ERR_STRING_EXPECTED, 3, __reginatext ) ;}
#line 3260 "yaccsrc.c"
    break;

  case 134: /* call_stat: call on c_action namespec  */
#line 529 "yaccsrc.y"
                                       { yyval = yyvsp[-3] ;
                                         yyval->type = X_CALL_SET ;
                                         yyval->p[0] = yyvsp[-2] ;
                                         yyval->name = (streng *)yyvsp[0] ;
                                         yyval->p[1] = yyvsp[-1] ; }
#line 3270 "yaccsrc.c"
    break;

  case 135: /* call_stat: call on c_action  */
#line 534 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_CALL_SET ;
                                         yyval->p[0] = yyvsp[-1] ;
                                         yyval->name = NULL ;
                                         yyval->p[1] = yyvsp[0] ; }
#line 3280 "yaccsrc.c"
    break;

  case 136: /* call_stat: call off c_action error  */
#line 539 "yaccsrc.y"
                                       { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 3286 "yaccsrc.c"
    break;

  case 137: /* call_stat: call off c_action  */
#line 540 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_CALL_SET ;
                                         yyval->p[0] = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ; }
#line 3295 "yaccsrc.c"
    break;

  case 138: /* call_name: asymbol  */
#line 546 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 3301 "yaccsrc.c"
    break;

  case 139: /* call_name: error  */
#line 547 "yaccsrc.y"
                                       { exiterror( ERR_STRING_EXPECTED, 2, __reginatext );}
#line 3307 "yaccsrc.c"
    break;

  case 140: /* call_args: exprs  */
#line 550 "yaccsrc.y"
                                       {
                                         /*
                                          * "call_args" accepted probably with
                                          * surrounding parentheses. Strip them.
                                          */
                                         yyval = reduce_expr_list( yyvsp[0],
                                                                REDUCE_CALL );
                                       }
#line 3320 "yaccsrc.c"
    break;

  case 141: /* call_args: exprs ')'  */
#line 558 "yaccsrc.y"
                                       { exiterror(ERR_UNEXPECTED_PARAN, 2); }
#line 3326 "yaccsrc.c"
    break;

  case 142: /* expr_stat: expr  */
#line 561 "yaccsrc.y"
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
#line 3351 "yaccsrc.c"
    break;

  case 143: /* end_stat: END  */
#line 583 "yaccsrc.y"
                                       { yyval = makenode(X_END,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ;
                                         level--; }
#line 3360 "yaccsrc.c"
    break;

  case 144: /* end: end_stat simsymb  */
#line 589 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->name = (streng*)(yyvsp[0]) ; }
#line 3367 "yaccsrc.c"
    break;

  case 145: /* end: end_stat  */
#line 591 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3373 "yaccsrc.c"
    break;

  case 146: /* end: end_stat simsymb error  */
#line 592 "yaccsrc.y"
                                       {  exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 3379 "yaccsrc.c"
    break;

  case 147: /* do_stat: do do_opts repetitor conditional ncl nxstats end  */
#line 596 "yaccsrc.y"
                                       { yyval = yyvsp[-6];
                                         yyval->p[0] = yyvsp[-4];
                                         yyval->p[1] = yyvsp[-3];
                                         yyval->p[2] = optgluelast( yyvsp[-2], yyvsp[-1] );
                                         if ( yyval->p[2] )
                                            yyval->p[2]->o.last = NULL;
                                         yyval->p[3] = yyvsp[0];
                                         finish_do( yyval, yyvsp[-5] ); }
#line 3392 "yaccsrc.c"
    break;

  case 148: /* do_opts: %empty  */
#line 607 "yaccsrc.y"
                                       { yyval = NULL ; }
#line 3398 "yaccsrc.c"
    break;

  case 149: /* do_opts: do_opts dolabel  */
#line 608 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ? yyvsp[-1] : makenode(X_OO_DOOPTS,0) ;
                                         if ( yyval->name )
                                            exiterror( ERR_INVALID_DO_SYNTAX, 1, "LABEL" ) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 3407 "yaccsrc.c"
    break;

  case 150: /* do_opts: do_opts docounter  */
#line 612 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ? yyvsp[-1] : makenode(X_OO_DOOPTS,0) ;
                                         if ( yyval->p[0] )
                                            exiterror( ERR_INVALID_DO_SYNTAX, 1, "COUNTER" ) ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3416 "yaccsrc.c"
    break;

  case 151: /* dolabel: DOLABEL  */
#line 618 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 3422 "yaccsrc.c"
    break;

  case 152: /* docounter: DOCOUNTER  */
#line 620 "yaccsrc.y"
                                       { yyval = create_head( retvalue ) ; }
#line 3428 "yaccsrc.c"
    break;

  case 153: /* doindex: DOINDEX  */
#line 622 "yaccsrc.y"
                                       { yyval = create_head( retvalue ) ; }
#line 3434 "yaccsrc.c"
    break;

  case 154: /* doitem: DOITEM  */
#line 624 "yaccsrc.y"
                                       { yyval = create_head( retvalue ) ; }
#line 3440 "yaccsrc.c"
    break;

  case 155: /* withvars: doindex  */
#line 628 "yaccsrc.y"
                                       { yyval = makenode(X_OO_DOOPTS,2,yyvsp[0],NULL) ; }
#line 3446 "yaccsrc.c"
    break;

  case 156: /* withvars: doitem  */
#line 629 "yaccsrc.y"
                                       { yyval = makenode(X_OO_DOOPTS,2,NULL,yyvsp[0]) ; }
#line 3452 "yaccsrc.c"
    break;

  case 157: /* withvars: doindex doitem  */
#line 630 "yaccsrc.y"
                                       { yyval = makenode(X_OO_DOOPTS,2,yyvsp[-1],yyvsp[0]) ; }
#line 3458 "yaccsrc.c"
    break;

  case 158: /* withvars: doitem doindex  */
#line 631 "yaccsrc.y"
                                       { yyval = makenode(X_OO_DOOPTS,2,yyvsp[0],yyvsp[-1]) ; }
#line 3464 "yaccsrc.c"
    break;

  case 159: /* repetitor: dovar '=' expr nspace tobyfor tobyfor tobyfor  */
#line 635 "yaccsrc.y"
                                       { yyval =makenode(X_REP,4,yyvsp[-4],yyvsp[-2],yyvsp[-1],yyvsp[0]) ;
                                         yyval->name = (streng *)yyvsp[-6] ;
                                         checkdosyntax(yyval) ; }
#line 3472 "yaccsrc.c"
    break;

  case 160: /* repetitor: dovar '=' expr nspace tobyfor tobyfor  */
#line 639 "yaccsrc.y"
                                       { yyval =makenode(X_REP,3,yyvsp[-3],yyvsp[-1],yyvsp[0]) ;
                                         yyval->name = (streng *)yyvsp[-5] ;
                                         checkdosyntax(yyval) ; }
#line 3480 "yaccsrc.c"
    break;

  case 161: /* repetitor: dovar '=' expr nspace tobyfor  */
#line 643 "yaccsrc.y"
                                       { yyval = makenode(X_REP,2,yyvsp[-2],yyvsp[0]) ;
                                         yyval->name = (streng *)yyvsp[-4] ;
                                         checkdosyntax(yyval) ; }
#line 3488 "yaccsrc.c"
    break;

  case 162: /* repetitor: dovar '=' expr nspace  */
#line 646 "yaccsrc.y"
                                       { yyval = makenode(X_REP,1,yyvsp[-1]) ;
                                         yyval->name = (streng *)yyvsp[-3] ;
                                         checkdosyntax(yyval) ; }
#line 3496 "yaccsrc.c"
    break;

  case 163: /* repetitor: FOREVER nspace  */
#line 649 "yaccsrc.y"
                                       { yyval = makenode(X_REP_FOREVER,0) ; }
#line 3502 "yaccsrc.c"
    break;

  case 164: /* repetitor: FOREVER error  */
#line 650 "yaccsrc.y"
                             { exiterror( ERR_INV_SUBKEYWORD, 16, "WHILE UNTIL", __reginatext ) ; }
#line 3508 "yaccsrc.c"
    break;

  case 165: /* repetitor: expr nspace  */
#line 651 "yaccsrc.y"
                                       { yyvsp[-1] = makenode(X_DO_EXPR,1,yyvsp[-1]) ;
                                         yyval = makenode(X_REP,2,NULL,yyvsp[-1]) ; }
#line 3515 "yaccsrc.c"
    break;

  case 166: /* repetitor: expr nspace FOR expr nspace  */
#line 655 "yaccsrc.y"
                                       { yyvsp[-4] = makenode(X_DO_EXPR,1,yyvsp[-4]) ;
                                         yyval = makenode(X_REP,3,NULL,yyvsp[-4],
                                                       makenode(X_DO_FOR,1,yyvsp[-1])) ;
                                         if ( do_over_var( yyval ) == NULL )
                                            exiterror( ERR_INVALID_DO_SYNTAX, 1, "FOR" ) ; }
#line 3525 "yaccsrc.c"
    break;

  case 167: /* repetitor: DOWITH withvars DOOVER expr nspace  */
#line 661 "yaccsrc.y"
                                       { yyval = makenode(X_OO_DOWITH,4,yyvsp[-1],yyvsp[-3]->p[0],yyvsp[-3]->p[1],NULL) ; }
#line 3531 "yaccsrc.c"
    break;

  case 168: /* repetitor: DOWITH withvars DOOVER expr nspace FOR expr nspace  */
#line 663 "yaccsrc.y"
                                       { yyval = makenode(X_OO_DOWITH,4,yyvsp[-4],yyvsp[-6]->p[0],yyvsp[-6]->p[1],
                                                       makenode(X_DO_FOR,1,yyvsp[-1])) ; }
#line 3538 "yaccsrc.c"
    break;

  case 169: /* repetitor: %empty  */
#line 665 "yaccsrc.y"
                                       { yyval = NULL ; }
#line 3544 "yaccsrc.c"
    break;

  case 170: /* nvir: CONSYMBOL  */
#line 668 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 3550 "yaccsrc.c"
    break;

  case 171: /* nvir: SIMSYMBOL  */
#line 669 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 3556 "yaccsrc.c"
    break;

  case 172: /* nvir: STRING  */
#line 670 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 3562 "yaccsrc.c"
    break;

  case 173: /* @7: %empty  */
#line 673 "yaccsrc.y"
                                       { SymbolDetect |= SD_ADDRWITH ;
                                         yyval = with = makenode(X_ADDR_WITH,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3571 "yaccsrc.c"
    break;

  case 174: /* naddr_with: @7 addr_with  */
#line 677 "yaccsrc.y"
                                       { with = NULL ;
                                         SymbolDetect &= ~SD_ADDRWITH ; }
#line 3578 "yaccsrc.c"
    break;

  case 175: /* naddr_with: %empty  */
#line 679 "yaccsrc.y"
                                       { yyval = NULL ; }
#line 3584 "yaccsrc.c"
    break;

  case 176: /* addr_with: WITH connection  */
#line 682 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 3590 "yaccsrc.c"
    break;

  case 177: /* addr_with: WITH connection error  */
#line 683 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 5, __reginatext ) ; }
#line 3596 "yaccsrc.c"
    break;

  case 178: /* addr_with: WITH nspace  */
#line 684 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 5, __reginatext ) ; }
#line 3602 "yaccsrc.c"
    break;

  case 182: /* connection: error  */
#line 690 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 5, __reginatext ) ; }
#line 3608 "yaccsrc.c"
    break;

  case 201: /* inputstmt: nspace INPUT nspace resourcei  */
#line 726 "yaccsrc.y"
                                             { with->p[0] = yyvsp[0]; }
#line 3614 "yaccsrc.c"
    break;

  case 202: /* inputstmt: nspace INPUT error  */
#line 727 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 6, __reginatext ) ; }
#line 3620 "yaccsrc.c"
    break;

  case 203: /* outputstmt: nspace OUTPUT nspace resourceo  */
#line 730 "yaccsrc.y"
                                              { with->p[1] = yyvsp[0]; }
#line 3626 "yaccsrc.c"
    break;

  case 204: /* outputstmt: nspace OUTPUT error  */
#line 731 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 7, __reginatext ) ; }
#line 3632 "yaccsrc.c"
    break;

  case 205: /* errorstmt: nspace ERROR nspace resourceo  */
#line 734 "yaccsrc.y"
                                             { with->p[2] = yyvsp[0]; }
#line 3638 "yaccsrc.c"
    break;

  case 206: /* errorstmt: nspace ERROR error  */
#line 735 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 14, __reginatext ) ; }
#line 3644 "yaccsrc.c"
    break;

  case 207: /* resourcei: resources  */
#line 738 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3650 "yaccsrc.c"
    break;

  case 208: /* resourcei: NOEOL resources  */
#line 739 "yaccsrc.y"
                                       { yyval = yyvsp[0] ;
                                         yyval->u.of.noeol = 1 ; }
#line 3657 "yaccsrc.c"
    break;

  case 209: /* resourcei: NOEOL error  */
#line 741 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 8, __reginatext ) ; }
#line 3663 "yaccsrc.c"
    break;

  case 210: /* resourcei: NORMAL  */
#line 742 "yaccsrc.y"
                                       { yyval = makenode(X_ADDR_WITH, 0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3671 "yaccsrc.c"
    break;

  case 211: /* resourceo: resources  */
#line 747 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3677 "yaccsrc.c"
    break;

  case 212: /* resourceo: APPEND resources  */
#line 748 "yaccsrc.y"
                                       { yyval = yyvsp[0] ;
                                         yyval->u.of.append = 1 ; }
#line 3684 "yaccsrc.c"
    break;

  case 213: /* resourceo: APPEND error  */
#line 750 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 8, __reginatext ) ; }
#line 3690 "yaccsrc.c"
    break;

  case 214: /* resourceo: REPLACE resources  */
#line 751 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3696 "yaccsrc.c"
    break;

  case 215: /* resourceo: REPLACE error  */
#line 752 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 9, __reginatext ) ; }
#line 3702 "yaccsrc.c"
    break;

  case 216: /* resourceo: NORMAL  */
#line 753 "yaccsrc.y"
                                       { yyval = makenode(X_ADDR_WITH, 0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3710 "yaccsrc.c"
    break;

  case 217: /* resources: STREAM nnvir  */
#line 758 "yaccsrc.y"
                                       { /* ANSI extension: nsimsymb is
                                          * used by the standard, but I think
                                          * there are no reasons why using
                                          * it here as a must. FGC
                                          */
                                         yyval = yyvsp[0] ;
                                         yyval->u.of.awt = awtSTREAM;
                                         SymbolDetect |= SD_ADDRWITH ; }
#line 3723 "yaccsrc.c"
    break;

  case 218: /* resources: STREAM error  */
#line 766 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_OPTION, 1, __reginatext ) ; }
#line 3729 "yaccsrc.c"
    break;

  case 219: /* resources: STEM nsimsymb  */
#line 767 "yaccsrc.y"
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
#line 3747 "yaccsrc.c"
    break;

  case 220: /* resources: STEM error  */
#line 780 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_OPTION, 2, __reginatext ) ; }
#line 3753 "yaccsrc.c"
    break;

  case 221: /* resources: LIFO nnvir  */
#line 781 "yaccsrc.y"
                                       {
                                         yyval = yyvsp[0] ;
                                         yyval->u.of.awt = awtLIFO ;
                                         SymbolDetect |= SD_ADDRWITH ; }
#line 3762 "yaccsrc.c"
    break;

  case 222: /* resources: LIFO error  */
#line 785 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_OPTION, 100, __reginatext ) ; }
#line 3768 "yaccsrc.c"
    break;

  case 223: /* resources: FIFO nnvir  */
#line 786 "yaccsrc.y"
                                       {
                                         yyval = yyvsp[0] ;
                                         yyval->u.of.awt = awtFIFO ;
                                         SymbolDetect |= SD_ADDRWITH ; }
#line 3777 "yaccsrc.c"
    break;

  case 224: /* resources: FIFO error  */
#line 790 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_OPTION, 101, __reginatext ) ; }
#line 3783 "yaccsrc.c"
    break;

  case 225: /* $@8: %empty  */
#line 793 "yaccsrc.y"
                                       { SymbolDetect &= ~SD_ADDRWITH ; }
#line 3789 "yaccsrc.c"
    break;

  case 226: /* nsimsymb: $@8 nspace addrSim  */
#line 794 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3795 "yaccsrc.c"
    break;

  case 227: /* $@9: %empty  */
#line 797 "yaccsrc.y"
                                       { SymbolDetect &= ~SD_ADDRWITH ; }
#line 3801 "yaccsrc.c"
    break;

  case 228: /* nnvir: $@9 nspace addrAll  */
#line 798 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3807 "yaccsrc.c"
    break;

  case 231: /* addrAll: addrSim  */
#line 805 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3813 "yaccsrc.c"
    break;

  case 232: /* addrAll: addrString  */
#line 806 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3819 "yaccsrc.c"
    break;

  case 233: /* addrSim: xsimsymb  */
#line 809 "yaccsrc.y"
                                       { yyval = makenode(X_ADDR_WITH, 0) ;
                                         yyval->name = (streng *) yyvsp[0] ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->u.of.ant = antSIMSYMBOL;
                                       }
#line 3830 "yaccsrc.c"
    break;

  case 234: /* addrString: string  */
#line 817 "yaccsrc.y"
                                       { yyval = makenode(X_ADDR_WITH, 0) ;
                                         yyval->name = (streng *) yyvsp[0] ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->u.of.ant = antSTRING;
                                       }
#line 3841 "yaccsrc.c"
    break;

  case 235: /* dovar: DOVARIABLE  */
#line 825 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 3847 "yaccsrc.c"
    break;

  case 236: /* tobyfor: TO expr nspace  */
#line 828 "yaccsrc.y"
                                       { yyval = makenode(X_DO_TO,1,yyvsp[-1]) ; }
#line 3853 "yaccsrc.c"
    break;

  case 237: /* tobyfor: FOR expr nspace  */
#line 829 "yaccsrc.y"
                                       { yyval = makenode(X_DO_FOR,1,yyvsp[-1]) ; }
#line 3859 "yaccsrc.c"
    break;

  case 238: /* tobyfor: BY expr nspace  */
#line 830 "yaccsrc.y"
                                       { yyval = makenode(X_DO_BY,1,yyvsp[-1]) ; }
#line 3865 "yaccsrc.c"
    break;

  case 239: /* conditional: WHILE expr nspace  */
#line 833 "yaccsrc.y"
                                       { yyval = makenode(X_WHILE,1,yyvsp[-1]) ; }
#line 3871 "yaccsrc.c"
    break;

  case 240: /* conditional: UNTIL expr nspace  */
#line 834 "yaccsrc.y"
                                       { yyval = makenode(X_UNTIL,1,yyvsp[-1]) ; }
#line 3877 "yaccsrc.c"
    break;

  case 241: /* conditional: %empty  */
#line 835 "yaccsrc.y"
                                       { yyval = NULL ; }
#line 3883 "yaccsrc.c"
    break;

  case 242: /* drop_stat: drop anyvars error  */
#line 838 "yaccsrc.y"
                                  { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ) ;}
#line 3889 "yaccsrc.c"
    break;

  case 243: /* drop_stat: drop anyvars  */
#line 839 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3896 "yaccsrc.c"
    break;

  case 244: /* upper_stat: upper anyvars error  */
#line 843 "yaccsrc.y"
                                   { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ) ;}
#line 3902 "yaccsrc.c"
    break;

  case 245: /* upper_stat: upper anyvars  */
#line 844 "yaccsrc.y"
                                        { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3909 "yaccsrc.c"
    break;

  case 246: /* exit_stat: exit nexpr  */
#line 848 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3916 "yaccsrc.c"
    break;

  case 247: /* if_stat: if expr nlncl THEN nlncl ystatement  */
#line 853 "yaccsrc.y"
                                       { move_labels( yyvsp[-5], yyvsp[0], level - 1 );
                                         yyval = yyvsp[-5];
                                         yyval->p[0] = optgluelast( yyvsp[-4], yyvsp[-3] );
                                         yyval->p[0]->o.last = NULL;
                                         yyval->p[1] = optgluelast( yyvsp[-1], yyvsp[0] );
                                         yyval->p[1]->o.last = NULL;
                                         level--; }
#line 3928 "yaccsrc.c"
    break;

  case 248: /* if_stat: if expr nlncl THEN nlncl ystatement ELSE nlncl ystatement  */
#line 861 "yaccsrc.y"
                                       { move_labels( yyvsp[-8], yyvsp[0], level - 1 );
                                         yyval = yyvsp[-8];
                                         yyval->p[0] = optgluelast( yyvsp[-7], yyvsp[-6] );
                                         yyval->p[0]->o.last = NULL;
                                         yyval->p[1] = optgluelast( yyvsp[-4], yyvsp[-3] );
                                         yyval->p[1]->o.last = NULL;
                                         yyval->p[2] = optgluelast( yyvsp[-1], yyvsp[0] );
                                         yyval->p[2]->o.last = NULL;
                                         level--; }
#line 3942 "yaccsrc.c"
    break;

  case 249: /* if_stat: if expr nlncl THEN nlncl ystatement ELSE nlncl error  */
#line 871 "yaccsrc.y"
                                       {  exiterror( ERR_INCOMPLETE_STRUCT, 4 ) ;}
#line 3948 "yaccsrc.c"
    break;

  case 250: /* if_stat: if expr nlncl THEN nlncl error  */
#line 873 "yaccsrc.y"
                                       {  exiterror( ERR_INCOMPLETE_STRUCT, 3 ) ;}
#line 3954 "yaccsrc.c"
    break;

  case 251: /* if_stat: if ncl  */
#line 874 "yaccsrc.y"
                                       {  exiterror( ERR_INCOMPLETE_STRUCT, 0 ) ;}
#line 3960 "yaccsrc.c"
    break;

  case 252: /* if_stat: if expr nlncl error  */
#line 875 "yaccsrc.y"
                                       {  exiterror( ERR_THEN_EXPECTED, 1, parser_data.if_linenr, __reginatext )  ; }
#line 3966 "yaccsrc.c"
    break;

  case 253: /* unexp_then: gruff THEN  */
#line 878 "yaccsrc.y"
                                       {  exiterror( ERR_THEN_UNEXPECTED, 1 )  ; }
#line 3972 "yaccsrc.c"
    break;

  case 254: /* unexp_else: gruff ELSE  */
#line 881 "yaccsrc.y"
                                       {  exiterror( ERR_THEN_UNEXPECTED, 2 )  ; }
#line 3978 "yaccsrc.c"
    break;

  case 255: /* ipret_stat: interpret expr  */
#line 884 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3985 "yaccsrc.c"
    break;

  case 256: /* iterate_stat: iterate simsymb  */
#line 889 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 3992 "yaccsrc.c"
    break;

  case 257: /* iterate_stat: iterate simsymb error  */
#line 891 "yaccsrc.y"
                                     { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 3998 "yaccsrc.c"
    break;

  case 258: /* iterate_stat: iterate  */
#line 892 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 4004 "yaccsrc.c"
    break;

  case 259: /* label_stat: labelname  */
#line 895 "yaccsrc.y"
                                       { yyval = yyvsp[0] ;
                                         yyval->u.trace_only =
                                                         (level == 0) ? 0 : 1;
                                         newlabel( (const tsd_t *)parser_data.TSD,
                                                   &parser_data,
                                                   yyvsp[0] ) ; }
#line 4015 "yaccsrc.c"
    break;

  case 260: /* labelname: label  */
#line 903 "yaccsrc.y"
                                       { yyval = yyvsp[0] ;
                                         yyval->name = Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 4022 "yaccsrc.c"
    break;

  case 261: /* leave_stat: leave simsymb  */
#line 907 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 4029 "yaccsrc.c"
    break;

  case 262: /* leave_stat: leave simsymb error  */
#line 909 "yaccsrc.y"
                                   { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 4035 "yaccsrc.c"
    break;

  case 263: /* leave_stat: leave  */
#line 910 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 4041 "yaccsrc.c"
    break;

  case 264: /* nop_stat: nop  */
#line 913 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 4047 "yaccsrc.c"
    break;

  case 265: /* nop_stat: nop error  */
#line 914 "yaccsrc.y"
                         {  exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 4053 "yaccsrc.c"
    break;

  case 266: /* numeric_stat: numeric DIGITS expr  */
#line 917 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_NUM_D ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4061 "yaccsrc.c"
    break;

  case 267: /* numeric_stat: numeric DIGITS  */
#line 920 "yaccsrc.y"
                                       { yyval = yyvsp[-1]; yyval->type = X_NUM_DDEF ; }
#line 4067 "yaccsrc.c"
    break;

  case 268: /* numeric_stat: numeric FORM form_expr error  */
#line 921 "yaccsrc.y"
                                            { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 4073 "yaccsrc.c"
    break;

  case 269: /* numeric_stat: numeric FORM form_expr  */
#line 922 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_NUM_F ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4081 "yaccsrc.c"
    break;

  case 270: /* numeric_stat: numeric FORM  */
#line 925 "yaccsrc.y"
                                       { /* NOTE! This clashes ANSI! */
                                         yyval = yyvsp[-1] ; yyval->type=X_NUM_FRMDEF ;}
#line 4088 "yaccsrc.c"
    break;

  case 271: /* numeric_stat: numeric FORM VALUE expr  */
#line 927 "yaccsrc.y"
                                       { yyval = yyvsp[-3] ; yyval->type=X_NUM_V ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4095 "yaccsrc.c"
    break;

  case 272: /* numeric_stat: numeric FORM error  */
#line 929 "yaccsrc.y"
                                  { exiterror( ERR_INV_SUBKEYWORD, 11, "ENGINEERING SCIENTIFIC", __reginatext ) ;}
#line 4101 "yaccsrc.c"
    break;

  case 273: /* numeric_stat: numeric FUZZ  */
#line 930 "yaccsrc.y"
                                       { yyval = yyvsp[-1]; yyval->type = X_NUM_FDEF ;}
#line 4107 "yaccsrc.c"
    break;

  case 274: /* numeric_stat: numeric FUZZ expr  */
#line 931 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_NUM_FUZZ ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4115 "yaccsrc.c"
    break;

  case 275: /* numeric_stat: numeric error  */
#line 934 "yaccsrc.y"
                             { exiterror( ERR_INV_SUBKEYWORD, 15, "DIGITS FORM FUZZ", __reginatext ) ;}
#line 4121 "yaccsrc.c"
    break;

  case 276: /* form_expr: SCIENTIFIC  */
#line 937 "yaccsrc.y"
                                       { yyval = makenode(X_NUM_SCI,0) ; }
#line 4127 "yaccsrc.c"
    break;

  case 277: /* form_expr: ENGINEERING  */
#line 938 "yaccsrc.y"
                                       { yyval = makenode(X_NUM_ENG,0) ; }
#line 4133 "yaccsrc.c"
    break;

  case 278: /* options_stat: options nexpr  */
#line 941 "yaccsrc.y"
                                       { (yyval=yyvsp[-1])->p[0]=yyvsp[0] ; }
#line 4139 "yaccsrc.c"
    break;

  case 279: /* parse_stat: parse parse_flags parse_param templs  */
#line 945 "yaccsrc.y"
                                       { yyval = yyvsp[-3] ;
                                         yyval->type = X_PARSE ;
                                         yyval->u.parseflags = (int) yyvsp[-2] ;
                                         yyval->p[0] = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ; }
#line 4149 "yaccsrc.c"
    break;

  case 280: /* parse_stat: parse parse_param templs  */
#line 951 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_PARSE ;
                                         yyval->u.parseflags = 0;
                                         yyval->p[0] = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ; }
#line 4159 "yaccsrc.c"
    break;

  case 281: /* parse_stat: parse parse_flags error  */
#line 956 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 12, "ARG EXTERNAL LINEIN PULL SOURCE VAR VALUE VERSION", __reginatext ) ;}
#line 4165 "yaccsrc.c"
    break;

  case 282: /* parse_stat: parse error  */
#line 957 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 12, "ARG CASELESS EXTERNAL LINEIN LOWER PULL SOURCE UPPER VAR VALUE VERSION", __reginatext ) ;}
#line 4171 "yaccsrc.c"
    break;

  case 283: /* parse_flags: UPPER  */
#line 960 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_UPPER  |
                                                         PARSE_NORMAL); }
#line 4178 "yaccsrc.c"
    break;

  case 284: /* parse_flags: UPPER CASELESS  */
#line 962 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_UPPER  |
                                                         PARSE_CASELESS); }
#line 4185 "yaccsrc.c"
    break;

  case 285: /* parse_flags: CASELESS UPPER  */
#line 964 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_UPPER  |
                                                         PARSE_CASELESS); }
#line 4192 "yaccsrc.c"
    break;

  case 286: /* parse_flags: LOWER  */
#line 966 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_LOWER  |
                                                         PARSE_NORMAL); }
#line 4199 "yaccsrc.c"
    break;

  case 287: /* parse_flags: LOWER CASELESS  */
#line 968 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_LOWER  |
                                                         PARSE_CASELESS); }
#line 4206 "yaccsrc.c"
    break;

  case 288: /* parse_flags: CASELESS LOWER  */
#line 970 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_LOWER  |
                                                         PARSE_CASELESS); }
#line 4213 "yaccsrc.c"
    break;

  case 289: /* parse_flags: CASELESS  */
#line 972 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_NORMAL |
                                                         PARSE_CASELESS); }
#line 4220 "yaccsrc.c"
    break;

  case 290: /* templs: templs ',' template  */
#line 976 "yaccsrc.y"
                                       { /* fixes bugs like bug 579711 */
                                         yyval = optgluelast( yyvsp[-2], yyvsp[0] ); }
#line 4227 "yaccsrc.c"
    break;

  case 291: /* templs: template  */
#line 978 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 4233 "yaccsrc.c"
    break;

  case 292: /* parse_param: ARG  */
#line 981 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_ARG,0) ; }
#line 4239 "yaccsrc.c"
    break;

  case 293: /* parse_param: LINEIN  */
#line 982 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_EXT,0) ; }
#line 4245 "yaccsrc.c"
    break;

  case 294: /* parse_param: EXTERNAL  */
#line 983 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_EXT,0) ; }
#line 4251 "yaccsrc.c"
    break;

  case 295: /* parse_param: VERSION  */
#line 984 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_VER,0) ; }
#line 4257 "yaccsrc.c"
    break;

  case 296: /* parse_param: PULL  */
#line 985 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_PULL,0) ; }
#line 4263 "yaccsrc.c"
    break;

  case 297: /* parse_param: SOURCE  */
#line 986 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_SRC,0) ; }
#line 4269 "yaccsrc.c"
    break;

  case 298: /* parse_param: VAR simsymb  */
#line 987 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_VAR,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 4276 "yaccsrc.c"
    break;

  case 299: /* parse_param: VALUE nexpr WITH  */
#line 989 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_VAL,1,yyvsp[-1]) ; }
#line 4282 "yaccsrc.c"
    break;

  case 300: /* parse_param: VALUE error  */
#line 990 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_TEMPLATE, 3 ) ;}
#line 4288 "yaccsrc.c"
    break;

  case 301: /* proc_stat: proc  */
#line 993 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 4294 "yaccsrc.c"
    break;

  case 302: /* proc_stat: proc error  */
#line 994 "yaccsrc.y"
                          { exiterror( ERR_INV_SUBKEYWORD, 17, __reginatext ) ;}
#line 4300 "yaccsrc.c"
    break;

  case 303: /* proc_stat: proc EXPOSE error  */
#line 995 "yaccsrc.y"
                                 { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ) ;}
#line 4306 "yaccsrc.c"
    break;

  case 304: /* proc_stat: proc EXPOSE anyvars error  */
#line 996 "yaccsrc.y"
                                         { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ) ;}
#line 4312 "yaccsrc.c"
    break;

  case 305: /* proc_stat: proc EXPOSE anyvars  */
#line 997 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4319 "yaccsrc.c"
    break;

  case 306: /* pull_stat: pull template  */
#line 1001 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4326 "yaccsrc.c"
    break;

  case 307: /* push_stat: push nexpr  */
#line 1005 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4333 "yaccsrc.c"
    break;

  case 308: /* queue_stat: queue nexpr  */
#line 1009 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4340 "yaccsrc.c"
    break;

  case 309: /* say_stat: say nexpr  */
#line 1013 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4347 "yaccsrc.c"
    break;

  case 310: /* return_stat: return nexpr  */
#line 1017 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         /* OO: "RETURN x." returns a Stem object */
                                         if ( yyvsp[0] && yyvsp[0]->type == X_STEM_SYMBOL
                                           && __regina_oo_keywords() )
                                            yyvsp[0] = makenode( X_OO_STEMOBJ, 1, yyvsp[0] ) ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4358 "yaccsrc.c"
    break;

  case 311: /* sel_end: END simsymb  */
#line 1025 "yaccsrc.y"
                                       { exiterror( ERR_UNMATCHED_END, 0 ) ;}
#line 4364 "yaccsrc.c"
    break;

  case 312: /* sel_end: END simsymb error  */
#line 1026 "yaccsrc.y"
                                       { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 4370 "yaccsrc.c"
    break;

  case 313: /* sel_end: END  */
#line 1027 "yaccsrc.y"
                                       { level--; }
#line 4376 "yaccsrc.c"
    break;

  case 314: /* select_stat: select ncl when_stats otherwise_stat sel_end  */
#line 1031 "yaccsrc.y"
                                       { yyval = yyvsp[-4];
                                         yyval->p[0] = optgluelast( yyvsp[-3], yyvsp[-2] );
                                         yyval->p[0]->o.last = NULL;
                                         yyval->p[1] = yyvsp[-1]; }
#line 4385 "yaccsrc.c"
    break;

  case 315: /* select_stat: select ncl END error  */
#line 1035 "yaccsrc.y"
                                       {  exiterror( ERR_WHEN_EXPECTED, 1, parser_data.select_linenr, __reginatext ) ;}
#line 4391 "yaccsrc.c"
    break;

  case 316: /* select_stat: select ncl otherwise error  */
#line 1037 "yaccsrc.y"
                                       {  exiterror( ERR_WHEN_EXPECTED, 2, parser_data.select_linenr, __reginatext ) ;}
#line 4397 "yaccsrc.c"
    break;

  case 317: /* select_stat: select error  */
#line 1038 "yaccsrc.y"
                                       {  exiterror( ERR_EXTRA_DATA, 1, __reginatext )  ;}
#line 4403 "yaccsrc.c"
    break;

  case 318: /* select_stat: select ncl THEN  */
#line 1039 "yaccsrc.y"
                                       {  exiterror( ERR_THEN_UNEXPECTED, 0 ) ;}
#line 4409 "yaccsrc.c"
    break;

  case 319: /* select_stat: select ncl when_stats otherwise error  */
#line 1041 "yaccsrc.y"
                                       {  exiterror( ERR_INCOMPLETE_STRUCT, 0 ) ;}
#line 4415 "yaccsrc.c"
    break;

  case 320: /* when_stats: when_stats when_stat  */
#line 1044 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 4421 "yaccsrc.c"
    break;

  case 321: /* when_stats: when_stat  */
#line 1045 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 4427 "yaccsrc.c"
    break;

  case 322: /* when_stats: error  */
#line 1046 "yaccsrc.y"
                                       {  exiterror( ERR_WHEN_EXPECTED, 2, parser_data.select_linenr, __reginatext )  ;}
#line 4433 "yaccsrc.c"
    break;

  case 323: /* when_stat: when expr nlncl THEN nlncl statement  */
#line 1050 "yaccsrc.y"
                                       { yyval = yyvsp[-5]; /* fixes bugs like bug 579711 */
                                         yyval->p[0] = optgluelast( yyvsp[-4], yyvsp[-3] );
                                         yyval->p[0]->o.last = NULL;
                                         yyval->p[1] = optgluelast( yyvsp[-1], yyvsp[0] );
                                         yyval->p[1]->o.last = NULL; }
#line 4443 "yaccsrc.c"
    break;

  case 324: /* when_stat: when expr nlncl THEN nlncl statement THEN  */
#line 1056 "yaccsrc.y"
                                       {  exiterror( ERR_THEN_UNEXPECTED, 0 ) ;}
#line 4449 "yaccsrc.c"
    break;

  case 325: /* when_stat: when expr  */
#line 1057 "yaccsrc.y"
                                       {  exiterror( ERR_THEN_EXPECTED, 2, parser_data.select_linenr, __reginatext )  ; }
#line 4455 "yaccsrc.c"
    break;

  case 326: /* when_stat: when error  */
#line 1058 "yaccsrc.y"
                                       {  exiterror( ERR_INVALID_EXPRESSION, 0 ) ;}
#line 4461 "yaccsrc.c"
    break;

  case 327: /* ex_when_stat: gruff when  */
#line 1068 "yaccsrc.y"
                                       { exiterror( ERR_WHEN_UNEXPECTED, 1 ); }
#line 4467 "yaccsrc.c"
    break;

  case 328: /* ex_when_stat: gruff otherwise  */
#line 1069 "yaccsrc.y"
                                       { exiterror( ERR_WHEN_UNEXPECTED, 2 ); }
#line 4473 "yaccsrc.c"
    break;

  case 329: /* otherwise_stat: otherwise nlncl nxstats  */
#line 1072 "yaccsrc.y"
                                         { yyval = yyvsp[-2];
                                         yyval->p[0] = optgluelast( yyvsp[-1], yyvsp[0] );
                                         if ( yyval->p[0] )
                                            yyval->p[0]->o.last = NULL; }
#line 4482 "yaccsrc.c"
    break;

  case 330: /* otherwise_stat: %empty  */
#line 1076 "yaccsrc.y"
                                       { yyval = makenode(X_NO_OTHERWISE,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 4490 "yaccsrc.c"
    break;

  case 331: /* signal_stat: signal VALUE expr  */
#line 1082 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_SIG_VAL ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4498 "yaccsrc.c"
    break;

  case 332: /* signal_stat: signal signal_name error  */
#line 1085 "yaccsrc.y"
                                       { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 4504 "yaccsrc.c"
    break;

  case 333: /* signal_stat: signal signal_name  */
#line 1086 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->name = (streng *)yyvsp[0] ; }
#line 4511 "yaccsrc.c"
    break;

  case 334: /* signal_stat: signal on error  */
#line 1088 "yaccsrc.y"
                              { exiterror( ERR_INV_SUBKEYWORD, 3, "ERROR FAILURE HALT NOTREADY NOVALUE SYNTAX LOSTDIGITS", __reginatext ) ;}
#line 4517 "yaccsrc.c"
    break;

  case 335: /* signal_stat: signal off error  */
#line 1089 "yaccsrc.y"
                               { exiterror( ERR_INV_SUBKEYWORD, 4, "ERROR FAILURE HALT NOTREADY NOVALUE SYNTAX LOSTDIGITS", __reginatext ) ;}
#line 4523 "yaccsrc.c"
    break;

  case 336: /* signal_stat: signal on s_action error  */
#line 1090 "yaccsrc.y"
                                       { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 4529 "yaccsrc.c"
    break;

  case 337: /* signal_stat: signal on s_action namespec error  */
#line 1091 "yaccsrc.y"
                                                { exiterror( ERR_STRING_EXPECTED, 3, __reginatext ) ;}
#line 4535 "yaccsrc.c"
    break;

  case 338: /* signal_stat: signal on s_action namespec  */
#line 1093 "yaccsrc.y"
                                       { yyval = yyvsp[-3] ;
                                         yyval->type = X_SIG_SET ;
                                         yyval->p[0] = yyvsp[-2] ;
                                         yyval->name = (streng *)yyvsp[0] ;
                                         yyval->p[1] = yyvsp[-1] ; }
#line 4545 "yaccsrc.c"
    break;

  case 339: /* signal_stat: signal on s_action  */
#line 1098 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_SIG_SET ;
                                         yyval->p[0] = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ; }
#line 4554 "yaccsrc.c"
    break;

  case 340: /* signal_stat: signal off s_action error  */
#line 1102 "yaccsrc.y"
                                        { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 4560 "yaccsrc.c"
    break;

  case 341: /* signal_stat: signal off s_action  */
#line 1103 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_SIG_SET ;
                                         yyval->p[0] = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ; }
#line 4569 "yaccsrc.c"
    break;

  case 342: /* signal_name: asymbol  */
#line 1109 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 4575 "yaccsrc.c"
    break;

  case 343: /* signal_name: STRING  */
#line 1110 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 4581 "yaccsrc.c"
    break;

  case 344: /* signal_name: error  */
#line 1111 "yaccsrc.y"
                                       { exiterror( ERR_STRING_EXPECTED, 4, __reginatext );}
#line 4587 "yaccsrc.c"
    break;

  case 345: /* namespec: NAME SIMSYMBOL  */
#line 1114 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue);}
#line 4593 "yaccsrc.c"
    break;

  case 346: /* namespec: NAME STRING  */
#line 1115 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 4599 "yaccsrc.c"
    break;

  case 347: /* namespec: NAME error  */
#line 1116 "yaccsrc.y"
                                       { exiterror( ERR_STRING_EXPECTED, 3, __reginatext ) ;}
#line 4605 "yaccsrc.c"
    break;

  case 348: /* asymbol: CONSYMBOL  */
#line 1119 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 4611 "yaccsrc.c"
    break;

  case 349: /* asymbol: SIMSYMBOL  */
#line 1120 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 4617 "yaccsrc.c"
    break;

  case 350: /* on: ON  */
#line 1123 "yaccsrc.y"
                                       { yyval = makenode(X_ON,0) ; }
#line 4623 "yaccsrc.c"
    break;

  case 351: /* off: OFF  */
#line 1126 "yaccsrc.y"
                                       { yyval = makenode(X_OFF,0) ; }
#line 4629 "yaccsrc.c"
    break;

  case 352: /* c_action: ERROR  */
#line 1129 "yaccsrc.y"
                                       { yyval = makenode(X_S_ERROR,0) ; }
#line 4635 "yaccsrc.c"
    break;

  case 353: /* c_action: HALT  */
#line 1130 "yaccsrc.y"
                                       { yyval = makenode(X_S_HALT,0) ; }
#line 4641 "yaccsrc.c"
    break;

  case 354: /* c_action: NOTREADY  */
#line 1131 "yaccsrc.y"
                                       { yyval = makenode(X_S_NOTREADY,0) ; }
#line 4647 "yaccsrc.c"
    break;

  case 355: /* c_action: FAILURE  */
#line 1132 "yaccsrc.y"
                                       { yyval = makenode(X_S_FAILURE,0) ; }
#line 4653 "yaccsrc.c"
    break;

  case 356: /* s_action: c_action  */
#line 1135 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 4659 "yaccsrc.c"
    break;

  case 357: /* s_action: NOVALUE  */
#line 1136 "yaccsrc.y"
                                       { yyval = makenode(X_S_NOVALUE,0) ; }
#line 4665 "yaccsrc.c"
    break;

  case 358: /* s_action: SYNTAX  */
#line 1137 "yaccsrc.y"
                                       { yyval = makenode(X_S_SYNTAX,0) ; }
#line 4671 "yaccsrc.c"
    break;

  case 359: /* s_action: LOSTDIGITS  */
#line 1138 "yaccsrc.y"
                                       { yyval = makenode(X_S_LOSTDIGITS,0) ; }
#line 4677 "yaccsrc.c"
    break;

  case 360: /* trace_stat: trace VALUE expr  */
#line 1141 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4684 "yaccsrc.c"
    break;

  case 361: /* trace_stat: trace nexpr  */
#line 1143 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4691 "yaccsrc.c"
    break;

  case 362: /* trace_stat: trace whatever error  */
#line 1145 "yaccsrc.y"
                                   { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 4697 "yaccsrc.c"
    break;

  case 363: /* trace_stat: trace whatever  */
#line 1146 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 4704 "yaccsrc.c"
    break;

  case 364: /* whatever: WHATEVER  */
#line 1150 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 4710 "yaccsrc.c"
    break;

  case 365: /* assignment: ass_part nexpr  */
#line 1154 "yaccsrc.y"
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
#line 4729 "yaccsrc.c"
    break;

  case 366: /* ass_part: ASSIGNMENTVARIABLE  */
#line 1170 "yaccsrc.y"
                                       { yyval = makenode(X_ASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4738 "yaccsrc.c"
    break;

  case 367: /* plusassignment: plus_ass_part nexpr  */
#line 1176 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "+=" );
                                       }
#line 4748 "yaccsrc.c"
    break;

  case 368: /* plus_ass_part: PLUSASSIGNMENTVARIABLE  */
#line 1183 "yaccsrc.y"
                                       { yyval = makenode(X_PLUSASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4757 "yaccsrc.c"
    break;

  case 369: /* minusassignment: minus_ass_part nexpr  */
#line 1189 "yaccsrc.y"
                                         { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "-=" );
                                       }
#line 4767 "yaccsrc.c"
    break;

  case 370: /* minus_ass_part: MINUSASSIGNMENTVARIABLE  */
#line 1196 "yaccsrc.y"
                                         { yyval = makenode(X_MINUSASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4776 "yaccsrc.c"
    break;

  case 371: /* multassignment: mult_ass_part nexpr  */
#line 1202 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "*=" );
                                       }
#line 4786 "yaccsrc.c"
    break;

  case 372: /* mult_ass_part: MULTASSIGNMENTVARIABLE  */
#line 1209 "yaccsrc.y"
                                       { yyval = makenode(X_MULTASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4795 "yaccsrc.c"
    break;

  case 373: /* divassignment: div_ass_part nexpr  */
#line 1215 "yaccsrc.y"
                                     { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "/=" );
                                       }
#line 4805 "yaccsrc.c"
    break;

  case 374: /* div_ass_part: DIVASSIGNMENTVARIABLE  */
#line 1222 "yaccsrc.y"
                                     { yyval = makenode(X_DIVASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4814 "yaccsrc.c"
    break;

  case 375: /* intdivassignment: intdiv_ass_part nexpr  */
#line 1228 "yaccsrc.y"
                                           { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "%=" );
                                       }
#line 4824 "yaccsrc.c"
    break;

  case 376: /* intdiv_ass_part: INTDIVASSIGNMENTVARIABLE  */
#line 1235 "yaccsrc.y"
                                           { yyval = makenode(X_INTDIVASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4833 "yaccsrc.c"
    break;

  case 377: /* modulusassignment: modulus_ass_part nexpr  */
#line 1241 "yaccsrc.y"
                                             { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "//=" );
                                       }
#line 4843 "yaccsrc.c"
    break;

  case 378: /* modulus_ass_part: MODULUSASSIGNMENTVARIABLE  */
#line 1248 "yaccsrc.y"
                                             { yyval = makenode(X_MODULUSASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4852 "yaccsrc.c"
    break;

  case 379: /* andassignment: and_ass_part nexpr  */
#line 1254 "yaccsrc.y"
                                     { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "&=" );
                                       }
#line 4862 "yaccsrc.c"
    break;

  case 380: /* and_ass_part: ANDASSIGNMENTVARIABLE  */
#line 1261 "yaccsrc.y"
                                     { yyval = makenode(X_ANDASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4871 "yaccsrc.c"
    break;

  case 381: /* xorassignment: xor_ass_part nexpr  */
#line 1267 "yaccsrc.y"
                                     { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "&&=" );
                                       }
#line 4881 "yaccsrc.c"
    break;

  case 382: /* xor_ass_part: XORASSIGNMENTVARIABLE  */
#line 1274 "yaccsrc.y"
                                     { yyval = makenode(X_XORASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4890 "yaccsrc.c"
    break;

  case 383: /* orassignment: or_ass_part nexpr  */
#line 1280 "yaccsrc.y"
                                   { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "|=" );
                                       }
#line 4900 "yaccsrc.c"
    break;

  case 384: /* or_ass_part: ORASSIGNMENTVARIABLE  */
#line 1287 "yaccsrc.y"
                                   { yyval = makenode(X_ORASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4909 "yaccsrc.c"
    break;

  case 385: /* concatassignment: concat_ass_part nexpr  */
#line 1293 "yaccsrc.y"
                                           { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                       }
#line 4917 "yaccsrc.c"
    break;

  case 386: /* concat_ass_part: CONCATASSIGNMENTVARIABLE  */
#line 1298 "yaccsrc.y"
                                           { yyval = makenode(X_CONCATASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4926 "yaccsrc.c"
    break;

  case 387: /* $@10: %empty  */
#line 1305 "yaccsrc.y"
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
#line 4947 "yaccsrc.c"
    break;

  case 388: /* expr: '(' $@10 exprs_sub  */
#line 1321 "yaccsrc.y"
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
#line 4969 "yaccsrc.c"
    break;

  case 389: /* $@11: %empty  */
#line 1338 "yaccsrc.y"
                                       { parendepth--; }
#line 4975 "yaccsrc.c"
    break;

  case 390: /* expr: expr '+' $@11 expr  */
#line 1339 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "+" );
                                         yyval = makenode( X_PLUSS, 2, yyvsp[-3], yyvsp[0] ); }
#line 4982 "yaccsrc.c"
    break;

  case 391: /* $@12: %empty  */
#line 1341 "yaccsrc.y"
                                       { parendepth--; }
#line 4988 "yaccsrc.c"
    break;

  case 392: /* expr: expr '-' $@12 expr  */
#line 1342 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "-" );
                                         yyval = makenode( X_MINUS, 2, yyvsp[-3], yyvsp[0] ); }
#line 4995 "yaccsrc.c"
    break;

  case 393: /* $@13: %empty  */
#line 1344 "yaccsrc.y"
                                       { parendepth--; }
#line 5001 "yaccsrc.c"
    break;

  case 394: /* expr: expr '*' $@13 expr  */
#line 1345 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "*" );
                                         yyval = makenode( X_MULT, 2, yyvsp[-3], yyvsp[0] ); }
#line 5008 "yaccsrc.c"
    break;

  case 395: /* expr: '*'  */
#line 1347 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "*" ); }
#line 5014 "yaccsrc.c"
    break;

  case 396: /* $@14: %empty  */
#line 1348 "yaccsrc.y"
                                       { parendepth--; }
#line 5020 "yaccsrc.c"
    break;

  case 397: /* expr: expr '/' $@14 expr  */
#line 1349 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "/" );
                                         yyval = makenode( X_DEVIDE, 2, yyvsp[-3], yyvsp[0] ); }
#line 5027 "yaccsrc.c"
    break;

  case 398: /* expr: '/'  */
#line 1351 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "/" ); }
#line 5033 "yaccsrc.c"
    break;

  case 399: /* $@15: %empty  */
#line 1352 "yaccsrc.y"
                                       { parendepth--; }
#line 5039 "yaccsrc.c"
    break;

  case 400: /* expr: expr MODULUS $@15 expr  */
#line 1353 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "//" );
                                         yyval = makenode( X_MODULUS, 2, yyvsp[-3], yyvsp[0] ); }
#line 5046 "yaccsrc.c"
    break;

  case 401: /* expr: MODULUS  */
#line 1355 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "//" ); }
#line 5052 "yaccsrc.c"
    break;

  case 402: /* $@16: %empty  */
#line 1356 "yaccsrc.y"
                                       { parendepth--; }
#line 5058 "yaccsrc.c"
    break;

  case 403: /* expr: expr '%' $@16 expr  */
#line 1357 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "%" );
                                         yyval = makenode( X_INTDIV, 2, yyvsp[-3], yyvsp[0] ); }
#line 5065 "yaccsrc.c"
    break;

  case 404: /* expr: '%'  */
#line 1359 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "%" ); }
#line 5071 "yaccsrc.c"
    break;

  case 405: /* $@17: %empty  */
#line 1360 "yaccsrc.y"
                                       { parendepth--; }
#line 5077 "yaccsrc.c"
    break;

  case 406: /* expr: expr '|' $@17 expr  */
#line 1361 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "|" );
                                         yyval = makenode( X_LOG_OR, 2, yyvsp[-3], yyvsp[0] ); }
#line 5084 "yaccsrc.c"
    break;

  case 407: /* expr: '|'  */
#line 1363 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "|" ); }
#line 5090 "yaccsrc.c"
    break;

  case 408: /* $@18: %empty  */
#line 1364 "yaccsrc.y"
                                       { parendepth--; }
#line 5096 "yaccsrc.c"
    break;

  case 409: /* expr: expr '&' $@18 expr  */
#line 1365 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "&" );
                                         yyval = makenode( X_LOG_AND, 2, yyvsp[-3], yyvsp[0] ); }
#line 5103 "yaccsrc.c"
    break;

  case 410: /* expr: '&'  */
#line 1367 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "&" ); }
#line 5109 "yaccsrc.c"
    break;

  case 411: /* $@19: %empty  */
#line 1368 "yaccsrc.y"
                                       { parendepth--; }
#line 5115 "yaccsrc.c"
    break;

  case 412: /* expr: expr XOR $@19 expr  */
#line 1369 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "&&" );
                                         yyval = makenode( X_LOG_XOR, 2, yyvsp[-3], yyvsp[0] ); }
#line 5122 "yaccsrc.c"
    break;

  case 413: /* expr: XOR  */
#line 1371 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "&&" ); }
#line 5128 "yaccsrc.c"
    break;

  case 414: /* $@20: %empty  */
#line 1372 "yaccsrc.y"
                                       { parendepth--; }
#line 5134 "yaccsrc.c"
    break;

  case 415: /* expr: expr EXP $@20 expr  */
#line 1373 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "**" );
                                         yyval = makenode( X_EXP, 2, yyvsp[-3], yyvsp[0] ); }
#line 5141 "yaccsrc.c"
    break;

  case 416: /* expr: EXP  */
#line 1375 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "**" ); }
#line 5147 "yaccsrc.c"
    break;

  case 417: /* $@21: %empty  */
#line 1376 "yaccsrc.y"
                                       { parendepth--; }
#line 5153 "yaccsrc.c"
    break;

  case 418: /* expr: expr SPACE $@21 expr  */
#line 1377 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], " " );
                                         yyval = makenode( X_SPACE, 2, yyvsp[-3], yyvsp[0] ); }
#line 5160 "yaccsrc.c"
    break;

  case 419: /* expr: SPACE  */
#line 1379 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, " " ); }
#line 5166 "yaccsrc.c"
    break;

  case 420: /* $@22: %empty  */
#line 1380 "yaccsrc.y"
                                       { parendepth--; }
#line 5172 "yaccsrc.c"
    break;

  case 421: /* expr: expr CONCATENATE $@22 expr  */
#line 1381 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "||" );
                                         yyval = makenode( X_CONCAT, 2, yyvsp[-3], yyvsp[0] ); }
#line 5179 "yaccsrc.c"
    break;

  case 422: /* expr: CONCATENATE  */
#line 1383 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "||" ); }
#line 5185 "yaccsrc.c"
    break;

  case 423: /* expr: NOT expr  */
#line 1384 "yaccsrc.y"
                                       { AUTO_REDUCE( yyvsp[0], "\\" );
                                         yyval = makenode( X_LOG_NOT, 1, yyvsp[0] ); }
#line 5192 "yaccsrc.c"
    break;

  case 424: /* expr: NOT  */
#line 1386 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "\\" ); }
#line 5198 "yaccsrc.c"
    break;

  case 425: /* $@23: %empty  */
#line 1387 "yaccsrc.y"
                                       { parendepth--; }
#line 5204 "yaccsrc.c"
    break;

  case 426: /* expr: expr OO_IS $@23 expr  */
#line 1388 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "IS" );
                                         /* OO, CREXX: x is .T */
                                         yyval = makenode( X_OO_IS, 2, yyvsp[-3], yyvsp[0] ); }
#line 5212 "yaccsrc.c"
    break;

  case 427: /* $@24: %empty  */
#line 1391 "yaccsrc.y"
                                       { parendepth--; }
#line 5218 "yaccsrc.c"
    break;

  case 428: /* expr: expr OO_AS $@24 expr  */
#line 1392 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "AS" );
                                         /* OO, CREXX: x as .T */
                                         yyval = makenode( X_OO_AS, 2, yyvsp[-3], yyvsp[0] ); }
#line 5226 "yaccsrc.c"
    break;

  case 429: /* $@25: %empty  */
#line 1395 "yaccsrc.y"
                                       { parendepth--; }
#line 5232 "yaccsrc.c"
    break;

  case 430: /* expr: expr '=' $@25 expr  */
#line 1396 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "=" );
                                         yyval = makenode( X_EQUAL, 2, yyvsp[-3], yyvsp[0] );
                                         transform( yyval ); }
#line 5240 "yaccsrc.c"
    break;

  case 431: /* expr: '='  */
#line 1399 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "=" ); }
#line 5246 "yaccsrc.c"
    break;

  case 432: /* $@26: %empty  */
#line 1400 "yaccsrc.y"
                                       { parendepth--; }
#line 5252 "yaccsrc.c"
    break;

  case 433: /* expr: expr GTE $@26 expr  */
#line 1401 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], ">=" );
                                         yyval = makenode( X_GTE, 2, yyvsp[-3], yyvsp[0] );
                                         transform( yyval ) ; }
#line 5260 "yaccsrc.c"
    break;

  case 434: /* expr: GTE  */
#line 1404 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, ">=" ); }
#line 5266 "yaccsrc.c"
    break;

  case 435: /* $@27: %empty  */
#line 1405 "yaccsrc.y"
                                       { parendepth--; }
#line 5272 "yaccsrc.c"
    break;

  case 436: /* expr: expr LTE $@27 expr  */
#line 1406 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "<=" );
                                         yyval = makenode( X_LTE, 2, yyvsp[-3], yyvsp[0] );
                                         transform( yyval ) ; }
#line 5280 "yaccsrc.c"
    break;

  case 437: /* expr: LTE  */
#line 1409 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "<=" ); }
#line 5286 "yaccsrc.c"
    break;

  case 438: /* $@28: %empty  */
#line 1410 "yaccsrc.y"
                                       { parendepth--; }
#line 5292 "yaccsrc.c"
    break;

  case 439: /* expr: expr GT $@28 expr  */
#line 1411 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], ">" );
                                         yyval = makenode( X_GT, 2, yyvsp[-3], yyvsp[0] );
                                         transform( yyval ) ; }
#line 5300 "yaccsrc.c"
    break;

  case 440: /* expr: GT  */
#line 1414 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, ">" ); }
#line 5306 "yaccsrc.c"
    break;

  case 441: /* $@29: %empty  */
#line 1415 "yaccsrc.y"
                                       { parendepth--; }
#line 5312 "yaccsrc.c"
    break;

  case 442: /* expr: expr LT $@29 expr  */
#line 1416 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "<" );
                                         yyval = makenode( X_LT, 2, yyvsp[-3], yyvsp[0] );
                                         transform( yyval ) ; }
#line 5320 "yaccsrc.c"
    break;

  case 443: /* expr: LT  */
#line 1419 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "<" ); }
#line 5326 "yaccsrc.c"
    break;

  case 444: /* $@30: %empty  */
#line 1420 "yaccsrc.y"
                                       { parendepth--; }
#line 5332 "yaccsrc.c"
    break;

  case 445: /* expr: expr DIFFERENT $@30 expr  */
#line 1421 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "\\=" );
                                         yyval = makenode( X_DIFF, 2, yyvsp[-3], yyvsp[0] );
                                         transform( yyval ) ; }
#line 5340 "yaccsrc.c"
    break;

  case 446: /* expr: DIFFERENT  */
#line 1424 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "\\=" ); }
#line 5346 "yaccsrc.c"
    break;

  case 447: /* $@31: %empty  */
#line 1425 "yaccsrc.y"
                                       { parendepth--; }
#line 5352 "yaccsrc.c"
    break;

  case 448: /* expr: expr EQUALEQUAL $@31 expr  */
#line 1426 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "==" );
                                         yyval = makenode( X_S_EQUAL, 2, yyvsp[-3], yyvsp[0] ); }
#line 5359 "yaccsrc.c"
    break;

  case 449: /* expr: EQUALEQUAL  */
#line 1428 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "==" ); }
#line 5365 "yaccsrc.c"
    break;

  case 450: /* $@32: %empty  */
#line 1429 "yaccsrc.y"
                                       { parendepth--; }
#line 5371 "yaccsrc.c"
    break;

  case 451: /* expr: expr NOTEQUALEQUAL $@32 expr  */
#line 1430 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "\\==" );
                                         yyval = makenode( X_S_DIFF, 2, yyvsp[-3], yyvsp[0] ); }
#line 5378 "yaccsrc.c"
    break;

  case 452: /* expr: NOTEQUALEQUAL  */
#line 1432 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "\\==" ); }
#line 5384 "yaccsrc.c"
    break;

  case 453: /* $@33: %empty  */
#line 1433 "yaccsrc.y"
                                       { parendepth--; }
#line 5390 "yaccsrc.c"
    break;

  case 454: /* expr: expr GTGT $@33 expr  */
#line 1434 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], ">>" );
                                         yyval = makenode( X_S_GT, 2, yyvsp[-3], yyvsp[0] ); }
#line 5397 "yaccsrc.c"
    break;

  case 455: /* expr: GTGT  */
#line 1436 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, ">>" ); }
#line 5403 "yaccsrc.c"
    break;

  case 456: /* $@34: %empty  */
#line 1437 "yaccsrc.y"
                                       { parendepth--; }
#line 5409 "yaccsrc.c"
    break;

  case 457: /* expr: expr LTLT $@34 expr  */
#line 1438 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "<<" );
                                         yyval = makenode( X_S_LT, 2, yyvsp[-3], yyvsp[0] ); }
#line 5416 "yaccsrc.c"
    break;

  case 458: /* expr: LTLT  */
#line 1440 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "<<" ); }
#line 5422 "yaccsrc.c"
    break;

  case 459: /* $@35: %empty  */
#line 1441 "yaccsrc.y"
                                       { parendepth--; }
#line 5428 "yaccsrc.c"
    break;

  case 460: /* expr: expr NOTGTGT $@35 expr  */
#line 1442 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "\\>>" );
                                         yyval = makenode( X_S_NGT, 2, yyvsp[-3], yyvsp[0] ); }
#line 5435 "yaccsrc.c"
    break;

  case 461: /* expr: NOTGTGT  */
#line 1444 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "\\>>" ); }
#line 5441 "yaccsrc.c"
    break;

  case 462: /* $@36: %empty  */
#line 1445 "yaccsrc.y"
                                       { parendepth--; }
#line 5447 "yaccsrc.c"
    break;

  case 463: /* expr: expr NOTLTLT $@36 expr  */
#line 1446 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "\\<<" );
                                         yyval = makenode( X_S_NLT, 2, yyvsp[-3], yyvsp[0] ); }
#line 5454 "yaccsrc.c"
    break;

  case 464: /* expr: NOTLTLT  */
#line 1448 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "\\<<" ); }
#line 5460 "yaccsrc.c"
    break;

  case 465: /* $@37: %empty  */
#line 1449 "yaccsrc.y"
                                       { parendepth--; }
#line 5466 "yaccsrc.c"
    break;

  case 466: /* expr: expr GTGTE $@37 expr  */
#line 1450 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], ">>=" );
                                         yyval = makenode( X_S_GTE, 2, yyvsp[-3], yyvsp[0] ); }
#line 5473 "yaccsrc.c"
    break;

  case 467: /* expr: GTGTE  */
#line 1452 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, ">>=" ); }
#line 5479 "yaccsrc.c"
    break;

  case 468: /* $@38: %empty  */
#line 1453 "yaccsrc.y"
                                       { parendepth--; }
#line 5485 "yaccsrc.c"
    break;

  case 469: /* expr: expr LTLTE $@38 expr  */
#line 1454 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "<<=" );
                                         yyval = makenode( X_S_LTE, 2, yyvsp[-3], yyvsp[0] ); }
#line 5492 "yaccsrc.c"
    break;

  case 470: /* expr: LTLTE  */
#line 1456 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "<<=" ); }
#line 5498 "yaccsrc.c"
    break;

  case 471: /* expr: expr MSGSEND msgtarget  */
#line 1457 "yaccsrc.y"
                                       { AUTO_REDUCE( yyvsp[-2], __regina_oo_operator );
                                         yyval = yyvsp[0];
                                         yyval->type = X_MSGSEND;
                                         yyval->p[0] = yyvsp[-2]; }
#line 5507 "yaccsrc.c"
    break;

  case 472: /* expr: expr CASCADE msgtarget  */
#line 1461 "yaccsrc.y"
                                       { AUTO_REDUCE( yyvsp[-2], __regina_oo_operator );
                                         yyval = yyvsp[0];
                                         yyval->type = X_CASCADE;
                                         yyval->p[0] = yyvsp[-2]; }
#line 5516 "yaccsrc.c"
    break;

  case 473: /* expr: MSGSEND  */
#line 1465 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, __regina_oo_operator ); }
#line 5522 "yaccsrc.c"
    break;

  case 474: /* expr: CASCADE  */
#line 1466 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, __regina_oo_operator ); }
#line 5528 "yaccsrc.c"
    break;

  case 475: /* expr: expr '[' bracket_args  */
#line 1467 "yaccsrc.y"
                                       { AUTO_REDUCE( yyvsp[-2], "[" );
                                         yyval = makenode( X_INDEX, 2, yyvsp[-2], yyvsp[0] ); }
#line 5535 "yaccsrc.c"
    break;

  case 476: /* expr: symbtree  */
#line 1469 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 5541 "yaccsrc.c"
    break;

  case 477: /* expr: CONSYMBOL  */
#line 1470 "yaccsrc.y"
                                       { /* OO: .NAME may be an environment
                                          * symbol, see OO_IS_ENVSYMBOL */
                                         yyval = makenode( ( retvalue[0] == '.'
                                                          && rx_isalpha( retvalue[1] )
                                                          && !__regina_oo_off() )
                                                        ? X_CON_SYMBOL : X_STRING, 0 );
                                         yyval->name = Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 5553 "yaccsrc.c"
    break;

  case 478: /* expr: HEXSTRING  */
#line 1477 "yaccsrc.y"
                                       { yyval = makenode( X_STRING, 0 );
                                         yyval->name = Str_make_TSD(parser_data.TSD,retlength) ;
                                         memcpy(yyval->name->value,retvalue,
                                                    yyval->name->len=retlength); }
#line 5562 "yaccsrc.c"
    break;

  case 479: /* expr: BINSTRING  */
#line 1481 "yaccsrc.y"
                                       { yyval = makenode( X_STRING, 0 );
                                         yyval->name = Str_make_TSD(parser_data.TSD,retlength) ;
                                         memcpy(yyval->name->value,retvalue,
                                                    yyval->name->len=retlength); }
#line 5571 "yaccsrc.c"
    break;

  case 480: /* expr: STRING  */
#line 1485 "yaccsrc.y"
                                       { yyval = makenode( X_STRING, 0 );
                                         yyval->name = Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 5578 "yaccsrc.c"
    break;

  case 481: /* expr: BLOCK  */
#line 1487 "yaccsrc.y"
                                       { /* OO: { clauses }, see oo.c */
                                         int line;
                                         yyval = makenode( X_OO_BLOCK, 0 );
                                         yyval->name = __regina_block_take( &line );
                                         yyval->lineno = line; }
#line 5588 "yaccsrc.c"
    break;

  case 482: /* expr: function  */
#line 1492 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 5594 "yaccsrc.c"
    break;

  case 483: /* expr: '+' expr  */
#line 1493 "yaccsrc.y"
                                       { AUTO_REDUCE( yyvsp[0], rxnullptr );
                                         yyval = makenode( X_U_PLUSS, 1, yyvsp[0] ); }
#line 5601 "yaccsrc.c"
    break;

  case 484: /* expr: '-' expr  */
#line 1495 "yaccsrc.y"
                                       { AUTO_REDUCE( yyvsp[0], rxnullptr );
                                         yyval = makenode( X_U_MINUS, 1, yyvsp[0] ); }
#line 5608 "yaccsrc.c"
    break;

  case 485: /* expr: '+' error  */
#line 1497 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, __reginatext ); }
#line 5614 "yaccsrc.c"
    break;

  case 486: /* expr: '-' error  */
#line 1498 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, __reginatext ); }
#line 5620 "yaccsrc.c"
    break;

  case 487: /* exprs_sub: exprs ')'  */
#line 1501 "yaccsrc.y"
                                       { yyval = yyvsp[-1]; }
#line 5626 "yaccsrc.c"
    break;

  case 488: /* exprs_sub: exprs error  */
#line 1502 "yaccsrc.y"
                                       { exiterror( ERR_UNMATCHED_PARAN, 0 ); }
#line 5632 "yaccsrc.c"
    break;

  case 489: /* exprs_sub: STATSEP  */
#line 1503 "yaccsrc.y"
                                       { exiterror( ERR_UNMATCHED_PARAN, 0 ); }
#line 5638 "yaccsrc.c"
    break;

  case 490: /* symbtree: SIMSYMBOL  */
#line 1506 "yaccsrc.y"
                                       { yyval = (nodeptr)create_head( (const char *)retvalue ) ; }
#line 5644 "yaccsrc.c"
    break;

  case 491: /* function: extfunc func_args  */
#line 1510 "yaccsrc.y"
                                       { yyval = makenode(X_EX_FUNC,1,yyvsp[0]) ;
                                         yyval->name = (streng *)yyvsp[-1] ; }
#line 5651 "yaccsrc.c"
    break;

  case 492: /* function: intfunc func_args  */
#line 1512 "yaccsrc.y"
                                       { yyval = makenode(X_IN_FUNC,1,yyvsp[0]) ;
                                         yyval->name = (streng *)yyvsp[-1] ; }
#line 5658 "yaccsrc.c"
    break;

  case 493: /* @39: %empty  */
#line 1516 "yaccsrc.y"
                                       { /* ugly fake preservs parendepth */
                                         yyval = (YYSTYPE) parendepth;
                                         parendepth = 0; }
#line 5666 "yaccsrc.c"
    break;

  case 494: /* func_args: @39 exprs_sub  */
#line 1519 "yaccsrc.y"
                                       { parendepth = (int) yyval;
                                         yyval = yyvsp[0]; }
#line 5673 "yaccsrc.c"
    break;

  case 495: /* msgtarget: msgname  */
#line 1528 "yaccsrc.y"
                                       { yyval = makenode( X_MSGSEND, 0 );
                                         yyval->name = (streng *) yyvsp[0]; }
#line 5680 "yaccsrc.c"
    break;

  case 496: /* msgtarget: msgname MSGSUPER  */
#line 1530 "yaccsrc.y"
                                       { /* ~name:super, see oo.c */
                                         yyval = makenode( X_MSGSEND, 0 );
                                         yyval->name = (streng *) yyvsp[-1];
                                         yyval->u.nonansi = 1; }
#line 5689 "yaccsrc.c"
    break;

  case 497: /* msgtarget: msgname MSGSUPER '(' func_args  */
#line 1535 "yaccsrc.y"
                                       { yyval = makenode( X_MSGSEND, 2, NULL, yyvsp[0] );
                                         yyval->name = (streng *) yyvsp[-3];
                                         yyval->u.nonansi = 1; }
#line 5697 "yaccsrc.c"
    break;

  case 498: /* msgtarget: STRING  */
#line 1538 "yaccsrc.y"
                                       { yyval = makenode( X_MSGSEND, 0 );
                                         yyval->name = Str_cre_TSD( parser_data.TSD, retvalue );
                                         mem_upper( yyval->name->value, yyval->name->len ); }
#line 5705 "yaccsrc.c"
    break;

  case 499: /* msgtarget: intfunc func_args  */
#line 1541 "yaccsrc.y"
                                       { yyval = makenode( X_MSGSEND, 2, NULL, yyvsp[0] );
                                         yyval->name = (streng *) yyvsp[-1];
                                         mem_upper( yyval->name->value, yyval->name->len ); }
#line 5713 "yaccsrc.c"
    break;

  case 500: /* msgtarget: extfunc func_args  */
#line 1544 "yaccsrc.y"
                                       { yyval = makenode( X_MSGSEND, 2, NULL, yyvsp[0] );
                                         yyval->name = (streng *) yyvsp[-1];
                                         mem_upper( yyval->name->value, yyval->name->len ); }
#line 5721 "yaccsrc.c"
    break;

  case 501: /* msgname: SIMSYMBOL  */
#line 1549 "yaccsrc.y"
                                       { yyval = (nodeptr) Str_cre_TSD( parser_data.TSD, retvalue ); }
#line 5727 "yaccsrc.c"
    break;

  case 502: /* @40: %empty  */
#line 1552 "yaccsrc.y"
                                       { /* like func_args, with "]" */
                                         yyval = (YYSTYPE) parendepth;
                                         parendepth = 0; }
#line 5735 "yaccsrc.c"
    break;

  case 503: /* bracket_args: @40 exprs ']'  */
#line 1555 "yaccsrc.y"
                                       { parendepth = (int) yyval;
                                         yyval = yyvsp[-1]; }
#line 5742 "yaccsrc.c"
    break;

  case 504: /* intfunc: INFUNCNAME  */
#line 1559 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 5748 "yaccsrc.c"
    break;

  case 505: /* extfunc: EXFUNCNAME  */
#line 1562 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 5754 "yaccsrc.c"
    break;

  case 506: /* template: pv solid template  */
#line 1565 "yaccsrc.y"
                                       { yyval =makenode(X_TPL_SOLID,3,yyvsp[-2],yyvsp[-1],yyvsp[0]);}
#line 5760 "yaccsrc.c"
    break;

  case 507: /* template: pv  */
#line 1566 "yaccsrc.y"
                                       { yyval =makenode(X_TPL_SOLID,1,yyvsp[0]) ; }
#line 5766 "yaccsrc.c"
    break;

  case 508: /* template: error  */
#line 1567 "yaccsrc.y"
                    { exiterror( ERR_INVALID_TEMPLATE, 1, __reginatext ) ;}
#line 5772 "yaccsrc.c"
    break;

  case 509: /* solid: '-' offset  */
#line 1570 "yaccsrc.y"
                                       { yyval = makenode(X_NEG_OFFS,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 5779 "yaccsrc.c"
    break;

  case 510: /* solid: '+' offset  */
#line 1572 "yaccsrc.y"
                                       { yyval = makenode(X_POS_OFFS,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 5786 "yaccsrc.c"
    break;

  case 511: /* solid: offset  */
#line 1574 "yaccsrc.y"
                                       { yyval = makenode(X_ABS_OFFS,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 5793 "yaccsrc.c"
    break;

  case 512: /* solid: '=' offset  */
#line 1576 "yaccsrc.y"
                                       { yyval = makenode(X_ABS_OFFS,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 5800 "yaccsrc.c"
    break;

  case 513: /* solid: '(' symbtree ')'  */
#line 1578 "yaccsrc.y"
                                       { yyval = makenode(X_TPL_VAR,0) ;
                                         yyval->p[0] = yyvsp[-1] ; }
#line 5807 "yaccsrc.c"
    break;

  case 514: /* solid: '-' '(' symbtree ')'  */
#line 1580 "yaccsrc.y"
                                       { yyval = makenode(X_NEG_OFFS,0) ;
                                         yyval->p[0] = yyvsp[-1] ; }
#line 5814 "yaccsrc.c"
    break;

  case 515: /* solid: '+' '(' symbtree ')'  */
#line 1582 "yaccsrc.y"
                                       { yyval = makenode(X_POS_OFFS,0) ;
                                         yyval->p[0] = yyvsp[-1] ; }
#line 5821 "yaccsrc.c"
    break;

  case 516: /* solid: '=' '(' symbtree ')'  */
#line 1584 "yaccsrc.y"
                                       { yyval = makenode(X_ABS_OFFS,0) ;
                                         yyval->p[0] = yyvsp[-1] ; }
#line 5828 "yaccsrc.c"
    break;

  case 517: /* solid: string  */
#line 1586 "yaccsrc.y"
                                       { yyval = makenode(X_TPL_MVE,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 5835 "yaccsrc.c"
    break;

  case 518: /* offset: OFFSET  */
#line 1590 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 5841 "yaccsrc.c"
    break;

  case 519: /* offset: CONSYMBOL  */
#line 1591 "yaccsrc.y"
                                       { streng *sptr = Str_cre_TSD(parser_data.TSD,retvalue) ;
                                          if (myisnumber(parser_data.TSD, sptr))
                                          {
                                             exiterror( ERR_INVALID_INTEGER, 4, sptr->value ) ;
                                          }
                                          else
                                             exiterror( ERR_INVALID_TEMPLATE, 0 ) ;}
#line 5853 "yaccsrc.c"
    break;

  case 520: /* string: STRING  */
#line 1600 "yaccsrc.y"
                                       { yyval = (nodeptr) Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 5859 "yaccsrc.c"
    break;

  case 521: /* string: HEXSTRING  */
#line 1601 "yaccsrc.y"
                                       { streng *sptr = Str_make_TSD(parser_data.TSD,retlength) ;
                                         memcpy(sptr->value,retvalue,
                                                 sptr->len=retlength) ;
                                         yyval = (nodeptr) sptr ; }
#line 5868 "yaccsrc.c"
    break;

  case 522: /* string: BINSTRING  */
#line 1605 "yaccsrc.y"
                                       { streng *sptr = Str_make_TSD(parser_data.TSD,retlength) ;
                                         memcpy(sptr->value,retvalue,
                                                 sptr->len=retlength) ;
                                         yyval = (nodeptr) sptr ; }
#line 5877 "yaccsrc.c"
    break;

  case 523: /* pv: PLACEHOLDER pv  */
#line 1611 "yaccsrc.y"
                                       { yyval = makenode(X_TPL_POINT,1,yyvsp[0]) ; }
#line 5883 "yaccsrc.c"
    break;

  case 524: /* pv: symbtree pv  */
#line 1612 "yaccsrc.y"
                                       { yyval = makenode(X_TPL_SYMBOL,2,yyvsp[0],yyvsp[-1]) ; }
#line 5889 "yaccsrc.c"
    break;

  case 525: /* pv: %empty  */
#line 1613 "yaccsrc.y"
                                       { yyval = NULL ; }
#line 5895 "yaccsrc.c"
    break;

  case 526: /* $@41: %empty  */
#line 1623 "yaccsrc.y"
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
#line 5943 "yaccsrc.c"
    break;

  case 527: /* exprs: nexpr ',' $@41 exprs  */
#line 1666 "yaccsrc.y"
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
#line 5985 "yaccsrc.c"
    break;

  case 528: /* exprs: nexpr  */
#line 1703 "yaccsrc.y"
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
#line 6032 "yaccsrc.c"
    break;

  case 529: /* nexpr: expr  */
#line 1747 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 6038 "yaccsrc.c"
    break;

  case 530: /* nexpr: %empty  */
#line 1748 "yaccsrc.y"
                                       { yyval = NULL ; }
#line 6044 "yaccsrc.c"
    break;

  case 531: /* anyvars: xsimsymb anyvars  */
#line 1751 "yaccsrc.y"
                                        { yyval = makenode(X_SIM_SYMBOL,1,yyvsp[0]) ;
                                         yyval->name = (streng *) yyvsp[-1] ; }
#line 6051 "yaccsrc.c"
    break;

  case 532: /* anyvars: xsimsymb  */
#line 1753 "yaccsrc.y"
                                        { yyval = makenode(X_SIM_SYMBOL,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 6058 "yaccsrc.c"
    break;

  case 533: /* anyvars: '(' xsimsymb ')' anyvars  */
#line 1755 "yaccsrc.y"
                                        { yyval = makenode(X_IND_SYMBOL,1,yyvsp[0]) ;
                                         yyval->name = (streng *) yyvsp[-2] ; }
#line 6065 "yaccsrc.c"
    break;

  case 534: /* anyvars: '(' xsimsymb ')'  */
#line 1757 "yaccsrc.y"
                                        { yyval = makenode(X_IND_SYMBOL,0) ;
                                         yyval->name = (streng *) yyvsp[-1] ; }
#line 6072 "yaccsrc.c"
    break;

  case 535: /* xsimsymb: SIMSYMBOL  */
#line 1761 "yaccsrc.y"
                                       { yyval = (treenode *) Str_cre_TSD(parser_data.TSD,retvalue);}
#line 6078 "yaccsrc.c"
    break;

  case 536: /* simsymb: SIMSYMBOL  */
#line 1764 "yaccsrc.y"
                                       { yyval = (treenode *) Str_cre_TSD(parser_data.TSD,retvalue);}
#line 6084 "yaccsrc.c"
    break;

  case 537: /* simsymb: error  */
#line 1765 "yaccsrc.y"
                                       { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ) ;}
#line 6090 "yaccsrc.c"
    break;


#line 6094 "yaccsrc.c"

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

#line 1768 "yaccsrc.y"


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

