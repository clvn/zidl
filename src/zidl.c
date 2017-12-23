/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is included that follows the "include" declaration
** in the input grammar file. */
#include <stdio.h>
#include <assert.h>
#line 7 "zidl.y"

#include <vector>
#include <stack>
#include <istream>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <cstdlib>
#include "zidlparser.h"
#line 19 "zidl.c"
/* Next is all token values, in a form suitable for use by makeheaders.
** This section will be null unless lemon is run with the -m switch.
*/
/* 
** These constants (all generated automatically by the parser generator)
** specify the various kinds of tokens (terminals) that the parser
** understands. 
**
** Each symbol here is a terminal symbol in the grammar.
*/
/* Make sure the INTERFACE macro is defined.
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/* The next thing included is series of defines which control
** various aspects of the generated parser.
**    YYCODETYPE         is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 terminals
**                       and nonterminals.  "int" is used otherwise.
**    YYNOCODE           is a number of type YYCODETYPE which corresponds
**                       to no legal terminal or nonterminal number.  This
**                       number is used to fill in empty slots of the hash 
**                       table.
**    YYFALLBACK         If defined, this indicates that one or more tokens
**                       have fall-back values which should be used if the
**                       original value of the token will not parse.
**    YYACTIONTYPE       is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 rules and
**                       states combined.  "int" is used otherwise.
**    ParseTOKENTYPE     is the data type used for minor tokens given 
**                       directly to the parser from the tokenizer.
**    YYMINORTYPE        is the data type used for all minor tokens.
**                       This is typically a union of many types, one of
**                       which is ParseTOKENTYPE.  The entry in the union
**                       for base tokens is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.  If
**                       zero the stack is dynamically sized using realloc()
**    ParseARG_SDECL     A static variable declaration for the %extra_argument
**    ParseARG_PDECL     A parameter declaration for the %extra_argument
**    ParseARG_STORE     Code to store %extra_argument into yypParser
**    ParseARG_FETCH     Code to extract %extra_argument from yypParser
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
*/
#define YYCODETYPE unsigned char
#define YYNOCODE 65
#define YYACTIONTYPE unsigned char
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
#define YYNSTATE 132
#define YYNRULE 63
#define YY_NO_ACTION      (YYNSTATE+YYNRULE+2)
#define YY_ACCEPT_ACTION  (YYNSTATE+YYNRULE+1)
#define YY_ERROR_ACTION   (YYNSTATE+YYNRULE)

