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
static void transform_do_over( nodeptr dostat, nodeptr var );
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
#line 159 "yaccsrc.y"

#ifdef NDEBUG
# define YYDEBUG 0
#else
# define YYDEBUG 1
#endif

#line 210 "yaccsrc.c"

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
  YYSYMBOL_GTGT = 88,                      /* GTGT  */
  YYSYMBOL_LTLT = 89,                      /* LTLT  */
  YYSYMBOL_NOTGTGT = 90,                   /* NOTGTGT  */
  YYSYMBOL_NOTLTLT = 91,                   /* NOTLTLT  */
  YYSYMBOL_GTGTE = 92,                     /* GTGTE  */
  YYSYMBOL_LTLTE = 93,                     /* LTLTE  */
  YYSYMBOL_INPUT = 94,                     /* INPUT  */
  YYSYMBOL_OUTPUT = 95,                    /* OUTPUT  */
  YYSYMBOL_ERROR = 96,                     /* ERROR  */
  YYSYMBOL_NORMAL = 97,                    /* NORMAL  */
  YYSYMBOL_APPEND = 98,                    /* APPEND  */
  YYSYMBOL_REPLACE = 99,                   /* REPLACE  */
  YYSYMBOL_STREAM = 100,                   /* STREAM  */
  YYSYMBOL_STEM = 101,                     /* STEM  */
  YYSYMBOL_LIFO = 102,                     /* LIFO  */
  YYSYMBOL_FIFO = 103,                     /* FIFO  */
  YYSYMBOL_NOEOL = 104,                    /* NOEOL  */
  YYSYMBOL_LOWER = 105,                    /* LOWER  */
  YYSYMBOL_CASELESS = 106,                 /* CASELESS  */
  YYSYMBOL_MSGSEND = 107,                  /* MSGSEND  */
  YYSYMBOL_CASCADE = 108,                  /* CASCADE  */
  YYSYMBOL_DCOLON = 109,                   /* DCOLON  */
  YYSYMBOL_USEARG = 110,                   /* USEARG  */
  YYSYMBOL_MSGSUPER = 111,                 /* MSGSUPER  */
  YYSYMBOL_PLUSASSIGNMENTVARIABLE = 112,   /* PLUSASSIGNMENTVARIABLE  */
  YYSYMBOL_MINUSASSIGNMENTVARIABLE = 113,  /* MINUSASSIGNMENTVARIABLE  */
  YYSYMBOL_MULTASSIGNMENTVARIABLE = 114,   /* MULTASSIGNMENTVARIABLE  */
  YYSYMBOL_DIVASSIGNMENTVARIABLE = 115,    /* DIVASSIGNMENTVARIABLE  */
  YYSYMBOL_MODULUSASSIGNMENTVARIABLE = 116, /* MODULUSASSIGNMENTVARIABLE  */
  YYSYMBOL_INTDIVASSIGNMENTVARIABLE = 117, /* INTDIVASSIGNMENTVARIABLE  */
  YYSYMBOL_ORASSIGNMENTVARIABLE = 118,     /* ORASSIGNMENTVARIABLE  */
  YYSYMBOL_XORASSIGNMENTVARIABLE = 119,    /* XORASSIGNMENTVARIABLE  */
  YYSYMBOL_ANDASSIGNMENTVARIABLE = 120,    /* ANDASSIGNMENTVARIABLE  */
  YYSYMBOL_CONCATASSIGNMENTVARIABLE = 121, /* CONCATASSIGNMENTVARIABLE  */
  YYSYMBOL_122_ = 122,                     /* '|'  */
  YYSYMBOL_123_ = 123,                     /* '&'  */
  YYSYMBOL_124_ = 124,                     /* '='  */
  YYSYMBOL_CCAT = 125,                     /* CCAT  */
  YYSYMBOL_126_ = 126,                     /* '+'  */
  YYSYMBOL_127_ = 127,                     /* '-'  */
  YYSYMBOL_128_ = 128,                     /* '*'  */
  YYSYMBOL_129_ = 129,                     /* '/'  */
  YYSYMBOL_130_ = 130,                     /* '%'  */
  YYSYMBOL_UMINUS = 131,                   /* UMINUS  */
  YYSYMBOL_UPLUS = 132,                    /* UPLUS  */
  YYSYMBOL_133_ = 133,                     /* '['  */
  YYSYMBOL_134_ = 134,                     /* ','  */
  YYSYMBOL_135_ = 135,                     /* '('  */
  YYSYMBOL_136_ = 136,                     /* ')'  */
  YYSYMBOL_137_ = 137,                     /* ']'  */
  YYSYMBOL_YYACCEPT = 138,                 /* $accept  */
  YYSYMBOL_start = 139,                    /* start  */
  YYSYMBOL_140_1 = 140,                    /* $@1  */
  YYSYMBOL_prog = 141,                     /* prog  */
  YYSYMBOL_dirsect = 142,                  /* dirsect  */
  YYSYMBOL_dirblock = 143,                 /* dirblock  */
  YYSYMBOL_directive = 144,                /* directive  */
  YYSYMBOL_145_2 = 145,                    /* @2  */
  YYSYMBOL_dirwords = 146,                 /* dirwords  */
  YYSYMBOL_dirword = 147,                  /* dirword  */
  YYSYMBOL_stats = 148,                    /* stats  */
  YYSYMBOL_xstats = 149,                   /* xstats  */
  YYSYMBOL_ystatement = 150,               /* ystatement  */
  YYSYMBOL_lonely_end = 151,               /* lonely_end  */
  YYSYMBOL_nxstats = 152,                  /* nxstats  */
  YYSYMBOL_ncl = 153,                      /* ncl  */
  YYSYMBOL_nlncl = 154,                    /* nlncl  */
  YYSYMBOL_optLabels = 155,                /* optLabels  */
  YYSYMBOL_statement = 156,                /* statement  */
  YYSYMBOL_gruff = 157,                    /* gruff  */
  YYSYMBOL_mttstatement = 158,             /* mttstatement  */
  YYSYMBOL_mtstatement = 159,              /* mtstatement  */
  YYSYMBOL_nclstatement = 160,             /* nclstatement  */
  YYSYMBOL_oo_expose_stat = 161,           /* oo_expose_stat  */
  YYSYMBOL_oo_usearg_stat = 162,           /* oo_usearg_stat  */
  YYSYMBOL_usevars = 163,                  /* usevars  */
  YYSYMBOL_usevar = 164,                   /* usevar  */
  YYSYMBOL_call = 165,                     /* call  */
  YYSYMBOL_do = 166,                       /* do  */
  YYSYMBOL_exit = 167,                     /* exit  */
  YYSYMBOL_if = 168,                       /* if  */
  YYSYMBOL_iterate = 169,                  /* iterate  */
  YYSYMBOL_leave = 170,                    /* leave  */
  YYSYMBOL_say = 171,                      /* say  */
  YYSYMBOL_return = 172,                   /* return  */
  YYSYMBOL_address = 173,                  /* address  */
  YYSYMBOL_arg = 174,                      /* arg  */
  YYSYMBOL_drop = 175,                     /* drop  */
  YYSYMBOL_interpret = 176,                /* interpret  */
  YYSYMBOL_label = 177,                    /* label  */
  YYSYMBOL_nop = 178,                      /* nop  */
  YYSYMBOL_numeric = 179,                  /* numeric  */
  YYSYMBOL_options = 180,                  /* options  */
  YYSYMBOL_parse = 181,                    /* parse  */
  YYSYMBOL_proc = 182,                     /* proc  */
  YYSYMBOL_pull = 183,                     /* pull  */
  YYSYMBOL_push = 184,                     /* push  */
  YYSYMBOL_queue = 185,                    /* queue  */
  YYSYMBOL_select = 186,                   /* select  */
  YYSYMBOL_signal = 187,                   /* signal  */
  YYSYMBOL_when = 188,                     /* when  */
  YYSYMBOL_otherwise = 189,                /* otherwise  */
  YYSYMBOL_trace = 190,                    /* trace  */
  YYSYMBOL_upper = 191,                    /* upper  */
  YYSYMBOL_address_stat = 192,             /* address_stat  */
  YYSYMBOL_193_3 = 193,                    /* @3  */
  YYSYMBOL_address_stat2 = 194,            /* address_stat2  */
  YYSYMBOL_195_4 = 195,                    /* $@4  */
  YYSYMBOL_arg_stat = 196,                 /* arg_stat  */
  YYSYMBOL_call_stat = 197,                /* call_stat  */
  YYSYMBOL_198_5 = 198,                    /* $@5  */
  YYSYMBOL_199_6 = 199,                    /* $@6  */
  YYSYMBOL_call_name = 200,                /* call_name  */
  YYSYMBOL_call_args = 201,                /* call_args  */
  YYSYMBOL_expr_stat = 202,                /* expr_stat  */
  YYSYMBOL_end_stat = 203,                 /* end_stat  */
  YYSYMBOL_end = 204,                      /* end  */
  YYSYMBOL_do_stat = 205,                  /* do_stat  */
  YYSYMBOL_repetitor = 206,                /* repetitor  */
  YYSYMBOL_nvir = 207,                     /* nvir  */
  YYSYMBOL_naddr_with = 208,               /* naddr_with  */
  YYSYMBOL_209_7 = 209,                    /* @7  */
  YYSYMBOL_addr_with = 210,                /* addr_with  */
  YYSYMBOL_connection = 211,               /* connection  */
  YYSYMBOL_inputstmts = 212,               /* inputstmts  */
  YYSYMBOL_outputstmts = 213,              /* outputstmts  */
  YYSYMBOL_errorstmts = 214,               /* errorstmts  */
  YYSYMBOL_adeo = 215,                     /* adeo  */
  YYSYMBOL_adei = 216,                     /* adei  */
  YYSYMBOL_adio = 217,                     /* adio  */
  YYSYMBOL_inputstmt = 218,                /* inputstmt  */
  YYSYMBOL_outputstmt = 219,               /* outputstmt  */
  YYSYMBOL_errorstmt = 220,                /* errorstmt  */
  YYSYMBOL_resourcei = 221,                /* resourcei  */
  YYSYMBOL_resourceo = 222,                /* resourceo  */
  YYSYMBOL_resources = 223,                /* resources  */
  YYSYMBOL_nsimsymb = 224,                 /* nsimsymb  */
  YYSYMBOL_225_8 = 225,                    /* $@8  */
  YYSYMBOL_nnvir = 226,                    /* nnvir  */
  YYSYMBOL_227_9 = 227,                    /* $@9  */
  YYSYMBOL_nspace = 228,                   /* nspace  */
  YYSYMBOL_addrAll = 229,                  /* addrAll  */
  YYSYMBOL_addrSim = 230,                  /* addrSim  */
  YYSYMBOL_addrString = 231,               /* addrString  */
  YYSYMBOL_dovar = 232,                    /* dovar  */
  YYSYMBOL_tobyfor = 233,                  /* tobyfor  */
  YYSYMBOL_conditional = 234,              /* conditional  */
  YYSYMBOL_drop_stat = 235,                /* drop_stat  */
  YYSYMBOL_upper_stat = 236,               /* upper_stat  */
  YYSYMBOL_exit_stat = 237,                /* exit_stat  */
  YYSYMBOL_if_stat = 238,                  /* if_stat  */
  YYSYMBOL_unexp_then = 239,               /* unexp_then  */
  YYSYMBOL_unexp_else = 240,               /* unexp_else  */
  YYSYMBOL_ipret_stat = 241,               /* ipret_stat  */
  YYSYMBOL_iterate_stat = 242,             /* iterate_stat  */
  YYSYMBOL_label_stat = 243,               /* label_stat  */
  YYSYMBOL_labelname = 244,                /* labelname  */
  YYSYMBOL_leave_stat = 245,               /* leave_stat  */
  YYSYMBOL_nop_stat = 246,                 /* nop_stat  */
  YYSYMBOL_numeric_stat = 247,             /* numeric_stat  */
  YYSYMBOL_form_expr = 248,                /* form_expr  */
  YYSYMBOL_options_stat = 249,             /* options_stat  */
  YYSYMBOL_parse_stat = 250,               /* parse_stat  */
  YYSYMBOL_parse_flags = 251,              /* parse_flags  */
  YYSYMBOL_templs = 252,                   /* templs  */
  YYSYMBOL_parse_param = 253,              /* parse_param  */
  YYSYMBOL_proc_stat = 254,                /* proc_stat  */
  YYSYMBOL_pull_stat = 255,                /* pull_stat  */
  YYSYMBOL_push_stat = 256,                /* push_stat  */
  YYSYMBOL_queue_stat = 257,               /* queue_stat  */
  YYSYMBOL_say_stat = 258,                 /* say_stat  */
  YYSYMBOL_return_stat = 259,              /* return_stat  */
  YYSYMBOL_sel_end = 260,                  /* sel_end  */
  YYSYMBOL_select_stat = 261,              /* select_stat  */
  YYSYMBOL_when_stats = 262,               /* when_stats  */
  YYSYMBOL_when_stat = 263,                /* when_stat  */
  YYSYMBOL_ex_when_stat = 264,             /* ex_when_stat  */
  YYSYMBOL_otherwise_stat = 265,           /* otherwise_stat  */
  YYSYMBOL_signal_stat = 266,              /* signal_stat  */
  YYSYMBOL_signal_name = 267,              /* signal_name  */
  YYSYMBOL_namespec = 268,                 /* namespec  */
  YYSYMBOL_asymbol = 269,                  /* asymbol  */
  YYSYMBOL_on = 270,                       /* on  */
  YYSYMBOL_off = 271,                      /* off  */
  YYSYMBOL_c_action = 272,                 /* c_action  */
  YYSYMBOL_s_action = 273,                 /* s_action  */
  YYSYMBOL_trace_stat = 274,               /* trace_stat  */
  YYSYMBOL_whatever = 275,                 /* whatever  */
  YYSYMBOL_assignment = 276,               /* assignment  */
  YYSYMBOL_ass_part = 277,                 /* ass_part  */
  YYSYMBOL_plusassignment = 278,           /* plusassignment  */
  YYSYMBOL_plus_ass_part = 279,            /* plus_ass_part  */
  YYSYMBOL_minusassignment = 280,          /* minusassignment  */
  YYSYMBOL_minus_ass_part = 281,           /* minus_ass_part  */
  YYSYMBOL_multassignment = 282,           /* multassignment  */
  YYSYMBOL_mult_ass_part = 283,            /* mult_ass_part  */
  YYSYMBOL_divassignment = 284,            /* divassignment  */
  YYSYMBOL_div_ass_part = 285,             /* div_ass_part  */
  YYSYMBOL_intdivassignment = 286,         /* intdivassignment  */
  YYSYMBOL_intdiv_ass_part = 287,          /* intdiv_ass_part  */
  YYSYMBOL_modulusassignment = 288,        /* modulusassignment  */
  YYSYMBOL_modulus_ass_part = 289,         /* modulus_ass_part  */
  YYSYMBOL_andassignment = 290,            /* andassignment  */
  YYSYMBOL_and_ass_part = 291,             /* and_ass_part  */
  YYSYMBOL_xorassignment = 292,            /* xorassignment  */
  YYSYMBOL_xor_ass_part = 293,             /* xor_ass_part  */
  YYSYMBOL_orassignment = 294,             /* orassignment  */
  YYSYMBOL_or_ass_part = 295,              /* or_ass_part  */
  YYSYMBOL_concatassignment = 296,         /* concatassignment  */
  YYSYMBOL_concat_ass_part = 297,          /* concat_ass_part  */
  YYSYMBOL_expr = 298,                     /* expr  */
  YYSYMBOL_299_10 = 299,                   /* $@10  */
  YYSYMBOL_300_11 = 300,                   /* $@11  */
  YYSYMBOL_301_12 = 301,                   /* $@12  */
  YYSYMBOL_302_13 = 302,                   /* $@13  */
  YYSYMBOL_303_14 = 303,                   /* $@14  */
  YYSYMBOL_304_15 = 304,                   /* $@15  */
  YYSYMBOL_305_16 = 305,                   /* $@16  */
  YYSYMBOL_306_17 = 306,                   /* $@17  */
  YYSYMBOL_307_18 = 307,                   /* $@18  */
  YYSYMBOL_308_19 = 308,                   /* $@19  */
  YYSYMBOL_309_20 = 309,                   /* $@20  */
  YYSYMBOL_310_21 = 310,                   /* $@21  */
  YYSYMBOL_311_22 = 311,                   /* $@22  */
  YYSYMBOL_312_23 = 312,                   /* $@23  */
  YYSYMBOL_313_24 = 313,                   /* $@24  */
  YYSYMBOL_314_25 = 314,                   /* $@25  */
  YYSYMBOL_315_26 = 315,                   /* $@26  */
  YYSYMBOL_316_27 = 316,                   /* $@27  */
  YYSYMBOL_317_28 = 317,                   /* $@28  */
  YYSYMBOL_318_29 = 318,                   /* $@29  */
  YYSYMBOL_319_30 = 319,                   /* $@30  */
  YYSYMBOL_320_31 = 320,                   /* $@31  */
  YYSYMBOL_321_32 = 321,                   /* $@32  */
  YYSYMBOL_322_33 = 322,                   /* $@33  */
  YYSYMBOL_323_34 = 323,                   /* $@34  */
  YYSYMBOL_324_35 = 324,                   /* $@35  */
  YYSYMBOL_325_36 = 325,                   /* $@36  */
  YYSYMBOL_exprs_sub = 326,                /* exprs_sub  */
  YYSYMBOL_symbtree = 327,                 /* symbtree  */
  YYSYMBOL_function = 328,                 /* function  */
  YYSYMBOL_func_args = 329,                /* func_args  */
  YYSYMBOL_330_37 = 330,                   /* @37  */
  YYSYMBOL_msgtarget = 331,                /* msgtarget  */
  YYSYMBOL_msgname = 332,                  /* msgname  */
  YYSYMBOL_bracket_args = 333,             /* bracket_args  */
  YYSYMBOL_334_38 = 334,                   /* @38  */
  YYSYMBOL_intfunc = 335,                  /* intfunc  */
  YYSYMBOL_extfunc = 336,                  /* extfunc  */
  YYSYMBOL_template = 337,                 /* template  */
  YYSYMBOL_solid = 338,                    /* solid  */
  YYSYMBOL_offset = 339,                   /* offset  */
  YYSYMBOL_string = 340,                   /* string  */
  YYSYMBOL_pv = 341,                       /* pv  */
  YYSYMBOL_exprs = 342,                    /* exprs  */
  YYSYMBOL_343_39 = 343,                   /* $@39  */
  YYSYMBOL_nexpr = 344,                    /* nexpr  */
  YYSYMBOL_anyvars = 345,                  /* anyvars  */
  YYSYMBOL_xsimsymb = 346,                 /* xsimsymb  */
  YYSYMBOL_simsymb = 347                   /* simsymb  */
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
#define YYLAST   2529

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  138
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  210
/* YYNRULES -- Number of rules.  */
#define YYNRULES  517
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  704

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   379


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
       2,     2,     2,     2,     2,     2,     2,   130,   123,     2,
     135,   136,   128,   126,   134,   127,     2,   129,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   124,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   133,     2,   137,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   122,     2,     2,     2,     2,     2,
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
     115,   116,   117,   118,   119,   120,   121,   125,   131,   132
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   169,   169,   169,   179,   182,   186,   191,   202,   203,
     206,   210,   215,   215,   227,   230,   231,   234,   236,   238,
     242,   244,   247,   249,   252,   253,   256,   261,   262,   265,
     266,   269,   270,   273,   274,   277,   278,   281,   285,   288,
     289,   290,   291,   294,   295,   296,   297,   298,   299,   300,
     301,   302,   303,   304,   305,   306,   307,   308,   309,   310,
     311,   312,   313,   314,   315,   316,   317,   318,   319,   320,
     321,   322,   323,   324,   325,   326,   327,   328,   329,   330,
     337,   340,   343,   346,   349,   351,   354,   356,   360,   364,
     369,   373,   378,   382,   386,   390,   394,   398,   402,   406,
     410,   414,   418,   422,   426,   430,   434,   438,   442,   446,
     451,   455,   459,   463,   467,   471,   471,   475,   478,   479,
     480,   480,   482,   486,   492,   498,   498,   503,   503,   509,
     510,   511,   512,   513,   519,   524,   525,   531,   532,   535,
     543,   546,   568,   574,   576,   577,   580,   614,   618,   622,
     626,   629,   630,   631,   633,   636,   637,   638,   641,   641,
     647,   650,   651,   652,   655,   656,   657,   658,   661,   665,
     669,   673,   674,   675,   676,   677,   680,   681,   682,   683,
     684,   687,   688,   689,   690,   691,   694,   695,   698,   699,
     702,   703,   706,   707,   709,   710,   715,   716,   718,   719,
     720,   721,   726,   734,   735,   748,   749,   753,   754,   758,
     761,   761,   765,   765,   769,   770,   773,   774,   777,   785,
     793,   796,   797,   798,   801,   802,   803,   806,   807,   811,
     812,   816,   820,   828,   838,   840,   842,   843,   846,   849,
     852,   857,   859,   860,   863,   871,   875,   877,   878,   881,
     882,   885,   888,   889,   890,   893,   895,   897,   898,   899,
     902,   905,   906,   909,   912,   918,   924,   925,   928,   930,
     932,   934,   936,   938,   940,   944,   946,   949,   950,   951,
     952,   953,   954,   955,   957,   958,   961,   962,   963,   964,
     965,   969,   973,   977,   981,   985,   989,   990,   991,   994,
     999,  1000,  1002,  1003,  1004,  1008,  1009,  1010,  1013,  1019,
    1021,  1022,  1032,  1033,  1036,  1040,  1046,  1049,  1050,  1052,
    1053,  1054,  1055,  1056,  1062,  1066,  1067,  1073,  1074,  1075,
    1078,  1079,  1080,  1083,  1084,  1087,  1090,  1093,  1094,  1095,
    1096,  1099,  1100,  1101,  1102,  1105,  1107,  1109,  1110,  1114,
    1118,  1134,  1140,  1147,  1153,  1160,  1166,  1173,  1179,  1186,
    1192,  1199,  1205,  1212,  1218,  1225,  1231,  1238,  1244,  1251,
    1257,  1262,  1269,  1269,  1302,  1302,  1305,  1305,  1308,  1308,
    1311,  1312,  1312,  1315,  1316,  1316,  1319,  1320,  1320,  1323,
    1324,  1324,  1327,  1328,  1328,  1331,  1332,  1332,  1335,  1336,
    1336,  1339,  1340,  1340,  1343,  1344,  1344,  1347,  1348,  1350,
    1351,  1351,  1355,  1356,  1356,  1360,  1361,  1361,  1365,  1366,
    1366,  1370,  1371,  1371,  1375,  1376,  1376,  1380,  1381,  1381,
    1384,  1385,  1385,  1388,  1389,  1389,  1392,  1393,  1393,  1396,
    1397,  1397,  1400,  1401,  1401,  1404,  1405,  1405,  1408,  1409,
    1409,  1412,  1413,  1417,  1421,  1422,  1423,  1425,  1426,  1432,
    1436,  1440,  1442,  1443,  1445,  1447,  1448,  1451,  1452,  1453,
    1456,  1460,  1462,  1466,  1466,  1478,  1480,  1484,  1488,  1491,
    1494,  1499,  1502,  1502,  1509,  1512,  1515,  1516,  1517,  1520,
    1522,  1524,  1526,  1528,  1530,  1532,  1534,  1536,  1540,  1541,
    1550,  1551,  1555,  1561,  1562,  1563,  1573,  1573,  1653,  1697,
    1698,  1701,  1703,  1705,  1707,  1711,  1714,  1715
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
  "LOSTDIGITS", "GTGT", "LTLT", "NOTGTGT", "NOTLTLT", "GTGTE", "LTLTE",
  "INPUT", "OUTPUT", "ERROR", "NORMAL", "APPEND", "REPLACE", "STREAM",
  "STEM", "LIFO", "FIFO", "NOEOL", "LOWER", "CASELESS", "MSGSEND",
  "CASCADE", "DCOLON", "USEARG", "MSGSUPER", "PLUSASSIGNMENTVARIABLE",
  "MINUSASSIGNMENTVARIABLE", "MULTASSIGNMENTVARIABLE",
  "DIVASSIGNMENTVARIABLE", "MODULUSASSIGNMENTVARIABLE",
  "INTDIVASSIGNMENTVARIABLE", "ORASSIGNMENTVARIABLE",
  "XORASSIGNMENTVARIABLE", "ANDASSIGNMENTVARIABLE",
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
  "expr_stat", "end_stat", "end", "do_stat", "repetitor", "nvir",
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

