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
extern int __regina_do_is_loop; /* lexsrc.l: the DO token was LOOP */
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
#line 161 "yaccsrc.y"

#ifdef NDEBUG
# define YYDEBUG 0
#else
# define YYDEBUG 1
#endif

#line 211 "yaccsrc.c"

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
  YYSYMBOL_PLUSASSIGNMENTVARIABLE = 118,   /* PLUSASSIGNMENTVARIABLE  */
  YYSYMBOL_MINUSASSIGNMENTVARIABLE = 119,  /* MINUSASSIGNMENTVARIABLE  */
  YYSYMBOL_MULTASSIGNMENTVARIABLE = 120,   /* MULTASSIGNMENTVARIABLE  */
  YYSYMBOL_DIVASSIGNMENTVARIABLE = 121,    /* DIVASSIGNMENTVARIABLE  */
  YYSYMBOL_MODULUSASSIGNMENTVARIABLE = 122, /* MODULUSASSIGNMENTVARIABLE  */
  YYSYMBOL_INTDIVASSIGNMENTVARIABLE = 123, /* INTDIVASSIGNMENTVARIABLE  */
  YYSYMBOL_ORASSIGNMENTVARIABLE = 124,     /* ORASSIGNMENTVARIABLE  */
  YYSYMBOL_XORASSIGNMENTVARIABLE = 125,    /* XORASSIGNMENTVARIABLE  */
  YYSYMBOL_ANDASSIGNMENTVARIABLE = 126,    /* ANDASSIGNMENTVARIABLE  */
  YYSYMBOL_CONCATASSIGNMENTVARIABLE = 127, /* CONCATASSIGNMENTVARIABLE  */
  YYSYMBOL_128_ = 128,                     /* '|'  */
  YYSYMBOL_129_ = 129,                     /* '&'  */
  YYSYMBOL_130_ = 130,                     /* '='  */
  YYSYMBOL_CCAT = 131,                     /* CCAT  */
  YYSYMBOL_132_ = 132,                     /* '+'  */
  YYSYMBOL_133_ = 133,                     /* '-'  */
  YYSYMBOL_134_ = 134,                     /* '*'  */
  YYSYMBOL_135_ = 135,                     /* '/'  */
  YYSYMBOL_136_ = 136,                     /* '%'  */
  YYSYMBOL_UMINUS = 137,                   /* UMINUS  */
  YYSYMBOL_UPLUS = 138,                    /* UPLUS  */
  YYSYMBOL_139_ = 139,                     /* '['  */
  YYSYMBOL_140_ = 140,                     /* ','  */
  YYSYMBOL_141_ = 141,                     /* '('  */
  YYSYMBOL_142_ = 142,                     /* ')'  */
  YYSYMBOL_143_ = 143,                     /* ']'  */
  YYSYMBOL_YYACCEPT = 144,                 /* $accept  */
  YYSYMBOL_start = 145,                    /* start  */
  YYSYMBOL_146_1 = 146,                    /* $@1  */
  YYSYMBOL_prog = 147,                     /* prog  */
  YYSYMBOL_dirsect = 148,                  /* dirsect  */
  YYSYMBOL_dirblock = 149,                 /* dirblock  */
  YYSYMBOL_directive = 150,                /* directive  */
  YYSYMBOL_151_2 = 151,                    /* @2  */
  YYSYMBOL_dirwords = 152,                 /* dirwords  */
  YYSYMBOL_dirword = 153,                  /* dirword  */
  YYSYMBOL_stats = 154,                    /* stats  */
  YYSYMBOL_xstats = 155,                   /* xstats  */
  YYSYMBOL_ystatement = 156,               /* ystatement  */
  YYSYMBOL_lonely_end = 157,               /* lonely_end  */
  YYSYMBOL_nxstats = 158,                  /* nxstats  */
  YYSYMBOL_ncl = 159,                      /* ncl  */
  YYSYMBOL_nlncl = 160,                    /* nlncl  */
  YYSYMBOL_optLabels = 161,                /* optLabels  */
  YYSYMBOL_statement = 162,                /* statement  */
  YYSYMBOL_gruff = 163,                    /* gruff  */
  YYSYMBOL_mttstatement = 164,             /* mttstatement  */
  YYSYMBOL_mtstatement = 165,              /* mtstatement  */
  YYSYMBOL_nclstatement = 166,             /* nclstatement  */
  YYSYMBOL_oo_expose_stat = 167,           /* oo_expose_stat  */
  YYSYMBOL_oo_usearg_stat = 168,           /* oo_usearg_stat  */
  YYSYMBOL_usevars = 169,                  /* usevars  */
  YYSYMBOL_usevar = 170,                   /* usevar  */
  YYSYMBOL_call = 171,                     /* call  */
  YYSYMBOL_do = 172,                       /* do  */
  YYSYMBOL_exit = 173,                     /* exit  */
  YYSYMBOL_if = 174,                       /* if  */
  YYSYMBOL_iterate = 175,                  /* iterate  */
  YYSYMBOL_leave = 176,                    /* leave  */
  YYSYMBOL_say = 177,                      /* say  */
  YYSYMBOL_return = 178,                   /* return  */
  YYSYMBOL_address = 179,                  /* address  */
  YYSYMBOL_arg = 180,                      /* arg  */
  YYSYMBOL_drop = 181,                     /* drop  */
  YYSYMBOL_interpret = 182,                /* interpret  */
  YYSYMBOL_label = 183,                    /* label  */
  YYSYMBOL_nop = 184,                      /* nop  */
  YYSYMBOL_numeric = 185,                  /* numeric  */
  YYSYMBOL_options = 186,                  /* options  */
  YYSYMBOL_parse = 187,                    /* parse  */
  YYSYMBOL_proc = 188,                     /* proc  */
  YYSYMBOL_pull = 189,                     /* pull  */
  YYSYMBOL_push = 190,                     /* push  */
  YYSYMBOL_queue = 191,                    /* queue  */
  YYSYMBOL_select = 192,                   /* select  */
  YYSYMBOL_signal = 193,                   /* signal  */
  YYSYMBOL_when = 194,                     /* when  */
  YYSYMBOL_otherwise = 195,                /* otherwise  */
  YYSYMBOL_trace = 196,                    /* trace  */
  YYSYMBOL_upper = 197,                    /* upper  */
  YYSYMBOL_address_stat = 198,             /* address_stat  */
  YYSYMBOL_199_3 = 199,                    /* @3  */
  YYSYMBOL_address_stat2 = 200,            /* address_stat2  */
  YYSYMBOL_201_4 = 201,                    /* $@4  */
  YYSYMBOL_arg_stat = 202,                 /* arg_stat  */
  YYSYMBOL_call_stat = 203,                /* call_stat  */
  YYSYMBOL_204_5 = 204,                    /* $@5  */
  YYSYMBOL_205_6 = 205,                    /* $@6  */
  YYSYMBOL_call_name = 206,                /* call_name  */
  YYSYMBOL_call_args = 207,                /* call_args  */
  YYSYMBOL_expr_stat = 208,                /* expr_stat  */
  YYSYMBOL_end_stat = 209,                 /* end_stat  */
  YYSYMBOL_end = 210,                      /* end  */
  YYSYMBOL_do_stat = 211,                  /* do_stat  */
  YYSYMBOL_do_opts = 212,                  /* do_opts  */
  YYSYMBOL_dolabel = 213,                  /* dolabel  */
  YYSYMBOL_docounter = 214,                /* docounter  */
  YYSYMBOL_doindex = 215,                  /* doindex  */
  YYSYMBOL_doitem = 216,                   /* doitem  */
  YYSYMBOL_withvars = 217,                 /* withvars  */
  YYSYMBOL_repetitor = 218,                /* repetitor  */
  YYSYMBOL_nvir = 219,                     /* nvir  */
  YYSYMBOL_naddr_with = 220,               /* naddr_with  */
  YYSYMBOL_221_7 = 221,                    /* @7  */
  YYSYMBOL_addr_with = 222,                /* addr_with  */
  YYSYMBOL_connection = 223,               /* connection  */
  YYSYMBOL_inputstmts = 224,               /* inputstmts  */
  YYSYMBOL_outputstmts = 225,              /* outputstmts  */
  YYSYMBOL_errorstmts = 226,               /* errorstmts  */
  YYSYMBOL_adeo = 227,                     /* adeo  */
  YYSYMBOL_adei = 228,                     /* adei  */
  YYSYMBOL_adio = 229,                     /* adio  */
  YYSYMBOL_inputstmt = 230,                /* inputstmt  */
  YYSYMBOL_outputstmt = 231,               /* outputstmt  */
  YYSYMBOL_errorstmt = 232,                /* errorstmt  */
  YYSYMBOL_resourcei = 233,                /* resourcei  */
  YYSYMBOL_resourceo = 234,                /* resourceo  */
  YYSYMBOL_resources = 235,                /* resources  */
  YYSYMBOL_nsimsymb = 236,                 /* nsimsymb  */
  YYSYMBOL_237_8 = 237,                    /* $@8  */
  YYSYMBOL_nnvir = 238,                    /* nnvir  */
  YYSYMBOL_239_9 = 239,                    /* $@9  */
  YYSYMBOL_nspace = 240,                   /* nspace  */
  YYSYMBOL_addrAll = 241,                  /* addrAll  */
  YYSYMBOL_addrSim = 242,                  /* addrSim  */
  YYSYMBOL_addrString = 243,               /* addrString  */
  YYSYMBOL_dovar = 244,                    /* dovar  */
  YYSYMBOL_tobyfor = 245,                  /* tobyfor  */
  YYSYMBOL_conditional = 246,              /* conditional  */
  YYSYMBOL_drop_stat = 247,                /* drop_stat  */
  YYSYMBOL_upper_stat = 248,               /* upper_stat  */
  YYSYMBOL_exit_stat = 249,                /* exit_stat  */
  YYSYMBOL_if_stat = 250,                  /* if_stat  */
  YYSYMBOL_unexp_then = 251,               /* unexp_then  */
  YYSYMBOL_unexp_else = 252,               /* unexp_else  */
  YYSYMBOL_ipret_stat = 253,               /* ipret_stat  */
  YYSYMBOL_iterate_stat = 254,             /* iterate_stat  */
  YYSYMBOL_label_stat = 255,               /* label_stat  */
  YYSYMBOL_labelname = 256,                /* labelname  */
  YYSYMBOL_leave_stat = 257,               /* leave_stat  */
  YYSYMBOL_nop_stat = 258,                 /* nop_stat  */
  YYSYMBOL_numeric_stat = 259,             /* numeric_stat  */
  YYSYMBOL_form_expr = 260,                /* form_expr  */
  YYSYMBOL_options_stat = 261,             /* options_stat  */
  YYSYMBOL_parse_stat = 262,               /* parse_stat  */
  YYSYMBOL_parse_flags = 263,              /* parse_flags  */
  YYSYMBOL_templs = 264,                   /* templs  */
  YYSYMBOL_parse_param = 265,              /* parse_param  */
  YYSYMBOL_proc_stat = 266,                /* proc_stat  */
  YYSYMBOL_pull_stat = 267,                /* pull_stat  */
  YYSYMBOL_push_stat = 268,                /* push_stat  */
  YYSYMBOL_queue_stat = 269,               /* queue_stat  */
  YYSYMBOL_say_stat = 270,                 /* say_stat  */
  YYSYMBOL_return_stat = 271,              /* return_stat  */
  YYSYMBOL_sel_end = 272,                  /* sel_end  */
  YYSYMBOL_select_stat = 273,              /* select_stat  */
  YYSYMBOL_when_stats = 274,               /* when_stats  */
  YYSYMBOL_when_stat = 275,                /* when_stat  */
  YYSYMBOL_ex_when_stat = 276,             /* ex_when_stat  */
  YYSYMBOL_otherwise_stat = 277,           /* otherwise_stat  */
  YYSYMBOL_signal_stat = 278,              /* signal_stat  */
  YYSYMBOL_signal_name = 279,              /* signal_name  */
  YYSYMBOL_namespec = 280,                 /* namespec  */
  YYSYMBOL_asymbol = 281,                  /* asymbol  */
  YYSYMBOL_on = 282,                       /* on  */
  YYSYMBOL_off = 283,                      /* off  */
  YYSYMBOL_c_action = 284,                 /* c_action  */
  YYSYMBOL_s_action = 285,                 /* s_action  */
  YYSYMBOL_trace_stat = 286,               /* trace_stat  */
  YYSYMBOL_whatever = 287,                 /* whatever  */
  YYSYMBOL_assignment = 288,               /* assignment  */
  YYSYMBOL_ass_part = 289,                 /* ass_part  */
  YYSYMBOL_plusassignment = 290,           /* plusassignment  */
  YYSYMBOL_plus_ass_part = 291,            /* plus_ass_part  */
  YYSYMBOL_minusassignment = 292,          /* minusassignment  */
  YYSYMBOL_minus_ass_part = 293,           /* minus_ass_part  */
  YYSYMBOL_multassignment = 294,           /* multassignment  */
  YYSYMBOL_mult_ass_part = 295,            /* mult_ass_part  */
  YYSYMBOL_divassignment = 296,            /* divassignment  */
  YYSYMBOL_div_ass_part = 297,             /* div_ass_part  */
  YYSYMBOL_intdivassignment = 298,         /* intdivassignment  */
  YYSYMBOL_intdiv_ass_part = 299,          /* intdiv_ass_part  */
  YYSYMBOL_modulusassignment = 300,        /* modulusassignment  */
  YYSYMBOL_modulus_ass_part = 301,         /* modulus_ass_part  */
  YYSYMBOL_andassignment = 302,            /* andassignment  */
  YYSYMBOL_and_ass_part = 303,             /* and_ass_part  */
  YYSYMBOL_xorassignment = 304,            /* xorassignment  */
  YYSYMBOL_xor_ass_part = 305,             /* xor_ass_part  */
  YYSYMBOL_orassignment = 306,             /* orassignment  */
  YYSYMBOL_or_ass_part = 307,              /* or_ass_part  */
  YYSYMBOL_concatassignment = 308,         /* concatassignment  */
  YYSYMBOL_concat_ass_part = 309,          /* concat_ass_part  */
  YYSYMBOL_expr = 310,                     /* expr  */
  YYSYMBOL_311_10 = 311,                   /* $@10  */
  YYSYMBOL_312_11 = 312,                   /* $@11  */
  YYSYMBOL_313_12 = 313,                   /* $@12  */
  YYSYMBOL_314_13 = 314,                   /* $@13  */
  YYSYMBOL_315_14 = 315,                   /* $@14  */
  YYSYMBOL_316_15 = 316,                   /* $@15  */
  YYSYMBOL_317_16 = 317,                   /* $@16  */
  YYSYMBOL_318_17 = 318,                   /* $@17  */
  YYSYMBOL_319_18 = 319,                   /* $@18  */
  YYSYMBOL_320_19 = 320,                   /* $@19  */
  YYSYMBOL_321_20 = 321,                   /* $@20  */
  YYSYMBOL_322_21 = 322,                   /* $@21  */
  YYSYMBOL_323_22 = 323,                   /* $@22  */
  YYSYMBOL_324_23 = 324,                   /* $@23  */
  YYSYMBOL_325_24 = 325,                   /* $@24  */
  YYSYMBOL_326_25 = 326,                   /* $@25  */
  YYSYMBOL_327_26 = 327,                   /* $@26  */
  YYSYMBOL_328_27 = 328,                   /* $@27  */
  YYSYMBOL_329_28 = 329,                   /* $@28  */
  YYSYMBOL_330_29 = 330,                   /* $@29  */
  YYSYMBOL_331_30 = 331,                   /* $@30  */
  YYSYMBOL_332_31 = 332,                   /* $@31  */
  YYSYMBOL_333_32 = 333,                   /* $@32  */
  YYSYMBOL_334_33 = 334,                   /* $@33  */
  YYSYMBOL_335_34 = 335,                   /* $@34  */
  YYSYMBOL_336_35 = 336,                   /* $@35  */
  YYSYMBOL_337_36 = 337,                   /* $@36  */
  YYSYMBOL_exprs_sub = 338,                /* exprs_sub  */
  YYSYMBOL_symbtree = 339,                 /* symbtree  */
  YYSYMBOL_function = 340,                 /* function  */
  YYSYMBOL_func_args = 341,                /* func_args  */
  YYSYMBOL_342_37 = 342,                   /* @37  */
  YYSYMBOL_msgtarget = 343,                /* msgtarget  */
  YYSYMBOL_msgname = 344,                  /* msgname  */
  YYSYMBOL_bracket_args = 345,             /* bracket_args  */
  YYSYMBOL_346_38 = 346,                   /* @38  */
  YYSYMBOL_intfunc = 347,                  /* intfunc  */
  YYSYMBOL_extfunc = 348,                  /* extfunc  */
  YYSYMBOL_template = 349,                 /* template  */
  YYSYMBOL_solid = 350,                    /* solid  */
  YYSYMBOL_offset = 351,                   /* offset  */
  YYSYMBOL_string = 352,                   /* string  */
  YYSYMBOL_pv = 353,                       /* pv  */
  YYSYMBOL_exprs = 354,                    /* exprs  */
  YYSYMBOL_355_39 = 355,                   /* $@39  */
  YYSYMBOL_nexpr = 356,                    /* nexpr  */
  YYSYMBOL_anyvars = 357,                  /* anyvars  */
  YYSYMBOL_xsimsymb = 358,                 /* xsimsymb  */
  YYSYMBOL_simsymb = 359                   /* simsymb  */
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
#define YYLAST   2628

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  144
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  216
/* YYNRULES -- Number of rules.  */
#define YYNRULES  531
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  726

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   385


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
       2,     2,     2,     2,     2,     2,     2,   136,   129,     2,
     141,   142,   134,   132,   140,   133,     2,   135,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   130,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   139,     2,   143,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,   128,     2,     2,     2,     2,     2,
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
     125,   126,   127,   131,   137,   138
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   171,   171,   171,   181,   184,   188,   193,   204,   205,
     208,   212,   217,   217,   229,   232,   233,   236,   238,   240,
     244,   246,   249,   251,   254,   255,   258,   263,   264,   267,
     268,   271,   272,   275,   276,   279,   280,   283,   287,   290,
     291,   292,   293,   296,   297,   298,   299,   300,   301,   302,
     303,   304,   305,   306,   307,   308,   309,   310,   311,   312,
     313,   314,   315,   316,   317,   318,   319,   320,   321,   322,
     323,   324,   325,   326,   327,   328,   329,   330,   331,   332,
     339,   342,   345,   348,   351,   353,   356,   358,   362,   366,
     373,   377,   382,   386,   390,   394,   398,   402,   406,   410,
     414,   418,   422,   426,   430,   434,   438,   442,   446,   450,
     455,   459,   463,   467,   471,   475,   475,   479,   482,   483,
     484,   484,   486,   490,   496,   502,   502,   507,   507,   513,
     514,   515,   516,   517,   523,   528,   529,   535,   536,   539,
     547,   550,   572,   578,   580,   581,   584,   596,   597,   601,
     607,   609,   611,   613,   617,   618,   619,   620,   623,   627,
     631,   635,   638,   639,   640,   643,   649,   651,   654,   657,
     658,   659,   662,   662,   668,   671,   672,   673,   676,   677,
     678,   679,   682,   686,   690,   694,   695,   696,   697,   698,
     701,   702,   703,   704,   705,   708,   709,   710,   711,   712,
     715,   716,   719,   720,   723,   724,   727,   728,   730,   731,
     736,   737,   739,   740,   741,   742,   747,   755,   756,   769,
     770,   774,   775,   779,   782,   782,   786,   786,   790,   791,
     794,   795,   798,   806,   814,   817,   818,   819,   822,   823,
     824,   827,   828,   832,   833,   837,   841,   849,   859,   861,
     863,   864,   867,   870,   873,   878,   880,   881,   884,   892,
     896,   898,   899,   902,   903,   906,   909,   910,   911,   914,
     916,   918,   919,   920,   923,   926,   927,   930,   933,   939,
     945,   946,   949,   951,   953,   955,   957,   959,   961,   965,
     967,   970,   971,   972,   973,   974,   975,   976,   978,   979,
     982,   983,   984,   985,   986,   990,   994,   998,  1002,  1006,
    1010,  1011,  1012,  1015,  1020,  1021,  1023,  1024,  1025,  1029,
    1030,  1031,  1034,  1040,  1042,  1043,  1053,  1054,  1057,  1061,
    1067,  1070,  1071,  1073,  1074,  1075,  1076,  1077,  1083,  1087,
    1088,  1094,  1095,  1096,  1099,  1100,  1101,  1104,  1105,  1108,
    1111,  1114,  1115,  1116,  1117,  1120,  1121,  1122,  1123,  1126,
    1128,  1130,  1131,  1135,  1139,  1155,  1161,  1168,  1174,  1181,
    1187,  1194,  1200,  1207,  1213,  1220,  1226,  1233,  1239,  1246,
    1252,  1259,  1265,  1272,  1278,  1283,  1290,  1290,  1323,  1323,
    1326,  1326,  1329,  1329,  1332,  1333,  1333,  1336,  1337,  1337,
    1340,  1341,  1341,  1344,  1345,  1345,  1348,  1349,  1349,  1352,
    1353,  1353,  1356,  1357,  1357,  1360,  1361,  1361,  1364,  1365,
    1365,  1368,  1369,  1371,  1372,  1372,  1376,  1377,  1377,  1381,
    1382,  1382,  1386,  1387,  1387,  1391,  1392,  1392,  1396,  1397,
    1397,  1401,  1402,  1402,  1405,  1406,  1406,  1409,  1410,  1410,
    1413,  1414,  1414,  1417,  1418,  1418,  1421,  1422,  1422,  1425,
    1426,  1426,  1429,  1430,  1430,  1433,  1434,  1438,  1442,  1443,
    1444,  1446,  1447,  1453,  1457,  1461,  1463,  1464,  1466,  1468,
    1469,  1472,  1473,  1474,  1477,  1481,  1483,  1487,  1487,  1499,
    1501,  1505,  1509,  1512,  1515,  1520,  1523,  1523,  1530,  1533,
    1536,  1537,  1538,  1541,  1543,  1545,  1547,  1549,  1551,  1553,
    1555,  1557,  1561,  1562,  1571,  1572,  1576,  1582,  1583,  1584,
    1594,  1594,  1674,  1718,  1719,  1722,  1724,  1726,  1728,  1732,
    1735,  1736
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