/* The yyzerominor constant is used to initialize instances of
** YYMINORTYPE objects to zero. */
static const YYMINORTYPE yyzerominor = { 0 };

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
**   0 <= N < YYNSTATE                  Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   YYNSTATE <= N < YYNSTATE+YYNRULE   Reduce by rule N-YYNSTATE.
**
**   N == YYNSTATE+YYNRULE              A syntax error has occurred.
**
**   N == YYNSTATE+YYNRULE+1            The parser accepts its input.
**
**   N == YYNSTATE+YYNRULE+2            No such action.  Denotes unused
**                                      slots in the yy_action[] table.
**
** The action table is constructed as a single large table named yy_action[].
** Given state S and lookahead X, the action is computed as
**
**      yy_action[ yy_shift_ofst[S] + X ]
**
** If the index value yy_shift_ofst[S]+X is out of range or if the value
** yy_lookahead[yy_shift_ofst[S]+X] is not equal to X or if yy_shift_ofst[S]
** is equal to YY_SHIFT_USE_DFLT, it means that the action is not in the table
** and that yy_default[S] should be used instead.  
**
** The formula above is for computing the action when the lookahead is
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
*/
static const YYACTIONTYPE yy_action[] = {
 /*     0 */    99,  132,   71,  131,   77,   61,   53,   45,   97,   96,
 /*    10 */    59,   98,  124,   44,   27,   48,   81,  118,    5,   25,
 /*    20 */    86,   76,  102,  101,  100,   94,   93,   92,   91,   90,
 /*    30 */    89,   87,   47,   81,  112,    5,   28,   86,   34,   81,
 /*    40 */    95,    5,  115,   82,  151,  151,   31,  103,  105,   76,
 /*    50 */    70,   18,   72,   10,  196,   39,    8,   57,  123,  110,
 /*    60 */    16,   30,    4,   31,   33,  104,    5,   52,   42,  106,
 /*    70 */    63,   62,    5,   54,   84,    5,   24,    4,  121,    9,
 /*    80 */   129,    8,  113,    7,  117,   78,   26,   83,  122,   80,
 /*    90 */    75,    3,   74,   11,   73,   12,   19,    6,   66,   35,
 /*   100 */    15,  128,   69,   88,   68,   79,   85,   21,   20,   65,
 /*   110 */    40,  130,  127,   32,   55,  120,   58,  107,  116,   46,
 /*   120 */    14,   29,   22,   60,  126,   23,   37,   64,  114,  109,
 /*   130 */    51,    1,   13,  108,   17,   67,   50,   38,  119,   43,
 /*   140 */    41,   36,  125,  111,  197,    2,  197,   56,   49,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     7,    0,    1,    2,    2,    4,    5,    6,   47,   48,
 /*    10 */    60,   50,   10,   11,   12,   13,   51,   10,   53,   54,
 /*    20 */    55,   19,   29,   30,   31,   32,   33,   34,   35,   36,
 /*    30 */    37,   38,    2,   51,   10,   53,   54,   55,   18,   51,
 /*    40 */    10,   53,    3,   55,   15,   16,   39,   52,   50,   19,
 /*    50 */    20,   21,   22,   43,   44,   16,   26,   62,   46,   47,
 /*    60 */    40,   15,   16,   39,   51,   10,   53,    7,    8,    7,
 /*    70 */    51,   60,   53,   51,    3,   53,   15,   16,    3,    8,
 /*    80 */    18,   26,   18,    8,   58,   17,   63,   56,   25,   61,
 /*    90 */     7,   49,    8,   59,    3,   57,    9,   45,    2,    7,
 /*   100 */    59,   42,    8,    3,    3,   24,   18,    9,    9,    7,
 /*   110 */     7,    3,    3,    7,    7,    3,    2,   28,    3,    8,
 /*   120 */    27,   17,    9,   28,    3,    9,    3,    7,   18,    3,
 /*   130 */     8,   14,   27,    3,    7,    3,    2,    8,    3,    2,
 /*   140 */     7,    3,    3,    3,   64,   14,   64,   23,    7,
};
#define YY_SHIFT_USE_DFLT (-8)
#define YY_SHIFT_MAX 81
static const short yy_shift_ofst[] = {
 /*     0 */    -8,   29,   29,   30,   -8,   -7,    2,   -8,   -8,   -8,
 /*    10 */     1,   24,   55,   62,   62,    7,   64,   68,   63,   -8,
 /*    20 */    -8,   -8,   -8,   -8,   71,   61,   39,   60,   46,   20,
 /*    30 */    75,  106,  104,  112,  115,  111,  113,  116,  123,  110,
 /*    40 */   117,  122,  132,  135,  137,  133,  124,  140,  141,  131,
 /*    50 */   139,  138,  129,  134,  130,  126,  120,  105,  121,   95,
 /*    60 */    93,  114,   89,  107,   81,  108,  109,   99,   98,  101,
 /*    70 */    94,   96,   92,   87,   91,   84,   83,  100,   88,  102,
 /*    80 */   103,  127,
};
#define YY_REDUCE_USE_DFLT (-51)
#define YY_REDUCE_MAX 23
static const signed char yy_reduce_ofst[] = {
 /*     0 */    10,  -18,  -35,  -39,  -12,   -5,   12,   13,   19,   22,
 /*    10 */    59,   26,   -2,  -50,   11,   26,   23,   31,   28,   42,
 /*    20 */    34,   38,   52,   41,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   134,  177,  177,  195,  177,  195,  195,  177,  177,  177,
 /*    10 */   195,  195,  195,  195,  195,  195,  195,  153,  169,  158,
 /*    20 */   190,  167,  141,  190,  195,  195,  195,  195,  195,  195,
 /*    30 */   195,  195,  195,  195,  195,  195,  195,  195,  195,  195,
 /*    40 */   195,  195,  195,  195,  195,  195,  195,  195,  195,  195,
 /*    50 */   195,  195,  195,  195,  195,  195,  195,  171,  195,  195,
 /*    60 */   172,  195,  195,  195,  195,  195,  195,  195,  195,  195,
 /*    70 */   195,  195,  195,  156,  195,  195,  195,  195,  195,  195,
 /*    80 */   195,  195,  149,  152,  147,  154,  150,  188,  148,  187,
 /*    90 */   186,  185,  184,  183,  182,  155,  157,  159,  160,  181,
 /*   100 */   180,  179,  178,  176,  161,  166,  174,  173,  146,  170,
 /*   110 */   145,  162,  144,  194,  193,  192,  191,  189,  143,  142,
 /*   120 */   163,  164,  168,  140,  138,  137,  136,  135,  133,  175,
 /*   130 */   165,  139,
};
#define YY_SZ_ACTTAB (int)(sizeof(yy_action)/sizeof(yy_action[0]))

/* The next table maps tokens into fallback tokens.  If a construct
** like the following:
** 
**      %fallback ID X Y Z.
**
** appears in the grammar, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
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
*/
struct yyStackEntry {
  YYACTIONTYPE stateno;  /* The state-number */
  YYCODETYPE major;      /* The major token value.  This is the code
                         ** number for the token at this stack level */
  YYMINORTYPE minor;     /* The user-supplied minor token value.  This
                         ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser {
  int yyidx;                    /* Index of top element in stack */
#ifdef YYTRACKMAXSTACKDEPTH
  int yyidxMax;                 /* Maximum value of yyidx */
#endif
  int yyerrcnt;                 /* Shifts left before out of the error */
  ParseARG_SDECL                /* A place to hold %extra_argument */
#if YYSTACKDEPTH<=0
  int yystksz;                  /* Current side of the stack */
  yyStackEntry *yystack;        /* The parser's stack */
#else
  yyStackEntry yystack[YYSTACKDEPTH];  /* The parser's stack */
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
** Try to increase the size of the parser stack.
*/
static void yyGrowStack(yyParser *p){
  int newSize;
  yyStackEntry *pNew;

  newSize = p->yystksz*2 + 100;
  pNew = realloc(p->yystack, newSize*sizeof(pNew[0]));
  if( pNew ){
    p->yystack = pNew;
    p->yystksz = newSize;
#ifndef NDEBUG
    if( yyTraceFILE ){
      fprintf(yyTraceFILE,"%sStack grows to %d entries!\n",
              yyTracePrompt, p->yystksz);
    }
#endif
  }
}
#endif

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
void *ParseAlloc(void *(*mallocProc)(size_t)){
  yyParser *pParser;
  pParser = (yyParser*)(*mallocProc)( (size_t)sizeof(yyParser) );
  if( pParser ){
    pParser->yyidx = -1;
#ifdef YYTRACKMAXSTACKDEPTH
    pParser->yyidxMax = 0;
#endif
#if YYSTACKDEPTH<=0
    pParser->yystack = NULL;
    pParser->yystksz = 0;
    yyGrowStack(pParser);
#endif
  }
  return pParser;
}

/* The following function deletes the value associated with a
** symbol.  The symbol can be either a terminal or nonterminal.
** "yymajor" is the symbol code, and "yypminor" is a pointer to
** the value.
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
    ** which appear on the RHS of the rule, but which are not used
    ** inside the C code.
    */
    default:  break;   /* If no destructor action specified: do nothing */
  }
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
**
** Return the major token number for the symbol popped.
*/
static int yy_pop_parser_stack(yyParser *pParser){
  YYCODETYPE yymajor;
  yyStackEntry *yytos = &pParser->yystack[pParser->yyidx];

  if( pParser->yyidx<0 ) return 0;
#ifndef NDEBUG
  if( yyTraceFILE && pParser->yyidx>=0 ){
    fprintf(yyTraceFILE,"%sPopping %s\n",
      yyTracePrompt,
      yyTokenName[yytos->major]);
  }
#endif
  yymajor = yytos->major;
  yy_destructor(pParser, yymajor, &yytos->minor);
  pParser->yyidx--;
  return yymajor;
}

