/*
** 2000-05-29
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** Driver template for the LEMON parser generator.
**
** The "lemon" program processes an LALR(1) input grammar file, then uses
** this template to construct a parser.  The "lemon" program inserts text
** at each "%%" line.  Also, any "P-a-r-s-e" identifer prefix (without the
** interstitial "-" characters) contained in this template is changed into
** the value of the %name directive from the grammar.  Otherwise, the content
** of this template is copied straight through into the generate parser
** source file.
**
** The following is the concatenation of all %include directives from the
** input grammar file:
*/
#include <stdio.h>
/************ Begin %include sections from the grammar ************************/
#line 7 "zidl.y"

#include <vector>
#include <stack>
#include <istream>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <cstdlib>
#include "zidlparser.h"
#line 38 "zidl.c"
/**************** End of %include directives **********************************/
/* These constants specify the various numeric values for terminal symbols
** in a format understandable to "makeheaders".  This section is blank unless
** "lemon" is run with the "-m" command-line option.
***************** Begin makeheaders token definitions *************************/
/**************** End makeheaders token definitions ***************************/

/* The next sections is a series of control #defines.
** various aspects of the generated parser.
**    YYCODETYPE         is the data type used to store the integer codes
**                       that represent terminal and non-terminal symbols.
**                       "unsigned char" is used if there are fewer than
**                       256 symbols.  Larger types otherwise.
**    YYNOCODE           is a number of type YYCODETYPE that is not used for
**                       any terminal or nonterminal symbol.
**    YYFALLBACK         If defined, this indicates that one or more tokens
**                       (also known as: "terminal symbols") have fall-back
**                       values which should be used if the original symbol
**                       would not parse.  This permits keywords to sometimes
**                       be used as identifiers, for example.
**    YYACTIONTYPE       is the data type used for "action codes" - numbers
**                       that indicate what to do in response to the next
**                       token.
**    ParseTOKENTYPE     is the data type used for minor type for terminal
**                       symbols.  Background: A "minor type" is a semantic
**                       value associated with a terminal or non-terminal
**                       symbols.  For example, for an "ID" terminal symbol,
**                       the minor type might be the name of the identifier.
**                       Each non-terminal can have a different minor type.
**                       Terminal symbols all have the same minor type, though.
**                       This macros defines the minor type for terminal 
**                       symbols.
**    YYMINORTYPE        is the data type used for all minor types.
**                       This is typically a union of many types, one of
**                       which is ParseTOKENTYPE.  The entry in the union
**                       for terminal symbols is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.  If
**                       zero the stack is dynamically sized using realloc()
**    ParseARG_SDECL     A static variable declaration for the %extra_argument
**    ParseARG_PDECL     A parameter declaration for the %extra_argument
**    ParseARG_STORE     Code to store %extra_argument into yypParser
**    ParseARG_FETCH     Code to extract %extra_argument from yypParser
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YY_MAX_SHIFT       Maximum value for shift actions
**    YY_MIN_SHIFTREDUCE Minimum value for shift-reduce actions
**    YY_MAX_SHIFTREDUCE Maximum value for shift-reduce actions
**    YY_MIN_REDUCE      Minimum value for reduce actions
**    YY_MAX_REDUCE      Maximum value for reduce actions
**    YY_ERROR_ACTION    The yy_action[] code for syntax error
**    YY_ACCEPT_ACTION   The yy_action[] code for accept
**    YY_NO_ACTION       The yy_action[] code for no-op
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/************* Begin control #defines *****************************************/
#define YYCODETYPE unsigned char
#define YYNOCODE 65
#define YYACTIONTYPE unsigned short int
#define ParseTOKENTYPE  token 
typedef union {
  int yyinit;
  ParseTOKENTYPE yy0;
  node* yy9;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define ParseARG_SDECL  parsecontext* context ;
#define ParseARG_PDECL , parsecontext* context 
#define ParseARG_FETCH  parsecontext* context  = yypParser->context 
#define ParseARG_STORE yypParser->context  = context 
#define YYNSTATE             82
#define YYNRULE              63
#define YY_MAX_SHIFT         81
#define YY_MIN_SHIFTREDUCE   132
#define YY_MAX_SHIFTREDUCE   194
#define YY_MIN_REDUCE        195
#define YY_MAX_REDUCE        257
#define YY_ERROR_ACTION      258
#define YY_ACCEPT_ACTION     259
#define YY_NO_ACTION         260
/************* End control #defines *******************************************/

/* Define the yytestcase() macro to be a no-op if is not already defined
** otherwise.
**
** Applications can choose to define yytestcase() in the %include section
** to a macro that can assist in verifying code coverage.  For production
** code the yytestcase() macro should be turned off.  But it is useful
** for testing.
*/
#ifndef yytestcase
# define yytestcase(X)
#endif


/* Next are the tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.  
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N <= YY_MAX_SHIFT             Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   N between YY_MIN_SHIFTREDUCE       Shift to an arbitrary state then
**     and YY_MAX_SHIFTREDUCE           reduce by rule N-YY_MIN_SHIFTREDUCE.
**
**   N between YY_MIN_REDUCE            Reduce by rule N-YY_MIN_REDUCE
**     and YY_MAX_REDUCE
**
**   N == YY_ERROR_ACTION               A syntax error has occurred.
**
**   N == YY_ACCEPT_ACTION              The parser accepts its input.
**
**   N == YY_NO_ACTION                  No such action.  Denotes unused
**                                      slots in the yy_action[] table.
**
** The action table is constructed as a single large table named yy_action[].
** Given state S and lookahead X, the action is computed as either:
**
**    (A)   N = yy_action[ yy_shift_ofst[S] + X ]
**    (B)   N = yy_default[S]
**
** The (A) formula is preferred.  The B formula is used instead if:
**    (1)  The yy_shift_ofst[S]+X value is out of range, or
**    (2)  yy_lookahead[yy_shift_ofst[S]+X] is not equal to X, or
**    (3)  yy_shift_ofst[S] equal YY_SHIFT_USE_DFLT.
** (Implementation note: YY_SHIFT_USE_DFLT is chosen so that
** YY_SHIFT_USE_DFLT+X will be out of range for all possible lookaheads X.
** Hence only tests (1) and (2) need to be evaluated.)
**
** The formulas above are for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the yy_reduce_ofst[] array is used in place of
** the yy_shift_ofst[] array and YY_REDUCE_USE_DFLT is used in place of
** YY_SHIFT_USE_DFLT.
**
** The following are the tables generated in this section:
**
**  yy_action[]        A single table containing all actions.
**  yy_lookahead[]     A table containing the lookahead for each entry in
**                     yy_action.  Used to detect hash collisions.
**  yy_shift_ofst[]    For each state, the offset into yy_action for
**                     shifting terminals.
**  yy_reduce_ofst[]   For each state, the offset into yy_action for
**                     shifting non-terminals after a reduce.
**  yy_default[]       Default action for each state.
**
*********** Begin parsing tables **********************************************/
#define YY_ACTTAB_COUNT (158)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */   181,   51,  195,   81,  139,  161,   79,   77,   75,  138,
 /*    10 */    71,   30,   60,   53,  196,    5,   25,  213,   50,   10,
 /*    20 */   259,    8,  178,  179,  180,  182,  183,  184,  185,  186,
 /*    30 */   187,  188,   41,   53,  229,    5,   27,  213,  222,  220,
 /*    40 */   155,  223,  144,   53,  143,    5,   54,  212,  239,   50,
 /*    50 */    46,   16,   37,  214,  214,   38,    8,    5,   57,  203,
 /*    60 */   208,   43,   58,    5,    5,  174,  192,  164,   24,    4,
 /*    70 */    64,   66,    7,   66,   56,  147,  175,   26,    4,   63,
 /*    80 */     9,   69,   62,  252,  168,   40,   52,  215,  194,   28,
 /*    90 */    11,    3,   18,    6,   14,   15,  165,   31,   33,   32,
 /*   100 */    34,  163,   35,   36,   39,  162,    1,  170,   42,   19,
 /*   110 */    44,   45,   47,   49,   20,  148,   48,  154,   17,  146,
 /*   120 */   197,  197,   61,  173,   59,  197,  191,   21,    2,  197,
 /*   130 */   197,   65,   67,  197,   22,  142,   68,   12,  193,   70,
 /*   140 */    74,   55,   13,   72,   29,   23,   73,  137,   76,  197,
 /*   150 */   136,   78,  197,  197,  135,  197,  197,   80,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     7,    2,    0,    1,    2,   10,    4,    5,    6,   10,
 /*    10 */    11,   12,   13,   51,   42,   53,   54,   55,   19,   43,
 /*    20 */    44,   26,   29,   30,   31,   32,   33,   34,   35,   36,
 /*    30 */    37,   38,    2,   51,   50,   53,   54,   55,   47,   48,
 /*    40 */    10,   50,   10,   51,   10,   53,   60,   55,   52,   19,
 /*    50 */    20,   21,   22,   15,   16,   51,   26,   53,   62,   46,
 /*    60 */    47,   51,   51,   53,   53,    7,    3,    3,   15,   16,
 /*    70 */    18,   39,    8,   39,   60,    3,   18,   15,   16,   16,
 /*    80 */     8,    7,    8,   58,   25,   61,   17,   56,   18,   63,
 /*    90 */    57,   49,   40,   45,   59,   59,    3,    7,    7,   24,
 /*   100 */    23,    3,    8,    7,    7,    3,   14,    3,    7,    9,
 /*   110 */     3,    8,    3,    7,    9,    3,    8,   18,    7,    3,
 /*   120 */    64,   64,    3,   28,    7,   64,    3,    9,   14,   64,
 /*   130 */    64,    7,    3,   64,    9,    3,    8,   27,   18,    2,
 /*   140 */     7,   28,   27,    3,   17,    9,    8,    3,    2,   64,
 /*   150 */     3,    2,   64,   64,    3,   64,   64,    2,
};
#define YY_SHIFT_USE_DFLT (158)
#define YY_SHIFT_COUNT    (81)
#define YY_SHIFT_MIN      (-7)
#define YY_SHIFT_MAX      (155)
static const short yy_shift_ofst[] = {
 /*     0 */   158,   38,   38,   30,  158,   -7,   -1,  158,  158,  158,
 /*    10 */     2,   -5,   58,   58,   32,   34,   59,   69,   70,  158,
 /*    20 */   158,  158,  158,  158,   64,   53,   72,   62,   63,   52,
 /*    30 */    74,   93,   90,   75,   91,   77,   94,   96,   98,   92,
 /*    40 */    97,  102,  104,  101,  100,  107,  103,  105,  109,  108,
 /*    50 */   106,  112,   99,  111,   95,  110,  113,  115,  116,  114,
 /*    60 */   117,  118,  119,  120,  123,  127,  124,  125,  129,  128,
 /*    70 */   132,  137,  136,  140,  138,  133,  144,  146,  147,  149,
 /*    80 */   151,  155,
};
#define YY_REDUCE_USE_DFLT (-39)
#define YY_REDUCE_COUNT (23)
#define YY_REDUCE_MIN   (-38)
#define YY_REDUCE_MAX   (48)
static const signed char yy_reduce_ofst[] = {
 /*     0 */   -24,  -38,  -18,   -9,   -8,   -4,   13,    4,   10,   11,
 /*    10 */   -28,  -16,  -14,   14,   25,   25,   24,   31,   26,   33,
 /*    20 */    42,   35,   36,   48,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   197,  240,  240,  258,  240,  258,  258,  240,  240,  240,
 /*    10 */   258,  258,  258,  258,  258,  258,  232,  216,  258,  230,
 /*    20 */   221,  253,  253,  204,  258,  258,  258,  258,  258,  258,
 /*    30 */   258,  258,  258,  258,  258,  258,  258,  258,  258,  258,
 /*    40 */   258,  258,  258,  258,  258,  258,  258,  219,  258,  258,
 /*    50 */   258,  258,  258,  258,  258,  235,  258,  234,  258,  258,
 /*    60 */   258,  258,  258,  258,  258,  258,  258,  258,  258,  258,
 /*    70 */   258,  258,  258,  258,  258,  258,  258,  258,  258,  258,
 /*    80 */   258,  258,
};
/********** End of lemon-generated parsing tables *****************************/