#define YYPACT_NINF (-583)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-511)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -583,    63,  -583,  -583,  -583,    24,    -4,   150,   -37,  -583,
      26,    27,  -583,  -583,  -583,  1334,  -583,  -583,  -583,  -583,
      32,  -583,  -583,  -583,  -583,   277,  -583,    33,   -37,  -583,
    -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,
    -583,  -583,  -583,    35,  -583,  -583,  -583,  -583,  -583,  -583,
    -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,   903,  -583,
    -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,
    -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,
    -583,  -583,  -583,  -583,  -583,  -583,  -583,     3,  -583,  -583,
    -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,
    -583,  1044,  1125,  -583,  -583,  -583,  -583,    37,  -583,    40,
    -583,  -583,   297,  1999,  2131,  1834,    57,   186,  2131,  2131,
    -583,    69,   127,  2131,    29,   216,  2131,   215,    34,   158,
    2131,  2131,    36,   331,  -583,  -583,  1671,   127,  -583,  -583,
    -583,  -583,   209,    40,  -583,  -583,  -583,  -583,  -583,  -583,
    -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,
    -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,  2131,  -583,
    2131,  -583,  2131,  -583,  2131,  -583,  2131,  -583,  2131,  -583,
    2131,  -583,  2131,  -583,  2131,  -583,  2131,  -583,  2131,  2237,
    -583,  -583,  -583,  -583,    74,  -583,  -583,  -583,  -583,    -1,
    -583,  -583,  -583,     3,  -583,   127,   388,    82,   -25,  -583,
    -583,   388,  -583,   388,  1917,  -583,  -583,    32,  -583,  -583,
    -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,   122,   247,
    -583,   352,  -583,   178,    73,  2290,  2237,  -583,    32,  2237,
    -583,  -583,   159,   165,  -583,  -583,    96,  -583,   -13,    50,
     -13,  -583,  1703,   193,  2237,  -583,  -583,  2131,   291,  2131,
    -583,  -583,  -583,  -583,  -583,    47,   963,  -583,   105,  -583,
    -583,   137,   -18,   511,    69,  -583,    44,  -583,  -583,  -583,
    -583,   359,  -583,  2131,  -583,   286,  -583,   443,   476,  2131,
    -583,   311,  -583,   341,   371,    32,  -583,  -583,  -583,  -583,
    -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,
    -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,
    -583,  -583,  -583,  -583,  -583,   514,   514,  -583,  -583,  -583,
    -583,  -583,  -583,  -583,  -583,  -583,  -583,  1917,  -583,    74,
     277,    32,   182,  -583,  -583,     3,  -583,  -583,    18,   180,
    2131,  -583,  -583,  -583,  -583,  -583,    31,  -583,   398,  2131,
    -583,  -583,  -583,  2131,  2131,    40,  2131,   704,  -583,    52,
    -583,  -583,  -583,  2131,    77,  -583,  -583,  -583,  2131,  -583,
    2131,  -583,  -583,    69,  -583,  -583,  -583,   120,   143,   248,
     191,    69,  -583,  -583,  -583,  2237,  -583,  2131,  -583,  -583,
     411,  2237,  -583,  -583,   312,  -583,  -583,  -583,  -583,  -583,
      69,    50,  -583,   412,  -583,  -583,   345,  1206,   349,   296,
    -583,  2237,  -583,  -583,  -583,  -583,  -583,  -583,    66,  -583,
     415,  2237,  -583,  -583,  -583,  2131,  2131,  2131,  2131,  2131,
    2131,  2131,  2131,  2131,  2131,  2131,  2131,  2131,  2131,  2131,
    2131,  2131,  2131,  -583,  -583,  -583,   246,  -583,  -583,  -583,
    2131,  2131,  2131,  2131,  2131,  2131,  2131,  2131,  -583,  2131,
    -583,  -583,   127,  -583,  -583,  -583,  -583,  -583,   240,  -583,
      21,   442,  -583,  -583,  2290,  2290,    32,  2290,  -583,  -583,
     342,  2052,  -583,   444,  -583,  -583,  -583,   408,   408,   408,
     326,  2184,   342,  -583,   191,  -583,   191,  -583,   191,  -583,
     272,  -583,  2237,  -583,  -583,    50,  -583,  -583,  -583,  1751,
    -583,   612,  -583,   430,  -583,   463,  -583,   441,   334,   475,
     475,   475,   475,   475,   475,   475,   441,   388,  2343,   475,
     475,   475,   475,   475,   475,   343,  -583,  -583,  2343,  2396,
     475,  1012,  1012,   334,   334,   334,   346,  -583,  2131,  -583,
    -583,  -583,  -583,  -583,  -583,  -583,   434,   446,  -583,  1462,
     520,   745,  -583,   480,  -583,  -583,  -583,   408,   408,   129,
    -583,   408,   408,   106,  -583,   408,   408,   138,   149,   208,
     225,   408,  -583,   353,   366,   378,  -583,   508,  -583,  -583,
     211,  -583,  -583,  -583,  -583,  -583,  -583,  1590,  -583,  -583,
    2131,  2131,  2131,   520,  -583,   522,  -583,   408,   451,   408,
     455,   408,   451,   408,   457,   408,   455,   408,   457,  -583,
     591,  -583,   478,  -583,   478,   342,  -583,  -583,  -583,  -583,
    -583,   472,  -583,  2290,  2290,  2290,   520,  -583,  -583,  -583,
    -583,  -583,  -583,  -583,  -583,   263,    60,   314,   416,    75,
    -583,  -583,  -583,    80,   200,  -583,  -583,  -583,  -583,  -583,
    -583,  -583,  -583,  -583,  -583,   878,  -583,  -583,   408,  -583,
    -583,   408,  -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,
    -583,  -583,   524,  -583,  -583,   432,     3,  -583,  -583,  -583,
    -583,  -583,  -583,  -583
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       2,     0,    34,     1,     3,    37,    32,     0,     7,     9,
      37,    37,    21,    25,    24,    37,    35,    36,    34,   100,
      31,   245,    33,   244,    14,     0,     8,    37,     6,    20,
      96,    97,    88,    89,    90,    91,    92,    99,    93,   101,
     102,   104,   105,     0,   106,   107,   108,    94,    95,   109,
     111,    98,   112,   110,   113,   142,   114,   351,   409,   407,
     386,   415,   421,   418,   424,   427,   430,   433,   404,   401,
     398,   458,   470,   485,   484,   459,   461,   460,   103,   436,
     439,   442,   445,   448,   451,   454,   455,    87,   353,   355,
     357,   359,   363,   361,   369,   367,   365,   371,   392,   395,
     412,     0,     0,   380,   383,   389,   372,     0,    38,     0,
      78,    79,     0,   154,   510,     0,     0,     0,   510,   510,
     115,     0,     0,     0,     0,     0,   510,     0,     0,     0,
     510,   510,     0,     0,   312,   313,   510,     0,    43,    45,
      46,    44,     0,     0,    47,    48,    66,    49,    40,    41,
      42,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    62,    61,    63,    64,    65,    77,   510,    67,
     510,    68,   510,    69,   510,    70,   510,    71,   510,    72,
     510,    75,   510,    73,   510,    74,   510,    76,   510,   141,
     457,   462,   473,   473,    30,    34,    18,    17,    19,     0,
      16,    81,   515,     0,    80,   512,   408,     0,    85,    86,
     465,   463,   466,   464,   510,   238,   239,    39,   138,   335,
     336,   333,   334,   501,   500,   502,   125,   137,     0,     0,
     127,     0,   220,   226,     0,   215,   509,   231,   236,    34,
     517,   516,     0,     0,   294,   295,     0,   488,   505,   124,
     505,   276,   487,     0,   240,   250,   260,   252,     0,   258,
     263,   267,   277,   279,   282,     0,     0,   281,   268,   280,
     278,   271,   274,     0,     0,   287,     0,   291,   292,   293,
     302,     0,   329,     0,   328,     0,   327,     0,     0,     0,
     349,     0,   346,     0,     0,    26,   350,   352,   354,   356,
     358,   360,   362,   364,   366,   368,   370,   405,   384,   413,
     419,   416,   422,   425,   428,   431,   402,   399,   396,   434,
     437,   440,   443,   446,   449,     0,     0,   390,   393,   410,
     374,   376,   378,   381,   387,   482,   472,   510,   471,    29,
       0,    13,     0,   511,    83,    87,   469,   373,     0,   508,
     510,   129,   338,   339,   340,   337,     0,   130,     0,   510,
     152,   214,   151,     0,     0,     0,     0,   402,   153,     0,
     242,   247,   120,     0,     0,   155,   156,   157,     0,   116,
     510,   118,   503,     0,   504,   498,   499,     0,     0,     0,
       0,     0,   491,   497,   227,   251,   257,     0,   261,   262,
       0,   259,   283,   285,     0,   269,   272,   270,   273,   266,
       0,   265,   288,     0,   307,   303,     0,     0,     0,   315,
     306,   316,   317,   319,   343,   342,   344,   341,     0,   320,
       0,   345,   347,   229,   145,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   481,   478,   452,   475,   473,   473,   453,
       0,     0,     0,     0,     0,     0,     0,     0,   456,   510,
     474,    15,   514,    84,   468,   467,   506,   126,   139,   131,
       0,     0,   135,   128,   215,   215,    37,   215,   237,    34,
     158,   158,   167,     0,   164,   165,   166,   215,   215,   215,
     163,     0,   158,   275,     0,   492,     0,   490,     0,   489,
       0,   486,   256,   253,   284,   264,   289,   300,   311,    34,
     301,     0,   305,     0,   321,     0,   325,   406,   385,   414,
     420,   417,   423,   426,   429,   432,   403,   400,   397,   435,
     438,   441,   444,   447,   450,   476,   479,   480,   391,   394,
     411,   375,   377,   379,   382,   388,     0,   513,   510,   140,
     332,   330,   331,   132,   224,   225,    37,     0,    37,    37,
     150,     0,   121,     0,   117,   162,   168,   215,   215,   175,
     169,   215,   215,   180,   170,   215,   215,   185,     0,     0,
       0,   215,   122,     0,     0,     0,   493,     0,   304,    37,
       0,   299,   322,   473,   483,   507,    22,    37,   146,    23,
       0,     0,     0,   149,   235,   232,   159,   215,   171,   215,
     173,   215,   176,   215,   178,   215,   181,   215,   183,   187,
       0,   189,     0,   191,     0,   158,   496,   495,   494,    34,
     314,     0,   477,   215,   215,   215,   148,    34,   172,   174,
     177,   179,   182,   184,   195,     0,     0,     0,     0,     0,
     186,   192,   201,     0,     0,   188,   196,   190,   123,    37,
     297,   221,   223,   222,   147,     0,   203,   202,   215,   205,
     204,   215,   207,   206,   209,   208,   194,   193,   198,   197,
     200,   199,   308,   234,   233,     0,     0,   309,   213,   216,
     217,   219,   218,   211
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -583,  -583,  -583,  -583,   542,    41,  -583,  -583,  -583,   222,
     546,  -583,   -11,  -583,   -35,   -75,  -233,    -9,  -471,   -14,
    -583,  -583,  -583,  -583,  -583,   220,  -583,  -583,  -583,  -583,
    -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,
    -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,
    -274,  -264,  -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,
    -583,  -583,  -583,   207,  -583,  -583,     6,  -583,  -583,  -583,
    -477,  -583,    11,  -583,  -583,  -583,  -583,  -583,  -583,  -583,
    -425,  -458,  -412,  -583,   -45,  -183,  -583,  -583,  -382,  -583,
    -120,  -583,  -102,  -583,  -583,  -582,  -583,  -583,  -583,  -583,
    -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,
    -583,  -583,  -583,  -583,  -254,   323,  -583,  -583,  -583,  -583,
    -583,  -583,  -583,  -583,  -583,   187,  -583,  -583,  -583,  -583,
     169,   474,   479,   481,    68,   321,  -583,  -583,  -583,  -583,
    -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,
    -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,
     -12,  -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,
    -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,
    -583,  -583,  -583,  -583,  -583,  -583,  -583,  -583,   282,  -111,
    -583,  -188,  -583,   294,  -583,  -583,  -583,    62,    85,  -118,
    -583,   156,  -239,   134,  -338,  -583,   -76,  -114,   -85,  -113
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,     2,     4,     8,     9,    10,    25,   199,   200,
      11,   566,    12,    13,   567,    20,     5,     6,    14,    15,
      16,   108,   109,   110,   111,   207,   208,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,    21,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   246,   379,   490,   139,   140,
     350,   359,   226,   477,   141,   142,   143,   144,   233,   380,
     572,   573,   381,   493,   494,   495,   496,   576,   580,   584,
     497,   498,   499,   660,   665,   666,   680,   681,   677,   678,
     362,   698,   699,   700,   234,   613,   365,   145,   146,   147,
     148,   149,   150,   151,   152,    22,    23,   153,   154,   155,
     400,   156,   157,   273,   249,   274,   158,   159,   160,   161,
     162,   163,   601,   164,   419,   420,    17,   523,   165,   285,
     481,   227,   228,   229,   427,   428,   166,   291,   167,   168,
     169,   170,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   186,   187,   188,
     236,   214,   463,   464,   465,   466,   436,   467,   460,   461,
     446,   445,   444,   435,   462,   437,   439,   438,   440,   441,
     442,   443,   447,   448,   449,   450,   451,   452,   347,   190,
     191,   336,   337,   455,   456,   468,   469,   192,   193,   251,
     391,   392,   230,   252,   348,   558,   349,   204,   205,   242
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      29,   107,   209,   189,   243,   338,   369,   417,   253,   194,
     250,   277,   478,   393,   574,   568,    29,   418,   250,   474,
     411,   478,   560,   293,    -5,   592,   -11,    -4,   407,   294,
     255,   646,   479,   -10,   217,   275,   201,   280,   237,   577,
     238,   586,   244,   245,    18,   412,   206,    18,   240,    26,
     260,   215,   216,   488,   278,   279,   248,   281,   240,    72,
     292,   679,   276,     3,   674,   340,   489,   524,   295,    26,
     247,    19,     7,   581,   585,   202,   686,  -249,   492,  -134,
     195,   688,  -286,   344,    18,   578,   582,   408,    18,   211,
     213,   343,   296,   561,   297,   606,   298,   372,   299,   562,
     300,   235,   301,   239,   302,  -243,   303,   202,   304,   345,
     305,   254,   306,   480,  -324,   368,   202,  -505,   342,   241,
     619,   373,   374,   351,   341,  -215,  -210,   625,   568,   241,
     -82,   556,  -210,     7,  -505,   -11,     7,   250,   248,   250,
    -505,    72,   -10,   361,  -119,   417,  -505,  -505,   480,    19,
     629,    24,   402,  -505,   475,   521,   515,   623,   668,   247,
     370,   627,   413,   250,   352,   617,   371,   375,   376,   621,
     203,  -215,  -215,  -215,   377,   655,   656,   657,   658,   203,
     655,   656,   657,   658,   383,   385,   339,   240,   363,   364,
     404,   386,   353,  -505,   394,  -505,  -505,   366,   692,   202,
     588,   690,   590,  -505,  -505,   354,  -505,  -241,   385,   631,
     240,   405,   240,  -246,   386,   361,   261,   256,   355,   262,
     605,   -12,   -12,  -505,   589,   590,   633,   248,   -12,  -505,
      72,   378,   588,   589,  -248,  -505,  -505,   263,   264,   265,
     266,  -228,  -505,   406,   267,   395,  -215,   401,   357,  -215,
    -215,  -215,  -215,  -215,   500,   504,   571,  -144,   241,  -298,
     209,   268,   203,    72,   676,   503,   257,   258,   259,   546,
     547,   421,   250,   511,   361,   683,   685,   431,   506,   510,
     250,   241,  -505,   241,  -505,  -505,   597,   422,   599,   352,
     486,   361,   396,  -505,   269,   270,   356,   358,   218,   250,
     655,   656,   657,   658,   502,  -215,  -215,  -215,  -215,  -215,
    -215,  -215,   432,   385,   476,   682,   397,   353,   472,   386,
     271,   272,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -212,
     354,    50,   282,    52,  -318,  -212,   219,   220,   514,  -255,
    -212,  -212,   433,   355,   398,   399,   517,  -212,   196,   197,
     520,   484,   485,   360,   487,   198,   283,   545,   557,  -348,
     414,   491,  -215,  -215,   564,   565,   501,   570,   221,   222,
     219,   220,   434,   415,   223,   224,   559,   579,   583,   587,
    -212,   225,   382,   508,   384,   512,  -212,   457,   457,  -230,
    -160,  -212,  -212,   593,    50,   594,    52,   595,  -212,   482,
    -215,   317,   221,   222,   416,   519,   669,   195,   596,   284,
     458,   458,   513,   516,   675,   642,   526,   684,   361,  -143,
     588,   589,   590,   527,   528,   529,   530,   531,   532,   533,
     534,   535,   536,   537,   538,   539,   540,   541,   542,   543,
     544,   325,   326,   563,   423,   575,  -136,   661,   548,   549,
     550,   551,   552,   553,   554,   555,   701,   618,   620,  -254,
    -290,   622,   624,  -326,   602,   626,   628,   335,   630,   632,
     634,   635,   569,   670,   361,   600,   687,   429,   603,   -27,
     689,   691,  -212,   604,   424,   352,   425,   641,  -212,   636,
    -133,    55,  -161,  -212,  -212,   325,   326,   648,   308,   649,
    -212,   650,   637,   651,   202,   652,   374,   653,   317,   223,
     224,  -323,   409,   353,   638,   262,   225,   424,   352,   425,
    -296,   335,   639,   671,   672,   673,   354,   610,   611,   612,
     426,   307,   308,   263,   264,   265,   266,   647,   697,   355,
     267,   316,   317,   505,   507,   509,   353,   590,   325,   326,
     589,   588,   607,    28,   609,   107,    27,   189,   695,   354,
     615,   696,   471,   426,   640,   473,   483,   330,   331,   332,
     333,   334,   355,   608,   335,   662,   663,   664,   655,   656,
     657,   658,   325,   326,   616,   569,   453,    73,    74,   667,
     269,   270,   454,   107,   703,   189,   410,   525,   643,   644,
     645,   330,   331,   332,   333,   334,   522,   286,   335,   430,
     702,   702,   287,   598,   288,   -34,   -34,   -34,   -34,   470,
     459,     0,     0,     0,   -34,   -34,   -34,   -34,   -34,   -34,
     -34,   -34,   -34,   -34,     0,     0,     0,     0,     0,   -34,
     -34,   -34,   -34,   -34,   -34,   -34,   -34,   -34,   -34,   -34,
     -34,     0,     0,     0,     0,   607,   -34,   -34,   -34,   -34,
     -34,     0,     0,     0,   694,     0,     0,   -34,   -34,   -34,
     -34,   -34,   -34,   -34,   -34,   -34,   -34,     0,   -34,   -34,
     -34,     0,     0,   -34,   -34,   -34,   -34,   -34,   654,   -34,
     -34,   655,   656,   657,   658,   659,   -34,   -34,     0,     0,
     -34,   -34,   -34,   -34,   -34,   -34,     0,     0,     0,     0,
       0,  -214,  -214,  -214,  -214,  -214,     0,     0,     0,   -34,
     -34,     0,   -34,     0,   -34,   -34,   -34,   -34,   -34,   -34,
     -34,   -34,   -34,   -34,   -34,   -34,   -34,     0,   -34,   -34,
     -34,   -34,   -34,     0,     0,     0,   614,   -34,   -37,   -37,
     -37,   -37,  -214,     0,     0,     0,     0,   -37,   -37,   -37,
     -37,   -37,   -37,   -37,   -37,   -37,   -37,     0,     0,     0,
       0,     0,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,
     -37,   -37,   -37,   -37,     0,     0,     0,     0,     0,   -37,
     -37,   -37,   -37,     0,     0,     0,     0,     0,     0,     0,
     -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,
       0,   -37,   -37,   -37,     0,     0,   -37,   -37,   -37,   -37,
       0,     0,   -37,   -37,     0,     0,     0,     0,     0,   -37,
     -37,     0,     0,   -37,   -37,   -37,   -37,   -37,   -37,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   -37,   -37,     0,   -37,     0,   -37,   -37,   -37,
     -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,
       0,   -37,   -37,   -37,   -37,   -37,     0,     0,     0,   693,
     -37,   -37,   -37,   -37,   -37,     0,     0,     0,     0,     0,
     -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,
       0,     0,     0,     0,     0,   -37,   -37,   -37,   -37,   -37,
     -37,   -37,   -37,   -37,   -37,   -37,   -37,     0,     0,     0,
       0,     0,   -37,   -37,   -37,   -37,     0,     0,     0,     0,
       0,     0,     0,   -37,   -37,   -37,   -37,   -37,   -37,   -37,
     -37,   -37,   -37,     0,   -37,   -37,   -37,     0,     0,   -37,
     -37,   -37,   -37,     0,     0,   -37,   -37,     0,    58,     0,
       0,     0,   -37,   -37,   403,     0,   -37,   -37,   -37,   -37,
     -37,   -37,     0,     0,    71,    72,    73,    74,     0,     0,
      75,    76,     0,     0,     0,   -37,   -37,    77,   -37,  -510,
     -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,
     -37,   -37,   -37,     0,   -37,   -37,   -37,   -37,   -37,     0,
      85,    86,     0,   -37,     0,     0,     0,     0,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,     0,    68,
      69,    70,     0,     0,    71,    72,    73,    74,   106,     0,
      75,    76,     0,     0,     0,   210,     0,    77,     0,     0,
       0,    79,    80,    81,    82,    83,    84,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   308,
      85,    86,     0,     0,     0,     0,     0,     0,     0,   317,
       0,     0,     0,     0,     0,    98,    99,   100,     0,   101,
     102,   103,   104,   105,     0,     0,     0,     0,   106,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,     0,
      68,    69,    70,     0,     0,    71,    72,    73,    74,   325,
     326,    75,    76,     0,     0,     0,   212,     0,    77,     0,
       0,     0,    79,    80,    81,    82,    83,    84,     0,     0,
     332,   333,   334,     0,     0,   335,     0,     0,     0,     0,
       0,    85,    86,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    98,    99,   100,     0,
     101,   102,   103,   104,   105,     0,     0,     0,     0,   106,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
       0,    68,    69,    70,     0,     0,    71,    72,    73,    74,
       0,     0,    75,    76,     0,     0,     0,   518,     0,    77,
       0,     0,     0,    79,    80,    81,    82,    83,    84,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    85,    86,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    98,    99,   100,
       0,   101,   102,   103,   104,   105,     0,     0,     0,     0,
     106,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,     0,    68,    69,    70,     0,     0,    71,    72,    73,
      74,     0,     0,    75,    76,     0,     0,     0,     0,     0,
      77,     0,     0,     0,    79,    80,    81,    82,    83,    84,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    85,    86,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    98,    99,
     100,     0,   101,   102,   103,   104,   105,    30,    31,    32,
      33,   106,     0,     0,     0,     0,    34,    35,     0,     0,
      36,    37,    38,    39,    40,    41,     0,     0,     0,     0,
       0,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,     0,     0,     0,     0,     0,    54,    55,
      56,    57,     0,     0,     0,     0,     0,     0,     0,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,     0,
      68,    69,    70,     0,     0,    71,    72,    73,    74,     0,
       0,    75,    76,     0,     0,     0,     0,     0,    77,    78,
       0,     0,    79,    80,    81,    82,    83,    84,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    85,    86,     0,    87,     0,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,     0,
     101,   102,   103,   104,   105,    30,    31,    32,    33,   106,
       0,     0,     0,     0,    34,    35,     0,     0,    36,    37,
      38,    39,    40,    41,     0,     0,     0,     0,     0,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,     0,     0,     0,     0,     0,    54,   -28,    56,    57,
       0,     0,     0,     0,     0,     0,     0,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,     0,    68,    69,
      70,     0,     0,    71,    72,    73,    74,     0,     0,    75,
      76,     0,     0,     0,     0,     0,    77,    78,     0,     0,
      79,    80,    81,    82,    83,    84,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    85,
      86,     0,    87,     0,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,     0,   101,   102,
     103,   104,   105,    30,    31,    32,    33,   106,     0,     0,
       0,     0,    34,    35,     0,     0,    36,    37,    38,    39,
      40,    41,     0,     0,     0,     0,     0,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,     0,
       0,     0,     0,     0,    54,     0,    56,    57,     0,     0,
       0,     0,     0,     0,     0,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,     0,    68,    69,    70,     0,
       0,    71,    72,    73,    74,     0,     0,    75,    76,     0,
       0,     0,     0,     0,    77,    78,     0,     0,    79,    80,
      81,    82,    83,    84,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   289,    85,    86,     0,
      87,     0,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,     0,   101,   102,   103,   104,
     105,     0,     0,     0,     0,   106,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,     0,    68,    69,    70,
       0,     0,    71,    72,    73,    74,     0,     0,    75,    76,
       0,     0,   290,     0,     0,    77,     0,     0,     0,    79,
      80,    81,    82,    83,    84,     0,     0,     0,   385,     0,
       0,     0,     0,     0,   386,     0,     0,     0,    85,    86,
     223,   224,     0,     0,     0,     0,  -310,   225,  -310,     0,
       0,     0,     0,    98,    99,   100,  -310,   101,   102,   103,
     104,   105,     0,     0,     0,     0,   106,   307,   308,   309,
     310,   311,   312,   313,   314,   315,     0,   316,   317,   318,
       0,     0,     0,     0,     0,     0,     0,   387,     0,   388,
     389,     0,     0,     0,     0,     0,     0,     0,   390,   319,
     320,   321,   322,   323,   324,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   325,   326,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   327,   328,   329,     0,   330,   331,   332,
     333,   334,    18,     0,   335,     0,     0,     0,     0,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,     0,
      68,    69,    70,     0,     0,    71,    72,    73,    74,     0,
       0,    75,    76,     0,     0,     0,     0,     0,    77,     0,
       0,     0,    79,    80,    81,    82,    83,    84,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    85,    86,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    98,    99,   100,     0,
     101,   102,   103,   104,   105,   346,     0,     0,     0,   106,
       0,     0,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,     0,    68,    69,    70,     0,     0,    71,    72,
      73,    74,     0,     0,    75,    76,     0,     0,     0,     0,
       0,    77,     0,     0,     0,    79,    80,    81,    82,    83,
      84,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    85,    86,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    98,
      99,   100,     0,   101,   102,   103,   104,   105,   231,     0,
       0,     0,   106,     0,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,     0,    68,    69,    70,     0,     0,
      71,    72,    73,    74,     0,   232,    75,    76,     0,     0,
       0,     0,     0,    77,     0,     0,     0,    79,    80,    81,
      82,    83,    84,     0,     0,     0,     0,     0,     0,     0,
    -160,     0,     0,     0,     0,     0,    85,    86,   307,   308,
     309,   310,   311,   312,   313,   314,   315,     0,   316,   317,
     318,    98,    99,   100,     0,   101,   102,   103,   104,   105,
       0,     0,     0,     0,   106,     0,     0,     0,     0,     0,
     319,   320,   321,   322,   323,   324,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   325,
     326,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   327,   328,   329,     0,   330,   331,
     332,   333,   334,     0,     0,   335,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,     0,    68,    69,    70,
       0,     0,    71,    72,    73,    74,     0,     0,    75,    76,
       0,     0,     0,     0,     0,    77,     0,     0,     0,    79,
      80,    81,    82,    83,    84,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    85,    86,
     307,   308,   309,   310,   311,   312,   313,   314,   315,     0,
     316,   317,   318,    98,    99,   100,     0,   101,   102,   103,
     104,   105,     0,     0,     0,     0,   106,     0,     0,     0,
       0,     0,   319,   320,   321,   322,   323,   324,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   325,   326,   307,   308,   309,   310,   311,   312,   313,
     314,   315,     0,   316,   317,   318,   327,   328,   329,     0,
     330,   331,   332,   333,   334,     0,     0,   335,     0,     0,
     591,     0,     0,     0,     0,   319,   320,   321,   322,   323,
     324,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   325,   326,   307,   308,   309,   310,
     311,   312,   313,   314,   315,     0,   367,   317,   318,   327,
     328,   329,     0,   330,   331,   332,   333,   334,     0,     0,
     335,     0,     0,     0,     0,     0,     0,     0,   319,   320,
     321,   322,   323,   324,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   325,   326,   307,
     308,   309,   310,   311,   312,   313,   314,   315,     0,   316,
     317,     0,   327,   328,   329,     0,   330,   331,   332,   333,
     334,     0,     0,   335,     0,     0,     0,     0,     0,     0,
       0,   319,   320,   321,   322,   323,   324,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     325,   326,   307,   308,   309,   310,   311,   312,   313,   314,
     315,     0,   316,   317,     0,     0,   328,   329,     0,   330,
     331,   332,   333,   334,     0,     0,   335,     0,     0,     0,
       0,     0,     0,     0,   319,   320,   321,   322,   323,   324,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   325,   326,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     329,     0,   330,   331,   332,   333,   334,     0,     0,   335
};