/* 
** Deallocate and destroy a parser.  Destructors are all called for
** all stack elements before shutting the parser down.
**
** Inputs:
** <ul>
** <li>  A pointer to the parser.  This should be a pointer
**       obtained from ParseAlloc.
** <li>  A pointer to a function used to reclaim memory obtained
**       from malloc.
** </ul>
*/
void ParseFree(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
  yyParser *pParser = (yyParser*)p;
  if( pParser==0 ) return;
  while( pParser->yyidx>=0 ) yy_pop_parser_stack(pParser);
#if YYSTACKDEPTH<=0
  free(pParser->yystack);
#endif
  (*freeProc)((void*)pParser);
}

/*
** Return the peak depth of the stack for a parser.
*/
#ifdef YYTRACKMAXSTACKDEPTH
int ParseStackPeak(void *p){
  yyParser *pParser = (yyParser*)p;
  return pParser->yyidxMax;
}
#endif

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_shift_action(
  yyParser *pParser,        /* The parser */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
  int stateno = pParser->yystack[pParser->yyidx].stateno;
 
  if( stateno>YY_SHIFT_MAX || (i = yy_shift_ofst[stateno])==YY_SHIFT_USE_DFLT ){
    return yy_default[stateno];
  }
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
  if( i<0 || i>=YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead ){
    if( iLookAhead>0 ){
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
        return yy_find_shift_action(pParser, iFallback);
      }
#endif
#ifdef YYWILDCARD
      {
        int j = i - iLookAhead + YYWILDCARD;
        if( j>=0 && j<YY_SZ_ACTTAB && yy_lookahead[j]==YYWILDCARD ){
#ifndef NDEBUG
          if( yyTraceFILE ){
            fprintf(yyTraceFILE, "%sWILDCARD %s => %s\n",
               yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[YYWILDCARD]);
          }
#endif /* NDEBUG */
          return yy_action[j];
        }
      }
#endif /* YYWILDCARD */
    }
    return yy_default[stateno];
  }else{
    return yy_action[i];
  }
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_reduce_action(
  int stateno,              /* Current state number */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
#ifdef YYERRORSYMBOL
  if( stateno>YY_REDUCE_MAX ){
    return yy_default[stateno];
  }
#else
  assert( stateno<=YY_REDUCE_MAX );
#endif
  i = yy_reduce_ofst[stateno];
  assert( i!=YY_REDUCE_USE_DFLT );
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
#ifdef YYERRORSYMBOL
  if( i<0 || i>=YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead ){
    return yy_default[stateno];
  }
#else
  assert( i>=0 && i<YY_SZ_ACTTAB );
  assert( yy_lookahead[i]==iLookAhead );
#endif
  return yy_action[i];
}

/*
** The following routine is called if the stack overflows.
*/
static void yyStackOverflow(yyParser *yypParser, YYMINORTYPE *yypMinor){
   ParseARG_FETCH;
   yypParser->yyidx--;
#ifndef NDEBUG
   if( yyTraceFILE ){
     fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
   }
#endif
   while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
   /* Here code is inserted which will execute if the parser
   ** stack every overflows */
   ParseARG_STORE; /* Suppress warning about unused %extra_argument var */
}

