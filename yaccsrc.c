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
extern const char *__regina_oo_operator; /* lexsrc.l, OO extension */

#define IS_EXPRLIST(x) ( ( (x) != NULL )                   \
                      && ( ( (x)->type == X_CEXPRLIST )    \
                        || ( (x)->type == X_EXPRLIST ) ) )

#define IS_FUNCTION(x) ( ( (x) != NULL )                  \
                      && ( ( (x)->type == X_EX_FUNC )     \
                        || ( (x)->type == X_IN_FUNC ) ) )

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
#line 153 "yaccsrc.y"

#ifdef NDEBUG
# define YYDEBUG 0
#else
# define YYDEBUG 1
#endif

#line 204 "yaccsrc.c"

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
  YYSYMBOL_PLUSASSIGNMENTVARIABLE = 110,   /* PLUSASSIGNMENTVARIABLE  */
  YYSYMBOL_MINUSASSIGNMENTVARIABLE = 111,  /* MINUSASSIGNMENTVARIABLE  */
  YYSYMBOL_MULTASSIGNMENTVARIABLE = 112,   /* MULTASSIGNMENTVARIABLE  */
  YYSYMBOL_DIVASSIGNMENTVARIABLE = 113,    /* DIVASSIGNMENTVARIABLE  */
  YYSYMBOL_MODULUSASSIGNMENTVARIABLE = 114, /* MODULUSASSIGNMENTVARIABLE  */
  YYSYMBOL_INTDIVASSIGNMENTVARIABLE = 115, /* INTDIVASSIGNMENTVARIABLE  */
  YYSYMBOL_ORASSIGNMENTVARIABLE = 116,     /* ORASSIGNMENTVARIABLE  */
  YYSYMBOL_XORASSIGNMENTVARIABLE = 117,    /* XORASSIGNMENTVARIABLE  */
  YYSYMBOL_ANDASSIGNMENTVARIABLE = 118,    /* ANDASSIGNMENTVARIABLE  */
  YYSYMBOL_CONCATASSIGNMENTVARIABLE = 119, /* CONCATASSIGNMENTVARIABLE  */
  YYSYMBOL_120_ = 120,                     /* '|'  */
  YYSYMBOL_121_ = 121,                     /* '&'  */
  YYSYMBOL_122_ = 122,                     /* '='  */
  YYSYMBOL_CCAT = 123,                     /* CCAT  */
  YYSYMBOL_124_ = 124,                     /* '+'  */
  YYSYMBOL_125_ = 125,                     /* '-'  */
  YYSYMBOL_126_ = 126,                     /* '*'  */
  YYSYMBOL_127_ = 127,                     /* '/'  */
  YYSYMBOL_128_ = 128,                     /* '%'  */
  YYSYMBOL_UMINUS = 129,                   /* UMINUS  */
  YYSYMBOL_UPLUS = 130,                    /* UPLUS  */
  YYSYMBOL_131_ = 131,                     /* '['  */
  YYSYMBOL_132_ = 132,                     /* '('  */
  YYSYMBOL_133_ = 133,                     /* ')'  */
  YYSYMBOL_134_ = 134,                     /* ','  */
  YYSYMBOL_135_ = 135,                     /* ']'  */
  YYSYMBOL_YYACCEPT = 136,                 /* $accept  */
  YYSYMBOL_start = 137,                    /* start  */
  YYSYMBOL_138_1 = 138,                    /* $@1  */
  YYSYMBOL_prog = 139,                     /* prog  */
  YYSYMBOL_dirsect = 140,                  /* dirsect  */
  YYSYMBOL_dirblock = 141,                 /* dirblock  */
  YYSYMBOL_directive = 142,                /* directive  */
  YYSYMBOL_143_2 = 143,                    /* @2  */
  YYSYMBOL_dirwords = 144,                 /* dirwords  */
  YYSYMBOL_dirword = 145,                  /* dirword  */
  YYSYMBOL_stats = 146,                    /* stats  */
  YYSYMBOL_xstats = 147,                   /* xstats  */
  YYSYMBOL_ystatement = 148,               /* ystatement  */
  YYSYMBOL_lonely_end = 149,               /* lonely_end  */
  YYSYMBOL_nxstats = 150,                  /* nxstats  */
  YYSYMBOL_ncl = 151,                      /* ncl  */
  YYSYMBOL_nlncl = 152,                    /* nlncl  */
  YYSYMBOL_optLabels = 153,                /* optLabels  */
  YYSYMBOL_statement = 154,                /* statement  */
  YYSYMBOL_gruff = 155,                    /* gruff  */
  YYSYMBOL_mttstatement = 156,             /* mttstatement  */
  YYSYMBOL_mtstatement = 157,              /* mtstatement  */
  YYSYMBOL_nclstatement = 158,             /* nclstatement  */
  YYSYMBOL_call = 159,                     /* call  */
  YYSYMBOL_do = 160,                       /* do  */
  YYSYMBOL_exit = 161,                     /* exit  */
  YYSYMBOL_if = 162,                       /* if  */
  YYSYMBOL_iterate = 163,                  /* iterate  */
  YYSYMBOL_leave = 164,                    /* leave  */
  YYSYMBOL_say = 165,                      /* say  */
  YYSYMBOL_return = 166,                   /* return  */
  YYSYMBOL_address = 167,                  /* address  */
  YYSYMBOL_arg = 168,                      /* arg  */
  YYSYMBOL_drop = 169,                     /* drop  */
  YYSYMBOL_interpret = 170,                /* interpret  */
  YYSYMBOL_label = 171,                    /* label  */
  YYSYMBOL_nop = 172,                      /* nop  */
  YYSYMBOL_numeric = 173,                  /* numeric  */
  YYSYMBOL_options = 174,                  /* options  */
  YYSYMBOL_parse = 175,                    /* parse  */
  YYSYMBOL_proc = 176,                     /* proc  */
  YYSYMBOL_pull = 177,                     /* pull  */
  YYSYMBOL_push = 178,                     /* push  */
  YYSYMBOL_queue = 179,                    /* queue  */
  YYSYMBOL_select = 180,                   /* select  */
  YYSYMBOL_signal = 181,                   /* signal  */
  YYSYMBOL_when = 182,                     /* when  */
  YYSYMBOL_otherwise = 183,                /* otherwise  */
  YYSYMBOL_trace = 184,                    /* trace  */
  YYSYMBOL_upper = 185,                    /* upper  */
  YYSYMBOL_address_stat = 186,             /* address_stat  */
  YYSYMBOL_187_3 = 187,                    /* @3  */
  YYSYMBOL_address_stat2 = 188,            /* address_stat2  */
  YYSYMBOL_189_4 = 189,                    /* $@4  */
  YYSYMBOL_arg_stat = 190,                 /* arg_stat  */
  YYSYMBOL_call_stat = 191,                /* call_stat  */
  YYSYMBOL_192_5 = 192,                    /* $@5  */
  YYSYMBOL_193_6 = 193,                    /* $@6  */
  YYSYMBOL_call_name = 194,                /* call_name  */
  YYSYMBOL_call_args = 195,                /* call_args  */
  YYSYMBOL_expr_stat = 196,                /* expr_stat  */
  YYSYMBOL_end_stat = 197,                 /* end_stat  */
  YYSYMBOL_end = 198,                      /* end  */
  YYSYMBOL_do_stat = 199,                  /* do_stat  */
  YYSYMBOL_repetitor = 200,                /* repetitor  */
  YYSYMBOL_nvir = 201,                     /* nvir  */
  YYSYMBOL_naddr_with = 202,               /* naddr_with  */
  YYSYMBOL_203_7 = 203,                    /* @7  */
  YYSYMBOL_addr_with = 204,                /* addr_with  */
  YYSYMBOL_connection = 205,               /* connection  */
  YYSYMBOL_inputstmts = 206,               /* inputstmts  */
  YYSYMBOL_outputstmts = 207,              /* outputstmts  */
  YYSYMBOL_errorstmts = 208,               /* errorstmts  */
  YYSYMBOL_adeo = 209,                     /* adeo  */
  YYSYMBOL_adei = 210,                     /* adei  */
  YYSYMBOL_adio = 211,                     /* adio  */
  YYSYMBOL_inputstmt = 212,                /* inputstmt  */
  YYSYMBOL_outputstmt = 213,               /* outputstmt  */
  YYSYMBOL_errorstmt = 214,                /* errorstmt  */
  YYSYMBOL_resourcei = 215,                /* resourcei  */
  YYSYMBOL_resourceo = 216,                /* resourceo  */
  YYSYMBOL_resources = 217,                /* resources  */
  YYSYMBOL_nsimsymb = 218,                 /* nsimsymb  */
  YYSYMBOL_219_8 = 219,                    /* $@8  */
  YYSYMBOL_nnvir = 220,                    /* nnvir  */
  YYSYMBOL_221_9 = 221,                    /* $@9  */
  YYSYMBOL_nspace = 222,                   /* nspace  */
  YYSYMBOL_addrAll = 223,                  /* addrAll  */
  YYSYMBOL_addrSim = 224,                  /* addrSim  */
  YYSYMBOL_addrString = 225,               /* addrString  */
  YYSYMBOL_dovar = 226,                    /* dovar  */
  YYSYMBOL_tobyfor = 227,                  /* tobyfor  */
  YYSYMBOL_conditional = 228,              /* conditional  */
  YYSYMBOL_drop_stat = 229,                /* drop_stat  */
  YYSYMBOL_upper_stat = 230,               /* upper_stat  */
  YYSYMBOL_exit_stat = 231,                /* exit_stat  */
  YYSYMBOL_if_stat = 232,                  /* if_stat  */
  YYSYMBOL_unexp_then = 233,               /* unexp_then  */
  YYSYMBOL_unexp_else = 234,               /* unexp_else  */
  YYSYMBOL_ipret_stat = 235,               /* ipret_stat  */
  YYSYMBOL_iterate_stat = 236,             /* iterate_stat  */
  YYSYMBOL_label_stat = 237,               /* label_stat  */
  YYSYMBOL_labelname = 238,                /* labelname  */
  YYSYMBOL_leave_stat = 239,               /* leave_stat  */
  YYSYMBOL_nop_stat = 240,                 /* nop_stat  */
  YYSYMBOL_numeric_stat = 241,             /* numeric_stat  */
  YYSYMBOL_form_expr = 242,                /* form_expr  */
  YYSYMBOL_options_stat = 243,             /* options_stat  */
  YYSYMBOL_parse_stat = 244,               /* parse_stat  */
  YYSYMBOL_parse_flags = 245,              /* parse_flags  */
  YYSYMBOL_templs = 246,                   /* templs  */
  YYSYMBOL_parse_param = 247,              /* parse_param  */
  YYSYMBOL_proc_stat = 248,                /* proc_stat  */
  YYSYMBOL_pull_stat = 249,                /* pull_stat  */
  YYSYMBOL_push_stat = 250,                /* push_stat  */
  YYSYMBOL_queue_stat = 251,               /* queue_stat  */
  YYSYMBOL_say_stat = 252,                 /* say_stat  */
  YYSYMBOL_return_stat = 253,              /* return_stat  */
  YYSYMBOL_sel_end = 254,                  /* sel_end  */
  YYSYMBOL_select_stat = 255,              /* select_stat  */
  YYSYMBOL_when_stats = 256,               /* when_stats  */
  YYSYMBOL_when_stat = 257,                /* when_stat  */
  YYSYMBOL_ex_when_stat = 258,             /* ex_when_stat  */
  YYSYMBOL_otherwise_stat = 259,           /* otherwise_stat  */
  YYSYMBOL_signal_stat = 260,              /* signal_stat  */
  YYSYMBOL_signal_name = 261,              /* signal_name  */
  YYSYMBOL_namespec = 262,                 /* namespec  */
  YYSYMBOL_asymbol = 263,                  /* asymbol  */
  YYSYMBOL_on = 264,                       /* on  */
  YYSYMBOL_off = 265,                      /* off  */
  YYSYMBOL_c_action = 266,                 /* c_action  */
  YYSYMBOL_s_action = 267,                 /* s_action  */
  YYSYMBOL_trace_stat = 268,               /* trace_stat  */
  YYSYMBOL_whatever = 269,                 /* whatever  */
  YYSYMBOL_assignment = 270,               /* assignment  */
  YYSYMBOL_ass_part = 271,                 /* ass_part  */
  YYSYMBOL_plusassignment = 272,           /* plusassignment  */
  YYSYMBOL_plus_ass_part = 273,            /* plus_ass_part  */
  YYSYMBOL_minusassignment = 274,          /* minusassignment  */
  YYSYMBOL_minus_ass_part = 275,           /* minus_ass_part  */
  YYSYMBOL_multassignment = 276,           /* multassignment  */
  YYSYMBOL_mult_ass_part = 277,            /* mult_ass_part  */
  YYSYMBOL_divassignment = 278,            /* divassignment  */
  YYSYMBOL_div_ass_part = 279,             /* div_ass_part  */
  YYSYMBOL_intdivassignment = 280,         /* intdivassignment  */
  YYSYMBOL_intdiv_ass_part = 281,          /* intdiv_ass_part  */
  YYSYMBOL_modulusassignment = 282,        /* modulusassignment  */
  YYSYMBOL_modulus_ass_part = 283,         /* modulus_ass_part  */
  YYSYMBOL_andassignment = 284,            /* andassignment  */
  YYSYMBOL_and_ass_part = 285,             /* and_ass_part  */
  YYSYMBOL_xorassignment = 286,            /* xorassignment  */
  YYSYMBOL_xor_ass_part = 287,             /* xor_ass_part  */
  YYSYMBOL_orassignment = 288,             /* orassignment  */
  YYSYMBOL_or_ass_part = 289,              /* or_ass_part  */
  YYSYMBOL_concatassignment = 290,         /* concatassignment  */
  YYSYMBOL_concat_ass_part = 291,          /* concat_ass_part  */
  YYSYMBOL_expr = 292,                     /* expr  */
  YYSYMBOL_293_10 = 293,                   /* $@10  */
  YYSYMBOL_294_11 = 294,                   /* $@11  */
  YYSYMBOL_295_12 = 295,                   /* $@12  */
  YYSYMBOL_296_13 = 296,                   /* $@13  */
  YYSYMBOL_297_14 = 297,                   /* $@14  */
  YYSYMBOL_298_15 = 298,                   /* $@15  */
  YYSYMBOL_299_16 = 299,                   /* $@16  */
  YYSYMBOL_300_17 = 300,                   /* $@17  */
  YYSYMBOL_301_18 = 301,                   /* $@18  */
  YYSYMBOL_302_19 = 302,                   /* $@19  */
  YYSYMBOL_303_20 = 303,                   /* $@20  */
  YYSYMBOL_304_21 = 304,                   /* $@21  */
  YYSYMBOL_305_22 = 305,                   /* $@22  */
  YYSYMBOL_306_23 = 306,                   /* $@23  */
  YYSYMBOL_307_24 = 307,                   /* $@24  */
  YYSYMBOL_308_25 = 308,                   /* $@25  */
  YYSYMBOL_309_26 = 309,                   /* $@26  */
  YYSYMBOL_310_27 = 310,                   /* $@27  */
  YYSYMBOL_311_28 = 311,                   /* $@28  */
  YYSYMBOL_312_29 = 312,                   /* $@29  */
  YYSYMBOL_313_30 = 313,                   /* $@30  */
  YYSYMBOL_314_31 = 314,                   /* $@31  */
  YYSYMBOL_315_32 = 315,                   /* $@32  */
  YYSYMBOL_316_33 = 316,                   /* $@33  */
  YYSYMBOL_317_34 = 317,                   /* $@34  */
  YYSYMBOL_318_35 = 318,                   /* $@35  */
  YYSYMBOL_319_36 = 319,                   /* $@36  */
  YYSYMBOL_exprs_sub = 320,                /* exprs_sub  */
  YYSYMBOL_symbtree = 321,                 /* symbtree  */
  YYSYMBOL_function = 322,                 /* function  */
  YYSYMBOL_func_args = 323,                /* func_args  */
  YYSYMBOL_324_37 = 324,                   /* @37  */
  YYSYMBOL_msgtarget = 325,                /* msgtarget  */
  YYSYMBOL_bracket_args = 326,             /* bracket_args  */
  YYSYMBOL_327_38 = 327,                   /* @38  */
  YYSYMBOL_intfunc = 328,                  /* intfunc  */
  YYSYMBOL_extfunc = 329,                  /* extfunc  */
  YYSYMBOL_template = 330,                 /* template  */
  YYSYMBOL_solid = 331,                    /* solid  */
  YYSYMBOL_offset = 332,                   /* offset  */
  YYSYMBOL_string = 333,                   /* string  */
  YYSYMBOL_pv = 334,                       /* pv  */
  YYSYMBOL_exprs = 335,                    /* exprs  */
  YYSYMBOL_336_39 = 336,                   /* $@39  */
  YYSYMBOL_nexpr = 337,                    /* nexpr  */
  YYSYMBOL_anyvars = 338,                  /* anyvars  */
  YYSYMBOL_xsimsymb = 339,                 /* xsimsymb  */
  YYSYMBOL_simsymb = 340                   /* simsymb  */
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
#define YYLAST   2486

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  136
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  205
/* YYNRULES -- Number of rules.  */
#define YYNRULES  504
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  688

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   377


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
       2,     2,     2,     2,     2,     2,     2,   128,   121,     2,
     132,   133,   126,   124,   134,   125,     2,   127,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   122,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   131,     2,   135,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   120,     2,     2,     2,     2,     2,
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
     115,   116,   117,   118,   119,   123,   129,   130
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   163,   163,   163,   173,   176,   180,   184,   194,   195,
     198,   202,   207,   207,   219,   222,   223,   226,   228,   230,
     234,   236,   239,   241,   244,   245,   248,   253,   254,   257,
     258,   261,   262,   265,   266,   269,   270,   273,   277,   280,
     281,   282,   283,   286,   287,   288,   289,   290,   291,   292,
     293,   294,   295,   296,   297,   298,   299,   300,   301,   302,
     303,   304,   305,   306,   307,   308,   309,   310,   311,   312,
     313,   314,   315,   316,   317,   318,   319,   320,   323,   327,
     332,   336,   341,   345,   349,   353,   357,   361,   365,   369,
     373,   377,   381,   385,   389,   393,   397,   401,   405,   409,
     414,   418,   422,   426,   430,   434,   434,   438,   441,   442,
     443,   443,   445,   449,   455,   461,   461,   466,   466,   472,
     473,   474,   475,   476,   482,   487,   488,   494,   495,   498,
     506,   509,   515,   521,   523,   524,   527,   548,   552,   556,
     560,   563,   564,   565,   567,   570,   571,   572,   575,   575,
     581,   584,   585,   586,   589,   590,   591,   592,   595,   599,
     603,   607,   608,   609,   610,   611,   614,   615,   616,   617,
     618,   621,   622,   623,   624,   625,   628,   629,   632,   633,
     636,   637,   640,   641,   643,   644,   649,   650,   652,   653,
     654,   655,   660,   668,   669,   682,   683,   687,   688,   692,
     695,   695,   699,   699,   703,   704,   707,   708,   711,   719,
     727,   730,   731,   732,   735,   736,   737,   740,   741,   745,
     746,   750,   754,   762,   772,   774,   776,   777,   780,   783,
     786,   791,   793,   794,   797,   805,   809,   811,   812,   815,
     816,   819,   822,   823,   824,   827,   829,   831,   832,   833,
     836,   839,   840,   843,   846,   852,   858,   859,   862,   864,
     866,   868,   870,   872,   874,   878,   880,   883,   884,   885,
     886,   887,   888,   889,   891,   892,   895,   896,   897,   898,
     899,   903,   907,   911,   915,   919,   923,   924,   925,   928,
     933,   934,   936,   937,   938,   942,   943,   944,   947,   953,
     955,   956,   966,   967,   970,   974,   980,   983,   984,   986,
     987,   988,   989,   990,   996,  1000,  1001,  1007,  1008,  1009,
    1012,  1013,  1014,  1017,  1018,  1021,  1024,  1027,  1028,  1029,
    1030,  1033,  1034,  1035,  1036,  1039,  1041,  1043,  1044,  1048,
    1052,  1068,  1074,  1081,  1087,  1094,  1100,  1107,  1113,  1120,
    1126,  1133,  1139,  1146,  1152,  1159,  1165,  1172,  1178,  1185,
    1191,  1196,  1203,  1203,  1236,  1236,  1239,  1239,  1242,  1242,
    1245,  1246,  1246,  1249,  1250,  1250,  1253,  1254,  1254,  1257,
    1258,  1258,  1261,  1262,  1262,  1265,  1266,  1266,  1269,  1270,
    1270,  1273,  1274,  1274,  1277,  1278,  1278,  1281,  1282,  1284,
    1285,  1285,  1289,  1290,  1290,  1294,  1295,  1295,  1299,  1300,
    1300,  1304,  1305,  1305,  1309,  1310,  1310,  1314,  1315,  1315,
    1318,  1319,  1319,  1322,  1323,  1323,  1326,  1327,  1327,  1330,
    1331,  1331,  1334,  1335,  1335,  1338,  1339,  1339,  1342,  1343,
    1343,  1346,  1347,  1351,  1355,  1356,  1357,  1359,  1360,  1362,
    1366,  1370,  1372,  1373,  1375,  1377,  1378,  1381,  1382,  1383,
    1386,  1390,  1392,  1396,  1396,  1408,  1410,  1412,  1414,  1418,
    1418,  1425,  1428,  1431,  1432,  1433,  1436,  1438,  1440,  1442,
    1444,  1446,  1448,  1450,  1452,  1456,  1457,  1466,  1467,  1471,
    1477,  1478,  1479,  1489,  1489,  1569,  1613,  1614,  1617,  1619,
    1621,  1623,  1627,  1630,  1631
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
  "CASCADE", "DCOLON", "PLUSASSIGNMENTVARIABLE", "MINUSASSIGNMENTVARIABLE",
  "MULTASSIGNMENTVARIABLE", "DIVASSIGNMENTVARIABLE",
  "MODULUSASSIGNMENTVARIABLE", "INTDIVASSIGNMENTVARIABLE",
  "ORASSIGNMENTVARIABLE", "XORASSIGNMENTVARIABLE", "ANDASSIGNMENTVARIABLE",
  "CONCATASSIGNMENTVARIABLE", "'|'", "'&'", "'='", "CCAT", "'+'", "'-'",
  "'*'", "'/'", "'%'", "UMINUS", "UPLUS", "'['", "'('", "')'", "','",
  "']'", "$accept", "start", "$@1", "prog", "dirsect", "dirblock",
  "directive", "@2", "dirwords", "dirword", "stats", "xstats",
  "ystatement", "lonely_end", "nxstats", "ncl", "nlncl", "optLabels",
  "statement", "gruff", "mttstatement", "mtstatement", "nclstatement",
  "call", "do", "exit", "if", "iterate", "leave", "say", "return",
  "address", "arg", "drop", "interpret", "label", "nop", "numeric",
  "options", "parse", "proc", "pull", "push", "queue", "select", "signal",
  "when", "otherwise", "trace", "upper", "address_stat", "@3",
  "address_stat2", "$@4", "arg_stat", "call_stat", "$@5", "$@6",
  "call_name", "call_args", "expr_stat", "end_stat", "end", "do_stat",
  "repetitor", "nvir", "naddr_with", "@7", "addr_with", "connection",
  "inputstmts", "outputstmts", "errorstmts", "adeo", "adei", "adio",
  "inputstmt", "outputstmt", "errorstmt", "resourcei", "resourceo",
  "resources", "nsimsymb", "$@8", "nnvir", "$@9", "nspace", "addrAll",
  "addrSim", "addrString", "dovar", "tobyfor", "conditional", "drop_stat",
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
  "function", "func_args", "@37", "msgtarget", "bracket_args", "@38",
  "intfunc", "extfunc", "template", "solid", "offset", "string", "pv",
  "exprs", "$@39", "nexpr", "anyvars", "xsimsymb", "simsymb", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-554)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-498)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -554,    61,  -554,  -554,  -554,    33,     9,    40,   -21,  -554,
      34,    38,  -554,  -554,  -554,  1298,  -554,  -554,  -554,  -554,
      67,  -554,  -554,  -554,  -554,   489,  -554,    39,   -21,  -554,
    -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,
    -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,
    -554,  -554,  -554,  -554,  -554,  -554,  -554,  1904,  -554,  -554,
    -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,
    -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,
    -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,
    -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  1012,
    1090,  -554,  -554,  -554,  -554,   350,  -554,   111,   393,  1878,
    2036,  1717,   183,   202,  2036,  2036,  -554,    45,   -37,  2036,
      53,   193,  2036,   196,    35,   124,  2036,  2036,   185,  1208,
    -554,  -554,  1636,   -37,  -554,  -554,  -554,  -554,   239,   111,
    -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,
    -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,
    -554,  -554,  -554,  -554,  2036,  -554,  2036,  -554,  2036,  -554,
    2036,  -554,  2036,  -554,  2036,  -554,  2036,  -554,  2036,  -554,
    2036,  -554,  2036,  -554,  2036,  2191,  -554,  -554,  -554,  -554,
      22,  -554,  -554,  -554,  -554,    -6,  -554,   411,  -554,   411,
    -554,   411,  1798,  -554,  -554,    67,  -554,  -554,  -554,  -554,
    -554,  -554,  -554,  -554,  -554,  -554,    49,   302,  -554,   472,
    -554,   444,    41,  2267,  2191,  -554,    67,  2191,  -554,  -554,
     225,   235,  -554,  -554,    78,  -554,   102,    56,   102,  -554,
    2075,  -554,   127,   291,   -37,  2191,  -554,  -554,  2036,    46,
    2036,  -554,  -554,  -554,  -554,  -554,    55,   934,  -554,   105,
    -554,  -554,   128,    -3,   483,    45,  -554,    36,  -554,  -554,
    -554,  -554,   529,  -554,  2036,  -554,   303,  -554,   237,   280,
    2036,  -554,   330,  -554,   342,   344,    67,  -554,  -554,  -554,
    -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,
    -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,
    -554,  -554,  -554,  -554,  -554,  -554,   546,   546,  -554,  -554,
    -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  1798,  -554,
      22,   489,    67,  -554,  -554,    28,   126,  2036,  -554,  -554,
    -554,  -554,  -554,    91,  -554,   348,  2036,  -554,  -554,  -554,
    2036,  2036,   111,  2036,   685,  -554,   174,  -554,  -554,  -554,
    2036,   240,  -554,  -554,  -554,  2036,  -554,  2036,  -554,  -554,
      45,  -554,  -554,  -554,     2,     4,   157,   167,    45,  -554,
    -554,   133,  -554,  -554,  2191,  -554,  2036,  -554,  -554,   360,
    2191,  -554,  -554,   264,  -554,  -554,  -554,  -554,  -554,    45,
      56,  -554,   386,  -554,  -554,   284,  1168,   292,   192,  -554,
    2191,  -554,  -554,  -554,  -554,  -554,  -554,   175,  -554,   398,
    2191,  -554,  -554,  -554,  2036,  2036,  2036,  2036,  2036,  2036,
    2036,  2036,  2036,  2036,  2036,  2036,  2036,  2036,  2036,  2036,
    2036,  2036,  -554,  -554,  -554,  -554,  -554,  -554,  2036,  2036,
    2036,  2036,  2036,  2036,  2036,  2036,  -554,  2036,  -554,  -554,
    -554,  -554,  -554,  -554,   172,  -554,    88,   399,  -554,  -554,
    2267,  2267,    67,  2267,  -554,  -554,   262,  1959,  -554,   403,
    -554,  -554,  -554,   275,   275,   275,   285,  2113,   262,  -554,
     167,  -554,   167,  -554,   167,  -554,   213,  -554,   -37,  2191,
    -554,  -554,    56,  -554,  -554,  -554,   465,  -554,   596,  -554,
     310,  -554,   430,  -554,   201,   299,  2008,  2008,  2008,  2008,
    2008,  2008,  2008,   201,   411,  2343,  2008,  2008,  2008,  2008,
    2008,  2008,  -554,  -554,  2343,  2355,  2008,   980,   980,   299,
     299,   299,   195,  2036,  -554,  -554,  -554,  -554,  -554,  -554,
    -554,   317,   323,  -554,  1428,   527,   726,  -554,   365,  -554,
    -554,  -554,   275,   275,   371,  -554,   275,   275,   136,  -554,
     275,   275,   409,   112,   197,   216,   275,  -554,   241,   268,
     277,  -554,  -554,   383,  -554,  -554,   357,  -554,  -554,  -554,
    -554,  -554,  1558,  -554,  -554,  2036,  2036,  2036,   527,  -554,
     448,  -554,   275,   373,   275,   377,   275,   373,   275,   381,
     275,   377,   275,   381,  -554,   575,  -554,   807,  -554,   807,
     262,  -554,  -554,  -554,  -554,  -554,   449,  2267,  2267,  2267,
     527,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,   276,
     119,   311,   402,    51,  -554,  -554,  -554,    80,   104,  -554,
    -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,   856,
    -554,  -554,   275,  -554,  -554,   275,  -554,  -554,  -554,  -554,
    -554,  -554,  -554,  -554,  -554,  -554,   467,  -554,  -554,   748,
     127,  -554,  -554,  -554,  -554,  -554,  -554,  -554
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       2,     0,    34,     1,     3,    37,    32,     0,     7,     9,
      37,    37,    21,    25,    24,    37,    35,    36,    34,    90,
      31,   235,    33,   234,    14,     0,     8,    37,     6,    20,
      86,    87,    78,    79,    80,    81,    82,    89,    83,    91,
      92,    94,    95,    96,    97,    98,    84,    85,    99,   101,
      88,   102,   100,   103,   132,   104,   341,   399,   397,   376,
     405,   411,   408,   414,   417,   420,   423,   394,   391,   388,
     448,   460,   472,   471,   449,   451,   450,    93,   426,   429,
     432,   435,   438,   441,   444,   445,   343,   345,   347,   349,
     353,   351,   359,   357,   355,   361,   382,   385,   402,     0,
       0,   370,   373,   379,   362,     0,    38,     0,     0,   144,
     497,     0,     0,     0,   497,   497,   105,     0,     0,     0,
       0,     0,   497,     0,     0,     0,   497,   497,     0,     0,
     302,   303,   497,     0,    43,    45,    46,    44,     0,     0,
      47,    48,    66,    49,    40,    41,    42,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    62,    61,
      63,    64,    65,    77,   497,    67,   497,    68,   497,    69,
     497,    70,   497,    71,   497,    72,   497,    75,   497,    73,
     497,    74,   497,    76,   497,   131,   447,   452,   463,   463,
      30,    34,    18,    17,    19,     0,    16,   398,   455,   453,
     456,   454,   497,   228,   229,    39,   128,   325,   326,   323,
     324,   488,   487,   489,   115,   127,     0,     0,   117,     0,
     210,   216,     0,   205,   496,   221,   226,    34,   504,   503,
       0,     0,   284,   285,     0,   475,   492,   114,   492,   266,
     474,   502,     0,     0,   499,   230,   240,   250,   242,     0,
     248,   253,   257,   267,   269,   272,     0,     0,   271,   258,
     270,   268,   261,   264,     0,     0,   277,     0,   281,   282,
     283,   292,     0,   319,     0,   318,     0,   317,     0,     0,
       0,   339,     0,   336,     0,     0,    26,   340,   342,   344,
     346,   348,   350,   352,   354,   356,   358,   360,   395,   374,
     403,   409,   406,   412,   415,   418,   421,   392,   389,   386,
     424,   427,   430,   433,   436,   439,     0,     0,   380,   383,
     400,   364,   366,   368,   371,   377,   469,   462,   497,   461,
      29,     0,    13,   459,   363,     0,   495,   497,   119,   328,
     329,   330,   327,     0,   120,     0,   497,   142,   204,   141,
       0,     0,     0,     0,   392,   143,     0,   232,   237,   110,
       0,     0,   145,   146,   147,     0,   106,   497,   108,   490,
       0,   491,   485,   486,     0,     0,     0,     0,     0,   478,
     484,     0,   217,   498,   241,   247,     0,   251,   252,     0,
     249,   273,   275,     0,   259,   262,   260,   263,   256,     0,
     255,   278,     0,   297,   293,     0,     0,     0,   305,   296,
     306,   307,   309,   333,   332,   334,   331,     0,   310,     0,
     335,   337,   219,   135,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   465,   466,   442,   463,   463,   443,     0,     0,
       0,     0,     0,     0,     0,     0,   446,   497,   464,    15,
     458,   457,   493,   116,   129,   121,     0,     0,   125,   118,
     205,   205,    37,   205,   227,    34,   148,   148,   157,     0,
     154,   155,   156,   205,   205,   205,   153,     0,   148,   265,
       0,   479,     0,   477,     0,   476,     0,   473,   501,   246,
     243,   274,   254,   279,   290,   301,    34,   291,     0,   295,
       0,   311,     0,   315,   396,   375,   404,   410,   407,   413,
     416,   419,   422,   393,   390,   387,   425,   428,   431,   434,
     437,   440,   467,   468,   381,   384,   401,   365,   367,   369,
     372,   378,     0,   497,   130,   322,   320,   321,   122,   214,
     215,    37,     0,    37,    37,   140,     0,   111,     0,   107,
     152,   158,   205,   205,   165,   159,   205,   205,   170,   160,
     205,   205,   175,     0,     0,     0,   205,   112,     0,     0,
       0,   480,   500,     0,   294,    37,     0,   289,   312,   470,
     494,    22,    37,   136,    23,     0,     0,     0,   139,   225,
     222,   149,   205,   161,   205,   163,   205,   166,   205,   168,
     205,   171,   205,   173,   177,     0,   179,     0,   181,     0,
     148,   483,   482,   481,    34,   304,     0,   205,   205,   205,
     138,    34,   162,   164,   167,   169,   172,   174,   185,     0,
       0,     0,     0,     0,   176,   182,   191,     0,     0,   178,
     186,   180,   113,    37,   287,   211,   213,   212,   137,     0,
     193,   192,   205,   195,   194,   205,   197,   196,   199,   198,
     184,   183,   188,   187,   190,   189,   298,   224,   223,     0,
       0,   299,   203,   206,   207,   209,   208,   201
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -554,  -554,  -554,  -554,   474,    57,  -554,  -554,  -554,   158,
     481,  -554,   -11,  -554,   -92,   -88,  -222,    -4,  -461,   -14,
    -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,
    -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,
    -554,  -554,  -554,  -554,  -554,  -554,  -250,  -246,  -554,  -554,
    -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,   149,
    -554,  -554,   -53,  -554,  -554,  -554,  -456,  -554,   -57,  -554,
    -554,  -554,  -554,  -554,  -554,  -554,  -436,  -430,  -425,  -554,
     -75,   159,  -554,  -554,  -125,  -554,  -114,  -554,  -171,  -554,
    -554,  -553,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,
    -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,
    -234,   247,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,
    -554,   144,  -554,  -554,  -554,  -554,   142,   436,   451,   452,
     333,   309,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,
    -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,
    -554,  -554,  -554,  -554,  -554,  -554,   -13,  -554,  -554,  -554,
    -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,
    -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,  -554,
    -554,  -554,  -554,  -554,   266,  -108,  -554,  -174,  -554,   278,
    -554,  -554,   230,   259,  -119,  -554,   194,  -236,    16,  -319,
    -554,  -102,  -123,  -235,  -110
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,     2,     4,     8,     9,    10,    25,   195,   196,
      11,   551,    12,    13,   552,    20,     5,     6,    14,    15,
      16,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,    21,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   234,   366,   476,   135,   136,   337,   346,   214,   463,
     137,   138,   139,   140,   221,   367,   557,   558,   368,   479,
     480,   481,   482,   561,   565,   569,   483,   484,   485,   644,
     649,   650,   664,   665,   661,   662,   349,   682,   683,   684,
     222,   598,   352,   141,   142,   143,   144,   145,   146,   147,
     148,    22,    23,   149,   150,   151,   389,   152,   153,   264,
     237,   265,   154,   155,   156,   157,   158,   159,   587,   160,
     408,   409,    17,   510,   161,   276,   467,   215,   216,   217,
     416,   417,   162,   282,   163,   164,   165,   166,   167,   168,
     169,   170,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   182,   183,   184,   224,   202,   451,   452,
     453,   454,   425,   455,   448,   449,   435,   434,   433,   424,
     450,   426,   428,   427,   429,   430,   431,   432,   436,   437,
     438,   439,   440,   441,   334,   186,   187,   327,   328,   444,
     456,   457,   188,   189,   239,   378,   379,   218,   240,   335,
     543,   336,   243,   244,   230
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      29,   105,   185,   231,   380,   356,   268,   381,   225,   238,
     284,   553,   232,   233,   190,   329,    29,   238,   464,   205,
     251,   559,   406,   226,   269,   270,   407,   464,   285,   460,
     283,   400,   577,    -5,   -11,   241,   266,   401,    -4,   -10,
     272,    24,    18,   396,   197,   630,   235,   385,   566,   570,
     338,   286,   670,   562,   246,   571,   228,    18,   563,   567,
     331,     3,   287,   267,   288,    26,   289,   372,   290,   372,
     291,   386,   292,   373,   293,   373,   294,   658,   295,   359,
     296,   672,   297,  -276,    19,    26,   199,   201,     7,   545,
     591,   339,   465,  -492,  -245,   242,   223,    19,   227,   387,
     388,  -239,   397,   360,   361,   674,   245,   332,   241,   355,
    -492,   -12,   -12,   614,   236,   191,  -492,    71,   -12,   340,
     663,   383,  -492,  -492,   553,   235,  -109,   229,   238,  -492,
     238,   608,   341,   604,   490,   612,   492,   602,   542,  -124,
     610,   606,     7,   -11,   402,   342,   391,     7,   -10,   362,
     363,   639,   640,   641,   642,   393,   364,   238,   406,    18,
     546,   461,   508,   353,   652,   502,   547,  -492,   242,  -492,
    -492,   236,  -492,   466,    71,   474,   511,  -492,   348,  -492,
     639,   640,   641,   642,   228,  -200,   271,   330,   475,  -492,
     370,  -200,   676,   236,   247,  -492,    71,   252,   616,   241,
     253,  -492,  -492,   228,   639,   640,   641,   642,  -492,  -205,
     365,   394,  -205,  -205,  -205,  -205,  -205,   618,   254,   255,
     256,   257,   372,  -314,   590,   258,   357,    49,   373,    51,
     573,  -233,   575,    18,   395,   384,   358,   390,   412,    71,
     228,   478,   259,   248,   249,   250,  -492,   486,  -492,  -492,
    -238,   489,   369,   556,   371,   229,  -492,   466,   299,   497,
     462,   410,   238,   348,   472,   488,   498,   420,   308,   496,
     238,   532,   533,  -231,   229,   260,   261,   660,   413,   339,
     414,   418,   348,  -236,   583,   504,   585,  -134,  -205,   494,
     501,   238,   382,   507,  -205,  -205,  -205,  -205,  -205,  -205,
    -205,   262,   263,   344,   411,   544,   348,   340,   316,   317,
    -150,   229,   666,  -205,  -205,  -205,  -205,  -205,  -205,  -205,
     341,   413,   339,   414,   415,   321,   322,   323,   324,   325,
     589,   421,   326,   342,  -205,  -205,  -205,   470,   471,  -218,
     473,   348,  -202,   422,   339,   423,   581,   477,  -202,   468,
     340,  -308,   487,  -202,  -202,   586,   549,   550,   228,   555,
    -202,   500,   -27,   341,   203,   204,   308,   415,    54,   564,
     568,   572,   340,   499,   621,   582,   342,  -202,  -338,   573,
     574,   575,   578,  -202,   579,   341,   580,   503,  -202,  -202,
    -220,   361,  -133,   506,   206,  -202,  -126,   624,   342,   513,
     548,   622,   653,   668,   560,  -288,   316,   317,  -244,   659,
     623,   514,   515,   516,   517,   518,   519,   520,   521,   522,
     523,   524,   525,   526,   527,   528,   529,   530,   531,   229,
     326,   588,   207,   208,  -280,   534,   535,   536,   537,   538,
     539,   540,   541,   685,   686,   686,  -316,  -123,   603,   605,
     654,  -151,   607,   609,   350,   351,   611,   613,   554,   615,
     617,   619,   620,   631,   209,   210,   574,   575,  -202,   575,
     211,   212,   574,   347,  -202,   573,   626,   213,  -313,  -202,
    -202,   681,  -205,  -205,   398,    28,  -202,   253,   632,   459,
     633,    27,   634,   625,   635,   469,   636,  -286,   637,   593,
    -300,   601,  -300,   573,   574,   254,   255,   256,   257,   687,
    -300,   399,   258,   655,   656,   657,   667,   669,   316,   317,
    -205,   298,   299,   300,   301,   302,   303,   304,   305,   306,
     403,   307,   308,   309,   595,   596,   597,   592,   348,   594,
     105,   185,   326,   404,   651,   600,   445,   445,   679,   343,
     345,   680,   509,   310,   311,   312,   313,   314,   315,   512,
     192,   193,   260,   261,    49,   277,    51,   194,   491,   493,
     495,   554,   316,   317,   405,   446,   446,   191,   105,   185,
     278,   279,   627,   628,   629,   318,   319,   320,   419,   321,
     322,   323,   324,   325,   458,   447,   326,   584,     0,   -34,
     -34,   -34,   -34,     0,     0,     0,     0,     0,   -34,   -34,
     -34,   -34,   -34,   -34,   -34,   -34,   -34,   -34,   442,    72,
      73,     0,     0,   -34,   443,   -34,   -34,   -34,   -34,   -34,
     -34,   -34,   -34,   -34,   -34,     0,     0,     0,     0,   592,
     -34,   -34,   -34,   -34,   -34,     0,     0,     0,   678,     0,
       0,   -34,   -34,   -34,   -34,   -34,   -34,   -34,   -34,   -34,
     -34,     0,   -34,   -34,   -34,     0,     0,   -34,   -34,   -34,
     -34,   -34,   638,   -34,   -34,   639,   640,   641,   642,   643,
     -34,   -34,     0,     0,   -34,   -34,   -34,   -34,   -34,   -34,
       0,     0,  -204,  -204,  -204,  -204,  -204,     0,     0,     0,
       0,     0,     0,   -34,   -34,     0,   -34,   -34,   -34,   -34,
     -34,   -34,   -34,   -34,   -34,   -34,   -34,   -34,   -34,     0,
     -34,   -34,   -34,   -34,   -34,     0,     0,   599,   -34,   -37,
     -37,   -37,   -37,  -204,     0,     0,     0,     0,   -37,   -37,
     -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,     0,     0,
       0,     0,     0,   -37,     0,   -37,   -37,   -37,   -37,   -37,
     -37,   -37,   -37,   -37,   -37,     0,     0,     0,     0,     0,
     -37,   -37,   -37,   -37,   645,     0,     0,     0,     0,     0,
       0,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,
     -37,     0,   -37,   -37,   -37,     0,     0,   -37,   -37,   -37,
     -37,     0,   671,   -37,   -37,     0,   673,   675,     0,     0,
     -37,   -37,     0,     0,   -37,   -37,   -37,   -37,   -37,   -37,
     241,     0,     0,     0,     0,   211,   212,     0,     0,     0,
       0,     0,   213,   -37,   -37,     0,   -37,   -37,   -37,   -37,
     -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,     0,
     -37,   -37,   -37,   -37,   -37,     0,     0,   677,   -37,   -37,
     -37,   -37,   -37,     0,     0,     0,     0,     0,   -37,   -37,
     -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,     0,     0,
       0,     0,     0,   -37,     0,   -37,   -37,   -37,   -37,   -37,
     -37,   -37,   -37,   -37,   -37,     0,     0,     0,     0,     0,
     -37,   -37,   -37,   -37,   646,   647,   648,   639,   640,   641,
     642,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,
     -37,     0,   -37,   -37,   -37,     0,     0,   -37,   -37,   -37,
     -37,     0,     0,   -37,   -37,   392,     0,     0,     0,     0,
     -37,   -37,     0,     0,   -37,   -37,   -37,   -37,   -37,   -37,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    -497,     0,     0,   -37,   -37,     0,   -37,   -37,   -37,   -37,
     -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,     0,
     -37,   -37,   -37,   -37,   -37,     0,     0,     0,   -37,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,     0,
      67,    68,    69,     0,     0,    70,    71,    72,    73,     0,
       0,    74,    75,   198,     0,     0,     0,     0,    76,     0,
       0,     0,    78,    79,    80,    81,    82,    83,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   299,     0,     0,
       0,    84,    85,     0,     0,     0,     0,   308,     0,     0,
       0,     0,     0,     0,    96,    97,    98,     0,    99,   100,
     101,   102,   103,     0,     0,     0,   104,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,     0,    67,    68,
      69,     0,     0,    70,    71,    72,    73,   316,   317,    74,
      75,   200,     0,     0,     0,     0,    76,     0,     0,     0,
      78,    79,    80,    81,    82,    83,   323,   324,   325,     0,
       0,   326,     0,     0,     0,     0,     0,     0,     0,    84,
      85,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    96,    97,    98,     0,    99,   100,   101,   102,
     103,     0,     0,     0,   104,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,     0,    67,    68,    69,     0,
       0,    70,    71,    72,    73,     0,     0,    74,    75,   505,
       0,     0,     0,     0,    76,     0,     0,     0,    78,    79,
      80,    81,    82,    83,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    84,    85,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   273,
      96,    97,    98,     0,    99,   100,   101,   102,   103,     0,
       0,     0,   104,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,   274,    67,    68,    69,     0,     0,    70,
      71,    72,    73,     0,     0,    74,    75,   207,   208,     0,
       0,     0,    76,     0,     0,     0,    78,    79,    80,    81,
      82,    83,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    84,    85,     0,     0,   209,
     210,     0,     0,     0,     0,     0,   275,     0,    96,    97,
      98,     0,    99,   100,   101,   102,   103,     0,     0,     0,
     104,    30,    31,    32,    33,     0,     0,     0,     0,     0,
      34,    35,     0,     0,    36,    37,    38,    39,    40,    41,
       0,     0,     0,     0,     0,    42,     0,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,     0,     0,     0,
       0,     0,    53,    54,    55,    56,     0,     0,     0,     0,
       0,     0,     0,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,     0,    67,    68,    69,     0,     0,    70,
      71,    72,    73,     0,     0,    74,    75,     0,     0,     0,
       0,     0,    76,    77,     0,     0,    78,    79,    80,    81,
      82,    83,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    84,    85,     0,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,     0,    99,   100,   101,   102,   103,     0,     0,     0,
     104,    30,    31,    32,    33,     0,     0,     0,     0,     0,
      34,    35,     0,     0,    36,    37,    38,    39,    40,    41,
       0,     0,     0,     0,     0,    42,     0,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,     0,     0,     0,
       0,     0,    53,   -28,    55,    56,     0,     0,     0,     0,
       0,     0,     0,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,     0,    67,    68,    69,     0,     0,    70,
      71,    72,    73,     0,     0,    74,    75,     0,     0,     0,
       0,     0,    76,    77,     0,     0,    78,    79,    80,    81,
      82,    83,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    84,    85,     0,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,     0,    99,   100,   101,   102,   103,     0,     0,     0,
     104,    30,    31,    32,    33,     0,     0,     0,     0,     0,
      34,    35,     0,     0,    36,    37,    38,    39,    40,    41,
       0,     0,     0,     0,     0,    42,     0,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,     0,     0,     0,
       0,     0,    53,     0,    55,    56,     0,     0,     0,     0,
       0,     0,     0,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,     0,    67,    68,    69,     0,     0,    70,
      71,    72,    73,     0,     0,    74,    75,     0,     0,     0,
       0,     0,    76,    77,     0,     0,    78,    79,    80,    81,
      82,    83,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   280,     0,     0,     0,    84,    85,     0,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,     0,    99,   100,   101,   102,   103,     0,     0,     0,
     104,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,     0,    67,    68,    69,     0,     0,    70,    71,    72,
      73,     0,     0,    74,    75,     0,     0,   281,     0,     0,
      76,     0,     0,     0,    78,    79,    80,    81,    82,    83,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    84,    85,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    96,    97,    98,     0,
      99,   100,   101,   102,   103,    18,     0,     0,   104,     0,
       0,     0,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,     0,    67,    68,    69,     0,     0,    70,    71,
      72,    73,     0,     0,    74,    75,     0,     0,     0,     0,
       0,    76,     0,     0,     0,    78,    79,    80,    81,    82,
      83,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    84,    85,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    96,    97,    98,
       0,    99,   100,   101,   102,   103,   333,     0,     0,   104,
       0,     0,     0,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,     0,    67,    68,    69,     0,     0,    70,
      71,    72,    73,     0,     0,    74,    75,     0,     0,     0,
       0,     0,    76,     0,     0,     0,    78,    79,    80,    81,
      82,    83,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    84,    85,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    96,    97,
      98,     0,    99,   100,   101,   102,   103,   219,     0,     0,
     104,     0,     0,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,     0,    67,    68,    69,     0,     0,    70,
      71,    72,    73,     0,   220,    74,    75,     0,     0,    57,
       0,     0,    76,     0,     0,     0,    78,    79,    80,    81,
      82,    83,     0,     0,     0,    70,    71,    72,    73,     0,
       0,    74,    75,     0,     0,    84,    85,     0,    76,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    96,    97,
      98,     0,    99,   100,   101,   102,   103,  -150,     0,     0,
     104,    84,    85,     0,     0,   298,   299,   300,   301,   302,
     303,   304,   305,   306,     0,   307,   308,   309,     0,     0,
       0,     0,     0,     0,     0,     0,   104,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   310,   311,   312,
     313,   314,   315,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   298,   299,   316,   317,     0,     0,
       0,     0,     0,     0,   307,   308,     0,     0,     0,   318,
     319,   320,     0,   321,   322,   323,   324,   325,     0,     0,
     326,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,     0,    67,    68,    69,     0,     0,    70,    71,    72,
      73,     0,     0,    74,    75,   316,   317,     0,     0,     0,
      76,     0,     0,     0,    78,    79,    80,    81,    82,    83,
       0,     0,   321,   322,   323,   324,   325,     0,     0,   326,
     372,     0,     0,    84,    85,     0,   373,     0,     0,     0,
       0,     0,   211,   212,     0,     0,    96,    97,    98,   213,
      99,   100,   101,   102,   103,     0,     0,     0,   104,   298,
     299,   300,   301,   302,   303,   304,   305,   306,     0,   307,
     308,   309,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   374,     0,   375,
     376,   310,   311,   312,   313,   314,   315,   377,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     316,   317,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   318,   319,   320,     0,   321,   322,   323,
     324,   325,     0,     0,   326,     0,   576,   298,   299,   300,
     301,   302,   303,   304,   305,   306,     0,   307,   308,   309,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   310,
     311,   312,   313,   314,   315,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   316,   317,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   318,   319,   320,     0,   321,   322,   323,   324,   325,
       0,     0,   326,   298,   299,   300,   301,   302,   303,   304,
     305,   306,     0,   354,   308,   309,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   310,   311,   312,   313,   314,
     315,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   316,   317,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   318,   319,   320,
       0,   321,   322,   323,   324,   325,     0,     0,   326,   298,
     299,   300,   301,   302,   303,   304,   305,   306,     0,   307,
     308,   298,   299,   300,   301,   302,   303,   304,   305,   306,
       0,   307,   308,     0,     0,     0,     0,     0,     0,     0,
       0,   310,   311,   312,   313,   314,   315,     0,     0,     0,
       0,     0,     0,   310,   311,   312,   313,   314,   315,     0,
     316,   317,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   316,   317,   319,   320,     0,   321,   322,   323,
     324,   325,     0,     0,   326,     0,     0,   320,     0,   321,
     322,   323,   324,   325,     0,     0,   326
};