static const yytype_int16 yycheck[] =
{
      11,    15,    87,    15,   117,   193,   239,   281,   122,    18,
     121,   129,   350,   252,   491,   486,    27,   281,   129,     1,
     274,   359,     1,   137,     0,   502,     0,     0,    46,   142,
       1,   613,     1,     0,   109,     1,     1,     1,   114,   497,
     115,   499,   118,   119,    48,     1,    58,    48,     1,     8,
     126,    14,    15,     1,   130,   131,    69,   132,     1,    72,
     136,     1,    28,     0,   646,    66,    14,     1,   143,    28,
       1,    75,   109,   498,   499,    72,     1,    48,     1,    48,
      48,     1,    48,     1,    48,   497,   498,   105,    48,   101,
     102,   205,   168,    72,   170,   566,   172,     1,   174,    78,
     176,   113,   178,   115,   180,    48,   182,    72,   184,   134,
     186,   123,   188,    82,    48,   235,    72,    48,   203,    72,
     578,    25,    26,     1,   199,    48,    66,   585,   599,    72,
      48,   469,    72,   109,    65,   109,   109,   248,    69,   250,
      71,    72,   109,    66,    48,   419,    77,    78,    82,    75,
       1,     1,   265,    84,   136,   419,   410,   582,   635,     1,
       1,   586,   276,   274,    42,   577,     1,    71,    72,   581,
     135,    94,    95,    96,    78,   100,   101,   102,   103,   135,
     100,   101,   102,   103,   134,    65,   195,     1,    10,    11,
     266,    71,    70,   124,     1,   126,   127,   124,   669,    72,
      94,     1,    96,   134,   135,    83,    48,    48,    65,     1,
       1,   106,     1,    48,    71,    66,     1,     1,    96,     4,
     558,    71,    72,    65,    95,    96,     1,    69,    78,    71,
      72,   135,    94,    95,    48,    77,    78,    22,    23,    24,
      25,    48,    84,   106,    29,   257,    97,   259,     1,   100,
     101,   102,   103,   104,   374,   135,   489,    48,    72,    48,
     345,    46,   135,    72,     1,   383,    50,    51,    52,   457,
     458,   283,   383,   391,    66,   657,   658,   289,   135,   390,
     391,    72,   124,    72,   126,   127,   519,     1,   521,    42,
     365,    66,     1,   135,    79,    80,   228,   229,     1,   410,
     100,   101,   102,   103,   380,    97,    98,    99,   100,   101,
     102,   103,     1,    65,   134,     1,    25,    70,   136,    71,
     105,   106,    97,    98,    99,   100,   101,   102,   103,    66,
      83,    35,     1,    37,    48,    72,    39,    40,    26,    48,
      77,    78,     1,    96,    53,    54,     1,    84,    71,    72,
       1,   363,   364,     1,   366,    78,    25,   111,   472,    48,
       1,   373,    10,    11,   484,   485,   378,   487,    71,    72,
      39,    40,     1,    14,    77,    78,   136,   497,   498,   499,
      66,    84,   248,   135,   250,   397,    72,   325,   326,    48,
      48,    77,    78,   504,    35,   506,    37,   508,    84,     1,
      48,    67,    71,    72,    45,   417,   639,    48,   136,    78,
     325,   326,     1,     1,   647,   603,     1,     1,    66,    48,
      94,    95,    96,   435,   436,   437,   438,   439,   440,   441,
     442,   443,   444,   445,   446,   447,   448,   449,   450,   451,
     452,   107,   108,     1,     1,     1,    48,   630,   460,   461,
     462,   463,   464,   465,   466,   467,   695,   577,   578,    48,
      48,   581,   582,    48,     1,   585,   586,   133,   588,   589,
     590,   591,   486,     1,    66,    45,   659,     1,   135,    45,
     663,   664,    66,   137,    41,    42,    43,   600,    72,   136,
      48,    45,    48,    77,    78,   107,   108,   617,    57,   619,
      84,   621,   136,   623,    72,   625,    26,   627,    67,    77,
      78,    48,     1,    70,   136,     4,    84,    41,    42,    43,
      48,   133,    14,   643,   644,   645,    83,     7,     8,     9,
      87,    56,    57,    22,    23,    24,    25,    15,    14,    96,
      29,    66,    67,   387,   388,   389,    70,    96,   107,   108,
      95,    94,   566,    11,   568,   569,    10,   569,   678,    83,
     571,   681,   340,    87,   599,   345,   359,   126,   127,   128,
     129,   130,    96,   567,   133,    97,    98,    99,   100,   101,
     102,   103,   107,   108,   573,   599,    72,    73,    74,   634,
      79,    80,    78,   607,   696,   607,   273,   428,   610,   611,
     612,   126,   127,   128,   129,   130,   419,   133,   133,   288,
     695,   696,   133,     1,   133,     3,     4,     5,     6,   337,
     326,    -1,    -1,    -1,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    -1,    -1,    -1,    -1,    -1,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    -1,    -1,    -1,    -1,   669,    44,    45,    46,    47,
      48,    -1,    -1,    -1,   675,    -1,    -1,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    -1,    66,    67,
      68,    -1,    -1,    71,    72,    73,    74,    75,    97,    77,
      78,   100,   101,   102,   103,   104,    84,    85,    -1,    -1,
      88,    89,    90,    91,    92,    93,    -1,    -1,    -1,    -1,
      -1,     7,     8,     9,    10,    11,    -1,    -1,    -1,   107,
     108,    -1,   110,    -1,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,    -1,   126,   127,
     128,   129,   130,    -1,    -1,    -1,     1,   135,     3,     4,
       5,     6,    48,    -1,    -1,    -1,    -1,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    -1,    -1,    -1,
      -1,    -1,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    -1,    -1,    -1,    -1,    -1,    44,
      45,    46,    47,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      -1,    66,    67,    68,    -1,    -1,    71,    72,    73,    74,
      -1,    -1,    77,    78,    -1,    -1,    -1,    -1,    -1,    84,
      85,    -1,    -1,    88,    89,    90,    91,    92,    93,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   107,   108,    -1,   110,    -1,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
      -1,   126,   127,   128,   129,   130,    -1,    -1,    -1,     1,
     135,     3,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      -1,    -1,    -1,    -1,    -1,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    -1,    -1,    -1,
      -1,    -1,    44,    45,    46,    47,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    -1,    66,    67,    68,    -1,    -1,    71,
      72,    73,    74,    -1,    -1,    77,    78,    -1,    55,    -1,
      -1,    -1,    84,    85,     1,    -1,    88,    89,    90,    91,
      92,    93,    -1,    -1,    71,    72,    73,    74,    -1,    -1,
      77,    78,    -1,    -1,    -1,   107,   108,    84,   110,    26,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,    -1,   126,   127,   128,   129,   130,    -1,
     107,   108,    -1,   135,    -1,    -1,    -1,    -1,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    -1,    66,
      67,    68,    -1,    -1,    71,    72,    73,    74,   135,    -1,
      77,    78,    -1,    -1,    -1,     1,    -1,    84,    -1,    -1,
      -1,    88,    89,    90,    91,    92,    93,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    57,
     107,   108,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,
      -1,    -1,    -1,    -1,    -1,   122,   123,   124,    -1,   126,
     127,   128,   129,   130,    -1,    -1,    -1,    -1,   135,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    -1,
      66,    67,    68,    -1,    -1,    71,    72,    73,    74,   107,
     108,    77,    78,    -1,    -1,    -1,     1,    -1,    84,    -1,
      -1,    -1,    88,    89,    90,    91,    92,    93,    -1,    -1,
     128,   129,   130,    -1,    -1,   133,    -1,    -1,    -1,    -1,
      -1,   107,   108,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   122,   123,   124,    -1,
     126,   127,   128,   129,   130,    -1,    -1,    -1,    -1,   135,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      -1,    66,    67,    68,    -1,    -1,    71,    72,    73,    74,
      -1,    -1,    77,    78,    -1,    -1,    -1,     1,    -1,    84,
      -1,    -1,    -1,    88,    89,    90,    91,    92,    93,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   107,   108,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   122,   123,   124,
      -1,   126,   127,   128,   129,   130,    -1,    -1,    -1,    -1,
     135,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    -1,    66,    67,    68,    -1,    -1,    71,    72,    73,
      74,    -1,    -1,    77,    78,    -1,    -1,    -1,    -1,    -1,
      84,    -1,    -1,    -1,    88,    89,    90,    91,    92,    93,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   107,   108,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   122,   123,
     124,    -1,   126,   127,   128,   129,   130,     3,     4,     5,
       6,   135,    -1,    -1,    -1,    -1,    12,    13,    -1,    -1,
      16,    17,    18,    19,    20,    21,    -1,    -1,    -1,    -1,
      -1,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    -1,    -1,    -1,    -1,    -1,    44,    45,
      46,    47,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    -1,
      66,    67,    68,    -1,    -1,    71,    72,    73,    74,    -1,
      -1,    77,    78,    -1,    -1,    -1,    -1,    -1,    84,    85,
      -1,    -1,    88,    89,    90,    91,    92,    93,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   107,   108,    -1,   110,    -1,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,    -1,
     126,   127,   128,   129,   130,     3,     4,     5,     6,   135,
      -1,    -1,    -1,    -1,    12,    13,    -1,    -1,    16,    17,
      18,    19,    20,    21,    -1,    -1,    -1,    -1,    -1,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    -1,    -1,    -1,    -1,    -1,    44,    45,    46,    47,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    -1,    66,    67,
      68,    -1,    -1,    71,    72,    73,    74,    -1,    -1,    77,
      78,    -1,    -1,    -1,    -1,    -1,    84,    85,    -1,    -1,
      88,    89,    90,    91,    92,    93,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,
     108,    -1,   110,    -1,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,    -1,   126,   127,
     128,   129,   130,     3,     4,     5,     6,   135,    -1,    -1,
      -1,    -1,    12,    13,    -1,    -1,    16,    17,    18,    19,
      20,    21,    -1,    -1,    -1,    -1,    -1,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    -1,
      -1,    -1,    -1,    -1,    44,    -1,    46,    47,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    -1,    66,    67,    68,    -1,
      -1,    71,    72,    73,    74,    -1,    -1,    77,    78,    -1,
      -1,    -1,    -1,    -1,    84,    85,    -1,    -1,    88,    89,
      90,    91,    92,    93,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    25,   107,   108,    -1,
     110,    -1,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,    -1,   126,   127,   128,   129,
     130,    -1,    -1,    -1,    -1,   135,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    -1,    66,    67,    68,
      -1,    -1,    71,    72,    73,    74,    -1,    -1,    77,    78,
      -1,    -1,    81,    -1,    -1,    84,    -1,    -1,    -1,    88,
      89,    90,    91,    92,    93,    -1,    -1,    -1,    65,    -1,
      -1,    -1,    -1,    -1,    71,    -1,    -1,    -1,   107,   108,
      77,    78,    -1,    -1,    -1,    -1,    35,    84,    37,    -1,
      -1,    -1,    -1,   122,   123,   124,    45,   126,   127,   128,
     129,   130,    -1,    -1,    -1,    -1,   135,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    -1,    66,    67,    68,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   124,    -1,   126,
     127,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   135,    88,
      89,    90,    91,    92,    93,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,   108,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   122,   123,   124,    -1,   126,   127,   128,
     129,   130,    48,    -1,   133,    -1,    -1,    -1,    -1,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    -1,
      66,    67,    68,    -1,    -1,    71,    72,    73,    74,    -1,
      -1,    77,    78,    -1,    -1,    -1,    -1,    -1,    84,    -1,
      -1,    -1,    88,    89,    90,    91,    92,    93,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   107,   108,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   122,   123,   124,    -1,
     126,   127,   128,   129,   130,    48,    -1,    -1,    -1,   135,
      -1,    -1,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    -1,    66,    67,    68,    -1,    -1,    71,    72,
      73,    74,    -1,    -1,    77,    78,    -1,    -1,    -1,    -1,
      -1,    84,    -1,    -1,    -1,    88,    89,    90,    91,    92,
      93,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   107,   108,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   122,
     123,   124,    -1,   126,   127,   128,   129,   130,    49,    -1,
      -1,    -1,   135,    -1,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    -1,    66,    67,    68,    -1,    -1,
      71,    72,    73,    74,    -1,    76,    77,    78,    -1,    -1,
      -1,    -1,    -1,    84,    -1,    -1,    -1,    88,    89,    90,
      91,    92,    93,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      48,    -1,    -1,    -1,    -1,    -1,   107,   108,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    -1,    66,    67,
      68,   122,   123,   124,    -1,   126,   127,   128,   129,   130,
      -1,    -1,    -1,    -1,   135,    -1,    -1,    -1,    -1,    -1,
      88,    89,    90,    91,    92,    93,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,
     108,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   122,   123,   124,    -1,   126,   127,
     128,   129,   130,    -1,    -1,   133,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    -1,    66,    67,    68,
      -1,    -1,    71,    72,    73,    74,    -1,    -1,    77,    78,
      -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,    -1,    88,
      89,    90,    91,    92,    93,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,   108,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    -1,
      66,    67,    68,   122,   123,   124,    -1,   126,   127,   128,
     129,   130,    -1,    -1,    -1,    -1,   135,    -1,    -1,    -1,
      -1,    -1,    88,    89,    90,    91,    92,    93,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   107,   108,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    -1,    66,    67,    68,   122,   123,   124,    -1,
     126,   127,   128,   129,   130,    -1,    -1,   133,    -1,    -1,
     136,    -1,    -1,    -1,    -1,    88,    89,    90,    91,    92,
      93,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   107,   108,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    -1,    66,    67,    68,   122,
     123,   124,    -1,   126,   127,   128,   129,   130,    -1,    -1,
     133,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    88,    89,
      90,    91,    92,    93,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,   108,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    -1,    66,
      67,    -1,   122,   123,   124,    -1,   126,   127,   128,   129,
     130,    -1,    -1,   133,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    88,    89,    90,    91,    92,    93,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     107,   108,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    -1,    66,    67,    -1,    -1,   123,   124,    -1,   126,
     127,   128,   129,   130,    -1,    -1,   133,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    88,    89,    90,    91,    92,    93,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   107,   108,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     124,    -1,   126,   127,   128,   129,   130,    -1,    -1,   133
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,   139,   140,     0,   141,   154,   155,   109,   142,   143,
     144,   148,   150,   151,   156,   157,   158,   264,    48,    75,
     153,   177,   243,   244,     1,   145,   143,   148,   142,   150,
       3,     4,     5,     6,    12,    13,    16,    17,    18,    19,
      20,    21,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    44,    45,    46,    47,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    66,    67,
      68,    71,    72,    73,    74,    77,    78,    84,    85,    88,
      89,    90,    91,    92,    93,   107,   108,   110,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   126,   127,   128,   129,   130,   135,   157,   159,   160,
     161,   162,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   176,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   188,   189,   190,   191,   192,   196,
     197,   202,   203,   204,   205,   235,   236,   237,   238,   239,
     240,   241,   242,   245,   246,   247,   249,   250,   254,   255,
     256,   257,   258,   259,   261,   266,   274,   276,   277,   278,
     279,   280,   281,   282,   283,   284,   285,   286,   287,   288,
     289,   290,   291,   292,   293,   294,   295,   296,   297,   298,
     327,   328,   335,   336,   155,    48,    71,    72,    78,   146,
     147,     1,    72,   135,   345,   346,   298,   163,   164,   346,
       1,   298,     1,   298,   299,    14,    15,   153,     1,    39,
      40,    71,    72,    77,    78,    84,   200,   269,   270,   271,
     340,    49,    76,   206,   232,   298,   298,   344,   153,   298,
       1,    72,   347,   347,   344,   344,   193,     1,    69,   252,
     327,   337,   341,   345,   298,     1,     1,    50,    51,    52,
     344,     1,     4,    22,    23,    24,    25,    29,    46,    79,
      80,   105,   106,   251,   253,     1,    28,   337,   344,   344,
       1,   153,     1,    25,    78,   267,   269,   270,   271,    25,
      81,   275,   344,   345,   347,   153,   344,   344,   344,   344,
     344,   344,   344,   344,   344,   344,   344,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    66,    67,    68,    88,
      89,    90,    91,    92,    93,   107,   108,   122,   123,   124,
     126,   127,   128,   129,   130,   133,   329,   330,   329,   155,
      66,   153,   346,   345,     1,   134,    48,   326,   342,   344,
     198,     1,    42,    70,    83,    96,   272,     1,   272,   199,
       1,    66,   228,    10,    11,   234,   124,    66,   228,   154,
       1,     1,     1,    25,    26,    71,    72,    78,   135,   194,
     207,   210,   341,   134,   341,    65,    71,   124,   126,   127,
     135,   338,   339,   340,     1,   298,     1,    25,    53,    54,
     248,   298,   347,     1,   344,   106,   106,    46,   105,     1,
     253,   252,     1,   345,     1,    14,    45,   188,   189,   262,
     263,   298,     1,     1,    41,    43,    87,   272,   273,     1,
     273,   298,     1,     1,     1,   311,   304,   313,   315,   314,
     316,   317,   318,   319,   310,   309,   308,   320,   321,   322,
     323,   324,   325,    72,    78,   331,   332,   335,   336,   331,
     306,   307,   312,   300,   301,   302,   303,   305,   333,   334,
     326,   147,   136,   163,     1,   136,   134,   201,   342,     1,
      82,   268,     1,   201,   298,   298,   153,   298,     1,    14,
     195,   298,     1,   211,   212,   213,   214,   218,   219,   220,
     228,   298,   344,   337,   135,   339,   135,   339,   135,   339,
     327,   337,   298,     1,    26,   252,     1,     1,     1,   298,
       1,   189,   263,   265,     1,   268,     1,   298,   298,   298,
     298,   298,   298,   298,   298,   298,   298,   298,   298,   298,
     298,   298,   298,   298,   298,   111,   329,   329,   298,   298,
     298,   298,   298,   298,   298,   298,   342,   345,   343,   136,
       1,    72,    78,     1,   228,   228,   149,   152,   156,   157,
     228,   154,   208,   209,   208,     1,   215,   219,   220,   228,
     216,   218,   220,   228,   217,   218,   219,   228,    94,    95,
      96,   136,   208,   327,   327,   327,   136,   154,     1,   154,
      45,   260,     1,   135,   137,   342,   156,   157,   204,   157,
       7,     8,     9,   233,     1,   150,   210,   220,   228,   219,
     228,   220,   228,   218,   228,   219,   228,   218,   228,     1,
     228,     1,   228,     1,   228,   228,   136,   136,   136,    14,
     152,   347,   329,   298,   298,   298,   233,    15,   228,   228,
     228,   228,   228,   228,    97,   100,   101,   102,   103,   104,
     221,   223,    97,    98,    99,   222,   223,   222,   208,   154,
       1,   228,   228,   228,   233,   154,     1,   226,   227,     1,
     224,   225,     1,   226,     1,   226,     1,   223,     1,   223,
       1,   223,   156,     1,   150,   228,   228,    14,   229,   230,
     231,   340,   346,   230
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   138,   140,   139,   141,   141,   141,   141,   142,   142,
     143,   143,   145,   144,   144,   146,   146,   147,   147,   147,
     148,   148,   149,   149,   150,   150,   151,   152,   152,   153,
     153,   154,   154,   155,   155,   156,   156,   157,   158,   159,
     159,   159,   159,   160,   160,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   160,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   160,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   160,   160,   160,   160,   160,   160,
     161,   161,   162,   162,   163,   163,   164,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   173,   174,   175,   176,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   189,   190,   191,   193,   192,   194,   194,   194,
     195,   194,   194,   194,   196,   198,   197,   199,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   200,   200,   201,
     201,   202,   203,   204,   204,   204,   205,   206,   206,   206,
     206,   206,   206,   206,   206,   207,   207,   207,   209,   208,
     208,   210,   210,   210,   211,   211,   211,   211,   212,   213,
     214,   215,   215,   215,   215,   215,   216,   216,   216,   216,
     216,   217,   217,   217,   217,   217,   218,   218,   219,   219,
     220,   220,   221,   221,   221,   221,   222,   222,   222,   222,
     222,   222,   223,   223,   223,   223,   223,   223,   223,   223,
     225,   224,   227,   226,   228,   228,   229,   229,   230,   231,
     232,   233,   233,   233,   234,   234,   234,   235,   235,   236,
     236,   237,   238,   238,   238,   238,   238,   238,   239,   240,
     241,   242,   242,   242,   243,   244,   245,   245,   245,   246,
     246,   247,   247,   247,   247,   247,   247,   247,   247,   247,
     247,   248,   248,   249,   250,   250,   250,   250,   251,   251,
     251,   251,   251,   251,   251,   252,   252,   253,   253,   253,
     253,   253,   253,   253,   253,   253,   254,   254,   254,   254,
     254,   255,   256,   257,   258,   259,   260,   260,   260,   261,
     261,   261,   261,   261,   261,   262,   262,   262,   263,   263,
     263,   263,   264,   264,   265,   265,   266,   266,   266,   266,
     266,   266,   266,   266,   266,   266,   266,   267,   267,   267,
     268,   268,   268,   269,   269,   270,   271,   272,   272,   272,
     272,   273,   273,   273,   273,   274,   274,   274,   274,   275,
     276,   277,   278,   279,   280,   281,   282,   283,   284,   285,
     286,   287,   288,   289,   290,   291,   292,   293,   294,   295,
     296,   297,   299,   298,   300,   298,   301,   298,   302,   298,
     298,   303,   298,   298,   304,   298,   298,   305,   298,   298,
     306,   298,   298,   307,   298,   298,   308,   298,   298,   309,
     298,   298,   310,   298,   298,   311,   298,   298,   298,   298,
     312,   298,   298,   313,   298,   298,   314,   298,   298,   315,
     298,   298,   316,   298,   298,   317,   298,   298,   318,   298,
     298,   319,   298,   298,   320,   298,   298,   321,   298,   298,
     322,   298,   298,   323,   298,   298,   324,   298,   298,   325,
     298,   298,   298,   298,   298,   298,   298,   298,   298,   298,
     298,   298,   298,   298,   298,   298,   298,   326,   326,   326,
     327,   328,   328,   330,   329,   331,   331,   331,   331,   331,
     331,   332,   334,   333,   335,   336,   337,   337,   337,   338,
     338,   338,   338,   338,   338,   338,   338,   338,   339,   339,
     340,   340,   340,   341,   341,   341,   343,   342,   342,   344,
     344,   345,   345,   345,   345,   346,   347,   347
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
       2,     1,     1,     2,     1,     3,     6,     7,     6,     5,
       4,     2,     2,     2,     0,     1,     1,     1,     0,     2,
       0,     2,     3,     2,     1,     1,     1,     1,     2,     2,
       2,     2,     3,     2,     3,     1,     2,     3,     2,     3,
       1,     2,     3,     2,     3,     1,     4,     3,     4,     3,
       4,     3,     1,     2,     2,     1,     1,     2,     2,     2,
       2,     1,     2,     2,     2,     2,     2,     2,     2,     2,
       0,     3,     0,     3,     1,     0,     1,     1,     1,     1,
       1,     3,     3,     3,     3,     3,     0,     3,     2,     3,
       2,     2,     6,     9,     9,     6,     2,     4,     2,     2,
       2,     2,     3,     1,     1,     1,     2,     3,     1,     1,
       2,     3,     2,     4,     3,     2,     4,     3,     2,     3,
       2,     1,     1,     2,     4,     3,     3,     2,     1,     2,
       2,     1,     2,     2,     1,     3,     1,     1,     1,     1,
       1,     1,     1,     2,     3,     2,     1,     2,     3,     4,
       3,     2,     2,     2,     2,     2,     2,     3,     1,     5,
       4,     4,     2,     3,     5,     2,     1,     1,     6,     7,
       2,     2,     2,     2,     3,     0,     3,     3,     2,     3,
       3,     4,     5,     4,     3,     4,     3,     1,     1,     1,
       2,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     2,     3,     2,     1,
       2,     1,     2,     1,     2,     1,     2,     1,     2,     1,
       2,     1,     2,     1,     2,     1,     2,     1,     2,     1,
       2,     1,     0,     3,     0,     4,     0,     4,     0,     4,
       1,     0,     4,     1,     0,     4,     1,     0,     4,     1,
       0,     4,     1,     0,     4,     1,     0,     4,     1,     0,
       4,     1,     0,     4,     1,     0,     4,     1,     2,     1,
       0,     4,     1,     0,     4,     1,     0,     4,     1,     0,
       4,     1,     0,     4,     1,     0,     4,     1,     0,     4,
       1,     0,     4,     1,     0,     4,     1,     0,     4,     1,
       0,     4,     1,     0,     4,     1,     0,     4,     1,     0,
       4,     1,     3,     3,     1,     1,     3,     1,     1,     1,
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
#line 169 "yaccsrc.y"
                                       { level = 0;
                                         if ( get_options_flag( parser_data.TSD->currlevel, EXT_CALLS_AS_FUNCS )
                                           && !get_options_flag( parser_data.TSD->currlevel, EXT_STRICT_ANSI ) )
                                            start_parendepth = 1;
                                         else
                                            start_parendepth = 0;
                                         parendepth = 0; }
#line 2512 "yaccsrc.c"
    break;

  case 4: /* prog: nlncl stats  */
#line 179 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-1], yyvsp[0] );
                                         yyval->o.last = NULL;
                                         EndProg( yyval ) ; }