/*
** Perform a shift action.
*/
static void yy_shift(
  yyParser *yypParser,          /* The parser to be shifted */
  int yyNewState,               /* The new state to shift in */
  int yyMajor,                  /* The major token to shift in */
  YYMINORTYPE *yypMinor         /* Pointer to the minor token to shift in */
){
  yyStackEntry *yytos;
  yypParser->yyidx++;
#ifdef YYTRACKMAXSTACKDEPTH
  if( yypParser->yyidx>yypParser->yyidxMax ){
    yypParser->yyidxMax = yypParser->yyidx;
  }
#endif
#if YYSTACKDEPTH>0 
  if( yypParser->yyidx>=YYSTACKDEPTH ){
    yyStackOverflow(yypParser, yypMinor);
    return;
  }
#else
  if( yypParser->yyidx>=yypParser->yystksz ){
    yyGrowStack(yypParser);
    if( yypParser->yyidx>=yypParser->yystksz ){
      yyStackOverflow(yypParser, yypMinor);
      return;
    }
  }
#endif
  yytos = &yypParser->yystack[yypParser->yyidx];
  yytos->stateno = (YYACTIONTYPE)yyNewState;
  yytos->major = (YYCODETYPE)yyMajor;
  yytos->minor = *yypMinor;
#ifndef NDEBUG
  if( yyTraceFILE && yypParser->yyidx>0 ){
    int i;
    fprintf(yyTraceFILE,"%sShift %d\n",yyTracePrompt,yyNewState);
    fprintf(yyTraceFILE,"%sStack:",yyTracePrompt);
    for(i=1; i<=yypParser->yyidx; i++)
      fprintf(yyTraceFILE," %s",yyTokenName[yypParser->yystack[i].major]);
    fprintf(yyTraceFILE,"\n");
  }
#endif
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static const struct {
  YYCODETYPE lhs;         /* Symbol on the left-hand side of the rule */
  unsigned char nrhs;     /* Number of right-hand side symbols in the rule */
} yyRuleInfo[] = {
  { 44, 1 },
  { 43, 2 },
  { 43, 0 },
  { 42, 3 },
  { 42, 3 },
  { 42, 3 },
  { 42, 7 },
  { 42, 1 },
  { 45, 2 },
  { 45, 0 },
  { 46, 3 },
  { 46, 7 },
  { 46, 6 },
  { 46, 1 },
  { 46, 8 },
  { 46, 6 },
  { 46, 2 },
  { 54, 3 },
  { 54, 1 },
  { 54, 0 },
  { 55, 3 },
  { 56, 0 },
  { 56, 2 },
  { 47, 7 },
  { 47, 4 },
  { 49, 2 },
  { 49, 0 },
  { 48, 1 },
  { 48, 1 },
  { 48, 6 },
  { 48, 2 },
  { 48, 9 },
  { 48, 7 },
  { 48, 8 },
  { 57, 2 },
  { 57, 0 },
  { 61, 1 },
  { 61, 0 },
  { 50, 4 },
  { 51, 2 },
  { 51, 5 },
  { 51, 8 },
  { 60, 1 },
  { 60, 1 },
  { 53, 2 },
  { 53, 0 },
  { 52, 1 },
  { 52, 1 },
  { 52, 1 },
  { 62, 1 },
  { 62, 1 },
  { 62, 1 },
  { 62, 1 },
  { 62, 1 },
  { 62, 1 },
  { 62, 1 },
  { 62, 1 },
  { 59, 2 },
  { 59, 0 },
  { 58, 5 },
  { 58, 6 },
  { 63, 3 },
  { 63, 1 },
};

static void yy_accept(yyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void yy_reduce(
  yyParser *yypParser,         /* The parser */
  int yyruleno                 /* Number of the rule by which to reduce */
){
  int yygoto;                     /* The next state */
  int yyact;                      /* The next action */
  YYMINORTYPE yygotominor;        /* The LHS of the rule reduced */
  yyStackEntry *yymsp;            /* The top of the parser's stack */
  int yysize;                     /* Amount to pop the stack */
  ParseARG_FETCH;
  yymsp = &yypParser->yystack[yypParser->yyidx];
#ifndef NDEBUG
  if( yyTraceFILE && yyruleno>=0 
        && yyruleno<(int)(sizeof(yyRuleName)/sizeof(yyRuleName[0])) ){
    fprintf(yyTraceFILE, "%sReduce [%s].\n", yyTracePrompt,
      yyRuleName[yyruleno]);
  }
#endif /* NDEBUG */

  /* Silence complaints from purify about yygotominor being uninitialized
  ** in some cases when it is copied into the stack after the following
  ** switch.  yygotominor is uninitialized when a rule reduces that does
  ** not set the value of its left-hand side nonterminal.  Leaving the
  ** value of the nonterminal uninitialized is utterly harmless as long
  ** as the value is never used.  So really the only thing this code
  ** accomplishes is to quieten purify.  
  **
  ** 2007-01-16:  The wireshark project (www.wireshark.org) reports that
  ** without this code, their parser segfaults.  I'm not sure what there
  ** parser is doing to make this happen.  This is the second bug report
  ** from wireshark this week.  Clearly they are stressing Lemon in ways
  ** that it has not been previously stressed...  (SQLite ticket #2172)
  */
  /*memset(&yygotominor, 0, sizeof(yygotominor));*/
  yygotominor = yyzerominor;


  switch( yyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  #line <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  #line <lineno> <thisfile>
  **     break;
  */
      case 0: /* program ::= programlist */
#line 56 "zidl.y"
{
	context->result = yymsp[0].minor.yy9;
}
#line 829 "zidl.c"
        break;
      case 1: /* programlist ::= programlist programbody */
      case 8: /* namespacelist ::= namespacelist namespacebody */ yytestcase(yyruleno==8);
#line 60 "zidl.y"
{
	yygotominor.yy9 = yymsp[-1].minor.yy9;
	yymsp[-1].minor.yy9->add_child(yymsp[0].minor.yy9);
}
#line 838 "zidl.c"
        break;
      case 2: /* programlist ::= */
#line 65 "zidl.y"
{ 
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _ProgramList;
}
#line 846 "zidl.c"
        break;
      case 3: /* programbody ::= INCLUDE DQSTR LINEBREAK */
#line 70 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _Include;
	yygotominor.yy9->name = stripquotes(yymsp[-1].minor.yy0.tok);
}
#line 855 "zidl.c"
        break;
      case 4: /* programbody ::= CHEADER DQSTR LINEBREAK */
#line 76 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _Cheader;
	yygotominor.yy9->name = stripquotes(yymsp[-1].minor.yy0.tok);
}
#line 864 "zidl.c"
        break;
      case 5: /* programbody ::= IMPORT DQSTR LINEBREAK */
#line 82 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _Import;
	yygotominor.yy9->name = stripquotes(yymsp[-1].minor.yy0.tok);
	// derive import filename from context->scriptpath 
	node* zidlimport = zidl_parse_file(context->scriptpath + yygotominor.yy9->name);
	if (zidlimport == 0) {
		// programmatic syntax error
		printf("error importing %s\n", yygotominor.yy9->name.c_str());
		context->error = 1;
	} else {
		yygotominor.yy9->children = zidlimport->children;
		yygotominor.yy9->set_parent(yygotominor.yy9->children);
		zidlimport->children.clear();
		delete zidlimport;
	}
}
#line 885 "zidl.c"
        break;
      case 6: /* programbody ::= NAMESPACE IDENTIFIER COLON LINEBREAK INDENT namespacelist DEDENT */
#line 100 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _Namespace;
	yygotominor.yy9->name = yymsp[-5].minor.yy0.tok;
	yygotominor.yy9->children = yymsp[-1].minor.yy9->children;
	yygotominor.yy9->set_parent(yygotominor.yy9->children);
	yymsp[-1].minor.yy9->children.clear();
	delete yymsp[-1].minor.yy9;
}
#line 898 "zidl.c"
        break;
      case 7: /* programbody ::= DQSTR */