static const yytype_int16 yycheck[] =
{
      11,    15,    15,   113,   240,   227,   125,   242,   110,   117,
     133,   472,   114,   115,    18,   189,    27,   125,   337,   107,
     122,   477,   272,   111,   126,   127,   272,   346,   138,     1,
     132,   265,   488,     0,     0,    72,     1,     1,     0,     0,
     128,     1,    48,    46,    57,   598,     1,     1,   484,   485,
       1,   139,     1,   483,     1,   485,     1,    48,   483,   484,
      66,     0,   164,    28,   166,     8,   168,    65,   170,    65,
     172,    25,   174,    71,   176,    71,   178,   630,   180,     1,
     182,     1,   184,    48,    75,    28,    99,   100,   109,     1,
     551,    42,     1,    48,    48,   132,   109,    75,   111,    53,
      54,    48,   105,    25,    26,     1,   119,   195,    72,   223,
      65,    71,    72,     1,    69,    48,    71,    72,    78,    70,
       1,   244,    77,    78,   585,     1,    48,    72,   236,    84,
     238,   567,    83,   563,   132,   571,   132,   562,   457,    48,
     570,   566,   109,   109,   267,    96,   256,   109,   109,    71,
      72,   100,   101,   102,   103,   257,    78,   265,   408,    48,
      72,   133,   408,   122,   620,   399,    78,   122,   132,   124,
     125,    69,    48,    82,    72,     1,     1,   132,    66,   134,
     100,   101,   102,   103,     1,    66,     1,   191,    14,    65,
     134,    72,   653,    69,     1,    71,    72,     1,     1,    72,
       4,    77,    78,     1,   100,   101,   102,   103,    84,    97,
     132,   106,   100,   101,   102,   103,   104,     1,    22,    23,
      24,    25,    65,    48,   543,    29,     1,    35,    71,    37,
      94,    48,    96,    48,   106,   248,     1,   250,     1,    72,
       1,     1,    46,    50,    51,    52,   122,   361,   124,   125,
      48,   370,   236,   475,   238,    72,   132,    82,    57,   378,
     134,   274,   370,    66,   352,   367,   133,   280,    67,   377,
     378,   445,   446,    48,    72,    79,    80,     1,    41,    42,
      43,     1,    66,    48,   506,     1,   508,    48,    48,   132,
      26,   399,     1,     1,    97,    98,    99,   100,   101,   102,
     103,   105,   106,     1,     1,   133,    66,    70,   107,   108,
      48,    72,     1,    97,    98,    99,   100,   101,   102,   103,
      83,    41,    42,    43,    87,   124,   125,   126,   127,   128,
     135,     1,   131,    96,    94,    95,    96,   350,   351,    48,
     353,    66,    66,     1,    42,     1,   133,   360,    72,     1,
      70,    48,   365,    77,    78,    45,   470,   471,     1,   473,
      84,     1,    45,    83,    14,    15,    67,    87,    45,   483,
     484,   485,    70,   386,   133,   498,    96,    66,    48,    94,
      95,    96,   490,    72,   492,    83,   494,     1,    77,    78,
      48,    26,    48,   406,     1,    84,    48,    14,    96,     1,
       1,   133,   624,     1,     1,    48,   107,   108,    48,   631,
     133,   424,   425,   426,   427,   428,   429,   430,   431,   432,
     433,   434,   435,   436,   437,   438,   439,   440,   441,    72,
     131,     1,    39,    40,    48,   448,   449,   450,   451,   452,
     453,   454,   455,   679,   679,   680,    48,    48,   562,   563,
       1,    48,   566,   567,    10,    11,   570,   571,   472,   573,
     574,   575,   576,    15,    71,    72,    95,    96,    66,    96,
      77,    78,    95,     1,    72,    94,   586,    84,    48,    77,
      78,    14,    10,    11,     1,    11,    84,     4,   602,   331,
     604,    10,   606,   585,   608,   346,   610,    48,   612,   552,
      35,   558,    37,    94,    95,    22,    23,    24,    25,   680,
      45,   264,    29,   627,   628,   629,   641,   642,   107,   108,
      48,    56,    57,    58,    59,    60,    61,    62,    63,    64,
       1,    66,    67,    68,     7,     8,     9,   551,    66,   553,
     554,   554,   131,    14,   619,   556,   316,   317,   662,   216,
     217,   665,   408,    88,    89,    90,    91,    92,    93,   417,
      71,    72,    79,    80,    35,   129,    37,    78,   374,   375,
     376,   585,   107,   108,    45,   316,   317,    48,   592,   592,
     129,   129,   595,   596,   597,   120,   121,   122,   279,   124,
     125,   126,   127,   128,   328,   317,   131,     1,    -1,     3,
       4,     5,     6,    -1,    -1,    -1,    -1,    -1,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    72,    73,
      74,    -1,    -1,    27,    78,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    -1,    -1,    -1,    -1,   653,
      44,    45,    46,    47,    48,    -1,    -1,    -1,   659,    -1,
      -1,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    -1,    66,    67,    68,    -1,    -1,    71,    72,    73,
      74,    75,    97,    77,    78,   100,   101,   102,   103,   104,
      84,    85,    -1,    -1,    88,    89,    90,    91,    92,    93,
      -1,    -1,     7,     8,     9,    10,    11,    -1,    -1,    -1,
      -1,    -1,    -1,   107,   108,    -1,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,    -1,
     124,   125,   126,   127,   128,    -1,    -1,     1,   132,     3,
       4,     5,     6,    48,    -1,    -1,    -1,    -1,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    -1,    -1,
      -1,    -1,    -1,    27,    -1,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    -1,    -1,    -1,    -1,    -1,
      44,    45,    46,    47,   615,    -1,    -1,    -1,    -1,    -1,
      -1,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    -1,    66,    67,    68,    -1,    -1,    71,    72,    73,
      74,    -1,   643,    77,    78,    -1,   647,   648,    -1,    -1,
      84,    85,    -1,    -1,    88,    89,    90,    91,    92,    93,
      72,    -1,    -1,    -1,    -1,    77,    78,    -1,    -1,    -1,
      -1,    -1,    84,   107,   108,    -1,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,    -1,
     124,   125,   126,   127,   128,    -1,    -1,     1,   132,     3,
       4,     5,     6,    -1,    -1,    -1,    -1,    -1,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    -1,    -1,
      -1,    -1,    -1,    27,    -1,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    -1,    -1,    -1,    -1,    -1,
      44,    45,    46,    47,    97,    98,    99,   100,   101,   102,
     103,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    -1,    66,    67,    68,    -1,    -1,    71,    72,    73,
      74,    -1,    -1,    77,    78,     1,    -1,    -1,    -1,    -1,
      84,    85,    -1,    -1,    88,    89,    90,    91,    92,    93,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      26,    -1,    -1,   107,   108,    -1,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,    -1,
     124,   125,   126,   127,   128,    -1,    -1,    -1,   132,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    -1,
      66,    67,    68,    -1,    -1,    71,    72,    73,    74,    -1,
      -1,    77,    78,     1,    -1,    -1,    -1,    -1,    84,    -1,
      -1,    -1,    88,    89,    90,    91,    92,    93,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    57,    -1,    -1,
      -1,   107,   108,    -1,    -1,    -1,    -1,    67,    -1,    -1,
      -1,    -1,    -1,    -1,   120,   121,   122,    -1,   124,   125,
     126,   127,   128,    -1,    -1,    -1,   132,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    -1,    66,    67,
      68,    -1,    -1,    71,    72,    73,    74,   107,   108,    77,
      78,     1,    -1,    -1,    -1,    -1,    84,    -1,    -1,    -1,
      88,    89,    90,    91,    92,    93,   126,   127,   128,    -1,
      -1,   131,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,
     108,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   120,   121,   122,    -1,   124,   125,   126,   127,
     128,    -1,    -1,    -1,   132,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    -1,    66,    67,    68,    -1,
      -1,    71,    72,    73,    74,    -1,    -1,    77,    78,     1,
      -1,    -1,    -1,    -1,    84,    -1,    -1,    -1,    88,    89,
      90,    91,    92,    93,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,   108,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     1,
     120,   121,   122,    -1,   124,   125,   126,   127,   128,    -1,
      -1,    -1,   132,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    25,    66,    67,    68,    -1,    -1,    71,
      72,    73,    74,    -1,    -1,    77,    78,    39,    40,    -1,
      -1,    -1,    84,    -1,    -1,    -1,    88,    89,    90,    91,
      92,    93,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   107,   108,    -1,    -1,    71,
      72,    -1,    -1,    -1,    -1,    -1,    78,    -1,   120,   121,
     122,    -1,   124,   125,   126,   127,   128,    -1,    -1,    -1,
     132,     3,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,
      12,    13,    -1,    -1,    16,    17,    18,    19,    20,    21,
      -1,    -1,    -1,    -1,    -1,    27,    -1,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    -1,    -1,    -1,
      -1,    -1,    44,    45,    46,    47,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    -1,    66,    67,    68,    -1,    -1,    71,
      72,    73,    74,    -1,    -1,    77,    78,    -1,    -1,    -1,
      -1,    -1,    84,    85,    -1,    -1,    88,    89,    90,    91,
      92,    93,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   107,   108,    -1,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,    -1,   124,   125,   126,   127,   128,    -1,    -1,    -1,
     132,     3,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,
      12,    13,    -1,    -1,    16,    17,    18,    19,    20,    21,
      -1,    -1,    -1,    -1,    -1,    27,    -1,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    -1,    -1,    -1,
      -1,    -1,    44,    45,    46,    47,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    -1,    66,    67,    68,    -1,    -1,    71,
      72,    73,    74,    -1,    -1,    77,    78,    -1,    -1,    -1,
      -1,    -1,    84,    85,    -1,    -1,    88,    89,    90,    91,
      92,    93,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   107,   108,    -1,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,    -1,   124,   125,   126,   127,   128,    -1,    -1,    -1,
     132,     3,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,
      12,    13,    -1,    -1,    16,    17,    18,    19,    20,    21,
      -1,    -1,    -1,    -1,    -1,    27,    -1,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    -1,    -1,    -1,
      -1,    -1,    44,    -1,    46,    47,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    -1,    66,    67,    68,    -1,    -1,    71,
      72,    73,    74,    -1,    -1,    77,    78,    -1,    -1,    -1,
      -1,    -1,    84,    85,    -1,    -1,    88,    89,    90,    91,
      92,    93,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    25,    -1,    -1,    -1,   107,   108,    -1,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,    -1,   124,   125,   126,   127,   128,    -1,    -1,    -1,
     132,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    -1,    66,    67,    68,    -1,    -1,    71,    72,    73,
      74,    -1,    -1,    77,    78,    -1,    -1,    81,    -1,    -1,
      84,    -1,    -1,    -1,    88,    89,    90,    91,    92,    93,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   107,   108,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   120,   121,   122,    -1,
     124,   125,   126,   127,   128,    48,    -1,    -1,   132,    -1,
      -1,    -1,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    -1,    66,    67,    68,    -1,    -1,    71,    72,
      73,    74,    -1,    -1,    77,    78,    -1,    -1,    -1,    -1,
      -1,    84,    -1,    -1,    -1,    88,    89,    90,    91,    92,
      93,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   107,   108,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   120,   121,   122,
      -1,   124,   125,   126,   127,   128,    48,    -1,    -1,   132,
      -1,    -1,    -1,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    -1,    66,    67,    68,    -1,    -1,    71,
      72,    73,    74,    -1,    -1,    77,    78,    -1,    -1,    -1,
      -1,    -1,    84,    -1,    -1,    -1,    88,    89,    90,    91,
      92,    93,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   107,   108,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   120,   121,
     122,    -1,   124,   125,   126,   127,   128,    49,    -1,    -1,
     132,    -1,    -1,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    -1,    66,    67,    68,    -1,    -1,    71,
      72,    73,    74,    -1,    76,    77,    78,    -1,    -1,    55,
      -1,    -1,    84,    -1,    -1,    -1,    88,    89,    90,    91,
      92,    93,    -1,    -1,    -1,    71,    72,    73,    74,    -1,
      -1,    77,    78,    -1,    -1,   107,   108,    -1,    84,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   120,   121,
     122,    -1,   124,   125,   126,   127,   128,    48,    -1,    -1,
     132,   107,   108,    -1,    -1,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    -1,    66,    67,    68,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   132,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    88,    89,    90,
      91,    92,    93,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    56,    57,   107,   108,    -1,    -1,
      -1,    -1,    -1,    -1,    66,    67,    -1,    -1,    -1,   120,
     121,   122,    -1,   124,   125,   126,   127,   128,    -1,    -1,
     131,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    -1,    66,    67,    68,    -1,    -1,    71,    72,    73,
      74,    -1,    -1,    77,    78,   107,   108,    -1,    -1,    -1,
      84,    -1,    -1,    -1,    88,    89,    90,    91,    92,    93,
      -1,    -1,   124,   125,   126,   127,   128,    -1,    -1,   131,
      65,    -1,    -1,   107,   108,    -1,    71,    -1,    -1,    -1,
      -1,    -1,    77,    78,    -1,    -1,   120,   121,   122,    84,
     124,   125,   126,   127,   128,    -1,    -1,    -1,   132,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    -1,    66,
      67,    68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   122,    -1,   124,
     125,    88,    89,    90,    91,    92,    93,   132,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     107,   108,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   120,   121,   122,    -1,   124,   125,   126,
     127,   128,    -1,    -1,   131,    -1,   133,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    -1,    66,    67,    68,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    88,
      89,    90,    91,    92,    93,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,   108,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   120,   121,   122,    -1,   124,   125,   126,   127,   128,
      -1,    -1,   131,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    -1,    66,    67,    68,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    88,    89,    90,    91,    92,
      93,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   107,   108,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   120,   121,   122,
      -1,   124,   125,   126,   127,   128,    -1,    -1,   131,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    -1,    66,
      67,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      -1,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    88,    89,    90,    91,    92,    93,    -1,    -1,    -1,
      -1,    -1,    -1,    88,    89,    90,    91,    92,    93,    -1,
     107,   108,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   107,   108,   121,   122,    -1,   124,   125,   126,
     127,   128,    -1,    -1,   131,    -1,    -1,   122,    -1,   124,
     125,   126,   127,   128,    -1,    -1,   131
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,   137,   138,     0,   139,   152,   153,   109,   140,   141,
     142,   146,   148,   149,   154,   155,   156,   258,    48,    75,
     151,   171,   237,   238,     1,   143,   141,   146,   140,   148,
       3,     4,     5,     6,    12,    13,    16,    17,    18,    19,
      20,    21,    27,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    44,    45,    46,    47,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    66,    67,    68,
      71,    72,    73,    74,    77,    78,    84,    85,    88,    89,
      90,    91,    92,    93,   107,   108,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   124,
     125,   126,   127,   128,   132,   155,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     172,   173,   174,   175,   176,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,   190,   191,   196,   197,   198,
     199,   229,   230,   231,   232,   233,   234,   235,   236,   239,
     240,   241,   243,   244,   248,   249,   250,   251,   252,   253,
     255,   260,   268,   270,   271,   272,   273,   274,   275,   276,
     277,   278,   279,   280,   281,   282,   283,   284,   285,   286,
     287,   288,   289,   290,   291,   292,   321,   322,   328,   329,
     153,    48,    71,    72,    78,   144,   145,   292,     1,   292,
       1,   292,   293,    14,    15,   151,     1,    39,    40,    71,
      72,    77,    78,    84,   194,   263,   264,   265,   333,    49,
      76,   200,   226,   292,   292,   337,   151,   292,     1,    72,
     340,   340,   337,   337,   187,     1,    69,   246,   321,   330,
     334,    72,   132,   338,   339,   292,     1,     1,    50,    51,
      52,   337,     1,     4,    22,    23,    24,    25,    29,    46,
      79,    80,   105,   106,   245,   247,     1,    28,   330,   337,
     337,     1,   151,     1,    25,    78,   261,   263,   264,   265,
      25,    81,   269,   337,   338,   340,   151,   337,   337,   337,
     337,   337,   337,   337,   337,   337,   337,   337,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    66,    67,    68,
      88,    89,    90,    91,    92,    93,   107,   108,   120,   121,
     122,   124,   125,   126,   127,   128,   131,   323,   324,   323,
     153,    66,   151,    48,   320,   335,   337,   192,     1,    42,
      70,    83,    96,   266,     1,   266,   193,     1,    66,   222,
      10,    11,   228,   122,    66,   222,   152,     1,     1,     1,
      25,    26,    71,    72,    78,   132,   188,   201,   204,   334,
     134,   334,    65,    71,   122,   124,   125,   132,   331,   332,
     333,   339,     1,   338,   292,     1,    25,    53,    54,   242,
     292,   340,     1,   337,   106,   106,    46,   105,     1,   247,
     246,     1,   338,     1,    14,    45,   182,   183,   256,   257,
     292,     1,     1,    41,    43,    87,   266,   267,     1,   267,
     292,     1,     1,     1,   305,   298,   307,   309,   308,   310,
     311,   312,   313,   304,   303,   302,   314,   315,   316,   317,
     318,   319,    72,    78,   325,   328,   329,   325,   300,   301,
     306,   294,   295,   296,   297,   299,   326,   327,   320,   145,
       1,   133,   134,   195,   335,     1,    82,   262,     1,   195,
     292,   292,   151,   292,     1,    14,   189,   292,     1,   205,
     206,   207,   208,   212,   213,   214,   222,   292,   337,   330,
     132,   332,   132,   332,   132,   332,   321,   330,   133,   292,
       1,    26,   246,     1,     1,     1,   292,     1,   183,   257,
     259,     1,   262,     1,   292,   292,   292,   292,   292,   292,
     292,   292,   292,   292,   292,   292,   292,   292,   292,   292,
     292,   292,   323,   323,   292,   292,   292,   292,   292,   292,
     292,   292,   335,   336,   133,     1,    72,    78,     1,   222,
     222,   147,   150,   154,   155,   222,   152,   202,   203,   202,
       1,   209,   213,   214,   222,   210,   212,   214,   222,   211,
     212,   213,   222,    94,    95,    96,   133,   202,   321,   321,
     321,   133,   338,   152,     1,   152,    45,   254,     1,   135,
     335,   154,   155,   198,   155,     7,     8,     9,   227,     1,
     148,   204,   214,   222,   213,   222,   214,   222,   212,   222,
     213,   222,   212,   222,     1,   222,     1,   222,     1,   222,
     222,   133,   133,   133,    14,   150,   340,   292,   292,   292,
     227,    15,   222,   222,   222,   222,   222,   222,    97,   100,
     101,   102,   103,   104,   215,   217,    97,    98,    99,   216,
     217,   216,   202,   152,     1,   222,   222,   222,   227,   152,
       1,   220,   221,     1,   218,   219,     1,   220,     1,   220,
       1,   217,     1,   217,     1,   217,   154,     1,   148,   222,
     222,    14,   223,   224,   225,   333,   339,   224
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   136,   138,   137,   139,   139,   139,   139,   140,   140,
     141,   141,   143,   142,   142,   144,   144,   145,   145,   145,
     146,   146,   147,   147,   148,   148,   149,   150,   150,   151,
     151,   152,   152,   153,   153,   154,   154,   155,   156,   157,
     157,   157,   157,   158,   158,   158,   158,   158,   158,   158,
     158,   158,   158,   158,   158,   158,   158,   158,   158,   158,
     158,   158,   158,   158,   158,   158,   158,   158,   158,   158,
     158,   158,   158,   158,   158,   158,   158,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   187,   186,   188,   188,   188,
     189,   188,   188,   188,   190,   192,   191,   193,   191,   191,
     191,   191,   191,   191,   191,   191,   191,   194,   194,   195,
     195,   196,   197,   198,   198,   198,   199,   200,   200,   200,
     200,   200,   200,   200,   200,   201,   201,   201,   203,   202,
     202,   204,   204,   204,   205,   205,   205,   205,   206,   207,
     208,   209,   209,   209,   209,   209,   210,   210,   210,   210,
     210,   211,   211,   211,   211,   211,   212,   212,   213,   213,
     214,   214,   215,   215,   215,   215,   216,   216,   216,   216,
     216,   216,   217,   217,   217,   217,   217,   217,   217,   217,
     219,   218,   221,   220,   222,   222,   223,   223,   224,   225,
     226,   227,   227,   227,   228,   228,   228,   229,   229,   230,
     230,   231,   232,   232,   232,   232,   232,   232,   233,   234,
     235,   236,   236,   236,   237,   238,   239,   239,   239,   240,
     240,   241,   241,   241,   241,   241,   241,   241,   241,   241,
     241,   242,   242,   243,   244,   244,   244,   244,   245,   245,
     245,   245,   245,   245,   245,   246,   246,   247,   247,   247,
     247,   247,   247,   247,   247,   247,   248,   248,   248,   248,
     248,   249,   250,   251,   252,   253,   254,   254,   254,   255,
     255,   255,   255,   255,   255,   256,   256,   256,   257,   257,
     257,   257,   258,   258,   259,   259,   260,   260,   260,   260,
     260,   260,   260,   260,   260,   260,   260,   261,   261,   261,
     262,   262,   262,   263,   263,   264,   265,   266,   266,   266,
     266,   267,   267,   267,   267,   268,   268,   268,   268,   269,
     270,   271,   272,   273,   274,   275,   276,   277,   278,   279,
     280,   281,   282,   283,   284,   285,   286,   287,   288,   289,
     290,   291,   293,   292,   294,   292,   295,   292,   296,   292,
     292,   297,   292,   292,   298,   292,   292,   299,   292,   292,
     300,   292,   292,   301,   292,   292,   302,   292,   292,   303,
     292,   292,   304,   292,   292,   305,   292,   292,   292,   292,
     306,   292,   292,   307,   292,   292,   308,   292,   292,   309,
     292,   292,   310,   292,   292,   311,   292,   292,   312,   292,
     292,   313,   292,   292,   314,   292,   292,   315,   292,   292,
     316,   292,   292,   317,   292,   292,   318,   292,   292,   319,
     292,   292,   292,   292,   292,   292,   292,   292,   292,   292,
     292,   292,   292,   292,   292,   292,   292,   320,   320,   320,
     321,   322,   322,   324,   323,   325,   325,   325,   325,   327,
     326,   328,   329,   330,   330,   330,   331,   331,   331,   331,
     331,   331,   331,   331,   331,   332,   332,   333,   333,   333,
     334,   334,   334,   336,   335,   335,   337,   337,   338,   338,
     338,   338,   339,   340,   340
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
       1,     2,     2,     0,     2,     1,     1,     2,     2,     0,
       3,     1,     1,     3,     1,     1,     2,     2,     1,     2,
       3,     4,     4,     4,     1,     1,     1,     1,     1,     1,
       2,     2,     0,     0,     4,     1,     1,     0,     2,     1,
       4,     3,     1,     1,     1
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
#line 163 "yaccsrc.y"
                                       { level = 0;
                                         if ( get_options_flag( parser_data.TSD->currlevel, EXT_CALLS_AS_FUNCS )
                                           && !get_options_flag( parser_data.TSD->currlevel, EXT_STRICT_ANSI ) )
                                            start_parendepth = 1;
                                         else
                                            start_parendepth = 0;
                                         parendepth = 0; }
#line 2480 "yaccsrc.c"
    break;

  case 4: /* prog: nlncl stats  */
#line 173 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-1], yyvsp[0] );
                                         yyval->o.last = NULL;
                                         EndProg( yyval ) ; }