#line 2520 "yaccsrc.c"
    break;

  case 5: /* prog: nlncl  */
#line 182 "yaccsrc.y"
                                       { yyval = yyvsp[0];
                                         if ( yyval != NULL )
                                            yyval->o.last = NULL;
                                         EndProg( yyval ); }
#line 2529 "yaccsrc.c"
    break;

  case 6: /* prog: nlncl stats dirsect  */
#line 186 "yaccsrc.y"
                                       { yyval = optgluelast( makenode( X_OO_INIT, 0 ), yyvsp[-2] );
                                         yyval = optgluelast( yyval, yyvsp[-1] );
                                         yyval = optgluelast( yyval, yyvsp[0] );
                                         yyval->o.last = NULL;
                                         EndProg( yyval ) ; }
#line 2539 "yaccsrc.c"
    break;

  case 7: /* prog: nlncl dirsect  */
#line 191 "yaccsrc.y"
                                       { yyval = optgluelast( makenode( X_OO_INIT, 0 ), yyvsp[-1] );
                                         yyval = optgluelast( yyval, yyvsp[0] );
                                         yyval->o.last = NULL;
                                         EndProg( yyval ) ; }
#line 2548 "yaccsrc.c"
    break;

  case 8: /* dirsect: dirsect dirblock  */
#line 202 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 2554 "yaccsrc.c"
    break;

  case 9: /* dirsect: dirblock  */
#line 203 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2560 "yaccsrc.c"
    break;

  case 10: /* dirblock: directive stats  */
#line 206 "yaccsrc.y"
                                       { yyval = yyvsp[-1];
                                         yyval->p[1] = optgluelast( yyval->p[1], yyvsp[0] );
                                         if ( yyval->p[1] != NULL )
                                            yyval->p[1]->o.last = NULL; }
#line 2569 "yaccsrc.c"
    break;

  case 11: /* dirblock: directive  */
#line 210 "yaccsrc.y"
                                       { yyval = yyvsp[0];
                                         if ( yyval->p[1] != NULL )
                                            yyval->p[1]->o.last = NULL; }
#line 2577 "yaccsrc.c"
    break;

  case 12: /* @2: %empty  */
#line 215 "yaccsrc.y"
                                       { yyval = makenode( X_DIRECTIVE, 0 );
                                         yyval->lineno = parser_data.tline;
                                         yyval->charnr = parser_data.tstart; }
#line 2585 "yaccsrc.c"
    break;

  case 13: /* directive: DCOLON @2 dirwords ncl  */
#line 218 "yaccsrc.y"
                                       { yyval = yyvsp[-2];
                                         /* take over the keyword; the
                                          * node must not own it twice */
                                         yyval->name = yyvsp[-1]->name;
                                         yyvsp[-1]->name = NULL;
                                         yyval->p[0] = yyvsp[-1]->next;
                                         if ( yyval->p[0] != NULL )
                                            yyval->p[0]->o.last = NULL;
                                         yyval->p[1] = yyvsp[0]; /* labels */ }
#line 2599 "yaccsrc.c"
    break;

  case 14: /* directive: DCOLON error  */
#line 227 "yaccsrc.y"
                                       { exiterror( ERR_YACC_SYNTAX, 1, parser_data.tline ); }
#line 2605 "yaccsrc.c"
    break;

  case 15: /* dirwords: dirwords SPACE dirword  */
#line 230 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-2], yyvsp[0] ); }
#line 2611 "yaccsrc.c"
    break;

  case 16: /* dirwords: dirword  */
#line 231 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2617 "yaccsrc.c"
    break;

  case 17: /* dirword: SIMSYMBOL  */
#line 234 "yaccsrc.y"
                                       { yyval = makenode( X_STRING, 0 );
                                         yyval->name = Str_cre_TSD( parser_data.TSD, retvalue ); }
#line 2624 "yaccsrc.c"
    break;

  case 18: /* dirword: CONSYMBOL  */
#line 236 "yaccsrc.y"
                                       { yyval = makenode( X_STRING, 0 );
                                         yyval->name = Str_cre_TSD( parser_data.TSD, retvalue ); }
#line 2631 "yaccsrc.c"
    break;

  case 19: /* dirword: STRING  */
#line 238 "yaccsrc.y"
                                       { yyval = makenode( X_STRING, 0 );
                                         yyval->name = Str_cre_TSD( parser_data.TSD, retvalue ); }
#line 2638 "yaccsrc.c"
    break;

  case 20: /* stats: stats ystatement  */
#line 242 "yaccsrc.y"
                                       { /* fixes bug 579711 */
                                         yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 2645 "yaccsrc.c"
    break;

  case 21: /* stats: ystatement  */
#line 244 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2651 "yaccsrc.c"
    break;

  case 22: /* xstats: xstats statement  */
#line 247 "yaccsrc.y"
                                       { /* fixes bug 579711 */
                                         yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 2658 "yaccsrc.c"
    break;

  case 23: /* xstats: statement gruff  */
#line 249 "yaccsrc.y"
                                       { yyval = yyvsp[-1]; }
#line 2664 "yaccsrc.c"
    break;

  case 24: /* ystatement: statement  */
#line 252 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 2670 "yaccsrc.c"
    break;

  case 25: /* ystatement: lonely_end  */
#line 253 "yaccsrc.y"
                                       { exiterror( ERR_UNMATCHED_END, 1 ); }
#line 2676 "yaccsrc.c"
    break;

  case 27: /* nxstats: xstats  */
#line 261 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2682 "yaccsrc.c"
    break;

  case 28: /* nxstats: gruff  */
#line 262 "yaccsrc.y"
                                       { yyval = NULL; }
#line 2688 "yaccsrc.c"
    break;

  case 29: /* ncl: ncl STATSEP optLabels  */
#line 265 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-2], yyvsp[0] ); }
#line 2694 "yaccsrc.c"
    break;

  case 30: /* ncl: STATSEP optLabels  */
#line 266 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2700 "yaccsrc.c"
    break;

  case 31: /* nlncl: optLabels ncl  */
#line 269 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 2706 "yaccsrc.c"
    break;

  case 32: /* nlncl: optLabels  */
#line 270 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2712 "yaccsrc.c"
    break;

  case 33: /* optLabels: optLabels label_stat  */
#line 273 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 2718 "yaccsrc.c"
    break;

  case 34: /* optLabels: %empty  */
#line 274 "yaccsrc.y"
                                       { yyval = NULL; }
#line 2724 "yaccsrc.c"
    break;

  case 37: /* gruff: %empty  */
#line 281 "yaccsrc.y"
                                       { tmpchr=parser_data.tstart;
                                         tmplno=parser_data.tline; }
#line 2731 "yaccsrc.c"
    break;

  case 38: /* mttstatement: gruff mtstatement  */
#line 285 "yaccsrc.y"
                                       { yyval=yyvsp[0]; }
#line 2737 "yaccsrc.c"
    break;

  case 39: /* mtstatement: nclstatement ncl  */
#line 288 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 2743 "yaccsrc.c"
    break;

  case 80: /* oo_expose_stat: EXPOSE anyvars  */
#line 337 "yaccsrc.y"
                                       { yyval = makenode( X_OO_EXPOSE, 1, yyvsp[0] );
                                         yyval->lineno = tmplno;
                                         yyval->charnr = tmpchr; }
#line 2751 "yaccsrc.c"
    break;

  case 81: /* oo_expose_stat: EXPOSE error  */
#line 340 "yaccsrc.y"
                                       { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ); }
#line 2757 "yaccsrc.c"
    break;

  case 82: /* oo_usearg_stat: USEARG usevars  */
#line 343 "yaccsrc.y"
                                       { yyval = makenode( X_OO_USEARG, 1, yyvsp[0] );
                                         yyval->lineno = tmplno;
                                         yyval->charnr = tmpchr; }
#line 2765 "yaccsrc.c"
    break;

  case 83: /* oo_usearg_stat: USEARG usevars error  */
#line 346 "yaccsrc.y"
                                       { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ); }
#line 2771 "yaccsrc.c"
    break;

  case 84: /* usevars: usevar ',' usevars  */
#line 349 "yaccsrc.y"
                                       { yyval = yyvsp[-2];
                                         yyval->p[0] = yyvsp[0]; }
#line 2778 "yaccsrc.c"
    break;

  case 85: /* usevars: usevar  */
#line 351 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2784 "yaccsrc.c"
    break;

  case 86: /* usevar: xsimsymb  */
#line 354 "yaccsrc.y"
                                       { yyval = makenode( X_SIM_SYMBOL, 0 );
                                         yyval->name = (streng *) yyvsp[0]; }
#line 2791 "yaccsrc.c"
    break;

  case 87: /* usevar: %empty  */
#line 356 "yaccsrc.y"
                                       { yyval = makenode( X_SIM_SYMBOL, 0 );
                                         yyval->name = NULL; }