#line 110 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _String;
	yygotominor.yy9->name = stripquotes(yymsp[0].minor.yy0.tok);
}
#line 907 "zidl.c"
        break;
      case 9: /* namespacelist ::= */
#line 121 "zidl.y"
{
	yygotominor.yy9 = new node();
}
#line 914 "zidl.c"
        break;
      case 10: /* namespacebody ::= DLNAME DQSTR LINEBREAK */
#line 125 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _DlName;
	yygotominor.yy9->name = stripquotes(yymsp[-1].minor.yy0.tok);
}
#line 923 "zidl.c"
        break;
      case 11: /* namespacebody ::= ENUM IDENTIFIER COLON LINEBREAK INDENT enumlist DEDENT */
#line 131 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _Enum;
	yygotominor.yy9->name = yymsp[-5].minor.yy0.tok;
	yygotominor.yy9->children = yymsp[-1].minor.yy9->children;
	yygotominor.yy9->set_parent(yygotominor.yy9->children);
	yymsp[-1].minor.yy9->children.clear();
	delete yymsp[-1].minor.yy9;
}
#line 936 "zidl.c"
        break;
      case 12: /* namespacebody ::= ENUM COLON LINEBREAK INDENT enumlist DEDENT */
#line 141 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _Enum;
	yygotominor.yy9->children = yymsp[-1].minor.yy9->children;
	yygotominor.yy9->set_parent(yygotominor.yy9->children);
	yymsp[-1].minor.yy9->children.clear();
}
#line 947 "zidl.c"
        break;
      case 13: /* namespacebody ::= classdef */
      case 27: /* classbody ::= classdef */ yytestcase(yyruleno==27);
      case 28: /* classbody ::= memberbody */ yytestcase(yyruleno==28);
#line 149 "zidl.y"
{
	yygotominor.yy9 = yymsp[0].minor.yy9;
}
#line 956 "zidl.c"
        break;
      case 14: /* namespacebody ::= PDEF IDENTIFIER OPENPAREN argumentlist CLOSEPAREN COLON membertype LINEBREAK */
#line 153 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _CallbackType;
	yygotominor.yy9->name = yymsp[-6].minor.yy0.tok;
	yygotominor.yy9->metatype = yymsp[-1].minor.yy9;
	yygotominor.yy9->children = yymsp[-4].minor.yy9->children;
	yygotominor.yy9->set_parent(yygotominor.yy9->children);
	yymsp[-4].minor.yy9->children.clear();
	delete yymsp[-4].minor.yy9;
}
#line 970 "zidl.c"
        break;
      case 15: /* namespacebody ::= PDEF IDENTIFIER OPENPAREN argumentlist CLOSEPAREN LINEBREAK */
#line 164 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _CallbackType;
	yygotominor.yy9->name = yymsp[-4].minor.yy0.tok;
	yygotominor.yy9->children = yymsp[-2].minor.yy9->children;
	yygotominor.yy9->set_parent(yygotominor.yy9->children);
	yymsp[-2].minor.yy9->children.clear();
	delete yymsp[-2].minor.yy9;
}
#line 983 "zidl.c"
        break;
      case 16: /* namespacebody ::= DQSTR LINEBREAK */
      case 30: /* classbody ::= DQSTR LINEBREAK */ yytestcase(yyruleno==30);
#line 174 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _String;
	yygotominor.yy9->name = stripquotes(yymsp[-1].minor.yy0.tok);
}
#line 993 "zidl.c"
        break;
      case 17: /* argumentlist ::= argumentlist COMMA argumentbody */
#line 180 "zidl.y"
{
	yygotominor.yy9 = yymsp[-2].minor.yy9;
	yygotominor.yy9->add_child(yymsp[0].minor.yy9);
}
#line 1001 "zidl.c"
        break;
      case 18: /* argumentlist ::= argumentbody */
