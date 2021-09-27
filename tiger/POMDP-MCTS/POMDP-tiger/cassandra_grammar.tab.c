
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "cassandra_grammar.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cassandra_grammar.h"
#include<iostream>
  using namespace std;
	
extern int yylex();
void yyerror(const char *s);
int nrdigit(int a, int base);

 Cassandra_return *toret=(Cassandra_return *) malloc(sizeof(Cassandra_return));
 //Cassandra_return *ret=(Cassandra_return *) malloc(sizeof(Cassandra_return));
float *theMatrix = NULL;
int theCursor = -1;



/* Line 189 of yacc.c  */
#line 93 "cassandra_grammar.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     DISCOUNT = 258,
     VALUES = 259,
     REWARD = 260,
     COST = 261,
     STATES = 262,
     ACTIONS = 263,
     NEWACTIONS = 264,
     NEWOBSERS = 265,
     OBSERVATIONS = 266,
     START = 267,
     T = 268,
     O = 269,
     R = 270,
     IDENTITY = 271,
     UNIFORM = 272,
     ASTERISK = 273,
     INTEGER = 274,
     STRING = 275,
     FLOAT = 276
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 20 "cassandra_grammar.y"

	char *	t_pchar;
	int *	t_pint;	
	float *	t_pfloat;
	int	t_int;
	float	t_float;



