
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
#define YYLSP_NEEDED 1



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "grammar.y"

  #define __extension__ 
  #define __attribute__(x) /*Nothing */

  #include <stdio.h>
  #include "symbol_table.h"
  #include "stack.h"
  #include <stdlib.h>
  #include "string.h"
  
  symbol_t table;
  symbol_t global;
  stack pilha_ids;
  symbol_t typedefs;
  symbol_t defines;
  symbol_t arquivos;
  stack nomesArq;
  stack tipos[2];
  stack parenteses;
  stack linhasFor;
  stack colunasFor;
  stack linhasArq;
  stack colunasArq;
  int atual = 0;
  char* nomeFuncao = NULL;
  char* NOMEGLOBAL = NULL;
  int numPonteiros = 0;
  int line = 1;
  int column = 1;

  #define INDEFINIDO -1
  #define LISTA_IDS 600
  #define NADA 601
  #define ARRAY 602


/* Line 189 of yacc.c  */
#line 110 "grammar.tab.c"

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
     IDENTIFIER = 258,
     TYPE_NAME = 259,
     CONSTANT = 260,
     CONSTANT_INT = 261,
     CONSTANT_FLOAT = 262,
     STRING_LITERAL = 263,
     SIZEOF = 264,
     PTR_OP = 265,
     INC_OP = 266,
     DEC_OP = 267,
     LEFT_OP = 268,
     RIGHT_OP = 269,
     LE_OP = 270,
     GE_OP = 271,
     EQ_OP = 272,
     NE_OP = 273,
     AND_OP = 274,
     OR_OP = 275,
     MUL_ASSIGN = 276,
     DIV_ASSIGN = 277,
     MOD_ASSIGN = 278,
     ADD_ASSIGN = 279,
     SUB_ASSIGN = 280,
     LEFT_ASSIGN = 281,
     RIGHT_ASSIGN = 282,
     AND_ASSIGN = 283,
     XOR_ASSIGN = 284,
     OR_ASSIGN = 285,
     TYPEDEF = 286,
     EXTERN = 287,
     STATIC = 288,
     AUTO = 289,
     REGISTER = 290,
     CHAR = 291,
     SHORT = 292,
     INT = 293,
     LONG = 294,
     SIGNED = 295,
     UNSIGNED = 296,
     FLOAT = 297,
     DOUBLE = 298,
     CONST = 299,
     VOLATILE = 300,
     VOID = 301,
     STRUCT = 302,
     UNION = 303,
     ENUM = 304,
     ELLIPSIS = 305,
     CASE = 306,
     DEFAULT = 307,
     IF = 308,
     ELSE = 309,
     SWITCH = 310,
     WHILE = 311,
     DO = 312,
     FOR = 313,
     GOTO = 314,
     CONTINUE = 315,
     BREAK = 316,
     RETURN = 317
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 38 "grammar.y"

	char* lexema;
	struct Tgeral { int token; char* lexema; entry_t* entrada; } geral;
	int num;
	struct Tgeral2 { int num; int token; entry_t* entrada; } geral2;