#line 185 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _ArgumentList;
	yygotominor.yy9->add_child(yymsp[0].minor.yy9);
}
#line 1010 "zidl.c"
        break;
      case 19: /* argumentlist ::= */
#line 191 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _ArgumentList;
}
#line 1018 "zidl.c"
        break;
      case 20: /* argumentbody ::= membertype IDENTIFIER argumentdefault */
#line 196 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _Argument;
	yygotominor.yy9->name = yymsp[-1].minor.yy0.tok;
	yygotominor.yy9->metatype = yymsp[-2].minor.yy9;
	if (yymsp[0].minor.yy9 != 0) yygotominor.yy9->modifiers.push_back(yymsp[0].minor.yy9);
}
#line 1029 "zidl.c"
        break;
      case 21: /* argumentdefault ::= */
#line 204 "zidl.y"
{ 
	/* OK */
}
#line 1036 "zidl.c"
        break;
      case 22: /* argumentdefault ::= EQ DIGITS */
#line 208 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _EnumValue;
	yygotominor.yy9->intvalue = atoi(yymsp[0].minor.yy0.tok);
}
#line 1045 "zidl.c"
        break;
      case 23: /* classdef ::= CLASS IDENTIFIER COLON LINEBREAK INDENT classlist DEDENT */
#line 214 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _Class;
	yygotominor.yy9->name = yymsp[-5].minor.yy0.tok;
	yygotominor.yy9->children = yymsp[-1].minor.yy9->children;
	yygotominor.yy9->set_parent(yygotominor.yy9->children);
	yymsp[-1].minor.yy9->children.clear();
	delete yymsp[-1].minor.yy9;
}
#line 1058 "zidl.c"
        break;
      case 24: /* classdef ::= CLASS IDENTIFIER COLON LINEBREAK */
#line 224 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _Class;
	yygotominor.yy9->name = yymsp[-2].minor.yy0.tok;
}
#line 1067 "zidl.c"
        break;
      case 25: /* classlist ::= classlist classbody */
      case 44: /* typemodifierlist ::= typemodifierlist typemodifier */ yytestcase(yyruleno==44);
      case 57: /* enumlist ::= enumlist enumbody */ yytestcase(yyruleno==57);
#line 230 "zidl.y"
{
	yygotominor.yy9 = yymsp[-1].minor.yy9;
	yygotominor.yy9->add_child(yymsp[0].minor.yy9);
}
#line 1077 "zidl.c"
        break;
      case 26: /* classlist ::= */
#line 235 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _ClassList;
}
#line 1085 "zidl.c"
        break;
      case 29: /* classbody ::= UNION COLON LINEBREAK INDENT unionlist DEDENT */
#line 248 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _Union;
	static int unioncounter = 0;
	std::stringstream idstrm;
	idstrm << "union_" << std::hex << std::setw(8) << std::setfill('0') << unioncounter;
	yygotominor.yy9->name = idstrm.str();
	unioncounter++;
	yygotominor.yy9->children = yymsp[-1].minor.yy9->children;
	yygotominor.yy9->set_parent(yygotominor.yy9->children);
	yymsp[-1].minor.yy9->children.clear();
	delete yymsp[-1].minor.yy9;
}
#line 1102 "zidl.c"
        break;
      case 31: /* classbody ::= DEF methodmodifier IDENTIFIER OPENPAREN argumentlist CLOSEPAREN COLON membertype LINEBREAK */
#line 268 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _Method;
	yygotominor.yy9->name = yymsp[-6].minor.yy0.tok;
	yygotominor.yy9->metatype = yymsp[-1].minor.yy9;
	yygotominor.yy9->children = yymsp[-4].minor.yy9->children;
	yygotominor.yy9->set_parent(yygotominor.yy9->children);
	yymsp[-4].minor.yy9->children.clear();
	delete yymsp[-4].minor.yy9;
	if (yymsp[-7].minor.yy9 != 0)
		yygotominor.yy9->modifiers.push_back(yymsp[-7].minor.yy9);
}
#line 1118 "zidl.c"
        break;
      case 32: /* classbody ::= DEF methodmodifier IDENTIFIER OPENPAREN argumentlist CLOSEPAREN LINEBREAK */
#line 281 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _Method;
	yygotominor.yy9->name = yymsp[-4].minor.yy0.tok;
	yygotominor.yy9->children = yymsp[-2].minor.yy9->children;
	yygotominor.yy9->set_parent(yygotominor.yy9->children);
	yymsp[-2].minor.yy9->children.clear();
	delete yymsp[-2].minor.yy9;
	if (yymsp[-5].minor.yy9 != 0)
		yygotominor.yy9->modifiers.push_back(yymsp[-5].minor.yy9);
}
#line 1133 "zidl.c"
        break;
      case 33: /* classbody ::= ITERATOR IDENTIFIER COLON FOR IDENTIFIER IN IDENTIFIER LINEBREAK */
#line 293 "zidl.y"
{
	/* ignore iterators */
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _Ignore;
}
#line 1142 "zidl.c"
        break;
      case 34: /* unionlist ::= unionlist memberbody */
#line 299 "zidl.y"
{
	yygotominor.yy9 = yymsp[-1].minor.yy9;
	yygotominor.yy9->add_child(yymsp[0].minor.yy9);	
}
#line 1150 "zidl.c"
        break;
      case 35: /* unionlist ::= */