/* Line 214 of yacc.c  */
#line 160 "cassandra_grammar.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 172 "cassandra_grammar.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

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
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
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
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   107

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  25
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  50
/* YYNRULES -- Number of rules.  */
#define YYNRULES  97
/* YYNRULES -- Number of states.  */
#define YYNSTATES  138

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   276

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    23,     2,    24,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    22,     2,
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
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     6,     7,    10,    12,    14,    16,    18,
      20,    22,    24,    26,    30,    31,    36,    38,    42,    44,
      46,    50,    52,    54,    57,    59,    63,    65,    67,    70,
      72,    76,    78,    80,    83,    85,    89,    91,    93,    96,
      98,   102,   104,   106,   109,   111,   114,   116,   118,   120,
     122,   123,   128,   135,   140,   143,   144,   149,   156,   161,
     164,   168,   177,   184,   189,   191,   193,   195,   197,   199,
     201,   203,   205,   207,   209,   211,   214,   216,   218,   220,
     222,   225,   227,   229,   231,   234,   236,   238,   240,   243,
     245,   248,   250,   252,   255,   258,   260,   262
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      26,     0,    -1,    27,    50,    -1,    -1,    27,    28,    -1,
      29,    -1,    33,    -1,    35,    -1,    38,    -1,    44,    -1,
      41,    -1,    47,    -1,    30,    -1,     3,    22,    73,    -1,
      -1,    12,    22,    31,    32,    -1,    65,    -1,     4,    22,
      34,    -1,     5,    -1,     6,    -1,     7,    22,    36,    -1,
      19,    -1,    37,    -1,    37,    20,    -1,    20,    -1,     8,
      22,    39,    -1,    19,    -1,    40,    -1,    40,    20,    -1,
      20,    -1,     9,    22,    42,    -1,    19,    -1,    43,    -1,
      43,    20,    -1,    20,    -1,    11,    22,    45,    -1,    19,
      -1,    46,    -1,    46,    20,    -1,    20,    -1,    10,    22,
      48,    -1,    19,    -1,    49,    -1,    49,    20,    -1,    20,
      -1,    50,    51,    -1,    51,    -1,    52,    -1,    55,    -1,
      58,    -1,    -1,    13,    22,    53,    54,    -1,    61,    22,
      60,    22,    60,    72,    -1,    61,    22,    60,    63,    -1,
      61,    65,    -1,    -1,    14,    22,    56,    57,    -1,    61,
      22,    60,    22,    62,    72,    -1,    61,    22,    60,    67,
      -1,    61,    69,    -1,    15,    22,    59,    -1,    61,    22,
      60,    22,    60,    22,    62,    73,    -1,    61,    22,    60,
      22,    60,    71,    -1,    61,    22,    60,    71,    -1,    19,
      -1,    20,    -1,    18,    -1,    19,    -1,    20,    -1,    18,
      -1,    19,    -1,    20,    -1,    18,    -1,    17,    -1,    64,
      -1,    64,    72,    -1,    72,    -1,    17,    -1,    16,    -1,
      66,    -1,    66,    72,    -1,    72,    -1,    17,    -1,    68,
      -1,    68,    72,    -1,    72,    -1,    17,    -1,    70,    -1,
      70,    72,    -1,    72,    -1,    71,    73,    -1,    73,    -1,
      21,    -1,    74,    21,    -1,    74,    19,    -1,    23,    -1,
      24,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    43,    43,    44,    58,    59,    60,    61,    62,    63,
      65,    66,    69,    72,    80,    80,    90,    96,    97,    98,
     101,   102,   112,   114,   125,   138,   139,   149,   151,   162,
     178,   179,   189,   191,   202,   223,   224,   234,   236,   247,
     263,   264,   274,   276,   287,   308,   308,   310,   314,   315,
     321,   321,   333,   347,   358,   371,   371,   382,   396,   407,
     420,   421,   422,   423,   427,   430,   441,   448,   451,   462,
     474,   477,   488,   497,   506,   507,   511,   522,   531,   541,
     542,   546,   557,   566,   567,   571,   582,   591,   592,   596,
     608,   608,   611,   612,   612,   613,   613,   613
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "DISCOUNT", "VALUES", "REWARD", "COST",
  "STATES", "ACTIONS", "NEWACTIONS", "NEWOBSERS", "OBSERVATIONS", "START",
  "T", "O", "R", "IDENTITY", "UNIFORM", "ASTERISK", "INTEGER", "STRING",
  "FLOAT", "':'", "'+'", "'-'", "$accept", "pomdp_file", "preamble",
  "paramtype", "discount_param", "start_param", "$@1", "start_val",
  "value_param", "value_val", "state_param", "state_val", "state_list",
  "act_param", "act_val", "act_list", "newact_param", "newact_val",
  "newact_list", "obs_param", "obs_val", "obs_list", "newobs_param",
  "newobs_val", "newobs_list", "param_list", "param_val", "transit_param",
  "$@2", "transit_val", "observ_param", "$@3", "observ_val",
  "rewards_param", "rewards_val", "state", "action", "obs", "matrix_1xs",
  "recur_1xs", "matrix_sxs", "recur_sxs", "matrix_1xo", "recur_1xo",
  "matrix_sxo", "recur_sxo", "nr_matrix", "prob", "number", "sign", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,    58,    43,    45
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    25,    26,    27,    27,    28,    28,    28,    28,    28,
      28,    28,    28,    29,    31,    30,    32,    33,    34,    34,
      35,    36,    36,    37,    37,    38,    39,    39,    40,    40,
      41,    42,    42,    43,    43,    44,    45,    45,    46,    46,
      47,    48,    48,    49,    49,    50,    50,    51,    51,    51,
      53,    52,    54,    54,    54,    56,    55,    57,    57,    57,
      58,    59,    59,    59,    60,    60,    60,    61,    61,    61,
      62,    62,    62,    63,    63,    64,    64,    65,    65,    65,
      66,    66,    67,    67,    68,    68,    69,    69,    70,    70,
      71,    71,    72,    73,    73,    74,    74,    74
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     0,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     0,     4,     1,     3,     1,     1,
       3,     1,     1,     2,     1,     3,     1,     1,     2,     1,
       3,     1,     1,     2,     1,     3,     1,     1,     2,     1,
       3,     1,     1,     2,     1,     2,     1,     1,     1,     1,
       0,     4,     6,     4,     2,     0,     4,     6,     4,     2,
       3,     8,     6,     4,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     1,     1,     1,     1,
       2,     1,     1,     1,     2,     1,     1,     1,     2,     1,
       2,     1,     1,     2,     2,     1,     1,     0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     0,     1,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     4,     5,    12,     6,     7,
       8,    10,     9,    11,     2,    46,    47,    48,    49,    97,
       0,     0,     0,     0,     0,     0,    14,    50,    55,     0,
      45,    95,    96,    13,     0,    18,    19,    17,    21,    24,
      20,    22,    26,    29,    25,    27,    31,    34,    30,    32,
      41,    44,    40,    42,    36,    39,    35,    37,     0,     0,
       0,    69,    67,    68,    60,     0,    94,    93,    23,    28,
      33,    43,    38,    78,    77,    92,    15,    16,    79,    81,
      51,     0,    56,     0,     0,    80,     0,    54,    86,     0,
      59,    87,    89,    66,    64,    65,    97,     0,     0,    88,
       0,    63,    91,    73,     0,    53,    74,    76,    82,     0,
      58,    83,    85,    97,    90,     0,    75,    72,    70,    71,
       0,    84,     0,    62,    52,    57,    97,    61
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    15,    16,    17,    68,    86,    18,    47,
      19,    50,    51,    20,    54,    55,    21,    58,    59,    22,
      66,    67,    23,    62,    63,    24,    25,    26,    69,    90,
      27,    70,    92,    28,    74,   106,    75,   130,   115,   116,
      87,    88,   120,   121,   100,   101,   111,    89,   112,    44
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -93
static const yytype_int8 yypact[] =
{
     -93,     2,    37,   -93,   -10,    -5,    -3,    11,    13,    17,
      20,    59,    71,    73,    74,   -93,   -93,   -93,   -93,   -93,
     -93,   -93,   -93,   -93,    41,   -93,   -93,   -93,   -93,    52,
      72,    60,    64,    66,    68,    70,   -93,   -93,   -93,    42,
     -93,   -93,   -93,   -93,   -16,   -93,   -93,   -93,   -93,   -93,
     -93,    39,   -93,   -93,   -93,    77,   -93,   -93,   -93,    78,
     -93,   -93,   -93,    79,   -93,   -93,   -93,    80,    -8,    42,
      42,   -93,   -93,   -93,   -93,    81,   -93,   -93,   -93,   -93,
     -93,   -93,   -93,   -93,   -93,   -93,   -93,   -93,     3,   -93,
     -93,    -6,   -93,     9,    45,   -93,    45,   -93,   -93,    45,
     -93,     3,   -93,   -93,   -93,   -93,    44,    15,    36,   -93,
      45,     4,   -93,   -93,    45,   -93,     3,   -93,   -93,    51,
     -93,     3,   -93,    50,   -93,     3,   -93,   -93,   -93,   -93,
       3,   -93,    51,     4,   -93,   -93,    52,   -93
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -93,   -93,   -93,   -93,   -93,   -93,   -93,   -93,   -93,   -93,
     -93,   -93,   -93,   -93,   -93,   -93,   -93,   -93,   -93,   -93,
     -93,   -93,   -93,   -93,   -93,   -93,    82,   -93,   -93,   -93,
     -93,   -93,   -93,   -93,   -93,   -92,    22,   -38,   -93,   -93,
      10,   -93,   -93,   -93,   -93,   -93,   -21,   -87,   -29,   -93
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -98
static const yytype_int16 yytable[] =
{
      43,    95,     3,    76,   107,    77,   102,   108,    83,    84,
      83,    84,    29,    85,   109,    85,    96,    30,   123,    31,
     117,   122,   125,   -97,    85,   -97,    98,    41,    42,   126,
      85,    99,   113,    32,   131,    33,    85,   114,   134,    34,
       4,     5,    35,   135,     6,     7,     8,     9,    10,    11,
      12,    13,    14,   118,    12,    13,    14,    85,   119,    78,
      71,    72,    73,   103,   104,   105,   110,    41,    42,   127,
     128,   129,   132,    41,    42,    41,    42,    45,    46,    48,
      49,    36,   124,    52,    53,    56,    57,    60,    61,    64,
      65,    91,    93,    37,   136,    38,    39,    79,    80,    81,
      82,    97,   133,    94,   124,     0,    40,   137
};

static const yytype_int16 yycheck[] =
{
      29,    88,     0,    19,    96,    21,    93,    99,    16,    17,
      16,    17,    22,    21,   101,    21,    22,    22,   110,    22,
     107,   108,   114,    19,    21,    21,    17,    23,    24,   116,
      21,    22,    17,    22,   121,    22,    21,    22,   125,    22,
       3,     4,    22,   130,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    17,    13,    14,    15,    21,    22,    20,
      18,    19,    20,    18,    19,    20,    22,    23,    24,    18,
      19,    20,    22,    23,    24,    23,    24,     5,     6,    19,
      20,    22,   111,    19,    20,    19,    20,    19,    20,    19,
      20,    69,    70,    22,   132,    22,    22,    20,    20,    20,
      20,    91,   123,    22,   133,    -1,    24,   136
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    26,    27,     0,     3,     4,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    28,    29,    30,    33,    35,
      38,    41,    44,    47,    50,    51,    52,    55,    58,    22,
      22,    22,    22,    22,    22,    22,    22,    22,    22,    22,
      51,    23,    24,    73,    74,     5,     6,    34,    19,    20,
      36,    37,    19,    20,    39,    40,    19,    20,    42,    43,
      19,    20,    48,    49,    19,    20,    45,    46,    31,    53,
      56,    18,    19,    20,    59,    61,    19,    21,    20,    20,
      20,    20,    20,    16,    17,    21,    32,    65,    66,    72,
      54,    61,    57,    61,    22,    72,    22,    65,    17,    22,
      69,    70,    72,    18,    19,    20,    60,    60,    60,    72,
      22,    71,    73,    17,    22,    63,    64,    72,    17,    22,
      67,    68,    72,    60,    73,    60,    72,    18,    19,    20,
      62,    72,    22,    71,    72,    72,    62,    73
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
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



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

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
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
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
      if (yyn == 0 || yyn == YYTABLE_NINF)
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

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

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
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 3:

/* Line 1455 of yacc.c  */
#line 44 "cassandra_grammar.y"
    {
                        toret->nrstates	= 0;
			toret->nract	= 0;
			toret->nrobs	= 0;

			toret->nrnewact	= 0;
			toret->nrnewobs	= 0;

                        toret->Start    = NULL;
			toret->Tr 	= NULL;
			toret->Ob	= NULL;
			toret->Rw	= NULL;
                  
		;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 72 "cassandra_grammar.y"
    {toret->discount=(yyvsp[(3) - (3)].t_float);/* set the discount factor */ ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 80 "cassandra_grammar.y"
    {
			if (toret->Start == NULL) {
                            int i;
				
				for (i=0; i<toret->nrstates; i++) { 
					toret->Start = (float *) malloc(toret->nrstates * sizeof(float ));
				}
			} 
		;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 90 "cassandra_grammar.y"
    {
			
			memcpy(toret->Start, theMatrix , toret->nrstates * sizeof(float *));
			free(theMatrix); theMatrix=NULL; theCursor=-1;
		;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 97 "cassandra_grammar.y"
    { toret->values=0;/* set wether values are rewards or costs */ ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 98 "cassandra_grammar.y"
    { toret->values=1;/* set wether values are rewards or costs */ ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 102 "cassandra_grammar.y"
    {
			/* initialize n states with default mnemonics */
			toret->nrstates = (yyvsp[(1) - (1)].t_int);
			toret->arr_states = (char **) malloc((yyvsp[(1) - (1)].t_int) * sizeof(char *));
			int i;
			for (i=0; i<(yyvsp[(1) - (1)].t_int); i++) { 
				toret->arr_states[i] = (char *) malloc((nrdigit((yyvsp[(1) - (1)].t_int),16)+2) * sizeof(char));
				sprintf(toret->arr_states[i], "s%X", i);
			}
		;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 114 "cassandra_grammar.y"
    {
			/* add new symbol with given mnemonic */
			toret->nrstates++;
			char **tmpptr = (char **) malloc(toret->nrstates * sizeof(char *));
			memcpy(tmpptr, toret->arr_states, (toret->nrstates-1)*sizeof(char *));
			free(toret->arr_states);
			toret->arr_states = tmpptr;
			toret->arr_states[toret->nrstates-1] = (char *) malloc((strlen((yyvsp[(2) - (2)].t_pchar))+1)*sizeof(char));
			strcpy(toret->arr_states[toret->nrstates-1], (yyvsp[(2) - (2)].t_pchar));
			free((yyvsp[(2) - (2)].t_pchar));
		;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 125 "cassandra_grammar.y"
    {
			/* add new symbol with given mnemonic */
			toret->nrstates++;
			char **tmpptr = (char **) malloc(toret->nrstates * sizeof(char *));
			memcpy(tmpptr, toret->arr_states, (toret->nrstates-1)*sizeof(char *));
			free(toret->arr_states);
			toret->arr_states = tmpptr;
			toret->arr_states[toret->nrstates-1] = (char *) malloc((strlen((yyvsp[(1) - (1)].t_pchar))+1)*sizeof(char));
			strcpy(toret->arr_states[toret->nrstates-1], (yyvsp[(1) - (1)].t_pchar));
			free((yyvsp[(1) - (1)].t_pchar));
		;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 139 "cassandra_grammar.y"
    {
			/* initialize n actions with default mnemonics */
			toret->nract = (yyvsp[(1) - (1)].t_int);
			toret->arr_act = (char **) malloc((yyvsp[(1) - (1)].t_int) * sizeof(char *));
			int i;
			for (i=0; i<(yyvsp[(1) - (1)].t_int); i++) {
				toret->arr_act[i] = (char *) malloc((nrdigit((yyvsp[(1) - (1)].t_int),16)+2) * sizeof(char));
				sprintf(toret->arr_act[i], "a%X", i);
			}
		;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 151 "cassandra_grammar.y"
    {
			/* add new action with given mnemonic */
			toret->nract++;
			char **tmpptr = (char **) malloc(toret->nract * sizeof(char *));
			memcpy(tmpptr, toret->arr_act, (toret->nract-1)*sizeof(char *));
			free(toret->arr_act);
			toret->arr_act = tmpptr;
			toret->arr_act[toret->nract-1] = (char *) malloc((strlen((yyvsp[(2) - (2)].t_pchar))+1)*sizeof(char));
			strcpy(toret->arr_act[toret->nract-1], (yyvsp[(2) - (2)].t_pchar));
			free((yyvsp[(2) - (2)].t_pchar));
		;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 162 "cassandra_grammar.y"
    {
			/* add new action with given mnemonic */
			toret->nract++;
			char **tmpptr = (char **) malloc(toret->nract * sizeof(char *));
			memcpy(tmpptr, toret->arr_act, (toret->nract-1)*sizeof(char *));
			free(toret->arr_act);
			toret->arr_act = tmpptr;
			toret->arr_act[toret->nract-1] = (char *) malloc((strlen((yyvsp[(1) - (1)].t_pchar))+1)*sizeof(char));
			strcpy(toret->arr_act[toret->nract-1], (yyvsp[(1) - (1)].t_pchar));
			free((yyvsp[(1) - (1)].t_pchar));
		;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 179 "cassandra_grammar.y"
    {
			/* initialize n actions with default mnemonics */
			toret->nrnewact = (yyvsp[(1) - (1)].t_int);
			toret->arr_newact = (char **) malloc((yyvsp[(1) - (1)].t_int) * sizeof(char *));
			int i;
			for (i=0; i<(yyvsp[(1) - (1)].t_int); i++) {
				toret->arr_newact[i] = (char *) malloc((nrdigit((yyvsp[(1) - (1)].t_int),16)+2) * sizeof(char));
				sprintf(toret->arr_newact[i], "n%X", i);
			}
		;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 191 "cassandra_grammar.y"
    {
			/* add new action with given mnemonic */
			toret->nrnewact++;
			char **tmpptr = (char **) malloc(toret->nrnewact * sizeof(char *));
			memcpy(tmpptr, toret->arr_newact, (toret->nrnewact-1)*sizeof(char *));
			free(toret->arr_newact);
			toret->arr_newact = tmpptr;
			toret->arr_newact[toret->nrnewact-1] = (char *) malloc((strlen((yyvsp[(2) - (2)].t_pchar))+1)*sizeof(char));
			strcpy(toret->arr_newact[toret->nrnewact-1], (yyvsp[(2) - (2)].t_pchar));
			free((yyvsp[(2) - (2)].t_pchar));
		;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 202 "cassandra_grammar.y"
    {
			/* add new action with given mnemonic */
			toret->nrnewact++;
			char **tmpptr = (char **) malloc(toret->nrnewact * sizeof(char *));
			memcpy(tmpptr, toret->arr_newact, (toret->nrnewact-1)*sizeof(char *));
			free(toret->arr_newact);
			toret->arr_newact = tmpptr;
			toret->arr_newact[toret->nrnewact-1] = (char *) malloc((strlen((yyvsp[(1) - (1)].t_pchar))+1)*sizeof(char));
			strcpy(toret->arr_newact[toret->nrnewact-1], (yyvsp[(1) - (1)].t_pchar));
			free((yyvsp[(1) - (1)].t_pchar));
		;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 224 "cassandra_grammar.y"
    {
			/* initialize n observations with default mnemonics */
			toret->nrobs = (yyvsp[(1) - (1)].t_int);
			toret->arr_obs = (char **) malloc((yyvsp[(1) - (1)].t_int) * sizeof(char *));
			int i;
			for (i=0; i<(yyvsp[(1) - (1)].t_int); i++) {   
				toret->arr_obs[i] = (char *) malloc((nrdigit((yyvsp[(1) - (1)].t_int),16)+2) * sizeof(char));
				sprintf(toret->arr_obs[i], "o%X", i);
			}
		;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 236 "cassandra_grammar.y"
    {
			/* add new observation with given mnemonic */
			toret->nrobs++;
			char **tmpptr = (char **) malloc(toret->nrobs * sizeof(char *));
			memcpy(tmpptr, toret->arr_obs, (toret->nrobs-1)*sizeof(char *));
			free(toret->arr_obs);
			toret->arr_obs = tmpptr;
			toret->arr_obs[toret->nrobs-1] = (char *) malloc((strlen((yyvsp[(2) - (2)].t_pchar))+1)*sizeof(char));
			strcpy(toret->arr_obs[toret->nrobs-1], (yyvsp[(2) - (2)].t_pchar));
			free((yyvsp[(2) - (2)].t_pchar));
		;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 247 "cassandra_grammar.y"
    {
			/* add new observation with given mnemonic */
			toret->nrobs++;
			char **tmpptr = (char **) malloc(toret->nrobs * sizeof(char *));
			memcpy(tmpptr, toret->arr_obs, (toret->nrobs-1)*sizeof(char *));
			free(toret->arr_obs);
			toret->arr_obs = tmpptr;
			toret->arr_obs[toret->nrobs-1] = (char *) malloc((strlen((yyvsp[(1) - (1)].t_pchar))+1)*sizeof(char));
			strcpy(toret->arr_obs[toret->nrobs-1], (yyvsp[(1) - (1)].t_pchar));
			free((yyvsp[(1) - (1)].t_pchar));
		;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 264 "cassandra_grammar.y"
    {
			/* initialize n observations with default mnemonics */
			toret->nrnewobs = (yyvsp[(1) - (1)].t_int);
			toret->arr_newobs = (char **) malloc((yyvsp[(1) - (1)].t_int) * sizeof(char *));
			int i;
			for (i=0; i<(yyvsp[(1) - (1)].t_int); i++) {   
				toret->arr_newobs[i] = (char *) malloc((nrdigit((yyvsp[(1) - (1)].t_int),16)+2) * sizeof(char));
				sprintf(toret->arr_newobs[i], "b%X", i);
			}
		;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 276 "cassandra_grammar.y"
    {
			/* add new observation with given mnemonic */
			toret->nrnewobs++;
			char **tmpptr = (char **) malloc(toret->nrnewobs * sizeof(char *));
			memcpy(tmpptr, toret->arr_newobs, (toret->nrnewobs-1)*sizeof(char *));
			free(toret->arr_newobs);
			toret->arr_newobs = tmpptr;
			toret->arr_newobs[toret->nrnewobs-1] = (char *) malloc((strlen((yyvsp[(2) - (2)].t_pchar))+1)*sizeof(char));
			strcpy(toret->arr_newobs[toret->nrnewobs-1], (yyvsp[(2) - (2)].t_pchar));
			free((yyvsp[(2) - (2)].t_pchar));
		;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 287 "cassandra_grammar.y"
    {
			/* add new observation with given mnemonic */
			toret->nrnewobs++;
			char **tmpptr = (char **) malloc(toret->nrnewobs * sizeof(char *));
			memcpy(tmpptr, toret->arr_newobs, (toret->nrnewobs-1)*sizeof(char *));
			free(toret->arr_newobs);
			toret->arr_newobs = tmpptr;
			toret->arr_newobs[toret->nrnewobs-1] = (char *) malloc((strlen((yyvsp[(1) - (1)].t_pchar))+1)*sizeof(char));
			strcpy(toret->arr_newobs[toret->nrnewobs-1], (yyvsp[(1) - (1)].t_pchar));
			free((yyvsp[(1) - (1)].t_pchar));
		;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 321 "cassandra_grammar.y"
    {
			if (toret->Tr == NULL) {
				int i, j;
				toret->Tr = (float ***) malloc(toret->nract * sizeof(float **));
				for (i=0; i<toret->nract; i++) { 
					toret->Tr[i] = (float **) malloc(toret->nrstates * sizeof(float *));
					for (j=0; j<toret->nrstates; j++)
						toret->Tr[i][j] = (float *) malloc(toret->nrstates * sizeof(float));
				}
			} // Blergh.
		;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 333 "cassandra_grammar.y"
    {
			/* record the prob of getting from state1 to state2 through action */
			int i = 0, j, k;
			while ((yyvsp[(1) - (6)].t_pint)[i]!=-1) { // need to loop in case of *'s
				j = 0;
				while ((yyvsp[(3) - (6)].t_pint)[j]!=-1) {
					k = 0;
					while ((yyvsp[(5) - (6)].t_pint)[k]!=-1) toret->Tr [(yyvsp[(1) - (6)].t_pint)[i]] [(yyvsp[(3) - (6)].t_pint)[j]] [(yyvsp[(5) - (6)].t_pint)[k++]] = (yyvsp[(6) - (6)].t_float);
					j++;
				}
				i++;
			}
			free((yyvsp[(1) - (6)].t_pint)); free((yyvsp[(3) - (6)].t_pint)); free((yyvsp[(5) - (6)].t_pint));			
		;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 347 "cassandra_grammar.y"
    {
			/* record the prob of getting anywhere from state through action */
			int i = 0, j;
			while ((yyvsp[(1) - (4)].t_pint)[i]!=-1) { // need to loop in case of *'s
				j = 0;
				while ((yyvsp[(3) - (4)].t_pint)[j]!=-1) memcpy(toret->Tr [(yyvsp[(1) - (4)].t_pint)[i]] [(yyvsp[(3) - (4)].t_pint)[j++]] , theMatrix , toret->nrstates * sizeof(float *));
				i++;
			}
			free(theMatrix); theMatrix=NULL; theCursor=-1;
			free((yyvsp[(1) - (4)].t_pint)); free((yyvsp[(3) - (4)].t_pint));
		;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 358 "cassandra_grammar.y"
    {
			/* record the prob of getting anywhere from anywhere through action */
			int i = 0, j;
			while ((yyvsp[(1) - (2)].t_pint)[i]!=-1) { // need to loop in case of *'s
				for (j=0; j<toret->nrstates; j++)
					memcpy(toret->Tr[(yyvsp[(1) - (2)].t_pint)[i]][j], theMatrix + j * toret->nrstates, toret->nrstates * sizeof(float *));
				i++;
			}
			free(theMatrix); theMatrix=NULL; theCursor=-1;
			free((yyvsp[(1) - (2)].t_pint));
		;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 371 "cassandra_grammar.y"
    {
			if (toret->Ob == NULL) {
				int i, j;
				toret->Ob = (float ***) malloc(toret->nract * sizeof(float **));
				for (i=0; i<toret->nract; i++) {
					toret->Ob[i] = (float **) malloc(toret->nrstates * sizeof(float *));
					for (j=0; j<toret->nrstates; j++)
						toret->Ob[i][j] = (float *) malloc(toret->nrobs * sizeof(float));
				}
			}
		;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 382 "cassandra_grammar.y"
    {
			/* record the prob of getting 'obs' if 'action' is done in 'state' */
			int i = 0, j, k;
			while ((yyvsp[(1) - (6)].t_pint)[i]!=-1) { // need to loop in case of *'s
				j = 0;
				while ((yyvsp[(3) - (6)].t_pint)[j]!=-1) {
					k = 0;
					while ((yyvsp[(5) - (6)].t_pint)[k]!=-1) toret->Ob [(yyvsp[(1) - (6)].t_pint)[i]] [(yyvsp[(3) - (6)].t_pint)[j]] [(yyvsp[(5) - (6)].t_pint)[k++]] = (yyvsp[(6) - (6)].t_float);
					j++;
				}
				i++;
			}
			free((yyvsp[(1) - (6)].t_pint)); free((yyvsp[(3) - (6)].t_pint)); free((yyvsp[(5) - (6)].t_pint));
		;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 396 "cassandra_grammar.y"
    {
			/* record the prob of getting any observation when doing 'action' in 'state' */
			int i = 0, j;
			while ((yyvsp[(1) - (4)].t_pint)[i]!=-1) { // need to loop in case of *'s
				j = 0;
				while ((yyvsp[(3) - (4)].t_pint)[j]!=-1) memcpy(toret->Ob [(yyvsp[(1) - (4)].t_pint)[i]] [(yyvsp[(3) - (4)].t_pint)[j++]] , theMatrix, toret->nrobs * sizeof(float *));
				i++;
			}
			free(theMatrix); theMatrix=NULL; theCursor=-1;
			free((yyvsp[(1) - (4)].t_pint)); free((yyvsp[(3) - (4)].t_pint));
		;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 407 "cassandra_grammar.y"
    {
			/* record the prob of getting any observation from any state when doing 'action' */
			int i = 0, j;
			while ((yyvsp[(1) - (2)].t_pint)[i]!=-1) { // need to loop in case of *'s
				for (j=0; j<toret->nrstates; j++)
					memcpy(toret->Ob[(yyvsp[(1) - (2)].t_pint)[i]][j], theMatrix + j * toret->nrobs, toret->nrobs * sizeof(float *));
				i++;
			}
			free(theMatrix); theMatrix=NULL; theCursor=-1;
			free((yyvsp[(1) - (2)].t_pint));
		;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 421 "cassandra_grammar.y"
    { free((yyvsp[(1) - (8)].t_pint)); free((yyvsp[(3) - (8)].t_pint)); free((yyvsp[(5) - (8)].t_pint)); free((yyvsp[(7) - (8)].t_pint)); ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 422 "cassandra_grammar.y"
    { free((yyvsp[(1) - (6)].t_pint)); free((yyvsp[(3) - (6)].t_pint)); free((yyvsp[(5) - (6)].t_pint)); ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 423 "cassandra_grammar.y"
    { free((yyvsp[(1) - (4)].t_pint)); free((yyvsp[(3) - (4)].t_pint)); ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 427 "cassandra_grammar.y"
    { // return an int array containing [state, -1]
			(yyval.t_pint) = (int *) malloc(2 * sizeof(int)); (yyval.t_pint)[0] = (yyvsp[(1) - (1)].t_int); (yyval.t_pint)[1] = -1;
		;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 430 "cassandra_grammar.y"
    { // return an int array containing [state, -1]
			int i, found=0;
			(yyval.t_pint) = (int *) malloc(2 * sizeof(int));
			for (i=0; i < toret->nrstates; i++) // first find the number of the given state
				if (!strcmp((yyvsp[(1) - (1)].t_pchar), toret->arr_states[i])) {
					(yyval.t_pint)[0]=i; found=1;
				}
			if (!found) (yyval.t_pint)[0]=-1;
			(yyval.t_pint)[1]=-1;
			free((yyvsp[(1) - (1)].t_pchar));
		;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 441 "cassandra_grammar.y"
    { // return an int array containing all states, followed by -1
			int i;
			(yyval.t_pint)= (int *) malloc(sizeof(int) * (toret->nrstates+1));
			for (i=0; i < toret->nrstates; i++) (yyval.t_pint)[i] = i;
			(yyval.t_pint)[toret->nrstates] = -1;
		;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 448 "cassandra_grammar.y"
    { // return an int array containing [action, -1]
			(yyval.t_pint) = (int *) malloc(2 * sizeof(int)); (yyval.t_pint)[0] = (yyvsp[(1) - (1)].t_int); (yyval.t_pint)[1] = -1;
		;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 451 "cassandra_grammar.y"
    { // return an int array containing [action, -1]
			int i, found=0;
			(yyval.t_pint) = (int *) malloc(2 * sizeof(int));
			for (i=0; i < toret->nract; i++) // first find the number of the given action
				if (!strcmp((yyvsp[(1) - (1)].t_pchar), toret->arr_act[i])) {
					(yyval.t_pint)[0]=i; found=1;
				}
			if (!found) (yyval.t_pint)[0]=-1;
			(yyval.t_pint)[1]=-1;
			free((yyvsp[(1) - (1)].t_pchar));
		;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 462 "cassandra_grammar.y"
    { // return an int array containing all actions, followed by -1
			int i;
			(yyval.t_pint) = (int *) malloc(sizeof(int) * (toret->nract+1));
			for (i=0; i < toret->nract; i++) (yyval.t_pint)[i] = i;
			(yyval.t_pint)[toret->nract] = -1;
		;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 474 "cassandra_grammar.y"
    { // return an int array containing [observation, -1]
			(yyval.t_pint) = (int *) malloc(2 * sizeof(int)); (yyval.t_pint)[0] = (yyvsp[(1) - (1)].t_int); (yyval.t_pint)[1] = -1;
		;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 477 "cassandra_grammar.y"
    { // return an int array containing [observation, -1]
			int i, found=0;
			(yyval.t_pint) = (int *) malloc(2 * sizeof(int));
			for (i=0; i < toret->nrobs; i++) // first find the number of the given observation
				if (!strcmp((yyvsp[(1) - (1)].t_pchar), toret->arr_obs[i])) {
					(yyval.t_pint)[0]=i; found=1;
				}
			if (!found) (yyval.t_pint)[0]=-1;
			(yyval.t_pint)[1]=-1;
			free((yyvsp[(1) - (1)].t_pchar));
		;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 488 "cassandra_grammar.y"
    { // return an int array containing all observations, followed by -1
			int i;
			(yyval.t_pint) = (int *) malloc(sizeof(int) * (toret->nrobs+1));
			for (i=0; i < toret->nrobs; i++) (yyval.t_pint)[i] = i;
			(yyval.t_pint)[toret->nrobs] = -1;
		;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 497 "cassandra_grammar.y"
    {
			float uniformVal = 1.0 / toret->nrstates;
			if (theMatrix!=NULL) {
				free(theMatrix);
				printf("Possible problem: new matrix started but previous one not freed.\n");
			}
			theMatrix = (float *) malloc(toret->nrstates * sizeof(float));
			for (theCursor=0; theCursor<toret->nrstates; theCursor++) theMatrix[theCursor] = uniformVal;
		;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 507 "cassandra_grammar.y"
    {
			if (theMatrix==NULL) printf("Critical problem: theMatrix ain't there!\n");
			else theMatrix[theCursor++] = (yyvsp[(2) - (2)].t_float);
		;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 511 "cassandra_grammar.y"
    { // the end of the recursion, ie the beginning of the matrix
			if (theMatrix!=NULL) {
				free(theMatrix);
				printf("Possible problem: new matrix started but previous one not freed.\n");
			}
			theMatrix = (float *) malloc(toret->nrstates * sizeof(float));
			theCursor = 0;
			theMatrix[theCursor++] = (yyvsp[(1) - (1)].t_float);
		;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 522 "cassandra_grammar.y"
    {
			float uniformVal = 1.0 / toret->nrstates;
			if (theMatrix!=NULL) {
				free(theMatrix);
				printf("Possible problem: new matrix started but previous one not freed.\n");
			}
			theMatrix = (float *) malloc(toret->nrstates * toret->nrstates * sizeof(float));
			for (theCursor=0; theCursor<toret->nrstates*toret->nrstates; theCursor++) theMatrix[theCursor] = uniformVal;
		;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 531 "cassandra_grammar.y"
    {
			if (theMatrix!=NULL) {
				free(theMatrix);
				printf("Possible problem: new matrix started but previous one not freed.\n");
			}
			theMatrix = (float *) malloc(toret->nrstates * toret->nrstates * sizeof(float));
			for (theCursor=0; theCursor<toret->nrstates*toret->nrstates; theCursor++) theMatrix[theCursor] = 0.0;
			for (theCursor=0; theCursor<toret->nrstates; theCursor++) theMatrix[theCursor*toret->nrstates+theCursor] = 1.0;
			theCursor = toret->nrstates * toret->nrstates;
		;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 542 "cassandra_grammar.y"
    {
			if (theMatrix==NULL) printf("Critical problem: theMatrix ain't there!\n");
			else theMatrix[theCursor++] = (yyvsp[(2) - (2)].t_float);
		;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 546 "cassandra_grammar.y"
    { // the end of the recursion, ie the beginning of the matrix
			if (theMatrix!=NULL) {
				free(theMatrix);
				printf("Possible problem: new matrix started but previous one not freed.\n");
			}
			theMatrix = (float *) malloc(toret->nrstates * toret->nrstates * sizeof(float));
			theCursor = 0;
			theMatrix[theCursor++] = (yyvsp[(1) - (1)].t_float);
		;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 557 "cassandra_grammar.y"
    {
			float uniformVal = 1.0 / toret->nrobs;
			if (theMatrix!=NULL) {
				free(theMatrix);
				printf("Possible problem: new matrix started but previous one not freed.\n");
			}
			theMatrix = (float *) malloc(toret->nrobs * sizeof(float));
			for (theCursor=0; theCursor<toret->nrobs; theCursor++) theMatrix[theCursor] = uniformVal;
		;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 567 "cassandra_grammar.y"
    {
			if (theMatrix==NULL) printf("Critical problem: theMatrix ain't there!\n");
			else theMatrix[theCursor++] = (yyvsp[(2) - (2)].t_float);
		;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 571 "cassandra_grammar.y"
    { // the end of the recursion, ie the beginning of the matrix
			if (theMatrix!=NULL) {
				free(theMatrix);
				printf("Possible problem: new matrix started but previous one not freed.\n");
			}
			theMatrix = (float *) malloc(toret->nrobs * sizeof(float));
			theCursor = 0;
			theMatrix[theCursor++] = (yyvsp[(1) - (1)].t_float);
		;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 582 "cassandra_grammar.y"
    {
			float uniformVal = 1.0 / toret->nrobs;
			if (theMatrix!=NULL) {
				free(theMatrix);
				printf("Possible problem: new matrix started but previous one not freed.\n");
			}
			theMatrix = (float *) malloc(toret->nrstates * toret->nrobs * sizeof(float));
			for (theCursor=0; theCursor<toret->nrstates * toret->nrobs; theCursor++) theMatrix[theCursor] = uniformVal;
		;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 592 "cassandra_grammar.y"
    {
			if (theMatrix==NULL) printf("Critical problem: theMatrix ain't there!\n");
			else theMatrix[theCursor++] = (yyvsp[(2) - (2)].t_float);
		;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 596 "cassandra_grammar.y"
    { // the end of the recursion, ie the beginning of the matrix
			if (theMatrix!=NULL) {
				free(theMatrix);
				printf("Possible problem: new matrix started but previous one not freed.\n");
			}
			theMatrix = (float *) malloc(toret->nrstates * toret->nrobs * sizeof(float));
			theCursor = 0;
			theMatrix[theCursor++] = (yyvsp[(1) - (1)].t_float);
		;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 611 "cassandra_grammar.y"
    { (yyval.t_float) = (float) (yyvsp[(1) - (1)].t_float); ;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 612 "cassandra_grammar.y"
    { (yyval.t_float) = (yyvsp[(1) - (2)].t_int) * (yyvsp[(2) - (2)].t_float); ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 612 "cassandra_grammar.y"
    { (yyval.t_float) = (float) ((yyvsp[(1) - (2)].t_int) * (yyvsp[(2) - (2)].t_int)); ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 613 "cassandra_grammar.y"
    { (yyval.t_int) = 1; ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 613 "cassandra_grammar.y"
    { (yyval.t_int) = -1; ;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 613 "cassandra_grammar.y"
    { (yyval.t_int) = 1; ;}
    break;



/* Line 1455 of yacc.c  */
#line 2305 "cassandra_grammar.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
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

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
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
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
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
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 616 "cassandra_grammar.y"


Cassandra_return *parse_Cassandra(FILE *f) {
	// This is the function that should be called from the cassandra_env constructor to load the data from file
  
	extern FILE *yyin;
	yyin = f;
	if (yyparse()!=0) return NULL;
	else return toret;
}


void yyerror(const char *s) { /* Called by yyparse on error */
	  printf("%s\n", s);
}

int nrdigit(int a, int base) { // returns number of digits a (in base 10) takes in base 'base'
	int i = 0;
	while (a/base > 0) { i++; a = a / base; }
	return i+1;
}
		

/*int main(int argc,char *argv[])
{Cassandra_return *ret;
  FILE *f;
 
char name[]={"/cheese.95.POMDP"};
 char buffer[80];
 getcwd(buffer, 80);
 printf("The current directory is: %s\n", buffer);
  char *cf=strcat(buffer,name);
 cout<<cf<<endl;  //char *filename = 当前目录+文件名; 利用getcwd(char* buff)函数获取当前工作路径
 if((f=fopen(cf,"r"))==NULL)
 {
     printf("cannot open file\n");
 exit(0);
}
  else printf("success\n");
 
 
 ret=parse_Cassandra(f);

cout<<"DISCOUNT="<<ret->discount<<endl;//加上61行toret->discount=$3;可正确输出；否则输出不对

cout<<"STATES="<<ret->nrstates<<endl;
cout<<"ACTIONS="<<ret->nract<<endl;
cout<<"OBSERVATIONS="<<ret->nrobs<<endl;

cout<<"VALUES="<<ret->values<<endl;//不管数据中是REWARD还是COST，只要用ret->vREWARD,输出0，用ret->vCOST, 输出1，只是输出了结构体中的值，而用ret->valuetype,会一直输出0，所以要在88行设置值， 怎么赋值？ 
cout<<"START="<<ret->Start[1]<<endl;//这句写法？ 结果不对


cout<<"Ob[1][2][2]="<<ret->Ob[1][2][2]<<endl;
 cout<<"Tr[1][11][7]="<<ret->Tr[1][1][1]<<endl;


//cout<<"Ob[0][59][1]="<<ret->Ob[0][59][1]<<endl;

//cout<<"Rw[1][2][56][0]="<<ret->Rw[1][2][56][0]<<endl;
}
*/


		