/* The next table maps tokens (terminal symbols) into fallback tokens.  
** If a construct like the following:
** 
**      %fallback ID X Y Z.
**
** appears in the grammar, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
**
** This feature can be used, for example, to cause some keywords in a language
** to revert to identifiers if they keyword does not apply in the context where
** it appears.
*/
#ifdef YYFALLBACK
static const YYCODETYPE yyFallback[] = {
};
#endif /* YYFALLBACK */

/* The following structure represents a single element of the
** parser's stack.  Information stored includes:
**
**   +  The state number for the parser at this level of the stack.
**
**   +  The value of the token stored at this level of the stack.
**      (In other words, the "major" token.)
**
**   +  The semantic value stored at this level of the stack.  This is
**      the information used by the action routines in the grammar.
**      It is sometimes called the "minor" token.
**
** After the "shift" half of a SHIFTREDUCE action, the stateno field
** actually contains the reduce action for the second half of the
** SHIFTREDUCE.
*/
struct yyStackEntry {
  YYACTIONTYPE stateno;  /* The state-number, or reduce action in SHIFTREDUCE */
  YYCODETYPE major;      /* The major token value.  This is the code
                         ** number for the token at this stack level */
  YYMINORTYPE minor;     /* The user-supplied minor token value.  This
                         ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser {
  yyStackEntry *yytos;          /* Pointer to top element of the stack */
#ifdef YYTRACKMAXSTACKDEPTH
  int yyhwm;                    /* High-water mark of the stack */
#endif
#ifndef YYNOERRORRECOVERY
  int yyerrcnt;                 /* Shifts left before out of the error */
#endif
  ParseARG_SDECL                /* A place to hold %extra_argument */
#if YYSTACKDEPTH<=0
  int yystksz;                  /* Current side of the stack */
  yyStackEntry *yystack;        /* The parser's stack */
  yyStackEntry yystk0;          /* First stack entry */
#else
  yyStackEntry yystack[YYSTACKDEPTH];  /* The parser's stack */
  yyStackEntry *yystackEnd;            /* Last entry in the stack */
#endif
};
typedef struct yyParser yyParser;

#ifndef NDEBUG
#include <stdio.h>
static FILE *yyTraceFILE = 0;
static char *yyTracePrompt = 0;
#endif /* NDEBUG */