#define YYPACT_NINF (-623)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-525)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -623,    50,  -623,  -623,  -623,    20,     1,   272,   -55,  -623,
      27,    33,  -623,  -623,  -623,  1343,  -623,  -623,  -623,  -623,
      18,  -623,  -623,  -623,  -623,   326,  -623,    34,   -55,  -623,
    -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,
    -623,  -623,  -623,    12,  -623,  -623,  -623,  -623,  -623,  -623,
    -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,   385,  -623,
    -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,
    -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,
    -623,  -623,  -623,  -623,  -623,  -623,  -623,     3,  -623,  -623,
    -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,
    -623,  1035,  1122,  -623,  -623,  -623,  -623,   368,  -623,    66,
    -623,  -623,   440,  -623,  2194,  1873,   291,   309,  2194,  2194,
    -623,    90,   -37,  2194,    41,   297,  2194,   224,   259,   125,
    2194,  2194,    62,   337,  -623,  -623,  1698,   -37,  -623,  -623,
    -623,  -623,   394,    66,  -623,  -623,  -623,  -623,  -623,  -623,
    -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,
    -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  2194,  -623,
    2194,  -623,  2194,  -623,  2194,  -623,  2194,  -623,  2194,  -623,
    2194,  -623,  2194,  -623,  2194,  -623,  2194,  -623,  2194,  2312,
    -623,  -623,  -623,  -623,     7,  -623,  -623,  -623,  -623,    -2,
    -623,  -623,  -623,     3,  -623,   -37,   -59,    80,   -16,  -623,
    -623,   -59,  -623,   -59,  1962,  -623,  -623,    18,  -623,  -623,
    -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,   249,   263,
    -623,  2050,  2312,  -623,    18,  2312,  -623,  -623,   206,   305,
    -623,  -623,    22,  -623,    26,    29,    26,  -623,   390,   307,
    2312,  -623,  -623,  2194,   339,  2194,  -623,  -623,  -623,  -623,
    -623,    31,   489,  -623,    21,  -623,  -623,    81,    -9,   289,
      90,  -623,    25,  -623,  -623,  -623,  -623,  1283,  -623,  2194,
    -623,   336,  -623,   258,   288,  2194,  -623,   341,  -623,   369,
     404,    18,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,
    -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,
    -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,
    -623,   436,   436,  -623,  -623,  -623,  -623,  -623,  -623,  -623,
    -623,  -623,  -623,  1962,  -623,     7,   326,    18,    72,  -623,
    -623,     3,  -623,  -623,    10,    61,  2194,  -623,  -623,  -623,
    -623,  -623,    38,  -623,   406,  2194,   598,  -623,  -623,  -623,
      59,  -623,  -623,   454,    99,  2371,    42,  -623,  -623,  -623,
    2194,    98,  -623,  -623,  -623,  2194,  -623,  2194,  -623,  -623,
      90,  -623,  -623,  -623,    74,   110,   156,   132,    90,  -623,
    -623,  -623,  2312,  -623,  2194,  -623,  -623,   437,  2312,  -623,
    -623,   241,  -623,  -623,  -623,  -623,  -623,    90,    29,  -623,
     438,  -623,  -623,   273,  1209,   276,   154,  -623,  2312,  -623,
    -623,  -623,  -623,  -623,  -623,   170,  -623,   459,  2312,  -623,
    -623,  -623,  2194,  2194,  2194,  2194,  2194,  2194,  2194,  2194,
    2194,  2194,  2194,  2194,  2194,  2194,  2194,  2194,  2194,  2194,
    -623,  -623,  -623,   169,  -623,  -623,  -623,  2194,  2194,  2194,
    2194,  2194,  2194,  2194,  2194,  -623,  2194,  -623,  -623,   -37,
    -623,  -623,  -623,  -623,  -623,   152,  -623,    44,   482,  -623,
    -623,  -623,  -623,  -623,  -623,  -623,   204,   225,   216,  2194,
    2194,    66,  2194,   702,   325,  -623,  -623,   304,  2109,  -623,
     486,  -623,  -623,  -623,   300,   300,   300,   181,  2253,   304,
    -623,   132,  -623,   132,  -623,   132,  -623,   217,  -623,  2312,
    -623,  -623,    29,  -623,  -623,  -623,  1784,  -623,   670,  -623,
     327,  -623,   488,  -623,  1090,   149,   523,   523,   523,   523,
     523,   523,   523,  1090,   -59,  2430,   523,   523,   523,   523,
     523,   523,   233,  -623,  -623,  2430,  2489,   523,  1175,  1175,
     149,   149,   149,   243,  -623,  2194,  -623,  -623,  -623,  -623,
    -623,  -623,  -623,  2194,  2371,  2371,    18,  2371,  2194,   809,
    -623,   365,  -623,  -623,  -623,   300,   300,   426,  -623,   300,
     300,   106,  -623,   300,   300,   370,    77,   129,   218,   300,
    -623,   252,   254,   260,  -623,   396,  -623,  -623,   405,  -623,
    -623,  -623,  -623,  -623,  2371,  -623,  -623,   371,   428,  -623,
    1477,   563,  2371,  -623,   399,  -623,   300,   386,   300,   401,
     300,   386,   300,   395,   300,   401,   300,   395,  -623,   560,
    -623,   653,  -623,   653,   304,  -623,  -623,  -623,  -623,  -623,
     490,  -623,   504,  -623,  1611,  -623,  -623,  2194,  2194,  2194,
     563,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,
     301,    40,   576,   818,    70,  -623,  -623,  -623,   104,   136,
    -623,  -623,  -623,  -623,  -623,  -623,  2194,  2371,  2371,  2371,
     563,   948,  -623,  -623,   300,  -623,  -623,   300,  -623,  -623,
    -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,   502,  2371,
    -623,  -623,  -623,  -623,  -623,  -623,   -19,     3,  -623,  -623,
    -623,  -623,  -623,  -623,  -623,  -623
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
     412,   472,   484,   499,   498,   473,   475,   474,   103,   450,
     453,   456,   459,   462,   465,   468,   469,    87,   367,   369,
     371,   373,   377,   375,   383,   381,   379,   385,   406,   409,
     426,     0,     0,   394,   397,   403,   386,     0,    38,     0,
      78,    79,     0,   147,   524,     0,     0,     0,   524,   524,
     115,     0,     0,     0,     0,     0,   524,     0,     0,     0,
     524,   524,     0,     0,   326,   327,   524,     0,    43,    45,
      46,    44,     0,     0,    47,    48,    66,    49,    40,    41,
      42,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    62,    61,    63,    64,    65,    77,   524,    67,
     524,    68,   524,    69,   524,    70,   524,    71,   524,    72,
     524,    75,   524,    73,   524,    74,   524,    76,   524,   141,
     471,   476,   487,   487,    30,    34,    18,    17,    19,     0,
      16,    81,   529,     0,    80,   526,   422,     0,    85,    86,
     479,   477,   480,   478,   524,   252,   253,    39,   138,   349,
     350,   347,   348,   515,   514,   516,   125,   137,     0,     0,
     127,   168,   523,   245,   250,    34,   531,   530,     0,     0,
     308,   309,     0,   502,   519,   124,   519,   290,   501,     0,
     254,   264,   274,   266,     0,   272,   277,   281,   291,   293,
     296,     0,     0,   295,   282,   294,   292,   285,   288,     0,
       0,   301,     0,   305,   306,   307,   316,     0,   343,     0,
     342,     0,   341,     0,     0,     0,   363,     0,   360,     0,
       0,    26,   364,   366,   368,   370,   372,   374,   376,   378,
     380,   382,   384,   419,   398,   427,   433,   430,   436,   439,
     442,   445,   416,   413,   410,   448,   451,   454,   457,   460,
     463,     0,     0,   404,   407,   424,   388,   390,   392,   395,
     401,   496,   486,   524,   485,    29,     0,    13,     0,   525,
      83,    87,   483,   387,     0,   522,   524,   129,   352,   353,
     354,   351,     0,   130,     0,   524,     0,   234,   150,   151,
       0,   148,   149,   240,     0,   229,     0,   256,   261,   120,
       0,     0,   169,   170,   171,     0,   116,   524,   118,   517,
       0,   518,   512,   513,     0,     0,     0,     0,     0,   505,
     511,   241,   265,   271,     0,   275,   276,     0,   273,   297,
     299,     0,   283,   286,   284,   287,   280,     0,   279,   302,
       0,   321,   317,     0,     0,     0,   329,   320,   330,   331,
     333,   357,   356,   358,   355,     0,   334,     0,   359,   361,
     243,   145,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     495,   492,   466,   489,   487,   487,   467,     0,     0,     0,
       0,     0,     0,     0,     0,   470,   524,   488,    15,   528,
      84,   482,   481,   520,   126,   139,   131,     0,     0,   135,
     128,   163,   228,   162,   152,   153,   154,   155,     0,     0,
       0,     0,     0,   416,   164,   251,    34,   172,   172,   181,
       0,   178,   179,   180,   229,   229,   229,   177,     0,   172,
     289,     0,   506,     0,   504,     0,   503,     0,   500,   270,
     267,   298,   278,   303,   314,   325,    34,   315,     0,   319,
       0,   335,     0,   339,   420,   399,   428,   434,   431,   437,
     440,   443,   446,   417,   414,   411,   449,   452,   455,   458,
     461,   464,   490,   493,   494,   405,   408,   425,   389,   391,
     393,   396,   402,     0,   527,   524,   140,   346,   344,   345,
     132,   156,   157,     0,   229,   229,    37,   229,     0,     0,
     121,     0,   117,   176,   182,   229,   229,   189,   183,   229,
     229,   194,   184,   229,   229,   199,     0,     0,     0,   229,
     122,     0,     0,     0,   507,     0,   318,    37,     0,   313,
     336,   487,   497,   521,   229,   238,   239,    37,     0,    37,
      37,   161,   229,   249,   246,   173,   229,   185,   229,   187,
     229,   190,   229,   192,   229,   195,   229,   197,   201,     0,
     203,     0,   205,     0,   172,   510,   509,   508,    34,   328,
       0,   491,   166,    22,    37,   146,    23,     0,     0,     0,
     160,   165,    34,   186,   188,   191,   193,   196,   198,   209,
       0,     0,     0,     0,     0,   200,   206,   215,     0,     0,
     202,   210,   204,   123,    37,   311,     0,   229,   229,   229,
     159,     0,   217,   216,   229,   219,   218,   229,   221,   220,
     223,   222,   208,   207,   212,   211,   214,   213,   322,   229,
     235,   237,   236,   158,   248,   247,     0,     0,   323,   167,
     227,   230,   231,   233,   232,   225
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -623,  -623,  -623,  -623,   508,    64,  -623,  -623,  -623,   189,
     522,  -623,   -11,  -623,   -67,  -103,  -211,   -13,  -540,    -7,
    -623,  -623,  -623,  -623,  -623,   201,  -623,  -623,  -623,  -623,
    -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,
    -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,
    -262,  -246,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,
    -623,  -623,  -623,   199,  -623,  -623,   -53,  -623,  -623,  -623,
    -623,    91,   119,  -623,  -623,  -623,  -484,  -623,    30,  -623,
    -623,  -623,  -623,  -623,  -623,  -623,  -454,  -421,  -397,  -623,
     -36,   -81,  -623,  -623,   -91,  -623,   -93,  -623,   -97,  -623,
    -623,  -622,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,
    -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,
    -251,   357,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,
    -623,   213,  -623,  -623,  -623,  -623,   208,   501,   505,   506,
     363,   351,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,
    -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,
    -623,  -623,  -623,  -623,  -623,  -623,   -14,  -623,  -623,  -623,
    -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,
    -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,  -623,
    -623,  -623,  -623,  -623,   308,  -112,  -623,  -175,  -623,   328,
    -623,  -623,  -623,   293,   306,  -119,  -623,   190,  -244,   -20,
    -325,  -623,   -57,  -115,   -85,  -114
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,     2,     4,     8,     9,    10,    25,   199,   200,
      11,   617,    12,    13,   618,    20,     5,     6,    14,    15,
      16,   108,   109,   110,   111,   207,   208,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,    21,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   242,   376,   497,   139,   140,
     346,   355,   226,   474,   141,   142,   143,   144,   231,   361,
     362,   486,   487,   488,   363,   377,   580,   581,   378,   500,
     501,   502,   503,   584,   588,   592,   504,   505,   506,   675,
     680,   681,   696,   697,   693,   694,   483,   720,   721,   722,
     364,   660,   491,   145,   146,   147,   148,   149,   150,   151,
     152,    22,    23,   153,   154,   155,   397,   156,   157,   269,
     245,   270,   158,   159,   160,   161,   162,   163,   609,   164,
     416,   417,    17,   530,   165,   281,   478,   227,   228,   229,
     424,   425,   166,   287,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   232,   214,   460,   461,
     462,   463,   433,   464,   457,   458,   443,   442,   441,   432,
     459,   434,   436,   435,   437,   438,   439,   440,   444,   445,
     446,   447,   448,   449,   343,   190,   191,   332,   333,   452,
     453,   465,   466,   192,   193,   247,   388,   389,   230,   248,
     344,   565,   345,   204,   205,   238
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      29,   189,   209,   239,   390,   194,   217,   249,   107,   246,
     273,   471,   234,   201,   582,   414,    29,   246,   334,   408,
      -5,   475,   289,   369,   366,   600,   409,   -11,   290,   277,
     475,   415,   236,    -4,   -10,   202,   619,   404,   690,   476,
     291,   695,   251,   495,   206,   567,    18,   370,   371,    18,
       3,   589,   593,   202,   321,   322,   496,   233,   223,   224,
       7,   240,   241,   276,   336,   225,   195,   619,   713,   256,
    -119,   702,    26,   274,   275,   202,    19,   653,   638,   288,
     331,   340,    19,   585,   202,   594,  -134,   211,   213,  -263,
     339,   243,    26,   372,   373,   244,   337,   202,    72,   499,
     374,   235,   405,   237,   203,   704,  -224,   586,   590,   250,
      18,   292,  -224,   293,    18,   294,   568,   295,   338,   296,
     477,   297,   569,   298,   341,   299,   243,   300,   -82,   301,
     640,   302,   246,   402,   246,     7,   632,   706,  -519,   382,
     636,   563,   -11,   482,   708,   383,  -229,   399,     7,   -10,
     484,   485,   472,   203,   414,  -519,   522,   410,   246,   244,
     683,  -519,    72,   375,   482,   628,   203,  -519,  -519,   380,
     528,   531,   634,  -519,  -519,   382,   670,   671,   672,   673,
    -229,   383,   335,  -229,  -229,  -229,  -229,  -229,   626,    50,
    -519,    52,   630,   403,   244,   482,  -519,    72,  -229,  -229,
    -229,   473,  -519,  -519,    72,   401,   596,   367,   598,  -519,
     670,   671,   672,   673,   469,   511,   313,   365,  -338,   642,
    -519,   382,  -519,  -519,   379,   257,   381,   383,   258,   492,
    -519,  -519,  -229,  -229,  -229,  -229,  -229,  -229,  -229,   392,
     613,   398,   670,   671,   672,   673,   259,   260,   261,   262,
     347,   513,   477,   263,  -255,  -519,   209,  -519,  -519,   420,
     271,   510,   321,   322,   353,   418,  -519,   521,   246,   518,
     264,   428,   494,    24,   524,   517,   246,   527,   507,   553,
     554,   596,   597,   598,   482,   579,   552,   272,   331,   426,
     406,   348,   236,   258,   566,   246,   485,   515,   252,   421,
     348,   422,   692,   265,   266,   348,   368,  -300,   391,   573,
     236,   259,   260,   261,   262,   605,   484,   607,   263,   349,
     509,  -229,  -229,  -229,  -229,  -229,  -229,  -229,   349,   421,
     348,   422,   350,   349,   578,   267,   268,   419,   278,  -257,
     393,   350,   429,   -12,   -12,   423,   350,   253,   254,   255,
     -12,   351,  -174,  -260,   564,  -242,   498,  -262,   349,   604,
     351,   508,   279,   237,   394,   351,   482,  -226,   265,   266,
     430,   350,   608,  -226,   611,   423,   219,   220,  -226,  -226,
     519,   237,   215,   216,  -332,  -226,   612,  -269,   576,  -362,
     351,   371,   395,   396,   645,   236,   646,   196,   197,   601,
     526,   602,   647,   603,   198,   431,   236,   479,   221,   222,
     648,   587,   591,   595,   662,   280,   -27,  -244,   534,   535,
     536,   537,   538,   539,   540,   541,   542,   543,   544,   545,
     546,   547,   548,   549,   550,   551,   651,   684,   520,   523,
      58,   218,  -144,   555,   556,   557,   558,   559,   560,   561,
     562,   691,  -143,  -312,  -136,   382,    71,    72,    73,    74,
     533,   383,    75,    76,   489,   490,   237,   223,   224,    77,
     596,   597,   723,    55,   225,   574,   575,   237,   577,   219,
     220,   615,   616,   570,   621,  -268,  -304,   583,   598,   610,
     400,   685,   627,   629,   650,   596,   631,   633,    85,    86,
     635,   637,   597,   639,   641,   643,   644,  -340,   450,    73,
      74,   221,   222,   686,   451,  -524,   718,   223,   224,    28,
     384,   652,   385,   386,   225,   468,   106,   597,   598,   661,
    -133,   387,    27,   663,  -175,   664,  -337,   665,  -310,   666,
     649,   667,   470,   668,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,   480,    68,    69,    70,   676,   614,
      71,    72,    73,    74,   622,   655,    75,    76,   624,   620,
     657,   658,   659,    77,   512,   514,   516,   698,   572,   303,
     304,   699,   701,    79,    80,    81,    82,    83,    84,   312,
     313,   352,   354,   703,   710,   711,   712,   705,   707,   481,
     620,   716,    85,    86,   717,   571,   189,   682,  -229,  -229,
     654,   625,   656,   107,   454,   454,   719,    98,    99,   100,
     725,   101,   102,   103,   104,   105,   407,   455,   455,   529,
     106,   724,   724,   532,   282,   427,   321,   322,   283,   284,
     189,   467,  -226,   687,   688,   689,  -229,   107,  -226,     0,
     456,     0,     0,  -226,  -226,   326,   327,   328,   329,   330,
    -226,     0,   331,   669,   482,     0,   670,   671,   672,   673,
     674,   606,   709,   -34,   -34,   -34,   -34,   654,     0,     0,
     715,     0,   -34,   -34,   -34,   -34,   -34,   -34,   -34,   -34,
     -34,   -34,     0,     0,     0,     0,     0,   -34,   -34,   -34,
     -34,   -34,   -34,   -34,   -34,   -34,   -34,   -34,   -34,  -228,
    -228,  -228,  -228,  -228,   -34,   -34,   -34,   -34,   -34,     0,
       0,     0,     0,     0,     0,   -34,   -34,   -34,   -34,   -34,
     -34,   -34,   -34,   -34,   -34,     0,   -34,   -34,   -34,     0,
       0,   -34,   -34,   -34,   -34,   -34,     0,   -34,   -34,     0,
    -228,     0,     0,     0,   -34,   -34,   677,   678,   679,   670,
     671,   672,   673,     0,   -34,   -34,   -34,   -34,   -34,   -34,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   -34,   -34,     0,   -34,     0,   -34,   -34,
     -34,   -34,   -34,   -34,   -34,   -34,   -34,   -34,   -34,   -34,
     -34,     0,   -34,   -34,   -34,   -34,   -34,     0,     0,     0,
     623,   -34,   -37,   -37,   -37,   -37,     0,     0,     0,   700,
       0,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,
     -37,     0,     0,     0,     0,     0,   -37,   -37,   -37,   -37,
     -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,     0,     0,
       0,     0,     0,   -37,   -37,   -37,   -37,     0,     0,     0,
       0,     0,     0,     0,   -37,   -37,   -37,   -37,   -37,   -37,
     -37,   -37,   -37,   -37,     0,   -37,   -37,   -37,     0,     0,
     -37,   -37,   -37,   -37,  -226,     0,   -37,   -37,     0,     0,
    -226,     0,     0,   -37,   -37,  -226,  -226,     0,     0,     0,
       0,     0,  -226,   -37,   -37,   -37,   -37,   -37,   -37,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   -37,   -37,     0,   -37,     0,   -37,   -37,   -37,
     -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,
       0,   -37,   -37,   -37,   -37,   -37,     0,     0,     0,   714,
     -37,   -37,   -37,   -37,   -37,     0,     0,     0,     0,     0,
     -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,
       0,     0,     0,     0,     0,   -37,   -37,   -37,   -37,   -37,
     -37,   -37,   -37,   -37,   -37,   -37,   -37,     0,     0,     0,
       0,     0,   -37,   -37,   -37,   -37,     0,     0,     0,     0,
       0,     0,     0,   -37,   -37,   -37,   -37,   -37,   -37,   -37,
     -37,   -37,   -37,     0,   -37,   -37,   -37,     0,     0,   -37,
     -37,   -37,   -37,     0,     0,   -37,   -37,     0,     0,     0,
       0,     0,   -37,   -37,     0,     0,   210,     0,     0,     0,
       0,     0,   -37,   -37,   -37,   -37,   -37,   -37,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   -37,   -37,     0,   -37,     0,   -37,   -37,   -37,   -37,
     -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,   -37,     0,
     -37,   -37,   -37,   -37,   -37,     0,     0,     0,     0,   -37,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
       0,    68,    69,    70,     0,     0,    71,    72,    73,    74,
       0,     0,    75,    76,     0,     0,     0,     0,     0,    77,
       0,     0,     0,   212,     0,     0,     0,     0,     0,    79,
      80,    81,    82,    83,    84,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   304,    85,    86,
       0,     0,     0,     0,     0,     0,     0,   313,     0,     0,
       0,     0,     0,    98,    99,   100,     0,   101,   102,   103,
     104,   105,     0,     0,     0,     0,   106,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,     0,    68,    69,
      70,     0,     0,    71,    72,    73,    74,     0,     0,    75,
      76,     0,     0,   321,   322,     0,    77,     0,     0,     0,
     525,     0,     0,     0,     0,     0,    79,    80,    81,    82,
      83,    84,   326,   327,   328,   329,   330,     0,     0,   331,
       0,     0,   304,     0,     0,    85,    86,     0,     0,     0,
       0,     0,   313,     0,     0,     0,     0,     0,     0,     0,
      98,    99,   100,     0,   101,   102,   103,   104,   105,     0,
       0,     0,     0,   106,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,     0,    68,    69,    70,     0,     0,
      71,    72,    73,    74,   411,     0,    75,    76,   321,   322,
       0,     0,     0,    77,     0,     0,     0,   412,     0,     0,
       0,     0,     0,    79,    80,    81,    82,    83,    84,   328,
     329,   330,     0,     0,   331,     0,     0,     0,    50,     0,
      52,     0,    85,    86,     0,     0,     0,     0,   413,     0,
       0,   195,     0,     0,     0,     0,     0,    98,    99,   100,
       0,   101,   102,   103,   104,   105,    30,    31,    32,    33,
     106,     0,     0,     0,     0,    34,    35,     0,     0,    36,
      37,    38,    39,    40,    41,     0,     0,     0,     0,     0,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,     0,     0,     0,     0,     0,    54,    55,    56,
      57,     0,     0,     0,     0,     0,     0,     0,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,     0,    68,
      69,    70,     0,     0,    71,    72,    73,    74,     0,     0,
      75,    76,     0,     0,     0,     0,     0,    77,    78,     0,
       0,     0,     0,     0,     0,     0,     0,    79,    80,    81,
      82,    83,    84,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    85,    86,     0,    87,
       0,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,     0,   101,   102,   103,   104,   105,
      30,    31,    32,    33,   106,     0,     0,     0,     0,    34,
      35,     0,     0,    36,    37,    38,    39,    40,    41,     0,
       0,     0,     0,     0,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,     0,     0,     0,     0,
       0,    54,   -28,    56,    57,     0,     0,     0,     0,     0,
       0,     0,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,     0,    68,    69,    70,     0,     0,    71,    72,
      73,    74,     0,     0,    75,    76,     0,     0,     0,     0,
       0,    77,    78,     0,     0,     0,     0,     0,     0,     0,
       0,    79,    80,    81,    82,    83,    84,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      85,    86,     0,    87,     0,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,     0,   101,
     102,   103,   104,   105,    30,    31,    32,    33,   106,     0,
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
       0,     0,     0,   285,    85,    86,     0,    87,     0,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,     0,   101,   102,   103,   104,   105,     0,     0,
       0,     0,   106,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,     0,    68,    69,    70,     0,     0,    71,
      72,    73,    74,     0,     0,    75,    76,     0,     0,   286,
       0,     0,    77,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    79,    80,    81,    82,    83,    84,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    85,    86,     0,     0,     0,     0,     0,     0,  -324,
       0,  -324,     0,     0,     0,     0,    98,    99,   100,  -324,
     101,   102,   103,   104,   105,     0,     0,     0,     0,   106,
     303,   304,   305,   306,   307,   308,   309,   310,   311,     0,
     312,   313,   314,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   315,   316,
     317,   318,   319,   320,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   321,   322,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   323,   324,   325,     0,   326,   327,   328,   329,
     330,    18,     0,   331,     0,     0,     0,     0,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,     0,    68,
      69,    70,     0,     0,    71,    72,    73,    74,     0,     0,
      75,    76,     0,     0,     0,     0,     0,    77,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    79,    80,    81,
      82,    83,    84,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    85,    86,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    98,    99,   100,     0,   101,   102,   103,   104,   105,
     342,     0,     0,     0,   106,     0,     0,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,     0,    68,    69,
      70,     0,     0,    71,    72,    73,    74,     0,     0,    75,
      76,     0,     0,     0,     0,     0,    77,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    79,    80,    81,    82,
      83,    84,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    85,    86,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      98,    99,   100,     0,   101,   102,   103,   104,   105,   356,
       0,     0,     0,   106,     0,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,     0,    68,    69,    70,     0,
       0,    71,    72,    73,    74,     0,   357,    75,    76,     0,
       0,     0,     0,     0,    77,     0,     0,     0,   358,   359,
     360,     0,     0,     0,    79,    80,    81,    82,    83,    84,
       0,     0,     0,     0,     0,     0,     0,  -174,     0,     0,
       0,     0,     0,    85,    86,   303,   304,   305,   306,   307,
     308,   309,   310,   311,     0,   312,   313,   314,    98,    99,
     100,     0,   101,   102,   103,   104,   105,     0,     0,     0,
       0,   106,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   315,   316,   317,   318,   319,   320,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   321,   322,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   323,   324,   325,
       0,   326,   327,   328,   329,   330,     0,     0,   331,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,     0,
      68,    69,    70,     0,     0,    71,    72,    73,    74,     0,
       0,    75,    76,     0,     0,     0,     0,     0,    77,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    79,    80,
      81,    82,    83,    84,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    85,    86,   303,
     304,   305,   306,   307,   308,   309,   310,   311,     0,   312,
     313,   314,    98,    99,   100,     0,   101,   102,   103,   104,
     105,     0,     0,     0,     0,   106,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   315,   316,   317,
     318,   319,   320,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   321,   322,   303,   304,
     305,   306,   307,   308,   309,   310,   311,     0,   312,   313,
     314,   323,   324,   325,     0,   326,   327,   328,   329,   330,
       0,     0,   331,     0,     0,   599,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   315,   316,   317,   318,
     319,   320,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   321,   322,   303,   304,   305,
     306,   307,   308,   309,   310,   311,     0,   493,   313,   314,
     323,   324,   325,     0,   326,   327,   328,   329,   330,     0,
       0,   331,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   315,   316,   317,   318,   319,
     320,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   321,   322,   303,   304,   305,   306,
     307,   308,   309,   310,   311,     0,   312,   313,     0,   323,
     324,   325,     0,   326,   327,   328,   329,   330,     0,     0,
     331,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   315,   316,   317,   318,   319,   320,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   321,   322,   303,   304,   305,   306,   307,
     308,   309,   310,   311,     0,   312,   313,     0,     0,   324,
     325,     0,   326,   327,   328,   329,   330,     0,     0,   331,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   315,   316,   317,   318,   319,   320,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   321,   322,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   325,
       0,   326,   327,   328,   329,   330,     0,     0,   331
};