#line 2488 "yaccsrc.c"
    break;

  case 5: /* prog: nlncl  */
#line 176 "yaccsrc.y"
                                       { yyval = yyvsp[0];
                                         if ( yyval != NULL )
                                            yyval->o.last = NULL;
                                         EndProg( yyval ); }
#line 2497 "yaccsrc.c"
    break;

  case 6: /* prog: nlncl stats dirsect  */
#line 180 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-2], yyvsp[-1] );
                                         yyval = optgluelast( yyval, yyvsp[0] );
                                         yyval->o.last = NULL;
                                         EndProg( yyval ) ; }
#line 2506 "yaccsrc.c"
    break;

  case 7: /* prog: nlncl dirsect  */
#line 184 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-1], yyvsp[0] );
                                         yyval->o.last = NULL;
                                         EndProg( yyval ) ; }
#line 2514 "yaccsrc.c"
    break;

  case 8: /* dirsect: dirsect dirblock  */
#line 194 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 2520 "yaccsrc.c"
    break;

  case 9: /* dirsect: dirblock  */
#line 195 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2526 "yaccsrc.c"
    break;

  case 10: /* dirblock: directive stats  */
#line 198 "yaccsrc.y"
                                       { yyval = yyvsp[-1];
                                         yyval->p[1] = optgluelast( yyval->p[1], yyvsp[0] );
                                         if ( yyval->p[1] != NULL )
                                            yyval->p[1]->o.last = NULL; }