#line 304 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _UnionList;
}
#line 1158 "zidl.c"
        break;
      case 36: /* methodmodifier ::= STATIC */
#line 309 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _TypeModifier;
	yygotominor.yy9->name = "static";
}
#line 1167 "zidl.c"
        break;
      case 37: /* methodmodifier ::= */
#line 315 "zidl.y"
{
	/* OK */
}
#line 1174 "zidl.c"
        break;
      case 38: /* memberbody ::= MEMBER membertype IDENTIFIER LINEBREAK */
#line 319 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _Member;
	yygotominor.yy9->name = yymsp[-1].minor.yy0.tok;
	yygotominor.yy9->metatype = yymsp[-2].minor.yy9;
}
#line 1184 "zidl.c"
        break;
      case 39: /* membertype ::= typemodifierlist typename */
#line 326 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _Type;
	yygotominor.yy9->name = yymsp[0].minor.yy0.tok;
	yygotominor.yy9->modifiers = yymsp[-1].minor.yy9->children;
	yygotominor.yy9->set_parent(yygotominor.yy9->children);
	yymsp[-1].minor.yy9->children.clear();
	delete yymsp[-1].minor.yy9;
}
#line 1197 "zidl.c"
        break;
      case 40: /* membertype ::= typemodifierlist typename OPENSQRPAREN arrayindexer CLOSESQRPAREN */
#line 336 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _Type;
	yygotominor.yy9->name = yymsp[-3].minor.yy0.tok;
	yygotominor.yy9->modifiers = yymsp[-4].minor.yy9->children;
	yygotominor.yy9->set_parent(yygotominor.yy9->modifiers);
	yygotominor.yy9->add_child(yymsp[-1].minor.yy9);
	yymsp[-4].minor.yy9->children.clear();
	delete yymsp[-4].minor.yy9;
}
#line 1211 "zidl.c"
        break;
      case 41: /* membertype ::= typemodifierlist typename OPENSQRPAREN arrayindexer CLOSESQRPAREN OPENSQRPAREN arrayindexer CLOSESQRPAREN */
#line 347 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _Type;
	yygotominor.yy9->name = yymsp[-6].minor.yy0.tok;
	yygotominor.yy9->modifiers = yymsp[-7].minor.yy9->children;
	yygotominor.yy9->set_parent(yygotominor.yy9->modifiers);
	yygotominor.yy9->add_child(yymsp[-4].minor.yy9);
	yygotominor.yy9->add_child(yymsp[-1].minor.yy9);
	yymsp[-7].minor.yy9->children.clear();
	delete yymsp[-7].minor.yy9;
}
#line 1226 "zidl.c"
        break;
      case 42: /* arrayindexer ::= IDENTIFIER */
#line 359 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _Indexer;
	yygotominor.yy9->name = yymsp[0].minor.yy0.tok;
	yygotominor.yy9->intvalue = 0;
}
#line 1236 "zidl.c"
        break;
      case 43: /* arrayindexer ::= DIGITS */
#line 366 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _Indexer;
	yygotominor.yy9->name = yymsp[0].minor.yy0.tok;
	yygotominor.yy9->intvalue = atoi(yymsp[0].minor.yy0.tok);
}
#line 1246 "zidl.c"
        break;
      case 45: /* typemodifierlist ::= */
#line 378 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _TypeModifierList;
}
#line 1254 "zidl.c"
        break;
      case 46: /* typemodifier ::= OUT */
#line 383 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _TypeModifier;
	yygotominor.yy9->name = "out";
}
#line 1263 "zidl.c"
        break;
      case 47: /* typemodifier ::= NOREF */
#line 389 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _TypeModifier;
	yygotominor.yy9->name = "noref";
}
#line 1272 "zidl.c"
        break;
      case 48: /* typemodifier ::= NO_PYTHON */
#line 395 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _TypeModifier;
	yygotominor.yy9->name = "no_python";
}
#line 1281 "zidl.c"
        break;
      case 49: /* typename ::= IDENTIFIER */
      case 50: /* typename ::= INT */ yytestcase(yyruleno==50);
      case 51: /* typename ::= UINT */ yytestcase(yyruleno==51);
      case 52: /* typename ::= FLOAT */ yytestcase(yyruleno==52);
      case 53: /* typename ::= STRING */ yytestcase(yyruleno==53);
      case 54: /* typename ::= CHAR */ yytestcase(yyruleno==54);
      case 55: /* typename ::= PVOID */ yytestcase(yyruleno==55);
      case 56: /* typename ::= BOOL */ yytestcase(yyruleno==56);
#line 401 "zidl.y"
{
	yygotominor.yy0 = yymsp[0].minor.yy0;
}
#line 1295 "zidl.c"
        break;
      case 58: /* enumlist ::= */
#line 438 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _EnumList;
}
#line 1303 "zidl.c"
        break;
      case 59: /* enumbody ::= SET IDENTIFIER EQ DIGITS LINEBREAK */
#line 443 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _EnumValue;
	yygotominor.yy9->name = yymsp[-3].minor.yy0.tok;
	yygotominor.yy9->intvalue = atoi(yymsp[-1].minor.yy0.tok);
}
#line 1313 "zidl.c"
        break;
      case 60: /* enumbody ::= SET IDENTIFIER EQ BIT bitvalues LINEBREAK */
