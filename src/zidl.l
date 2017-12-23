/*

	ZIDL lexer for re2c lexer generator

*/

#define YYCTYPE char
#define YYCURSOR s->cursor()
#define YYLIMIT s->limit()
#define YYMARKER q
#define YYFILL(n) s->fill(n)

int scan(scanner* s) {
	int depth;
	char* q;

	if (s->bufferline == s->cursor()) goto indent;

ignoretoken:
	s->token = s->cursor();

/*!re2c

all			= [\000-\377];
eof			= [\000];
any			= all\eof;

digits		= [0-9]+;
dqstr		= ["] ((any\["\n])|([\\]["]))* ["];
identifier	= [a-zA-Z_][a-zA-Z_0-9\.]*;
linebreak	= ("\r" "\n")|("\n");
whitespace	= [ \t]+;
comment		= "#" (any\[\n\r])*;

"cheader"		{ return CHEADER; }
"include"		{ return INCLUDE; }
"namespace"		{ return NAMESPACE; }
"import"		{ return IMPORT; }
"enum"			{ return ENUM; }
"class"			{ return CLASS; }
"union"			{ return UNION; }
"member"		{ return MEMBER; }
"noref"			{ return NOREF; }
"set"			{ return SET; }
"bit"			{ return BIT; }

"int"			{ return INT; }
"uint"			{ return UINT; }
"bool"			{ return BOOL; }
"pvoid"			{ return PVOID; }
"float"			{ return FLOAT; }
"string"		{ return STRING; }
"char"			{ return CHAR; }

"out"			{ return OUT; }
"pdef"			{ return PDEF; }
"def"			{ return DEF; }
"iterator"		{ return ITERATOR; }
"for"			{ return FOR; }
"in"			{ return IN; }
"static"		{ return STATIC; }
"no_python"		{ return NO_PYTHON; }
"dlname"		{ return DLNAME; }
":"				{ return COLON; }
"("				{ return OPENPAREN; }
")"				{ return CLOSEPAREN; }
"["				{ return OPENSQRPAREN; }
"]"				{ return CLOSESQRPAREN; }
","				{ return COMMA; }
"="				{ return EQ; }
dqstr			{ return DQSTR; }
digits			{ return DIGITS; }
linebreak		{ 
	s->bufferline = s->cursor(); 
	s->linecounter++;
	if (s->linetokens == 0) 
		goto indent;
	s->linetokens = 0; 
	return LINEBREAK; 
}
identifier		{ return IDENTIFIER; }
whitespace		{ goto ignoretoken; }
comment			{ goto ignoretoken; }
eof				{ goto indent; }
any				{ return -1; }

*/


indent:
	s->token = s->cursor();

/*!re2c

whitespace comment linebreak {
	s->bufferline = s->cursor();
	s->linecounter++;
	goto indent;
}

comment linebreak {
	s->bufferline = s->cursor();
	s->linecounter++;
	goto indent;
}

whitespace linebreak		{
	s->bufferline = s->cursor();
	s->linecounter++;
	goto indent;
}

linebreak {
	s->bufferline = s->cursor();
	s->linecounter++;
	goto indent;
}

whitespace []		{
		depth = s->get_token_length();
		if (depth > s->indentstack.top()) {
			s->indentstack.push(depth);
			return INDENT;
		}
		if (depth < s->indentstack.top()) {
			s->cursor() = s->token; // retry this token until we have matching indent depths
			s->indentstack.pop();
			return DEDENT;
		}
		goto ignoretoken;
	}
eof {
		if (s->indentstack.top() > 0) {
			s->indentstack.pop();
			s->cursor() = s->token; // retry this token until we have matching indent depths
			return DEDENT;
		}
		return 0;
	}
[] {
		if (s->indentstack.top() > 0) {
			s->indentstack.pop();
			s->cursor() = s->token; // retry this token until we have matching indent depths
			return DEDENT;
		}
		goto ignoretoken;
	}
*/
}