#line 2535 "yaccsrc.c"
    break;

  case 11: /* dirblock: directive  */
#line 202 "yaccsrc.y"
                                       { yyval = yyvsp[0];
                                         if ( yyval->p[1] != NULL )
                                            yyval->p[1]->o.last = NULL; }
#line 2543 "yaccsrc.c"
    break;

  case 12: /* @2: %empty  */
#line 207 "yaccsrc.y"
                                       { yyval = makenode( X_DIRECTIVE, 0 );
                                         yyval->lineno = parser_data.tline;
                                         yyval->charnr = parser_data.tstart; }
#line 2551 "yaccsrc.c"
    break;

  case 13: /* directive: DCOLON @2 dirwords ncl  */
#line 210 "yaccsrc.y"
                                       { yyval = yyvsp[-2];
                                         /* take over the keyword; the
                                          * node must not own it twice */
                                         yyval->name = yyvsp[-1]->name;
                                         yyvsp[-1]->name = NULL;
                                         yyval->p[0] = yyvsp[-1]->next;
                                         if ( yyval->p[0] != NULL )
                                            yyval->p[0]->o.last = NULL;
                                         yyval->p[1] = yyvsp[0]; /* labels */ }
#line 2565 "yaccsrc.c"
    break;

  case 14: /* directive: DCOLON error  */
#line 219 "yaccsrc.y"
                                       { exiterror( ERR_YACC_SYNTAX, 1, parser_data.tline ); }
#line 2571 "yaccsrc.c"
    break;

  case 15: /* dirwords: dirwords SPACE dirword  */
#line 222 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-2], yyvsp[0] ); }
#line 2577 "yaccsrc.c"
    break;

  case 16: /* dirwords: dirword  */
#line 223 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2583 "yaccsrc.c"
    break;

  case 17: /* dirword: SIMSYMBOL  */
#line 226 "yaccsrc.y"
                                       { yyval = makenode( X_STRING, 0 );
                                         yyval->name = Str_cre_TSD( parser_data.TSD, retvalue ); }
#line 2590 "yaccsrc.c"
    break;

  case 18: /* dirword: CONSYMBOL  */
#line 228 "yaccsrc.y"
                                       { yyval = makenode( X_STRING, 0 );
                                         yyval->name = Str_cre_TSD( parser_data.TSD, retvalue ); }
#line 2597 "yaccsrc.c"
    break;

  case 19: /* dirword: STRING  */
#line 230 "yaccsrc.y"
                                       { yyval = makenode( X_STRING, 0 );
                                         yyval->name = Str_cre_TSD( parser_data.TSD, retvalue ); }
#line 2604 "yaccsrc.c"
    break;

  case 20: /* stats: stats ystatement  */