#line 2798 "yaccsrc.c"
    break;

  case 88: /* call: CALL  */
#line 360 "yaccsrc.y"
                                       { yyval = makenode(X_CALL,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2806 "yaccsrc.c"
    break;

  case 89: /* do: DO  */
#line 364 "yaccsrc.y"
                                       { yyval = makenode(X_DO,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ;
                                         level++; }
#line 2815 "yaccsrc.c"
    break;

  case 90: /* exit: EXIT  */
#line 369 "yaccsrc.y"
                                       { yyval = makenode(X_EXIT,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2823 "yaccsrc.c"
    break;

  case 91: /* if: IF  */
#line 373 "yaccsrc.y"
                                       { yyval = makenode(X_IF,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ;
                                         level++; }
#line 2832 "yaccsrc.c"
    break;

  case 92: /* iterate: ITERATE  */
#line 378 "yaccsrc.y"
                                       { yyval = makenode(X_ITERATE,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2840 "yaccsrc.c"
    break;

  case 93: /* leave: LEAVE  */
#line 382 "yaccsrc.y"
                                       { yyval = makenode(X_LEAVE,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2848 "yaccsrc.c"
    break;

  case 94: /* say: SAY  */
#line 386 "yaccsrc.y"
                                       { yyval = makenode(X_SAY,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2856 "yaccsrc.c"
    break;

  case 95: /* return: RETURN  */
#line 390 "yaccsrc.y"
                                       { yyval = makenode(X_RETURN,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2864 "yaccsrc.c"
    break;

  case 96: /* address: ADDRESS  */
#line 394 "yaccsrc.y"
                                       { yyval = makenode(X_ADDR_N,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2872 "yaccsrc.c"
    break;

  case 97: /* arg: ARG  */
#line 398 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_ARG,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2880 "yaccsrc.c"
    break;

  case 98: /* drop: DROP  */
#line 402 "yaccsrc.y"
                                       { yyval = makenode(X_DROP,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2888 "yaccsrc.c"
    break;

  case 99: /* interpret: INTERPRET  */
#line 406 "yaccsrc.y"
                                       { yyval = makenode(X_IPRET,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2896 "yaccsrc.c"
    break;

  case 100: /* label: LABEL  */
#line 410 "yaccsrc.y"
                                       { yyval = makenode(X_LABEL,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2904 "yaccsrc.c"
    break;

  case 101: /* nop: NOP  */
#line 414 "yaccsrc.y"
                                       { yyval = makenode(X_NULL,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2912 "yaccsrc.c"
    break;

  case 102: /* numeric: NUMERIC  */
#line 418 "yaccsrc.y"
                                       { yyval = makenode(0,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2920 "yaccsrc.c"
    break;

  case 103: /* options: OPTIONS  */
#line 422 "yaccsrc.y"
                                       { yyval = makenode(X_OPTIONS,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2928 "yaccsrc.c"
    break;

  case 104: /* parse: PARSE  */
#line 426 "yaccsrc.y"
                                       { yyval = makenode(0,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2936 "yaccsrc.c"
    break;

  case 105: /* proc: PROCEDURE  */
#line 430 "yaccsrc.y"
                                       { yyval = makenode(X_PROC,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2944 "yaccsrc.c"
    break;

  case 106: /* pull: PULL  */
#line 434 "yaccsrc.y"
                                       { yyval = makenode(X_PULL,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2952 "yaccsrc.c"
    break;

  case 107: /* push: PUSH  */
#line 438 "yaccsrc.y"
                                       { yyval = makenode(X_PUSH,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2960 "yaccsrc.c"
    break;

  case 108: /* queue: QUEUE  */
#line 442 "yaccsrc.y"
                                       { yyval = makenode(X_QUEUE,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2968 "yaccsrc.c"
    break;

  case 109: /* select: SELECT  */
#line 446 "yaccsrc.y"
                                       { yyval = makenode(X_SELECT,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ;
                                         level++; }
#line 2977 "yaccsrc.c"
    break;

  case 110: /* signal: SIGNAL  */
#line 451 "yaccsrc.y"
                                       { yyval = makenode(X_SIG_LAB,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2985 "yaccsrc.c"
    break;

  case 111: /* when: WHEN  */
#line 455 "yaccsrc.y"
                                       { yyval = makenode(X_WHEN,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2993 "yaccsrc.c"
    break;

  case 112: /* otherwise: OTHERWISE  */
#line 459 "yaccsrc.y"
                                       { yyval = makenode(X_OTHERWISE,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3001 "yaccsrc.c"
    break;

  case 113: /* trace: TRACE  */
#line 463 "yaccsrc.y"
                                       { yyval = makenode(X_TRACE,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3009 "yaccsrc.c"
    break;

  case 114: /* upper: UPPER  */
#line 467 "yaccsrc.y"
                                       { yyval = makenode(X_UPPER_VAR,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3017 "yaccsrc.c"
    break;

  case 115: /* @3: %empty  */
#line 471 "yaccsrc.y"
                                       { yyval = current = yyvsp[0] ; }
#line 3023 "yaccsrc.c"
    break;

  case 117: /* address_stat2: VALUE expr naddr_with  */
#line 475 "yaccsrc.y"
                                       { current->type = X_ADDR_V ;
                                         current->p[0] = yyvsp[-1] ;
                                         current->p[1] = yyvsp[0] ; }
#line 3031 "yaccsrc.c"
    break;

  case 118: /* address_stat2: addr_with  */
#line 478 "yaccsrc.y"
                                       { exiterror( ERR_STRING_EXPECTED, 1, __reginatext ) ;}
#line 3037 "yaccsrc.c"
    break;

  case 119: /* address_stat2: %empty  */
#line 479 "yaccsrc.y"
                                       { current->type = X_ADDR_S ; }
#line 3043 "yaccsrc.c"
    break;

  case 120: /* $@4: %empty  */
#line 480 "yaccsrc.y"
                                       { exiterror( ERR_STRING_EXPECTED, 1, __reginatext ) ;}
#line 3049 "yaccsrc.c"
    break;

  case 122: /* address_stat2: nvir nexpr naddr_with  */
#line 482 "yaccsrc.y"
                                       { current->name = (streng *)yyvsp[-2] ;
                                         current->type = X_ADDR_N ;
                                         current->p[0] = yyvsp[-1] ;
                                         current->p[1] = yyvsp[0] ; }
#line 3058 "yaccsrc.c"
    break;

  case 123: /* address_stat2: '(' expr ')' nspace naddr_with  */
#line 486 "yaccsrc.y"
                                              { current->type = X_ADDR_V ;
                                         current->p[0] = yyvsp[-3] ;
                                         current->p[1] = yyvsp[0] ;
                                         current->u.nonansi = 1 ; }
#line 3067 "yaccsrc.c"
    break;

  case 124: /* arg_stat: arg templs  */
#line 492 "yaccsrc.y"
                                       { yyval = makenode( X_PARSE, 2, yyvsp[-1], yyvsp[0] );
                                         yyval->u.parseflags = PARSE_UPPER;
                                         yyval->lineno = yyvsp[-1]->lineno;
                                         yyval->charnr = yyvsp[-1]->charnr; }
#line 3076 "yaccsrc.c"
    break;

  case 125: /* $@5: %empty  */
#line 498 "yaccsrc.y"
                                       { parendepth = start_parendepth; }
#line 3082 "yaccsrc.c"
    break;

  case 126: /* call_stat: call call_name $@5 call_args  */
#line 499 "yaccsrc.y"
                                       { yyval = yyvsp[-3];
                                         yyval->p[0] = yyvsp[0];
                                         yyval->name = (streng *) yyvsp[-2];
                                         parendepth = 0; }
#line 3091 "yaccsrc.c"
    break;

  case 127: /* $@6: %empty  */
#line 503 "yaccsrc.y"
                                       { parendepth = start_parendepth; }
#line 3097 "yaccsrc.c"
    break;

  case 128: /* call_stat: call string $@6 call_args  */
#line 504 "yaccsrc.y"
                                       { yyval = yyvsp[-3];
                                         yyval->type = X_EX_FUNC;
                                         yyval->p[0] = yyvsp[0];
                                         yyval->name = (streng *) yyvsp[-2];
                                         parendepth = 0; }
#line 3107 "yaccsrc.c"
    break;

  case 129: /* call_stat: call on error  */
#line 509 "yaccsrc.y"
                             { exiterror( ERR_INV_SUBKEYWORD, 1, "ERROR FAILURE HALT NOTREADY", __reginatext ) ;}
#line 3113 "yaccsrc.c"
    break;

  case 130: /* call_stat: call off error  */
#line 510 "yaccsrc.y"
                              { exiterror( ERR_INV_SUBKEYWORD, 2, "ERROR FAILURE HALT NOTREADY", __reginatext ) ;}
#line 3119 "yaccsrc.c"
    break;

  case 131: /* call_stat: call on c_action error  */
#line 511 "yaccsrc.y"
                                      { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 3125 "yaccsrc.c"
    break;

  case 132: /* call_stat: call on c_action namespec error  */
#line 512 "yaccsrc.y"
                                               { exiterror( ERR_STRING_EXPECTED, 3, __reginatext ) ;}
#line 3131 "yaccsrc.c"
    break;

  case 133: /* call_stat: call on c_action namespec  */
#line 514 "yaccsrc.y"
                                       { yyval = yyvsp[-3] ;
                                         yyval->type = X_CALL_SET ;
                                         yyval->p[0] = yyvsp[-2] ;
                                         yyval->name = (streng *)yyvsp[0] ;
                                         yyval->p[1] = yyvsp[-1] ; }
#line 3141 "yaccsrc.c"
    break;

  case 134: /* call_stat: call on c_action  */
#line 519 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_CALL_SET ;
                                         yyval->p[0] = yyvsp[-1] ;
                                         yyval->name = NULL ;
                                         yyval->p[1] = yyvsp[0] ; }
#line 3151 "yaccsrc.c"
    break;

  case 135: /* call_stat: call off c_action error  */
#line 524 "yaccsrc.y"
                                       { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 3157 "yaccsrc.c"
    break;

  case 136: /* call_stat: call off c_action  */
#line 525 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_CALL_SET ;
                                         yyval->p[0] = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ; }
#line 3166 "yaccsrc.c"
    break;

  case 137: /* call_name: asymbol  */
#line 531 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 3172 "yaccsrc.c"
    break;

  case 138: /* call_name: error  */
#line 532 "yaccsrc.y"
                                       { exiterror( ERR_STRING_EXPECTED, 2, __reginatext );}
#line 3178 "yaccsrc.c"
    break;

  case 139: /* call_args: exprs  */
#line 535 "yaccsrc.y"
                                       {
                                         /*
                                          * "call_args" accepted probably with
                                          * surrounding parentheses. Strip them.
                                          */
                                         yyval = reduce_expr_list( yyvsp[0],
                                                                REDUCE_CALL );
                                       }
#line 3191 "yaccsrc.c"
    break;

  case 140: /* call_args: exprs ')'  */
#line 543 "yaccsrc.y"
                                       { exiterror(ERR_UNEXPECTED_PARAN, 2); }
#line 3197 "yaccsrc.c"
    break;

  case 141: /* expr_stat: expr  */
#line 546 "yaccsrc.y"
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
#line 3222 "yaccsrc.c"
    break;

  case 142: /* end_stat: END  */
#line 568 "yaccsrc.y"
                                       { yyval = makenode(X_END,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ;
                                         level--; }
#line 3231 "yaccsrc.c"
    break;

  case 143: /* end: end_stat simsymb  */
#line 574 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->name = (streng*)(yyvsp[0]) ; }
#line 3238 "yaccsrc.c"
    break;

  case 144: /* end: end_stat  */
#line 576 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3244 "yaccsrc.c"
    break;

  case 145: /* end: end_stat simsymb error  */
#line 577 "yaccsrc.y"
                                       {  exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 3250 "yaccsrc.c"
    break;

  case 146: /* do_stat: do repetitor conditional ncl nxstats end  */
#line 581 "yaccsrc.y"
                                       { nodeptr overvar;
                                         yyval = yyvsp[-5];
                                         yyval->p[0] = yyvsp[-4];
                                         yyval->p[1] = yyvsp[-3];
                                         yyval->p[2] = optgluelast( yyvsp[-2], yyvsp[-1] );
                                         if ( yyval->p[2] )
                                            yyval->p[2]->o.last = NULL;
                                         yyval->p[3] = yyvsp[0];
                                         overvar = do_over_var( yyvsp[-4] );
                          if ( overvar )
                          {
                             /* OO: DO name OVER collection */
                             if ( yyval->p[3]->name
                               && Str_cmp( yyval->p[3]->name, overvar->name ) != 0 )
                                exiterror( ERR_UNMATCHED_END, 0 );
                             transform_do_over( yyval, overvar );
                          }
                          else
                          {
                          if ((yyval->p[0]==NULL || yyval->p[0]->name==NULL)
                              && yyval->p[3]->name)
                                            exiterror( ERR_UNMATCHED_END, 0 );
                          if ((yyval->p[0])&&(yyval->p[0]->name)&&
                              (yyval->p[3]->name)&&
                              ((yyval->p[3]->name->len != yyval->p[0]->name->len)||
                               (strncmp(yyval->p[3]->name->value,
                                        yyval->p[0]->name->value,
                                        yyval->p[0]->name->len))))
                                            exiterror( ERR_UNMATCHED_END, 0 );
                          }
                                       }
#line 3286 "yaccsrc.c"
    break;

  case 147: /* repetitor: dovar '=' expr nspace tobyfor tobyfor tobyfor  */
#line 615 "yaccsrc.y"
                                       { yyval =makenode(X_REP,4,yyvsp[-4],yyvsp[-2],yyvsp[-1],yyvsp[0]) ;
                                         yyval->name = (streng *)yyvsp[-6] ;
                                         checkdosyntax(yyval) ; }
#line 3294 "yaccsrc.c"
    break;

  case 148: /* repetitor: dovar '=' expr nspace tobyfor tobyfor  */
#line 619 "yaccsrc.y"
                                       { yyval =makenode(X_REP,3,yyvsp[-3],yyvsp[-1],yyvsp[0]) ;
                                         yyval->name = (streng *)yyvsp[-5] ;
                                         checkdosyntax(yyval) ; }
#line 3302 "yaccsrc.c"
    break;

  case 149: /* repetitor: dovar '=' expr nspace tobyfor  */
#line 623 "yaccsrc.y"
                                       { yyval = makenode(X_REP,2,yyvsp[-2],yyvsp[0]) ;
                                         yyval->name = (streng *)yyvsp[-4] ;
                                         checkdosyntax(yyval) ; }
#line 3310 "yaccsrc.c"
    break;

  case 150: /* repetitor: dovar '=' expr nspace  */
#line 626 "yaccsrc.y"
                                       { yyval = makenode(X_REP,1,yyvsp[-1]) ;
                                         yyval->name = (streng *)yyvsp[-3] ;
                                         checkdosyntax(yyval) ; }
#line 3318 "yaccsrc.c"
    break;

  case 151: /* repetitor: FOREVER nspace  */
#line 629 "yaccsrc.y"
                                       { yyval = makenode(X_REP_FOREVER,0) ; }
#line 3324 "yaccsrc.c"
    break;

  case 152: /* repetitor: FOREVER error  */
#line 630 "yaccsrc.y"
                             { exiterror( ERR_INV_SUBKEYWORD, 16, "WHILE UNTIL", __reginatext ) ; }
#line 3330 "yaccsrc.c"
    break;

  case 153: /* repetitor: expr nspace  */
#line 631 "yaccsrc.y"
                                       { yyvsp[-1] = makenode(X_DO_EXPR,1,yyvsp[-1]) ;
                                         yyval = makenode(X_REP,2,NULL,yyvsp[-1]) ; }
#line 3337 "yaccsrc.c"
    break;

  case 154: /* repetitor: %empty  */
#line 633 "yaccsrc.y"
                                       { yyval = NULL ; }
#line 3343 "yaccsrc.c"
    break;

  case 155: /* nvir: CONSYMBOL  */
#line 636 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 3349 "yaccsrc.c"
    break;

  case 156: /* nvir: SIMSYMBOL  */
#line 637 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 3355 "yaccsrc.c"
    break;

  case 157: /* nvir: STRING  */
#line 638 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 3361 "yaccsrc.c"
    break;

  case 158: /* @7: %empty  */
#line 641 "yaccsrc.y"
                                       { SymbolDetect |= SD_ADDRWITH ;
                                         yyval = with = makenode(X_ADDR_WITH,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3370 "yaccsrc.c"
    break;

  case 159: /* naddr_with: @7 addr_with  */
#line 645 "yaccsrc.y"
                                       { with = NULL ;
                                         SymbolDetect &= ~SD_ADDRWITH ; }
#line 3377 "yaccsrc.c"
    break;

  case 160: /* naddr_with: %empty  */
#line 647 "yaccsrc.y"
                                       { yyval = NULL ; }
#line 3383 "yaccsrc.c"
    break;

  case 161: /* addr_with: WITH connection  */
#line 650 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 3389 "yaccsrc.c"
    break;

  case 162: /* addr_with: WITH connection error  */
#line 651 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 5, __reginatext ) ; }
#line 3395 "yaccsrc.c"
    break;

  case 163: /* addr_with: WITH nspace  */
#line 652 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 5, __reginatext ) ; }
#line 3401 "yaccsrc.c"
    break;

  case 167: /* connection: error  */
#line 658 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 5, __reginatext ) ; }
#line 3407 "yaccsrc.c"
    break;

  case 186: /* inputstmt: nspace INPUT nspace resourcei  */
#line 694 "yaccsrc.y"
                                             { with->p[0] = yyvsp[0]; }
#line 3413 "yaccsrc.c"
    break;

  case 187: /* inputstmt: nspace INPUT error  */
#line 695 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 6, __reginatext ) ; }
#line 3419 "yaccsrc.c"
    break;

  case 188: /* outputstmt: nspace OUTPUT nspace resourceo  */
#line 698 "yaccsrc.y"
                                              { with->p[1] = yyvsp[0]; }
#line 3425 "yaccsrc.c"
    break;

  case 189: /* outputstmt: nspace OUTPUT error  */
#line 699 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 7, __reginatext ) ; }
#line 3431 "yaccsrc.c"
    break;

  case 190: /* errorstmt: nspace ERROR nspace resourceo  */
#line 702 "yaccsrc.y"
                                             { with->p[2] = yyvsp[0]; }
#line 3437 "yaccsrc.c"
    break;

  case 191: /* errorstmt: nspace ERROR error  */
#line 703 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 14, __reginatext ) ; }
#line 3443 "yaccsrc.c"
    break;

  case 192: /* resourcei: resources  */
#line 706 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3449 "yaccsrc.c"
    break;

  case 193: /* resourcei: NOEOL resources  */
#line 707 "yaccsrc.y"
                                       { yyval = yyvsp[0] ;
                                         yyval->u.of.noeol = 1 ; }
#line 3456 "yaccsrc.c"
    break;

  case 194: /* resourcei: NOEOL error  */
#line 709 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 8, __reginatext ) ; }
#line 3462 "yaccsrc.c"
    break;

  case 195: /* resourcei: NORMAL  */
#line 710 "yaccsrc.y"
                                       { yyval = makenode(X_ADDR_WITH, 0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3470 "yaccsrc.c"
    break;

  case 196: /* resourceo: resources  */
#line 715 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3476 "yaccsrc.c"
    break;

  case 197: /* resourceo: APPEND resources  */
#line 716 "yaccsrc.y"
                                       { yyval = yyvsp[0] ;
                                         yyval->u.of.append = 1 ; }
#line 3483 "yaccsrc.c"
    break;

  case 198: /* resourceo: APPEND error  */
#line 718 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 8, __reginatext ) ; }
#line 3489 "yaccsrc.c"
    break;

  case 199: /* resourceo: REPLACE resources  */
#line 719 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3495 "yaccsrc.c"
    break;

  case 200: /* resourceo: REPLACE error  */
#line 720 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 9, __reginatext ) ; }
#line 3501 "yaccsrc.c"
    break;

  case 201: /* resourceo: NORMAL  */
#line 721 "yaccsrc.y"
                                       { yyval = makenode(X_ADDR_WITH, 0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3509 "yaccsrc.c"
    break;

  case 202: /* resources: STREAM nnvir  */
#line 726 "yaccsrc.y"
                                       { /* ANSI extension: nsimsymb is
                                          * used by the standard, but I think
                                          * there are no reasons why using
                                          * it here as a must. FGC
                                          */
                                         yyval = yyvsp[0] ;
                                         yyval->u.of.awt = awtSTREAM;
                                         SymbolDetect |= SD_ADDRWITH ; }
#line 3522 "yaccsrc.c"
    break;

  case 203: /* resources: STREAM error  */
#line 734 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_OPTION, 1, __reginatext ) ; }
#line 3528 "yaccsrc.c"
    break;

  case 204: /* resources: STEM nsimsymb  */
#line 735 "yaccsrc.y"
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
#line 3546 "yaccsrc.c"
    break;

  case 205: /* resources: STEM error  */
#line 748 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_OPTION, 2, __reginatext ) ; }
#line 3552 "yaccsrc.c"
    break;

  case 206: /* resources: LIFO nnvir  */
#line 749 "yaccsrc.y"
                                       {
                                         yyval = yyvsp[0] ;
                                         yyval->u.of.awt = awtLIFO ;
                                         SymbolDetect |= SD_ADDRWITH ; }
#line 3561 "yaccsrc.c"
    break;

  case 207: /* resources: LIFO error  */
#line 753 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_OPTION, 100, __reginatext ) ; }
#line 3567 "yaccsrc.c"
    break;

  case 208: /* resources: FIFO nnvir  */
#line 754 "yaccsrc.y"
                                       {
                                         yyval = yyvsp[0] ;
                                         yyval->u.of.awt = awtFIFO ;
                                         SymbolDetect |= SD_ADDRWITH ; }
#line 3576 "yaccsrc.c"
    break;

  case 209: /* resources: FIFO error  */
#line 758 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_OPTION, 101, __reginatext ) ; }
#line 3582 "yaccsrc.c"
    break;

  case 210: /* $@8: %empty  */
#line 761 "yaccsrc.y"
                                       { SymbolDetect &= ~SD_ADDRWITH ; }
#line 3588 "yaccsrc.c"
    break;

  case 211: /* nsimsymb: $@8 nspace addrSim  */
#line 762 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3594 "yaccsrc.c"
    break;

  case 212: /* $@9: %empty  */
#line 765 "yaccsrc.y"
                                       { SymbolDetect &= ~SD_ADDRWITH ; }
#line 3600 "yaccsrc.c"
    break;

  case 213: /* nnvir: $@9 nspace addrAll  */
#line 766 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3606 "yaccsrc.c"
    break;

  case 216: /* addrAll: addrSim  */
#line 773 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3612 "yaccsrc.c"
    break;

  case 217: /* addrAll: addrString  */
#line 774 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3618 "yaccsrc.c"
    break;

  case 218: /* addrSim: xsimsymb  */
#line 777 "yaccsrc.y"
                                       { yyval = makenode(X_ADDR_WITH, 0) ;
                                         yyval->name = (streng *) yyvsp[0] ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->u.of.ant = antSIMSYMBOL;
                                       }
#line 3629 "yaccsrc.c"
    break;

  case 219: /* addrString: string  */
#line 785 "yaccsrc.y"
                                       { yyval = makenode(X_ADDR_WITH, 0) ;
                                         yyval->name = (streng *) yyvsp[0] ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->u.of.ant = antSTRING;
                                       }
#line 3640 "yaccsrc.c"
    break;

  case 220: /* dovar: DOVARIABLE  */
#line 793 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 3646 "yaccsrc.c"
    break;

  case 221: /* tobyfor: TO expr nspace  */
#line 796 "yaccsrc.y"
                                       { yyval = makenode(X_DO_TO,1,yyvsp[-1]) ; }
#line 3652 "yaccsrc.c"
    break;

  case 222: /* tobyfor: FOR expr nspace  */
#line 797 "yaccsrc.y"
                                       { yyval = makenode(X_DO_FOR,1,yyvsp[-1]) ; }
#line 3658 "yaccsrc.c"
    break;

  case 223: /* tobyfor: BY expr nspace  */
#line 798 "yaccsrc.y"
                                       { yyval = makenode(X_DO_BY,1,yyvsp[-1]) ; }
#line 3664 "yaccsrc.c"
    break;

  case 224: /* conditional: WHILE expr nspace  */
#line 801 "yaccsrc.y"
                                       { yyval = makenode(X_WHILE,1,yyvsp[-1]) ; }
#line 3670 "yaccsrc.c"
    break;

  case 225: /* conditional: UNTIL expr nspace  */
#line 802 "yaccsrc.y"
                                       { yyval = makenode(X_UNTIL,1,yyvsp[-1]) ; }
#line 3676 "yaccsrc.c"
    break;

  case 226: /* conditional: %empty  */
#line 803 "yaccsrc.y"
                                       { yyval = NULL ; }
#line 3682 "yaccsrc.c"
    break;

  case 227: /* drop_stat: drop anyvars error  */
#line 806 "yaccsrc.y"
                                  { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ) ;}
#line 3688 "yaccsrc.c"
    break;

  case 228: /* drop_stat: drop anyvars  */
#line 807 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3695 "yaccsrc.c"
    break;

  case 229: /* upper_stat: upper anyvars error  */
#line 811 "yaccsrc.y"
                                   { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ) ;}
#line 3701 "yaccsrc.c"
    break;

  case 230: /* upper_stat: upper anyvars  */
#line 812 "yaccsrc.y"
                                        { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3708 "yaccsrc.c"
    break;

  case 231: /* exit_stat: exit nexpr  */
#line 816 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3715 "yaccsrc.c"
    break;

  case 232: /* if_stat: if expr nlncl THEN nlncl ystatement  */
#line 821 "yaccsrc.y"
                                       { move_labels( yyvsp[-5], yyvsp[0], level - 1 );
                                         yyval = yyvsp[-5];
                                         yyval->p[0] = optgluelast( yyvsp[-4], yyvsp[-3] );
                                         yyval->p[0]->o.last = NULL;
                                         yyval->p[1] = optgluelast( yyvsp[-1], yyvsp[0] );
                                         yyval->p[1]->o.last = NULL;
                                         level--; }
#line 3727 "yaccsrc.c"
    break;

  case 233: /* if_stat: if expr nlncl THEN nlncl ystatement ELSE nlncl ystatement  */
#line 829 "yaccsrc.y"
                                       { move_labels( yyvsp[-8], yyvsp[0], level - 1 );
                                         yyval = yyvsp[-8];
                                         yyval->p[0] = optgluelast( yyvsp[-7], yyvsp[-6] );
                                         yyval->p[0]->o.last = NULL;
                                         yyval->p[1] = optgluelast( yyvsp[-4], yyvsp[-3] );
                                         yyval->p[1]->o.last = NULL;
                                         yyval->p[2] = optgluelast( yyvsp[-1], yyvsp[0] );
                                         yyval->p[2]->o.last = NULL;
                                         level--; }
#line 3741 "yaccsrc.c"
    break;

  case 234: /* if_stat: if expr nlncl THEN nlncl ystatement ELSE nlncl error  */
#line 839 "yaccsrc.y"
                                       {  exiterror( ERR_INCOMPLETE_STRUCT, 4 ) ;}
#line 3747 "yaccsrc.c"
    break;

  case 235: /* if_stat: if expr nlncl THEN nlncl error  */
#line 841 "yaccsrc.y"
                                       {  exiterror( ERR_INCOMPLETE_STRUCT, 3 ) ;}
#line 3753 "yaccsrc.c"
    break;

  case 236: /* if_stat: if ncl  */
#line 842 "yaccsrc.y"
                                       {  exiterror( ERR_INCOMPLETE_STRUCT, 0 ) ;}
#line 3759 "yaccsrc.c"
    break;

  case 237: /* if_stat: if expr nlncl error  */
#line 843 "yaccsrc.y"
                                       {  exiterror( ERR_THEN_EXPECTED, 1, parser_data.if_linenr, __reginatext )  ; }
#line 3765 "yaccsrc.c"
    break;

  case 238: /* unexp_then: gruff THEN  */
#line 846 "yaccsrc.y"
                                       {  exiterror( ERR_THEN_UNEXPECTED, 1 )  ; }
#line 3771 "yaccsrc.c"
    break;

  case 239: /* unexp_else: gruff ELSE  */
#line 849 "yaccsrc.y"
                                       {  exiterror( ERR_THEN_UNEXPECTED, 2 )  ; }
#line 3777 "yaccsrc.c"
    break;

  case 240: /* ipret_stat: interpret expr  */
#line 852 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3784 "yaccsrc.c"
    break;

  case 241: /* iterate_stat: iterate simsymb  */
#line 857 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 3791 "yaccsrc.c"
    break;

  case 242: /* iterate_stat: iterate simsymb error  */
#line 859 "yaccsrc.y"
                                     { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 3797 "yaccsrc.c"
    break;

  case 243: /* iterate_stat: iterate  */
#line 860 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3803 "yaccsrc.c"
    break;

  case 244: /* label_stat: labelname  */
#line 863 "yaccsrc.y"
                                       { yyval = yyvsp[0] ;
                                         yyval->u.trace_only =
                                                         (level == 0) ? 0 : 1;
                                         newlabel( (const tsd_t *)parser_data.TSD,
                                                   &parser_data,
                                                   yyvsp[0] ) ; }
#line 3814 "yaccsrc.c"
    break;

  case 245: /* labelname: label  */
#line 871 "yaccsrc.y"
                                       { yyval = yyvsp[0] ;
                                         yyval->name = Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 3821 "yaccsrc.c"
    break;

  case 246: /* leave_stat: leave simsymb  */
#line 875 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 3828 "yaccsrc.c"
    break;

  case 247: /* leave_stat: leave simsymb error  */
#line 877 "yaccsrc.y"
                                   { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 3834 "yaccsrc.c"
    break;

  case 248: /* leave_stat: leave  */
#line 878 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3840 "yaccsrc.c"
    break;

  case 249: /* nop_stat: nop  */
#line 881 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3846 "yaccsrc.c"
    break;

  case 250: /* nop_stat: nop error  */
#line 882 "yaccsrc.y"
                         {  exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 3852 "yaccsrc.c"
    break;

  case 251: /* numeric_stat: numeric DIGITS expr  */
#line 885 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_NUM_D ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3860 "yaccsrc.c"
    break;

  case 252: /* numeric_stat: numeric DIGITS  */
#line 888 "yaccsrc.y"
                                       { yyval = yyvsp[-1]; yyval->type = X_NUM_DDEF ; }
#line 3866 "yaccsrc.c"
    break;

  case 253: /* numeric_stat: numeric FORM form_expr error  */
#line 889 "yaccsrc.y"
                                            { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 3872 "yaccsrc.c"
    break;

  case 254: /* numeric_stat: numeric FORM form_expr  */
#line 890 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_NUM_F ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3880 "yaccsrc.c"
    break;

  case 255: /* numeric_stat: numeric FORM  */
#line 893 "yaccsrc.y"
                                       { /* NOTE! This clashes ANSI! */
                                         yyval = yyvsp[-1] ; yyval->type=X_NUM_FRMDEF ;}
#line 3887 "yaccsrc.c"
    break;

  case 256: /* numeric_stat: numeric FORM VALUE expr  */
#line 895 "yaccsrc.y"
                                       { yyval = yyvsp[-3] ; yyval->type=X_NUM_V ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3894 "yaccsrc.c"
    break;

  case 257: /* numeric_stat: numeric FORM error  */
#line 897 "yaccsrc.y"
                                  { exiterror( ERR_INV_SUBKEYWORD, 11, "ENGINEERING SCIENTIFIC", __reginatext ) ;}
#line 3900 "yaccsrc.c"
    break;

  case 258: /* numeric_stat: numeric FUZZ  */
#line 898 "yaccsrc.y"
                                       { yyval = yyvsp[-1]; yyval->type = X_NUM_FDEF ;}
#line 3906 "yaccsrc.c"
    break;

  case 259: /* numeric_stat: numeric FUZZ expr  */
#line 899 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_NUM_FUZZ ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3914 "yaccsrc.c"
    break;

  case 260: /* numeric_stat: numeric error  */
#line 902 "yaccsrc.y"
                             { exiterror( ERR_INV_SUBKEYWORD, 15, "DIGITS FORM FUZZ", __reginatext ) ;}
#line 3920 "yaccsrc.c"
    break;

  case 261: /* form_expr: SCIENTIFIC  */
#line 905 "yaccsrc.y"
                                       { yyval = makenode(X_NUM_SCI,0) ; }
#line 3926 "yaccsrc.c"
    break;

  case 262: /* form_expr: ENGINEERING  */
#line 906 "yaccsrc.y"
                                       { yyval = makenode(X_NUM_ENG,0) ; }
#line 3932 "yaccsrc.c"
    break;

  case 263: /* options_stat: options nexpr  */
#line 909 "yaccsrc.y"
                                       { (yyval=yyvsp[-1])->p[0]=yyvsp[0] ; }
#line 3938 "yaccsrc.c"
    break;

  case 264: /* parse_stat: parse parse_flags parse_param templs  */
#line 913 "yaccsrc.y"
                                       { yyval = yyvsp[-3] ;
                                         yyval->type = X_PARSE ;
                                         yyval->u.parseflags = (int) yyvsp[-2] ;
                                         yyval->p[0] = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ; }
#line 3948 "yaccsrc.c"
    break;

  case 265: /* parse_stat: parse parse_param templs  */
#line 919 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_PARSE ;
                                         yyval->u.parseflags = 0;
                                         yyval->p[0] = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ; }
#line 3958 "yaccsrc.c"
    break;

  case 266: /* parse_stat: parse parse_flags error  */
#line 924 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 12, "ARG EXTERNAL LINEIN PULL SOURCE VAR VALUE VERSION", __reginatext ) ;}
#line 3964 "yaccsrc.c"
    break;

  case 267: /* parse_stat: parse error  */
#line 925 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 12, "ARG CASELESS EXTERNAL LINEIN LOWER PULL SOURCE UPPER VAR VALUE VERSION", __reginatext ) ;}
#line 3970 "yaccsrc.c"
    break;

  case 268: /* parse_flags: UPPER  */
#line 928 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_UPPER  |
                                                         PARSE_NORMAL); }
#line 3977 "yaccsrc.c"
    break;

  case 269: /* parse_flags: UPPER CASELESS  */
#line 930 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_UPPER  |
                                                         PARSE_CASELESS); }
#line 3984 "yaccsrc.c"
    break;

  case 270: /* parse_flags: CASELESS UPPER  */
#line 932 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_UPPER  |
                                                         PARSE_CASELESS); }
#line 3991 "yaccsrc.c"
    break;

  case 271: /* parse_flags: LOWER  */
#line 934 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_LOWER  |
                                                         PARSE_NORMAL); }
#line 3998 "yaccsrc.c"
    break;

  case 272: /* parse_flags: LOWER CASELESS  */
#line 936 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_LOWER  |
                                                         PARSE_CASELESS); }
#line 4005 "yaccsrc.c"
    break;

  case 273: /* parse_flags: CASELESS LOWER  */
#line 938 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_LOWER  |
                                                         PARSE_CASELESS); }
#line 4012 "yaccsrc.c"
    break;

  case 274: /* parse_flags: CASELESS  */
#line 940 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_NORMAL |
                                                         PARSE_CASELESS); }
#line 4019 "yaccsrc.c"
    break;

  case 275: /* templs: templs ',' template  */
#line 944 "yaccsrc.y"
                                       { /* fixes bugs like bug 579711 */
                                         yyval = optgluelast( yyvsp[-2], yyvsp[0] ); }
#line 4026 "yaccsrc.c"
    break;

  case 276: /* templs: template  */
#line 946 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 4032 "yaccsrc.c"
    break;

  case 277: /* parse_param: ARG  */
#line 949 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_ARG,0) ; }
#line 4038 "yaccsrc.c"
    break;

  case 278: /* parse_param: LINEIN  */
#line 950 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_EXT,0) ; }
#line 4044 "yaccsrc.c"
    break;

  case 279: /* parse_param: EXTERNAL  */
#line 951 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_EXT,0) ; }
#line 4050 "yaccsrc.c"
    break;

  case 280: /* parse_param: VERSION  */
#line 952 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_VER,0) ; }
#line 4056 "yaccsrc.c"
    break;

  case 281: /* parse_param: PULL  */
#line 953 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_PULL,0) ; }
#line 4062 "yaccsrc.c"
    break;

  case 282: /* parse_param: SOURCE  */
#line 954 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_SRC,0) ; }
#line 4068 "yaccsrc.c"
    break;

  case 283: /* parse_param: VAR simsymb  */
#line 955 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_VAR,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 4075 "yaccsrc.c"
    break;

  case 284: /* parse_param: VALUE nexpr WITH  */
#line 957 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_VAL,1,yyvsp[-1]) ; }
#line 4081 "yaccsrc.c"
    break;

  case 285: /* parse_param: VALUE error  */
#line 958 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_TEMPLATE, 3 ) ;}
#line 4087 "yaccsrc.c"
    break;

  case 286: /* proc_stat: proc  */
#line 961 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 4093 "yaccsrc.c"
    break;

  case 287: /* proc_stat: proc error  */
#line 962 "yaccsrc.y"
                          { exiterror( ERR_INV_SUBKEYWORD, 17, __reginatext ) ;}
#line 4099 "yaccsrc.c"
    break;

  case 288: /* proc_stat: proc EXPOSE error  */
#line 963 "yaccsrc.y"
                                 { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ) ;}
#line 4105 "yaccsrc.c"
    break;

  case 289: /* proc_stat: proc EXPOSE anyvars error  */
#line 964 "yaccsrc.y"
                                         { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ) ;}
#line 4111 "yaccsrc.c"
    break;

  case 290: /* proc_stat: proc EXPOSE anyvars  */
#line 965 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4118 "yaccsrc.c"
    break;

  case 291: /* pull_stat: pull template  */
#line 969 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4125 "yaccsrc.c"
    break;

  case 292: /* push_stat: push nexpr  */
#line 973 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4132 "yaccsrc.c"
    break;

  case 293: /* queue_stat: queue nexpr  */
#line 977 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4139 "yaccsrc.c"
    break;

  case 294: /* say_stat: say nexpr  */
#line 981 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4146 "yaccsrc.c"
    break;

  case 295: /* return_stat: return nexpr  */
#line 985 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4153 "yaccsrc.c"
    break;

  case 296: /* sel_end: END simsymb  */
#line 989 "yaccsrc.y"
                                       { exiterror( ERR_UNMATCHED_END, 0 ) ;}
#line 4159 "yaccsrc.c"
    break;

  case 297: /* sel_end: END simsymb error  */
#line 990 "yaccsrc.y"
                                       { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 4165 "yaccsrc.c"
    break;

  case 298: /* sel_end: END  */
#line 991 "yaccsrc.y"
                                       { level--; }
#line 4171 "yaccsrc.c"
    break;

  case 299: /* select_stat: select ncl when_stats otherwise_stat sel_end  */
#line 995 "yaccsrc.y"
                                       { yyval = yyvsp[-4];
                                         yyval->p[0] = optgluelast( yyvsp[-3], yyvsp[-2] );
                                         yyval->p[0]->o.last = NULL;
                                         yyval->p[1] = yyvsp[-1]; }
#line 4180 "yaccsrc.c"
    break;

  case 300: /* select_stat: select ncl END error  */
#line 999 "yaccsrc.y"
                                       {  exiterror( ERR_WHEN_EXPECTED, 1, parser_data.select_linenr, __reginatext ) ;}
#line 4186 "yaccsrc.c"
    break;

  case 301: /* select_stat: select ncl otherwise error  */
#line 1001 "yaccsrc.y"
                                       {  exiterror( ERR_WHEN_EXPECTED, 2, parser_data.select_linenr, __reginatext ) ;}
#line 4192 "yaccsrc.c"
    break;

  case 302: /* select_stat: select error  */
#line 1002 "yaccsrc.y"
                                       {  exiterror( ERR_EXTRA_DATA, 1, __reginatext )  ;}
#line 4198 "yaccsrc.c"
    break;

  case 303: /* select_stat: select ncl THEN  */
#line 1003 "yaccsrc.y"
                                       {  exiterror( ERR_THEN_UNEXPECTED, 0 ) ;}
#line 4204 "yaccsrc.c"
    break;

  case 304: /* select_stat: select ncl when_stats otherwise error  */
#line 1005 "yaccsrc.y"
                                       {  exiterror( ERR_INCOMPLETE_STRUCT, 0 ) ;}
#line 4210 "yaccsrc.c"
    break;

  case 305: /* when_stats: when_stats when_stat  */
#line 1008 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 4216 "yaccsrc.c"
    break;

  case 306: /* when_stats: when_stat  */
#line 1009 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 4222 "yaccsrc.c"
    break;

  case 307: /* when_stats: error  */
#line 1010 "yaccsrc.y"
                                       {  exiterror( ERR_WHEN_EXPECTED, 2, parser_data.select_linenr, __reginatext )  ;}
#line 4228 "yaccsrc.c"
    break;

  case 308: /* when_stat: when expr nlncl THEN nlncl statement  */
#line 1014 "yaccsrc.y"
                                       { yyval = yyvsp[-5]; /* fixes bugs like bug 579711 */
                                         yyval->p[0] = optgluelast( yyvsp[-4], yyvsp[-3] );
                                         yyval->p[0]->o.last = NULL;
                                         yyval->p[1] = optgluelast( yyvsp[-1], yyvsp[0] );
                                         yyval->p[1]->o.last = NULL; }
#line 4238 "yaccsrc.c"
    break;

  case 309: /* when_stat: when expr nlncl THEN nlncl statement THEN  */
#line 1020 "yaccsrc.y"
                                       {  exiterror( ERR_THEN_UNEXPECTED, 0 ) ;}
#line 4244 "yaccsrc.c"
    break;

  case 310: /* when_stat: when expr  */
#line 1021 "yaccsrc.y"
                                       {  exiterror( ERR_THEN_EXPECTED, 2, parser_data.select_linenr, __reginatext )  ; }
#line 4250 "yaccsrc.c"
    break;

  case 311: /* when_stat: when error  */
#line 1022 "yaccsrc.y"
                                       {  exiterror( ERR_INVALID_EXPRESSION, 0 ) ;}
#line 4256 "yaccsrc.c"
    break;

  case 312: /* ex_when_stat: gruff when  */
#line 1032 "yaccsrc.y"
                                       { exiterror( ERR_WHEN_UNEXPECTED, 1 ); }
#line 4262 "yaccsrc.c"
    break;

  case 313: /* ex_when_stat: gruff otherwise  */
#line 1033 "yaccsrc.y"
                                       { exiterror( ERR_WHEN_UNEXPECTED, 2 ); }
#line 4268 "yaccsrc.c"
    break;

  case 314: /* otherwise_stat: otherwise nlncl nxstats  */
#line 1036 "yaccsrc.y"
                                         { yyval = yyvsp[-2];
                                         yyval->p[0] = optgluelast( yyvsp[-1], yyvsp[0] );
                                         if ( yyval->p[0] )
                                            yyval->p[0]->o.last = NULL; }
#line 4277 "yaccsrc.c"
    break;

  case 315: /* otherwise_stat: %empty  */
#line 1040 "yaccsrc.y"
                                       { yyval = makenode(X_NO_OTHERWISE,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 4285 "yaccsrc.c"
    break;

  case 316: /* signal_stat: signal VALUE expr  */
#line 1046 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_SIG_VAL ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4293 "yaccsrc.c"
    break;

  case 317: /* signal_stat: signal signal_name error  */
#line 1049 "yaccsrc.y"
                                       { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 4299 "yaccsrc.c"
    break;

  case 318: /* signal_stat: signal signal_name  */
#line 1050 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->name = (streng *)yyvsp[0] ; }
#line 4306 "yaccsrc.c"
    break;

  case 319: /* signal_stat: signal on error  */
#line 1052 "yaccsrc.y"
                              { exiterror( ERR_INV_SUBKEYWORD, 3, "ERROR FAILURE HALT NOTREADY NOVALUE SYNTAX LOSTDIGITS", __reginatext ) ;}
#line 4312 "yaccsrc.c"
    break;

  case 320: /* signal_stat: signal off error  */
#line 1053 "yaccsrc.y"
                               { exiterror( ERR_INV_SUBKEYWORD, 4, "ERROR FAILURE HALT NOTREADY NOVALUE SYNTAX LOSTDIGITS", __reginatext ) ;}
#line 4318 "yaccsrc.c"
    break;

  case 321: /* signal_stat: signal on s_action error  */
#line 1054 "yaccsrc.y"
                                       { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 4324 "yaccsrc.c"
    break;

  case 322: /* signal_stat: signal on s_action namespec error  */
#line 1055 "yaccsrc.y"
                                                { exiterror( ERR_STRING_EXPECTED, 3, __reginatext ) ;}
#line 4330 "yaccsrc.c"
    break;

  case 323: /* signal_stat: signal on s_action namespec  */
#line 1057 "yaccsrc.y"
                                       { yyval = yyvsp[-3] ;
                                         yyval->type = X_SIG_SET ;
                                         yyval->p[0] = yyvsp[-2] ;
                                         yyval->name = (streng *)yyvsp[0] ;
                                         yyval->p[1] = yyvsp[-1] ; }
#line 4340 "yaccsrc.c"
    break;

  case 324: /* signal_stat: signal on s_action  */
#line 1062 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_SIG_SET ;
                                         yyval->p[0] = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ; }
#line 4349 "yaccsrc.c"
    break;

  case 325: /* signal_stat: signal off s_action error  */
#line 1066 "yaccsrc.y"
                                        { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 4355 "yaccsrc.c"
    break;

  case 326: /* signal_stat: signal off s_action  */
#line 1067 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_SIG_SET ;
                                         yyval->p[0] = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ; }
#line 4364 "yaccsrc.c"
    break;

  case 327: /* signal_name: asymbol  */
#line 1073 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 4370 "yaccsrc.c"
    break;

  case 328: /* signal_name: STRING  */
#line 1074 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 4376 "yaccsrc.c"
    break;

  case 329: /* signal_name: error  */
#line 1075 "yaccsrc.y"
                                       { exiterror( ERR_STRING_EXPECTED, 4, __reginatext );}
#line 4382 "yaccsrc.c"
    break;

  case 330: /* namespec: NAME SIMSYMBOL  */
#line 1078 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue);}
#line 4388 "yaccsrc.c"
    break;

  case 331: /* namespec: NAME STRING  */
#line 1079 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 4394 "yaccsrc.c"
    break;

  case 332: /* namespec: NAME error  */
#line 1080 "yaccsrc.y"
                                       { exiterror( ERR_STRING_EXPECTED, 3, __reginatext ) ;}
#line 4400 "yaccsrc.c"
    break;

  case 333: /* asymbol: CONSYMBOL  */
#line 1083 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 4406 "yaccsrc.c"
    break;

  case 334: /* asymbol: SIMSYMBOL  */
#line 1084 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 4412 "yaccsrc.c"
    break;

  case 335: /* on: ON  */
#line 1087 "yaccsrc.y"
                                       { yyval = makenode(X_ON,0) ; }
#line 4418 "yaccsrc.c"
    break;

  case 336: /* off: OFF  */
#line 1090 "yaccsrc.y"
                                       { yyval = makenode(X_OFF,0) ; }
#line 4424 "yaccsrc.c"
    break;

  case 337: /* c_action: ERROR  */
#line 1093 "yaccsrc.y"
                                       { yyval = makenode(X_S_ERROR,0) ; }
#line 4430 "yaccsrc.c"
    break;

  case 338: /* c_action: HALT  */
#line 1094 "yaccsrc.y"
                                       { yyval = makenode(X_S_HALT,0) ; }
#line 4436 "yaccsrc.c"
    break;

  case 339: /* c_action: NOTREADY  */
#line 1095 "yaccsrc.y"
                                       { yyval = makenode(X_S_NOTREADY,0) ; }
#line 4442 "yaccsrc.c"
    break;

  case 340: /* c_action: FAILURE  */
#line 1096 "yaccsrc.y"
                                       { yyval = makenode(X_S_FAILURE,0) ; }
#line 4448 "yaccsrc.c"
    break;

  case 341: /* s_action: c_action  */
#line 1099 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 4454 "yaccsrc.c"
    break;

  case 342: /* s_action: NOVALUE  */
#line 1100 "yaccsrc.y"
                                       { yyval = makenode(X_S_NOVALUE,0) ; }
#line 4460 "yaccsrc.c"
    break;

  case 343: /* s_action: SYNTAX  */
#line 1101 "yaccsrc.y"
                                       { yyval = makenode(X_S_SYNTAX,0) ; }
#line 4466 "yaccsrc.c"
    break;

  case 344: /* s_action: LOSTDIGITS  */
#line 1102 "yaccsrc.y"
                                       { yyval = makenode(X_S_LOSTDIGITS,0) ; }
#line 4472 "yaccsrc.c"
    break;

  case 345: /* trace_stat: trace VALUE expr  */
#line 1105 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4479 "yaccsrc.c"
    break;

  case 346: /* trace_stat: trace nexpr  */
#line 1107 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4486 "yaccsrc.c"
    break;

  case 347: /* trace_stat: trace whatever error  */
#line 1109 "yaccsrc.y"
                                   { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 4492 "yaccsrc.c"
    break;

  case 348: /* trace_stat: trace whatever  */
#line 1110 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 4499 "yaccsrc.c"
    break;

  case 349: /* whatever: WHATEVER  */
#line 1114 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 4505 "yaccsrc.c"
    break;

  case 350: /* assignment: ass_part nexpr  */
#line 1118 "yaccsrc.y"
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
#line 4524 "yaccsrc.c"
    break;

  case 351: /* ass_part: ASSIGNMENTVARIABLE  */
#line 1134 "yaccsrc.y"
                                       { yyval = makenode(X_ASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4533 "yaccsrc.c"
    break;

  case 352: /* plusassignment: plus_ass_part nexpr  */
#line 1140 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "+=" );
                                       }
#line 4543 "yaccsrc.c"
    break;

  case 353: /* plus_ass_part: PLUSASSIGNMENTVARIABLE  */
#line 1147 "yaccsrc.y"
                                       { yyval = makenode(X_PLUSASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4552 "yaccsrc.c"
    break;

  case 354: /* minusassignment: minus_ass_part nexpr  */
#line 1153 "yaccsrc.y"
                                         { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "-=" );
                                       }
#line 4562 "yaccsrc.c"
    break;

  case 355: /* minus_ass_part: MINUSASSIGNMENTVARIABLE  */
#line 1160 "yaccsrc.y"
                                         { yyval = makenode(X_MINUSASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4571 "yaccsrc.c"
    break;

  case 356: /* multassignment: mult_ass_part nexpr  */
#line 1166 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "*=" );
                                       }
#line 4581 "yaccsrc.c"
    break;

  case 357: /* mult_ass_part: MULTASSIGNMENTVARIABLE  */
#line 1173 "yaccsrc.y"
                                       { yyval = makenode(X_MULTASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4590 "yaccsrc.c"
    break;

  case 358: /* divassignment: div_ass_part nexpr  */
#line 1179 "yaccsrc.y"
                                     { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "/=" );
                                       }
#line 4600 "yaccsrc.c"
    break;

  case 359: /* div_ass_part: DIVASSIGNMENTVARIABLE  */
#line 1186 "yaccsrc.y"
                                     { yyval = makenode(X_DIVASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4609 "yaccsrc.c"
    break;

  case 360: /* intdivassignment: intdiv_ass_part nexpr  */
#line 1192 "yaccsrc.y"
                                           { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "%=" );
                                       }
#line 4619 "yaccsrc.c"
    break;

  case 361: /* intdiv_ass_part: INTDIVASSIGNMENTVARIABLE  */
#line 1199 "yaccsrc.y"
                                           { yyval = makenode(X_INTDIVASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4628 "yaccsrc.c"
    break;

  case 362: /* modulusassignment: modulus_ass_part nexpr  */
#line 1205 "yaccsrc.y"
                                             { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "//=" );
                                       }
#line 4638 "yaccsrc.c"
    break;

  case 363: /* modulus_ass_part: MODULUSASSIGNMENTVARIABLE  */
#line 1212 "yaccsrc.y"
                                             { yyval = makenode(X_MODULUSASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4647 "yaccsrc.c"
    break;

  case 364: /* andassignment: and_ass_part nexpr  */
#line 1218 "yaccsrc.y"
                                     { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "&=" );
                                       }
#line 4657 "yaccsrc.c"
    break;

  case 365: /* and_ass_part: ANDASSIGNMENTVARIABLE  */
#line 1225 "yaccsrc.y"
                                     { yyval = makenode(X_ANDASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4666 "yaccsrc.c"
    break;

  case 366: /* xorassignment: xor_ass_part nexpr  */
#line 1231 "yaccsrc.y"
                                     { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "&&=" );
                                       }
#line 4676 "yaccsrc.c"
    break;

  case 367: /* xor_ass_part: XORASSIGNMENTVARIABLE  */
#line 1238 "yaccsrc.y"
                                     { yyval = makenode(X_XORASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4685 "yaccsrc.c"
    break;

  case 368: /* orassignment: or_ass_part nexpr  */
#line 1244 "yaccsrc.y"
                                   { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "|=" );
                                       }
#line 4695 "yaccsrc.c"
    break;

  case 369: /* or_ass_part: ORASSIGNMENTVARIABLE  */
#line 1251 "yaccsrc.y"
                                   { yyval = makenode(X_ORASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4704 "yaccsrc.c"
    break;

  case 370: /* concatassignment: concat_ass_part nexpr  */
#line 1257 "yaccsrc.y"
                                           { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                       }
#line 4712 "yaccsrc.c"
    break;

  case 371: /* concat_ass_part: CONCATASSIGNMENTVARIABLE  */
#line 1262 "yaccsrc.y"
                                           { yyval = makenode(X_CONCATASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4721 "yaccsrc.c"
    break;

  case 372: /* $@10: %empty  */
#line 1269 "yaccsrc.y"
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
#line 4742 "yaccsrc.c"
    break;

  case 373: /* expr: '(' $@10 exprs_sub  */
#line 1285 "yaccsrc.y"
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
#line 4764 "yaccsrc.c"
    break;

  case 374: /* $@11: %empty  */
#line 1302 "yaccsrc.y"
                                       { parendepth--; }
#line 4770 "yaccsrc.c"
    break;

  case 375: /* expr: expr '+' $@11 expr  */
#line 1303 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "+" );
                                         yyval = makenode( X_PLUSS, 2, yyvsp[-3], yyvsp[0] ); }
#line 4777 "yaccsrc.c"
    break;

  case 376: /* $@12: %empty  */
#line 1305 "yaccsrc.y"
                                       { parendepth--; }
#line 4783 "yaccsrc.c"
    break;

  case 377: /* expr: expr '-' $@12 expr  */
#line 1306 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "-" );
                                         yyval = makenode( X_MINUS, 2, yyvsp[-3], yyvsp[0] ); }
#line 4790 "yaccsrc.c"
    break;

  case 378: /* $@13: %empty  */
#line 1308 "yaccsrc.y"
                                       { parendepth--; }
#line 4796 "yaccsrc.c"
    break;

  case 379: /* expr: expr '*' $@13 expr  */
#line 1309 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "*" );
                                         yyval = makenode( X_MULT, 2, yyvsp[-3], yyvsp[0] ); }
#line 4803 "yaccsrc.c"
    break;

  case 380: /* expr: '*'  */
#line 1311 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "*" ); }
#line 4809 "yaccsrc.c"
    break;

  case 381: /* $@14: %empty  */
#line 1312 "yaccsrc.y"
                                       { parendepth--; }
#line 4815 "yaccsrc.c"
    break;

  case 382: /* expr: expr '/' $@14 expr  */
#line 1313 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "/" );
                                         yyval = makenode( X_DEVIDE, 2, yyvsp[-3], yyvsp[0] ); }
#line 4822 "yaccsrc.c"
    break;

  case 383: /* expr: '/'  */
#line 1315 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "/" ); }
#line 4828 "yaccsrc.c"
    break;

  case 384: /* $@15: %empty  */
#line 1316 "yaccsrc.y"
                                       { parendepth--; }
#line 4834 "yaccsrc.c"
    break;

  case 385: /* expr: expr MODULUS $@15 expr  */
#line 1317 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "//" );
                                         yyval = makenode( X_MODULUS, 2, yyvsp[-3], yyvsp[0] ); }
#line 4841 "yaccsrc.c"
    break;

  case 386: /* expr: MODULUS  */
#line 1319 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "//" ); }
#line 4847 "yaccsrc.c"
    break;

  case 387: /* $@16: %empty  */
#line 1320 "yaccsrc.y"
                                       { parendepth--; }
#line 4853 "yaccsrc.c"
    break;

  case 388: /* expr: expr '%' $@16 expr  */
#line 1321 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "%" );
                                         yyval = makenode( X_INTDIV, 2, yyvsp[-3], yyvsp[0] ); }
#line 4860 "yaccsrc.c"
    break;

  case 389: /* expr: '%'  */
#line 1323 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "%" ); }
#line 4866 "yaccsrc.c"
    break;

  case 390: /* $@17: %empty  */
#line 1324 "yaccsrc.y"
                                       { parendepth--; }
#line 4872 "yaccsrc.c"
    break;

  case 391: /* expr: expr '|' $@17 expr  */
#line 1325 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "|" );
                                         yyval = makenode( X_LOG_OR, 2, yyvsp[-3], yyvsp[0] ); }
#line 4879 "yaccsrc.c"
    break;

  case 392: /* expr: '|'  */
#line 1327 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "|" ); }
#line 4885 "yaccsrc.c"
    break;

  case 393: /* $@18: %empty  */
#line 1328 "yaccsrc.y"
                                       { parendepth--; }
#line 4891 "yaccsrc.c"
    break;

  case 394: /* expr: expr '&' $@18 expr  */
#line 1329 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "&" );
                                         yyval = makenode( X_LOG_AND, 2, yyvsp[-3], yyvsp[0] ); }
#line 4898 "yaccsrc.c"
    break;

  case 395: /* expr: '&'  */
#line 1331 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "&" ); }
#line 4904 "yaccsrc.c"
    break;

  case 396: /* $@19: %empty  */
#line 1332 "yaccsrc.y"
                                       { parendepth--; }
#line 4910 "yaccsrc.c"
    break;

  case 397: /* expr: expr XOR $@19 expr  */
#line 1333 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "&&" );
                                         yyval = makenode( X_LOG_XOR, 2, yyvsp[-3], yyvsp[0] ); }
#line 4917 "yaccsrc.c"
    break;

  case 398: /* expr: XOR  */
#line 1335 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "&&" ); }
#line 4923 "yaccsrc.c"
    break;

  case 399: /* $@20: %empty  */
#line 1336 "yaccsrc.y"
                                       { parendepth--; }
#line 4929 "yaccsrc.c"
    break;

  case 400: /* expr: expr EXP $@20 expr  */
#line 1337 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "**" );
                                         yyval = makenode( X_EXP, 2, yyvsp[-3], yyvsp[0] ); }
#line 4936 "yaccsrc.c"
    break;

  case 401: /* expr: EXP  */
#line 1339 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "**" ); }
#line 4942 "yaccsrc.c"
    break;

  case 402: /* $@21: %empty  */
#line 1340 "yaccsrc.y"
                                       { parendepth--; }
#line 4948 "yaccsrc.c"
    break;

  case 403: /* expr: expr SPACE $@21 expr  */
#line 1341 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], " " );
                                         yyval = makenode( X_SPACE, 2, yyvsp[-3], yyvsp[0] ); }
#line 4955 "yaccsrc.c"
    break;

  case 404: /* expr: SPACE  */
#line 1343 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, " " ); }
#line 4961 "yaccsrc.c"
    break;

  case 405: /* $@22: %empty  */
#line 1344 "yaccsrc.y"
                                       { parendepth--; }
#line 4967 "yaccsrc.c"
    break;

  case 406: /* expr: expr CONCATENATE $@22 expr  */
#line 1345 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "||" );
                                         yyval = makenode( X_CONCAT, 2, yyvsp[-3], yyvsp[0] ); }
#line 4974 "yaccsrc.c"
    break;

  case 407: /* expr: CONCATENATE  */
#line 1347 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "||" ); }
#line 4980 "yaccsrc.c"
    break;

  case 408: /* expr: NOT expr  */
#line 1348 "yaccsrc.y"
                                       { AUTO_REDUCE( yyvsp[0], "\\" );
                                         yyval = makenode( X_LOG_NOT, 1, yyvsp[0] ); }
#line 4987 "yaccsrc.c"
    break;

  case 409: /* expr: NOT  */
#line 1350 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "\\" ); }
#line 4993 "yaccsrc.c"
    break;

  case 410: /* $@23: %empty  */
#line 1351 "yaccsrc.y"
                                       { parendepth--; }
#line 4999 "yaccsrc.c"
    break;

  case 411: /* expr: expr '=' $@23 expr  */
#line 1352 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "=" );
                                         yyval = makenode( X_EQUAL, 2, yyvsp[-3], yyvsp[0] );
                                         transform( yyval ); }
#line 5007 "yaccsrc.c"
    break;

  case 412: /* expr: '='  */
#line 1355 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "=" ); }
#line 5013 "yaccsrc.c"
    break;

  case 413: /* $@24: %empty  */
#line 1356 "yaccsrc.y"
                                       { parendepth--; }
#line 5019 "yaccsrc.c"
    break;

  case 414: /* expr: expr GTE $@24 expr  */
#line 1357 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], ">=" );
                                         yyval = makenode( X_GTE, 2, yyvsp[-3], yyvsp[0] );
                                         transform( yyval ) ; }
#line 5027 "yaccsrc.c"
    break;

  case 415: /* expr: GTE  */
#line 1360 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, ">=" ); }
#line 5033 "yaccsrc.c"
    break;

  case 416: /* $@25: %empty  */
#line 1361 "yaccsrc.y"
                                       { parendepth--; }
#line 5039 "yaccsrc.c"
    break;

  case 417: /* expr: expr LTE $@25 expr  */
#line 1362 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "<=" );
                                         yyval = makenode( X_LTE, 2, yyvsp[-3], yyvsp[0] );
                                         transform( yyval ) ; }
#line 5047 "yaccsrc.c"
    break;

  case 418: /* expr: LTE  */
#line 1365 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "<=" ); }
#line 5053 "yaccsrc.c"
    break;

  case 419: /* $@26: %empty  */
#line 1366 "yaccsrc.y"
                                       { parendepth--; }
#line 5059 "yaccsrc.c"
    break;

  case 420: /* expr: expr GT $@26 expr  */
#line 1367 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], ">" );
                                         yyval = makenode( X_GT, 2, yyvsp[-3], yyvsp[0] );
                                         transform( yyval ) ; }
#line 5067 "yaccsrc.c"
    break;

  case 421: /* expr: GT  */
#line 1370 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, ">" ); }
#line 5073 "yaccsrc.c"
    break;

  case 422: /* $@27: %empty  */
#line 1371 "yaccsrc.y"
                                       { parendepth--; }
#line 5079 "yaccsrc.c"
    break;

  case 423: /* expr: expr LT $@27 expr  */
#line 1372 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "<" );
                                         yyval = makenode( X_LT, 2, yyvsp[-3], yyvsp[0] );
                                         transform( yyval ) ; }
#line 5087 "yaccsrc.c"
    break;

  case 424: /* expr: LT  */
#line 1375 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "<" ); }
#line 5093 "yaccsrc.c"
    break;

  case 425: /* $@28: %empty  */
#line 1376 "yaccsrc.y"
                                       { parendepth--; }
#line 5099 "yaccsrc.c"
    break;

  case 426: /* expr: expr DIFFERENT $@28 expr  */
#line 1377 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "\\=" );
                                         yyval = makenode( X_DIFF, 2, yyvsp[-3], yyvsp[0] );
                                         transform( yyval ) ; }
#line 5107 "yaccsrc.c"
    break;

  case 427: /* expr: DIFFERENT  */
#line 1380 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "\\=" ); }
#line 5113 "yaccsrc.c"
    break;

  case 428: /* $@29: %empty  */
#line 1381 "yaccsrc.y"
                                       { parendepth--; }
#line 5119 "yaccsrc.c"
    break;

  case 429: /* expr: expr EQUALEQUAL $@29 expr  */
#line 1382 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "==" );
                                         yyval = makenode( X_S_EQUAL, 2, yyvsp[-3], yyvsp[0] ); }
#line 5126 "yaccsrc.c"
    break;

  case 430: /* expr: EQUALEQUAL  */
#line 1384 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "==" ); }
#line 5132 "yaccsrc.c"
    break;

  case 431: /* $@30: %empty  */
#line 1385 "yaccsrc.y"
                                       { parendepth--; }
#line 5138 "yaccsrc.c"
    break;

  case 432: /* expr: expr NOTEQUALEQUAL $@30 expr  */
#line 1386 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "\\==" );
                                         yyval = makenode( X_S_DIFF, 2, yyvsp[-3], yyvsp[0] ); }
#line 5145 "yaccsrc.c"
    break;

  case 433: /* expr: NOTEQUALEQUAL  */
#line 1388 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "\\==" ); }
#line 5151 "yaccsrc.c"
    break;

  case 434: /* $@31: %empty  */
#line 1389 "yaccsrc.y"
                                       { parendepth--; }
#line 5157 "yaccsrc.c"
    break;

  case 435: /* expr: expr GTGT $@31 expr  */
#line 1390 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], ">>" );
                                         yyval = makenode( X_S_GT, 2, yyvsp[-3], yyvsp[0] ); }
#line 5164 "yaccsrc.c"
    break;

  case 436: /* expr: GTGT  */
#line 1392 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, ">>" ); }
#line 5170 "yaccsrc.c"
    break;

  case 437: /* $@32: %empty  */
#line 1393 "yaccsrc.y"
                                       { parendepth--; }
#line 5176 "yaccsrc.c"
    break;

  case 438: /* expr: expr LTLT $@32 expr  */
#line 1394 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "<<" );
                                         yyval = makenode( X_S_LT, 2, yyvsp[-3], yyvsp[0] ); }
#line 5183 "yaccsrc.c"
    break;

  case 439: /* expr: LTLT  */
#line 1396 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "<<" ); }
#line 5189 "yaccsrc.c"
    break;

  case 440: /* $@33: %empty  */
#line 1397 "yaccsrc.y"
                                       { parendepth--; }
#line 5195 "yaccsrc.c"
    break;

  case 441: /* expr: expr NOTGTGT $@33 expr  */
#line 1398 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "\\>>" );
                                         yyval = makenode( X_S_NGT, 2, yyvsp[-3], yyvsp[0] ); }
#line 5202 "yaccsrc.c"
    break;

  case 442: /* expr: NOTGTGT  */
#line 1400 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "\\>>" ); }
#line 5208 "yaccsrc.c"
    break;

  case 443: /* $@34: %empty  */
#line 1401 "yaccsrc.y"
                                       { parendepth--; }
#line 5214 "yaccsrc.c"
    break;

  case 444: /* expr: expr NOTLTLT $@34 expr  */
#line 1402 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "\\<<" );
                                         yyval = makenode( X_S_NLT, 2, yyvsp[-3], yyvsp[0] ); }
#line 5221 "yaccsrc.c"
    break;

  case 445: /* expr: NOTLTLT  */
#line 1404 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "\\<<" ); }
#line 5227 "yaccsrc.c"
    break;

  case 446: /* $@35: %empty  */
#line 1405 "yaccsrc.y"
                                       { parendepth--; }
#line 5233 "yaccsrc.c"
    break;

  case 447: /* expr: expr GTGTE $@35 expr  */
#line 1406 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], ">>=" );
                                         yyval = makenode( X_S_GTE, 2, yyvsp[-3], yyvsp[0] ); }
#line 5240 "yaccsrc.c"
    break;

  case 448: /* expr: GTGTE  */
#line 1408 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, ">>=" ); }
#line 5246 "yaccsrc.c"
    break;

  case 449: /* $@36: %empty  */
#line 1409 "yaccsrc.y"
                                       { parendepth--; }
#line 5252 "yaccsrc.c"
    break;

  case 450: /* expr: expr LTLTE $@36 expr  */
#line 1410 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "<<=" );
                                         yyval = makenode( X_S_LTE, 2, yyvsp[-3], yyvsp[0] ); }
#line 5259 "yaccsrc.c"
    break;

  case 451: /* expr: LTLTE  */
#line 1412 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "<<=" ); }
#line 5265 "yaccsrc.c"
    break;

  case 452: /* expr: expr MSGSEND msgtarget  */
#line 1413 "yaccsrc.y"
                                       { AUTO_REDUCE( yyvsp[-2], __regina_oo_operator );
                                         yyval = yyvsp[0];
                                         yyval->type = X_MSGSEND;
                                         yyval->p[0] = yyvsp[-2]; }
#line 5274 "yaccsrc.c"
    break;

  case 453: /* expr: expr CASCADE msgtarget  */
#line 1417 "yaccsrc.y"
                                       { AUTO_REDUCE( yyvsp[-2], __regina_oo_operator );
                                         yyval = yyvsp[0];
                                         yyval->type = X_CASCADE;
                                         yyval->p[0] = yyvsp[-2]; }
#line 5283 "yaccsrc.c"
    break;

  case 454: /* expr: MSGSEND  */
#line 1421 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, __regina_oo_operator ); }
#line 5289 "yaccsrc.c"
    break;

  case 455: /* expr: CASCADE  */
#line 1422 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, __regina_oo_operator ); }
#line 5295 "yaccsrc.c"
    break;

  case 456: /* expr: expr '[' bracket_args  */
#line 1423 "yaccsrc.y"
                                       { AUTO_REDUCE( yyvsp[-2], "[" );
                                         yyval = makenode( X_INDEX, 2, yyvsp[-2], yyvsp[0] ); }
#line 5302 "yaccsrc.c"
    break;

  case 457: /* expr: symbtree  */
#line 1425 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 5308 "yaccsrc.c"
    break;

  case 458: /* expr: CONSYMBOL  */
#line 1426 "yaccsrc.y"
                                       { /* OO: .NAME may be an environment
                                          * symbol, see OO_IS_ENVSYMBOL */
                                         yyval = makenode( ( retvalue[0] == '.'
                                                          && rx_isalpha( retvalue[1] ) )
                                                        ? X_CON_SYMBOL : X_STRING, 0 );
                                         yyval->name = Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 5319 "yaccsrc.c"
    break;

  case 459: /* expr: HEXSTRING  */
#line 1432 "yaccsrc.y"
                                       { yyval = makenode( X_STRING, 0 );
                                         yyval->name = Str_make_TSD(parser_data.TSD,retlength) ;
                                         memcpy(yyval->name->value,retvalue,
                                                    yyval->name->len=retlength); }
#line 5328 "yaccsrc.c"
    break;

  case 460: /* expr: BINSTRING  */
#line 1436 "yaccsrc.y"
                                       { yyval = makenode( X_STRING, 0 );
                                         yyval->name = Str_make_TSD(parser_data.TSD,retlength) ;
                                         memcpy(yyval->name->value,retvalue,
                                                    yyval->name->len=retlength); }
#line 5337 "yaccsrc.c"
    break;

  case 461: /* expr: STRING  */
#line 1440 "yaccsrc.y"
                                       { yyval = makenode( X_STRING, 0 );
                                         yyval->name = Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 5344 "yaccsrc.c"
    break;

  case 462: /* expr: function  */
#line 1442 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 5350 "yaccsrc.c"
    break;

  case 463: /* expr: '+' expr  */
#line 1443 "yaccsrc.y"
                                       { AUTO_REDUCE( yyvsp[0], rxnullptr );
                                         yyval = makenode( X_U_PLUSS, 1, yyvsp[0] ); }
#line 5357 "yaccsrc.c"
    break;

  case 464: /* expr: '-' expr  */
#line 1445 "yaccsrc.y"
                                       { AUTO_REDUCE( yyvsp[0], rxnullptr );
                                         yyval = makenode( X_U_MINUS, 1, yyvsp[0] ); }
#line 5364 "yaccsrc.c"
    break;

  case 465: /* expr: '+' error  */
#line 1447 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, __reginatext ); }
#line 5370 "yaccsrc.c"
    break;

  case 466: /* expr: '-' error  */
#line 1448 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, __reginatext ); }
#line 5376 "yaccsrc.c"
    break;

  case 467: /* exprs_sub: exprs ')'  */
#line 1451 "yaccsrc.y"
                                       { yyval = yyvsp[-1]; }
#line 5382 "yaccsrc.c"
    break;

  case 468: /* exprs_sub: exprs error  */
#line 1452 "yaccsrc.y"
                                       { exiterror( ERR_UNMATCHED_PARAN, 0 ); }
#line 5388 "yaccsrc.c"
    break;

  case 469: /* exprs_sub: STATSEP  */
#line 1453 "yaccsrc.y"
                                       { exiterror( ERR_UNMATCHED_PARAN, 0 ); }
#line 5394 "yaccsrc.c"
    break;

  case 470: /* symbtree: SIMSYMBOL  */
#line 1456 "yaccsrc.y"
                                       { yyval = (nodeptr)create_head( (const char *)retvalue ) ; }
#line 5400 "yaccsrc.c"
    break;

  case 471: /* function: extfunc func_args  */
#line 1460 "yaccsrc.y"
                                       { yyval = makenode(X_EX_FUNC,1,yyvsp[0]) ;
                                         yyval->name = (streng *)yyvsp[-1] ; }
#line 5407 "yaccsrc.c"
    break;

  case 472: /* function: intfunc func_args  */
#line 1462 "yaccsrc.y"
                                       { yyval = makenode(X_IN_FUNC,1,yyvsp[0]) ;
                                         yyval->name = (streng *)yyvsp[-1] ; }
#line 5414 "yaccsrc.c"
    break;

  case 473: /* @37: %empty  */
#line 1466 "yaccsrc.y"
                                       { /* ugly fake preservs parendepth */
                                         yyval = (YYSTYPE) parendepth;
                                         parendepth = 0; }
#line 5422 "yaccsrc.c"
    break;

  case 474: /* func_args: @37 exprs_sub  */
#line 1469 "yaccsrc.y"
                                       { parendepth = (int) yyval;
                                         yyval = yyvsp[0]; }
#line 5429 "yaccsrc.c"
    break;

  case 475: /* msgtarget: msgname  */
#line 1478 "yaccsrc.y"
                                       { yyval = makenode( X_MSGSEND, 0 );
                                         yyval->name = (streng *) yyvsp[0]; }
#line 5436 "yaccsrc.c"
    break;

  case 476: /* msgtarget: msgname MSGSUPER  */
#line 1480 "yaccsrc.y"
                                       { /* ~name:super, see oo.c */
                                         yyval = makenode( X_MSGSEND, 0 );
                                         yyval->name = (streng *) yyvsp[-1];
                                         yyval->u.nonansi = 1; }
#line 5445 "yaccsrc.c"
    break;

  case 477: /* msgtarget: msgname MSGSUPER '(' func_args  */
#line 1485 "yaccsrc.y"
                                       { yyval = makenode( X_MSGSEND, 2, NULL, yyvsp[0] );
                                         yyval->name = (streng *) yyvsp[-3];
                                         yyval->u.nonansi = 1; }
#line 5453 "yaccsrc.c"
    break;

  case 478: /* msgtarget: STRING  */
#line 1488 "yaccsrc.y"
                                       { yyval = makenode( X_MSGSEND, 0 );
                                         yyval->name = Str_cre_TSD( parser_data.TSD, retvalue );
                                         mem_upper( yyval->name->value, yyval->name->len ); }
#line 5461 "yaccsrc.c"
    break;

  case 479: /* msgtarget: intfunc func_args  */
#line 1491 "yaccsrc.y"
                                       { yyval = makenode( X_MSGSEND, 2, NULL, yyvsp[0] );
                                         yyval->name = (streng *) yyvsp[-1];
                                         mem_upper( yyval->name->value, yyval->name->len ); }
#line 5469 "yaccsrc.c"
    break;

  case 480: /* msgtarget: extfunc func_args  */
#line 1494 "yaccsrc.y"
                                       { yyval = makenode( X_MSGSEND, 2, NULL, yyvsp[0] );
                                         yyval->name = (streng *) yyvsp[-1];
                                         mem_upper( yyval->name->value, yyval->name->len ); }
#line 5477 "yaccsrc.c"
    break;

  case 481: /* msgname: SIMSYMBOL  */
#line 1499 "yaccsrc.y"
                                       { yyval = (nodeptr) Str_cre_TSD( parser_data.TSD, retvalue ); }
#line 5483 "yaccsrc.c"
    break;

  case 482: /* @38: %empty  */
#line 1502 "yaccsrc.y"
                                       { /* like func_args, with "]" */
                                         yyval = (YYSTYPE) parendepth;
                                         parendepth = 0; }
#line 5491 "yaccsrc.c"
    break;

  case 483: /* bracket_args: @38 exprs ']'  */
#line 1505 "yaccsrc.y"
                                       { parendepth = (int) yyval;
                                         yyval = yyvsp[-1]; }
#line 5498 "yaccsrc.c"
    break;

  case 484: /* intfunc: INFUNCNAME  */
#line 1509 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 5504 "yaccsrc.c"
    break;

  case 485: /* extfunc: EXFUNCNAME  */
#line 1512 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 5510 "yaccsrc.c"
    break;

  case 486: /* template: pv solid template  */
#line 1515 "yaccsrc.y"
                                       { yyval =makenode(X_TPL_SOLID,3,yyvsp[-2],yyvsp[-1],yyvsp[0]);}
#line 5516 "yaccsrc.c"
    break;

  case 487: /* template: pv  */
#line 1516 "yaccsrc.y"
                                       { yyval =makenode(X_TPL_SOLID,1,yyvsp[0]) ; }
#line 5522 "yaccsrc.c"
    break;

  case 488: /* template: error  */
#line 1517 "yaccsrc.y"
                    { exiterror( ERR_INVALID_TEMPLATE, 1, __reginatext ) ;}
#line 5528 "yaccsrc.c"
    break;

  case 489: /* solid: '-' offset  */
#line 1520 "yaccsrc.y"
                                       { yyval = makenode(X_NEG_OFFS,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 5535 "yaccsrc.c"
    break;

  case 490: /* solid: '+' offset  */
#line 1522 "yaccsrc.y"
                                       { yyval = makenode(X_POS_OFFS,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 5542 "yaccsrc.c"
    break;

  case 491: /* solid: offset  */
#line 1524 "yaccsrc.y"
                                       { yyval = makenode(X_ABS_OFFS,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 5549 "yaccsrc.c"
    break;

  case 492: /* solid: '=' offset  */
#line 1526 "yaccsrc.y"
                                       { yyval = makenode(X_ABS_OFFS,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 5556 "yaccsrc.c"
    break;

  case 493: /* solid: '(' symbtree ')'  */
#line 1528 "yaccsrc.y"
                                       { yyval = makenode(X_TPL_VAR,0) ;
                                         yyval->p[0] = yyvsp[-1] ; }
#line 5563 "yaccsrc.c"
    break;

  case 494: /* solid: '-' '(' symbtree ')'  */
#line 1530 "yaccsrc.y"
                                       { yyval = makenode(X_NEG_OFFS,0) ;
                                         yyval->p[0] = yyvsp[-1] ; }
#line 5570 "yaccsrc.c"
    break;

  case 495: /* solid: '+' '(' symbtree ')'  */
#line 1532 "yaccsrc.y"
                                       { yyval = makenode(X_POS_OFFS,0) ;
                                         yyval->p[0] = yyvsp[-1] ; }
#line 5577 "yaccsrc.c"
    break;

  case 496: /* solid: '=' '(' symbtree ')'  */
#line 1534 "yaccsrc.y"
                                       { yyval = makenode(X_ABS_OFFS,0) ;
                                         yyval->p[0] = yyvsp[-1] ; }
#line 5584 "yaccsrc.c"
    break;

  case 497: /* solid: string  */
#line 1536 "yaccsrc.y"
                                       { yyval = makenode(X_TPL_MVE,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 5591 "yaccsrc.c"
    break;

  case 498: /* offset: OFFSET  */
#line 1540 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 5597 "yaccsrc.c"
    break;

  case 499: /* offset: CONSYMBOL  */
#line 1541 "yaccsrc.y"
                                       { streng *sptr = Str_cre_TSD(parser_data.TSD,retvalue) ;
                                          if (myisnumber(parser_data.TSD, sptr))
                                          {
                                             exiterror( ERR_INVALID_INTEGER, 4, sptr->value ) ;
                                          }
                                          else
                                             exiterror( ERR_INVALID_TEMPLATE, 0 ) ;}
#line 5609 "yaccsrc.c"
    break;

  case 500: /* string: STRING  */
#line 1550 "yaccsrc.y"
                                       { yyval = (nodeptr) Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 5615 "yaccsrc.c"
    break;

  case 501: /* string: HEXSTRING  */
#line 1551 "yaccsrc.y"
                                       { streng *sptr = Str_make_TSD(parser_data.TSD,retlength) ;
                                         memcpy(sptr->value,retvalue,
                                                 sptr->len=retlength) ;
                                         yyval = (nodeptr) sptr ; }
#line 5624 "yaccsrc.c"
    break;

  case 502: /* string: BINSTRING  */
#line 1555 "yaccsrc.y"
                                       { streng *sptr = Str_make_TSD(parser_data.TSD,retlength) ;
                                         memcpy(sptr->value,retvalue,
                                                 sptr->len=retlength) ;
                                         yyval = (nodeptr) sptr ; }
#line 5633 "yaccsrc.c"
    break;

  case 503: /* pv: PLACEHOLDER pv  */
#line 1561 "yaccsrc.y"
                                       { yyval = makenode(X_TPL_POINT,1,yyvsp[0]) ; }
#line 5639 "yaccsrc.c"
    break;

  case 504: /* pv: symbtree pv  */
#line 1562 "yaccsrc.y"
                                       { yyval = makenode(X_TPL_SYMBOL,2,yyvsp[0],yyvsp[-1]) ; }
#line 5645 "yaccsrc.c"
    break;

  case 505: /* pv: %empty  */
#line 1563 "yaccsrc.y"
                                       { yyval = NULL ; }
#line 5651 "yaccsrc.c"
    break;

  case 506: /* $@39: %empty  */
#line 1573 "yaccsrc.y"
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
#line 5699 "yaccsrc.c"
    break;

  case 507: /* exprs: nexpr ',' $@39 exprs  */
#line 1616 "yaccsrc.y"
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
#line 5741 "yaccsrc.c"
    break;

  case 508: /* exprs: nexpr  */
#line 1653 "yaccsrc.y"
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
#line 5788 "yaccsrc.c"
    break;

  case 509: /* nexpr: expr  */
#line 1697 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 5794 "yaccsrc.c"
    break;

  case 510: /* nexpr: %empty  */
#line 1698 "yaccsrc.y"
                                       { yyval = NULL ; }
#line 5800 "yaccsrc.c"
    break;

  case 511: /* anyvars: xsimsymb anyvars  */
#line 1701 "yaccsrc.y"
                                        { yyval = makenode(X_SIM_SYMBOL,1,yyvsp[0]) ;
                                         yyval->name = (streng *) yyvsp[-1] ; }
#line 5807 "yaccsrc.c"
    break;

  case 512: /* anyvars: xsimsymb  */
#line 1703 "yaccsrc.y"
                                        { yyval = makenode(X_SIM_SYMBOL,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 5814 "yaccsrc.c"
    break;

  case 513: /* anyvars: '(' xsimsymb ')' anyvars  */
#line 1705 "yaccsrc.y"
                                        { yyval = makenode(X_IND_SYMBOL,1,yyvsp[0]) ;
                                         yyval->name = (streng *) yyvsp[-2] ; }
#line 5821 "yaccsrc.c"
    break;

  case 514: /* anyvars: '(' xsimsymb ')'  */
#line 1707 "yaccsrc.y"
                                        { yyval = makenode(X_IND_SYMBOL,0) ;
                                         yyval->name = (streng *) yyvsp[-1] ; }
#line 5828 "yaccsrc.c"
    break;

  case 515: /* xsimsymb: SIMSYMBOL  */
#line 1711 "yaccsrc.y"
                                       { yyval = (treenode *) Str_cre_TSD(parser_data.TSD,retvalue);}
#line 5834 "yaccsrc.c"
    break;

  case 516: /* simsymb: SIMSYMBOL  */
#line 1714 "yaccsrc.y"
                                       { yyval = (treenode *) Str_cre_TSD(parser_data.TSD,retvalue);}
#line 5840 "yaccsrc.c"
    break;

  case 517: /* simsymb: error  */
#line 1715 "yaccsrc.y"
                                       { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ) ;}
#line 5846 "yaccsrc.c"
    break;


#line 5850 "yaccsrc.c"

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

#line 1718 "yaccsrc.y"


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

/*
 * Rewrites the loop into
 *    DO <idx> = 1 TO <snapshot of the collection, its item count>
 *       name = <item idx of the snapshot>
 *       ...
 * with two hidden variables that a program cannot name.
 */
static void transform_do_over( nodeptr dostat, nodeptr var )
{
   static unsigned long counter = 0;
   char abuf[32], ibuf[32];
   nodeptr coll, rep, init, over, to, asg, item;
   streng *arrname, *idxname;

   coll = dostat->p[0]->p[1]->p[0]->p[1];
   counter++;
   sprintf( abuf, "\001OVERA%lu", counter );
   arrname = Str_cre_TSD( parser_data.TSD, abuf );
   sprintf( ibuf, "\001OVERI%lu", counter );
   idxname = Str_cre_TSD( parser_data.TSD, ibuf );

   init = makenode( X_STRING, 0 );
   init->name = Str_cre_TSD( parser_data.TSD, "1" );
   /*
    * p[1] and p[2] let the run time recognize the classic meaning
    * "DO expr" of "DO x OVER y" (x and OVER empty, y a whole number).
    */
   over = makenode( X_OO_OVERINIT, 3, coll, var,
                    dostat->p[0]->p[1]->p[0]->p[0]->p[1] );
   over->name = Str_dup_TSD( parser_data.TSD, arrname );
   to = makenode( X_DO_TO, 1, over );
   rep = makenode( X_REP, 2, init, to );
   rep->name = idxname;

   item = makenode( X_OO_OVERITEM, 2, create_head( abuf ), create_head( ibuf ) );
   Free_string_TSD( parser_data.TSD, arrname );
   asg = makenode( X_OO_OVERSTEP, 2, var, item );
   asg->lineno = dostat->lineno;
   asg->charnr = dostat->charnr;

   /*
    * WHILE is tested after the control variable is set: the condition
    * assigns the item itself before it is evaluated.
    */
   if ( dostat->p[1] && dostat->p[1]->type == X_WHILE )
      dostat->p[1]->p[0] = makenode( X_OO_OVERWHILE, 3, var, item,
                                     dostat->p[1]->p[0] );
   /* OVERSTEP and OVERWHILE share the item node */

   dostat->p[0] = rep;
   dostat->p[2] = optgluelast( asg, dostat->p[2] );
   dostat->p[2]->o.last = NULL;
   /* LEAVE name and ITERATE name use the visible name */
   dostat->name = Str_dup_TSD( parser_data.TSD, var->name );
}