static const yytype_int16 yycheck[] =
{
      11,    15,    87,   117,   248,    18,   109,   122,    15,   121,
     129,     1,   115,     1,   498,   277,    27,   129,   193,   270,
       0,   346,   137,     1,   235,   509,     1,     0,   142,   132,
     355,   277,     1,     0,     0,    72,   576,    46,   660,     1,
     143,     1,     1,     1,    58,     1,    48,    25,    26,    48,
       0,   505,   506,    72,   113,   114,    14,   114,    77,    78,
     115,   118,   119,     1,    66,    84,    48,   607,   690,   126,
      48,     1,     8,   130,   131,    72,    75,   617,     1,   136,
     139,     1,    75,   504,    72,   506,    48,   101,   102,    48,
     205,     1,    28,    71,    72,    69,   199,    72,    72,     1,
      78,   115,   111,    72,   141,     1,    66,   504,   505,   123,
      48,   168,    72,   170,    48,   172,    72,   174,   203,   176,
      82,   178,    78,   180,   140,   182,     1,   184,    48,   186,
       1,   188,   244,   112,   246,   115,   590,     1,    48,    65,
     594,   466,   115,    66,   684,    71,    48,   261,   115,   115,
      91,    92,   142,   141,   416,    65,   407,   272,   270,    69,
     644,    71,    72,   141,    66,   586,   141,    77,    78,   140,
     416,     1,   593,    48,    84,    65,   106,   107,   108,   109,
     103,    71,   195,   106,   107,   108,   109,   110,   585,    35,
      65,    37,   589,   112,    69,    66,    71,    72,   100,   101,
     102,   140,    77,    78,    72,   262,   100,     1,   102,    84,
     106,   107,   108,   109,   142,   141,    67,   231,    48,     1,
     130,    65,   132,   133,   244,     1,   246,    71,     4,   130,
     140,   141,   103,   104,   105,   106,   107,   108,   109,   253,
     565,   255,   106,   107,   108,   109,    22,    23,    24,    25,
       1,   141,    82,    29,    48,   130,   341,   132,   133,     1,
       1,   380,   113,   114,     1,   279,   141,    26,   380,   388,
      46,   285,   365,     1,     1,   387,   388,     1,   371,   454,
     455,   100,   101,   102,    66,   496,   117,    28,   139,     1,
       1,    42,     1,     4,   142,   407,    92,   141,     1,    41,
      42,    43,     1,    79,    80,    42,     1,    48,     1,    93,
       1,    22,    23,    24,    25,   526,    91,   528,    29,    70,
     377,   103,   104,   105,   106,   107,   108,   109,    70,    41,
      42,    43,    83,    70,     9,   111,   112,     1,     1,    48,
       1,    83,     1,    71,    72,    87,    83,    50,    51,    52,
      78,   102,    48,    48,   469,    48,   370,    48,    70,   142,
     102,   375,    25,    72,    25,   102,    66,    66,    79,    80,
       1,    83,    45,    72,   141,    87,    39,    40,    77,    78,
     394,    72,    14,    15,    48,    84,   143,    48,   491,    48,
     102,    26,    53,    54,   142,     1,   142,    71,    72,   511,
     414,   513,   142,   515,    78,     1,     1,     1,    71,    72,
      14,   504,   505,   506,    15,    78,    45,    48,   432,   433,
     434,   435,   436,   437,   438,   439,   440,   441,   442,   443,
     444,   445,   446,   447,   448,   449,   611,   648,     1,     1,
      55,     1,    48,   457,   458,   459,   460,   461,   462,   463,
     464,   662,    48,    48,    48,    65,    71,    72,    73,    74,
       1,    71,    77,    78,    10,    11,    72,    77,    78,    84,
     100,   101,   716,    45,    84,   489,   490,    72,   492,    39,
      40,   574,   575,     1,   577,    48,    48,     1,   102,     1,
       1,     1,   585,   586,   608,   100,   589,   590,   113,   114,
     593,   594,   101,   596,   597,   598,   599,    48,    72,    73,
      74,    71,    72,     9,    78,    26,    14,    77,    78,    11,
     130,   614,   132,   133,    84,   336,   141,   101,   102,   622,
      48,   141,    10,   626,    48,   628,    48,   630,    48,   632,
     607,   634,   341,   636,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,   355,    66,    67,    68,   639,   573,
      71,    72,    73,    74,   578,   618,    77,    78,   579,   576,
       7,     8,     9,    84,   384,   385,   386,     1,   487,    56,
      57,   672,   673,    94,    95,    96,    97,    98,    99,    66,
      67,   228,   229,   674,   687,   688,   689,   678,   679,     1,
     607,   694,   113,   114,   697,   486,   620,   643,    10,    11,
     617,   581,   619,   620,   321,   322,   709,   128,   129,   130,
     717,   132,   133,   134,   135,   136,   269,   321,   322,   416,
     141,   716,   717,   425,   133,   284,   113,   114,   133,   133,
     654,   333,    66,   657,   658,   659,    48,   654,    72,    -1,
     322,    -1,    -1,    77,    78,   132,   133,   134,   135,   136,
      84,    -1,   139,   103,    66,    -1,   106,   107,   108,   109,
     110,     1,   686,     3,     4,     5,     6,   684,    -1,    -1,
     691,    -1,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    -1,    -1,    -1,    -1,    -1,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,     7,
       8,     9,    10,    11,    44,    45,    46,    47,    48,    -1,
      -1,    -1,    -1,    -1,    -1,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    -1,    66,    67,    68,    -1,
      -1,    71,    72,    73,    74,    75,    -1,    77,    78,    -1,
      48,    -1,    -1,    -1,    84,    85,   103,   104,   105,   106,
     107,   108,   109,    -1,    94,    95,    96,    97,    98,    99,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   113,   114,    -1,   116,    -1,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,    -1,   132,   133,   134,   135,   136,    -1,    -1,    -1,
       1,   141,     3,     4,     5,     6,    -1,    -1,    -1,     1,
      -1,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    -1,    -1,    -1,    -1,    -1,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    -1,    -1,
      -1,    -1,    -1,    44,    45,    46,    47,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    -1,    66,    67,    68,    -1,    -1,
      71,    72,    73,    74,    66,    -1,    77,    78,    -1,    -1,
      72,    -1,    -1,    84,    85,    77,    78,    -1,    -1,    -1,
      -1,    -1,    84,    94,    95,    96,    97,    98,    99,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   113,   114,    -1,   116,    -1,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
      -1,   132,   133,   134,   135,   136,    -1,    -1,    -1,     1,
     141,     3,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      -1,    -1,    -1,    -1,    -1,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    -1,    -1,    -1,
      -1,    -1,    44,    45,    46,    47,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    -1,    66,    67,    68,    -1,    -1,    71,
      72,    73,    74,    -1,    -1,    77,    78,    -1,    -1,    -1,
      -1,    -1,    84,    85,    -1,    -1,     1,    -1,    -1,    -1,
      -1,    -1,    94,    95,    96,    97,    98,    99,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   113,   114,    -1,   116,    -1,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,    -1,
     132,   133,   134,   135,   136,    -1,    -1,    -1,    -1,   141,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      -1,    66,    67,    68,    -1,    -1,    71,    72,    73,    74,
      -1,    -1,    77,    78,    -1,    -1,    -1,    -1,    -1,    84,
      -1,    -1,    -1,     1,    -1,    -1,    -1,    -1,    -1,    94,
      95,    96,    97,    98,    99,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    57,   113,   114,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,    -1,    -1,
      -1,    -1,    -1,   128,   129,   130,    -1,   132,   133,   134,
     135,   136,    -1,    -1,    -1,    -1,   141,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    -1,    66,    67,
      68,    -1,    -1,    71,    72,    73,    74,    -1,    -1,    77,
      78,    -1,    -1,   113,   114,    -1,    84,    -1,    -1,    -1,
       1,    -1,    -1,    -1,    -1,    -1,    94,    95,    96,    97,
      98,    99,   132,   133,   134,   135,   136,    -1,    -1,   139,
      -1,    -1,    57,    -1,    -1,   113,   114,    -1,    -1,    -1,
      -1,    -1,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     128,   129,   130,    -1,   132,   133,   134,   135,   136,    -1,
      -1,    -1,    -1,   141,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    -1,    66,    67,    68,    -1,    -1,
      71,    72,    73,    74,     1,    -1,    77,    78,   113,   114,
      -1,    -1,    -1,    84,    -1,    -1,    -1,    14,    -1,    -1,
      -1,    -1,    -1,    94,    95,    96,    97,    98,    99,   134,
     135,   136,    -1,    -1,   139,    -1,    -1,    -1,    35,    -1,
      37,    -1,   113,   114,    -1,    -1,    -1,    -1,    45,    -1,
      -1,    48,    -1,    -1,    -1,    -1,    -1,   128,   129,   130,
      -1,   132,   133,   134,   135,   136,     3,     4,     5,     6,
     141,    -1,    -1,    -1,    -1,    12,    13,    -1,    -1,    16,
      17,    18,    19,    20,    21,    -1,    -1,    -1,    -1,    -1,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    -1,    -1,    -1,    -1,    -1,    44,    45,    46,
      47,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    -1,    66,
      67,    68,    -1,    -1,    71,    72,    73,    74,    -1,    -1,
      77,    78,    -1,    -1,    -1,    -1,    -1,    84,    85,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,    95,    96,
      97,    98,    99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   113,   114,    -1,   116,
      -1,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,    -1,   132,   133,   134,   135,   136,
       3,     4,     5,     6,   141,    -1,    -1,    -1,    -1,    12,
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
     113,   114,    -1,   116,    -1,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,    -1,   132,
     133,   134,   135,   136,     3,     4,     5,     6,   141,    -1,
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
      -1,    -1,    -1,    25,   113,   114,    -1,   116,    -1,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,    -1,   132,   133,   134,   135,   136,    -1,    -1,
      -1,    -1,   141,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    -1,    66,    67,    68,    -1,    -1,    71,
      72,    73,    74,    -1,    -1,    77,    78,    -1,    -1,    81,
      -1,    -1,    84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    94,    95,    96,    97,    98,    99,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   113,   114,    -1,    -1,    -1,    -1,    -1,    -1,    35,
      -1,    37,    -1,    -1,    -1,    -1,   128,   129,   130,    45,
     132,   133,   134,   135,   136,    -1,    -1,    -1,    -1,   141,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    -1,
      66,    67,    68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,    95,
      96,    97,    98,    99,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   113,   114,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   128,   129,   130,    -1,   132,   133,   134,   135,
     136,    48,    -1,   139,    -1,    -1,    -1,    -1,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    -1,    66,
      67,    68,    -1,    -1,    71,    72,    73,    74,    -1,    -1,
      77,    78,    -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,    95,    96,
      97,    98,    99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   113,   114,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   128,   129,   130,    -1,   132,   133,   134,   135,   136,
      48,    -1,    -1,    -1,   141,    -1,    -1,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    -1,    66,    67,
      68,    -1,    -1,    71,    72,    73,    74,    -1,    -1,    77,
      78,    -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    94,    95,    96,    97,
      98,    99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   113,   114,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     128,   129,   130,    -1,   132,   133,   134,   135,   136,    49,
      -1,    -1,    -1,   141,    -1,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    -1,    66,    67,    68,    -1,
      -1,    71,    72,    73,    74,    -1,    76,    77,    78,    -1,
      -1,    -1,    -1,    -1,    84,    -1,    -1,    -1,    88,    89,
      90,    -1,    -1,    -1,    94,    95,    96,    97,    98,    99,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    48,    -1,    -1,
      -1,    -1,    -1,   113,   114,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    -1,    66,    67,    68,   128,   129,
     130,    -1,   132,   133,   134,   135,   136,    -1,    -1,    -1,
      -1,   141,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    94,    95,    96,    97,    98,    99,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   113,   114,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   128,   129,   130,
      -1,   132,   133,   134,   135,   136,    -1,    -1,   139,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    -1,
      66,    67,    68,    -1,    -1,    71,    72,    73,    74,    -1,
      -1,    77,    78,    -1,    -1,    -1,    -1,    -1,    84,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,    95,
      96,    97,    98,    99,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   113,   114,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    -1,    66,
      67,    68,   128,   129,   130,    -1,   132,   133,   134,   135,
     136,    -1,    -1,    -1,    -1,   141,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    94,    95,    96,
      97,    98,    99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   113,   114,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    -1,    66,    67,
      68,   128,   129,   130,    -1,   132,   133,   134,   135,   136,
      -1,    -1,   139,    -1,    -1,   142,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    94,    95,    96,    97,
      98,    99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   113,   114,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    -1,    66,    67,    68,
     128,   129,   130,    -1,   132,   133,   134,   135,   136,    -1,
      -1,   139,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    94,    95,    96,    97,    98,
      99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   113,   114,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    -1,    66,    67,    -1,   128,
     129,   130,    -1,   132,   133,   134,   135,   136,    -1,    -1,
     139,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    94,    95,    96,    97,    98,    99,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   113,   114,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    -1,    66,    67,    -1,    -1,   129,
     130,    -1,   132,   133,   134,   135,   136,    -1,    -1,   139,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    94,    95,    96,    97,    98,    99,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   113,   114,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   130,
      -1,   132,   133,   134,   135,   136,    -1,    -1,   139
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,   145,   146,     0,   147,   160,   161,   115,   148,   149,
     150,   154,   156,   157,   162,   163,   164,   276,    48,    75,
     159,   183,   255,   256,     1,   151,   149,   154,   148,   156,
       3,     4,     5,     6,    12,    13,    16,    17,    18,    19,
      20,    21,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    44,    45,    46,    47,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    66,    67,
      68,    71,    72,    73,    74,    77,    78,    84,    85,    94,
      95,    96,    97,    98,    99,   113,   114,   116,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   132,   133,   134,   135,   136,   141,   163,   165,   166,
     167,   168,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   182,   184,   185,   186,   187,   188,   189,
     190,   191,   192,   193,   194,   195,   196,   197,   198,   202,
     203,   208,   209,   210,   211,   247,   248,   249,   250,   251,
     252,   253,   254,   257,   258,   259,   261,   262,   266,   267,
     268,   269,   270,   271,   273,   278,   286,   288,   289,   290,
     291,   292,   293,   294,   295,   296,   297,   298,   299,   300,
     301,   302,   303,   304,   305,   306,   307,   308,   309,   310,
     339,   340,   347,   348,   161,    48,    71,    72,    78,   152,
     153,     1,    72,   141,   357,   358,   310,   169,   170,   358,
       1,   310,     1,   310,   311,    14,    15,   159,     1,    39,
      40,    71,    72,    77,    78,    84,   206,   281,   282,   283,
     352,   212,   310,   356,   159,   310,     1,    72,   359,   359,
     356,   356,   199,     1,    69,   264,   339,   349,   353,   357,
     310,     1,     1,    50,    51,    52,   356,     1,     4,    22,
      23,    24,    25,    29,    46,    79,    80,   111,   112,   263,
     265,     1,    28,   349,   356,   356,     1,   159,     1,    25,
      78,   279,   281,   282,   283,    25,    81,   287,   356,   357,
     359,   159,   356,   356,   356,   356,   356,   356,   356,   356,
     356,   356,   356,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    66,    67,    68,    94,    95,    96,    97,    98,
      99,   113,   114,   128,   129,   130,   132,   133,   134,   135,
     136,   139,   341,   342,   341,   161,    66,   159,   358,   357,
       1,   140,    48,   338,   354,   356,   204,     1,    42,    70,
      83,   102,   284,     1,   284,   205,    49,    76,    88,    89,
      90,   213,   214,   218,   244,   310,   160,     1,     1,     1,
      25,    26,    71,    72,    78,   141,   200,   219,   222,   353,
     140,   353,    65,    71,   130,   132,   133,   141,   350,   351,
     352,     1,   310,     1,    25,    53,    54,   260,   310,   359,
       1,   356,   112,   112,    46,   111,     1,   265,   264,     1,
     357,     1,    14,    45,   194,   195,   274,   275,   310,     1,
       1,    41,    43,    87,   284,   285,     1,   285,   310,     1,
       1,     1,   323,   316,   325,   327,   326,   328,   329,   330,
     331,   322,   321,   320,   332,   333,   334,   335,   336,   337,
      72,    78,   343,   344,   347,   348,   343,   318,   319,   324,
     312,   313,   314,   315,   317,   345,   346,   338,   153,   142,
     169,     1,   142,   140,   207,   354,     1,    82,   280,     1,
     207,     1,    66,   240,    91,    92,   215,   216,   217,    10,
      11,   246,   130,    66,   240,     1,    14,   201,   310,     1,
     223,   224,   225,   226,   230,   231,   232,   240,   310,   356,
     349,   141,   351,   141,   351,   141,   351,   339,   349,   310,
       1,    26,   264,     1,     1,     1,   310,     1,   195,   275,
     277,     1,   280,     1,   310,   310,   310,   310,   310,   310,
     310,   310,   310,   310,   310,   310,   310,   310,   310,   310,
     310,   310,   117,   341,   341,   310,   310,   310,   310,   310,
     310,   310,   310,   354,   357,   355,   142,     1,    72,    78,
       1,   216,   215,    93,   310,   310,   159,   310,     9,   160,
     220,   221,   220,     1,   227,   231,   232,   240,   228,   230,
     232,   240,   229,   230,   231,   240,   100,   101,   102,   142,
     220,   339,   339,   339,   142,   160,     1,   160,    45,   272,
       1,   141,   143,   354,   310,   240,   240,   155,   158,   162,
     163,   240,   310,     1,   156,   222,   232,   240,   231,   240,
     232,   240,   230,   240,   231,   240,   230,   240,     1,   240,
       1,   240,     1,   240,   240,   142,   142,   142,    14,   158,
     359,   341,   240,   162,   163,   210,   163,     7,     8,     9,
     245,   240,    15,   240,   240,   240,   240,   240,   240,   103,
     106,   107,   108,   109,   110,   233,   235,   103,   104,   105,
     234,   235,   234,   220,   160,     1,     9,   310,   310,   310,
     245,   160,     1,   238,   239,     1,   236,   237,     1,   238,
       1,   238,     1,   235,     1,   235,     1,   235,   162,   310,
     240,   240,   240,   245,     1,   156,   240,   240,    14,   240,
     241,   242,   243,   352,   358,   242
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   144,   146,   145,   147,   147,   147,   147,   148,   148,
     149,   149,   151,   150,   150,   152,   152,   153,   153,   153,
     154,   154,   155,   155,   156,   156,   157,   158,   158,   159,
     159,   160,   160,   161,   161,   162,   162,   163,   164,   165,
     165,   165,   165,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     167,   167,   168,   168,   169,   169,   170,   170,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   189,   190,   191,   192,
     193,   194,   195,   196,   197,   199,   198,   200,   200,   200,
     201,   200,   200,   200,   202,   204,   203,   205,   203,   203,
     203,   203,   203,   203,   203,   203,   203,   206,   206,   207,
     207,   208,   209,   210,   210,   210,   211,   212,   212,   212,
     213,   214,   215,   216,   217,   217,   217,   217,   218,   218,
     218,   218,   218,   218,   218,   218,   218,   218,   218,   219,
     219,   219,   221,   220,   220,   222,   222,   222,   223,   223,
     223,   223,   224,   225,   226,   227,   227,   227,   227,   227,
     228,   228,   228,   228,   228,   229,   229,   229,   229,   229,
     230,   230,   231,   231,   232,   232,   233,   233,   233,   233,
     234,   234,   234,   234,   234,   234,   235,   235,   235,   235,
     235,   235,   235,   235,   237,   236,   239,   238,   240,   240,
     241,   241,   242,   243,   244,   245,   245,   245,   246,   246,
     246,   247,   247,   248,   248,   249,   250,   250,   250,   250,
     250,   250,   251,   252,   253,   254,   254,   254,   255,   256,
     257,   257,   257,   258,   258,   259,   259,   259,   259,   259,
     259,   259,   259,   259,   259,   260,   260,   261,   262,   262,
     262,   262,   263,   263,   263,   263,   263,   263,   263,   264,
     264,   265,   265,   265,   265,   265,   265,   265,   265,   265,
     266,   266,   266,   266,   266,   267,   268,   269,   270,   271,
     272,   272,   272,   273,   273,   273,   273,   273,   273,   274,
     274,   274,   275,   275,   275,   275,   276,   276,   277,   277,
     278,   278,   278,   278,   278,   278,   278,   278,   278,   278,
     278,   279,   279,   279,   280,   280,   280,   281,   281,   282,
     283,   284,   284,   284,   284,   285,   285,   285,   285,   286,
     286,   286,   286,   287,   288,   289,   290,   291,   292,   293,
     294,   295,   296,   297,   298,   299,   300,   301,   302,   303,
     304,   305,   306,   307,   308,   309,   311,   310,   312,   310,
     313,   310,   314,   310,   310,   315,   310,   310,   316,   310,
     310,   317,   310,   310,   318,   310,   310,   319,   310,   310,
     320,   310,   310,   321,   310,   310,   322,   310,   310,   323,
     310,   310,   310,   310,   324,   310,   310,   325,   310,   310,
     326,   310,   310,   327,   310,   310,   328,   310,   310,   329,
     310,   310,   330,   310,   310,   331,   310,   310,   332,   310,
     310,   333,   310,   310,   334,   310,   310,   335,   310,   310,
     336,   310,   310,   337,   310,   310,   310,   310,   310,   310,
     310,   310,   310,   310,   310,   310,   310,   310,   310,   310,
     310,   338,   338,   338,   339,   340,   340,   342,   341,   343,
     343,   343,   343,   343,   343,   344,   346,   345,   347,   348,
     349,   349,   349,   350,   350,   350,   350,   350,   350,   350,
     350,   350,   351,   351,   352,   352,   352,   353,   353,   353,
     355,   354,   354,   356,   356,   357,   357,   357,   357,   358,
     359,   359
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
       3,     1,     1,     1,     1,     1,     1,     2,     2,     2,
       2,     2,     2,     1,     1,     2,     2,     0,     2,     1,
       2,     4,     1,     2,     2,     1,     0,     3,     1,     1,
       3,     1,     1,     2,     2,     1,     2,     3,     4,     4,
       4,     1,     1,     1,     1,     1,     1,     2,     2,     0,
       0,     4,     1,     1,     0,     2,     1,     4,     3,     1,
       1,     1
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
#line 171 "yaccsrc.y"
                                       { level = 0;
                                         if ( get_options_flag( parser_data.TSD->currlevel, EXT_CALLS_AS_FUNCS )
                                           && !get_options_flag( parser_data.TSD->currlevel, EXT_STRICT_ANSI ) )
                                            start_parendepth = 1;
                                         else
                                            start_parendepth = 0;
                                         parendepth = 0; }
#line 2562 "yaccsrc.c"
    break;

  case 4: /* prog: nlncl stats  */
#line 181 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-1], yyvsp[0] );
                                         yyval->o.last = NULL;
                                         EndProg( yyval ) ; }