#line 234 "yaccsrc.y"
                                       { /* fixes bug 579711 */
                                         yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 2611 "yaccsrc.c"
    break;

  case 21: /* stats: ystatement  */
#line 236 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2617 "yaccsrc.c"
    break;

  case 22: /* xstats: xstats statement  */
#line 239 "yaccsrc.y"
                                       { /* fixes bug 579711 */
                                         yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 2624 "yaccsrc.c"
    break;

  case 23: /* xstats: statement gruff  */
#line 241 "yaccsrc.y"
                                       { yyval = yyvsp[-1]; }
#line 2630 "yaccsrc.c"
    break;

  case 24: /* ystatement: statement  */
#line 244 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 2636 "yaccsrc.c"
    break;

  case 25: /* ystatement: lonely_end  */
#line 245 "yaccsrc.y"
                                       { exiterror( ERR_UNMATCHED_END, 1 ); }
#line 2642 "yaccsrc.c"
    break;

  case 27: /* nxstats: xstats  */
#line 253 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2648 "yaccsrc.c"
    break;

  case 28: /* nxstats: gruff  */
#line 254 "yaccsrc.y"
                                       { yyval = NULL; }
#line 2654 "yaccsrc.c"
    break;

  case 29: /* ncl: ncl STATSEP optLabels  */
#line 257 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-2], yyvsp[0] ); }
#line 2660 "yaccsrc.c"
    break;

  case 30: /* ncl: STATSEP optLabels  */
#line 258 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2666 "yaccsrc.c"
    break;

  case 31: /* nlncl: optLabels ncl  */
#line 261 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 2672 "yaccsrc.c"
    break;

  case 32: /* nlncl: optLabels  */
#line 262 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2678 "yaccsrc.c"
    break;

  case 33: /* optLabels: optLabels label_stat  */
#line 265 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 2684 "yaccsrc.c"
    break;

  case 34: /* optLabels: %empty  */
#line 266 "yaccsrc.y"
                                       { yyval = NULL; }
#line 2690 "yaccsrc.c"
    break;

  case 37: /* gruff: %empty  */
#line 273 "yaccsrc.y"
                                       { tmpchr=parser_data.tstart;
                                         tmplno=parser_data.tline; }
#line 2697 "yaccsrc.c"
    break;

  case 38: /* mttstatement: gruff mtstatement  */
#line 277 "yaccsrc.y"
                                       { yyval=yyvsp[0]; }
#line 2703 "yaccsrc.c"
    break;

  case 39: /* mtstatement: nclstatement ncl  */
#line 280 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 2709 "yaccsrc.c"
    break;

  case 78: /* call: CALL  */
#line 323 "yaccsrc.y"
                                       { yyval = makenode(X_CALL,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2717 "yaccsrc.c"
    break;

  case 79: /* do: DO  */
#line 327 "yaccsrc.y"
                                       { yyval = makenode(X_DO,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ;
                                         level++; }
#line 2726 "yaccsrc.c"
    break;

  case 80: /* exit: EXIT  */
#line 332 "yaccsrc.y"
                                       { yyval = makenode(X_EXIT,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2734 "yaccsrc.c"
    break;

  case 81: /* if: IF  */
#line 336 "yaccsrc.y"
                                       { yyval = makenode(X_IF,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ;
                                         level++; }
#line 2743 "yaccsrc.c"
    break;

  case 82: /* iterate: ITERATE  */
#line 341 "yaccsrc.y"
                                       { yyval = makenode(X_ITERATE,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2751 "yaccsrc.c"
    break;

  case 83: /* leave: LEAVE  */
#line 345 "yaccsrc.y"
                                       { yyval = makenode(X_LEAVE,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2759 "yaccsrc.c"
    break;

  case 84: /* say: SAY  */
#line 349 "yaccsrc.y"
                                       { yyval = makenode(X_SAY,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2767 "yaccsrc.c"
    break;

  case 85: /* return: RETURN  */
#line 353 "yaccsrc.y"
                                       { yyval = makenode(X_RETURN,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2775 "yaccsrc.c"
    break;

  case 86: /* address: ADDRESS  */
#line 357 "yaccsrc.y"
                                       { yyval = makenode(X_ADDR_N,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2783 "yaccsrc.c"
    break;

  case 87: /* arg: ARG  */
#line 361 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_ARG,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2791 "yaccsrc.c"
    break;

  case 88: /* drop: DROP  */
#line 365 "yaccsrc.y"
                                       { yyval = makenode(X_DROP,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2799 "yaccsrc.c"
    break;

  case 89: /* interpret: INTERPRET  */
#line 369 "yaccsrc.y"
                                       { yyval = makenode(X_IPRET,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2807 "yaccsrc.c"
    break;

  case 90: /* label: LABEL  */
#line 373 "yaccsrc.y"
                                       { yyval = makenode(X_LABEL,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2815 "yaccsrc.c"
    break;

  case 91: /* nop: NOP  */
#line 377 "yaccsrc.y"
                                       { yyval = makenode(X_NULL,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2823 "yaccsrc.c"
    break;

  case 92: /* numeric: NUMERIC  */
#line 381 "yaccsrc.y"
                                       { yyval = makenode(0,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2831 "yaccsrc.c"
    break;

  case 93: /* options: OPTIONS  */
#line 385 "yaccsrc.y"
                                       { yyval = makenode(X_OPTIONS,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2839 "yaccsrc.c"
    break;

  case 94: /* parse: PARSE  */
#line 389 "yaccsrc.y"
                                       { yyval = makenode(0,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2847 "yaccsrc.c"
    break;

  case 95: /* proc: PROCEDURE  */
#line 393 "yaccsrc.y"
                                       { yyval = makenode(X_PROC,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2855 "yaccsrc.c"
    break;

  case 96: /* pull: PULL  */
#line 397 "yaccsrc.y"
                                       { yyval = makenode(X_PULL,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2863 "yaccsrc.c"
    break;

  case 97: /* push: PUSH  */
#line 401 "yaccsrc.y"
                                       { yyval = makenode(X_PUSH,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2871 "yaccsrc.c"
    break;

  case 98: /* queue: QUEUE  */
#line 405 "yaccsrc.y"
                                       { yyval = makenode(X_QUEUE,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2879 "yaccsrc.c"
    break;

  case 99: /* select: SELECT  */
#line 409 "yaccsrc.y"
                                       { yyval = makenode(X_SELECT,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ;
                                         level++; }
#line 2888 "yaccsrc.c"
    break;

  case 100: /* signal: SIGNAL  */
#line 414 "yaccsrc.y"
                                       { yyval = makenode(X_SIG_LAB,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2896 "yaccsrc.c"
    break;

  case 101: /* when: WHEN  */
#line 418 "yaccsrc.y"
                                       { yyval = makenode(X_WHEN,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2904 "yaccsrc.c"
    break;

  case 102: /* otherwise: OTHERWISE  */
#line 422 "yaccsrc.y"
                                       { yyval = makenode(X_OTHERWISE,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2912 "yaccsrc.c"
    break;

  case 103: /* trace: TRACE  */
#line 426 "yaccsrc.y"
                                       { yyval = makenode(X_TRACE,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2920 "yaccsrc.c"
    break;

  case 104: /* upper: UPPER  */
#line 430 "yaccsrc.y"
                                       { yyval = makenode(X_UPPER_VAR,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2928 "yaccsrc.c"
    break;

  case 105: /* @3: %empty  */
#line 434 "yaccsrc.y"
                                       { yyval = current = yyvsp[0] ; }
#line 2934 "yaccsrc.c"
    break;

  case 107: /* address_stat2: VALUE expr naddr_with  */
#line 438 "yaccsrc.y"
                                       { current->type = X_ADDR_V ;
                                         current->p[0] = yyvsp[-1] ;
                                         current->p[1] = yyvsp[0] ; }
#line 2942 "yaccsrc.c"
    break;

  case 108: /* address_stat2: addr_with  */
#line 441 "yaccsrc.y"
                                       { exiterror( ERR_STRING_EXPECTED, 1, __reginatext ) ;}
#line 2948 "yaccsrc.c"
    break;

  case 109: /* address_stat2: %empty  */
#line 442 "yaccsrc.y"
                                       { current->type = X_ADDR_S ; }
#line 2954 "yaccsrc.c"
    break;

  case 110: /* $@4: %empty  */
#line 443 "yaccsrc.y"
                                       { exiterror( ERR_STRING_EXPECTED, 1, __reginatext ) ;}
#line 2960 "yaccsrc.c"
    break;

  case 112: /* address_stat2: nvir nexpr naddr_with  */
#line 445 "yaccsrc.y"
                                       { current->name = (streng *)yyvsp[-2] ;
                                         current->type = X_ADDR_N ;
                                         current->p[0] = yyvsp[-1] ;
                                         current->p[1] = yyvsp[0] ; }
#line 2969 "yaccsrc.c"
    break;

  case 113: /* address_stat2: '(' expr ')' nspace naddr_with  */
#line 449 "yaccsrc.y"
                                              { current->type = X_ADDR_V ;
                                         current->p[0] = yyvsp[-3] ;
                                         current->p[1] = yyvsp[0] ;
                                         current->u.nonansi = 1 ; }
#line 2978 "yaccsrc.c"
    break;

  case 114: /* arg_stat: arg templs  */
#line 455 "yaccsrc.y"
                                       { yyval = makenode( X_PARSE, 2, yyvsp[-1], yyvsp[0] );
                                         yyval->u.parseflags = PARSE_UPPER;
                                         yyval->lineno = yyvsp[-1]->lineno;
                                         yyval->charnr = yyvsp[-1]->charnr; }
#line 2987 "yaccsrc.c"
    break;

  case 115: /* $@5: %empty  */
#line 461 "yaccsrc.y"
                                       { parendepth = start_parendepth; }
#line 2993 "yaccsrc.c"
    break;

  case 116: /* call_stat: call call_name $@5 call_args  */
#line 462 "yaccsrc.y"
                                       { yyval = yyvsp[-3];
                                         yyval->p[0] = yyvsp[0];
                                         yyval->name = (streng *) yyvsp[-2];
                                         parendepth = 0; }
#line 3002 "yaccsrc.c"
    break;

  case 117: /* $@6: %empty  */
#line 466 "yaccsrc.y"
                                       { parendepth = start_parendepth; }
#line 3008 "yaccsrc.c"
    break;

  case 118: /* call_stat: call string $@6 call_args  */
#line 467 "yaccsrc.y"
                                       { yyval = yyvsp[-3];
                                         yyval->type = X_EX_FUNC;
                                         yyval->p[0] = yyvsp[0];
                                         yyval->name = (streng *) yyvsp[-2];
                                         parendepth = 0; }
#line 3018 "yaccsrc.c"
    break;

  case 119: /* call_stat: call on error  */
#line 472 "yaccsrc.y"
                             { exiterror( ERR_INV_SUBKEYWORD, 1, "ERROR FAILURE HALT NOTREADY", __reginatext ) ;}
#line 3024 "yaccsrc.c"
    break;

  case 120: /* call_stat: call off error  */
#line 473 "yaccsrc.y"
                              { exiterror( ERR_INV_SUBKEYWORD, 2, "ERROR FAILURE HALT NOTREADY", __reginatext ) ;}
#line 3030 "yaccsrc.c"
    break;

  case 121: /* call_stat: call on c_action error  */
#line 474 "yaccsrc.y"
                                      { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 3036 "yaccsrc.c"
    break;

  case 122: /* call_stat: call on c_action namespec error  */
#line 475 "yaccsrc.y"
                                               { exiterror( ERR_STRING_EXPECTED, 3, __reginatext ) ;}
#line 3042 "yaccsrc.c"
    break;

  case 123: /* call_stat: call on c_action namespec  */
#line 477 "yaccsrc.y"
                                       { yyval = yyvsp[-3] ;
                                         yyval->type = X_CALL_SET ;
                                         yyval->p[0] = yyvsp[-2] ;
                                         yyval->name = (streng *)yyvsp[0] ;
                                         yyval->p[1] = yyvsp[-1] ; }
#line 3052 "yaccsrc.c"
    break;

  case 124: /* call_stat: call on c_action  */
#line 482 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_CALL_SET ;
                                         yyval->p[0] = yyvsp[-1] ;
                                         yyval->name = NULL ;
                                         yyval->p[1] = yyvsp[0] ; }
#line 3062 "yaccsrc.c"
    break;

  case 125: /* call_stat: call off c_action error  */
#line 487 "yaccsrc.y"
                                       { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 3068 "yaccsrc.c"
    break;

  case 126: /* call_stat: call off c_action  */
#line 488 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_CALL_SET ;
                                         yyval->p[0] = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ; }
#line 3077 "yaccsrc.c"
    break;

  case 127: /* call_name: asymbol  */
#line 494 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 3083 "yaccsrc.c"
    break;

  case 128: /* call_name: error  */
#line 495 "yaccsrc.y"
                                       { exiterror( ERR_STRING_EXPECTED, 2, __reginatext );}
#line 3089 "yaccsrc.c"
    break;

  case 129: /* call_args: exprs  */
#line 498 "yaccsrc.y"
                                       {
                                         /*
                                          * "call_args" accepted probably with
                                          * surrounding parentheses. Strip them.
                                          */
                                         yyval = reduce_expr_list( yyvsp[0],
                                                                REDUCE_CALL );
                                       }
#line 3102 "yaccsrc.c"
    break;

  case 130: /* call_args: exprs ')'  */
#line 506 "yaccsrc.y"
                                       { exiterror(ERR_UNEXPECTED_PARAN, 2); }
#line 3108 "yaccsrc.c"
    break;

  case 131: /* expr_stat: expr  */
#line 509 "yaccsrc.y"
                                       { yyval = makenode(X_COMMAND,0) ;
                                         yyval->charnr = tmpchr ;
                                         yyval->lineno = tmplno;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3117 "yaccsrc.c"
    break;

  case 132: /* end_stat: END  */
#line 515 "yaccsrc.y"
                                       { yyval = makenode(X_END,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ;
                                         level--; }
#line 3126 "yaccsrc.c"
    break;

  case 133: /* end: end_stat simsymb  */
#line 521 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->name = (streng*)(yyvsp[0]) ; }
#line 3133 "yaccsrc.c"
    break;

  case 134: /* end: end_stat  */
#line 523 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3139 "yaccsrc.c"
    break;

  case 135: /* end: end_stat simsymb error  */
#line 524 "yaccsrc.y"
                                       {  exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 3145 "yaccsrc.c"
    break;

  case 136: /* do_stat: do repetitor conditional ncl nxstats end  */
#line 528 "yaccsrc.y"
                                       { yyval = yyvsp[-5];
                                         yyval->p[0] = yyvsp[-4];
                                         yyval->p[1] = yyvsp[-3];
                                         yyval->p[2] = optgluelast( yyvsp[-2], yyvsp[-1] );
                                         if ( yyval->p[2] )
                                            yyval->p[2]->o.last = NULL;
                                         yyval->p[3] = yyvsp[0];
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
#line 3168 "yaccsrc.c"
    break;

  case 137: /* repetitor: dovar '=' expr nspace tobyfor tobyfor tobyfor  */
#line 549 "yaccsrc.y"
                                       { yyval =makenode(X_REP,4,yyvsp[-4],yyvsp[-2],yyvsp[-1],yyvsp[0]) ;
                                         yyval->name = (streng *)yyvsp[-6] ;
                                         checkdosyntax(yyval) ; }
#line 3176 "yaccsrc.c"
    break;

  case 138: /* repetitor: dovar '=' expr nspace tobyfor tobyfor  */
#line 553 "yaccsrc.y"
                                       { yyval =makenode(X_REP,3,yyvsp[-3],yyvsp[-1],yyvsp[0]) ;
                                         yyval->name = (streng *)yyvsp[-5] ;
                                         checkdosyntax(yyval) ; }
#line 3184 "yaccsrc.c"
    break;

  case 139: /* repetitor: dovar '=' expr nspace tobyfor  */
#line 557 "yaccsrc.y"
                                       { yyval = makenode(X_REP,2,yyvsp[-2],yyvsp[0]) ;
                                         yyval->name = (streng *)yyvsp[-4] ;
                                         checkdosyntax(yyval) ; }
#line 3192 "yaccsrc.c"
    break;

  case 140: /* repetitor: dovar '=' expr nspace  */
#line 560 "yaccsrc.y"
                                       { yyval = makenode(X_REP,1,yyvsp[-1]) ;
                                         yyval->name = (streng *)yyvsp[-3] ;
                                         checkdosyntax(yyval) ; }
#line 3200 "yaccsrc.c"
    break;

  case 141: /* repetitor: FOREVER nspace  */
#line 563 "yaccsrc.y"
                                       { yyval = makenode(X_REP_FOREVER,0) ; }
#line 3206 "yaccsrc.c"
    break;

  case 142: /* repetitor: FOREVER error  */
#line 564 "yaccsrc.y"
                             { exiterror( ERR_INV_SUBKEYWORD, 16, "WHILE UNTIL", __reginatext ) ; }
#line 3212 "yaccsrc.c"
    break;

  case 143: /* repetitor: expr nspace  */
#line 565 "yaccsrc.y"
                                       { yyvsp[-1] = makenode(X_DO_EXPR,1,yyvsp[-1]) ;
                                         yyval = makenode(X_REP,2,NULL,yyvsp[-1]) ; }
#line 3219 "yaccsrc.c"
    break;

  case 144: /* repetitor: %empty  */
#line 567 "yaccsrc.y"
                                       { yyval = NULL ; }
#line 3225 "yaccsrc.c"
    break;

  case 145: /* nvir: CONSYMBOL  */
#line 570 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 3231 "yaccsrc.c"
    break;

  case 146: /* nvir: SIMSYMBOL  */
#line 571 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 3237 "yaccsrc.c"
    break;

  case 147: /* nvir: STRING  */
#line 572 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 3243 "yaccsrc.c"
    break;

  case 148: /* @7: %empty  */
#line 575 "yaccsrc.y"
                                       { SymbolDetect |= SD_ADDRWITH ;
                                         yyval = with = makenode(X_ADDR_WITH,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3252 "yaccsrc.c"
    break;

  case 149: /* naddr_with: @7 addr_with  */
#line 579 "yaccsrc.y"
                                       { with = NULL ;
                                         SymbolDetect &= ~SD_ADDRWITH ; }
#line 3259 "yaccsrc.c"
    break;

  case 150: /* naddr_with: %empty  */
#line 581 "yaccsrc.y"
                                       { yyval = NULL ; }
#line 3265 "yaccsrc.c"
    break;

  case 151: /* addr_with: WITH connection  */
#line 584 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 3271 "yaccsrc.c"
    break;

  case 152: /* addr_with: WITH connection error  */
#line 585 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 5, __reginatext ) ; }
#line 3277 "yaccsrc.c"
    break;

  case 153: /* addr_with: WITH nspace  */
#line 586 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 5, __reginatext ) ; }
#line 3283 "yaccsrc.c"
    break;

  case 157: /* connection: error  */
#line 592 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 5, __reginatext ) ; }
#line 3289 "yaccsrc.c"
    break;

  case 176: /* inputstmt: nspace INPUT nspace resourcei  */
#line 628 "yaccsrc.y"
                                             { with->p[0] = yyvsp[0]; }
#line 3295 "yaccsrc.c"
    break;

  case 177: /* inputstmt: nspace INPUT error  */
#line 629 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 6, __reginatext ) ; }
#line 3301 "yaccsrc.c"
    break;

  case 178: /* outputstmt: nspace OUTPUT nspace resourceo  */
#line 632 "yaccsrc.y"
                                              { with->p[1] = yyvsp[0]; }
#line 3307 "yaccsrc.c"
    break;

  case 179: /* outputstmt: nspace OUTPUT error  */
#line 633 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 7, __reginatext ) ; }
#line 3313 "yaccsrc.c"
    break;

  case 180: /* errorstmt: nspace ERROR nspace resourceo  */
#line 636 "yaccsrc.y"
                                             { with->p[2] = yyvsp[0]; }
#line 3319 "yaccsrc.c"
    break;

  case 181: /* errorstmt: nspace ERROR error  */
#line 637 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 14, __reginatext ) ; }
#line 3325 "yaccsrc.c"
    break;

  case 182: /* resourcei: resources  */
#line 640 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3331 "yaccsrc.c"
    break;

  case 183: /* resourcei: NOEOL resources  */
#line 641 "yaccsrc.y"
                                       { yyval = yyvsp[0] ;
                                         yyval->u.of.noeol = 1 ; }
#line 3338 "yaccsrc.c"
    break;

  case 184: /* resourcei: NOEOL error  */
#line 643 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 8, __reginatext ) ; }
#line 3344 "yaccsrc.c"
    break;

  case 185: /* resourcei: NORMAL  */
#line 644 "yaccsrc.y"
                                       { yyval = makenode(X_ADDR_WITH, 0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3352 "yaccsrc.c"
    break;

  case 186: /* resourceo: resources  */
#line 649 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3358 "yaccsrc.c"
    break;

  case 187: /* resourceo: APPEND resources  */
#line 650 "yaccsrc.y"
                                       { yyval = yyvsp[0] ;
                                         yyval->u.of.append = 1 ; }
#line 3365 "yaccsrc.c"
    break;

  case 188: /* resourceo: APPEND error  */
#line 652 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 8, __reginatext ) ; }
#line 3371 "yaccsrc.c"
    break;

  case 189: /* resourceo: REPLACE resources  */
#line 653 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3377 "yaccsrc.c"
    break;

  case 190: /* resourceo: REPLACE error  */
#line 654 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 9, __reginatext ) ; }
#line 3383 "yaccsrc.c"
    break;

  case 191: /* resourceo: NORMAL  */
#line 655 "yaccsrc.y"
                                       { yyval = makenode(X_ADDR_WITH, 0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3391 "yaccsrc.c"
    break;

  case 192: /* resources: STREAM nnvir  */
#line 660 "yaccsrc.y"
                                       { /* ANSI extension: nsimsymb is
                                          * used by the standard, but I think
                                          * there are no reasons why using
                                          * it here as a must. FGC
                                          */
                                         yyval = yyvsp[0] ;
                                         yyval->u.of.awt = awtSTREAM;
                                         SymbolDetect |= SD_ADDRWITH ; }
#line 3404 "yaccsrc.c"
    break;

  case 193: /* resources: STREAM error  */
#line 668 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_OPTION, 1, __reginatext ) ; }
#line 3410 "yaccsrc.c"
    break;

  case 194: /* resources: STEM nsimsymb  */
#line 669 "yaccsrc.y"
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
#line 3428 "yaccsrc.c"
    break;

  case 195: /* resources: STEM error  */
#line 682 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_OPTION, 2, __reginatext ) ; }
#line 3434 "yaccsrc.c"
    break;

  case 196: /* resources: LIFO nnvir  */
#line 683 "yaccsrc.y"
                                       {
                                         yyval = yyvsp[0] ;
                                         yyval->u.of.awt = awtLIFO ;
                                         SymbolDetect |= SD_ADDRWITH ; }
#line 3443 "yaccsrc.c"
    break;

  case 197: /* resources: LIFO error  */
#line 687 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_OPTION, 100, __reginatext ) ; }
#line 3449 "yaccsrc.c"
    break;

  case 198: /* resources: FIFO nnvir  */
#line 688 "yaccsrc.y"
                                       {
                                         yyval = yyvsp[0] ;
                                         yyval->u.of.awt = awtFIFO ;
                                         SymbolDetect |= SD_ADDRWITH ; }
#line 3458 "yaccsrc.c"
    break;

  case 199: /* resources: FIFO error  */
#line 692 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_OPTION, 101, __reginatext ) ; }
#line 3464 "yaccsrc.c"
    break;

  case 200: /* $@8: %empty  */
#line 695 "yaccsrc.y"
                                       { SymbolDetect &= ~SD_ADDRWITH ; }
#line 3470 "yaccsrc.c"
    break;

  case 201: /* nsimsymb: $@8 nspace addrSim  */
#line 696 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3476 "yaccsrc.c"
    break;

  case 202: /* $@9: %empty  */
#line 699 "yaccsrc.y"
                                       { SymbolDetect &= ~SD_ADDRWITH ; }
#line 3482 "yaccsrc.c"
    break;

  case 203: /* nnvir: $@9 nspace addrAll  */
#line 700 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3488 "yaccsrc.c"
    break;

  case 206: /* addrAll: addrSim  */
#line 707 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3494 "yaccsrc.c"
    break;

  case 207: /* addrAll: addrString  */
#line 708 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3500 "yaccsrc.c"
    break;

  case 208: /* addrSim: xsimsymb  */
#line 711 "yaccsrc.y"
                                       { yyval = makenode(X_ADDR_WITH, 0) ;
                                         yyval->name = (streng *) yyvsp[0] ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->u.of.ant = antSIMSYMBOL;
                                       }
#line 3511 "yaccsrc.c"
    break;

  case 209: /* addrString: string  */
#line 719 "yaccsrc.y"
                                       { yyval = makenode(X_ADDR_WITH, 0) ;
                                         yyval->name = (streng *) yyvsp[0] ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->u.of.ant = antSTRING;
                                       }
#line 3522 "yaccsrc.c"
    break;

  case 210: /* dovar: DOVARIABLE  */
#line 727 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 3528 "yaccsrc.c"
    break;

  case 211: /* tobyfor: TO expr nspace  */
#line 730 "yaccsrc.y"
                                       { yyval = makenode(X_DO_TO,1,yyvsp[-1]) ; }
#line 3534 "yaccsrc.c"
    break;

  case 212: /* tobyfor: FOR expr nspace  */
#line 731 "yaccsrc.y"
                                       { yyval = makenode(X_DO_FOR,1,yyvsp[-1]) ; }
#line 3540 "yaccsrc.c"
    break;

  case 213: /* tobyfor: BY expr nspace  */
#line 732 "yaccsrc.y"
                                       { yyval = makenode(X_DO_BY,1,yyvsp[-1]) ; }
#line 3546 "yaccsrc.c"
    break;

  case 214: /* conditional: WHILE expr nspace  */
#line 735 "yaccsrc.y"
                                       { yyval = makenode(X_WHILE,1,yyvsp[-1]) ; }
#line 3552 "yaccsrc.c"
    break;

  case 215: /* conditional: UNTIL expr nspace  */
#line 736 "yaccsrc.y"
                                       { yyval = makenode(X_UNTIL,1,yyvsp[-1]) ; }
#line 3558 "yaccsrc.c"
    break;

  case 216: /* conditional: %empty  */
#line 737 "yaccsrc.y"
                                       { yyval = NULL ; }
#line 3564 "yaccsrc.c"
    break;

  case 217: /* drop_stat: drop anyvars error  */
#line 740 "yaccsrc.y"
                                  { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ) ;}
#line 3570 "yaccsrc.c"
    break;

  case 218: /* drop_stat: drop anyvars  */
#line 741 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3577 "yaccsrc.c"
    break;

  case 219: /* upper_stat: upper anyvars error  */
#line 745 "yaccsrc.y"
                                   { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ) ;}
#line 3583 "yaccsrc.c"
    break;

  case 220: /* upper_stat: upper anyvars  */
#line 746 "yaccsrc.y"
                                        { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3590 "yaccsrc.c"
    break;

  case 221: /* exit_stat: exit nexpr  */
#line 750 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3597 "yaccsrc.c"
    break;

  case 222: /* if_stat: if expr nlncl THEN nlncl ystatement  */
#line 755 "yaccsrc.y"
                                       { move_labels( yyvsp[-5], yyvsp[0], level - 1 );
                                         yyval = yyvsp[-5];
                                         yyval->p[0] = optgluelast( yyvsp[-4], yyvsp[-3] );
                                         yyval->p[0]->o.last = NULL;
                                         yyval->p[1] = optgluelast( yyvsp[-1], yyvsp[0] );
                                         yyval->p[1]->o.last = NULL;
                                         level--; }
#line 3609 "yaccsrc.c"
    break;

  case 223: /* if_stat: if expr nlncl THEN nlncl ystatement ELSE nlncl ystatement  */
#line 763 "yaccsrc.y"
                                       { move_labels( yyvsp[-8], yyvsp[0], level - 1 );
                                         yyval = yyvsp[-8];
                                         yyval->p[0] = optgluelast( yyvsp[-7], yyvsp[-6] );
                                         yyval->p[0]->o.last = NULL;
                                         yyval->p[1] = optgluelast( yyvsp[-4], yyvsp[-3] );
                                         yyval->p[1]->o.last = NULL;
                                         yyval->p[2] = optgluelast( yyvsp[-1], yyvsp[0] );
                                         yyval->p[2]->o.last = NULL;
                                         level--; }
#line 3623 "yaccsrc.c"
    break;

  case 224: /* if_stat: if expr nlncl THEN nlncl ystatement ELSE nlncl error  */
#line 773 "yaccsrc.y"
                                       {  exiterror( ERR_INCOMPLETE_STRUCT, 4 ) ;}
#line 3629 "yaccsrc.c"
    break;

  case 225: /* if_stat: if expr nlncl THEN nlncl error  */
#line 775 "yaccsrc.y"
                                       {  exiterror( ERR_INCOMPLETE_STRUCT, 3 ) ;}
#line 3635 "yaccsrc.c"
    break;

  case 226: /* if_stat: if ncl  */
#line 776 "yaccsrc.y"
                                       {  exiterror( ERR_INCOMPLETE_STRUCT, 0 ) ;}
#line 3641 "yaccsrc.c"
    break;

  case 227: /* if_stat: if expr nlncl error  */
#line 777 "yaccsrc.y"
                                       {  exiterror( ERR_THEN_EXPECTED, 1, parser_data.if_linenr, __reginatext )  ; }
#line 3647 "yaccsrc.c"
    break;

  case 228: /* unexp_then: gruff THEN  */
#line 780 "yaccsrc.y"
                                       {  exiterror( ERR_THEN_UNEXPECTED, 1 )  ; }
#line 3653 "yaccsrc.c"
    break;

  case 229: /* unexp_else: gruff ELSE  */
#line 783 "yaccsrc.y"
                                       {  exiterror( ERR_THEN_UNEXPECTED, 2 )  ; }
#line 3659 "yaccsrc.c"
    break;

  case 230: /* ipret_stat: interpret expr  */
#line 786 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3666 "yaccsrc.c"
    break;

  case 231: /* iterate_stat: iterate simsymb  */
#line 791 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 3673 "yaccsrc.c"
    break;

  case 232: /* iterate_stat: iterate simsymb error  */
#line 793 "yaccsrc.y"
                                     { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 3679 "yaccsrc.c"
    break;

  case 233: /* iterate_stat: iterate  */
#line 794 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3685 "yaccsrc.c"
    break;

  case 234: /* label_stat: labelname  */
#line 797 "yaccsrc.y"
                                       { yyval = yyvsp[0] ;
                                         yyval->u.trace_only =
                                                         (level == 0) ? 0 : 1;
                                         newlabel( (const tsd_t *)parser_data.TSD,
                                                   &parser_data,
                                                   yyvsp[0] ) ; }
#line 3696 "yaccsrc.c"
    break;

  case 235: /* labelname: label  */
#line 805 "yaccsrc.y"
                                       { yyval = yyvsp[0] ;
                                         yyval->name = Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 3703 "yaccsrc.c"
    break;

  case 236: /* leave_stat: leave simsymb  */
#line 809 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 3710 "yaccsrc.c"
    break;

  case 237: /* leave_stat: leave simsymb error  */
#line 811 "yaccsrc.y"
                                   { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 3716 "yaccsrc.c"
    break;

  case 238: /* leave_stat: leave  */
#line 812 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3722 "yaccsrc.c"
    break;

  case 239: /* nop_stat: nop  */
#line 815 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3728 "yaccsrc.c"
    break;

  case 240: /* nop_stat: nop error  */
#line 816 "yaccsrc.y"
                         {  exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 3734 "yaccsrc.c"
    break;

  case 241: /* numeric_stat: numeric DIGITS expr  */
#line 819 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_NUM_D ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3742 "yaccsrc.c"
    break;

  case 242: /* numeric_stat: numeric DIGITS  */
#line 822 "yaccsrc.y"
                                       { yyval = yyvsp[-1]; yyval->type = X_NUM_DDEF ; }
#line 3748 "yaccsrc.c"
    break;

  case 243: /* numeric_stat: numeric FORM form_expr error  */
#line 823 "yaccsrc.y"
                                            { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 3754 "yaccsrc.c"
    break;

  case 244: /* numeric_stat: numeric FORM form_expr  */
#line 824 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_NUM_F ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3762 "yaccsrc.c"
    break;

  case 245: /* numeric_stat: numeric FORM  */
#line 827 "yaccsrc.y"
                                       { /* NOTE! This clashes ANSI! */
                                         yyval = yyvsp[-1] ; yyval->type=X_NUM_FRMDEF ;}
#line 3769 "yaccsrc.c"
    break;

  case 246: /* numeric_stat: numeric FORM VALUE expr  */
#line 829 "yaccsrc.y"
                                       { yyval = yyvsp[-3] ; yyval->type=X_NUM_V ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3776 "yaccsrc.c"
    break;

  case 247: /* numeric_stat: numeric FORM error  */
#line 831 "yaccsrc.y"
                                  { exiterror( ERR_INV_SUBKEYWORD, 11, "ENGINEERING SCIENTIFIC", __reginatext ) ;}
#line 3782 "yaccsrc.c"
    break;

  case 248: /* numeric_stat: numeric FUZZ  */
#line 832 "yaccsrc.y"
                                       { yyval = yyvsp[-1]; yyval->type = X_NUM_FDEF ;}
#line 3788 "yaccsrc.c"
    break;

  case 249: /* numeric_stat: numeric FUZZ expr  */
#line 833 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_NUM_FUZZ ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3796 "yaccsrc.c"
    break;

  case 250: /* numeric_stat: numeric error  */
#line 836 "yaccsrc.y"
                             { exiterror( ERR_INV_SUBKEYWORD, 15, "DIGITS FORM FUZZ", __reginatext ) ;}
#line 3802 "yaccsrc.c"
    break;

  case 251: /* form_expr: SCIENTIFIC  */
#line 839 "yaccsrc.y"
                                       { yyval = makenode(X_NUM_SCI,0) ; }
#line 3808 "yaccsrc.c"
    break;

  case 252: /* form_expr: ENGINEERING  */
#line 840 "yaccsrc.y"
                                       { yyval = makenode(X_NUM_ENG,0) ; }
#line 3814 "yaccsrc.c"
    break;

  case 253: /* options_stat: options nexpr  */
#line 843 "yaccsrc.y"
                                       { (yyval=yyvsp[-1])->p[0]=yyvsp[0] ; }
#line 3820 "yaccsrc.c"
    break;

  case 254: /* parse_stat: parse parse_flags parse_param templs  */
#line 847 "yaccsrc.y"
                                       { yyval = yyvsp[-3] ;
                                         yyval->type = X_PARSE ;
                                         yyval->u.parseflags = (int) yyvsp[-2] ;
                                         yyval->p[0] = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ; }
#line 3830 "yaccsrc.c"
    break;

  case 255: /* parse_stat: parse parse_param templs  */
#line 853 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_PARSE ;
                                         yyval->u.parseflags = 0;
                                         yyval->p[0] = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ; }
#line 3840 "yaccsrc.c"
    break;

  case 256: /* parse_stat: parse parse_flags error  */
#line 858 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 12, "ARG EXTERNAL LINEIN PULL SOURCE VAR VALUE VERSION", __reginatext ) ;}
#line 3846 "yaccsrc.c"
    break;

  case 257: /* parse_stat: parse error  */
#line 859 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 12, "ARG CASELESS EXTERNAL LINEIN LOWER PULL SOURCE UPPER VAR VALUE VERSION", __reginatext ) ;}
#line 3852 "yaccsrc.c"
    break;

  case 258: /* parse_flags: UPPER  */
#line 862 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_UPPER  |
                                                         PARSE_NORMAL); }
#line 3859 "yaccsrc.c"
    break;

  case 259: /* parse_flags: UPPER CASELESS  */
#line 864 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_UPPER  |
                                                         PARSE_CASELESS); }
#line 3866 "yaccsrc.c"
    break;

  case 260: /* parse_flags: CASELESS UPPER  */
#line 866 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_UPPER  |
                                                         PARSE_CASELESS); }
#line 3873 "yaccsrc.c"
    break;

  case 261: /* parse_flags: LOWER  */
#line 868 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_LOWER  |
                                                         PARSE_NORMAL); }
#line 3880 "yaccsrc.c"
    break;

  case 262: /* parse_flags: LOWER CASELESS  */
#line 870 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_LOWER  |
                                                         PARSE_CASELESS); }
#line 3887 "yaccsrc.c"
    break;

  case 263: /* parse_flags: CASELESS LOWER  */
#line 872 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_LOWER  |
                                                         PARSE_CASELESS); }
#line 3894 "yaccsrc.c"
    break;

  case 264: /* parse_flags: CASELESS  */
#line 874 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_NORMAL |
                                                         PARSE_CASELESS); }
#line 3901 "yaccsrc.c"
    break;

  case 265: /* templs: templs ',' template  */
#line 878 "yaccsrc.y"
                                       { /* fixes bugs like bug 579711 */
                                         yyval = optgluelast( yyvsp[-2], yyvsp[0] ); }
#line 3908 "yaccsrc.c"
    break;

  case 266: /* templs: template  */
#line 880 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3914 "yaccsrc.c"
    break;

  case 267: /* parse_param: ARG  */
#line 883 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_ARG,0) ; }
#line 3920 "yaccsrc.c"
    break;

  case 268: /* parse_param: LINEIN  */
#line 884 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_EXT,0) ; }
#line 3926 "yaccsrc.c"
    break;

  case 269: /* parse_param: EXTERNAL  */
#line 885 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_EXT,0) ; }
#line 3932 "yaccsrc.c"
    break;

  case 270: /* parse_param: VERSION  */
#line 886 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_VER,0) ; }
#line 3938 "yaccsrc.c"
    break;

  case 271: /* parse_param: PULL  */
#line 887 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_PULL,0) ; }
#line 3944 "yaccsrc.c"
    break;

  case 272: /* parse_param: SOURCE  */
#line 888 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_SRC,0) ; }
#line 3950 "yaccsrc.c"
    break;

  case 273: /* parse_param: VAR simsymb  */
#line 889 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_VAR,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 3957 "yaccsrc.c"
    break;

  case 274: /* parse_param: VALUE nexpr WITH  */
#line 891 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_VAL,1,yyvsp[-1]) ; }
#line 3963 "yaccsrc.c"
    break;

  case 275: /* parse_param: VALUE error  */
#line 892 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_TEMPLATE, 3 ) ;}
#line 3969 "yaccsrc.c"
    break;

  case 276: /* proc_stat: proc  */
#line 895 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3975 "yaccsrc.c"
    break;

  case 277: /* proc_stat: proc error  */
#line 896 "yaccsrc.y"
                          { exiterror( ERR_INV_SUBKEYWORD, 17, __reginatext ) ;}
#line 3981 "yaccsrc.c"
    break;

  case 278: /* proc_stat: proc EXPOSE error  */
#line 897 "yaccsrc.y"
                                 { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ) ;}
#line 3987 "yaccsrc.c"
    break;

  case 279: /* proc_stat: proc EXPOSE anyvars error  */
#line 898 "yaccsrc.y"
                                         { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ) ;}
#line 3993 "yaccsrc.c"
    break;

  case 280: /* proc_stat: proc EXPOSE anyvars  */
#line 899 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4000 "yaccsrc.c"
    break;

  case 281: /* pull_stat: pull template  */
#line 903 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4007 "yaccsrc.c"
    break;

  case 282: /* push_stat: push nexpr  */
#line 907 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4014 "yaccsrc.c"
    break;

  case 283: /* queue_stat: queue nexpr  */
#line 911 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4021 "yaccsrc.c"
    break;

  case 284: /* say_stat: say nexpr  */
#line 915 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4028 "yaccsrc.c"
    break;

  case 285: /* return_stat: return nexpr  */
#line 919 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4035 "yaccsrc.c"
    break;

  case 286: /* sel_end: END simsymb  */
#line 923 "yaccsrc.y"
                                       { exiterror( ERR_UNMATCHED_END, 0 ) ;}
#line 4041 "yaccsrc.c"
    break;

  case 287: /* sel_end: END simsymb error  */
#line 924 "yaccsrc.y"
                                       { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 4047 "yaccsrc.c"
    break;

  case 288: /* sel_end: END  */
#line 925 "yaccsrc.y"
                                       { level--; }
#line 4053 "yaccsrc.c"
    break;

  case 289: /* select_stat: select ncl when_stats otherwise_stat sel_end  */
#line 929 "yaccsrc.y"
                                       { yyval = yyvsp[-4];
                                         yyval->p[0] = optgluelast( yyvsp[-3], yyvsp[-2] );
                                         yyval->p[0]->o.last = NULL;
                                         yyval->p[1] = yyvsp[-1]; }
#line 4062 "yaccsrc.c"
    break;

  case 290: /* select_stat: select ncl END error  */
#line 933 "yaccsrc.y"
                                       {  exiterror( ERR_WHEN_EXPECTED, 1, parser_data.select_linenr, __reginatext ) ;}
#line 4068 "yaccsrc.c"
    break;

  case 291: /* select_stat: select ncl otherwise error  */
#line 935 "yaccsrc.y"
                                       {  exiterror( ERR_WHEN_EXPECTED, 2, parser_data.select_linenr, __reginatext ) ;}
#line 4074 "yaccsrc.c"
    break;

  case 292: /* select_stat: select error  */
#line 936 "yaccsrc.y"
                                       {  exiterror( ERR_EXTRA_DATA, 1, __reginatext )  ;}
#line 4080 "yaccsrc.c"
    break;

  case 293: /* select_stat: select ncl THEN  */
#line 937 "yaccsrc.y"
                                       {  exiterror( ERR_THEN_UNEXPECTED, 0 ) ;}
#line 4086 "yaccsrc.c"
    break;

  case 294: /* select_stat: select ncl when_stats otherwise error  */
#line 939 "yaccsrc.y"
                                       {  exiterror( ERR_INCOMPLETE_STRUCT, 0 ) ;}
#line 4092 "yaccsrc.c"
    break;

  case 295: /* when_stats: when_stats when_stat  */
#line 942 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 4098 "yaccsrc.c"
    break;

  case 296: /* when_stats: when_stat  */
#line 943 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 4104 "yaccsrc.c"
    break;

  case 297: /* when_stats: error  */
#line 944 "yaccsrc.y"
                                       {  exiterror( ERR_WHEN_EXPECTED, 2, parser_data.select_linenr, __reginatext )  ;}
#line 4110 "yaccsrc.c"
    break;

  case 298: /* when_stat: when expr nlncl THEN nlncl statement  */
#line 948 "yaccsrc.y"
                                       { yyval = yyvsp[-5]; /* fixes bugs like bug 579711 */
                                         yyval->p[0] = optgluelast( yyvsp[-4], yyvsp[-3] );
                                         yyval->p[0]->o.last = NULL;
                                         yyval->p[1] = optgluelast( yyvsp[-1], yyvsp[0] );
                                         yyval->p[1]->o.last = NULL; }
#line 4120 "yaccsrc.c"
    break;

  case 299: /* when_stat: when expr nlncl THEN nlncl statement THEN  */
#line 954 "yaccsrc.y"
                                       {  exiterror( ERR_THEN_UNEXPECTED, 0 ) ;}
#line 4126 "yaccsrc.c"
    break;

  case 300: /* when_stat: when expr  */
#line 955 "yaccsrc.y"
                                       {  exiterror( ERR_THEN_EXPECTED, 2, parser_data.select_linenr, __reginatext )  ; }
#line 4132 "yaccsrc.c"
    break;

  case 301: /* when_stat: when error  */
#line 956 "yaccsrc.y"
                                       {  exiterror( ERR_INVALID_EXPRESSION, 0 ) ;}
#line 4138 "yaccsrc.c"
    break;

  case 302: /* ex_when_stat: gruff when  */
#line 966 "yaccsrc.y"
                                       { exiterror( ERR_WHEN_UNEXPECTED, 1 ); }
#line 4144 "yaccsrc.c"
    break;

  case 303: /* ex_when_stat: gruff otherwise  */
#line 967 "yaccsrc.y"
                                       { exiterror( ERR_WHEN_UNEXPECTED, 2 ); }
#line 4150 "yaccsrc.c"
    break;

  case 304: /* otherwise_stat: otherwise nlncl nxstats  */
#line 970 "yaccsrc.y"
                                         { yyval = yyvsp[-2];
                                         yyval->p[0] = optgluelast( yyvsp[-1], yyvsp[0] );
                                         if ( yyval->p[0] )
                                            yyval->p[0]->o.last = NULL; }
#line 4159 "yaccsrc.c"
    break;

  case 305: /* otherwise_stat: %empty  */
#line 974 "yaccsrc.y"
                                       { yyval = makenode(X_NO_OTHERWISE,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 4167 "yaccsrc.c"
    break;

  case 306: /* signal_stat: signal VALUE expr  */
#line 980 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_SIG_VAL ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4175 "yaccsrc.c"
    break;

  case 307: /* signal_stat: signal signal_name error  */
#line 983 "yaccsrc.y"
                                       { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 4181 "yaccsrc.c"
    break;

  case 308: /* signal_stat: signal signal_name  */
#line 984 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->name = (streng *)yyvsp[0] ; }
#line 4188 "yaccsrc.c"
    break;

  case 309: /* signal_stat: signal on error  */
#line 986 "yaccsrc.y"
                              { exiterror( ERR_INV_SUBKEYWORD, 3, "ERROR FAILURE HALT NOTREADY NOVALUE SYNTAX LOSTDIGITS", __reginatext ) ;}
#line 4194 "yaccsrc.c"
    break;

  case 310: /* signal_stat: signal off error  */
#line 987 "yaccsrc.y"
                               { exiterror( ERR_INV_SUBKEYWORD, 4, "ERROR FAILURE HALT NOTREADY NOVALUE SYNTAX LOSTDIGITS", __reginatext ) ;}
#line 4200 "yaccsrc.c"
    break;

  case 311: /* signal_stat: signal on s_action error  */
#line 988 "yaccsrc.y"
                                       { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 4206 "yaccsrc.c"
    break;

  case 312: /* signal_stat: signal on s_action namespec error  */
#line 989 "yaccsrc.y"
                                                { exiterror( ERR_STRING_EXPECTED, 3, __reginatext ) ;}
#line 4212 "yaccsrc.c"
    break;

  case 313: /* signal_stat: signal on s_action namespec  */
#line 991 "yaccsrc.y"
                                       { yyval = yyvsp[-3] ;
                                         yyval->type = X_SIG_SET ;
                                         yyval->p[0] = yyvsp[-2] ;
                                         yyval->name = (streng *)yyvsp[0] ;
                                         yyval->p[1] = yyvsp[-1] ; }
#line 4222 "yaccsrc.c"
    break;

  case 314: /* signal_stat: signal on s_action  */
#line 996 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_SIG_SET ;
                                         yyval->p[0] = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ; }
#line 4231 "yaccsrc.c"
    break;

  case 315: /* signal_stat: signal off s_action error  */
#line 1000 "yaccsrc.y"
                                        { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 4237 "yaccsrc.c"
    break;

  case 316: /* signal_stat: signal off s_action  */
#line 1001 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_SIG_SET ;
                                         yyval->p[0] = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ; }
#line 4246 "yaccsrc.c"
    break;

  case 317: /* signal_name: asymbol  */
#line 1007 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 4252 "yaccsrc.c"
    break;

  case 318: /* signal_name: STRING  */
#line 1008 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 4258 "yaccsrc.c"
    break;

  case 319: /* signal_name: error  */
#line 1009 "yaccsrc.y"
                                       { exiterror( ERR_STRING_EXPECTED, 4, __reginatext );}
#line 4264 "yaccsrc.c"
    break;

  case 320: /* namespec: NAME SIMSYMBOL  */
#line 1012 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue);}
#line 4270 "yaccsrc.c"
    break;

  case 321: /* namespec: NAME STRING  */
#line 1013 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 4276 "yaccsrc.c"
    break;

  case 322: /* namespec: NAME error  */
#line 1014 "yaccsrc.y"
                                       { exiterror( ERR_STRING_EXPECTED, 3, __reginatext ) ;}
#line 4282 "yaccsrc.c"
    break;

  case 323: /* asymbol: CONSYMBOL  */
#line 1017 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 4288 "yaccsrc.c"
    break;

  case 324: /* asymbol: SIMSYMBOL  */
#line 1018 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 4294 "yaccsrc.c"
    break;

  case 325: /* on: ON  */
#line 1021 "yaccsrc.y"
                                       { yyval = makenode(X_ON,0) ; }
#line 4300 "yaccsrc.c"
    break;

  case 326: /* off: OFF  */
#line 1024 "yaccsrc.y"
                                       { yyval = makenode(X_OFF,0) ; }
#line 4306 "yaccsrc.c"
    break;

  case 327: /* c_action: ERROR  */
#line 1027 "yaccsrc.y"
                                       { yyval = makenode(X_S_ERROR,0) ; }
#line 4312 "yaccsrc.c"
    break;

  case 328: /* c_action: HALT  */
#line 1028 "yaccsrc.y"
                                       { yyval = makenode(X_S_HALT,0) ; }
#line 4318 "yaccsrc.c"
    break;

  case 329: /* c_action: NOTREADY  */
#line 1029 "yaccsrc.y"
                                       { yyval = makenode(X_S_NOTREADY,0) ; }
#line 4324 "yaccsrc.c"
    break;

  case 330: /* c_action: FAILURE  */
#line 1030 "yaccsrc.y"
                                       { yyval = makenode(X_S_FAILURE,0) ; }
#line 4330 "yaccsrc.c"
    break;

  case 331: /* s_action: c_action  */
#line 1033 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 4336 "yaccsrc.c"
    break;

  case 332: /* s_action: NOVALUE  */
#line 1034 "yaccsrc.y"
                                       { yyval = makenode(X_S_NOVALUE,0) ; }
#line 4342 "yaccsrc.c"
    break;

  case 333: /* s_action: SYNTAX  */
#line 1035 "yaccsrc.y"
                                       { yyval = makenode(X_S_SYNTAX,0) ; }
#line 4348 "yaccsrc.c"
    break;

  case 334: /* s_action: LOSTDIGITS  */
#line 1036 "yaccsrc.y"
                                       { yyval = makenode(X_S_LOSTDIGITS,0) ; }
#line 4354 "yaccsrc.c"
    break;

  case 335: /* trace_stat: trace VALUE expr  */
#line 1039 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4361 "yaccsrc.c"
    break;

  case 336: /* trace_stat: trace nexpr  */
#line 1041 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4368 "yaccsrc.c"
    break;

  case 337: /* trace_stat: trace whatever error  */
#line 1043 "yaccsrc.y"
                                   { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 4374 "yaccsrc.c"
    break;

  case 338: /* trace_stat: trace whatever  */
#line 1044 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 4381 "yaccsrc.c"
    break;

  case 339: /* whatever: WHATEVER  */
#line 1048 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 4387 "yaccsrc.c"
    break;

  case 340: /* assignment: ass_part nexpr  */
#line 1052 "yaccsrc.y"
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
#line 4406 "yaccsrc.c"
    break;

  case 341: /* ass_part: ASSIGNMENTVARIABLE  */
#line 1068 "yaccsrc.y"
                                       { yyval = makenode(X_ASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4415 "yaccsrc.c"
    break;

  case 342: /* plusassignment: plus_ass_part nexpr  */
#line 1074 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "+=" );
                                       }
#line 4425 "yaccsrc.c"
    break;

  case 343: /* plus_ass_part: PLUSASSIGNMENTVARIABLE  */
#line 1081 "yaccsrc.y"
                                       { yyval = makenode(X_PLUSASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4434 "yaccsrc.c"
    break;

  case 344: /* minusassignment: minus_ass_part nexpr  */
#line 1087 "yaccsrc.y"
                                         { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "-=" );
                                       }
#line 4444 "yaccsrc.c"
    break;

  case 345: /* minus_ass_part: MINUSASSIGNMENTVARIABLE  */
#line 1094 "yaccsrc.y"
                                         { yyval = makenode(X_MINUSASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4453 "yaccsrc.c"
    break;

  case 346: /* multassignment: mult_ass_part nexpr  */
#line 1100 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "*=" );
                                       }
#line 4463 "yaccsrc.c"
    break;

  case 347: /* mult_ass_part: MULTASSIGNMENTVARIABLE  */
#line 1107 "yaccsrc.y"
                                       { yyval = makenode(X_MULTASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4472 "yaccsrc.c"
    break;

  case 348: /* divassignment: div_ass_part nexpr  */
#line 1113 "yaccsrc.y"
                                     { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "/=" );
                                       }
#line 4482 "yaccsrc.c"
    break;

  case 349: /* div_ass_part: DIVASSIGNMENTVARIABLE  */
#line 1120 "yaccsrc.y"
                                     { yyval = makenode(X_DIVASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4491 "yaccsrc.c"
    break;

  case 350: /* intdivassignment: intdiv_ass_part nexpr  */
#line 1126 "yaccsrc.y"
                                           { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "%=" );
                                       }
#line 4501 "yaccsrc.c"
    break;

  case 351: /* intdiv_ass_part: INTDIVASSIGNMENTVARIABLE  */
#line 1133 "yaccsrc.y"
                                           { yyval = makenode(X_INTDIVASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4510 "yaccsrc.c"
    break;

  case 352: /* modulusassignment: modulus_ass_part nexpr  */
#line 1139 "yaccsrc.y"
                                             { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "//=" );
                                       }
#line 4520 "yaccsrc.c"
    break;

  case 353: /* modulus_ass_part: MODULUSASSIGNMENTVARIABLE  */
#line 1146 "yaccsrc.y"
                                             { yyval = makenode(X_MODULUSASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4529 "yaccsrc.c"
    break;

  case 354: /* andassignment: and_ass_part nexpr  */
#line 1152 "yaccsrc.y"
                                     { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "&=" );
                                       }
#line 4539 "yaccsrc.c"
    break;

  case 355: /* and_ass_part: ANDASSIGNMENTVARIABLE  */
#line 1159 "yaccsrc.y"
                                     { yyval = makenode(X_ANDASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4548 "yaccsrc.c"
    break;

  case 356: /* xorassignment: xor_ass_part nexpr  */
#line 1165 "yaccsrc.y"
                                     { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "&&=" );
                                       }
#line 4558 "yaccsrc.c"
    break;

  case 357: /* xor_ass_part: XORASSIGNMENTVARIABLE  */
#line 1172 "yaccsrc.y"
                                     { yyval = makenode(X_XORASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4567 "yaccsrc.c"
    break;

  case 358: /* orassignment: or_ass_part nexpr  */
#line 1178 "yaccsrc.y"
                                   { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "|=" );
                                       }
#line 4577 "yaccsrc.c"
    break;

  case 359: /* or_ass_part: ORASSIGNMENTVARIABLE  */
#line 1185 "yaccsrc.y"
                                   { yyval = makenode(X_ORASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4586 "yaccsrc.c"
    break;

  case 360: /* concatassignment: concat_ass_part nexpr  */
#line 1191 "yaccsrc.y"
                                           { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                       }
#line 4594 "yaccsrc.c"
    break;

  case 361: /* concat_ass_part: CONCATASSIGNMENTVARIABLE  */
#line 1196 "yaccsrc.y"
                                           { yyval = makenode(X_CONCATASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4603 "yaccsrc.c"
    break;

  case 362: /* $@10: %empty  */
#line 1203 "yaccsrc.y"
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
#line 4624 "yaccsrc.c"
    break;

  case 363: /* expr: '(' $@10 exprs_sub  */
#line 1219 "yaccsrc.y"
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
#line 4646 "yaccsrc.c"
    break;

  case 364: /* $@11: %empty  */
#line 1236 "yaccsrc.y"
                                       { parendepth--; }
#line 4652 "yaccsrc.c"
    break;

  case 365: /* expr: expr '+' $@11 expr  */
#line 1237 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "+" );
                                         yyval = makenode( X_PLUSS, 2, yyvsp[-3], yyvsp[0] ); }
#line 4659 "yaccsrc.c"
    break;

  case 366: /* $@12: %empty  */
#line 1239 "yaccsrc.y"
                                       { parendepth--; }
#line 4665 "yaccsrc.c"
    break;

  case 367: /* expr: expr '-' $@12 expr  */
#line 1240 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "-" );
                                         yyval = makenode( X_MINUS, 2, yyvsp[-3], yyvsp[0] ); }
#line 4672 "yaccsrc.c"
    break;

  case 368: /* $@13: %empty  */
#line 1242 "yaccsrc.y"
                                       { parendepth--; }
#line 4678 "yaccsrc.c"
    break;

  case 369: /* expr: expr '*' $@13 expr  */
#line 1243 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "*" );
                                         yyval = makenode( X_MULT, 2, yyvsp[-3], yyvsp[0] ); }
#line 4685 "yaccsrc.c"
    break;

  case 370: /* expr: '*'  */
#line 1245 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "*" ); }
#line 4691 "yaccsrc.c"
    break;

  case 371: /* $@14: %empty  */
#line 1246 "yaccsrc.y"
                                       { parendepth--; }
#line 4697 "yaccsrc.c"
    break;

  case 372: /* expr: expr '/' $@14 expr  */
#line 1247 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "/" );
                                         yyval = makenode( X_DEVIDE, 2, yyvsp[-3], yyvsp[0] ); }
#line 4704 "yaccsrc.c"
    break;

  case 373: /* expr: '/'  */
#line 1249 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "/" ); }
#line 4710 "yaccsrc.c"
    break;

  case 374: /* $@15: %empty  */
#line 1250 "yaccsrc.y"
                                       { parendepth--; }
#line 4716 "yaccsrc.c"
    break;

  case 375: /* expr: expr MODULUS $@15 expr  */
#line 1251 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "//" );
                                         yyval = makenode( X_MODULUS, 2, yyvsp[-3], yyvsp[0] ); }
#line 4723 "yaccsrc.c"
    break;

  case 376: /* expr: MODULUS  */
#line 1253 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "//" ); }
#line 4729 "yaccsrc.c"
    break;

  case 377: /* $@16: %empty  */
#line 1254 "yaccsrc.y"
                                       { parendepth--; }
#line 4735 "yaccsrc.c"
    break;

  case 378: /* expr: expr '%' $@16 expr  */
#line 1255 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "%" );
                                         yyval = makenode( X_INTDIV, 2, yyvsp[-3], yyvsp[0] ); }
#line 4742 "yaccsrc.c"
    break;

  case 379: /* expr: '%'  */
#line 1257 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "%" ); }
#line 4748 "yaccsrc.c"
    break;

  case 380: /* $@17: %empty  */
#line 1258 "yaccsrc.y"
                                       { parendepth--; }
#line 4754 "yaccsrc.c"
    break;

  case 381: /* expr: expr '|' $@17 expr  */
#line 1259 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "|" );
                                         yyval = makenode( X_LOG_OR, 2, yyvsp[-3], yyvsp[0] ); }
#line 4761 "yaccsrc.c"
    break;

  case 382: /* expr: '|'  */
#line 1261 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "|" ); }
#line 4767 "yaccsrc.c"
    break;

  case 383: /* $@18: %empty  */
#line 1262 "yaccsrc.y"
                                       { parendepth--; }
#line 4773 "yaccsrc.c"
    break;

  case 384: /* expr: expr '&' $@18 expr  */
#line 1263 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "&" );
                                         yyval = makenode( X_LOG_AND, 2, yyvsp[-3], yyvsp[0] ); }
#line 4780 "yaccsrc.c"
    break;

  case 385: /* expr: '&'  */
#line 1265 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "&" ); }
#line 4786 "yaccsrc.c"
    break;

  case 386: /* $@19: %empty  */
#line 1266 "yaccsrc.y"
                                       { parendepth--; }
#line 4792 "yaccsrc.c"
    break;

  case 387: /* expr: expr XOR $@19 expr  */
#line 1267 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "&&" );
                                         yyval = makenode( X_LOG_XOR, 2, yyvsp[-3], yyvsp[0] ); }
#line 4799 "yaccsrc.c"
    break;

  case 388: /* expr: XOR  */
#line 1269 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "&&" ); }
#line 4805 "yaccsrc.c"
    break;

  case 389: /* $@20: %empty  */
#line 1270 "yaccsrc.y"
                                       { parendepth--; }
#line 4811 "yaccsrc.c"
    break;

  case 390: /* expr: expr EXP $@20 expr  */
#line 1271 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "**" );
                                         yyval = makenode( X_EXP, 2, yyvsp[-3], yyvsp[0] ); }
#line 4818 "yaccsrc.c"
    break;

  case 391: /* expr: EXP  */
#line 1273 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "**" ); }
#line 4824 "yaccsrc.c"
    break;

  case 392: /* $@21: %empty  */
#line 1274 "yaccsrc.y"
                                       { parendepth--; }
#line 4830 "yaccsrc.c"
    break;

  case 393: /* expr: expr SPACE $@21 expr  */
#line 1275 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], " " );
                                         yyval = makenode( X_SPACE, 2, yyvsp[-3], yyvsp[0] ); }
#line 4837 "yaccsrc.c"
    break;

  case 394: /* expr: SPACE  */
#line 1277 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, " " ); }
#line 4843 "yaccsrc.c"
    break;

  case 395: /* $@22: %empty  */
#line 1278 "yaccsrc.y"
                                       { parendepth--; }
#line 4849 "yaccsrc.c"
    break;

  case 396: /* expr: expr CONCATENATE $@22 expr  */
#line 1279 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "||" );
                                         yyval = makenode( X_CONCAT, 2, yyvsp[-3], yyvsp[0] ); }
#line 4856 "yaccsrc.c"
    break;

  case 397: /* expr: CONCATENATE  */
#line 1281 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "||" ); }
#line 4862 "yaccsrc.c"
    break;

  case 398: /* expr: NOT expr  */
#line 1282 "yaccsrc.y"
                                       { AUTO_REDUCE( yyvsp[0], "\\" );
                                         yyval = makenode( X_LOG_NOT, 1, yyvsp[0] ); }
#line 4869 "yaccsrc.c"
    break;

  case 399: /* expr: NOT  */
#line 1284 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "\\" ); }
#line 4875 "yaccsrc.c"
    break;

  case 400: /* $@23: %empty  */
#line 1285 "yaccsrc.y"
                                       { parendepth--; }
#line 4881 "yaccsrc.c"
    break;

  case 401: /* expr: expr '=' $@23 expr  */
#line 1286 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "=" );
                                         yyval = makenode( X_EQUAL, 2, yyvsp[-3], yyvsp[0] );
                                         transform( yyval ); }
#line 4889 "yaccsrc.c"
    break;

  case 402: /* expr: '='  */
#line 1289 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "=" ); }
#line 4895 "yaccsrc.c"
    break;

  case 403: /* $@24: %empty  */
#line 1290 "yaccsrc.y"
                                       { parendepth--; }
#line 4901 "yaccsrc.c"
    break;

  case 404: /* expr: expr GTE $@24 expr  */
#line 1291 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], ">=" );
                                         yyval = makenode( X_GTE, 2, yyvsp[-3], yyvsp[0] );
                                         transform( yyval ) ; }
#line 4909 "yaccsrc.c"
    break;

  case 405: /* expr: GTE  */
#line 1294 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, ">=" ); }
#line 4915 "yaccsrc.c"
    break;

  case 406: /* $@25: %empty  */
#line 1295 "yaccsrc.y"
                                       { parendepth--; }
#line 4921 "yaccsrc.c"
    break;

  case 407: /* expr: expr LTE $@25 expr  */
#line 1296 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "<=" );
                                         yyval = makenode( X_LTE, 2, yyvsp[-3], yyvsp[0] );
                                         transform( yyval ) ; }
#line 4929 "yaccsrc.c"
    break;

  case 408: /* expr: LTE  */
#line 1299 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "<=" ); }
#line 4935 "yaccsrc.c"
    break;

  case 409: /* $@26: %empty  */
#line 1300 "yaccsrc.y"
                                       { parendepth--; }
#line 4941 "yaccsrc.c"
    break;

  case 410: /* expr: expr GT $@26 expr  */
#line 1301 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], ">" );
                                         yyval = makenode( X_GT, 2, yyvsp[-3], yyvsp[0] );
                                         transform( yyval ) ; }
#line 4949 "yaccsrc.c"
    break;

  case 411: /* expr: GT  */
#line 1304 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, ">" ); }
#line 4955 "yaccsrc.c"
    break;

  case 412: /* $@27: %empty  */
#line 1305 "yaccsrc.y"
                                       { parendepth--; }
#line 4961 "yaccsrc.c"
    break;

  case 413: /* expr: expr LT $@27 expr  */
#line 1306 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "<" );
                                         yyval = makenode( X_LT, 2, yyvsp[-3], yyvsp[0] );
                                         transform( yyval ) ; }
#line 4969 "yaccsrc.c"
    break;

  case 414: /* expr: LT  */
#line 1309 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "<" ); }
#line 4975 "yaccsrc.c"
    break;

  case 415: /* $@28: %empty  */
#line 1310 "yaccsrc.y"
                                       { parendepth--; }
#line 4981 "yaccsrc.c"
    break;

  case 416: /* expr: expr DIFFERENT $@28 expr  */
#line 1311 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "\\=" );
                                         yyval = makenode( X_DIFF, 2, yyvsp[-3], yyvsp[0] );
                                         transform( yyval ) ; }
#line 4989 "yaccsrc.c"
    break;

  case 417: /* expr: DIFFERENT  */
#line 1314 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "\\=" ); }
#line 4995 "yaccsrc.c"
    break;

  case 418: /* $@29: %empty  */
#line 1315 "yaccsrc.y"
                                       { parendepth--; }
#line 5001 "yaccsrc.c"
    break;

  case 419: /* expr: expr EQUALEQUAL $@29 expr  */
#line 1316 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "==" );
                                         yyval = makenode( X_S_EQUAL, 2, yyvsp[-3], yyvsp[0] ); }
#line 5008 "yaccsrc.c"
    break;

  case 420: /* expr: EQUALEQUAL  */
#line 1318 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "==" ); }
#line 5014 "yaccsrc.c"
    break;

  case 421: /* $@30: %empty  */
#line 1319 "yaccsrc.y"
                                       { parendepth--; }
#line 5020 "yaccsrc.c"
    break;

  case 422: /* expr: expr NOTEQUALEQUAL $@30 expr  */
#line 1320 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "\\==" );
                                         yyval = makenode( X_S_DIFF, 2, yyvsp[-3], yyvsp[0] ); }
#line 5027 "yaccsrc.c"
    break;

  case 423: /* expr: NOTEQUALEQUAL  */
#line 1322 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "\\==" ); }
#line 5033 "yaccsrc.c"
    break;

  case 424: /* $@31: %empty  */
#line 1323 "yaccsrc.y"
                                       { parendepth--; }
#line 5039 "yaccsrc.c"
    break;

  case 425: /* expr: expr GTGT $@31 expr  */
#line 1324 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], ">>" );
                                         yyval = makenode( X_S_GT, 2, yyvsp[-3], yyvsp[0] ); }
#line 5046 "yaccsrc.c"
    break;

  case 426: /* expr: GTGT  */
#line 1326 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, ">>" ); }
#line 5052 "yaccsrc.c"
    break;

  case 427: /* $@32: %empty  */
#line 1327 "yaccsrc.y"
                                       { parendepth--; }
#line 5058 "yaccsrc.c"
    break;

  case 428: /* expr: expr LTLT $@32 expr  */
#line 1328 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "<<" );
                                         yyval = makenode( X_S_LT, 2, yyvsp[-3], yyvsp[0] ); }
#line 5065 "yaccsrc.c"
    break;

  case 429: /* expr: LTLT  */
#line 1330 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "<<" ); }
#line 5071 "yaccsrc.c"
    break;

  case 430: /* $@33: %empty  */
#line 1331 "yaccsrc.y"
                                       { parendepth--; }
#line 5077 "yaccsrc.c"
    break;

  case 431: /* expr: expr NOTGTGT $@33 expr  */
#line 1332 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "\\>>" );
                                         yyval = makenode( X_S_NGT, 2, yyvsp[-3], yyvsp[0] ); }
#line 5084 "yaccsrc.c"
    break;

  case 432: /* expr: NOTGTGT  */
#line 1334 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "\\>>" ); }
#line 5090 "yaccsrc.c"
    break;

  case 433: /* $@34: %empty  */
#line 1335 "yaccsrc.y"
                                       { parendepth--; }
#line 5096 "yaccsrc.c"
    break;

  case 434: /* expr: expr NOTLTLT $@34 expr  */
#line 1336 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "\\<<" );
                                         yyval = makenode( X_S_NLT, 2, yyvsp[-3], yyvsp[0] ); }
#line 5103 "yaccsrc.c"
    break;

  case 435: /* expr: NOTLTLT  */
#line 1338 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "\\<<" ); }
#line 5109 "yaccsrc.c"
    break;

  case 436: /* $@35: %empty  */
#line 1339 "yaccsrc.y"
                                       { parendepth--; }
#line 5115 "yaccsrc.c"
    break;

  case 437: /* expr: expr GTGTE $@35 expr  */
#line 1340 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], ">>=" );
                                         yyval = makenode( X_S_GTE, 2, yyvsp[-3], yyvsp[0] ); }
#line 5122 "yaccsrc.c"
    break;

  case 438: /* expr: GTGTE  */
#line 1342 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, ">>=" ); }
#line 5128 "yaccsrc.c"
    break;

  case 439: /* $@36: %empty  */
#line 1343 "yaccsrc.y"
                                       { parendepth--; }
#line 5134 "yaccsrc.c"
    break;

  case 440: /* expr: expr LTLTE $@36 expr  */
#line 1344 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "<<=" );
                                         yyval = makenode( X_S_LTE, 2, yyvsp[-3], yyvsp[0] ); }
#line 5141 "yaccsrc.c"
    break;

  case 441: /* expr: LTLTE  */
#line 1346 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "<<=" ); }
#line 5147 "yaccsrc.c"
    break;

  case 442: /* expr: expr MSGSEND msgtarget  */
#line 1347 "yaccsrc.y"
                                       { AUTO_REDUCE( yyvsp[-2], __regina_oo_operator );
                                         yyval = yyvsp[0];
                                         yyval->type = X_MSGSEND;
                                         yyval->p[0] = yyvsp[-2]; }
#line 5156 "yaccsrc.c"
    break;

  case 443: /* expr: expr CASCADE msgtarget  */
#line 1351 "yaccsrc.y"
                                       { AUTO_REDUCE( yyvsp[-2], __regina_oo_operator );
                                         yyval = yyvsp[0];
                                         yyval->type = X_CASCADE;
                                         yyval->p[0] = yyvsp[-2]; }
#line 5165 "yaccsrc.c"
    break;

  case 444: /* expr: MSGSEND  */
#line 1355 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, __regina_oo_operator ); }
#line 5171 "yaccsrc.c"
    break;

  case 445: /* expr: CASCADE  */
#line 1356 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, __regina_oo_operator ); }
#line 5177 "yaccsrc.c"
    break;

  case 446: /* expr: expr '[' bracket_args  */
#line 1357 "yaccsrc.y"
                                       { AUTO_REDUCE( yyvsp[-2], "[" );
                                         yyval = makenode( X_INDEX, 2, yyvsp[-2], yyvsp[0] ); }
#line 5184 "yaccsrc.c"
    break;

  case 447: /* expr: symbtree  */
#line 1359 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 5190 "yaccsrc.c"
    break;

  case 448: /* expr: CONSYMBOL  */
#line 1360 "yaccsrc.y"
                                       { yyval = makenode( X_STRING, 0 );
                                         yyval->name = Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 5197 "yaccsrc.c"
    break;

  case 449: /* expr: HEXSTRING  */
#line 1362 "yaccsrc.y"
                                       { yyval = makenode( X_STRING, 0 );
                                         yyval->name = Str_make_TSD(parser_data.TSD,retlength) ;
                                         memcpy(yyval->name->value,retvalue,
                                                    yyval->name->len=retlength); }
#line 5206 "yaccsrc.c"
    break;

  case 450: /* expr: BINSTRING  */
#line 1366 "yaccsrc.y"
                                       { yyval = makenode( X_STRING, 0 );
                                         yyval->name = Str_make_TSD(parser_data.TSD,retlength) ;
                                         memcpy(yyval->name->value,retvalue,
                                                    yyval->name->len=retlength); }
#line 5215 "yaccsrc.c"
    break;

  case 451: /* expr: STRING  */
#line 1370 "yaccsrc.y"
                                       { yyval = makenode( X_STRING, 0 );
                                         yyval->name = Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 5222 "yaccsrc.c"
    break;

  case 452: /* expr: function  */
#line 1372 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 5228 "yaccsrc.c"
    break;

  case 453: /* expr: '+' expr  */
#line 1373 "yaccsrc.y"
                                       { AUTO_REDUCE( yyvsp[0], rxnullptr );
                                         yyval = makenode( X_U_PLUSS, 1, yyvsp[0] ); }
#line 5235 "yaccsrc.c"
    break;

  case 454: /* expr: '-' expr  */
#line 1375 "yaccsrc.y"
                                       { AUTO_REDUCE( yyvsp[0], rxnullptr );
                                         yyval = makenode( X_U_MINUS, 1, yyvsp[0] ); }
#line 5242 "yaccsrc.c"
    break;

  case 455: /* expr: '+' error  */
#line 1377 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, __reginatext ); }
#line 5248 "yaccsrc.c"
    break;

  case 456: /* expr: '-' error  */
#line 1378 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, __reginatext ); }
#line 5254 "yaccsrc.c"
    break;

  case 457: /* exprs_sub: exprs ')'  */
#line 1381 "yaccsrc.y"
                                       { yyval = yyvsp[-1]; }
#line 5260 "yaccsrc.c"
    break;

  case 458: /* exprs_sub: exprs error  */
#line 1382 "yaccsrc.y"
                                       { exiterror( ERR_UNMATCHED_PARAN, 0 ); }
#line 5266 "yaccsrc.c"
    break;

  case 459: /* exprs_sub: STATSEP  */
#line 1383 "yaccsrc.y"
                                       { exiterror( ERR_UNMATCHED_PARAN, 0 ); }
#line 5272 "yaccsrc.c"
    break;

  case 460: /* symbtree: SIMSYMBOL  */
#line 1386 "yaccsrc.y"
                                       { yyval = (nodeptr)create_head( (const char *)retvalue ) ; }
#line 5278 "yaccsrc.c"
    break;

  case 461: /* function: extfunc func_args  */
#line 1390 "yaccsrc.y"
                                       { yyval = makenode(X_EX_FUNC,1,yyvsp[0]) ;
                                         yyval->name = (streng *)yyvsp[-1] ; }
#line 5285 "yaccsrc.c"
    break;

  case 462: /* function: intfunc func_args  */
#line 1392 "yaccsrc.y"
                                       { yyval = makenode(X_IN_FUNC,1,yyvsp[0]) ;
                                         yyval->name = (streng *)yyvsp[-1] ; }
#line 5292 "yaccsrc.c"
    break;

  case 463: /* @37: %empty  */
#line 1396 "yaccsrc.y"
                                       { /* ugly fake preservs parendepth */
                                         yyval = (YYSTYPE) parendepth;
                                         parendepth = 0; }
#line 5300 "yaccsrc.c"
    break;

  case 464: /* func_args: @37 exprs_sub  */
#line 1399 "yaccsrc.y"
                                       { parendepth = (int) yyval;
                                         yyval = yyvsp[0]; }
#line 5307 "yaccsrc.c"
    break;

  case 465: /* msgtarget: SIMSYMBOL  */
#line 1408 "yaccsrc.y"
                                       { yyval = makenode( X_MSGSEND, 0 );
                                         yyval->name = Str_cre_TSD( parser_data.TSD, retvalue ); }
#line 5314 "yaccsrc.c"
    break;

  case 466: /* msgtarget: STRING  */
#line 1410 "yaccsrc.y"
                                       { yyval = makenode( X_MSGSEND, 0 );
                                         yyval->name = Str_cre_TSD( parser_data.TSD, retvalue ); }
#line 5321 "yaccsrc.c"
    break;

  case 467: /* msgtarget: intfunc func_args  */
#line 1412 "yaccsrc.y"
                                       { yyval = makenode( X_MSGSEND, 2, NULL, yyvsp[0] );
                                         yyval->name = (streng *) yyvsp[-1]; }
#line 5328 "yaccsrc.c"
    break;

  case 468: /* msgtarget: extfunc func_args  */
#line 1414 "yaccsrc.y"
                                       { yyval = makenode( X_MSGSEND, 2, NULL, yyvsp[0] );
                                         yyval->name = (streng *) yyvsp[-1]; }
#line 5335 "yaccsrc.c"
    break;

  case 469: /* @38: %empty  */
#line 1418 "yaccsrc.y"
                                       { /* like func_args, with "]" */
                                         yyval = (YYSTYPE) parendepth;
                                         parendepth = 0; }
#line 5343 "yaccsrc.c"
    break;

  case 470: /* bracket_args: @38 exprs ']'  */
#line 1421 "yaccsrc.y"
                                       { parendepth = (int) yyval;
                                         yyval = yyvsp[-1]; }
#line 5350 "yaccsrc.c"
    break;

  case 471: /* intfunc: INFUNCNAME  */
#line 1425 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 5356 "yaccsrc.c"
    break;

  case 472: /* extfunc: EXFUNCNAME  */
#line 1428 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 5362 "yaccsrc.c"
    break;

  case 473: /* template: pv solid template  */
#line 1431 "yaccsrc.y"
                                       { yyval =makenode(X_TPL_SOLID,3,yyvsp[-2],yyvsp[-1],yyvsp[0]);}
#line 5368 "yaccsrc.c"
    break;

  case 474: /* template: pv  */
#line 1432 "yaccsrc.y"
                                       { yyval =makenode(X_TPL_SOLID,1,yyvsp[0]) ; }
#line 5374 "yaccsrc.c"
    break;

  case 475: /* template: error  */
#line 1433 "yaccsrc.y"
                    { exiterror( ERR_INVALID_TEMPLATE, 1, __reginatext ) ;}
#line 5380 "yaccsrc.c"
    break;

  case 476: /* solid: '-' offset  */
#line 1436 "yaccsrc.y"
                                       { yyval = makenode(X_NEG_OFFS,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 5387 "yaccsrc.c"
    break;

  case 477: /* solid: '+' offset  */
#line 1438 "yaccsrc.y"
                                       { yyval = makenode(X_POS_OFFS,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 5394 "yaccsrc.c"
    break;

  case 478: /* solid: offset  */
#line 1440 "yaccsrc.y"
                                       { yyval = makenode(X_ABS_OFFS,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 5401 "yaccsrc.c"
    break;

  case 479: /* solid: '=' offset  */
#line 1442 "yaccsrc.y"
                                       { yyval = makenode(X_ABS_OFFS,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 5408 "yaccsrc.c"
    break;

  case 480: /* solid: '(' symbtree ')'  */
#line 1444 "yaccsrc.y"
                                       { yyval = makenode(X_TPL_VAR,0) ;
                                         yyval->p[0] = yyvsp[-1] ; }
#line 5415 "yaccsrc.c"
    break;

  case 481: /* solid: '-' '(' symbtree ')'  */
#line 1446 "yaccsrc.y"
                                       { yyval = makenode(X_NEG_OFFS,0) ;
                                         yyval->p[0] = yyvsp[-1] ; }
#line 5422 "yaccsrc.c"
    break;

  case 482: /* solid: '+' '(' symbtree ')'  */
#line 1448 "yaccsrc.y"
                                       { yyval = makenode(X_POS_OFFS,0) ;
                                         yyval->p[0] = yyvsp[-1] ; }
#line 5429 "yaccsrc.c"
    break;

  case 483: /* solid: '=' '(' symbtree ')'  */
#line 1450 "yaccsrc.y"
                                       { yyval = makenode(X_ABS_OFFS,0) ;
                                         yyval->p[0] = yyvsp[-1] ; }
#line 5436 "yaccsrc.c"
    break;

  case 484: /* solid: string  */
#line 1452 "yaccsrc.y"
                                       { yyval = makenode(X_TPL_MVE,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 5443 "yaccsrc.c"
    break;

  case 485: /* offset: OFFSET  */
#line 1456 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 5449 "yaccsrc.c"
    break;

  case 486: /* offset: CONSYMBOL  */
#line 1457 "yaccsrc.y"
                                       { streng *sptr = Str_cre_TSD(parser_data.TSD,retvalue) ;
                                          if (myisnumber(parser_data.TSD, sptr))
                                          {
                                             exiterror( ERR_INVALID_INTEGER, 4, sptr->value ) ;
                                          }
                                          else
                                             exiterror( ERR_INVALID_TEMPLATE, 0 ) ;}
#line 5461 "yaccsrc.c"
    break;

  case 487: /* string: STRING  */
#line 1466 "yaccsrc.y"
                                       { yyval = (nodeptr) Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 5467 "yaccsrc.c"
    break;

  case 488: /* string: HEXSTRING  */
#line 1467 "yaccsrc.y"
                                       { streng *sptr = Str_make_TSD(parser_data.TSD,retlength) ;
                                         memcpy(sptr->value,retvalue,
                                                 sptr->len=retlength) ;
                                         yyval = (nodeptr) sptr ; }
#line 5476 "yaccsrc.c"
    break;

  case 489: /* string: BINSTRING  */
#line 1471 "yaccsrc.y"
                                       { streng *sptr = Str_make_TSD(parser_data.TSD,retlength) ;
                                         memcpy(sptr->value,retvalue,
                                                 sptr->len=retlength) ;
                                         yyval = (nodeptr) sptr ; }
#line 5485 "yaccsrc.c"
    break;

  case 490: /* pv: PLACEHOLDER pv  */
#line 1477 "yaccsrc.y"
                                       { yyval = makenode(X_TPL_POINT,1,yyvsp[0]) ; }
#line 5491 "yaccsrc.c"
    break;

  case 491: /* pv: symbtree pv  */
#line 1478 "yaccsrc.y"
                                       { yyval = makenode(X_TPL_SYMBOL,2,yyvsp[0],yyvsp[-1]) ; }
#line 5497 "yaccsrc.c"
    break;

  case 492: /* pv: %empty  */
#line 1479 "yaccsrc.y"
                                       { yyval = NULL ; }
#line 5503 "yaccsrc.c"
    break;

  case 493: /* $@39: %empty  */
#line 1489 "yaccsrc.y"
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
#line 5551 "yaccsrc.c"
    break;

  case 494: /* exprs: nexpr ',' $@39 exprs  */
#line 1532 "yaccsrc.y"
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
#line 5593 "yaccsrc.c"
    break;

  case 495: /* exprs: nexpr  */
#line 1569 "yaccsrc.y"
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
#line 5640 "yaccsrc.c"
    break;

  case 496: /* nexpr: expr  */
#line 1613 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 5646 "yaccsrc.c"
    break;

  case 497: /* nexpr: %empty  */
#line 1614 "yaccsrc.y"
                                       { yyval = NULL ; }
#line 5652 "yaccsrc.c"
    break;

  case 498: /* anyvars: xsimsymb anyvars  */
#line 1617 "yaccsrc.y"
                                        { yyval = makenode(X_SIM_SYMBOL,1,yyvsp[0]) ;
                                         yyval->name = (streng *) yyvsp[-1] ; }
#line 5659 "yaccsrc.c"
    break;

  case 499: /* anyvars: xsimsymb  */
#line 1619 "yaccsrc.y"
                                        { yyval = makenode(X_SIM_SYMBOL,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 5666 "yaccsrc.c"
    break;

  case 500: /* anyvars: '(' xsimsymb ')' anyvars  */
#line 1621 "yaccsrc.y"
                                        { yyval = makenode(X_IND_SYMBOL,1,yyvsp[0]) ;
                                         yyval->name = (streng *) yyvsp[-2] ; }
#line 5673 "yaccsrc.c"
    break;

  case 501: /* anyvars: '(' xsimsymb ')'  */
#line 1623 "yaccsrc.y"
                                        { yyval = makenode(X_IND_SYMBOL,0) ;
                                         yyval->name = (streng *) yyvsp[-1] ; }
#line 5680 "yaccsrc.c"
    break;

  case 502: /* xsimsymb: SIMSYMBOL  */
#line 1627 "yaccsrc.y"
                                       { yyval = (treenode *) Str_cre_TSD(parser_data.TSD,retvalue);}
#line 5686 "yaccsrc.c"
    break;

  case 503: /* simsymb: SIMSYMBOL  */
#line 1630 "yaccsrc.y"
                                       { yyval = (treenode *) Str_cre_TSD(parser_data.TSD,retvalue);}
#line 5692 "yaccsrc.c"
    break;

  case 504: /* simsymb: error  */
#line 1631 "yaccsrc.y"
                                       { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ) ;}
#line 5698 "yaccsrc.c"
    break;


#line 5702 "yaccsrc.c"

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

#line 1634 "yaccsrc.y"


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
      case X_STRING:
      case X_CON_SYMBOL:
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