#line 450 "zidl.y"
{
	yygotominor.yy9 = new node();
	yygotominor.yy9->type = _EnumValue;
	yygotominor.yy9->name = yymsp[-4].minor.yy0.tok;
	yygotominor.yy9->intvalue = yymsp[-1].minor.yy0.tokval;
}
#line 1323 "zidl.c"
        break;
      case 61: /* bitvalues ::= bitvalues COMMA DIGITS */
#line 457 "zidl.y"
{
	yygotominor.yy0.tokval = yymsp[-2].minor.yy0.tokval | (1 << atoi(yymsp[0].minor.yy0.tok));
}
#line 1330 "zidl.c"
        break;
      case 62: /* bitvalues ::= DIGITS */
#line 461 "zidl.y"
{
	yygotominor.yy0.tokval = 1 << atoi(yymsp[0].minor.yy0.tok);
}
#line 1337 "zidl.c"
        break;
      default:
        break;
  };
  yygoto = yyRuleInfo[yyruleno].lhs;
  yysize = yyRuleInfo[yyruleno].nrhs;
  yypParser->yyidx -= yysize;
  yyact = yy_find_reduce_action(yymsp[-yysize].stateno,(YYCODETYPE)yygoto);
  if( yyact < YYNSTATE ){
#ifdef NDEBUG
    /* If we are not debugging and the reduce action popped at least
    ** one element off the stack, then we can push the new element back
    ** onto the stack here, and skip the stack overflow test in yy_shift().
    ** That gives a significant speed improvement. */
    if( yysize ){
      yypParser->yyidx++;
      yymsp -= yysize-1;
      yymsp->stateno = (YYACTIONTYPE)yyact;
      yymsp->major = (YYCODETYPE)yygoto;
      yymsp->minor = yygotominor;
    }else
#endif
    {
      yy_shift(yypParser,yyact,yygoto,&yygotominor);
    }
  }else{
    assert( yyact == YYNSTATE + YYNRULE + 1 );
    yy_accept(yypParser);
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
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}
#endif /* YYNOERRORRECOVERY */

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  int yymajor,                   /* The major type of the error token */
  YYMINORTYPE yyminor            /* The minor type of the error token */
){
  ParseARG_FETCH;
#define TOKEN (yyminor.yy0)
#line 20 "zidl.y"

	context->error = 1;
#line 1402 "zidl.c"
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
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
#line 24 "zidl.y"

	/* printf("parsing complete!\n"); */
#line 1424 "zidl.c"
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
  int yyact;            /* The parser action. */
  int yyendofinput;     /* True if we are at the end of input */
#ifdef YYERRORSYMBOL
  int yyerrorhit = 0;   /* True if yymajor has invoked an error */
#endif
  yyParser *yypParser;  /* The parser */

  /* (re)initialize the parser, if necessary */
  yypParser = (yyParser*)yyp;
  if( yypParser->yyidx<0 ){
#if YYSTACKDEPTH<=0
    if( yypParser->yystksz <=0 ){
      /*memset(&yyminorunion, 0, sizeof(yyminorunion));*/
      yyminorunion = yyzerominor;
      yyStackOverflow(yypParser, &yyminorunion);
      return;
    }
#endif
    yypParser->yyidx = 0;
    yypParser->yyerrcnt = -1;
    yypParser->yystack[0].stateno = 0;
    yypParser->yystack[0].major = 0;
  }
  yyminorunion.yy0 = yyminor;
  yyendofinput = (yymajor==0);
  ParseARG_STORE;

#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sInput %s\n",yyTracePrompt,yyTokenName[yymajor]);
  }
#endif

  do{
    yyact = yy_find_shift_action(yypParser,(YYCODETYPE)yymajor);
    if( yyact<YYNSTATE ){
      assert( !yyendofinput );  /* Impossible to shift the $ token */
      yy_shift(yypParser,yyact,yymajor,&yyminorunion);
      yypParser->yyerrcnt--;
      yymajor = YYNOCODE;
    }else if( yyact < YYNSTATE + YYNRULE ){
      yy_reduce(yypParser,yyact-YYNSTATE);
    }else{
      assert( yyact == YY_ERROR_ACTION );
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
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yymx = yypParser->yystack[yypParser->yyidx].major;
      if( yymx==YYERRORSYMBOL || yyerrorhit ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE,"%sDiscard input token %s\n",
             yyTracePrompt,yyTokenName[yymajor]);
        }
#endif
        yy_destructor(yypParser, (YYCODETYPE)yymajor,&yyminorunion);
        yymajor = YYNOCODE;
      }else{
         while(
          yypParser->yyidx >= 0 &&
          yymx != YYERRORSYMBOL &&
          (yyact = yy_find_reduce_action(
                        yypParser->yystack[yypParser->yyidx].stateno,
                        YYERRORSYMBOL)) >= YYNSTATE
        ){
          yy_pop_parser_stack(yypParser);
        }
        if( yypParser->yyidx < 0 || yymajor==0 ){
          yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
          yy_parse_failed(yypParser);
          yymajor = YYNOCODE;
        }else if( yymx!=YYERRORSYMBOL ){
          YYMINORTYPE u2;
          u2.YYERRSYMDT = 0;
          yy_shift(yypParser,yyact,YYERRORSYMBOL,&u2);
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
      yy_syntax_error(yypParser,yymajor,yyminorunion);
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
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yypParser->yyerrcnt = 3;
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      if( yyendofinput ){
        yy_parse_failed(yypParser);
      }
      yymajor = YYNOCODE;
#endif
    }
  }while( yymajor!=YYNOCODE && yypParser->yyidx>=0 );
  return;
}