#line 2570 "yaccsrc.c"
    break;

  case 5: /* prog: nlncl  */
#line 184 "yaccsrc.y"
                                       { yyval = yyvsp[0];
                                         if ( yyval != NULL )
                                            yyval->o.last = NULL;
                                         EndProg( yyval ); }
#line 2579 "yaccsrc.c"
    break;

  case 6: /* prog: nlncl stats dirsect  */
#line 188 "yaccsrc.y"
                                       { yyval = optgluelast( makenode( X_OO_INIT, 0 ), yyvsp[-2] );
                                         yyval = optgluelast( yyval, yyvsp[-1] );
                                         yyval = optgluelast( yyval, yyvsp[0] );
                                         yyval->o.last = NULL;
                                         EndProg( yyval ) ; }
#line 2589 "yaccsrc.c"
    break;

  case 7: /* prog: nlncl dirsect  */
#line 193 "yaccsrc.y"
                                       { yyval = optgluelast( makenode( X_OO_INIT, 0 ), yyvsp[-1] );
                                         yyval = optgluelast( yyval, yyvsp[0] );
                                         yyval->o.last = NULL;
                                         EndProg( yyval ) ; }
#line 2598 "yaccsrc.c"
    break;

  case 8: /* dirsect: dirsect dirblock  */
#line 204 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 2604 "yaccsrc.c"
    break;

  case 9: /* dirsect: dirblock  */
#line 205 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2610 "yaccsrc.c"
    break;

  case 10: /* dirblock: directive stats  */
#line 208 "yaccsrc.y"
                                       { yyval = yyvsp[-1];
                                         yyval->p[1] = optgluelast( yyval->p[1], yyvsp[0] );
                                         if ( yyval->p[1] != NULL )
                                            yyval->p[1]->o.last = NULL; }
#line 2619 "yaccsrc.c"
    break;

  case 11: /* dirblock: directive  */
#line 212 "yaccsrc.y"
                                       { yyval = yyvsp[0];
                                         if ( yyval->p[1] != NULL )
                                            yyval->p[1]->o.last = NULL; }
#line 2627 "yaccsrc.c"
    break;

  case 12: /* @2: %empty  */
#line 217 "yaccsrc.y"
                                       { yyval = makenode( X_DIRECTIVE, 0 );
                                         yyval->lineno = parser_data.tline;
                                         yyval->charnr = parser_data.tstart; }
#line 2635 "yaccsrc.c"
    break;

  case 13: /* directive: DCOLON @2 dirwords ncl  */
#line 220 "yaccsrc.y"
                                       { yyval = yyvsp[-2];
                                         /* take over the keyword; the
                                          * node must not own it twice */
                                         yyval->name = yyvsp[-1]->name;
                                         yyvsp[-1]->name = NULL;
                                         yyval->p[0] = yyvsp[-1]->next;
                                         if ( yyval->p[0] != NULL )
                                            yyval->p[0]->o.last = NULL;
                                         yyval->p[1] = yyvsp[0]; /* labels */ }
#line 2649 "yaccsrc.c"
    break;

  case 14: /* directive: DCOLON error  */
#line 229 "yaccsrc.y"
                                       { exiterror( ERR_YACC_SYNTAX, 1, parser_data.tline ); }
#line 2655 "yaccsrc.c"
    break;

  case 15: /* dirwords: dirwords SPACE dirword  */
#line 232 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-2], yyvsp[0] ); }
#line 2661 "yaccsrc.c"
    break;

  case 16: /* dirwords: dirword  */
#line 233 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2667 "yaccsrc.c"
    break;

  case 17: /* dirword: SIMSYMBOL  */
#line 236 "yaccsrc.y"
                                       { yyval = makenode( X_STRING, 0 );
                                         yyval->name = Str_cre_TSD( parser_data.TSD, retvalue ); }
#line 2674 "yaccsrc.c"
    break;

  case 18: /* dirword: CONSYMBOL  */
#line 238 "yaccsrc.y"
                                       { yyval = makenode( X_STRING, 0 );
                                         yyval->name = Str_cre_TSD( parser_data.TSD, retvalue ); }
#line 2681 "yaccsrc.c"
    break;

  case 19: /* dirword: STRING  */
#line 240 "yaccsrc.y"
                                       { yyval = makenode( X_STRING, 0 );
                                         yyval->name = Str_cre_TSD( parser_data.TSD, retvalue ); }
#line 2688 "yaccsrc.c"
    break;

  case 20: /* stats: stats ystatement  */
#line 244 "yaccsrc.y"
                                       { /* fixes bug 579711 */
                                         yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 2695 "yaccsrc.c"
    break;

  case 21: /* stats: ystatement  */
#line 246 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2701 "yaccsrc.c"
    break;

  case 22: /* xstats: xstats statement  */
#line 249 "yaccsrc.y"
                                       { /* fixes bug 579711 */
                                         yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 2708 "yaccsrc.c"
    break;

  case 23: /* xstats: statement gruff  */
#line 251 "yaccsrc.y"
                                       { yyval = yyvsp[-1]; }
#line 2714 "yaccsrc.c"
    break;

  case 24: /* ystatement: statement  */
#line 254 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 2720 "yaccsrc.c"
    break;

  case 25: /* ystatement: lonely_end  */
#line 255 "yaccsrc.y"
                                       { exiterror( ERR_UNMATCHED_END, 1 ); }
#line 2726 "yaccsrc.c"
    break;

  case 27: /* nxstats: xstats  */
#line 263 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2732 "yaccsrc.c"
    break;

  case 28: /* nxstats: gruff  */
#line 264 "yaccsrc.y"
                                       { yyval = NULL; }
#line 2738 "yaccsrc.c"
    break;

  case 29: /* ncl: ncl STATSEP optLabels  */
#line 267 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-2], yyvsp[0] ); }
#line 2744 "yaccsrc.c"
    break;

  case 30: /* ncl: STATSEP optLabels  */
#line 268 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2750 "yaccsrc.c"
    break;

  case 31: /* nlncl: optLabels ncl  */
#line 271 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 2756 "yaccsrc.c"
    break;

  case 32: /* nlncl: optLabels  */
#line 272 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2762 "yaccsrc.c"
    break;

  case 33: /* optLabels: optLabels label_stat  */
#line 275 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 2768 "yaccsrc.c"
    break;

  case 34: /* optLabels: %empty  */
#line 276 "yaccsrc.y"
                                       { yyval = NULL; }
#line 2774 "yaccsrc.c"
    break;

  case 37: /* gruff: %empty  */
#line 283 "yaccsrc.y"
                                       { tmpchr=parser_data.tstart;
                                         tmplno=parser_data.tline; }
#line 2781 "yaccsrc.c"
    break;

  case 38: /* mttstatement: gruff mtstatement  */
#line 287 "yaccsrc.y"
                                       { yyval=yyvsp[0]; }
#line 2787 "yaccsrc.c"
    break;

  case 39: /* mtstatement: nclstatement ncl  */
#line 290 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 2793 "yaccsrc.c"
    break;

  case 80: /* oo_expose_stat: EXPOSE anyvars  */
#line 339 "yaccsrc.y"
                                       { yyval = makenode( X_OO_EXPOSE, 1, yyvsp[0] );
                                         yyval->lineno = tmplno;
                                         yyval->charnr = tmpchr; }
#line 2801 "yaccsrc.c"
    break;

  case 81: /* oo_expose_stat: EXPOSE error  */
#line 342 "yaccsrc.y"
                                       { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ); }
#line 2807 "yaccsrc.c"
    break;

  case 82: /* oo_usearg_stat: USEARG usevars  */
#line 345 "yaccsrc.y"
                                       { yyval = makenode( X_OO_USEARG, 1, yyvsp[0] );
                                         yyval->lineno = tmplno;
                                         yyval->charnr = tmpchr; }
#line 2815 "yaccsrc.c"
    break;

  case 83: /* oo_usearg_stat: USEARG usevars error  */
#line 348 "yaccsrc.y"
                                       { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ); }
#line 2821 "yaccsrc.c"
    break;

  case 84: /* usevars: usevar ',' usevars  */
#line 351 "yaccsrc.y"
                                       { yyval = yyvsp[-2];
                                         yyval->p[0] = yyvsp[0]; }
#line 2828 "yaccsrc.c"
    break;

  case 85: /* usevars: usevar  */
#line 353 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 2834 "yaccsrc.c"
    break;

  case 86: /* usevar: xsimsymb  */
#line 356 "yaccsrc.y"
                                       { yyval = makenode( X_SIM_SYMBOL, 0 );
                                         yyval->name = (streng *) yyvsp[0]; }
#line 2841 "yaccsrc.c"
    break;

  case 87: /* usevar: %empty  */
#line 358 "yaccsrc.y"
                                       { yyval = makenode( X_SIM_SYMBOL, 0 );
                                         yyval->name = NULL; }
#line 2848 "yaccsrc.c"
    break;

  case 88: /* call: CALL  */