/* Line 214 of yacc.c  */
#line 217 "grammar.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 242 "grammar.tab.c"

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
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

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
#define YYFINAL  63
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1266

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  87
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  70
/* YYNRULES -- Number of rules.  */
#define YYNRULES  221
/* YYNRULES -- Number of states.  */
#define YYNSTATES  358

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   317

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    74,     2,     2,     2,    76,    69,     2,
      63,    64,    70,    71,    68,    72,    67,    75,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    82,    84,
      77,    83,    78,    81,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    65,     2,    66,    79,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    85,    80,    86,    73,     2,     2,     2,
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
      55,    56,    57,    58,    59,    60,    61,    62
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    11,    13,    17,    19,
      24,    28,    33,    37,    41,    44,    47,    49,    53,    55,
      58,    61,    64,    67,    72,    74,    76,    78,    80,    82,
      84,    86,    91,    93,    97,   101,   105,   107,   111,   115,
     117,   121,   125,   127,   131,   135,   139,   143,   145,   149,
     153,   155,   159,   161,   165,   167,   171,   173,   177,   179,
     183,   185,   191,   193,   197,   199,   201,   203,   205,   207,
     209,   211,   213,   215,   217,   219,   221,   225,   227,   230,
     234,   236,   239,   241,   244,   246,   249,   251,   255,   257,
     261,   263,   265,   267,   269,   271,   273,   275,   277,   279,
     281,   283,   285,   287,   289,   291,   293,   295,   301,   306,
     309,   311,   313,   315,   318,   322,   325,   327,   330,   332,
     334,   338,   340,   343,   347,   352,   358,   361,   363,   367,
     369,   373,   375,   377,   380,   382,   384,   388,   393,   397,
     398,   399,   406,   411,   412,   417,   419,   422,   425,   429,
     431,   434,   436,   440,   442,   446,   449,   452,   454,   456,
     460,   462,   465,   467,   469,   472,   476,   479,   483,   487,
     492,   495,   499,   503,   508,   510,   514,   519,   521,   525,
     527,   529,   531,   533,   535,   537,   541,   546,   550,   553,
     557,   561,   566,   568,   571,   573,   576,   578,   581,   587,
     595,   601,   607,   615,   616,   621,   626,   632,   636,   639,
     642,   645,   649,   651,   654,   656,   658,   663,   667,   671,
     674,   676
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     156,     0,    -1,     3,    -1,     5,    -1,     6,    -1,     7,
      -1,     8,    -1,    63,   107,    64,    -1,    88,    -1,    89,
      65,   107,    66,    -1,    89,    63,    64,    -1,    89,    63,
      90,    64,    -1,    89,    67,     3,    -1,    89,    10,     3,
      -1,    89,    11,    -1,    89,    12,    -1,   105,    -1,    90,
      68,   105,    -1,    89,    -1,    11,    91,    -1,    12,    91,
      -1,    92,    93,    -1,     9,    91,    -1,     9,    63,   137,
      64,    -1,    69,    -1,    70,    -1,    71,    -1,    72,    -1,
      73,    -1,    74,    -1,    91,    -1,    63,   137,    64,    93,
      -1,    93,    -1,    94,    70,    93,    -1,    94,    75,    93,
      -1,    94,    76,    93,    -1,    94,    -1,    95,    71,    94,
      -1,    95,    72,    94,    -1,    95,    -1,    96,    13,    95,
      -1,    96,    14,    95,    -1,    96,    -1,    97,    77,    96,
      -1,    97,    78,    96,    -1,    97,    15,    96,    -1,    97,
      16,    96,    -1,    97,    -1,    98,    17,    97,    -1,    98,
      18,    97,    -1,    98,    -1,    99,    69,    98,    -1,    99,
      -1,   100,    79,    99,    -1,   100,    -1,   101,    80,   100,
      -1,   101,    -1,   102,    19,   101,    -1,   102,    -1,   103,
      20,   102,    -1,   103,    -1,   103,    81,   107,    82,   104,
      -1,   104,    -1,    91,   106,   105,    -1,    83,    -1,    21,
      -1,    22,    -1,    23,    -1,    24,    -1,    25,    -1,    26,
      -1,    27,    -1,    28,    -1,    29,    -1,    30,    -1,   105,
      -1,   107,    68,   105,    -1,   104,    -1,   110,    84,    -1,
     110,   111,    84,    -1,   113,    -1,   113,   110,    -1,   114,
      -1,   114,   110,    -1,   125,    -1,   125,   110,    -1,   112,
      -1,   111,    68,   112,    -1,   126,    -1,   126,    83,   140,
      -1,    31,    -1,    32,    -1,    33,    -1,    34,    -1,    35,
      -1,    46,    -1,    36,    -1,    37,    -1,    38,    -1,    39,
      -1,    42,    -1,    43,    -1,    40,    -1,    41,    -1,   115,
      -1,   122,    -1,     4,    -1,   116,     3,    85,   117,    86,
      -1,   116,    85,   117,    86,    -1,   116,     3,    -1,    47,
      -1,    48,    -1,   118,    -1,   117,   118,    -1,   119,   120,
      84,    -1,   114,   119,    -1,   114,    -1,   125,   119,    -1,
     125,    -1,   121,    -1,   120,    68,   121,    -1,   126,    -1,
      82,   108,    -1,   126,    82,   108,    -1,    49,    85,   123,
      86,    -1,    49,     3,    85,   123,    86,    -1,    49,     3,
      -1,   124,    -1,   123,    68,   124,    -1,     3,    -1,     3,
      83,   108,    -1,    44,    -1,    45,    -1,   131,   127,    -1,
     127,    -1,     3,    -1,    63,   126,    64,    -1,   127,    65,
     108,    66,    -1,   127,    65,    66,    -1,    -1,    -1,   127,
      63,   128,   133,   129,    64,    -1,   127,    63,   136,    64,
      -1,    -1,   127,    63,   130,    64,    -1,    70,    -1,    70,
     132,    -1,    70,   131,    -1,    70,   132,   131,    -1,   125,
      -1,   132,   125,    -1,   134,    -1,   134,    68,    50,    -1,
     135,    -1,   134,    68,   135,    -1,   110,   126,    -1,   110,
     138,    -1,   110,    -1,     3,    -1,   136,    68,     3,    -1,
     119,    -1,   119,   138,    -1,   131,    -1,   139,    -1,   131,
     139,    -1,    63,   138,    64,    -1,    65,    66,    -1,    65,
     108,    66,    -1,   139,    65,    66,    -1,   139,    65,   108,
      66,    -1,    63,    64,    -1,    63,   133,    64,    -1,   139,
      63,    64,    -1,   139,    63,   133,    64,    -1,   105,    -1,
      85,   141,    86,    -1,    85,   141,    68,    86,    -1,   140,
      -1,   141,    68,   140,    -1,   143,    -1,   144,    -1,   147,
      -1,   148,    -1,   149,    -1,   152,    -1,     3,    82,   142,
      -1,    51,   108,    82,   142,    -1,    52,    82,   142,    -1,
      85,    86,    -1,    85,   146,    86,    -1,    85,   145,    86,
      -1,    85,   145,   146,    86,    -1,   109,    -1,   145,   109,
      -1,   142,    -1,   146,   142,    -1,    84,    -1,   107,    84,
      -1,    53,    63,   107,    64,   142,    -1,    53,    63,   107,
      64,   142,    54,   142,    -1,    55,    63,   107,    64,   142,
      -1,    56,    63,   107,    64,   142,    -1,    57,   142,    56,
      63,   107,    64,    84,    -1,    -1,    58,    63,   150,   151,
      -1,   147,   147,    64,   142,    -1,   147,   147,   107,    64,
     142,    -1,    59,     3,    84,    -1,    60,    84,    -1,    61,
      84,    -1,    62,    84,    -1,    62,   107,    84,    -1,   154,
      -1,   153,   154,    -1,   155,    -1,   109,    -1,   110,   126,
     145,   144,    -1,   110,   126,   144,    -1,   126,   145,   144,
      -1,   126,   144,    -1,   153,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    81,    81,   107,   108,   109,   110,   111,   115,   116,
     117,   118,   119,   120,   121,   128,   138,   139,   143,   144,
     151,   158,   159,   160,   164,   165,   166,   167,   168,   169,
     173,   174,   178,   185,   198,   211,   226,   227,   235,   246,
     247,   248,   252,   253,   254,   255,   256,   260,   261,   262,
     266,   267,   271,   272,   284,   285,   296,   297,   301,   302,
     306,   307,   311,   312,   325,   326,   327,   328,   329,   330,
     331,   332,   333,   334,   335,   339,   340,   344,   348,   349,
     398,   399,   407,   408,   409,   410,   414,   420,   429,   430,
     434,   435,   436,   437,   438,   442,   443,   444,   445,   446,
     447,   448,   449,   450,   451,   452,   453,   457,   458,   459,
     463,   464,   468,   469,   473,   477,   478,   479,   480,   484,
     485,   489,   490,   491,   495,   496,   497,   501,   502,   506,
     507,   511,   512,   516,   517,   521,   542,   543,   568,   595,
     642,   593,   649,   658,   656,   713,   714,   715,   716,   720,
     721,   726,   727,   731,   732,   736,   778,   779,   783,   784,
     788,   789,   793,   794,   795,   799,   800,   801,   802,   803,
     804,   805,   806,   807,   811,   812,   813,   817,   818,   822,
     823,   824,   825,   826,   827,   831,   832,   833,   837,   838,
     839,   840,   844,   845,   849,   850,   854,   855,   859,   860,
     861,   865,   866,   869,   867,   882,   892,   903,   904,   905,
     906,   907,   911,   912,   916,   917,   921,   922,   923,   924,
     928,   929
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "IDENTIFIER", "TYPE_NAME", "CONSTANT",
  "CONSTANT_INT", "CONSTANT_FLOAT", "STRING_LITERAL", "SIZEOF", "PTR_OP",
  "INC_OP", "DEC_OP", "LEFT_OP", "RIGHT_OP", "LE_OP", "GE_OP", "EQ_OP",
  "NE_OP", "AND_OP", "OR_OP", "MUL_ASSIGN", "DIV_ASSIGN", "MOD_ASSIGN",
  "ADD_ASSIGN", "SUB_ASSIGN", "LEFT_ASSIGN", "RIGHT_ASSIGN", "AND_ASSIGN",
  "XOR_ASSIGN", "OR_ASSIGN", "TYPEDEF", "EXTERN", "STATIC", "AUTO",
  "REGISTER", "CHAR", "SHORT", "INT", "LONG", "SIGNED", "UNSIGNED",
  "FLOAT", "DOUBLE", "CONST", "VOLATILE", "VOID", "STRUCT", "UNION",
  "ENUM", "ELLIPSIS", "CASE", "DEFAULT", "IF", "ELSE", "SWITCH", "WHILE",
  "DO", "FOR", "GOTO", "CONTINUE", "BREAK", "RETURN", "'('", "')'", "'['",
  "']'", "'.'", "','", "'&'", "'*'", "'+'", "'-'", "'~'", "'!'", "'/'",
  "'%'", "'<'", "'>'", "'^'", "'|'", "'?'", "':'", "'='", "';'", "'{'",
  "'}'", "$accept", "primary_expression", "postfix_expression",
  "argument_expression_list", "unary_expression", "unary_operator",
  "cast_expression", "multiplicative_expression", "additive_expression",
  "shift_expression", "relational_expression", "equality_expression",
  "and_expression", "exclusive_or_expression", "inclusive_or_expression",
  "logical_and_expression", "logical_or_expression",
  "conditional_expression", "assignment_expression", "assignment_operator",
  "expression", "constant_expression", "declaration",
  "declaration_specifiers", "init_declarator_list", "init_declarator",
  "storage_class_specifier", "type_specifier", "struct_or_union_specifier",
  "struct_or_union", "struct_declaration_list", "struct_declaration",
  "specifier_qualifier_list", "struct_declarator_list",
  "struct_declarator", "enum_specifier", "enumerator_list", "enumerator",
  "type_qualifier", "declarator", "direct_declarator", "$@1", "$@2", "$@3",
  "pointer", "type_qualifier_list", "parameter_type_list",
  "parameter_list", "parameter_declaration", "identifier_list",
  "type_name", "abstract_declarator", "direct_abstract_declarator",
  "initializer", "initializer_list", "statement", "labeled_statement",
  "compound_statement", "declaration_list", "statement_list",
  "expression_statement", "selection_statement", "iteration_statement",
  "$@4", "for_statement", "jump_statement", "translation_unit",
  "external_declaration", "function_definition", "init_unit", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,    40,    41,    91,    93,    46,    44,    38,
      42,    43,    45,   126,    33,    47,    37,    60,    62,    94,
     124,    63,    58,    61,    59,   123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    87,    88,    88,    88,    88,    88,    88,    89,    89,
      89,    89,    89,    89,    89,    89,    90,    90,    91,    91,
      91,    91,    91,    91,    92,    92,    92,    92,    92,    92,
      93,    93,    94,    94,    94,    94,    95,    95,    95,    96,
      96,    96,    97,    97,    97,    97,    97,    98,    98,    98,
      99,    99,   100,   100,   101,   101,   102,   102,   103,   103,
     104,   104,   105,   105,   106,   106,   106,   106,   106,   106,
     106,   106,   106,   106,   106,   107,   107,   108,   109,   109,
     110,   110,   110,   110,   110,   110,   111,   111,   112,   112,
     113,   113,   113,   113,   113,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   115,   115,   115,
     116,   116,   117,   117,   118,   119,   119,   119,   119,   120,
     120,   121,   121,   121,   122,   122,   122,   123,   123,   124,
     124,   125,   125,   126,   126,   127,   127,   127,   127,   128,
     129,   127,   127,   130,   127,   131,   131,   131,   131,   132,
     132,   133,   133,   134,   134,   135,   135,   135,   136,   136,
     137,   137,   138,   138,   138,   139,   139,   139,   139,   139,
     139,   139,   139,   139,   140,   140,   140,   141,   141,   142,
     142,   142,   142,   142,   142,   143,   143,   143,   144,   144,
     144,   144,   145,   145,   146,   146,   147,   147,   148,   148,
     148,   149,   149,   150,   149,   151,   151,   152,   152,   152,
     152,   152,   153,   153,   154,   154,   155,   155,   155,   155,
     156,   156
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     3,     1,     4,
       3,     4,     3,     3,     2,     2,     1,     3,     1,     2,
       2,     2,     2,     4,     1,     1,     1,     1,     1,     1,
       1,     4,     1,     3,     3,     3,     1,     3,     3,     1,
       3,     3,     1,     3,     3,     3,     3,     1,     3,     3,
       1,     3,     1,     3,     1,     3,     1,     3,     1,     3,
       1,     5,     1,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     1,     2,     3,
       1,     2,     1,     2,     1,     2,     1,     3,     1,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     5,     4,     2,
       1,     1,     1,     2,     3,     2,     1,     2,     1,     1,
       3,     1,     2,     3,     4,     5,     2,     1,     3,     1,
       3,     1,     1,     2,     1,     1,     3,     4,     3,     0,
       0,     6,     4,     0,     4,     1,     2,     2,     3,     1,
       2,     1,     3,     1,     3,     2,     2,     1,     1,     3,
       1,     2,     1,     1,     2,     3,     2,     3,     3,     4,
       2,     3,     3,     4,     1,     3,     4,     1,     3,     1,
       1,     1,     1,     1,     1,     3,     4,     3,     2,     3,
       3,     4,     1,     2,     1,     2,     1,     2,     5,     7,
       5,     5,     7,     0,     4,     4,     5,     3,     2,     2,
       2,     3,     1,     2,     1,     1,     4,     3,     3,     2,
       1,     0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
     221,   135,   106,    90,    91,    92,    93,    94,    96,    97,
      98,    99,   102,   103,   100,   101,   131,   132,    95,   110,
     111,     0,     0,   145,   215,     0,    80,    82,   104,     0,
     105,    84,     0,   134,     0,   220,   212,   214,     0,   126,
       0,     0,   149,   147,   146,    78,     0,    86,    88,    81,
      83,   109,     0,    85,     0,   192,     0,   219,     0,   139,
       0,   133,   213,     1,     0,   129,     0,   127,   136,   150,
     148,     0,    79,     0,   217,     0,     0,   116,     0,   112,
       0,   118,     2,     3,     4,     5,     6,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    24,    25,    26,    27,    28,    29,   196,   188,
       8,    18,    30,     0,    32,    36,    39,    42,    47,    50,
      52,    54,    56,    58,    60,    62,    75,     0,   194,   179,
     180,     0,     0,   181,   182,   183,   184,    88,   193,   218,
     158,     0,     0,     0,     2,   138,    30,    77,     0,     0,
       0,     0,   124,    87,     0,   174,    89,   216,     0,   115,
     108,   113,     0,     0,   119,   121,   117,     0,     0,    22,
       0,    19,    20,     0,     0,     0,     0,     0,     0,   203,
       0,   208,   209,   210,     0,     0,   160,     0,     0,    14,
      15,     0,     0,     0,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    64,     0,    21,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   197,   190,     0,
     189,   195,   157,   140,   151,   153,   144,   142,     0,   137,
     125,   130,   128,   177,     0,   107,   122,     0,   114,     0,
     185,     0,     0,   187,     0,     0,     0,     0,     0,   207,
     211,     7,     0,     0,   162,   161,   163,     0,    13,    10,
       0,    16,     0,    12,    63,    33,    34,    35,    37,    38,
      40,    41,    45,    46,    43,    44,    48,    49,    51,    53,
      55,    57,    59,     0,    76,   191,     0,   155,   162,   156,
       0,     0,   159,     0,   175,   120,   123,    23,   186,     0,
       0,     0,     0,     0,   204,   170,     0,     0,   166,     0,
     164,     0,     0,    31,    11,     0,     9,     0,   141,   152,
     154,   176,   178,   198,   200,   201,     0,     0,   171,   165,
     167,   172,     0,   168,     0,    17,    61,     0,     0,     0,
       0,   173,   169,   199,   202,   205,     0,   206
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,   110,   111,   270,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   205,
     127,   148,    55,    56,    46,    47,    26,    27,    28,    29,
      78,    79,    80,   163,   164,    30,    66,    67,    31,    32,
      33,   141,   300,   142,    34,    44,   316,   234,   235,   143,
     187,   317,   266,   156,   244,   128,   129,   130,    58,   132,
     133,   134,   135,   258,   314,   136,    35,    36,    37,    38
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -247
static const yytype_int16 yypact[] =
{
    1110,  -247,  -247,  -247,  -247,  -247,  -247,  -247,  -247,  -247,
    -247,  -247,  -247,  -247,  -247,  -247,  -247,  -247,  -247,  -247,
    -247,     7,    39,   113,  -247,    16,  1202,  1202,  -247,    13,
    -247,  1202,   734,    85,    62,  1110,  -247,  -247,    21,   -56,
      60,   -26,  -247,  -247,   113,  -247,   -15,  -247,   715,  -247,
    -247,   -31,  1217,  -247,   319,  -247,    16,  -247,   734,    17,
     822,    85,  -247,  -247,    60,   -17,   -25,  -247,  -247,  -247,
    -247,    39,  -247,   660,  -247,   734,  1217,  1217,   298,  -247,
      52,  1217,    -8,  -247,  -247,  -247,  -247,   966,   991,   991,
    1016,    69,    67,   122,   137,   643,   140,   205,   155,   157,
     781,   872,  -247,  -247,  -247,  -247,  -247,  -247,  -247,  -247,
    -247,   132,   265,  1016,  -247,   126,    35,   224,    75,   225,
     176,   144,   167,   234,    30,  -247,  -247,    71,  -247,  -247,
    -247,   403,   487,  -247,  -247,  -247,  -247,   173,  -247,  -247,
    -247,  1202,   197,   -34,  -247,  -247,  -247,  -247,   203,   -10,
    1016,    60,  -247,  -247,   660,  -247,  -247,  -247,   381,  -247,
    -247,  -247,  1016,    72,  -247,   193,  -247,   643,   872,  -247,
    1016,  -247,  -247,   195,   643,  1016,  1016,  1016,   222,  -247,
     198,  -247,  -247,  -247,    78,   141,    84,   215,   278,  -247,
    -247,   243,  1016,   280,  -247,  -247,  -247,  -247,  -247,  -247,
    -247,  -247,  -247,  -247,  -247,  1016,  -247,  1016,  1016,  1016,
    1016,  1016,  1016,  1016,  1016,  1016,  1016,  1016,  1016,  1016,
    1016,  1016,  1016,  1016,  1016,  1016,  1016,  -247,  -247,   523,
    -247,  -247,    53,  -247,   216,  -247,  -247,  -247,   294,  -247,
    -247,  -247,  -247,  -247,    33,  -247,  -247,    52,  -247,  1016,
    -247,   235,   643,  -247,   147,   149,   152,   237,   798,  -247,
    -247,  -247,   594,   894,    98,  -247,   125,  1016,  -247,  -247,
     154,  -247,   163,  -247,  -247,  -247,  -247,  -247,   126,   126,
      35,    35,   224,   224,   224,   224,    75,    75,   225,   176,
     144,   167,   234,    28,  -247,  -247,  1063,  -247,    70,  -247,
     239,   474,  -247,   607,  -247,  -247,  -247,  -247,  -247,   643,
     643,   643,  1016,   798,  -247,  -247,   240,   241,  -247,   232,
     125,  1156,   919,  -247,  -247,  1016,  -247,  1016,  -247,  -247,
    -247,  -247,  -247,   254,  -247,  -247,   160,   944,  -247,  -247,
    -247,  -247,   246,  -247,   253,  -247,  -247,   643,   236,   643,
     162,  -247,  -247,  -247,  -247,  -247,   643,  -247
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -247,  -247,  -247,  -247,   -42,  -247,   -81,    49,    51,    18,
      48,   109,   111,   127,   110,   145,  -247,   -57,   -67,  -247,
     -93,   -65,    14,     0,  -247,   302,  -247,   -37,  -247,  -247,
     307,   -55,   -64,  -247,   148,  -247,   322,   245,    36,   -20,
     -30,  -247,  -247,  -247,   -22,  -247,  -132,  -247,    86,  -247,
     226,  -162,  -236,  -143,  -247,   -38,  -247,    20,   139,   266,
    -246,  -247,  -247,  -247,  -247,  -247,  -247,   363,  -247,  -247
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -144
static const yytype_int16 yytable[] =
{
      25,    43,    41,   147,    61,    48,   155,   184,   185,   233,
      39,   243,   313,   159,    24,    77,    51,   166,   146,     1,
     140,    63,    70,   161,   265,   173,    49,    50,   320,    64,
     237,    53,   206,   147,   238,    25,   137,   186,    68,    77,
      77,    77,     1,   151,    77,   169,   171,   172,   146,    24,
     224,   137,    57,    71,    76,     1,     1,   178,   151,    42,
     165,   152,   320,    65,    77,     1,   150,   337,    74,    72,
     299,   146,   138,     1,   167,   185,   240,   185,   139,    22,
      69,  -143,   254,   255,   256,   241,    23,   155,    81,   138,
     214,   215,    40,   147,   231,   157,   226,   246,    52,   272,
      45,   303,    22,   161,   186,   147,   210,   211,   146,    23,
     327,   225,    81,    81,    81,    22,   296,    81,   263,   304,
     146,    77,    23,    23,   271,    22,   275,   276,   277,   250,
     175,    77,   293,   296,   162,   263,   253,    81,   274,   226,
     247,   232,   188,   189,   190,   138,   226,   262,    59,   263,
      60,   174,   216,   217,    23,   227,   248,    16,    17,   294,
     332,   262,   260,   263,   264,   146,   146,   146,   146,   146,
     146,   146,   146,   146,   146,   146,   146,   146,   146,   146,
     146,   146,   146,    23,   306,   176,   323,    75,   321,   342,
     322,   231,   147,   131,    81,   191,   207,   192,   319,   193,
     177,   208,   209,   179,    81,   261,   147,   146,   180,   226,
     298,   309,   297,   310,   308,   226,   311,   226,   324,   336,
     226,   146,   325,   221,   348,   146,   356,   165,   226,   326,
     226,   226,   282,   283,   284,   285,   155,   212,   213,   181,
     264,   182,   218,   219,   350,   220,   144,   222,    83,    84,
      85,    86,    87,   223,    88,    89,    73,   344,   345,   278,
     279,   236,   232,   280,   281,   147,   286,   287,    61,   239,
     346,   333,   334,   335,   298,   249,    41,   252,   257,   267,
     146,   268,   259,   273,   301,   146,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   232,   302,   340,   307,
     312,   232,     2,   328,   338,   339,   101,   269,   347,   353,
     351,   355,   102,   103,   104,   105,   106,   107,   357,   352,
     354,   232,    82,     2,    83,    84,    85,    86,    87,   288,
      88,    89,   289,   291,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,   204,   290,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,   292,
      90,    91,    92,   153,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   158,   160,     2,   149,   330,   102,   103,
     104,   105,   106,   107,   251,   305,   242,   229,    62,     0,
       0,     0,     0,   108,    54,   109,    82,     2,    83,    84,
      85,    86,    87,     0,    88,    89,     0,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,     0,     0,     0,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,     0,    90,    91,    92,     0,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   245,     0,     0,
       0,     0,   102,   103,   104,   105,   106,   107,     2,     0,
       0,     0,     0,     0,     0,     0,     0,   108,    54,   228,
      82,     0,    83,    84,    85,    86,    87,     0,    88,    89,
       0,     0,     0,     0,     0,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,   329,     0,    82,     0,    83,    84,
      85,    86,    87,     0,    88,    89,     0,     0,    90,    91,
      92,     0,    93,    94,    95,    96,    97,    98,    99,   100,
     101,     0,     0,     0,     0,     0,   102,   103,   104,   105,
     106,   107,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   108,    54,   230,    90,    91,    92,     0,    93,    94,
      95,    96,    97,    98,    99,   100,   101,     0,     0,     0,
       0,     0,   102,   103,   104,   105,   106,   107,     2,     0,
       0,     0,     0,     0,     0,     0,     0,   108,    54,   295,
     144,     0,    83,    84,    85,    86,    87,     0,    88,    89,
       0,     0,     0,     0,     0,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,     0,     0,    82,     0,    83,    84,
      85,    86,    87,     0,    88,    89,     0,   262,   315,   263,
       0,     0,     0,   144,    23,    83,    84,    85,    86,    87,
     101,    88,    89,     0,     0,     0,   102,   103,   104,   105,
     106,   107,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   154,   331,    90,    91,    92,     0,    93,    94,
      95,    96,    97,    98,    99,   100,   101,     0,     0,     0,
       0,     0,   102,   103,   104,   105,   106,   107,     0,     2,
       0,     0,     0,   101,     0,     0,     0,   108,    54,   102,
     103,   104,   105,   106,   107,     0,     0,     0,     2,     0,
       0,     0,     0,     0,     0,   154,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,   144,     0,    83,    84,    85,    86,
      87,     0,    88,    89,     0,     0,     0,     0,    73,     0,
      54,   144,     0,    83,    84,    85,    86,    87,     0,    88,
      89,     0,     0,     0,     0,     0,     0,     0,     0,    54,
       0,     0,     0,     0,     0,   144,     0,    83,    84,    85,
      86,    87,     0,    88,    89,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   101,     0,     0,     0,     0,     0,
     102,   103,   104,   105,   106,   107,     0,     0,     0,     0,
       0,   101,     0,     0,     0,   183,     0,   102,   103,   104,
     105,   106,   107,     0,     0,   144,     2,    83,    84,    85,
      86,    87,   108,    88,    89,   101,     0,     0,   145,     0,
       0,   102,   103,   104,   105,   106,   107,   144,     0,    83,
      84,    85,    86,    87,     0,    88,    89,     0,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,   144,     0,    83,    84,    85,    86,    87,     0,
      88,    89,     0,     0,     0,   101,     0,     0,     0,     0,
       0,   102,   103,   104,   105,   106,   107,   144,     0,    83,
      84,    85,    86,    87,     0,    88,    89,   101,     0,     0,
     318,     0,     0,   102,   103,   104,   105,   106,   107,   144,
       0,    83,    84,    85,    86,    87,     0,    88,    89,     0,
       0,     0,   101,     0,     0,   343,     0,     0,   102,   103,
     104,   105,   106,   107,   144,     0,    83,    84,    85,    86,
      87,     0,    88,    89,     0,     0,     0,   101,   349,     0,
       0,     0,     0,   102,   103,   104,   105,   106,   107,   144,
       0,    83,    84,    85,    86,    87,     0,    88,    89,   168,
       0,     0,     0,     0,     0,   102,   103,   104,   105,   106,
     107,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   170,     0,     0,     0,     0,     0,
     102,   103,   104,   105,   106,   107,     1,     2,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   101,
       0,     0,     0,     0,     0,   102,   103,   104,   105,   106,
     107,     0,     0,     0,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,     1,     2,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   296,   315,   263,     0,
       0,     0,     0,    23,     0,     0,     0,     0,     0,     0,
       0,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
       2,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    22,     0,     0,     0,     0,     0,     0,
      23,     0,     0,     0,     0,     0,     0,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,     2,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     341,     2,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,     0,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21
};

static const yytype_int16 yycheck[] =
{
       0,    23,    22,    60,    34,    25,    73,   100,   101,   141,
       3,   154,   258,    77,     0,    52,     3,    81,    60,     3,
       3,     0,    44,    78,   186,    90,    26,    27,   264,    85,
      64,    31,   113,    90,    68,    35,    56,   101,    64,    76,
      77,    78,     3,    68,    81,    87,    88,    89,    90,    35,
      20,    71,    32,    68,    85,     3,     3,    95,    68,    23,
      80,    86,   298,     3,   101,     3,    83,   313,    48,    84,
     232,   113,    58,     3,    82,   168,    86,   170,    58,    63,
      44,    64,   175,   176,   177,   150,    70,   154,    52,    75,
      15,    16,    85,   150,   132,    75,    68,   162,    85,   192,
      84,    68,    63,   158,   168,   162,    71,    72,   150,    70,
      82,    81,    76,    77,    78,    63,    63,    81,    65,    86,
     162,   158,    70,    70,   191,    63,   207,   208,   209,   167,
      63,   168,   225,    63,    82,    65,   174,   101,   205,    68,
      68,   141,    10,    11,    12,   131,    68,    63,    63,    65,
      65,    82,    77,    78,    70,    84,    84,    44,    45,   226,
     303,    63,    84,    65,   186,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,   224,    70,   249,    63,   267,    48,    63,   321,
      65,   229,   249,    54,   158,    63,    70,    65,   263,    67,
      63,    75,    76,    63,   168,    64,   263,   249,     3,    68,
     232,    64,   232,    64,   252,    68,    64,    68,    64,   312,
      68,   263,    68,    79,    64,   267,    64,   247,    68,    66,
      68,    68,   214,   215,   216,   217,   303,    13,    14,    84,
     262,    84,    17,    18,   337,    69,     3,    80,     5,     6,
       7,     8,     9,    19,    11,    12,    83,   322,   325,   210,
     211,    64,   262,   212,   213,   322,   218,   219,   298,    66,
     327,   309,   310,   311,   296,    82,   296,    82,    56,    64,
     322,     3,    84,     3,    68,   327,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,   296,     3,    66,    64,
      63,   301,     4,    64,    64,    64,    63,    64,    54,   347,
      64,   349,    69,    70,    71,    72,    73,    74,   356,    66,
      84,   321,     3,     4,     5,     6,     7,     8,     9,   220,
      11,    12,   221,   223,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    83,   222,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,   224,
      51,    52,    53,    71,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    76,    86,     4,    64,   301,    69,    70,
      71,    72,    73,    74,   168,   247,   151,   131,    35,    -1,
      -1,    -1,    -1,    84,    85,    86,     3,     4,     5,     6,
       7,     8,     9,    -1,    11,    12,    -1,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    -1,    -1,    -1,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    -1,    51,    52,    53,    -1,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    86,    -1,    -1,
      -1,    -1,    69,    70,    71,    72,    73,    74,     4,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    85,    86,
       3,    -1,     5,     6,     7,     8,     9,    -1,    11,    12,
      -1,    -1,    -1,    -1,    -1,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    -1,     3,    -1,     5,     6,
       7,     8,     9,    -1,    11,    12,    -1,    -1,    51,    52,
      53,    -1,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    -1,    -1,    -1,    -1,    -1,    69,    70,    71,    72,
      73,    74,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    84,    85,    86,    51,    52,    53,    -1,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    -1,    -1,    -1,
      -1,    -1,    69,    70,    71,    72,    73,    74,     4,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    85,    86,
       3,    -1,     5,     6,     7,     8,     9,    -1,    11,    12,
      -1,    -1,    -1,    -1,    -1,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    -1,    -1,     3,    -1,     5,     6,
       7,     8,     9,    -1,    11,    12,    -1,    63,    64,    65,
      -1,    -1,    -1,     3,    70,     5,     6,     7,     8,     9,
      63,    11,    12,    -1,    -1,    -1,    69,    70,    71,    72,
      73,    74,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    85,    86,    51,    52,    53,    -1,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    -1,    -1,    -1,
      -1,    -1,    69,    70,    71,    72,    73,    74,    -1,     4,
      -1,    -1,    -1,    63,    -1,    -1,    -1,    84,    85,    69,
      70,    71,    72,    73,    74,    -1,    -1,    -1,     4,    -1,
      -1,    -1,    -1,    -1,    -1,    85,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,     3,    -1,     5,     6,     7,     8,
       9,    -1,    11,    12,    -1,    -1,    -1,    -1,    83,    -1,
      85,     3,    -1,     5,     6,     7,     8,     9,    -1,    11,
      12,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    85,
      -1,    -1,    -1,    -1,    -1,     3,    -1,     5,     6,     7,
       8,     9,    -1,    11,    12,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    63,    -1,    -1,    -1,    -1,    -1,
      69,    70,    71,    72,    73,    74,    -1,    -1,    -1,    -1,
      -1,    63,    -1,    -1,    -1,    84,    -1,    69,    70,    71,
      72,    73,    74,    -1,    -1,     3,     4,     5,     6,     7,
       8,     9,    84,    11,    12,    63,    -1,    -1,    66,    -1,
      -1,    69,    70,    71,    72,    73,    74,     3,    -1,     5,
       6,     7,     8,     9,    -1,    11,    12,    -1,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,     3,    -1,     5,     6,     7,     8,     9,    -1,
      11,    12,    -1,    -1,    -1,    63,    -1,    -1,    -1,    -1,
      -1,    69,    70,    71,    72,    73,    74,     3,    -1,     5,
       6,     7,     8,     9,    -1,    11,    12,    63,    -1,    -1,
      66,    -1,    -1,    69,    70,    71,    72,    73,    74,     3,
      -1,     5,     6,     7,     8,     9,    -1,    11,    12,    -1,
      -1,    -1,    63,    -1,    -1,    66,    -1,    -1,    69,    70,
      71,    72,    73,    74,     3,    -1,     5,     6,     7,     8,
       9,    -1,    11,    12,    -1,    -1,    -1,    63,    64,    -1,
      -1,    -1,    -1,    69,    70,    71,    72,    73,    74,     3,
      -1,     5,     6,     7,     8,     9,    -1,    11,    12,    63,
      -1,    -1,    -1,    -1,    -1,    69,    70,    71,    72,    73,
      74,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    63,    -1,    -1,    -1,    -1,    -1,
      69,    70,    71,    72,    73,    74,     3,     4,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    63,
      -1,    -1,    -1,    -1,    -1,    69,    70,    71,    72,    73,
      74,    -1,    -1,    -1,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,     3,     4,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    63,    64,    65,    -1,
      -1,    -1,    -1,    70,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
       4,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    63,    -1,    -1,    -1,    -1,    -1,    -1,
      70,    -1,    -1,    -1,    -1,    -1,    -1,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,     4,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      64,     4,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    -1,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    63,    70,   109,   110,   113,   114,   115,   116,
     122,   125,   126,   127,   131,   153,   154,   155,   156,     3,
      85,   126,   125,   131,   132,    84,   111,   112,   126,   110,
     110,     3,    85,   110,    85,   109,   110,   144,   145,    63,
      65,   127,   154,     0,    85,     3,   123,   124,    64,   125,
     131,    68,    84,    83,   144,   145,    85,   114,   117,   118,
     119,   125,     3,     5,     6,     7,     8,     9,    11,    12,
      51,    52,    53,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    69,    70,    71,    72,    73,    74,    84,    86,
      88,    89,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   107,   142,   143,
     144,   145,   146,   147,   148,   149,   152,   126,   109,   144,
       3,   128,   130,   136,     3,    66,    91,   104,   108,   123,
      83,    68,    86,   112,    85,   105,   140,   144,   117,   119,
      86,   118,    82,   120,   121,   126,   119,    82,    63,    91,
      63,    91,    91,   108,    82,    63,    63,    63,   142,    63,
       3,    84,    84,    84,   107,   107,   119,   137,    10,    11,
      12,    63,    65,    67,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    83,   106,    93,    70,    75,    76,
      71,    72,    13,    14,    15,    16,    77,    78,    17,    18,
      69,    79,    80,    19,    20,    81,    68,    84,    86,   146,
      86,   142,   110,   133,   134,   135,    64,    64,    68,    66,
      86,   108,   124,   140,   141,    86,   108,    68,    84,    82,
     142,   137,    82,   142,   107,   107,   107,    56,   150,    84,
      84,    64,    63,    65,   131,   138,   139,    64,     3,    64,
      90,   105,   107,     3,   105,    93,    93,    93,    94,    94,
      95,    95,    96,    96,    96,    96,    97,    97,    98,    99,
     100,   101,   102,   107,   105,    86,    63,   126,   131,   138,
     129,    68,     3,    68,    86,   121,   108,    64,   142,    64,
      64,    64,    63,   147,   151,    64,   133,   138,    66,   108,
     139,    63,    65,    93,    64,    68,    66,    82,    64,    50,
     135,    86,   140,   142,   142,   142,   107,   147,    64,    64,
      66,    64,   133,    66,   108,   105,   104,    54,    64,    64,
     107,    64,    66,   142,    84,   142,    64,   142
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
		  Type, Value, Location); \
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
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
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
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
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
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule); \
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);

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

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc;

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
       `yyls': related to locations.

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

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[2];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yyls = yylsa;
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
  yylsp = yyls;

#if YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 1;
#endif

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
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);

	yyls = yyls1;
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
	YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

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
  *++yylsp = yylloc;
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

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 83 "grammar.y"
    { (yyval.geral2).num = -1; (yyval.geral2).token = IDENTIFIER;
  	    entry_t* entrada = lookup(table,(yyvsp[(1) - (1)].geral).lexema);
 	    if (entrada == NULL)
  	    { 
		entrada = lookup(global,(yyvsp[(1) - (1)].geral).lexema);
		if (entrada == NULL)
		{
			/* situação em que a variável ou função está definida em algum typedef */
			(yyval.geral2).entrada = NULL;
		}
  	    } 
  	    (yyval.geral2).entrada = entrada;

	    if ((yyval.geral2).entrada != NULL)
	    {	
 	    	// tratamento de escopos se limita APENAS para arrays QUANDO presentes em escopos de FOR
  	    	if (!empty(linhasFor) && !empty(colunasFor) && ((yyval.geral2).entrada->isArray || (yyval.geral2).entrada->numPonteiros != 0 )
			&& !scope_already_has_variable((yyval.geral2).entrada,*(int*)top(linhasFor),*(int*)top(colunasFor),top(nomesArq)))
		{
  		 	new_access_element((yyval.geral2).entrada,*(int*)top(linhasFor),*(int*)top(colunasFor),top(nomesArq));
		}
	    }
	 ;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 107 "grammar.y"
    { (yyval.geral2).num = -1; (yyval.geral2).token = CONSTANT; (yyval.geral2).entrada = NULL; ;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 108 "grammar.y"
    { (yyval.geral2).num = atoi((yyvsp[(1) - (1)].geral).lexema); (yyval.geral2).token = INT; (yyval.geral2).entrada = NULL; ;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 109 "grammar.y"
    { (yyval.geral2).num = -1; (yyval.geral2).token = FLOAT; (yyval.geral2).entrada = NULL; ;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 110 "grammar.y"
    { (yyval.geral2).num = -1; (yyval.geral2).entrada = NULL; ;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 111 "grammar.y"
    { (yyval.geral2) = (yyvsp[(2) - (3)].geral2); ;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 115 "grammar.y"
    { (yyval.geral2) = (yyvsp[(1) - (1)].geral2); ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 116 "grammar.y"
    { (yyval.geral2) = (yyvsp[(1) - (4)].geral2); ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 117 "grammar.y"
    { (yyval.geral2) = (yyvsp[(1) - (3)].geral2); ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 118 "grammar.y"
    { (yyval.geral2) = (yyvsp[(1) - (4)].geral2); ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 119 "grammar.y"
    { (yyval.geral2) = (yyvsp[(1) - (3)].geral2); ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 120 "grammar.y"
    { (yyval.geral2) = (yyvsp[(1) - (3)].geral2); ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 123 "grammar.y"
    { if ((yyvsp[(1) - (2)].geral2).entrada != NULL && !empty(linhasFor) && !empty(colunasFor))
		 update_element((yyvsp[(1) - (2)].geral2).entrada,WRITE,*(int*)top(linhasFor),*(int*)top(colunasFor),top(nomesArq));
	   (yyval.geral2) = (yyvsp[(1) - (2)].geral2); 
	  ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 130 "grammar.y"
    { if ((yyvsp[(1) - (2)].geral2).entrada != NULL && !empty(linhasFor) && !empty(colunasFor)) 
		update_element((yyvsp[(1) - (2)].geral2).entrada,WRITE,*(int*)top(linhasFor),*(int*)top(colunasFor),top(nomesArq)); 
	    (yyval.geral2) = (yyvsp[(1) - (2)].geral2); 
	  ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 143 "grammar.y"
    { (yyval.geral2) = (yyvsp[(1) - (1)].geral2); ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 146 "grammar.y"
    { if ((yyvsp[(2) - (2)].geral2).entrada != NULL && !empty(linhasFor) && !empty(colunasFor)) 
		  update_element((yyvsp[(2) - (2)].geral2).entrada,WRITE,*(int*)top(linhasFor),*(int*)top(colunasFor),top(nomesArq)); 
	    (yyval.geral2) = (yyvsp[(2) - (2)].geral2); 
	  ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 153 "grammar.y"
    { if ((yyvsp[(2) - (2)].geral2).entrada != NULL && !empty(linhasFor) && !empty(colunasFor)) 
		  update_element((yyvsp[(2) - (2)].geral2).entrada,WRITE,*(int*)top(linhasFor),*(int*)top(colunasFor),top(nomesArq));
	    (yyval.geral2) = (yyvsp[(2) - (2)].geral2);
	  ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 158 "grammar.y"
    { (yyval.geral2) = (yyvsp[(2) - (2)].geral2); ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 159 "grammar.y"
    { (yyval.geral2) = (yyvsp[(2) - (2)].geral2); ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 160 "grammar.y"
    { (yyval.geral2).num = -1; (yyval.geral2).entrada = NULL; (yyval.geral2).token = NADA; ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 173 "grammar.y"
    { (yyval.geral2) = (yyvsp[(1) - (1)].geral2); ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 174 "grammar.y"
    { (yyval.geral2) = (yyvsp[(4) - (4)].geral2); ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 180 "grammar.y"
    { if ((yyvsp[(1) - (1)].geral2).entrada != NULL && !empty(linhasFor) && !empty(colunasFor)) 
		update_element((yyvsp[(1) - (1)].geral2).entrada,READ,*(int*)top(linhasFor),*(int*)top(colunasFor),top(nomesArq));
	    (yyval.geral2) = (yyvsp[(1) - (1)].geral2);
	  ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 187 "grammar.y"
    { if ((yyvsp[(1) - (3)].geral2).token == INT && (yyvsp[(3) - (3)].geral2).token == INT)
	      { (yyval.geral2).num = (yyvsp[(1) - (3)].geral2).num * (yyvsp[(3) - (3)].geral2).num; (yyval.geral2).token = INT; } 
	    else 
	      { (yyval.geral2).token = -1; (yyval.geral2).num = -1; }
	      
	   if ((yyvsp[(3) - (3)].geral2).entrada != NULL && !empty(linhasFor) && !empty(colunasFor)) 
		update_element((yyvsp[(3) - (3)].geral2).entrada,READ,*(int*)top(linhasFor),*(int*)top(colunasFor),top(nomesArq));
		
	    (yyval.geral2).entrada = NULL;
	  ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 200 "grammar.y"
    { if ((yyvsp[(1) - (3)].geral2).token == INT && (yyvsp[(3) - (3)].geral2).token == INT) 
	    { (yyval.geral2).num = (yyvsp[(1) - (3)].geral2).num / (yyvsp[(3) - (3)].geral2).num; (yyval.geral2).token = INT; }
	 else 
	    { (yyval.geral2).token = -1; (yyval.geral2).num = -1; } 
	    	      
	 if ((yyvsp[(3) - (3)].geral2).entrada != NULL && !empty(linhasFor) && !empty(colunasFor)) 
	     update_element((yyvsp[(3) - (3)].geral2).entrada,READ,*(int*)top(linhasFor),*(int*)top(colunasFor),top(nomesArq));
		
         (yyval.geral2).entrada = NULL; 
        ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 213 "grammar.y"
    { if ((yyvsp[(1) - (3)].geral2).token == INT && (yyvsp[(3) - (3)].geral2).token == INT)
	    { (yyval.geral2).num = (yyvsp[(1) - (3)].geral2).num % (yyvsp[(3) - (3)].geral2).num; (yyval.geral2).token = INT; } 
	  else { (yyval.geral2).token = -1; (yyval.geral2).num = -1; } 
	  	      
	  if ((yyvsp[(3) - (3)].geral2).entrada != NULL && !empty(linhasFor) && !empty(colunasFor)) 
	  	update_element((yyvsp[(3) - (3)].geral2).entrada,READ,*(int*)top(linhasFor),*(int*)top(colunasFor),top(nomesArq));
		
	  (yyval.geral2).entrada = NULL; 
	;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 226 "grammar.y"
    { (yyval.geral2) = (yyvsp[(1) - (1)].geral2); ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 229 "grammar.y"
    { if ((yyvsp[(1) - (3)].geral2).token == INT && (yyvsp[(3) - (3)].geral2).token == INT)
	     { (yyval.geral2).num = (yyvsp[(1) - (3)].geral2).num + (yyvsp[(3) - (3)].geral2).num; (yyval.geral2).token = INT; }
	   else { (yyval.geral2).token = -1; (yyval.geral2).num = -1; } 
	   (yyval.geral2).entrada = NULL; 
	 ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 237 "grammar.y"
    { if ((yyvsp[(1) - (3)].geral2).token == INT && (yyvsp[(3) - (3)].geral2).token == INT)
	      { (yyval.geral2).num = (yyvsp[(1) - (3)].geral2).num - (yyvsp[(3) - (3)].geral2).num; (yyval.geral2).token = INT; } 
	    else { (yyval.geral2).token = -1; (yyval.geral2).num = -1; } 
	    (yyval.geral2).entrada = NULL; 
	  ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 246 "grammar.y"
    { (yyval.geral2) = (yyvsp[(1) - (1)].geral2); ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 247 "grammar.y"
    { (yyval.geral2).num = -1; (yyval.geral2).entrada = NULL; ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 248 "grammar.y"
    { (yyval.geral2).num = -1; (yyval.geral2).entrada = NULL; ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 252 "grammar.y"
    { (yyval.geral2) = (yyvsp[(1) - (1)].geral2); ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 253 "grammar.y"
    { (yyval.geral2).num = -1; (yyval.geral2).entrada = NULL; ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 254 "grammar.y"
    { (yyval.geral2).num = -1; (yyval.geral2).entrada = NULL; ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 255 "grammar.y"
    { (yyval.geral2).num = -1; (yyval.geral2).entrada = NULL; ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 256 "grammar.y"
    { (yyval.geral2).num = -1; (yyval.geral2).entrada = NULL; ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 260 "grammar.y"
    { (yyval.geral2) = (yyvsp[(1) - (1)].geral2); ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 261 "grammar.y"
    { (yyval.geral2).num = -1; (yyval.geral2).entrada = NULL; ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 262 "grammar.y"
    { (yyval.geral2).num = -1; (yyval.geral2).entrada = NULL; ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 266 "grammar.y"
    { (yyval.geral2) = (yyvsp[(1) - (1)].geral2); ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 267 "grammar.y"
    { (yyval.geral2).num = -1; (yyval.geral2).entrada = NULL; ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 271 "grammar.y"
    { (yyval.geral2) = (yyvsp[(1) - (1)].geral2); ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 274 "grammar.y"
    { if ((yyvsp[(1) - (3)].geral2).token == INT && (yyvsp[(3) - (3)].geral2).token == INT)
	   { (yyval.geral2).num = (yyvsp[(1) - (3)].geral2).num ^ (yyvsp[(3) - (3)].geral2).num; (yyval.geral2).token = INT; } 
	  else 
	   { (yyval.geral2).token = -1; (yyval.geral2).num = -1; } 
	  (yyval.geral2).entrada = NULL;
	;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 284 "grammar.y"
    { (yyval.geral2) = (yyvsp[(1) - (1)].geral2); ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 287 "grammar.y"
    { if ((yyvsp[(1) - (3)].geral2).token == INT && (yyvsp[(3) - (3)].geral2).token == INT) 
	      { (yyval.geral2).num = (yyvsp[(1) - (3)].geral2).num | (yyvsp[(3) - (3)].geral2).num; (yyval.geral2).token = INT; }
	    else { (yyval.geral2).token = -1; (yyval.geral2).num = -1; }
	    (yyval.geral2).entrada = NULL; 
	  ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 296 "grammar.y"
    { (yyval.geral2) = (yyvsp[(1) - (1)].geral2); ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 297 "grammar.y"
    { (yyval.geral2).num = -1; (yyval.geral2).entrada = NULL; ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 301 "grammar.y"
    { (yyval.geral2) = (yyvsp[(1) - (1)].geral2); ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 302 "grammar.y"
    { (yyval.geral2).num = -1;  (yyval.geral2).entrada = NULL;;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 306 "grammar.y"
    { (yyval.geral2) = (yyvsp[(1) - (1)].geral2); ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 307 "grammar.y"
    { (yyval.geral2).num = -1; (yyval.geral2).entrada = NULL; ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 311 "grammar.y"
    { (yyval.geral2) = (yyvsp[(1) - (1)].geral2); ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 314 "grammar.y"
    { if (((yyvsp[(1) - (3)].geral2).token == IDENTIFIER) && ((yyvsp[(1) - (3)].geral2).entrada != NULL) && !empty(linhasFor) && !empty(colunasFor)
		&& ((yyvsp[(1) - (3)].geral2).entrada->isArray || (yyvsp[(1) - (3)].geral2).entrada->numPonteiros != 0 ))
	  {
		update_element((yyvsp[(1) - (3)].geral2).entrada,WRITE,*(int*)top(linhasFor),*(int*)top(colunasFor),top(nomesArq));
	  }
  	    (yyval.geral2) = (yyvsp[(3) - (3)].geral2);
	  ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 351 "grammar.y"
    { if ((yyvsp[(2) - (3)].geral).token == LISTA_IDS)
  	    {  
  		int tamanho_tipo = 0;

  		while (!empty(tipos[0]))
  		{
			push(&tipos[1],pop(&tipos[0]));
			tamanho_tipo += strlen((char*)top(tipos[1])) + 1;
  		}
				
  		char* tipo_atual = (char*)malloc(tamanho_tipo);
  		tipo_atual[0] = '\0';
							
  		while (!empty(tipos[1]))
  		{
			strcat(tipo_atual,(char*)pop(&tipos[1]));
			if (!empty(tipos[1])) 
				strcat(tipo_atual," ");
  		}								

  		while (!empty(pilha_ids))
		{ 
			char* nomeID = (char*)pop(&pilha_ids);
			entry_t* entrada = lookup(table,nomeID);
			if (entrada == NULL) 
				entrada = lookup(global,nomeID);

			if (entrada == NULL)
			{
				printf("ERRO - IDENTIFICADOR NÃO ENCONTRADO!\n");
				exit(1);
			}

                	entrada->tipo_str = (void*)tipo_atual;	
			entrada->listaAcessos = NULL;

			if ((yyvsp[(1) - (3)].geral).token == TYPEDEF)
			{
				insert(&typedefs,entrada);			
			}
		} // do while
  	    } // do if											
 	  ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 398 "grammar.y"
    { (yyval.geral).token = NADA; ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 401 "grammar.y"
    { if (((yyvsp[(2) - (2)].geral).token == IDENTIFIER) || ((yyvsp[(2) - (2)].geral).token == STRUCT))
	     (yyval.geral).token = NADA; 
	  else 
	     (yyval.geral).token = (yyvsp[(1) - (2)].geral).token;
	;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 407 "grammar.y"
    { (yyval.geral).token = (yyvsp[(1) - (1)].geral).token; ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 408 "grammar.y"
    { (yyval.geral).token = (yyvsp[(1) - (2)].geral).token; ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 409 "grammar.y"
    { (yyval.geral).token = (yyvsp[(1) - (1)].geral).token; ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 410 "grammar.y"
    { (yyval.geral).token = (yyvsp[(1) - (2)].geral).token; ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 416 "grammar.y"
    { if ((yyvsp[(1) - (1)].geral).token == IDENTIFIER)
    	      (yyval.geral).token = LISTA_IDS;  
	  ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 422 "grammar.y"
    { if (((yyvsp[(3) - (3)].geral).token == IDENTIFIER) || ((yyvsp[(1) - (3)].geral).token == LISTA_IDS))
	      (yyval.geral).token = LISTA_IDS; 
	  ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 429 "grammar.y"
    { (yyval.geral) = (yyvsp[(1) - (1)].geral); ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 430 "grammar.y"
    { (yyval.geral) = (yyvsp[(1) - (3)].geral); ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 434 "grammar.y"
    { (yyval.geral).token = TYPEDEF;  ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 435 "grammar.y"
    { (yyval.geral).token = EXTERN; push(&tipos[0],(void*)"extern"); ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 436 "grammar.y"
    { (yyval.geral).token = STATIC; push(&tipos[0],(void*)"static"); ;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 437 "grammar.y"
    { (yyval.geral).token = AUTO; push(&tipos[0],(void*)"auto"); ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 438 "grammar.y"
    { (yyval.geral).token = REGISTER; push(&tipos[0],(void*)"register"); ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 442 "grammar.y"
    { (yyval.geral).token = VOID; push(&tipos[0],(void*)"void"); ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 443 "grammar.y"
    { (yyval.geral).token = CHAR; push(&tipos[0],(void*)"char"); ;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 444 "grammar.y"
    { (yyval.geral).token = SHORT; push(&tipos[0],(void*)"short"); ;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 445 "grammar.y"
    { (yyval.geral).token = INT; push(&tipos[0],(void*)"int"); ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 446 "grammar.y"
    { (yyval.geral).token = LONG; push(&tipos[0],(void*)"long"); ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 447 "grammar.y"
    { (yyval.geral).token = FLOAT; push(&tipos[0],(void*)"float"); ;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 448 "grammar.y"
    { (yyval.geral).token = DOUBLE; push(&tipos[0],(void*)"double"); ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 449 "grammar.y"
    { (yyval.geral).token = SIGNED; push(&tipos[0],(void*)"signed"); ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 450 "grammar.y"
    { (yyval.geral).token = UNSIGNED; push(&tipos[0],(void*)"unsigned"); ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 451 "grammar.y"
    { (yyval.geral).token = (yyvsp[(1) - (1)].geral).token; ;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 452 "grammar.y"
    { (yyval.geral).token = ENUM; ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 453 "grammar.y"
    { (yyval.geral).token = (yyvsp[(1) - (1)].geral).token; push(&tipos[0],(yyvsp[(1) - (1)].geral).lexema); ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 457 "grammar.y"
    { push(&tipos[0],(void*)(yyvsp[(2) - (5)].geral).lexema); (yyval.geral).token = (yyvsp[(1) - (5)].geral).token; ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 458 "grammar.y"
    { (yyval.geral).token = NADA; ;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 459 "grammar.y"
    { push(&tipos[0],(void*)(yyvsp[(2) - (2)].geral).lexema); (yyval.geral).token = (yyvsp[(1) - (2)].geral).token; ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 463 "grammar.y"
    { (yyval.geral).token = STRUCT; push(&tipos[0],(void*)"struct"); ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 464 "grammar.y"
    { (yyval.geral).token = UNION; push(&tipos[0],(void*)"union"); ;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 511 "grammar.y"
    { (yyval.geral).token = CONST; ;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 512 "grammar.y"
    { (yyval.geral).token = VOLATILE; ;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 516 "grammar.y"
    { (yyval.geral) = (yyvsp[(2) - (2)].geral); ;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 517 "grammar.y"
    { (yyval.geral) = (yyvsp[(1) - (1)].geral); ;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 523 "grammar.y"
    { (yyval.geral).token = IDENTIFIER; 
	    push(&pilha_ids,(yyvsp[(1) - (1)].geral).lexema); 
	    entry_t* entrada = (entry_t*)malloc(sizeof(entry_t));
	    entrada->isArray = 0;
	    entrada->name = (yyvsp[(1) - (1)].geral).lexema;
            entrada->dimArray = NULL;
 	    entrada->escopo = (char*)malloc(strlen(nomeFuncao)+1);
	    entrada->numPonteiros = numPonteiros;
	    entrada->arquivo = top(nomesArq);
	    entrada->listaAcessos = NULL;
	    numPonteiros = 0;
	    strcpy(entrada->escopo,nomeFuncao);
	    if (nomeFuncao == NOMEGLOBAL)
	      insert(&global,entrada);
	    else
              insert(&table,entrada);
	    (yyval.geral).entrada = entrada; 
	  ;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 542 "grammar.y"
    { (yyval.geral) = (yyvsp[(2) - (3)].geral); ;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 545 "grammar.y"
    { if (!(yyvsp[(1) - (4)].geral).entrada->isArray)
	   {
     		 (yyvsp[(1) - (4)].geral).entrada->isArray = 1;
      		 (yyvsp[(1) - (4)].geral).entrada->dimArray = (infoArray*)malloc(sizeof(infoArray));
      		 (yyvsp[(1) - (4)].geral).entrada->dimArray->numDim = 1;
      		 (yyvsp[(1) - (4)].geral).entrada->dimArray->dimensoes = malloc(sizeof(lista_dim));
      		 (yyvsp[(1) - (4)].geral).entrada->dimArray->dimensoes->dim = (yyvsp[(3) - (4)].geral2).num;
      		 (yyvsp[(1) - (4)].geral).entrada->dimArray->dimensoes->prox = NULL;
   	   }
   	   else
   	   {
		lista_dim* p = (yyvsp[(1) - (4)].geral).entrada->dimArray->dimensoes;
		while (p->prox != NULL)
			p = p->prox;
		p->prox = malloc(sizeof(lista_dim));
		p->prox->dim = (yyvsp[(3) - (4)].geral2).num;
		p->prox->prox = NULL;
        	(yyvsp[(1) - (4)].geral).entrada->dimArray->numDim++;
   	   }						
   	   (yyval.geral).token = (yyvsp[(1) - (4)].geral).token;
   	   (yyval.geral).entrada = (yyvsp[(1) - (4)].geral).entrada;
        ;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 570 "grammar.y"
    { if (!(yyvsp[(1) - (3)].geral).entrada->isArray)
   	   {
      		(yyvsp[(1) - (3)].geral).entrada->isArray = 1;
     		(yyvsp[(1) - (3)].geral).entrada->dimArray = (infoArray*)malloc(sizeof(infoArray));
      		(yyvsp[(1) - (3)].geral).entrada->dimArray->numDim = 1;
      		(yyvsp[(1) - (3)].geral).entrada->dimArray->dimensoes = malloc(sizeof(lista_dim));
      		(yyvsp[(1) - (3)].geral).entrada->dimArray->dimensoes->dim = INDEFINIDO;
      		(yyvsp[(1) - (3)].geral).entrada->dimArray->dimensoes->prox = NULL;
  	   }
   	   else
   	   {
		lista_dim* p = (yyvsp[(1) - (3)].geral).entrada->dimArray->dimensoes;
		while (p->prox != NULL)
			p = p->prox;
		p->prox = malloc(sizeof(lista_dim));
		p->prox->dim = INDEFINIDO;
		p->prox->prox = NULL;
        	(yyvsp[(1) - (3)].geral).entrada->dimArray->numDim++;
   	   }						
   	   (yyval.geral).token = (yyvsp[(1) - (3)].geral).token; 
   	   (yyval.geral).entrada = (yyvsp[(1) - (3)].geral).entrada; 
	 ;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 595 "grammar.y"
    { if ((yyvsp[(1) - (2)].geral).token == IDENTIFIER) 
  	    { 
     		nomeFuncao = malloc(strlen((yyvsp[(1) - (2)].geral).lexema)+1); 
     		strcpy(nomeFuncao,(yyvsp[(1) - (2)].geral).lexema); 

  		int tamanho_tipo = 0;

  		while (!empty(tipos[0]))
  		{
			push(&tipos[1],pop(&tipos[0]));
			tamanho_tipo += strlen((char*)top(tipos[1])) + 1;
  		}
				
  		char* tipo_atual = (char*)malloc(tamanho_tipo);
  		tipo_atual[0] = '\0';
							
  		while (!empty(tipos[1]))
  		{
			strcat(tipo_atual,(char*)pop(&tipos[1]));
			if (!empty(tipos[1])) 
				strcat(tipo_atual," ");
  		}								

  		while (!empty(pilha_ids))
		{ 
			char* nomeID = (char*)pop(&pilha_ids);
			entry_t* entrada = lookup(table,nomeID);
			if (entrada == NULL) 
				entrada = lookup(global,nomeID);

			if (entrada == NULL)
			{
				printf("ERRO - IDENTIFICADOR NÃO ENCONTRADO!\n");
				exit(1);
			}

                	entrada->tipo_str = (void*)tipo_atual;
	   		entrada->listaAcessos = NULL;	 

			if ((yyvsp[(1) - (2)].geral).token == TYPEDEF)
			{
				insert(&typedefs,entrada);			
			}
		}
  	    }
	  ;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 642 "grammar.y"
    { free_stack(&tipos[0]); ;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 644 "grammar.y"
    { (yyval.geral).token = (yyvsp[(1) - (6)].geral).token; 
	    (yyval.geral).lexema = (yyvsp[(1) - (6)].geral).lexema; 
	    (yyval.geral).entrada = NULL; 
	  ;}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 651 "grammar.y"
    { (yyval.geral).token = (yyvsp[(1) - (4)].geral).token;
	    (yyval.geral).lexema = (yyvsp[(1) - (4)].geral).lexema; 
	    (yyval.geral).entrada = NULL; 
	  ;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 658 "grammar.y"
    { if ((yyvsp[(1) - (2)].geral).token == IDENTIFIER) 
  	    { 
     		nomeFuncao = malloc(strlen((yyvsp[(1) - (2)].geral).lexema)+1); 
     		strcpy(nomeFuncao,(yyvsp[(1) - (2)].geral).lexema); 

  		int tamanho_tipo = 0;

  		while (!empty(tipos[0]))
  		{
			push(&tipos[1],pop(&tipos[0]));
			tamanho_tipo += strlen((char*)top(tipos[1])) + 1;
  		}
				
  		char* tipo_atual = (char*)malloc(tamanho_tipo);
  		tipo_atual[0] = '\0';
							
  		while (!empty(tipos[1]))
  		{
			strcat(tipo_atual,(char*)pop(&tipos[1]));
			if (!empty(tipos[1])) 
				strcat(tipo_atual," ");
  		}								

  		while (!empty(pilha_ids))
		{ 
			char* nomeID = (char*)pop(&pilha_ids);
			entry_t* entrada = lookup(table,nomeID);
			if (entrada == NULL) 
				entrada = lookup(global,nomeID);

			if (entrada == NULL)
			{
				printf("ERRO - IDENTIFICADOR NÃO ENCONTRADO!\n");
				exit(1);
			}

                	entrada->tipo_str = (void*)tipo_atual;
			entrada->listaAcessos = NULL;	 

			if ((yyvsp[(1) - (2)].geral).token == TYPEDEF)
			{
				insert(&typedefs,entrada);			
			}
		}
  	    }
	 ;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 705 "grammar.y"
    { (yyval.geral).token = (yyvsp[(1) - (4)].geral).token; 
	   (yyval.geral).lexema = (yyvsp[(1) - (4)].geral).lexema; 
	   (yyval.geral).entrada = NULL;
	 ;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 713 "grammar.y"
    { numPonteiros++; ;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 714 "grammar.y"
    { numPonteiros++; ;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 715 "grammar.y"
    { numPonteiros++; ;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 716 "grammar.y"
    { numPonteiros++; ;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 737 "grammar.y"
    { if ((yyvsp[(2) - (2)].geral).token == IDENTIFIER)
  	    {  
  		int tamanho_tipo = 0;

  		while (!empty(tipos[0]))
  		{
			push(&tipos[1],pop(&tipos[0]));
			tamanho_tipo += strlen((char*)top(tipos[1])) + 1;
  		}
					
  		char* tipo_atual = (char*)malloc(tamanho_tipo);
  		tipo_atual[0] = '\0';
									
  		while (!empty(tipos[1]))
  		{
			strcat(tipo_atual,(char*)pop(&tipos[1]));
			if (!empty(tipos[1])) 
				strcat(tipo_atual," ");
  		}								

  		while (!empty(pilha_ids))
		{ 
			char* nomeID = (char*)pop(&pilha_ids);
			entry_t* entrada = lookup(table,nomeID);
			if (entrada == NULL) 
				entrada = lookup(global,nomeID);

			if (entrada == NULL)
			{
				printf("ERRO - IDENTIFICADOR NÃO ENCONTRADO!\n");
				exit(1);
			}

               		entrada->tipo_str = (void*)tipo_atual;	
			entrada->listaAcessos = NULL; 

			if ((yyvsp[(1) - (2)].geral).token == TYPEDEF)
				insert(&typedefs,entrada);
		}
  	    } // do if											
 	  ;}
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 793 "grammar.y"
    { numPonteiros--; ;}
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 795 "grammar.y"
    { numPonteiros--; ;}
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 854 "grammar.y"
    { (yyval.geral2).token = NADA; (yyval.geral2).entrada = NULL; (yyval.geral2).num = -1; ;}
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 855 "grammar.y"
    { (yyval.geral2) = (yyvsp[(1) - (2)].geral2); ;}
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 869 "grammar.y"
    { int *i = (int*)malloc(sizeof(int));
  	    *i = (yylsp[(1) - (2)]).first_line;
  	    push(&linhasFor,(void*)i);
  	    i = (int*)malloc(sizeof(int));
            *i = (yylsp[(1) - (2)]).first_column;
            push(&colunasFor,(void*)i);
	  ;}
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 884 "grammar.y"
    { int *i = (int*)pop(&linhasFor);
  	    free(i);
  	    i = pop(&colunasFor);
  	    free(i);
 	  ;}
    break;

  case 206:

/* Line 1455 of yacc.c  */
#line 894 "grammar.y"
    { int *i = (int*)pop(&linhasFor);
  	    free(i);
  	    i = pop(&colunasFor);
  	    free(i);
 	  ;}
    break;

  case 214:

/* Line 1455 of yacc.c  */
#line 916 "grammar.y"
    { print_table(table); free_table(&table); ;}
    break;

  case 216:

/* Line 1455 of yacc.c  */
#line 921 "grammar.y"
    { nomeFuncao = NOMEGLOBAL; ;}
    break;

  case 217:

/* Line 1455 of yacc.c  */
#line 922 "grammar.y"
    { nomeFuncao = NOMEGLOBAL; ;}
    break;

  case 218:

/* Line 1455 of yacc.c  */
#line 923 "grammar.y"
    { nomeFuncao = NOMEGLOBAL; ;}
    break;

  case 219:

/* Line 1455 of yacc.c  */
#line 924 "grammar.y"
    { nomeFuncao = NOMEGLOBAL; ;}
    break;



/* Line 1455 of yacc.c  */
#line 3200 "grammar.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

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

  yyerror_range[0] = yylloc;

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
		      yytoken, &yylval, &yylloc);
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

  yyerror_range[0] = yylsp[1-yylen];
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

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
  *++yylsp = yyloc;

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
		 yytoken, &yylval, &yylloc);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp);
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
#line 932 "grammar.y"

#include <stdio.h>

extern char yytext[];
extern int column;
extern FILE* yyin;
extern int yylineno;

int main(int argc, char* argv[]) 
{  
   init_stack(&pilha_ids);
   init_stack(&tipos[0]);
   init_stack(&tipos[1]);
   init_stack(&nomesArq);
   init_stack(&parenteses);
   init_stack(&linhasFor);
   init_stack(&colunasFor);
   init_table(&table);
   init_table(&arquivos);
   init_table(&typedefs);
   init_table(&defines);
   init_table(&global);

   nomeFuncao = (char*)malloc(100);
   NOMEGLOBAL = (char*)malloc(10);
   NOMEGLOBAL[0] = '\0';
   strcat(NOMEGLOBAL,"global");

   nomeFuncao = NOMEGLOBAL;

   if (argc != 2) 
   {
     printf("uso: %s <input_file>. Try again!\n", argv[0]);
     exit(-1);
   }

   yyin = fopen(argv[1], "r");

   if (!yyin) 
   {
     printf("Uso: %s <input_file>. Could not find %s. Try again!\n", argv[0], argv[1]);
     exit(-1);
   }

   push(&nomesArq,argv[1]);

   while (1) 
   {
	if (!yyparse()) 
	{ }
  	else 
      		printf("ERROR em arquivo %s.\n",(char*)top(nomesArq));

	pop(&nomesArq);

	if (empty(nomesArq))
		break;

	yyrestart(yyin);
	yyin = fopen(top(nomesArq),"r");
	nomeFuncao = NOMEGLOBAL;
	line = 1;	
        column = 1;
   }

   print_table(global);

   return(0);
}

yyerror(s)
char *s;
{
	fflush(stdout);
	printf("\n%*s\n%*s\n", column, "^", column, s);
}