#ifndef NDEBUG
/* 
** Turn parser tracing on by giving a stream to which to write the trace
** and a prompt to preface each trace message.  Tracing is turned off
** by making either argument NULL 
**
** Inputs:
** <ul>
** <li> A FILE* to which trace output should be written.
**      If NULL, then tracing is turned off.
** <li> A prefix string written at the beginning of every
**      line of trace output.  If NULL, then tracing is
**      turned off.
** </ul>
**
** Outputs:
** None.
*/
void ParseTrace(FILE *TraceFILE, char *zTracePrompt){
  yyTraceFILE = TraceFILE;
  yyTracePrompt = zTracePrompt;
  if( yyTraceFILE==0 ) yyTracePrompt = 0;
  else if( yyTracePrompt==0 ) yyTraceFILE = 0;
}
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *const yyTokenName[] = { 
  "$",             "INCLUDE",       "DQSTR",         "LINEBREAK",   
  "CHEADER",       "IMPORT",        "NAMESPACE",     "IDENTIFIER",  
  "COLON",         "INDENT",        "DEDENT",        "DLNAME",      
  "ENUM",          "PDEF",          "OPENPAREN",     "CLOSEPAREN",  
  "COMMA",         "EQ",            "DIGITS",        "CLASS",       
  "UNION",         "DEF",           "ITERATOR",      "FOR",         
  "IN",            "STATIC",        "MEMBER",        "OPENSQRPAREN",
  "CLOSESQRPAREN",  "OUT",           "NOREF",         "NO_PYTHON",   
  "INT",           "UINT",          "FLOAT",         "STRING",      
  "CHAR",          "PVOID",         "BOOL",          "SET",         
  "BIT",           "error",         "programbody",   "programlist", 
  "program",       "namespacelist",  "namespacebody",  "classdef",    
  "classbody",     "classlist",     "memberbody",    "membertype",  
  "typemodifier",  "typemodifierlist",  "argumentlist",  "argumentbody",
  "argumentdefault",  "unionlist",     "enumbody",      "enumlist",    
  "arrayindexer",  "methodmodifier",  "typename",      "bitvalues",   
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] = {
 /*   0 */ "program ::= programlist",
 /*   1 */ "programlist ::= programlist programbody",
 /*   2 */ "programlist ::=",
 /*   3 */ "programbody ::= INCLUDE DQSTR LINEBREAK",
 /*   4 */ "programbody ::= CHEADER DQSTR LINEBREAK",
 /*   5 */ "programbody ::= IMPORT DQSTR LINEBREAK",
 /*   6 */ "programbody ::= NAMESPACE IDENTIFIER COLON LINEBREAK INDENT namespacelist DEDENT",
 /*   7 */ "programbody ::= DQSTR",
 /*   8 */ "namespacelist ::= namespacelist namespacebody",
 /*   9 */ "namespacelist ::=",
 /*  10 */ "namespacebody ::= DLNAME DQSTR LINEBREAK",
 /*  11 */ "namespacebody ::= ENUM IDENTIFIER COLON LINEBREAK INDENT enumlist DEDENT",
 /*  12 */ "namespacebody ::= ENUM COLON LINEBREAK INDENT enumlist DEDENT",
 /*  13 */ "namespacebody ::= classdef",
 /*  14 */ "namespacebody ::= PDEF IDENTIFIER OPENPAREN argumentlist CLOSEPAREN COLON membertype LINEBREAK",
 /*  15 */ "namespacebody ::= PDEF IDENTIFIER OPENPAREN argumentlist CLOSEPAREN LINEBREAK",
 /*  16 */ "namespacebody ::= DQSTR LINEBREAK",
 /*  17 */ "argumentlist ::= argumentlist COMMA argumentbody",
 /*  18 */ "argumentlist ::= argumentbody",
 /*  19 */ "argumentlist ::=",
 /*  20 */ "argumentbody ::= membertype IDENTIFIER argumentdefault",
 /*  21 */ "argumentdefault ::=",
 /*  22 */ "argumentdefault ::= EQ DIGITS",
 /*  23 */ "classdef ::= CLASS IDENTIFIER COLON LINEBREAK INDENT classlist DEDENT",
 /*  24 */ "classdef ::= CLASS IDENTIFIER COLON LINEBREAK",
 /*  25 */ "classlist ::= classlist classbody",
 /*  26 */ "classlist ::=",
 /*  27 */ "classbody ::= classdef",
 /*  28 */ "classbody ::= memberbody",
 /*  29 */ "classbody ::= UNION COLON LINEBREAK INDENT unionlist DEDENT",
 /*  30 */ "classbody ::= DQSTR LINEBREAK",
 /*  31 */ "classbody ::= DEF methodmodifier IDENTIFIER OPENPAREN argumentlist CLOSEPAREN COLON membertype LINEBREAK",
 /*  32 */ "classbody ::= DEF methodmodifier IDENTIFIER OPENPAREN argumentlist CLOSEPAREN LINEBREAK",
 /*  33 */ "classbody ::= ITERATOR IDENTIFIER COLON FOR IDENTIFIER IN IDENTIFIER LINEBREAK",
 /*  34 */ "unionlist ::= unionlist memberbody",
 /*  35 */ "unionlist ::=",
 /*  36 */ "methodmodifier ::= STATIC",
 /*  37 */ "methodmodifier ::=",
 /*  38 */ "memberbody ::= MEMBER membertype IDENTIFIER LINEBREAK",
 /*  39 */ "membertype ::= typemodifierlist typename",
 /*  40 */ "membertype ::= typemodifierlist typename OPENSQRPAREN arrayindexer CLOSESQRPAREN",
 /*  41 */ "membertype ::= typemodifierlist typename OPENSQRPAREN arrayindexer CLOSESQRPAREN OPENSQRPAREN arrayindexer CLOSESQRPAREN",
 /*  42 */ "arrayindexer ::= IDENTIFIER",
 /*  43 */ "arrayindexer ::= DIGITS",
 /*  44 */ "typemodifierlist ::= typemodifierlist typemodifier",
 /*  45 */ "typemodifierlist ::=",
 /*  46 */ "typemodifier ::= OUT",
 /*  47 */ "typemodifier ::= NOREF",
 /*  48 */ "typemodifier ::= NO_PYTHON",
 /*  49 */ "typename ::= IDENTIFIER",
 /*  50 */ "typename ::= INT",
 /*  51 */ "typename ::= UINT",
 /*  52 */ "typename ::= FLOAT",
 /*  53 */ "typename ::= STRING",
 /*  54 */ "typename ::= CHAR",
 /*  55 */ "typename ::= PVOID",
 /*  56 */ "typename ::= BOOL",
 /*  57 */ "enumlist ::= enumlist enumbody",
 /*  58 */ "enumlist ::=",
 /*  59 */ "enumbody ::= SET IDENTIFIER EQ DIGITS LINEBREAK",
 /*  60 */ "enumbody ::= SET IDENTIFIER EQ BIT bitvalues LINEBREAK",
 /*  61 */ "bitvalues ::= bitvalues COMMA DIGITS",
 /*  62 */ "bitvalues ::= DIGITS",
};
#endif /* NDEBUG */


#if YYSTACKDEPTH<=0
/*
** Try to increase the size of the parser stack.  Return the number
** of errors.  Return 0 on success.
*/
static int yyGrowStack(yyParser *p){
  int newSize;
  int idx;
  yyStackEntry *pNew;

  newSize = p->yystksz*2 + 100;
  idx = p->yytos ? (int)(p->yytos - p->yystack) : 0;
  if( p->yystack==&p->yystk0 ){
    pNew = malloc(newSize*sizeof(pNew[0]));
    if( pNew ) pNew[0] = p->yystk0;
  }else{
    pNew = realloc(p->yystack, newSize*sizeof(pNew[0]));
  }
  if( pNew ){
    p->yystack = pNew;
    p->yytos = &p->yystack[idx];
#ifndef NDEBUG
    if( yyTraceFILE ){
      fprintf(yyTraceFILE,"%sStack grows from %d to %d entries.\n",
              yyTracePrompt, p->yystksz, newSize);
    }
#endif
    p->yystksz = newSize;
  }
  return pNew==0; 
}
#endif

/* Datatype of the argument to the memory allocated passed as the
** second argument to ParseAlloc() below.  This can be changed by
** putting an appropriate #define in the %include section of the input
** grammar.
*/
#ifndef YYMALLOCARGTYPE
# define YYMALLOCARGTYPE size_t
#endif

/* Initialize a new parser that has already been allocated.
*/
void ParseInit(void *yypParser){
  yyParser *pParser = (yyParser*)yypParser;
#ifdef YYTRACKMAXSTACKDEPTH
  pParser->yyhwm = 0;
#endif
#if YYSTACKDEPTH<=0
  pParser->yytos = NULL;
  pParser->yystack = NULL;
  pParser->yystksz = 0;
  if( yyGrowStack(pParser) ){
    pParser->yystack = &pParser->yystk0;
    pParser->yystksz = 1;
  }
#endif
#ifndef YYNOERRORRECOVERY
  pParser->yyerrcnt = -1;
#endif
  pParser->yytos = pParser->yystack;
  pParser->yystack[0].stateno = 0;
  pParser->yystack[0].major = 0;
#if YYSTACKDEPTH>0
  pParser->yystackEnd = &pParser->yystack[YYSTACKDEPTH-1];
#endif
}

#ifndef Parse_ENGINEALWAYSONSTACK
/* 
** This function allocates a new parser.
** The only argument is a pointer to a function which works like
** malloc.
**
** Inputs:
** A pointer to the function used to allocate memory.
**
** Outputs:
** A pointer to a parser.  This pointer is used in subsequent calls
** to Parse and ParseFree.
*/
void *ParseAlloc(void *(*mallocProc)(YYMALLOCARGTYPE)){
  yyParser *pParser;
  pParser = (yyParser*)(*mallocProc)( (YYMALLOCARGTYPE)sizeof(yyParser) );
  if( pParser ) ParseInit(pParser);
  return pParser;
}
#endif /* Parse_ENGINEALWAYSONSTACK */


/* The following function deletes the "minor type" or semantic value
** associated with a symbol.  The symbol can be either a terminal
** or nonterminal. "yymajor" is the symbol code, and "yypminor" is
** a pointer to the value to be deleted.  The code used to do the 
** deletions is derived from the %destructor and/or %token_destructor
** directives of the input grammar.
*/
static void yy_destructor(
  yyParser *yypParser,    /* The parser */
  YYCODETYPE yymajor,     /* Type code for object to destroy */
  YYMINORTYPE *yypminor   /* The object to be destroyed */
){
  ParseARG_FETCH;
  switch( yymajor ){
    /* Here is inserted the actions which take place when a
    ** terminal or non-terminal is destroyed.  This can happen
    ** when the symbol is popped from the stack during a
    ** reduce or during error processing or when a parser is 
    ** being destroyed before it is finished parsing.
    **
    ** Note: during a reduce, the only symbols destroyed are those
    ** which appear on the RHS of the rule, but which are *not* used
    ** inside the C code.
    */
/********* Begin destructor definitions ***************************************/
/********* End destructor definitions *****************************************/
    default:  break;   /* If no destructor action specified: do nothing */
  }
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
*/
static void yy_pop_parser_stack(yyParser *pParser){
  yyStackEntry *yytos;
  assert( pParser->yytos!=0 );
  assert( pParser->yytos > pParser->yystack );
  yytos = pParser->yytos--;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sPopping %s\n",
      yyTracePrompt,
      yyTokenName[yytos->major]);
  }