#line 362 "yaccsrc.y"
                                       { yyval = makenode(X_CALL,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2856 "yaccsrc.c"
    break;

  case 89: /* do: DO  */
#line 366 "yaccsrc.y"
                                       { yyval = makenode(X_DO,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ;
                                         /* OO: LOOP, see finish_do() */
                                         yyval->u.nonansi = __regina_do_is_loop ;
                                         level++; }
#line 2867 "yaccsrc.c"
    break;

  case 90: /* exit: EXIT  */
#line 373 "yaccsrc.y"
                                       { yyval = makenode(X_EXIT,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2875 "yaccsrc.c"
    break;

  case 91: /* if: IF  */
#line 377 "yaccsrc.y"
                                       { yyval = makenode(X_IF,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ;
                                         level++; }
#line 2884 "yaccsrc.c"
    break;

  case 92: /* iterate: ITERATE  */
#line 382 "yaccsrc.y"
                                       { yyval = makenode(X_ITERATE,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2892 "yaccsrc.c"
    break;

  case 93: /* leave: LEAVE  */
#line 386 "yaccsrc.y"
                                       { yyval = makenode(X_LEAVE,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2900 "yaccsrc.c"
    break;

  case 94: /* say: SAY  */
#line 390 "yaccsrc.y"
                                       { yyval = makenode(X_SAY,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2908 "yaccsrc.c"
    break;

  case 95: /* return: RETURN  */
#line 394 "yaccsrc.y"
                                       { yyval = makenode(X_RETURN,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2916 "yaccsrc.c"
    break;

  case 96: /* address: ADDRESS  */
#line 398 "yaccsrc.y"
                                       { yyval = makenode(X_ADDR_N,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2924 "yaccsrc.c"
    break;

  case 97: /* arg: ARG  */
#line 402 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_ARG,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2932 "yaccsrc.c"
    break;

  case 98: /* drop: DROP  */
#line 406 "yaccsrc.y"
                                       { yyval = makenode(X_DROP,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2940 "yaccsrc.c"
    break;

  case 99: /* interpret: INTERPRET  */
#line 410 "yaccsrc.y"
                                       { yyval = makenode(X_IPRET,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2948 "yaccsrc.c"
    break;

  case 100: /* label: LABEL  */
#line 414 "yaccsrc.y"
                                       { yyval = makenode(X_LABEL,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2956 "yaccsrc.c"
    break;

  case 101: /* nop: NOP  */
#line 418 "yaccsrc.y"
                                       { yyval = makenode(X_NULL,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2964 "yaccsrc.c"
    break;

  case 102: /* numeric: NUMERIC  */
#line 422 "yaccsrc.y"
                                       { yyval = makenode(0,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2972 "yaccsrc.c"
    break;

  case 103: /* options: OPTIONS  */
#line 426 "yaccsrc.y"
                                       { yyval = makenode(X_OPTIONS,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2980 "yaccsrc.c"
    break;

  case 104: /* parse: PARSE  */
#line 430 "yaccsrc.y"
                                       { yyval = makenode(0,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2988 "yaccsrc.c"
    break;

  case 105: /* proc: PROCEDURE  */
#line 434 "yaccsrc.y"
                                       { yyval = makenode(X_PROC,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 2996 "yaccsrc.c"
    break;

  case 106: /* pull: PULL  */
#line 438 "yaccsrc.y"
                                       { yyval = makenode(X_PULL,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3004 "yaccsrc.c"
    break;

  case 107: /* push: PUSH  */
#line 442 "yaccsrc.y"
                                       { yyval = makenode(X_PUSH,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3012 "yaccsrc.c"
    break;

  case 108: /* queue: QUEUE  */
#line 446 "yaccsrc.y"
                                       { yyval = makenode(X_QUEUE,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3020 "yaccsrc.c"
    break;

  case 109: /* select: SELECT  */
#line 450 "yaccsrc.y"
                                       { yyval = makenode(X_SELECT,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ;
                                         level++; }
#line 3029 "yaccsrc.c"
    break;

  case 110: /* signal: SIGNAL  */
#line 455 "yaccsrc.y"
                                       { yyval = makenode(X_SIG_LAB,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3037 "yaccsrc.c"
    break;

  case 111: /* when: WHEN  */
#line 459 "yaccsrc.y"
                                       { yyval = makenode(X_WHEN,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3045 "yaccsrc.c"
    break;

  case 112: /* otherwise: OTHERWISE  */
#line 463 "yaccsrc.y"
                                       { yyval = makenode(X_OTHERWISE,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3053 "yaccsrc.c"
    break;

  case 113: /* trace: TRACE  */
#line 467 "yaccsrc.y"
                                       { yyval = makenode(X_TRACE,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3061 "yaccsrc.c"
    break;

  case 114: /* upper: UPPER  */
#line 471 "yaccsrc.y"
                                       { yyval = makenode(X_UPPER_VAR,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3069 "yaccsrc.c"
    break;

  case 115: /* @3: %empty  */
#line 475 "yaccsrc.y"
                                       { yyval = current = yyvsp[0] ; }
#line 3075 "yaccsrc.c"
    break;

  case 117: /* address_stat2: VALUE expr naddr_with  */
#line 479 "yaccsrc.y"
                                       { current->type = X_ADDR_V ;
                                         current->p[0] = yyvsp[-1] ;
                                         current->p[1] = yyvsp[0] ; }
#line 3083 "yaccsrc.c"
    break;

  case 118: /* address_stat2: addr_with  */
#line 482 "yaccsrc.y"
                                       { exiterror( ERR_STRING_EXPECTED, 1, __reginatext ) ;}
#line 3089 "yaccsrc.c"
    break;

  case 119: /* address_stat2: %empty  */
#line 483 "yaccsrc.y"
                                       { current->type = X_ADDR_S ; }
#line 3095 "yaccsrc.c"
    break;

  case 120: /* $@4: %empty  */
#line 484 "yaccsrc.y"
                                       { exiterror( ERR_STRING_EXPECTED, 1, __reginatext ) ;}
#line 3101 "yaccsrc.c"
    break;

  case 122: /* address_stat2: nvir nexpr naddr_with  */
#line 486 "yaccsrc.y"
                                       { current->name = (streng *)yyvsp[-2] ;
                                         current->type = X_ADDR_N ;
                                         current->p[0] = yyvsp[-1] ;
                                         current->p[1] = yyvsp[0] ; }
#line 3110 "yaccsrc.c"
    break;

  case 123: /* address_stat2: '(' expr ')' nspace naddr_with  */
#line 490 "yaccsrc.y"
                                              { current->type = X_ADDR_V ;
                                         current->p[0] = yyvsp[-3] ;
                                         current->p[1] = yyvsp[0] ;
                                         current->u.nonansi = 1 ; }
#line 3119 "yaccsrc.c"
    break;

  case 124: /* arg_stat: arg templs  */
#line 496 "yaccsrc.y"
                                       { yyval = makenode( X_PARSE, 2, yyvsp[-1], yyvsp[0] );
                                         yyval->u.parseflags = PARSE_UPPER;
                                         yyval->lineno = yyvsp[-1]->lineno;
                                         yyval->charnr = yyvsp[-1]->charnr; }
#line 3128 "yaccsrc.c"
    break;

  case 125: /* $@5: %empty  */
#line 502 "yaccsrc.y"
                                       { parendepth = start_parendepth; }
#line 3134 "yaccsrc.c"
    break;

  case 126: /* call_stat: call call_name $@5 call_args  */
#line 503 "yaccsrc.y"
                                       { yyval = yyvsp[-3];
                                         yyval->p[0] = yyvsp[0];
                                         yyval->name = (streng *) yyvsp[-2];
                                         parendepth = 0; }
#line 3143 "yaccsrc.c"
    break;

  case 127: /* $@6: %empty  */
#line 507 "yaccsrc.y"
                                       { parendepth = start_parendepth; }
#line 3149 "yaccsrc.c"
    break;

  case 128: /* call_stat: call string $@6 call_args  */
#line 508 "yaccsrc.y"
                                       { yyval = yyvsp[-3];
                                         yyval->type = X_EX_FUNC;
                                         yyval->p[0] = yyvsp[0];
                                         yyval->name = (streng *) yyvsp[-2];
                                         parendepth = 0; }
#line 3159 "yaccsrc.c"
    break;

  case 129: /* call_stat: call on error  */
#line 513 "yaccsrc.y"
                             { exiterror( ERR_INV_SUBKEYWORD, 1, "ERROR FAILURE HALT NOTREADY", __reginatext ) ;}
#line 3165 "yaccsrc.c"
    break;

  case 130: /* call_stat: call off error  */
#line 514 "yaccsrc.y"
                              { exiterror( ERR_INV_SUBKEYWORD, 2, "ERROR FAILURE HALT NOTREADY", __reginatext ) ;}
#line 3171 "yaccsrc.c"
    break;

  case 131: /* call_stat: call on c_action error  */
#line 515 "yaccsrc.y"
                                      { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 3177 "yaccsrc.c"
    break;

  case 132: /* call_stat: call on c_action namespec error  */
#line 516 "yaccsrc.y"
                                               { exiterror( ERR_STRING_EXPECTED, 3, __reginatext ) ;}
#line 3183 "yaccsrc.c"
    break;

  case 133: /* call_stat: call on c_action namespec  */
#line 518 "yaccsrc.y"
                                       { yyval = yyvsp[-3] ;
                                         yyval->type = X_CALL_SET ;
                                         yyval->p[0] = yyvsp[-2] ;
                                         yyval->name = (streng *)yyvsp[0] ;
                                         yyval->p[1] = yyvsp[-1] ; }
#line 3193 "yaccsrc.c"
    break;

  case 134: /* call_stat: call on c_action  */
#line 523 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_CALL_SET ;
                                         yyval->p[0] = yyvsp[-1] ;
                                         yyval->name = NULL ;
                                         yyval->p[1] = yyvsp[0] ; }
#line 3203 "yaccsrc.c"
    break;

  case 135: /* call_stat: call off c_action error  */
#line 528 "yaccsrc.y"
                                       { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 3209 "yaccsrc.c"
    break;

  case 136: /* call_stat: call off c_action  */
#line 529 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_CALL_SET ;
                                         yyval->p[0] = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ; }
#line 3218 "yaccsrc.c"
    break;

  case 137: /* call_name: asymbol  */
#line 535 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 3224 "yaccsrc.c"
    break;

  case 138: /* call_name: error  */
#line 536 "yaccsrc.y"
                                       { exiterror( ERR_STRING_EXPECTED, 2, __reginatext );}
#line 3230 "yaccsrc.c"
    break;

  case 139: /* call_args: exprs  */
#line 539 "yaccsrc.y"
                                       {
                                         /*
                                          * "call_args" accepted probably with
                                          * surrounding parentheses. Strip them.
                                          */
                                         yyval = reduce_expr_list( yyvsp[0],
                                                                REDUCE_CALL );
                                       }
#line 3243 "yaccsrc.c"
    break;

  case 140: /* call_args: exprs ')'  */
#line 547 "yaccsrc.y"
                                       { exiterror(ERR_UNEXPECTED_PARAN, 2); }
#line 3249 "yaccsrc.c"
    break;

  case 141: /* expr_stat: expr  */
#line 550 "yaccsrc.y"
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
#line 3274 "yaccsrc.c"
    break;

  case 142: /* end_stat: END  */
#line 572 "yaccsrc.y"
                                       { yyval = makenode(X_END,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ;
                                         level--; }
#line 3283 "yaccsrc.c"
    break;

  case 143: /* end: end_stat simsymb  */
#line 578 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->name = (streng*)(yyvsp[0]) ; }
#line 3290 "yaccsrc.c"
    break;

  case 144: /* end: end_stat  */
#line 580 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3296 "yaccsrc.c"
    break;

  case 145: /* end: end_stat simsymb error  */
#line 581 "yaccsrc.y"
                                       {  exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 3302 "yaccsrc.c"
    break;

  case 146: /* do_stat: do do_opts repetitor conditional ncl nxstats end  */
#line 585 "yaccsrc.y"
                                       { yyval = yyvsp[-6];
                                         yyval->p[0] = yyvsp[-4];
                                         yyval->p[1] = yyvsp[-3];
                                         yyval->p[2] = optgluelast( yyvsp[-2], yyvsp[-1] );
                                         if ( yyval->p[2] )
                                            yyval->p[2]->o.last = NULL;
                                         yyval->p[3] = yyvsp[0];
                                         finish_do( yyval, yyvsp[-5] ); }
#line 3315 "yaccsrc.c"
    break;

  case 147: /* do_opts: %empty  */
#line 596 "yaccsrc.y"
                                       { yyval = NULL ; }
#line 3321 "yaccsrc.c"
    break;

  case 148: /* do_opts: do_opts dolabel  */
#line 597 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ? yyvsp[-1] : makenode(X_OO_DOOPTS,0) ;
                                         if ( yyval->name )
                                            exiterror( ERR_INVALID_DO_SYNTAX, 1, "LABEL" ) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 3330 "yaccsrc.c"
    break;

  case 149: /* do_opts: do_opts docounter  */
#line 601 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ? yyvsp[-1] : makenode(X_OO_DOOPTS,0) ;
                                         if ( yyval->p[0] )
                                            exiterror( ERR_INVALID_DO_SYNTAX, 1, "COUNTER" ) ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3339 "yaccsrc.c"
    break;

  case 150: /* dolabel: DOLABEL  */
#line 607 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 3345 "yaccsrc.c"
    break;

  case 151: /* docounter: DOCOUNTER  */
#line 609 "yaccsrc.y"
                                       { yyval = create_head( retvalue ) ; }
#line 3351 "yaccsrc.c"
    break;

  case 152: /* doindex: DOINDEX  */
#line 611 "yaccsrc.y"
                                       { yyval = create_head( retvalue ) ; }
#line 3357 "yaccsrc.c"
    break;

  case 153: /* doitem: DOITEM  */
#line 613 "yaccsrc.y"
                                       { yyval = create_head( retvalue ) ; }
#line 3363 "yaccsrc.c"
    break;

  case 154: /* withvars: doindex  */
#line 617 "yaccsrc.y"
                                       { yyval = makenode(X_OO_DOOPTS,2,yyvsp[0],NULL) ; }
#line 3369 "yaccsrc.c"
    break;

  case 155: /* withvars: doitem  */
#line 618 "yaccsrc.y"
                                       { yyval = makenode(X_OO_DOOPTS,2,NULL,yyvsp[0]) ; }
#line 3375 "yaccsrc.c"
    break;

  case 156: /* withvars: doindex doitem  */
#line 619 "yaccsrc.y"
                                       { yyval = makenode(X_OO_DOOPTS,2,yyvsp[-1],yyvsp[0]) ; }
#line 3381 "yaccsrc.c"
    break;

  case 157: /* withvars: doitem doindex  */
#line 620 "yaccsrc.y"
                                       { yyval = makenode(X_OO_DOOPTS,2,yyvsp[0],yyvsp[-1]) ; }
#line 3387 "yaccsrc.c"
    break;

  case 158: /* repetitor: dovar '=' expr nspace tobyfor tobyfor tobyfor  */
#line 624 "yaccsrc.y"
                                       { yyval =makenode(X_REP,4,yyvsp[-4],yyvsp[-2],yyvsp[-1],yyvsp[0]) ;
                                         yyval->name = (streng *)yyvsp[-6] ;
                                         checkdosyntax(yyval) ; }
#line 3395 "yaccsrc.c"
    break;

  case 159: /* repetitor: dovar '=' expr nspace tobyfor tobyfor  */
#line 628 "yaccsrc.y"
                                       { yyval =makenode(X_REP,3,yyvsp[-3],yyvsp[-1],yyvsp[0]) ;
                                         yyval->name = (streng *)yyvsp[-5] ;
                                         checkdosyntax(yyval) ; }
#line 3403 "yaccsrc.c"
    break;

  case 160: /* repetitor: dovar '=' expr nspace tobyfor  */
#line 632 "yaccsrc.y"
                                       { yyval = makenode(X_REP,2,yyvsp[-2],yyvsp[0]) ;
                                         yyval->name = (streng *)yyvsp[-4] ;
                                         checkdosyntax(yyval) ; }
#line 3411 "yaccsrc.c"
    break;

  case 161: /* repetitor: dovar '=' expr nspace  */
#line 635 "yaccsrc.y"
                                       { yyval = makenode(X_REP,1,yyvsp[-1]) ;
                                         yyval->name = (streng *)yyvsp[-3] ;
                                         checkdosyntax(yyval) ; }
#line 3419 "yaccsrc.c"
    break;

  case 162: /* repetitor: FOREVER nspace  */
#line 638 "yaccsrc.y"
                                       { yyval = makenode(X_REP_FOREVER,0) ; }
#line 3425 "yaccsrc.c"
    break;

  case 163: /* repetitor: FOREVER error  */
#line 639 "yaccsrc.y"
                             { exiterror( ERR_INV_SUBKEYWORD, 16, "WHILE UNTIL", __reginatext ) ; }
#line 3431 "yaccsrc.c"
    break;

  case 164: /* repetitor: expr nspace  */
#line 640 "yaccsrc.y"
                                       { yyvsp[-1] = makenode(X_DO_EXPR,1,yyvsp[-1]) ;
                                         yyval = makenode(X_REP,2,NULL,yyvsp[-1]) ; }
#line 3438 "yaccsrc.c"
    break;

  case 165: /* repetitor: expr nspace FOR expr nspace  */
#line 644 "yaccsrc.y"
                                       { yyvsp[-4] = makenode(X_DO_EXPR,1,yyvsp[-4]) ;
                                         yyval = makenode(X_REP,3,NULL,yyvsp[-4],
                                                       makenode(X_DO_FOR,1,yyvsp[-1])) ;
                                         if ( do_over_var( yyval ) == NULL )
                                            exiterror( ERR_INVALID_DO_SYNTAX, 1, "FOR" ) ; }
#line 3448 "yaccsrc.c"
    break;

  case 166: /* repetitor: DOWITH withvars DOOVER expr nspace  */
#line 650 "yaccsrc.y"
                                       { yyval = makenode(X_OO_DOWITH,4,yyvsp[-1],yyvsp[-3]->p[0],yyvsp[-3]->p[1],NULL) ; }
#line 3454 "yaccsrc.c"
    break;

  case 167: /* repetitor: DOWITH withvars DOOVER expr nspace FOR expr nspace  */
#line 652 "yaccsrc.y"
                                       { yyval = makenode(X_OO_DOWITH,4,yyvsp[-4],yyvsp[-6]->p[0],yyvsp[-6]->p[1],
                                                       makenode(X_DO_FOR,1,yyvsp[-1])) ; }
#line 3461 "yaccsrc.c"
    break;

  case 168: /* repetitor: %empty  */
#line 654 "yaccsrc.y"
                                       { yyval = NULL ; }
#line 3467 "yaccsrc.c"
    break;

  case 169: /* nvir: CONSYMBOL  */
#line 657 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 3473 "yaccsrc.c"
    break;

  case 170: /* nvir: SIMSYMBOL  */
#line 658 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 3479 "yaccsrc.c"
    break;

  case 171: /* nvir: STRING  */
#line 659 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 3485 "yaccsrc.c"
    break;

  case 172: /* @7: %empty  */
#line 662 "yaccsrc.y"
                                       { SymbolDetect |= SD_ADDRWITH ;
                                         yyval = with = makenode(X_ADDR_WITH,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3494 "yaccsrc.c"
    break;

  case 173: /* naddr_with: @7 addr_with  */
#line 666 "yaccsrc.y"
                                       { with = NULL ;
                                         SymbolDetect &= ~SD_ADDRWITH ; }
#line 3501 "yaccsrc.c"
    break;

  case 174: /* naddr_with: %empty  */
#line 668 "yaccsrc.y"
                                       { yyval = NULL ; }
#line 3507 "yaccsrc.c"
    break;

  case 175: /* addr_with: WITH connection  */
#line 671 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 3513 "yaccsrc.c"
    break;

  case 176: /* addr_with: WITH connection error  */
#line 672 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 5, __reginatext ) ; }
#line 3519 "yaccsrc.c"
    break;

  case 177: /* addr_with: WITH nspace  */
#line 673 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 5, __reginatext ) ; }
#line 3525 "yaccsrc.c"
    break;

  case 181: /* connection: error  */
#line 679 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 5, __reginatext ) ; }
#line 3531 "yaccsrc.c"
    break;

  case 200: /* inputstmt: nspace INPUT nspace resourcei  */
#line 715 "yaccsrc.y"
                                             { with->p[0] = yyvsp[0]; }
#line 3537 "yaccsrc.c"
    break;

  case 201: /* inputstmt: nspace INPUT error  */
#line 716 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 6, __reginatext ) ; }
#line 3543 "yaccsrc.c"
    break;

  case 202: /* outputstmt: nspace OUTPUT nspace resourceo  */
#line 719 "yaccsrc.y"
                                              { with->p[1] = yyvsp[0]; }
#line 3549 "yaccsrc.c"
    break;

  case 203: /* outputstmt: nspace OUTPUT error  */
#line 720 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 7, __reginatext ) ; }
#line 3555 "yaccsrc.c"
    break;

  case 204: /* errorstmt: nspace ERROR nspace resourceo  */
#line 723 "yaccsrc.y"
                                             { with->p[2] = yyvsp[0]; }
#line 3561 "yaccsrc.c"
    break;

  case 205: /* errorstmt: nspace ERROR error  */
#line 724 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 14, __reginatext ) ; }
#line 3567 "yaccsrc.c"
    break;

  case 206: /* resourcei: resources  */
#line 727 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3573 "yaccsrc.c"
    break;

  case 207: /* resourcei: NOEOL resources  */
#line 728 "yaccsrc.y"
                                       { yyval = yyvsp[0] ;
                                         yyval->u.of.noeol = 1 ; }
#line 3580 "yaccsrc.c"
    break;

  case 208: /* resourcei: NOEOL error  */
#line 730 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 8, __reginatext ) ; }
#line 3586 "yaccsrc.c"
    break;

  case 209: /* resourcei: NORMAL  */
#line 731 "yaccsrc.y"
                                       { yyval = makenode(X_ADDR_WITH, 0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3594 "yaccsrc.c"
    break;

  case 210: /* resourceo: resources  */
#line 736 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3600 "yaccsrc.c"
    break;

  case 211: /* resourceo: APPEND resources  */
#line 737 "yaccsrc.y"
                                       { yyval = yyvsp[0] ;
                                         yyval->u.of.append = 1 ; }
#line 3607 "yaccsrc.c"
    break;

  case 212: /* resourceo: APPEND error  */
#line 739 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 8, __reginatext ) ; }
#line 3613 "yaccsrc.c"
    break;

  case 213: /* resourceo: REPLACE resources  */
#line 740 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3619 "yaccsrc.c"
    break;

  case 214: /* resourceo: REPLACE error  */
#line 741 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 9, __reginatext ) ; }
#line 3625 "yaccsrc.c"
    break;

  case 215: /* resourceo: NORMAL  */
#line 742 "yaccsrc.y"
                                       { yyval = makenode(X_ADDR_WITH, 0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 3633 "yaccsrc.c"
    break;

  case 216: /* resources: STREAM nnvir  */
#line 747 "yaccsrc.y"
                                       { /* ANSI extension: nsimsymb is
                                          * used by the standard, but I think
                                          * there are no reasons why using
                                          * it here as a must. FGC
                                          */
                                         yyval = yyvsp[0] ;
                                         yyval->u.of.awt = awtSTREAM;
                                         SymbolDetect |= SD_ADDRWITH ; }
#line 3646 "yaccsrc.c"
    break;

  case 217: /* resources: STREAM error  */
#line 755 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_OPTION, 1, __reginatext ) ; }
#line 3652 "yaccsrc.c"
    break;

  case 218: /* resources: STEM nsimsymb  */
#line 756 "yaccsrc.y"
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
#line 3670 "yaccsrc.c"
    break;

  case 219: /* resources: STEM error  */
#line 769 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_OPTION, 2, __reginatext ) ; }
#line 3676 "yaccsrc.c"
    break;

  case 220: /* resources: LIFO nnvir  */
#line 770 "yaccsrc.y"
                                       {
                                         yyval = yyvsp[0] ;
                                         yyval->u.of.awt = awtLIFO ;
                                         SymbolDetect |= SD_ADDRWITH ; }
#line 3685 "yaccsrc.c"
    break;

  case 221: /* resources: LIFO error  */
#line 774 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_OPTION, 100, __reginatext ) ; }
#line 3691 "yaccsrc.c"
    break;

  case 222: /* resources: FIFO nnvir  */
#line 775 "yaccsrc.y"
                                       {
                                         yyval = yyvsp[0] ;
                                         yyval->u.of.awt = awtFIFO ;
                                         SymbolDetect |= SD_ADDRWITH ; }
#line 3700 "yaccsrc.c"
    break;

  case 223: /* resources: FIFO error  */
#line 779 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_OPTION, 101, __reginatext ) ; }
#line 3706 "yaccsrc.c"
    break;

  case 224: /* $@8: %empty  */
#line 782 "yaccsrc.y"
                                       { SymbolDetect &= ~SD_ADDRWITH ; }
#line 3712 "yaccsrc.c"
    break;

  case 225: /* nsimsymb: $@8 nspace addrSim  */
#line 783 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3718 "yaccsrc.c"
    break;

  case 226: /* $@9: %empty  */
#line 786 "yaccsrc.y"
                                       { SymbolDetect &= ~SD_ADDRWITH ; }
#line 3724 "yaccsrc.c"
    break;

  case 227: /* nnvir: $@9 nspace addrAll  */
#line 787 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3730 "yaccsrc.c"
    break;

  case 230: /* addrAll: addrSim  */
#line 794 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3736 "yaccsrc.c"
    break;

  case 231: /* addrAll: addrString  */
#line 795 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3742 "yaccsrc.c"
    break;

  case 232: /* addrSim: xsimsymb  */
#line 798 "yaccsrc.y"
                                       { yyval = makenode(X_ADDR_WITH, 0) ;
                                         yyval->name = (streng *) yyvsp[0] ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->u.of.ant = antSIMSYMBOL;
                                       }
#line 3753 "yaccsrc.c"
    break;

  case 233: /* addrString: string  */
#line 806 "yaccsrc.y"
                                       { yyval = makenode(X_ADDR_WITH, 0) ;
                                         yyval->name = (streng *) yyvsp[0] ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->u.of.ant = antSTRING;
                                       }
#line 3764 "yaccsrc.c"
    break;

  case 234: /* dovar: DOVARIABLE  */
#line 814 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 3770 "yaccsrc.c"
    break;

  case 235: /* tobyfor: TO expr nspace  */
#line 817 "yaccsrc.y"
                                       { yyval = makenode(X_DO_TO,1,yyvsp[-1]) ; }
#line 3776 "yaccsrc.c"
    break;

  case 236: /* tobyfor: FOR expr nspace  */
#line 818 "yaccsrc.y"
                                       { yyval = makenode(X_DO_FOR,1,yyvsp[-1]) ; }
#line 3782 "yaccsrc.c"
    break;

  case 237: /* tobyfor: BY expr nspace  */
#line 819 "yaccsrc.y"
                                       { yyval = makenode(X_DO_BY,1,yyvsp[-1]) ; }
#line 3788 "yaccsrc.c"
    break;

  case 238: /* conditional: WHILE expr nspace  */
#line 822 "yaccsrc.y"
                                       { yyval = makenode(X_WHILE,1,yyvsp[-1]) ; }
#line 3794 "yaccsrc.c"
    break;

  case 239: /* conditional: UNTIL expr nspace  */
#line 823 "yaccsrc.y"
                                       { yyval = makenode(X_UNTIL,1,yyvsp[-1]) ; }
#line 3800 "yaccsrc.c"
    break;

  case 240: /* conditional: %empty  */
#line 824 "yaccsrc.y"
                                       { yyval = NULL ; }
#line 3806 "yaccsrc.c"
    break;

  case 241: /* drop_stat: drop anyvars error  */
#line 827 "yaccsrc.y"
                                  { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ) ;}
#line 3812 "yaccsrc.c"
    break;

  case 242: /* drop_stat: drop anyvars  */
#line 828 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3819 "yaccsrc.c"
    break;

  case 243: /* upper_stat: upper anyvars error  */
#line 832 "yaccsrc.y"
                                   { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ) ;}
#line 3825 "yaccsrc.c"
    break;

  case 244: /* upper_stat: upper anyvars  */
#line 833 "yaccsrc.y"
                                        { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3832 "yaccsrc.c"
    break;

  case 245: /* exit_stat: exit nexpr  */
#line 837 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3839 "yaccsrc.c"
    break;

  case 246: /* if_stat: if expr nlncl THEN nlncl ystatement  */
#line 842 "yaccsrc.y"
                                       { move_labels( yyvsp[-5], yyvsp[0], level - 1 );
                                         yyval = yyvsp[-5];
                                         yyval->p[0] = optgluelast( yyvsp[-4], yyvsp[-3] );
                                         yyval->p[0]->o.last = NULL;
                                         yyval->p[1] = optgluelast( yyvsp[-1], yyvsp[0] );
                                         yyval->p[1]->o.last = NULL;
                                         level--; }
#line 3851 "yaccsrc.c"
    break;

  case 247: /* if_stat: if expr nlncl THEN nlncl ystatement ELSE nlncl ystatement  */
#line 850 "yaccsrc.y"
                                       { move_labels( yyvsp[-8], yyvsp[0], level - 1 );
                                         yyval = yyvsp[-8];
                                         yyval->p[0] = optgluelast( yyvsp[-7], yyvsp[-6] );
                                         yyval->p[0]->o.last = NULL;
                                         yyval->p[1] = optgluelast( yyvsp[-4], yyvsp[-3] );
                                         yyval->p[1]->o.last = NULL;
                                         yyval->p[2] = optgluelast( yyvsp[-1], yyvsp[0] );
                                         yyval->p[2]->o.last = NULL;
                                         level--; }
#line 3865 "yaccsrc.c"
    break;

  case 248: /* if_stat: if expr nlncl THEN nlncl ystatement ELSE nlncl error  */
#line 860 "yaccsrc.y"
                                       {  exiterror( ERR_INCOMPLETE_STRUCT, 4 ) ;}
#line 3871 "yaccsrc.c"
    break;

  case 249: /* if_stat: if expr nlncl THEN nlncl error  */
#line 862 "yaccsrc.y"
                                       {  exiterror( ERR_INCOMPLETE_STRUCT, 3 ) ;}
#line 3877 "yaccsrc.c"
    break;

  case 250: /* if_stat: if ncl  */
#line 863 "yaccsrc.y"
                                       {  exiterror( ERR_INCOMPLETE_STRUCT, 0 ) ;}
#line 3883 "yaccsrc.c"
    break;

  case 251: /* if_stat: if expr nlncl error  */
#line 864 "yaccsrc.y"
                                       {  exiterror( ERR_THEN_EXPECTED, 1, parser_data.if_linenr, __reginatext )  ; }
#line 3889 "yaccsrc.c"
    break;

  case 252: /* unexp_then: gruff THEN  */
#line 867 "yaccsrc.y"
                                       {  exiterror( ERR_THEN_UNEXPECTED, 1 )  ; }
#line 3895 "yaccsrc.c"
    break;

  case 253: /* unexp_else: gruff ELSE  */
#line 870 "yaccsrc.y"
                                       {  exiterror( ERR_THEN_UNEXPECTED, 2 )  ; }
#line 3901 "yaccsrc.c"
    break;

  case 254: /* ipret_stat: interpret expr  */
#line 873 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3908 "yaccsrc.c"
    break;

  case 255: /* iterate_stat: iterate simsymb  */
#line 878 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 3915 "yaccsrc.c"
    break;

  case 256: /* iterate_stat: iterate simsymb error  */
#line 880 "yaccsrc.y"
                                     { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 3921 "yaccsrc.c"
    break;

  case 257: /* iterate_stat: iterate  */
#line 881 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3927 "yaccsrc.c"
    break;

  case 258: /* label_stat: labelname  */
#line 884 "yaccsrc.y"
                                       { yyval = yyvsp[0] ;
                                         yyval->u.trace_only =
                                                         (level == 0) ? 0 : 1;
                                         newlabel( (const tsd_t *)parser_data.TSD,
                                                   &parser_data,
                                                   yyvsp[0] ) ; }
#line 3938 "yaccsrc.c"
    break;

  case 259: /* labelname: label  */
#line 892 "yaccsrc.y"
                                       { yyval = yyvsp[0] ;
                                         yyval->name = Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 3945 "yaccsrc.c"
    break;

  case 260: /* leave_stat: leave simsymb  */
#line 896 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 3952 "yaccsrc.c"
    break;

  case 261: /* leave_stat: leave simsymb error  */
#line 898 "yaccsrc.y"
                                   { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 3958 "yaccsrc.c"
    break;

  case 262: /* leave_stat: leave  */
#line 899 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3964 "yaccsrc.c"
    break;

  case 263: /* nop_stat: nop  */
#line 902 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 3970 "yaccsrc.c"
    break;

  case 264: /* nop_stat: nop error  */
#line 903 "yaccsrc.y"
                         {  exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 3976 "yaccsrc.c"
    break;

  case 265: /* numeric_stat: numeric DIGITS expr  */
#line 906 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_NUM_D ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 3984 "yaccsrc.c"
    break;

  case 266: /* numeric_stat: numeric DIGITS  */
#line 909 "yaccsrc.y"
                                       { yyval = yyvsp[-1]; yyval->type = X_NUM_DDEF ; }
#line 3990 "yaccsrc.c"
    break;

  case 267: /* numeric_stat: numeric FORM form_expr error  */
#line 910 "yaccsrc.y"
                                            { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 3996 "yaccsrc.c"
    break;

  case 268: /* numeric_stat: numeric FORM form_expr  */
#line 911 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_NUM_F ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4004 "yaccsrc.c"
    break;

  case 269: /* numeric_stat: numeric FORM  */
#line 914 "yaccsrc.y"
                                       { /* NOTE! This clashes ANSI! */
                                         yyval = yyvsp[-1] ; yyval->type=X_NUM_FRMDEF ;}
#line 4011 "yaccsrc.c"
    break;

  case 270: /* numeric_stat: numeric FORM VALUE expr  */
#line 916 "yaccsrc.y"
                                       { yyval = yyvsp[-3] ; yyval->type=X_NUM_V ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4018 "yaccsrc.c"
    break;

  case 271: /* numeric_stat: numeric FORM error  */
#line 918 "yaccsrc.y"
                                  { exiterror( ERR_INV_SUBKEYWORD, 11, "ENGINEERING SCIENTIFIC", __reginatext ) ;}
#line 4024 "yaccsrc.c"
    break;

  case 272: /* numeric_stat: numeric FUZZ  */
#line 919 "yaccsrc.y"
                                       { yyval = yyvsp[-1]; yyval->type = X_NUM_FDEF ;}
#line 4030 "yaccsrc.c"
    break;

  case 273: /* numeric_stat: numeric FUZZ expr  */
#line 920 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_NUM_FUZZ ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4038 "yaccsrc.c"
    break;

  case 274: /* numeric_stat: numeric error  */
#line 923 "yaccsrc.y"
                             { exiterror( ERR_INV_SUBKEYWORD, 15, "DIGITS FORM FUZZ", __reginatext ) ;}
#line 4044 "yaccsrc.c"
    break;

  case 275: /* form_expr: SCIENTIFIC  */
#line 926 "yaccsrc.y"
                                       { yyval = makenode(X_NUM_SCI,0) ; }
#line 4050 "yaccsrc.c"
    break;

  case 276: /* form_expr: ENGINEERING  */
#line 927 "yaccsrc.y"
                                       { yyval = makenode(X_NUM_ENG,0) ; }
#line 4056 "yaccsrc.c"
    break;

  case 277: /* options_stat: options nexpr  */
#line 930 "yaccsrc.y"
                                       { (yyval=yyvsp[-1])->p[0]=yyvsp[0] ; }
#line 4062 "yaccsrc.c"
    break;

  case 278: /* parse_stat: parse parse_flags parse_param templs  */
#line 934 "yaccsrc.y"
                                       { yyval = yyvsp[-3] ;
                                         yyval->type = X_PARSE ;
                                         yyval->u.parseflags = (int) yyvsp[-2] ;
                                         yyval->p[0] = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ; }
#line 4072 "yaccsrc.c"
    break;

  case 279: /* parse_stat: parse parse_param templs  */
#line 940 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_PARSE ;
                                         yyval->u.parseflags = 0;
                                         yyval->p[0] = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ; }
#line 4082 "yaccsrc.c"
    break;

  case 280: /* parse_stat: parse parse_flags error  */
#line 945 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 12, "ARG EXTERNAL LINEIN PULL SOURCE VAR VALUE VERSION", __reginatext ) ;}
#line 4088 "yaccsrc.c"
    break;

  case 281: /* parse_stat: parse error  */
#line 946 "yaccsrc.y"
                                       { exiterror( ERR_INV_SUBKEYWORD, 12, "ARG CASELESS EXTERNAL LINEIN LOWER PULL SOURCE UPPER VAR VALUE VERSION", __reginatext ) ;}
#line 4094 "yaccsrc.c"
    break;

  case 282: /* parse_flags: UPPER  */
#line 949 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_UPPER  |
                                                         PARSE_NORMAL); }
#line 4101 "yaccsrc.c"
    break;

  case 283: /* parse_flags: UPPER CASELESS  */
#line 951 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_UPPER  |
                                                         PARSE_CASELESS); }
#line 4108 "yaccsrc.c"
    break;

  case 284: /* parse_flags: CASELESS UPPER  */
#line 953 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_UPPER  |
                                                         PARSE_CASELESS); }
#line 4115 "yaccsrc.c"
    break;

  case 285: /* parse_flags: LOWER  */
#line 955 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_LOWER  |
                                                         PARSE_NORMAL); }
#line 4122 "yaccsrc.c"
    break;

  case 286: /* parse_flags: LOWER CASELESS  */
#line 957 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_LOWER  |
                                                         PARSE_CASELESS); }
#line 4129 "yaccsrc.c"
    break;

  case 287: /* parse_flags: CASELESS LOWER  */
#line 959 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_LOWER  |
                                                         PARSE_CASELESS); }
#line 4136 "yaccsrc.c"
    break;

  case 288: /* parse_flags: CASELESS  */
#line 961 "yaccsrc.y"
                                       { yyval = (nodeptr) (PARSE_NORMAL |
                                                         PARSE_CASELESS); }
#line 4143 "yaccsrc.c"
    break;

  case 289: /* templs: templs ',' template  */
#line 965 "yaccsrc.y"
                                       { /* fixes bugs like bug 579711 */
                                         yyval = optgluelast( yyvsp[-2], yyvsp[0] ); }
#line 4150 "yaccsrc.c"
    break;

  case 290: /* templs: template  */
#line 967 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 4156 "yaccsrc.c"
    break;

  case 291: /* parse_param: ARG  */
#line 970 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_ARG,0) ; }
#line 4162 "yaccsrc.c"
    break;

  case 292: /* parse_param: LINEIN  */
#line 971 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_EXT,0) ; }
#line 4168 "yaccsrc.c"
    break;

  case 293: /* parse_param: EXTERNAL  */
#line 972 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_EXT,0) ; }
#line 4174 "yaccsrc.c"
    break;

  case 294: /* parse_param: VERSION  */
#line 973 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_VER,0) ; }
#line 4180 "yaccsrc.c"
    break;

  case 295: /* parse_param: PULL  */
#line 974 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_PULL,0) ; }
#line 4186 "yaccsrc.c"
    break;

  case 296: /* parse_param: SOURCE  */
#line 975 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_SRC,0) ; }
#line 4192 "yaccsrc.c"
    break;

  case 297: /* parse_param: VAR simsymb  */
#line 976 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_VAR,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 4199 "yaccsrc.c"
    break;

  case 298: /* parse_param: VALUE nexpr WITH  */
#line 978 "yaccsrc.y"
                                       { yyval = makenode(X_PARSE_VAL,1,yyvsp[-1]) ; }
#line 4205 "yaccsrc.c"
    break;

  case 299: /* parse_param: VALUE error  */
#line 979 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_TEMPLATE, 3 ) ;}
#line 4211 "yaccsrc.c"
    break;

  case 300: /* proc_stat: proc  */
#line 982 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 4217 "yaccsrc.c"
    break;

  case 301: /* proc_stat: proc error  */
#line 983 "yaccsrc.y"
                          { exiterror( ERR_INV_SUBKEYWORD, 17, __reginatext ) ;}
#line 4223 "yaccsrc.c"
    break;

  case 302: /* proc_stat: proc EXPOSE error  */
#line 984 "yaccsrc.y"
                                 { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ) ;}
#line 4229 "yaccsrc.c"
    break;

  case 303: /* proc_stat: proc EXPOSE anyvars error  */
#line 985 "yaccsrc.y"
                                         { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ) ;}
#line 4235 "yaccsrc.c"
    break;

  case 304: /* proc_stat: proc EXPOSE anyvars  */
#line 986 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4242 "yaccsrc.c"
    break;

  case 305: /* pull_stat: pull template  */
#line 990 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4249 "yaccsrc.c"
    break;

  case 306: /* push_stat: push nexpr  */
#line 994 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4256 "yaccsrc.c"
    break;

  case 307: /* queue_stat: queue nexpr  */
#line 998 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4263 "yaccsrc.c"
    break;

  case 308: /* say_stat: say nexpr  */
#line 1002 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4270 "yaccsrc.c"
    break;

  case 309: /* return_stat: return nexpr  */
#line 1006 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4277 "yaccsrc.c"
    break;

  case 310: /* sel_end: END simsymb  */
#line 1010 "yaccsrc.y"
                                       { exiterror( ERR_UNMATCHED_END, 0 ) ;}
#line 4283 "yaccsrc.c"
    break;

  case 311: /* sel_end: END simsymb error  */
#line 1011 "yaccsrc.y"
                                       { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 4289 "yaccsrc.c"
    break;

  case 312: /* sel_end: END  */
#line 1012 "yaccsrc.y"
                                       { level--; }
#line 4295 "yaccsrc.c"
    break;

  case 313: /* select_stat: select ncl when_stats otherwise_stat sel_end  */
#line 1016 "yaccsrc.y"
                                       { yyval = yyvsp[-4];
                                         yyval->p[0] = optgluelast( yyvsp[-3], yyvsp[-2] );
                                         yyval->p[0]->o.last = NULL;
                                         yyval->p[1] = yyvsp[-1]; }
#line 4304 "yaccsrc.c"
    break;

  case 314: /* select_stat: select ncl END error  */
#line 1020 "yaccsrc.y"
                                       {  exiterror( ERR_WHEN_EXPECTED, 1, parser_data.select_linenr, __reginatext ) ;}
#line 4310 "yaccsrc.c"
    break;

  case 315: /* select_stat: select ncl otherwise error  */
#line 1022 "yaccsrc.y"
                                       {  exiterror( ERR_WHEN_EXPECTED, 2, parser_data.select_linenr, __reginatext ) ;}
#line 4316 "yaccsrc.c"
    break;

  case 316: /* select_stat: select error  */
#line 1023 "yaccsrc.y"
                                       {  exiterror( ERR_EXTRA_DATA, 1, __reginatext )  ;}
#line 4322 "yaccsrc.c"
    break;

  case 317: /* select_stat: select ncl THEN  */
#line 1024 "yaccsrc.y"
                                       {  exiterror( ERR_THEN_UNEXPECTED, 0 ) ;}
#line 4328 "yaccsrc.c"
    break;

  case 318: /* select_stat: select ncl when_stats otherwise error  */
#line 1026 "yaccsrc.y"
                                       {  exiterror( ERR_INCOMPLETE_STRUCT, 0 ) ;}
#line 4334 "yaccsrc.c"
    break;

  case 319: /* when_stats: when_stats when_stat  */
#line 1029 "yaccsrc.y"
                                       { yyval = optgluelast( yyvsp[-1], yyvsp[0] ); }
#line 4340 "yaccsrc.c"
    break;

  case 320: /* when_stats: when_stat  */
#line 1030 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 4346 "yaccsrc.c"
    break;

  case 321: /* when_stats: error  */
#line 1031 "yaccsrc.y"
                                       {  exiterror( ERR_WHEN_EXPECTED, 2, parser_data.select_linenr, __reginatext )  ;}
#line 4352 "yaccsrc.c"
    break;

  case 322: /* when_stat: when expr nlncl THEN nlncl statement  */
#line 1035 "yaccsrc.y"
                                       { yyval = yyvsp[-5]; /* fixes bugs like bug 579711 */
                                         yyval->p[0] = optgluelast( yyvsp[-4], yyvsp[-3] );
                                         yyval->p[0]->o.last = NULL;
                                         yyval->p[1] = optgluelast( yyvsp[-1], yyvsp[0] );
                                         yyval->p[1]->o.last = NULL; }
#line 4362 "yaccsrc.c"
    break;

  case 323: /* when_stat: when expr nlncl THEN nlncl statement THEN  */
#line 1041 "yaccsrc.y"
                                       {  exiterror( ERR_THEN_UNEXPECTED, 0 ) ;}
#line 4368 "yaccsrc.c"
    break;

  case 324: /* when_stat: when expr  */
#line 1042 "yaccsrc.y"
                                       {  exiterror( ERR_THEN_EXPECTED, 2, parser_data.select_linenr, __reginatext )  ; }
#line 4374 "yaccsrc.c"
    break;

  case 325: /* when_stat: when error  */
#line 1043 "yaccsrc.y"
                                       {  exiterror( ERR_INVALID_EXPRESSION, 0 ) ;}
#line 4380 "yaccsrc.c"
    break;

  case 326: /* ex_when_stat: gruff when  */
#line 1053 "yaccsrc.y"
                                       { exiterror( ERR_WHEN_UNEXPECTED, 1 ); }
#line 4386 "yaccsrc.c"
    break;

  case 327: /* ex_when_stat: gruff otherwise  */
#line 1054 "yaccsrc.y"
                                       { exiterror( ERR_WHEN_UNEXPECTED, 2 ); }
#line 4392 "yaccsrc.c"
    break;

  case 328: /* otherwise_stat: otherwise nlncl nxstats  */
#line 1057 "yaccsrc.y"
                                         { yyval = yyvsp[-2];
                                         yyval->p[0] = optgluelast( yyvsp[-1], yyvsp[0] );
                                         if ( yyval->p[0] )
                                            yyval->p[0]->o.last = NULL; }
#line 4401 "yaccsrc.c"
    break;

  case 329: /* otherwise_stat: %empty  */
#line 1061 "yaccsrc.y"
                                       { yyval = makenode(X_NO_OTHERWISE,0) ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->charnr = parser_data.tstart ; }
#line 4409 "yaccsrc.c"
    break;

  case 330: /* signal_stat: signal VALUE expr  */
#line 1067 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_SIG_VAL ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4417 "yaccsrc.c"
    break;

  case 331: /* signal_stat: signal signal_name error  */
#line 1070 "yaccsrc.y"
                                       { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 4423 "yaccsrc.c"
    break;

  case 332: /* signal_stat: signal signal_name  */
#line 1071 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->name = (streng *)yyvsp[0] ; }
#line 4430 "yaccsrc.c"
    break;

  case 333: /* signal_stat: signal on error  */
#line 1073 "yaccsrc.y"
                              { exiterror( ERR_INV_SUBKEYWORD, 3, "ERROR FAILURE HALT NOTREADY NOVALUE SYNTAX LOSTDIGITS", __reginatext ) ;}
#line 4436 "yaccsrc.c"
    break;

  case 334: /* signal_stat: signal off error  */
#line 1074 "yaccsrc.y"
                               { exiterror( ERR_INV_SUBKEYWORD, 4, "ERROR FAILURE HALT NOTREADY NOVALUE SYNTAX LOSTDIGITS", __reginatext ) ;}
#line 4442 "yaccsrc.c"
    break;

  case 335: /* signal_stat: signal on s_action error  */
#line 1075 "yaccsrc.y"
                                       { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 4448 "yaccsrc.c"
    break;

  case 336: /* signal_stat: signal on s_action namespec error  */
#line 1076 "yaccsrc.y"
                                                { exiterror( ERR_STRING_EXPECTED, 3, __reginatext ) ;}
#line 4454 "yaccsrc.c"
    break;

  case 337: /* signal_stat: signal on s_action namespec  */
#line 1078 "yaccsrc.y"
                                       { yyval = yyvsp[-3] ;
                                         yyval->type = X_SIG_SET ;
                                         yyval->p[0] = yyvsp[-2] ;
                                         yyval->name = (streng *)yyvsp[0] ;
                                         yyval->p[1] = yyvsp[-1] ; }
#line 4464 "yaccsrc.c"
    break;

  case 338: /* signal_stat: signal on s_action  */
#line 1083 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_SIG_SET ;
                                         yyval->p[0] = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ; }
#line 4473 "yaccsrc.c"
    break;

  case 339: /* signal_stat: signal off s_action error  */
#line 1087 "yaccsrc.y"
                                        { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 4479 "yaccsrc.c"
    break;

  case 340: /* signal_stat: signal off s_action  */
#line 1088 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->type = X_SIG_SET ;
                                         yyval->p[0] = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ; }
#line 4488 "yaccsrc.c"
    break;

  case 341: /* signal_name: asymbol  */
#line 1094 "yaccsrc.y"
                                       { yyval = yyvsp[0]; }
#line 4494 "yaccsrc.c"
    break;

  case 342: /* signal_name: STRING  */
#line 1095 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 4500 "yaccsrc.c"
    break;

  case 343: /* signal_name: error  */
#line 1096 "yaccsrc.y"
                                       { exiterror( ERR_STRING_EXPECTED, 4, __reginatext );}
#line 4506 "yaccsrc.c"
    break;

  case 344: /* namespec: NAME SIMSYMBOL  */
#line 1099 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue);}
#line 4512 "yaccsrc.c"
    break;

  case 345: /* namespec: NAME STRING  */
#line 1100 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 4518 "yaccsrc.c"
    break;

  case 346: /* namespec: NAME error  */
#line 1101 "yaccsrc.y"
                                       { exiterror( ERR_STRING_EXPECTED, 3, __reginatext ) ;}
#line 4524 "yaccsrc.c"
    break;

  case 347: /* asymbol: CONSYMBOL  */
#line 1104 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 4530 "yaccsrc.c"
    break;

  case 348: /* asymbol: SIMSYMBOL  */
#line 1105 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 4536 "yaccsrc.c"
    break;

  case 349: /* on: ON  */
#line 1108 "yaccsrc.y"
                                       { yyval = makenode(X_ON,0) ; }
#line 4542 "yaccsrc.c"
    break;

  case 350: /* off: OFF  */
#line 1111 "yaccsrc.y"
                                       { yyval = makenode(X_OFF,0) ; }
#line 4548 "yaccsrc.c"
    break;

  case 351: /* c_action: ERROR  */
#line 1114 "yaccsrc.y"
                                       { yyval = makenode(X_S_ERROR,0) ; }
#line 4554 "yaccsrc.c"
    break;

  case 352: /* c_action: HALT  */
#line 1115 "yaccsrc.y"
                                       { yyval = makenode(X_S_HALT,0) ; }
#line 4560 "yaccsrc.c"
    break;

  case 353: /* c_action: NOTREADY  */
#line 1116 "yaccsrc.y"
                                       { yyval = makenode(X_S_NOTREADY,0) ; }
#line 4566 "yaccsrc.c"
    break;

  case 354: /* c_action: FAILURE  */
#line 1117 "yaccsrc.y"
                                       { yyval = makenode(X_S_FAILURE,0) ; }
#line 4572 "yaccsrc.c"
    break;

  case 355: /* s_action: c_action  */
#line 1120 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 4578 "yaccsrc.c"
    break;

  case 356: /* s_action: NOVALUE  */
#line 1121 "yaccsrc.y"
                                       { yyval = makenode(X_S_NOVALUE,0) ; }
#line 4584 "yaccsrc.c"
    break;

  case 357: /* s_action: SYNTAX  */
#line 1122 "yaccsrc.y"
                                       { yyval = makenode(X_S_SYNTAX,0) ; }
#line 4590 "yaccsrc.c"
    break;

  case 358: /* s_action: LOSTDIGITS  */
#line 1123 "yaccsrc.y"
                                       { yyval = makenode(X_S_LOSTDIGITS,0) ; }
#line 4596 "yaccsrc.c"
    break;

  case 359: /* trace_stat: trace VALUE expr  */
#line 1126 "yaccsrc.y"
                                       { yyval = yyvsp[-2] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4603 "yaccsrc.c"
    break;

  case 360: /* trace_stat: trace nexpr  */
#line 1128 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[0] = yyvsp[0] ; }
#line 4610 "yaccsrc.c"
    break;

  case 361: /* trace_stat: trace whatever error  */
#line 1130 "yaccsrc.y"
                                   { exiterror( ERR_EXTRA_DATA, 1, __reginatext ) ;}
#line 4616 "yaccsrc.c"
    break;

  case 362: /* trace_stat: trace whatever  */
#line 1131 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 4623 "yaccsrc.c"
    break;

  case 363: /* whatever: WHATEVER  */
#line 1135 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 4629 "yaccsrc.c"
    break;

  case 364: /* assignment: ass_part nexpr  */
#line 1139 "yaccsrc.y"
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
#line 4648 "yaccsrc.c"
    break;

  case 365: /* ass_part: ASSIGNMENTVARIABLE  */
#line 1155 "yaccsrc.y"
                                       { yyval = makenode(X_ASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4657 "yaccsrc.c"
    break;

  case 366: /* plusassignment: plus_ass_part nexpr  */
#line 1161 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "+=" );
                                       }
#line 4667 "yaccsrc.c"
    break;

  case 367: /* plus_ass_part: PLUSASSIGNMENTVARIABLE  */
#line 1168 "yaccsrc.y"
                                       { yyval = makenode(X_PLUSASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4676 "yaccsrc.c"
    break;

  case 368: /* minusassignment: minus_ass_part nexpr  */
#line 1174 "yaccsrc.y"
                                         { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "-=" );
                                       }
#line 4686 "yaccsrc.c"
    break;

  case 369: /* minus_ass_part: MINUSASSIGNMENTVARIABLE  */
#line 1181 "yaccsrc.y"
                                         { yyval = makenode(X_MINUSASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4695 "yaccsrc.c"
    break;

  case 370: /* multassignment: mult_ass_part nexpr  */
#line 1187 "yaccsrc.y"
                                       { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "*=" );
                                       }
#line 4705 "yaccsrc.c"
    break;

  case 371: /* mult_ass_part: MULTASSIGNMENTVARIABLE  */
#line 1194 "yaccsrc.y"
                                       { yyval = makenode(X_MULTASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4714 "yaccsrc.c"
    break;

  case 372: /* divassignment: div_ass_part nexpr  */
#line 1200 "yaccsrc.y"
                                     { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "/=" );
                                       }
#line 4724 "yaccsrc.c"
    break;

  case 373: /* div_ass_part: DIVASSIGNMENTVARIABLE  */
#line 1207 "yaccsrc.y"
                                     { yyval = makenode(X_DIVASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4733 "yaccsrc.c"
    break;

  case 374: /* intdivassignment: intdiv_ass_part nexpr  */
#line 1213 "yaccsrc.y"
                                           { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "%=" );
                                       }
#line 4743 "yaccsrc.c"
    break;

  case 375: /* intdiv_ass_part: INTDIVASSIGNMENTVARIABLE  */
#line 1220 "yaccsrc.y"
                                           { yyval = makenode(X_INTDIVASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4752 "yaccsrc.c"
    break;

  case 376: /* modulusassignment: modulus_ass_part nexpr  */
#line 1226 "yaccsrc.y"
                                             { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "//=" );
                                       }
#line 4762 "yaccsrc.c"
    break;

  case 377: /* modulus_ass_part: MODULUSASSIGNMENTVARIABLE  */
#line 1233 "yaccsrc.y"
                                             { yyval = makenode(X_MODULUSASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4771 "yaccsrc.c"
    break;

  case 378: /* andassignment: and_ass_part nexpr  */
#line 1239 "yaccsrc.y"
                                     { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "&=" );
                                       }
#line 4781 "yaccsrc.c"
    break;

  case 379: /* and_ass_part: ANDASSIGNMENTVARIABLE  */
#line 1246 "yaccsrc.y"
                                     { yyval = makenode(X_ANDASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4790 "yaccsrc.c"
    break;

  case 380: /* xorassignment: xor_ass_part nexpr  */
#line 1252 "yaccsrc.y"
                                     { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "&&=" );
                                       }
#line 4800 "yaccsrc.c"
    break;

  case 381: /* xor_ass_part: XORASSIGNMENTVARIABLE  */
#line 1259 "yaccsrc.y"
                                     { yyval = makenode(X_XORASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4809 "yaccsrc.c"
    break;

  case 382: /* orassignment: or_ass_part nexpr  */
#line 1265 "yaccsrc.y"
                                   { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                         if (yyvsp[0]==NULL)
                                            exiterror( ERR_BAD_ARITHMETIC, 2, "", "|=" );
                                       }
#line 4819 "yaccsrc.c"
    break;

  case 383: /* or_ass_part: ORASSIGNMENTVARIABLE  */
#line 1272 "yaccsrc.y"
                                   { yyval = makenode(X_ORASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4828 "yaccsrc.c"
    break;

  case 384: /* concatassignment: concat_ass_part nexpr  */
#line 1278 "yaccsrc.y"
                                           { yyval = yyvsp[-1] ;
                                         yyval->p[1] = yyvsp[0] ;
                                       }
#line 4836 "yaccsrc.c"
    break;

  case 385: /* concat_ass_part: CONCATASSIGNMENTVARIABLE  */
#line 1283 "yaccsrc.y"
                                           { yyval = makenode(X_CONCATASSIGN,0) ;
                                         yyval->charnr = parser_data.tstart ;
                                         yyval->lineno = parser_data.tline ;
                                         yyval->p[0] = (nodeptr)create_head( (const char *)retvalue ); }
#line 4845 "yaccsrc.c"
    break;

  case 386: /* $@10: %empty  */
#line 1290 "yaccsrc.y"
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
#line 4866 "yaccsrc.c"
    break;

  case 387: /* expr: '(' $@10 exprs_sub  */
#line 1306 "yaccsrc.y"
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
#line 4888 "yaccsrc.c"
    break;

  case 388: /* $@11: %empty  */
#line 1323 "yaccsrc.y"
                                       { parendepth--; }
#line 4894 "yaccsrc.c"
    break;

  case 389: /* expr: expr '+' $@11 expr  */
#line 1324 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "+" );
                                         yyval = makenode( X_PLUSS, 2, yyvsp[-3], yyvsp[0] ); }
#line 4901 "yaccsrc.c"
    break;

  case 390: /* $@12: %empty  */
#line 1326 "yaccsrc.y"
                                       { parendepth--; }
#line 4907 "yaccsrc.c"
    break;

  case 391: /* expr: expr '-' $@12 expr  */
#line 1327 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "-" );
                                         yyval = makenode( X_MINUS, 2, yyvsp[-3], yyvsp[0] ); }
#line 4914 "yaccsrc.c"
    break;

  case 392: /* $@13: %empty  */
#line 1329 "yaccsrc.y"
                                       { parendepth--; }
#line 4920 "yaccsrc.c"
    break;

  case 393: /* expr: expr '*' $@13 expr  */
#line 1330 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "*" );
                                         yyval = makenode( X_MULT, 2, yyvsp[-3], yyvsp[0] ); }
#line 4927 "yaccsrc.c"
    break;

  case 394: /* expr: '*'  */
#line 1332 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "*" ); }
#line 4933 "yaccsrc.c"
    break;

  case 395: /* $@14: %empty  */
#line 1333 "yaccsrc.y"
                                       { parendepth--; }
#line 4939 "yaccsrc.c"
    break;

  case 396: /* expr: expr '/' $@14 expr  */
#line 1334 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "/" );
                                         yyval = makenode( X_DEVIDE, 2, yyvsp[-3], yyvsp[0] ); }
#line 4946 "yaccsrc.c"
    break;

  case 397: /* expr: '/'  */
#line 1336 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "/" ); }
#line 4952 "yaccsrc.c"
    break;

  case 398: /* $@15: %empty  */
#line 1337 "yaccsrc.y"
                                       { parendepth--; }
#line 4958 "yaccsrc.c"
    break;

  case 399: /* expr: expr MODULUS $@15 expr  */
#line 1338 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "//" );
                                         yyval = makenode( X_MODULUS, 2, yyvsp[-3], yyvsp[0] ); }
#line 4965 "yaccsrc.c"
    break;

  case 400: /* expr: MODULUS  */
#line 1340 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "//" ); }
#line 4971 "yaccsrc.c"
    break;

  case 401: /* $@16: %empty  */
#line 1341 "yaccsrc.y"
                                       { parendepth--; }
#line 4977 "yaccsrc.c"
    break;

  case 402: /* expr: expr '%' $@16 expr  */
#line 1342 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "%" );
                                         yyval = makenode( X_INTDIV, 2, yyvsp[-3], yyvsp[0] ); }
#line 4984 "yaccsrc.c"
    break;

  case 403: /* expr: '%'  */
#line 1344 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "%" ); }
#line 4990 "yaccsrc.c"
    break;

  case 404: /* $@17: %empty  */
#line 1345 "yaccsrc.y"
                                       { parendepth--; }
#line 4996 "yaccsrc.c"
    break;

  case 405: /* expr: expr '|' $@17 expr  */
#line 1346 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "|" );
                                         yyval = makenode( X_LOG_OR, 2, yyvsp[-3], yyvsp[0] ); }
#line 5003 "yaccsrc.c"
    break;

  case 406: /* expr: '|'  */
#line 1348 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "|" ); }
#line 5009 "yaccsrc.c"
    break;

  case 407: /* $@18: %empty  */
#line 1349 "yaccsrc.y"
                                       { parendepth--; }
#line 5015 "yaccsrc.c"
    break;

  case 408: /* expr: expr '&' $@18 expr  */
#line 1350 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "&" );
                                         yyval = makenode( X_LOG_AND, 2, yyvsp[-3], yyvsp[0] ); }
#line 5022 "yaccsrc.c"
    break;

  case 409: /* expr: '&'  */
#line 1352 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "&" ); }
#line 5028 "yaccsrc.c"
    break;

  case 410: /* $@19: %empty  */
#line 1353 "yaccsrc.y"
                                       { parendepth--; }
#line 5034 "yaccsrc.c"
    break;

  case 411: /* expr: expr XOR $@19 expr  */
#line 1354 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "&&" );
                                         yyval = makenode( X_LOG_XOR, 2, yyvsp[-3], yyvsp[0] ); }
#line 5041 "yaccsrc.c"
    break;

  case 412: /* expr: XOR  */
#line 1356 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "&&" ); }
#line 5047 "yaccsrc.c"
    break;

  case 413: /* $@20: %empty  */
#line 1357 "yaccsrc.y"
                                       { parendepth--; }
#line 5053 "yaccsrc.c"
    break;

  case 414: /* expr: expr EXP $@20 expr  */
#line 1358 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "**" );
                                         yyval = makenode( X_EXP, 2, yyvsp[-3], yyvsp[0] ); }
#line 5060 "yaccsrc.c"
    break;

  case 415: /* expr: EXP  */
#line 1360 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "**" ); }
#line 5066 "yaccsrc.c"
    break;

  case 416: /* $@21: %empty  */
#line 1361 "yaccsrc.y"
                                       { parendepth--; }
#line 5072 "yaccsrc.c"
    break;

  case 417: /* expr: expr SPACE $@21 expr  */
#line 1362 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], " " );
                                         yyval = makenode( X_SPACE, 2, yyvsp[-3], yyvsp[0] ); }
#line 5079 "yaccsrc.c"
    break;

  case 418: /* expr: SPACE  */
#line 1364 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, " " ); }
#line 5085 "yaccsrc.c"
    break;

  case 419: /* $@22: %empty  */
#line 1365 "yaccsrc.y"
                                       { parendepth--; }
#line 5091 "yaccsrc.c"
    break;

  case 420: /* expr: expr CONCATENATE $@22 expr  */
#line 1366 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "||" );
                                         yyval = makenode( X_CONCAT, 2, yyvsp[-3], yyvsp[0] ); }
#line 5098 "yaccsrc.c"
    break;

  case 421: /* expr: CONCATENATE  */
#line 1368 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "||" ); }
#line 5104 "yaccsrc.c"
    break;

  case 422: /* expr: NOT expr  */
#line 1369 "yaccsrc.y"
                                       { AUTO_REDUCE( yyvsp[0], "\\" );
                                         yyval = makenode( X_LOG_NOT, 1, yyvsp[0] ); }
#line 5111 "yaccsrc.c"
    break;

  case 423: /* expr: NOT  */
#line 1371 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "\\" ); }
#line 5117 "yaccsrc.c"
    break;

  case 424: /* $@23: %empty  */
#line 1372 "yaccsrc.y"
                                       { parendepth--; }
#line 5123 "yaccsrc.c"
    break;

  case 425: /* expr: expr '=' $@23 expr  */
#line 1373 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "=" );
                                         yyval = makenode( X_EQUAL, 2, yyvsp[-3], yyvsp[0] );
                                         transform( yyval ); }
#line 5131 "yaccsrc.c"
    break;

  case 426: /* expr: '='  */
#line 1376 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "=" ); }
#line 5137 "yaccsrc.c"
    break;

  case 427: /* $@24: %empty  */
#line 1377 "yaccsrc.y"
                                       { parendepth--; }
#line 5143 "yaccsrc.c"
    break;

  case 428: /* expr: expr GTE $@24 expr  */
#line 1378 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], ">=" );
                                         yyval = makenode( X_GTE, 2, yyvsp[-3], yyvsp[0] );
                                         transform( yyval ) ; }
#line 5151 "yaccsrc.c"
    break;

  case 429: /* expr: GTE  */
#line 1381 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, ">=" ); }
#line 5157 "yaccsrc.c"
    break;

  case 430: /* $@25: %empty  */
#line 1382 "yaccsrc.y"
                                       { parendepth--; }
#line 5163 "yaccsrc.c"
    break;

  case 431: /* expr: expr LTE $@25 expr  */
#line 1383 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "<=" );
                                         yyval = makenode( X_LTE, 2, yyvsp[-3], yyvsp[0] );
                                         transform( yyval ) ; }
#line 5171 "yaccsrc.c"
    break;

  case 432: /* expr: LTE  */
#line 1386 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "<=" ); }
#line 5177 "yaccsrc.c"
    break;

  case 433: /* $@26: %empty  */
#line 1387 "yaccsrc.y"
                                       { parendepth--; }
#line 5183 "yaccsrc.c"
    break;

  case 434: /* expr: expr GT $@26 expr  */
#line 1388 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], ">" );
                                         yyval = makenode( X_GT, 2, yyvsp[-3], yyvsp[0] );
                                         transform( yyval ) ; }
#line 5191 "yaccsrc.c"
    break;

  case 435: /* expr: GT  */
#line 1391 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, ">" ); }
#line 5197 "yaccsrc.c"
    break;

  case 436: /* $@27: %empty  */
#line 1392 "yaccsrc.y"
                                       { parendepth--; }
#line 5203 "yaccsrc.c"
    break;

  case 437: /* expr: expr LT $@27 expr  */
#line 1393 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "<" );
                                         yyval = makenode( X_LT, 2, yyvsp[-3], yyvsp[0] );
                                         transform( yyval ) ; }
#line 5211 "yaccsrc.c"
    break;

  case 438: /* expr: LT  */
#line 1396 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "<" ); }
#line 5217 "yaccsrc.c"
    break;

  case 439: /* $@28: %empty  */
#line 1397 "yaccsrc.y"
                                       { parendepth--; }
#line 5223 "yaccsrc.c"
    break;

  case 440: /* expr: expr DIFFERENT $@28 expr  */
#line 1398 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "\\=" );
                                         yyval = makenode( X_DIFF, 2, yyvsp[-3], yyvsp[0] );
                                         transform( yyval ) ; }
#line 5231 "yaccsrc.c"
    break;

  case 441: /* expr: DIFFERENT  */
#line 1401 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "\\=" ); }
#line 5237 "yaccsrc.c"
    break;

  case 442: /* $@29: %empty  */
#line 1402 "yaccsrc.y"
                                       { parendepth--; }
#line 5243 "yaccsrc.c"
    break;

  case 443: /* expr: expr EQUALEQUAL $@29 expr  */
#line 1403 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "==" );
                                         yyval = makenode( X_S_EQUAL, 2, yyvsp[-3], yyvsp[0] ); }
#line 5250 "yaccsrc.c"
    break;

  case 444: /* expr: EQUALEQUAL  */
#line 1405 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "==" ); }
#line 5256 "yaccsrc.c"
    break;

  case 445: /* $@30: %empty  */
#line 1406 "yaccsrc.y"
                                       { parendepth--; }
#line 5262 "yaccsrc.c"
    break;

  case 446: /* expr: expr NOTEQUALEQUAL $@30 expr  */
#line 1407 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "\\==" );
                                         yyval = makenode( X_S_DIFF, 2, yyvsp[-3], yyvsp[0] ); }
#line 5269 "yaccsrc.c"
    break;

  case 447: /* expr: NOTEQUALEQUAL  */
#line 1409 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "\\==" ); }
#line 5275 "yaccsrc.c"
    break;

  case 448: /* $@31: %empty  */
#line 1410 "yaccsrc.y"
                                       { parendepth--; }
#line 5281 "yaccsrc.c"
    break;

  case 449: /* expr: expr GTGT $@31 expr  */
#line 1411 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], ">>" );
                                         yyval = makenode( X_S_GT, 2, yyvsp[-3], yyvsp[0] ); }
#line 5288 "yaccsrc.c"
    break;

  case 450: /* expr: GTGT  */
#line 1413 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, ">>" ); }
#line 5294 "yaccsrc.c"
    break;

  case 451: /* $@32: %empty  */
#line 1414 "yaccsrc.y"
                                       { parendepth--; }
#line 5300 "yaccsrc.c"
    break;

  case 452: /* expr: expr LTLT $@32 expr  */
#line 1415 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "<<" );
                                         yyval = makenode( X_S_LT, 2, yyvsp[-3], yyvsp[0] ); }
#line 5307 "yaccsrc.c"
    break;

  case 453: /* expr: LTLT  */
#line 1417 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "<<" ); }
#line 5313 "yaccsrc.c"
    break;

  case 454: /* $@33: %empty  */
#line 1418 "yaccsrc.y"
                                       { parendepth--; }
#line 5319 "yaccsrc.c"
    break;

  case 455: /* expr: expr NOTGTGT $@33 expr  */
#line 1419 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "\\>>" );
                                         yyval = makenode( X_S_NGT, 2, yyvsp[-3], yyvsp[0] ); }
#line 5326 "yaccsrc.c"
    break;

  case 456: /* expr: NOTGTGT  */
#line 1421 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "\\>>" ); }
#line 5332 "yaccsrc.c"
    break;

  case 457: /* $@34: %empty  */
#line 1422 "yaccsrc.y"
                                       { parendepth--; }
#line 5338 "yaccsrc.c"
    break;

  case 458: /* expr: expr NOTLTLT $@34 expr  */
#line 1423 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "\\<<" );
                                         yyval = makenode( X_S_NLT, 2, yyvsp[-3], yyvsp[0] ); }
#line 5345 "yaccsrc.c"
    break;

  case 459: /* expr: NOTLTLT  */
#line 1425 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "\\<<" ); }
#line 5351 "yaccsrc.c"
    break;

  case 460: /* $@35: %empty  */
#line 1426 "yaccsrc.y"
                                       { parendepth--; }
#line 5357 "yaccsrc.c"
    break;

  case 461: /* expr: expr GTGTE $@35 expr  */
#line 1427 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], ">>=" );
                                         yyval = makenode( X_S_GTE, 2, yyvsp[-3], yyvsp[0] ); }
#line 5364 "yaccsrc.c"
    break;

  case 462: /* expr: GTGTE  */
#line 1429 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, ">>=" ); }
#line 5370 "yaccsrc.c"
    break;

  case 463: /* $@36: %empty  */
#line 1430 "yaccsrc.y"
                                       { parendepth--; }
#line 5376 "yaccsrc.c"
    break;

  case 464: /* expr: expr LTLTE $@36 expr  */
#line 1431 "yaccsrc.y"
                                       { parendepth++; AUTO_REDUCE( yyvsp[-3], "<<=" );
                                         yyval = makenode( X_S_LTE, 2, yyvsp[-3], yyvsp[0] ); }
#line 5383 "yaccsrc.c"
    break;

  case 465: /* expr: LTLTE  */
#line 1433 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, "<<=" ); }
#line 5389 "yaccsrc.c"
    break;

  case 466: /* expr: expr MSGSEND msgtarget  */
#line 1434 "yaccsrc.y"
                                       { AUTO_REDUCE( yyvsp[-2], __regina_oo_operator );
                                         yyval = yyvsp[0];
                                         yyval->type = X_MSGSEND;
                                         yyval->p[0] = yyvsp[-2]; }
#line 5398 "yaccsrc.c"
    break;

  case 467: /* expr: expr CASCADE msgtarget  */
#line 1438 "yaccsrc.y"
                                       { AUTO_REDUCE( yyvsp[-2], __regina_oo_operator );
                                         yyval = yyvsp[0];
                                         yyval->type = X_CASCADE;
                                         yyval->p[0] = yyvsp[-2]; }
#line 5407 "yaccsrc.c"
    break;

  case 468: /* expr: MSGSEND  */
#line 1442 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, __regina_oo_operator ); }
#line 5413 "yaccsrc.c"
    break;

  case 469: /* expr: CASCADE  */
#line 1443 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, __regina_oo_operator ); }
#line 5419 "yaccsrc.c"
    break;

  case 470: /* expr: expr '[' bracket_args  */
#line 1444 "yaccsrc.y"
                                       { AUTO_REDUCE( yyvsp[-2], "[" );
                                         yyval = makenode( X_INDEX, 2, yyvsp[-2], yyvsp[0] ); }
#line 5426 "yaccsrc.c"
    break;

  case 471: /* expr: symbtree  */
#line 1446 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 5432 "yaccsrc.c"
    break;

  case 472: /* expr: CONSYMBOL  */
#line 1447 "yaccsrc.y"
                                       { /* OO: .NAME may be an environment
                                          * symbol, see OO_IS_ENVSYMBOL */
                                         yyval = makenode( ( retvalue[0] == '.'
                                                          && rx_isalpha( retvalue[1] ) )
                                                        ? X_CON_SYMBOL : X_STRING, 0 );
                                         yyval->name = Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 5443 "yaccsrc.c"
    break;

  case 473: /* expr: HEXSTRING  */
#line 1453 "yaccsrc.y"
                                       { yyval = makenode( X_STRING, 0 );
                                         yyval->name = Str_make_TSD(parser_data.TSD,retlength) ;
                                         memcpy(yyval->name->value,retvalue,
                                                    yyval->name->len=retlength); }
#line 5452 "yaccsrc.c"
    break;

  case 474: /* expr: BINSTRING  */
#line 1457 "yaccsrc.y"
                                       { yyval = makenode( X_STRING, 0 );
                                         yyval->name = Str_make_TSD(parser_data.TSD,retlength) ;
                                         memcpy(yyval->name->value,retvalue,
                                                    yyval->name->len=retlength); }
#line 5461 "yaccsrc.c"
    break;

  case 475: /* expr: STRING  */
#line 1461 "yaccsrc.y"
                                       { yyval = makenode( X_STRING, 0 );
                                         yyval->name = Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 5468 "yaccsrc.c"
    break;

  case 476: /* expr: function  */
#line 1463 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 5474 "yaccsrc.c"
    break;

  case 477: /* expr: '+' expr  */
#line 1464 "yaccsrc.y"
                                       { AUTO_REDUCE( yyvsp[0], rxnullptr );
                                         yyval = makenode( X_U_PLUSS, 1, yyvsp[0] ); }
#line 5481 "yaccsrc.c"
    break;

  case 478: /* expr: '-' expr  */
#line 1466 "yaccsrc.y"
                                       { AUTO_REDUCE( yyvsp[0], rxnullptr );
                                         yyval = makenode( X_U_MINUS, 1, yyvsp[0] ); }
#line 5488 "yaccsrc.c"
    break;

  case 479: /* expr: '+' error  */
#line 1468 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, __reginatext ); }
#line 5494 "yaccsrc.c"
    break;

  case 480: /* expr: '-' error  */
#line 1469 "yaccsrc.y"
                                       { exiterror( ERR_INVALID_EXPRESSION, 1, __reginatext ); }
#line 5500 "yaccsrc.c"
    break;

  case 481: /* exprs_sub: exprs ')'  */
#line 1472 "yaccsrc.y"
                                       { yyval = yyvsp[-1]; }
#line 5506 "yaccsrc.c"
    break;

  case 482: /* exprs_sub: exprs error  */
#line 1473 "yaccsrc.y"
                                       { exiterror( ERR_UNMATCHED_PARAN, 0 ); }
#line 5512 "yaccsrc.c"
    break;

  case 483: /* exprs_sub: STATSEP  */
#line 1474 "yaccsrc.y"
                                       { exiterror( ERR_UNMATCHED_PARAN, 0 ); }
#line 5518 "yaccsrc.c"
    break;

  case 484: /* symbtree: SIMSYMBOL  */
#line 1477 "yaccsrc.y"
                                       { yyval = (nodeptr)create_head( (const char *)retvalue ) ; }
#line 5524 "yaccsrc.c"
    break;

  case 485: /* function: extfunc func_args  */
#line 1481 "yaccsrc.y"
                                       { yyval = makenode(X_EX_FUNC,1,yyvsp[0]) ;
                                         yyval->name = (streng *)yyvsp[-1] ; }
#line 5531 "yaccsrc.c"
    break;

  case 486: /* function: intfunc func_args  */
#line 1483 "yaccsrc.y"
                                       { yyval = makenode(X_IN_FUNC,1,yyvsp[0]) ;
                                         yyval->name = (streng *)yyvsp[-1] ; }
#line 5538 "yaccsrc.c"
    break;

  case 487: /* @37: %empty  */
#line 1487 "yaccsrc.y"
                                       { /* ugly fake preservs parendepth */
                                         yyval = (YYSTYPE) parendepth;
                                         parendepth = 0; }
#line 5546 "yaccsrc.c"
    break;

  case 488: /* func_args: @37 exprs_sub  */
#line 1490 "yaccsrc.y"
                                       { parendepth = (int) yyval;
                                         yyval = yyvsp[0]; }
#line 5553 "yaccsrc.c"
    break;

  case 489: /* msgtarget: msgname  */
#line 1499 "yaccsrc.y"
                                       { yyval = makenode( X_MSGSEND, 0 );
                                         yyval->name = (streng *) yyvsp[0]; }
#line 5560 "yaccsrc.c"
    break;

  case 490: /* msgtarget: msgname MSGSUPER  */
#line 1501 "yaccsrc.y"
                                       { /* ~name:super, see oo.c */
                                         yyval = makenode( X_MSGSEND, 0 );
                                         yyval->name = (streng *) yyvsp[-1];
                                         yyval->u.nonansi = 1; }
#line 5569 "yaccsrc.c"
    break;

  case 491: /* msgtarget: msgname MSGSUPER '(' func_args  */
#line 1506 "yaccsrc.y"
                                       { yyval = makenode( X_MSGSEND, 2, NULL, yyvsp[0] );
                                         yyval->name = (streng *) yyvsp[-3];
                                         yyval->u.nonansi = 1; }
#line 5577 "yaccsrc.c"
    break;

  case 492: /* msgtarget: STRING  */
#line 1509 "yaccsrc.y"
                                       { yyval = makenode( X_MSGSEND, 0 );
                                         yyval->name = Str_cre_TSD( parser_data.TSD, retvalue );
                                         mem_upper( yyval->name->value, yyval->name->len ); }
#line 5585 "yaccsrc.c"
    break;

  case 493: /* msgtarget: intfunc func_args  */
#line 1512 "yaccsrc.y"
                                       { yyval = makenode( X_MSGSEND, 2, NULL, yyvsp[0] );
                                         yyval->name = (streng *) yyvsp[-1];
                                         mem_upper( yyval->name->value, yyval->name->len ); }
#line 5593 "yaccsrc.c"
    break;

  case 494: /* msgtarget: extfunc func_args  */
#line 1515 "yaccsrc.y"
                                       { yyval = makenode( X_MSGSEND, 2, NULL, yyvsp[0] );
                                         yyval->name = (streng *) yyvsp[-1];
                                         mem_upper( yyval->name->value, yyval->name->len ); }
#line 5601 "yaccsrc.c"
    break;

  case 495: /* msgname: SIMSYMBOL  */
#line 1520 "yaccsrc.y"
                                       { yyval = (nodeptr) Str_cre_TSD( parser_data.TSD, retvalue ); }
#line 5607 "yaccsrc.c"
    break;

  case 496: /* @38: %empty  */
#line 1523 "yaccsrc.y"
                                       { /* like func_args, with "]" */
                                         yyval = (YYSTYPE) parendepth;
                                         parendepth = 0; }
#line 5615 "yaccsrc.c"
    break;

  case 497: /* bracket_args: @38 exprs ']'  */
#line 1526 "yaccsrc.y"
                                       { parendepth = (int) yyval;
                                         yyval = yyvsp[-1]; }
#line 5622 "yaccsrc.c"
    break;

  case 498: /* intfunc: INFUNCNAME  */
#line 1530 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 5628 "yaccsrc.c"
    break;

  case 499: /* extfunc: EXFUNCNAME  */
#line 1533 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 5634 "yaccsrc.c"
    break;

  case 500: /* template: pv solid template  */
#line 1536 "yaccsrc.y"
                                       { yyval =makenode(X_TPL_SOLID,3,yyvsp[-2],yyvsp[-1],yyvsp[0]);}
#line 5640 "yaccsrc.c"
    break;

  case 501: /* template: pv  */
#line 1537 "yaccsrc.y"
                                       { yyval =makenode(X_TPL_SOLID,1,yyvsp[0]) ; }
#line 5646 "yaccsrc.c"
    break;

  case 502: /* template: error  */
#line 1538 "yaccsrc.y"
                    { exiterror( ERR_INVALID_TEMPLATE, 1, __reginatext ) ;}
#line 5652 "yaccsrc.c"
    break;

  case 503: /* solid: '-' offset  */
#line 1541 "yaccsrc.y"
                                       { yyval = makenode(X_NEG_OFFS,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 5659 "yaccsrc.c"
    break;

  case 504: /* solid: '+' offset  */
#line 1543 "yaccsrc.y"
                                       { yyval = makenode(X_POS_OFFS,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 5666 "yaccsrc.c"
    break;

  case 505: /* solid: offset  */
#line 1545 "yaccsrc.y"
                                       { yyval = makenode(X_ABS_OFFS,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 5673 "yaccsrc.c"
    break;

  case 506: /* solid: '=' offset  */
#line 1547 "yaccsrc.y"
                                       { yyval = makenode(X_ABS_OFFS,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 5680 "yaccsrc.c"
    break;

  case 507: /* solid: '(' symbtree ')'  */
#line 1549 "yaccsrc.y"
                                       { yyval = makenode(X_TPL_VAR,0) ;
                                         yyval->p[0] = yyvsp[-1] ; }
#line 5687 "yaccsrc.c"
    break;

  case 508: /* solid: '-' '(' symbtree ')'  */
#line 1551 "yaccsrc.y"
                                       { yyval = makenode(X_NEG_OFFS,0) ;
                                         yyval->p[0] = yyvsp[-1] ; }
#line 5694 "yaccsrc.c"
    break;

  case 509: /* solid: '+' '(' symbtree ')'  */
#line 1553 "yaccsrc.y"
                                       { yyval = makenode(X_POS_OFFS,0) ;
                                         yyval->p[0] = yyvsp[-1] ; }
#line 5701 "yaccsrc.c"
    break;

  case 510: /* solid: '=' '(' symbtree ')'  */
#line 1555 "yaccsrc.y"
                                       { yyval = makenode(X_ABS_OFFS,0) ;
                                         yyval->p[0] = yyvsp[-1] ; }
#line 5708 "yaccsrc.c"
    break;

  case 511: /* solid: string  */
#line 1557 "yaccsrc.y"
                                       { yyval = makenode(X_TPL_MVE,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 5715 "yaccsrc.c"
    break;

  case 512: /* offset: OFFSET  */
#line 1561 "yaccsrc.y"
                                       { yyval = (nodeptr)Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 5721 "yaccsrc.c"
    break;

  case 513: /* offset: CONSYMBOL  */
#line 1562 "yaccsrc.y"
                                       { streng *sptr = Str_cre_TSD(parser_data.TSD,retvalue) ;
                                          if (myisnumber(parser_data.TSD, sptr))
                                          {
                                             exiterror( ERR_INVALID_INTEGER, 4, sptr->value ) ;
                                          }
                                          else
                                             exiterror( ERR_INVALID_TEMPLATE, 0 ) ;}
#line 5733 "yaccsrc.c"
    break;

  case 514: /* string: STRING  */
#line 1571 "yaccsrc.y"
                                       { yyval = (nodeptr) Str_cre_TSD(parser_data.TSD,retvalue) ; }
#line 5739 "yaccsrc.c"
    break;

  case 515: /* string: HEXSTRING  */
#line 1572 "yaccsrc.y"
                                       { streng *sptr = Str_make_TSD(parser_data.TSD,retlength) ;
                                         memcpy(sptr->value,retvalue,
                                                 sptr->len=retlength) ;
                                         yyval = (nodeptr) sptr ; }
#line 5748 "yaccsrc.c"
    break;

  case 516: /* string: BINSTRING  */
#line 1576 "yaccsrc.y"
                                       { streng *sptr = Str_make_TSD(parser_data.TSD,retlength) ;
                                         memcpy(sptr->value,retvalue,
                                                 sptr->len=retlength) ;
                                         yyval = (nodeptr) sptr ; }
#line 5757 "yaccsrc.c"
    break;

  case 517: /* pv: PLACEHOLDER pv  */
#line 1582 "yaccsrc.y"
                                       { yyval = makenode(X_TPL_POINT,1,yyvsp[0]) ; }
#line 5763 "yaccsrc.c"
    break;

  case 518: /* pv: symbtree pv  */
#line 1583 "yaccsrc.y"
                                       { yyval = makenode(X_TPL_SYMBOL,2,yyvsp[0],yyvsp[-1]) ; }
#line 5769 "yaccsrc.c"
    break;

  case 519: /* pv: %empty  */
#line 1584 "yaccsrc.y"
                                       { yyval = NULL ; }
#line 5775 "yaccsrc.c"
    break;

  case 520: /* $@39: %empty  */
#line 1594 "yaccsrc.y"
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
#line 5823 "yaccsrc.c"
    break;

  case 521: /* exprs: nexpr ',' $@39 exprs  */
#line 1637 "yaccsrc.y"
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
#line 5865 "yaccsrc.c"
    break;

  case 522: /* exprs: nexpr  */
#line 1674 "yaccsrc.y"
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
#line 5912 "yaccsrc.c"
    break;

  case 523: /* nexpr: expr  */
#line 1718 "yaccsrc.y"
                                       { yyval = yyvsp[0] ; }
#line 5918 "yaccsrc.c"
    break;

  case 524: /* nexpr: %empty  */
#line 1719 "yaccsrc.y"
                                       { yyval = NULL ; }
#line 5924 "yaccsrc.c"
    break;

  case 525: /* anyvars: xsimsymb anyvars  */
#line 1722 "yaccsrc.y"
                                        { yyval = makenode(X_SIM_SYMBOL,1,yyvsp[0]) ;
                                         yyval->name = (streng *) yyvsp[-1] ; }
#line 5931 "yaccsrc.c"
    break;

  case 526: /* anyvars: xsimsymb  */
#line 1724 "yaccsrc.y"
                                        { yyval = makenode(X_SIM_SYMBOL,0) ;
                                         yyval->name = (streng *) yyvsp[0] ; }
#line 5938 "yaccsrc.c"
    break;

  case 527: /* anyvars: '(' xsimsymb ')' anyvars  */
#line 1726 "yaccsrc.y"
                                        { yyval = makenode(X_IND_SYMBOL,1,yyvsp[0]) ;
                                         yyval->name = (streng *) yyvsp[-2] ; }
#line 5945 "yaccsrc.c"
    break;

  case 528: /* anyvars: '(' xsimsymb ')'  */
#line 1728 "yaccsrc.y"
                                        { yyval = makenode(X_IND_SYMBOL,0) ;
                                         yyval->name = (streng *) yyvsp[-1] ; }
#line 5952 "yaccsrc.c"
    break;

  case 529: /* xsimsymb: SIMSYMBOL  */
#line 1732 "yaccsrc.y"
                                       { yyval = (treenode *) Str_cre_TSD(parser_data.TSD,retvalue);}
#line 5958 "yaccsrc.c"
    break;

  case 530: /* simsymb: SIMSYMBOL  */
#line 1735 "yaccsrc.y"
                                       { yyval = (treenode *) Str_cre_TSD(parser_data.TSD,retvalue);}
#line 5964 "yaccsrc.c"
    break;

  case 531: /* simsymb: error  */
#line 1736 "yaccsrc.y"
                                       { exiterror( ERR_SYMBOL_EXPECTED, 1, __reginatext ) ;}
#line 5970 "yaccsrc.c"
    break;


#line 5974 "yaccsrc.c"

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

#line 1739 "yaccsrc.y"


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
   else if ( ( overvar = do_over_var( rep ) ) != NULL )
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