#endif
  yy_destructor(pParser, yytos->major, &yytos->minor);
}

/*
** Clear all secondary memory allocations from the parser
*/
void ParseFinalize(void *p){
  yyParser *pParser = (yyParser*)p;
  while( pParser->yytos>pParser->yystack ) yy_pop_parser_stack(pParser);
#if YYSTACKDEPTH<=0
  if( pParser->yystack!=&pParser->yystk0 ) free(pParser->yystack);
#endif
}

#ifndef Parse_ENGINEALWAYSONSTACK
/* 
** Deallocate and destroy a parser.  Destructors are called for
** all stack elements before shutting the parser down.
**
** If the YYPARSEFREENEVERNULL macro exists (for example because it
** is defined in a %include section of the input grammar) then it is
** assumed that the input pointer is never NULL.
*/
void ParseFree(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
#ifndef YYPARSEFREENEVERNULL
  if( p==0 ) return;
#endif
  ParseFinalize(p);
  (*freeProc)(p);
}
#endif /* Parse_ENGINEALWAYSONSTACK */

/*
** Return the peak depth of the stack for a parser.
*/
#ifdef YYTRACKMAXSTACKDEPTH
int ParseStackPeak(void *p){
  yyParser *pParser = (yyParser*)p;
  return pParser->yyhwm;
}
#endif

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
*/
static unsigned int yy_find_shift_action(
  yyParser *pParser,        /* The parser */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
  int stateno = pParser->yytos->stateno;
 
  if( stateno>=YY_MIN_REDUCE ) return stateno;
  assert( stateno <= YY_SHIFT_COUNT );
  do{
    i = yy_shift_ofst[stateno];
    assert( iLookAhead!=YYNOCODE );
    i += iLookAhead;
    if( i<0 || i>=YY_ACTTAB_COUNT || yy_lookahead[i]!=iLookAhead ){
#ifdef YYFALLBACK
      YYCODETYPE iFallback;            /* Fallback token */
      if( iLookAhead<sizeof(yyFallback)/sizeof(yyFallback[0])
             && (iFallback = yyFallback[iLookAhead])!=0 ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE, "%sFALLBACK %s => %s\n",
             yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[iFallback]);
        }
#endif
        assert( yyFallback[iFallback]==0 ); /* Fallback loop must terminate */
        iLookAhead = iFallback;
        continue;
      }
#endif
#ifdef YYWILDCARD
      {
        int j = i - iLookAhead + YYWILDCARD;
        if( 
#if YY_SHIFT_MIN+YYWILDCARD<0
          j>=0 &&
#endif
#if YY_SHIFT_MAX+YYWILDCARD>=YY_ACTTAB_COUNT
          j<YY_ACTTAB_COUNT &&
#endif
          yy_lookahead[j]==YYWILDCARD && iLookAhead>0
        ){
#ifndef NDEBUG
          if( yyTraceFILE ){
            fprintf(yyTraceFILE, "%sWILDCARD %s => %s\n",
               yyTracePrompt, yyTokenName[iLookAhead],
               yyTokenName[YYWILDCARD]);
          }
#endif /* NDEBUG */
          return yy_action[j];
        }
      }
#endif /* YYWILDCARD */
      return yy_default[stateno];
    }else{
      return yy_action[i];
    }
  }while(1);
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
*/
static int yy_find_reduce_action(
  int stateno,              /* Current state number */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
#ifdef YYERRORSYMBOL
  if( stateno>YY_REDUCE_COUNT ){
    return yy_default[stateno];
  }
#else
  assert( stateno<=YY_REDUCE_COUNT );
#endif
  i = yy_reduce_ofst[stateno];
  assert( i!=YY_REDUCE_USE_DFLT );
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
#ifdef YYERRORSYMBOL
  if( i<0 || i>=YY_ACTTAB_COUNT || yy_lookahead[i]!=iLookAhead ){
    return yy_default[stateno];
  }
#else
  assert( i>=0 && i<YY_ACTTAB_COUNT );
  assert( yy_lookahead[i]==iLookAhead );
#endif
  return yy_action[i];
}

/*
** The following routine is called if the stack overflows.
*/
static void yyStackOverflow(yyParser *yypParser){
   ParseARG_FETCH;
#ifndef NDEBUG
   if( yyTraceFILE ){
     fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
   }
#endif
   while( yypParser->yytos>yypParser->yystack ) yy_pop_parser_stack(yypParser);
   /* Here code is inserted which will execute if the parser
   ** stack every overflows */
/******** Begin %stack_overflow code ******************************************/
/******** End %stack_overflow code ********************************************/
   ParseARG_STORE; /* Suppress warning about unused %extra_argument var */
}

/*
** Print tracing information for a SHIFT action
*/
#ifndef NDEBUG
static void yyTraceShift(yyParser *yypParser, int yyNewState){
  if( yyTraceFILE ){
    if( yyNewState<YYNSTATE ){
      fprintf(yyTraceFILE,"%sShift '%s', go to state %d\n",
         yyTracePrompt,yyTokenName[yypParser->yytos->major],
         yyNewState);
    }else{
      fprintf(yyTraceFILE,"%sShift '%s'\n",
         yyTracePrompt,yyTokenName[yypParser->yytos->major]);
    }
  }
}
#else
# define yyTraceShift(X,Y)
#endif

/*
** Perform a shift action.
*/
static void yy_shift(
  yyParser *yypParser,          /* The parser to be shifted */
  int yyNewState,               /* The new state to shift in */
  int yyMajor,                  /* The major token to shift in */
  ParseTOKENTYPE yyMinor        /* The minor token to shift in */
){
  yyStackEntry *yytos;
  yypParser->yytos++;
#ifdef YYTRACKMAXSTACKDEPTH
  if( (int)(yypParser->yytos - yypParser->yystack)>yypParser->yyhwm ){
    yypParser->yyhwm++;
    assert( yypParser->yyhwm == (int)(yypParser->yytos - yypParser->yystack) );
  }
#endif
#if YYSTACKDEPTH>0 
  if( yypParser->yytos>yypParser->yystackEnd ){
    yypParser->yytos--;
    yyStackOverflow(yypParser);
    return;
  }
#else
  if( yypParser->yytos>=&yypParser->yystack[yypParser->yystksz] ){
    if( yyGrowStack(yypParser) ){
      yypParser->yytos--;
      yyStackOverflow(yypParser);
      return;
    }
  }
#endif
  if( yyNewState > YY_MAX_SHIFT ){
    yyNewState += YY_MIN_REDUCE - YY_MIN_SHIFTREDUCE;
  }
  yytos = yypParser->yytos;
  yytos->stateno = (YYACTIONTYPE)yyNewState;
  yytos->major = (YYCODETYPE)yyMajor;
  yytos->minor.yy0 = yyMinor;
  yyTraceShift(yypParser, yyNewState);
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static const struct {
  YYCODETYPE lhs;       /* Symbol on the left-hand side of the rule */
  signed char nrhs;     /* Negative of the number of RHS symbols in the rule */
} yyRuleInfo[] = {
  { 44, -1 },
  { 43, -2 },
  { 43, 0 },
  { 42, -3 },
  { 42, -3 },
  { 42, -3 },
  { 42, -7 },
  { 42, -1 },
  { 45, -2 },
  { 45, 0 },
  { 46, -3 },
  { 46, -7 },
  { 46, -6 },
  { 46, -1 },
  { 46, -8 },
  { 46, -6 },
  { 46, -2 },
  { 54, -3 },
  { 54, -1 },
  { 54, 0 },
  { 55, -3 },
  { 56, 0 },
  { 56, -2 },
  { 47, -7 },
  { 47, -4 },
  { 49, -2 },
  { 49, 0 },
  { 48, -1 },
  { 48, -1 },
  { 48, -6 },
  { 48, -2 },
  { 48, -9 },
  { 48, -7 },
  { 48, -8 },
  { 57, -2 },
  { 57, 0 },
  { 61, -1 },
  { 61, 0 },
  { 50, -4 },
  { 51, -2 },
  { 51, -5 },
  { 51, -8 },
  { 60, -1 },
  { 60, -1 },
  { 53, -2 },
  { 53, 0 },
  { 52, -1 },
  { 52, -1 },
  { 52, -1 },
  { 62, -1 },
  { 62, -1 },
  { 62, -1 },
  { 62, -1 },
  { 62, -1 },
  { 62, -1 },
  { 62, -1 },
  { 62, -1 },
  { 59, -2 },
  { 59, 0 },
  { 58, -5 },
  { 58, -6 },
  { 63, -3 },
  { 63, -1 },
};

static void yy_accept(yyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void yy_reduce(
  yyParser *yypParser,         /* The parser */
  unsigned int yyruleno        /* Number of the rule by which to reduce */
){
  int yygoto;                     /* The next state */
  int yyact;                      /* The next action */
  yyStackEntry *yymsp;            /* The top of the parser's stack */
  int yysize;                     /* Amount to pop the stack */
  ParseARG_FETCH;
  yymsp = yypParser->yytos;
#ifndef NDEBUG
  if( yyTraceFILE && yyruleno<(int)(sizeof(yyRuleName)/sizeof(yyRuleName[0])) ){
    yysize = yyRuleInfo[yyruleno].nrhs;
    fprintf(yyTraceFILE, "%sReduce [%s], go to state %d.\n", yyTracePrompt,
      yyRuleName[yyruleno], yymsp[yysize].stateno);
  }
#endif /* NDEBUG */

  /* Check that the stack is large enough to grow by a single entry
  ** if the RHS of the rule is empty.  This ensures that there is room
  ** enough on the stack to push the LHS value */
  if( yyRuleInfo[yyruleno].nrhs==0 ){
#ifdef YYTRACKMAXSTACKDEPTH
    if( (int)(yypParser->yytos - yypParser->yystack)>yypParser->yyhwm ){
      yypParser->yyhwm++;
      assert( yypParser->yyhwm == (int)(yypParser->yytos - yypParser->yystack));
    }
#endif
#if YYSTACKDEPTH>0 
    if( yypParser->yytos>=yypParser->yystackEnd ){
      yyStackOverflow(yypParser);
      return;
    }
#else
    if( yypParser->yytos>=&yypParser->yystack[yypParser->yystksz-1] ){
      if( yyGrowStack(yypParser) ){
        yyStackOverflow(yypParser);
        return;
      }
      yymsp = yypParser->yytos;
    }
#endif
  }

  switch( yyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  #line <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  #line <lineno> <thisfile>
  **     break;
  */
/********** Begin reduce actions **********************************************/
        YYMINORTYPE yylhsminor;
      case 0: /* program ::= programlist */
#line 56 "zidl.y"
{
	context->result = yymsp[0].minor.yy9;
}
#line 957 "zidl.c"
        break;
      case 1: /* programlist ::= programlist programbody */
      case 8: /* namespacelist ::= namespacelist namespacebody */ yytestcase(yyruleno==8);
#line 60 "zidl.y"
{
	yylhsminor.yy9 = yymsp[-1].minor.yy9;
	yymsp[-1].minor.yy9->add_child(yymsp[0].minor.yy9);
}
#line 966 "zidl.c"
  yymsp[-1].minor.yy9 = yylhsminor.yy9;
        break;
      case 2: /* programlist ::= */
#line 65 "zidl.y"
{ 
	yymsp[1].minor.yy9 = new node();
	yymsp[1].minor.yy9->type = _ProgramList;
}
#line 975 "zidl.c"
        break;
      case 3: /* programbody ::= INCLUDE DQSTR LINEBREAK */
#line 70 "zidl.y"
{
	yymsp[-2].minor.yy9 = new node();
	yymsp[-2].minor.yy9->type = _Include;
	yymsp[-2].minor.yy9->name = stripquotes(yymsp[-1].minor.yy0.tok);
}
#line 984 "zidl.c"
        break;
      case 4: /* programbody ::= CHEADER DQSTR LINEBREAK */
#line 76 "zidl.y"
{
	yymsp[-2].minor.yy9 = new node();
	yymsp[-2].minor.yy9->type = _Cheader;
	yymsp[-2].minor.yy9->name = stripquotes(yymsp[-1].minor.yy0.tok);
}
#line 993 "zidl.c"
        break;
      case 5: /* programbody ::= IMPORT DQSTR LINEBREAK */
#line 82 "zidl.y"
{
	yymsp[-2].minor.yy9 = new node();
	yymsp[-2].minor.yy9->type = _Import;
	yymsp[-2].minor.yy9->name = stripquotes(yymsp[-1].minor.yy0.tok);
	// derive import filename from context->scriptpath 
	node* zidlimport = zidl_parse_file(context->scriptpath + yymsp[-2].minor.yy9->name);
	if (zidlimport == 0) {
		// programmatic syntax error
		printf("error importing %s\n", yymsp[-2].minor.yy9->name.c_str());
		context->error = 1;
	} else {
		yymsp[-2].minor.yy9->children = zidlimport->children;
		yymsp[-2].minor.yy9->set_parent(yymsp[-2].minor.yy9->children);
		zidlimport->children.clear();
		delete zidlimport;
	}
}
#line 1014 "zidl.c"
        break;
      case 6: /* programbody ::= NAMESPACE IDENTIFIER COLON LINEBREAK INDENT namespacelist DEDENT */
#line 100 "zidl.y"
{
	yymsp[-6].minor.yy9 = new node();
	yymsp[-6].minor.yy9->type = _Namespace;
	yymsp[-6].minor.yy9->name = yymsp[-5].minor.yy0.tok;
	yymsp[-6].minor.yy9->children = yymsp[-1].minor.yy9->children;
	yymsp[-6].minor.yy9->set_parent(yymsp[-6].minor.yy9->children);
	yymsp[-1].minor.yy9->children.clear();
	delete yymsp[-1].minor.yy9;
}
#line 1027 "zidl.c"
        break;
      case 7: /* programbody ::= DQSTR */
#line 110 "zidl.y"
{
	yylhsminor.yy9 = new node();
	yylhsminor.yy9->type = _String;
	yylhsminor.yy9->name = stripquotes(yymsp[0].minor.yy0.tok);
}
#line 1036 "zidl.c"
  yymsp[0].minor.yy9 = yylhsminor.yy9;
        break;
      case 9: /* namespacelist ::= */
#line 121 "zidl.y"
{
	yymsp[1].minor.yy9 = new node();
}
#line 1044 "zidl.c"
        break;
      case 10: /* namespacebody ::= DLNAME DQSTR LINEBREAK */
#line 125 "zidl.y"
{
	yymsp[-2].minor.yy9 = new node();
	yymsp[-2].minor.yy9->type = _DlName;
	yymsp[-2].minor.yy9->name = stripquotes(yymsp[-1].minor.yy0.tok);
}
#line 1053 "zidl.c"
        break;
      case 11: /* namespacebody ::= ENUM IDENTIFIER COLON LINEBREAK INDENT enumlist DEDENT */
#line 131 "zidl.y"
{
	yymsp[-6].minor.yy9 = new node();
	yymsp[-6].minor.yy9->type = _Enum;
	yymsp[-6].minor.yy9->name = yymsp[-5].minor.yy0.tok;
	yymsp[-6].minor.yy9->children = yymsp[-1].minor.yy9->children;
	yymsp[-6].minor.yy9->set_parent(yymsp[-6].minor.yy9->children);
	yymsp[-1].minor.yy9->children.clear();
	delete yymsp[-1].minor.yy9;
}
#line 1066 "zidl.c"
        break;
      case 12: /* namespacebody ::= ENUM COLON LINEBREAK INDENT enumlist DEDENT */
#line 141 "zidl.y"
{
	yymsp[-5].minor.yy9 = new node();
	yymsp[-5].minor.yy9->type = _Enum;
	yymsp[-5].minor.yy9->children = yymsp[-1].minor.yy9->children;
	yymsp[-5].minor.yy9->set_parent(yymsp[-5].minor.yy9->children);
	yymsp[-1].minor.yy9->children.clear();
}
#line 1077 "zidl.c"
        break;
      case 13: /* namespacebody ::= classdef */
      case 27: /* classbody ::= classdef */ yytestcase(yyruleno==27);
      case 28: /* classbody ::= memberbody */ yytestcase(yyruleno==28);
#line 149 "zidl.y"
{
	yylhsminor.yy9 = yymsp[0].minor.yy9;
}
#line 1086 "zidl.c"
  yymsp[0].minor.yy9 = yylhsminor.yy9;
        break;
      case 14: /* namespacebody ::= PDEF IDENTIFIER OPENPAREN argumentlist CLOSEPAREN COLON membertype LINEBREAK */
#line 153 "zidl.y"
{
	yymsp[-7].minor.yy9 = new node();
	yymsp[-7].minor.yy9->type = _CallbackType;
	yymsp[-7].minor.yy9->name = yymsp[-6].minor.yy0.tok;
	yymsp[-7].minor.yy9->metatype = yymsp[-1].minor.yy9;
	yymsp[-7].minor.yy9->children = yymsp[-4].minor.yy9->children;
	yymsp[-7].minor.yy9->set_parent(yymsp[-7].minor.yy9->children);
	yymsp[-4].minor.yy9->children.clear();
	delete yymsp[-4].minor.yy9;
}
#line 1101 "zidl.c"
        break;
      case 15: /* namespacebody ::= PDEF IDENTIFIER OPENPAREN argumentlist CLOSEPAREN LINEBREAK */
#line 164 "zidl.y"
{
	yymsp[-5].minor.yy9 = new node();
	yymsp[-5].minor.yy9->type = _CallbackType;
	yymsp[-5].minor.yy9->name = yymsp[-4].minor.yy0.tok;
	yymsp[-5].minor.yy9->children = yymsp[-2].minor.yy9->children;
	yymsp[-5].minor.yy9->set_parent(yymsp[-5].minor.yy9->children);
	yymsp[-2].minor.yy9->children.clear();
	delete yymsp[-2].minor.yy9;
}
#line 1114 "zidl.c"
        break;
      case 16: /* namespacebody ::= DQSTR LINEBREAK */
      case 30: /* classbody ::= DQSTR LINEBREAK */ yytestcase(yyruleno==30);
#line 174 "zidl.y"
{
	yylhsminor.yy9 = new node();
	yylhsminor.yy9->type = _String;
	yylhsminor.yy9->name = stripquotes(yymsp[-1].minor.yy0.tok);
}
#line 1124 "zidl.c"
  yymsp[-1].minor.yy9 = yylhsminor.yy9;
        break;
      case 17: /* argumentlist ::= argumentlist COMMA argumentbody */
#line 180 "zidl.y"
{
	yylhsminor.yy9 = yymsp[-2].minor.yy9;
	yylhsminor.yy9->add_child(yymsp[0].minor.yy9);
}
#line 1133 "zidl.c"
  yymsp[-2].minor.yy9 = yylhsminor.yy9;
        break;
      case 18: /* argumentlist ::= argumentbody */
#line 185 "zidl.y"
{
	yylhsminor.yy9 = new node();
	yylhsminor.yy9->type = _ArgumentList;
	yylhsminor.yy9->add_child(yymsp[0].minor.yy9);
}
#line 1143 "zidl.c"
  yymsp[0].minor.yy9 = yylhsminor.yy9;
        break;
      case 19: /* argumentlist ::= */
#line 191 "zidl.y"
{
	yymsp[1].minor.yy9 = new node();
	yymsp[1].minor.yy9->type = _ArgumentList;
}
#line 1152 "zidl.c"
        break;
      case 20: /* argumentbody ::= membertype IDENTIFIER argumentdefault */
#line 196 "zidl.y"
{
	yylhsminor.yy9 = new node();
	yylhsminor.yy9->type = _Argument;
	yylhsminor.yy9->name = yymsp[-1].minor.yy0.tok;
	yylhsminor.yy9->metatype = yymsp[-2].minor.yy9;
	if (yymsp[0].minor.yy9 != 0) yylhsminor.yy9->modifiers.push_back(yymsp[0].minor.yy9);
}
#line 1163 "zidl.c"
  yymsp[-2].minor.yy9 = yylhsminor.yy9;
        break;
      case 21: /* argumentdefault ::= */
#line 204 "zidl.y"
{ 
	/* OK */
	yymsp[1].minor.yy9 = 0;
}
#line 1172 "zidl.c"
        break;
      case 22: /* argumentdefault ::= EQ DIGITS */
#line 209 "zidl.y"
{
	yymsp[-1].minor.yy9 = new node();
	yymsp[-1].minor.yy9->type = _EnumValue;
	yymsp[-1].minor.yy9->intvalue = atoi(yymsp[0].minor.yy0.tok);
}
#line 1181 "zidl.c"
        break;
      case 23: /* classdef ::= CLASS IDENTIFIER COLON LINEBREAK INDENT classlist DEDENT */
#line 215 "zidl.y"
{
	yymsp[-6].minor.yy9 = new node();
	yymsp[-6].minor.yy9->type = _Class;
	yymsp[-6].minor.yy9->name = yymsp[-5].minor.yy0.tok;
	yymsp[-6].minor.yy9->children = yymsp[-1].minor.yy9->children;
	yymsp[-6].minor.yy9->set_parent(yymsp[-6].minor.yy9->children);
	yymsp[-1].minor.yy9->children.clear();
	delete yymsp[-1].minor.yy9;
}
#line 1194 "zidl.c"
        break;
      case 24: /* classdef ::= CLASS IDENTIFIER COLON LINEBREAK */
#line 225 "zidl.y"
{
	yymsp[-3].minor.yy9 = new node();
	yymsp[-3].minor.yy9->type = _Class;
	yymsp[-3].minor.yy9->name = yymsp[-2].minor.yy0.tok;
}
#line 1203 "zidl.c"
        break;
      case 25: /* classlist ::= classlist classbody */
      case 44: /* typemodifierlist ::= typemodifierlist typemodifier */ yytestcase(yyruleno==44);
      case 57: /* enumlist ::= enumlist enumbody */ yytestcase(yyruleno==57);
#line 231 "zidl.y"
{
	yylhsminor.yy9 = yymsp[-1].minor.yy9;
	yylhsminor.yy9->add_child(yymsp[0].minor.yy9);
}
#line 1213 "zidl.c"
  yymsp[-1].minor.yy9 = yylhsminor.yy9;
        break;
      case 26: /* classlist ::= */
#line 236 "zidl.y"
{
	yymsp[1].minor.yy9 = new node();
	yymsp[1].minor.yy9->type = _ClassList;
}
#line 1222 "zidl.c"
        break;
      case 29: /* classbody ::= UNION COLON LINEBREAK INDENT unionlist DEDENT */
#line 249 "zidl.y"
{
	yymsp[-5].minor.yy9 = new node();
	yymsp[-5].minor.yy9->type = _Union;
	static int unioncounter = 0;
	std::stringstream idstrm;
	idstrm << "union_" << std::hex << std::setw(8) << std::setfill('0') << unioncounter;
	yymsp[-5].minor.yy9->name = idstrm.str();
	unioncounter++;
	yymsp[-5].minor.yy9->children = yymsp[-1].minor.yy9->children;
	yymsp[-5].minor.yy9->set_parent(yymsp[-5].minor.yy9->children);
	yymsp[-1].minor.yy9->children.clear();
	delete yymsp[-1].minor.yy9;
}
#line 1239 "zidl.c"
        break;
      case 31: /* classbody ::= DEF methodmodifier IDENTIFIER OPENPAREN argumentlist CLOSEPAREN COLON membertype LINEBREAK */
#line 269 "zidl.y"
{
	yymsp[-8].minor.yy9 = new node();
	yymsp[-8].minor.yy9->type = _Method;
	yymsp[-8].minor.yy9->name = yymsp[-6].minor.yy0.tok;
	yymsp[-8].minor.yy9->metatype = yymsp[-1].minor.yy9;
	yymsp[-8].minor.yy9->children = yymsp[-4].minor.yy9->children;
	yymsp[-8].minor.yy9->set_parent(yymsp[-8].minor.yy9->children);
	yymsp[-4].minor.yy9->children.clear();
	delete yymsp[-4].minor.yy9;
	if (yymsp[-7].minor.yy9 != 0)
		yymsp[-8].minor.yy9->modifiers.push_back(yymsp[-7].minor.yy9);
}
#line 1255 "zidl.c"
        break;
      case 32: /* classbody ::= DEF methodmodifier IDENTIFIER OPENPAREN argumentlist CLOSEPAREN LINEBREAK */
#line 282 "zidl.y"
{
	yymsp[-6].minor.yy9 = new node();
	yymsp[-6].minor.yy9->type = _Method;
	yymsp[-6].minor.yy9->name = yymsp[-4].minor.yy0.tok;
	yymsp[-6].minor.yy9->children = yymsp[-2].minor.yy9->children;
	yymsp[-6].minor.yy9->set_parent(yymsp[-6].minor.yy9->children);
	yymsp[-2].minor.yy9->children.clear();
	delete yymsp[-2].minor.yy9;
	if (yymsp[-5].minor.yy9 != 0)
		yymsp[-6].minor.yy9->modifiers.push_back(yymsp[-5].minor.yy9);
}
#line 1270 "zidl.c"
        break;
      case 33: /* classbody ::= ITERATOR IDENTIFIER COLON FOR IDENTIFIER IN IDENTIFIER LINEBREAK */
#line 294 "zidl.y"
{
	/* ignore iterators */
	yymsp[-7].minor.yy9 = new node();
	yymsp[-7].minor.yy9->type = _Ignore;
}
#line 1279 "zidl.c"
        break;
      case 34: /* unionlist ::= unionlist memberbody */
#line 300 "zidl.y"
{
	yylhsminor.yy9 = yymsp[-1].minor.yy9;
	yylhsminor.yy9->add_child(yymsp[0].minor.yy9);	
}
#line 1287 "zidl.c"
  yymsp[-1].minor.yy9 = yylhsminor.yy9;
        break;
      case 35: /* unionlist ::= */
#line 305 "zidl.y"
{
	yymsp[1].minor.yy9 = new node();
	yymsp[1].minor.yy9->type = _UnionList;
}
#line 1296 "zidl.c"
        break;
      case 36: /* methodmodifier ::= STATIC */
#line 310 "zidl.y"
{
	yymsp[0].minor.yy9 = new node();
	yymsp[0].minor.yy9->type = _TypeModifier;
	yymsp[0].minor.yy9->name = "static";
}
#line 1305 "zidl.c"
        break;
      case 37: /* methodmodifier ::= */
#line 316 "zidl.y"
{
	yymsp[1].minor.yy9 = 0;
}
#line 1312 "zidl.c"
        break;
      case 38: /* memberbody ::= MEMBER membertype IDENTIFIER LINEBREAK */
#line 320 "zidl.y"
{
	yymsp[-3].minor.yy9 = new node();
	yymsp[-3].minor.yy9->type = _Member;
	yymsp[-3].minor.yy9->name = yymsp[-1].minor.yy0.tok;
	yymsp[-3].minor.yy9->metatype = yymsp[-2].minor.yy9;
}
#line 1322 "zidl.c"
        break;
      case 39: /* membertype ::= typemodifierlist typename */
#line 327 "zidl.y"
{
	yylhsminor.yy9 = new node();
	yylhsminor.yy9->type = _Type;
	yylhsminor.yy9->name = yymsp[0].minor.yy0.tok;
	yylhsminor.yy9->modifiers = yymsp[-1].minor.yy9->children;
	yylhsminor.yy9->set_parent(yylhsminor.yy9->children);
	yymsp[-1].minor.yy9->children.clear();
	delete yymsp[-1].minor.yy9;
}
#line 1335 "zidl.c"
  yymsp[-1].minor.yy9 = yylhsminor.yy9;
        break;
      case 40: /* membertype ::= typemodifierlist typename OPENSQRPAREN arrayindexer CLOSESQRPAREN */
#line 337 "zidl.y"
{
	yylhsminor.yy9 = new node();
	yylhsminor.yy9->type = _Type;
	yylhsminor.yy9->name = yymsp[-3].minor.yy0.tok;
	yylhsminor.yy9->modifiers = yymsp[-4].minor.yy9->children;
	yylhsminor.yy9->set_parent(yylhsminor.yy9->modifiers);
	yylhsminor.yy9->add_child(yymsp[-1].minor.yy9);
	yymsp[-4].minor.yy9->children.clear();
	delete yymsp[-4].minor.yy9;
}
#line 1350 "zidl.c"
  yymsp[-4].minor.yy9 = yylhsminor.yy9;
        break;
      case 41: /* membertype ::= typemodifierlist typename OPENSQRPAREN arrayindexer CLOSESQRPAREN OPENSQRPAREN arrayindexer CLOSESQRPAREN */
#line 348 "zidl.y"
{
	yylhsminor.yy9 = new node();
	yylhsminor.yy9->type = _Type;
	yylhsminor.yy9->name = yymsp[-6].minor.yy0.tok;
	yylhsminor.yy9->modifiers = yymsp[-7].minor.yy9->children;
	yylhsminor.yy9->set_parent(yylhsminor.yy9->modifiers);
	yylhsminor.yy9->add_child(yymsp[-4].minor.yy9);
	yylhsminor.yy9->add_child(yymsp[-1].minor.yy9);
	yymsp[-7].minor.yy9->children.clear();
	delete yymsp[-7].minor.yy9;
}
#line 1366 "zidl.c"
  yymsp[-7].minor.yy9 = yylhsminor.yy9;
        break;
      case 42: /* arrayindexer ::= IDENTIFIER */
#line 360 "zidl.y"
{
	yylhsminor.yy9 = new node();
	yylhsminor.yy9->type = _Indexer;
	yylhsminor.yy9->name = yymsp[0].minor.yy0.tok;
	yylhsminor.yy9->intvalue = 0;
}
#line 1377 "zidl.c"
  yymsp[0].minor.yy9 = yylhsminor.yy9;
        break;
      case 43: /* arrayindexer ::= DIGITS */
#line 367 "zidl.y"
{
	yylhsminor.yy9 = new node();
	yylhsminor.yy9->type = _Indexer;
	yylhsminor.yy9->name = yymsp[0].minor.yy0.tok;
	yylhsminor.yy9->intvalue = atoi(yymsp[0].minor.yy0.tok);
}
#line 1388 "zidl.c"
  yymsp[0].minor.yy9 = yylhsminor.yy9;
        break;
      case 45: /* typemodifierlist ::= */
#line 379 "zidl.y"
{
	yymsp[1].minor.yy9 = new node();
	yymsp[1].minor.yy9->type = _TypeModifierList;
}
#line 1397 "zidl.c"
        break;
      case 46: /* typemodifier ::= OUT */
#line 384 "zidl.y"
{
	yymsp[0].minor.yy9 = new node();
	yymsp[0].minor.yy9->type = _TypeModifier;
	yymsp[0].minor.yy9->name = "out";
}
#line 1406 "zidl.c"
        break;
      case 47: /* typemodifier ::= NOREF */
#line 390 "zidl.y"
{
	yymsp[0].minor.yy9 = new node();
	yymsp[0].minor.yy9->type = _TypeModifier;
	yymsp[0].minor.yy9->name = "noref";
}
#line 1415 "zidl.c"
        break;
      case 48: /* typemodifier ::= NO_PYTHON */
#line 396 "zidl.y"
{
	yymsp[0].minor.yy9 = new node();
	yymsp[0].minor.yy9->type = _TypeModifier;
	yymsp[0].minor.yy9->name = "no_python";
}
#line 1424 "zidl.c"
        break;
      case 49: /* typename ::= IDENTIFIER */
      case 50: /* typename ::= INT */ yytestcase(yyruleno==50);
      case 51: /* typename ::= UINT */ yytestcase(yyruleno==51);
      case 52: /* typename ::= FLOAT */ yytestcase(yyruleno==52);
      case 53: /* typename ::= STRING */ yytestcase(yyruleno==53);
      case 54: /* typename ::= CHAR */ yytestcase(yyruleno==54);
      case 55: /* typename ::= PVOID */ yytestcase(yyruleno==55);
      case 56: /* typename ::= BOOL */ yytestcase(yyruleno==56);
#line 402 "zidl.y"
{
	yylhsminor.yy0 = yymsp[0].minor.yy0;
}
#line 1438 "zidl.c"
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 58: /* enumlist ::= */
#line 439 "zidl.y"
{
	yymsp[1].minor.yy9 = new node();
	yymsp[1].minor.yy9->type = _EnumList;
}
#line 1447 "zidl.c"
        break;
      case 59: /* enumbody ::= SET IDENTIFIER EQ DIGITS LINEBREAK */
#line 444 "zidl.y"
{
	yymsp[-4].minor.yy9 = new node();
	yymsp[-4].minor.yy9->type = _EnumValue;
	yymsp[-4].minor.yy9->name = yymsp[-3].minor.yy0.tok;
	yymsp[-4].minor.yy9->intvalue = atoi(yymsp[-1].minor.yy0.tok);
}
#line 1457 "zidl.c"
        break;
      case 60: /* enumbody ::= SET IDENTIFIER EQ BIT bitvalues LINEBREAK */
#line 451 "zidl.y"
{
	yymsp[-5].minor.yy9 = new node();
	yymsp[-5].minor.yy9->type = _EnumValue;
	yymsp[-5].minor.yy9->name = yymsp[-4].minor.yy0.tok;
	yymsp[-5].minor.yy9->intvalue = yymsp[-1].minor.yy0.tokval;
}
#line 1467 "zidl.c"
        break;
      case 61: /* bitvalues ::= bitvalues COMMA DIGITS */
#line 458 "zidl.y"
{
	yylhsminor.yy0.tokval = yymsp[-2].minor.yy0.tokval | (1 << atoi(yymsp[0].minor.yy0.tok));
}
#line 1474 "zidl.c"
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 62: /* bitvalues ::= DIGITS */
#line 462 "zidl.y"
{
	yylhsminor.yy0.tokval = 1 << atoi(yymsp[0].minor.yy0.tok);
}
#line 1482 "zidl.c"
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      default:
        break;
/********** End reduce actions ************************************************/
  };
  assert( yyruleno<sizeof(yyRuleInfo)/sizeof(yyRuleInfo[0]) );
  yygoto = yyRuleInfo[yyruleno].lhs;
  yysize = yyRuleInfo[yyruleno].nrhs;
  yyact = yy_find_reduce_action(yymsp[yysize].stateno,(YYCODETYPE)yygoto);

  /* There are no SHIFTREDUCE actions on nonterminals because the table
  ** generator has simplified them to pure REDUCE actions. */
  assert( !(yyact>YY_MAX_SHIFT && yyact<=YY_MAX_SHIFTREDUCE) );

  /* It is not possible for a REDUCE to be followed by an error */
  assert( yyact!=YY_ERROR_ACTION );

  if( yyact==YY_ACCEPT_ACTION ){
    yypParser->yytos += yysize;
    yy_accept(yypParser);
  }else{
    yymsp += yysize+1;
    yypParser->yytos = yymsp;
    yymsp->stateno = (YYACTIONTYPE)yyact;
    yymsp->major = (YYCODETYPE)yygoto;
    yyTraceShift(yypParser, yyact);
  }
}

/*
** The following code executes when the parse fails
*/
#ifndef YYNOERRORRECOVERY
static void yy_parse_failed(
  yyParser *yypParser           /* The parser */
){
  ParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yytos>yypParser->yystack ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
/************ Begin %parse_failure code ***************************************/
/************ End %parse_failure code *****************************************/
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}
#endif /* YYNOERRORRECOVERY */

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  int yymajor,                   /* The major type of the error token */
  ParseTOKENTYPE yyminor         /* The minor type of the error token */
){
  ParseARG_FETCH;
#define TOKEN yyminor
/************ Begin %syntax_error code ****************************************/
#line 20 "zidl.y"

	context->error = 1;
#line 1549 "zidl.c"
/************ End %syntax_error code ******************************************/
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
  yyParser *yypParser           /* The parser */
){
  ParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sAccept!\n",yyTracePrompt);
  }
#endif
#ifndef YYNOERRORRECOVERY
  yypParser->yyerrcnt = -1;
#endif
  assert( yypParser->yytos==yypParser->yystack );
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
/*********** Begin %parse_accept code *****************************************/
#line 24 "zidl.y"

	/* printf("parsing complete!\n"); */
#line 1576 "zidl.c"
/*********** End %parse_accept code *******************************************/
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "ParseAlloc" which describes the current state of the parser.
** The second argument is the major token number.  The third is
** the minor token.  The fourth optional argument is whatever the
** user wants (and specified in the grammar) and is available for
** use by the action routines.
**
** Inputs:
** <ul>
** <li> A pointer to the parser (an opaque structure.)
** <li> The major token number.
** <li> The minor token number.
** <li> An option argument of a grammar-specified type.
** </ul>
**
** Outputs:
** None.
*/
void Parse(
  void *yyp,                   /* The parser */
  int yymajor,                 /* The major token code number */
  ParseTOKENTYPE yyminor       /* The value for the token */
  ParseARG_PDECL               /* Optional %extra_argument parameter */
){
  YYMINORTYPE yyminorunion;
  unsigned int yyact;   /* The parser action. */
#if !defined(YYERRORSYMBOL) && !defined(YYNOERRORRECOVERY)
  int yyendofinput;     /* True if we are at the end of input */
#endif
#ifdef YYERRORSYMBOL
  int yyerrorhit = 0;   /* True if yymajor has invoked an error */
#endif
  yyParser *yypParser;  /* The parser */

  yypParser = (yyParser*)yyp;
  assert( yypParser->yytos!=0 );
#if !defined(YYERRORSYMBOL) && !defined(YYNOERRORRECOVERY)
  yyendofinput = (yymajor==0);
#endif
  ParseARG_STORE;

#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sInput '%s'\n",yyTracePrompt,yyTokenName[yymajor]);
  }
#endif

  do{
    yyact = yy_find_shift_action(yypParser,(YYCODETYPE)yymajor);
    if( yyact <= YY_MAX_SHIFTREDUCE ){
      yy_shift(yypParser,yyact,yymajor,yyminor);
#ifndef YYNOERRORRECOVERY
      yypParser->yyerrcnt--;
#endif
      yymajor = YYNOCODE;
    }else if( yyact <= YY_MAX_REDUCE ){
      yy_reduce(yypParser,yyact-YY_MIN_REDUCE);
    }else{
      assert( yyact == YY_ERROR_ACTION );
      yyminorunion.yy0 = yyminor;
#ifdef YYERRORSYMBOL
      int yymx;
#endif
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE,"%sSyntax Error!\n",yyTracePrompt);
      }
#endif
#ifdef YYERRORSYMBOL
      /* A syntax error has occurred.
      ** The response to an error depends upon whether or not the
      ** grammar defines an error token "ERROR".  
      **
      ** This is what we do if the grammar does define ERROR:
      **
      **  * Call the %syntax_error function.
      **
      **  * Begin popping the stack until we enter a state where
      **    it is legal to shift the error symbol, then shift
      **    the error symbol.
      **
      **  * Set the error count to three.
      **
      **  * Begin accepting and shifting new tokens.  No new error
      **    processing will occur until three tokens have been
      **    shifted successfully.
      **
      */
      if( yypParser->yyerrcnt<0 ){
        yy_syntax_error(yypParser,yymajor,yyminor);
      }
      yymx = yypParser->yytos->major;
      if( yymx==YYERRORSYMBOL || yyerrorhit ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE,"%sDiscard input token %s\n",
             yyTracePrompt,yyTokenName[yymajor]);
        }
#endif
        yy_destructor(yypParser, (YYCODETYPE)yymajor, &yyminorunion);
        yymajor = YYNOCODE;
      }else{
        while( yypParser->yytos >= yypParser->yystack
            && yymx != YYERRORSYMBOL
            && (yyact = yy_find_reduce_action(
                        yypParser->yytos->stateno,
                        YYERRORSYMBOL)) >= YY_MIN_REDUCE
        ){
          yy_pop_parser_stack(yypParser);
        }
        if( yypParser->yytos < yypParser->yystack || yymajor==0 ){
          yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
          yy_parse_failed(yypParser);
#ifndef YYNOERRORRECOVERY
          yypParser->yyerrcnt = -1;
#endif
          yymajor = YYNOCODE;
        }else if( yymx!=YYERRORSYMBOL ){
          yy_shift(yypParser,yyact,YYERRORSYMBOL,yyminor);
        }
      }
      yypParser->yyerrcnt = 3;
      yyerrorhit = 1;
#elif defined(YYNOERRORRECOVERY)
      /* If the YYNOERRORRECOVERY macro is defined, then do not attempt to
      ** do any kind of error recovery.  Instead, simply invoke the syntax
      ** error routine and continue going as if nothing had happened.
      **
      ** Applications can set this macro (for example inside %include) if
      ** they intend to abandon the parse upon the first syntax error seen.
      */
      yy_syntax_error(yypParser,yymajor, yyminor);
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      yymajor = YYNOCODE;
      
#else  /* YYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if( yypParser->yyerrcnt<=0 ){
        yy_syntax_error(yypParser,yymajor, yyminor);
      }
      yypParser->yyerrcnt = 3;
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      if( yyendofinput ){
        yy_parse_failed(yypParser);
#ifndef YYNOERRORRECOVERY
        yypParser->yyerrcnt = -1;
#endif
      }
      yymajor = YYNOCODE;
#endif
    }
  }while( yymajor!=YYNOCODE && yypParser->yytos>yypParser->yystack );
#ifndef NDEBUG
  if( yyTraceFILE ){
    yyStackEntry *i;
    char cDiv = '[';
    fprintf(yyTraceFILE,"%sReturn. Stack=",yyTracePrompt);
    for(i=&yypParser->yystack[1]; i<=yypParser->yytos; i++){
      fprintf(yyTraceFILE,"%c%s", cDiv, yyTokenName[i->major]);
      cDiv = ' ';
    }
    fprintf(yyTraceFILE,"]\n");
  }
#endif
  return;
}
