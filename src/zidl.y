/*

	Zzub IDL grammar for Lemon parser generator

*/

%include {
#include <vector>
#include <stack>
#include <istream>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <cstdlib>
#include "zidlparser.h"
}

%extra_argument { parsecontext* context }

%syntax_error {
	context->error = 1;
}

%parse_accept {
	/* printf("parsing complete!\n"); */
}

/*

	Grammar starts here

*/

%token_type { token }
%type programbody { node* }
%type programlist { node* }
%type program { node* }
%type namespacelist { node* }
%type namespacebody { node* }
%type classdef { node* }
%type classbody { node* }
%type classlist { node* }
%type memberbody { node* }
%type membertype { node* }
%type typemodifier { node* }
%type typemodifierlist { node* }
%type argumentlist { node* }
%type argumentbody { node* }
%type argumentdefault { node* }
%type unionlist { node* }
%type enumbody { node* }
%type enumlist { node* }
%type arrayindexer { node* }
%type methodmodifier { node* }

program ::= programlist(B) . {
	context->result = B;
}

programlist(A) ::= programlist(B) programbody(C) . {
	A = B;
	B->add_child(C);
}

programlist(A) ::= . { 
	A = new node();
	A->type = _ProgramList;
}

programbody(A) ::= INCLUDE DQSTR(B) LINEBREAK . {
	A = new node();
	A->type = _Include;
	A->name = stripquotes(B.tok);
}

programbody(A) ::= CHEADER DQSTR(B) LINEBREAK . {
	A = new node();
	A->type = _Cheader;
	A->name = stripquotes(B.tok);
}

programbody(A) ::= IMPORT DQSTR(B) LINEBREAK . {
	A = new node();
	A->type = _Import;
	A->name = stripquotes(B.tok);
	// derive import filename from context->scriptpath 
	node* zidlimport = zidl_parse_file(context->scriptpath + A->name);
	if (zidlimport == 0) {
		// programmatic syntax error
		printf("error importing %s\n", A->name.c_str());
		context->error = 1;
	} else {
		A->children = zidlimport->children;
		A->set_parent(A->children);
		zidlimport->children.clear();
		delete zidlimport;
	}
}

programbody(A) ::= NAMESPACE IDENTIFIER(B) COLON LINEBREAK INDENT namespacelist(C) DEDENT . {
	A = new node();
	A->type = _Namespace;
	A->name = B.tok;
	A->children = C->children;
	A->set_parent(A->children);
	C->children.clear();
	delete C;
}

programbody(A) ::= DQSTR(B) . {
	A = new node();
	A->type = _String;
	A->name = stripquotes(B.tok);
}

namespacelist(A) ::= namespacelist(B) namespacebody(C) . {
	A = B;
	B->add_child(C);
}

namespacelist(A) ::= . {
	A = new node();
}

namespacebody(A) ::= DLNAME DQSTR(B) LINEBREAK . {
	A = new node();
	A->type = _DlName;
	A->name = stripquotes(B.tok);
}

namespacebody(A) ::= ENUM IDENTIFIER(B) COLON LINEBREAK INDENT enumlist(C) DEDENT . {
	A = new node();
	A->type = _Enum;
	A->name = B.tok;
	A->children = C->children;
	A->set_parent(A->children);
	C->children.clear();
	delete C;
}

namespacebody(A) ::= ENUM COLON LINEBREAK INDENT enumlist(B) DEDENT . {
	A = new node();
	A->type = _Enum;
	A->children = B->children;
	A->set_parent(A->children);
	B->children.clear();
}

namespacebody(A) ::= classdef(B) . {
	A = B;
}

namespacebody(A) ::= PDEF IDENTIFIER(B) OPENPAREN argumentlist(C) CLOSEPAREN COLON membertype(D) LINEBREAK . {
	A = new node();
	A->type = _CallbackType;
	A->name = B.tok;
	A->metatype = D;
	A->children = C->children;
	A->set_parent(A->children);
	C->children.clear();
	delete C;
}

namespacebody(A) ::= PDEF IDENTIFIER(B) OPENPAREN argumentlist(C) CLOSEPAREN LINEBREAK . {
	A = new node();
	A->type = _CallbackType;
	A->name = B.tok;
	A->children = C->children;
	A->set_parent(A->children);
	C->children.clear();
	delete C;
}

namespacebody(A) ::= DQSTR(B) LINEBREAK . {
	A = new node();
	A->type = _String;
	A->name = stripquotes(B.tok);
}

argumentlist(A) ::= argumentlist(B) COMMA argumentbody(C) . {
	A = B;
	A->add_child(C);
}

argumentlist(A) ::= argumentbody(B) . {
	A = new node();
	A->type = _ArgumentList;
	A->add_child(B);
}

argumentlist(A) ::= . {
	A = new node();
	A->type = _ArgumentList;
}

argumentbody(A) ::= membertype(B) IDENTIFIER(C) argumentdefault(D) . {
	A = new node();
	A->type = _Argument;
	A->name = C.tok;
	A->metatype = B;
	if (D != 0) A->modifiers.push_back(D);
}

argumentdefault(A) ::= . { 
	A = 0;
}

argumentdefault(A) ::= EQ DIGITS(B) . {
	A = new node();
	A->type = _EnumValue;
	A->intvalue = atoi(B.tok);
}

classdef(A) ::= CLASS IDENTIFIER(B) COLON LINEBREAK INDENT classlist(C) DEDENT . {
	A = new node();
	A->type = _Class;
	A->name = B.tok;
	A->children = C->children;
	A->set_parent(A->children);
	C->children.clear();
	delete C;
}

classdef(A) ::= CLASS IDENTIFIER(B) COLON LINEBREAK . {
	A = new node();
	A->type = _Class;
	A->name = B.tok;
}

classlist(A) ::= classlist(B) classbody(C) . {
	A = B;
	A->add_child(C);
}

classlist(A) ::= . {
	A = new node();
	A->type = _ClassList;
}

classbody(A) ::= classdef(B) . {
	A = B;
}

classbody(A) ::= memberbody(B) . {
	A = B;
}

classbody(A) ::= UNION COLON LINEBREAK INDENT unionlist(B) DEDENT . {
	A = new node();
	A->type = _Union;
	static int unioncounter = 0;
	std::stringstream idstrm;
	idstrm << "union_" << std::hex << std::setw(8) << std::setfill('0') << unioncounter;
	A->name = idstrm.str();
	unioncounter++;
	A->children = B->children;
	A->set_parent(A->children);
	B->children.clear();
	delete B;
}

classbody(A) ::= DQSTR(B) LINEBREAK . {
	A = new node();
	A->type = _String;
	A->name = stripquotes(B.tok);
}

classbody(A) ::= DEF methodmodifier(B) IDENTIFIER(C) OPENPAREN argumentlist(D) CLOSEPAREN COLON membertype(E) LINEBREAK . {
	A = new node();
	A->type = _Method;
	A->name = C.tok;
	A->metatype = E;
	A->children = D->children;
	A->set_parent(A->children);
	D->children.clear();
	delete D;
	if (B != 0)
		A->modifiers.push_back(B);
}

classbody(A) ::= DEF methodmodifier(B) IDENTIFIER(C) OPENPAREN argumentlist(D) CLOSEPAREN LINEBREAK . {
	A = new node();
	A->type = _Method;
	A->name = C.tok;
	A->children = D->children;
	A->set_parent(A->children);
	D->children.clear();
	delete D;
	if (B != 0)
		A->modifiers.push_back(B);
}

classbody(A) ::= ITERATOR IDENTIFIER COLON FOR IDENTIFIER IN IDENTIFIER LINEBREAK . {
	/* ignore iterators */
	A = new node();
	A->type = _Ignore;
}

unionlist(A) ::= unionlist(B) memberbody(C) . {
	A = B;
	A->add_child(C);	
}

unionlist(A) ::= . {
	A = new node();
	A->type = _UnionList;
}

methodmodifier(A) ::= STATIC . {
	A = new node();
	A->type = _TypeModifier;
	A->name = "static";
}

methodmodifier(A) ::= . {
	A = 0;
}

memberbody(A) ::= MEMBER membertype(B) IDENTIFIER(C) LINEBREAK . {
	A = new node();
	A->type = _Member;
	A->name = C.tok;
	A->metatype = B;
}

membertype(A) ::= typemodifierlist(B) typename(C) . {
	A = new node();
	A->type = _Type;
	A->name = C.tok;
	A->modifiers = B->children;
	A->set_parent(A->children);
	B->children.clear();
	delete B;
}

membertype(A) ::= typemodifierlist(B) typename(C) OPENSQRPAREN arrayindexer(D) CLOSESQRPAREN . {
	A = new node();
	A->type = _Type;
	A->name = C.tok;
	A->modifiers = B->children;
	A->set_parent(A->modifiers);
	A->add_child(D);
	B->children.clear();
	delete B;
}

membertype(A) ::= typemodifierlist(B) typename(C) OPENSQRPAREN arrayindexer(D) CLOSESQRPAREN OPENSQRPAREN arrayindexer(E) CLOSESQRPAREN . {
	A = new node();
	A->type = _Type;
	A->name = C.tok;
	A->modifiers = B->children;
	A->set_parent(A->modifiers);
	A->add_child(D);
	A->add_child(E);
	B->children.clear();
	delete B;
}

arrayindexer(A) ::= IDENTIFIER(B) . {
	A = new node();
	A->type = _Indexer;
	A->name = B.tok;
	A->intvalue = 0;
}

arrayindexer(A) ::= DIGITS(B) . {
	A = new node();
	A->type = _Indexer;
	A->name = B.tok;
	A->intvalue = atoi(B.tok);
}

typemodifierlist(A) ::= typemodifierlist(B) typemodifier(C) . {
	A = B;
	A->add_child(C);
}

typemodifierlist(A) ::= . {
	A = new node();
	A->type = _TypeModifierList;
}

typemodifier(A) ::= OUT . {
	A = new node();
	A->type = _TypeModifier;
	A->name = "out";
}

typemodifier(A) ::= NOREF . {
	A = new node();
	A->type = _TypeModifier;
	A->name = "noref";
}

typemodifier(A) ::= NO_PYTHON . {
	A = new node();
	A->type = _TypeModifier;
	A->name = "no_python";
}

typename(A) ::= IDENTIFIER(B) . {
	A = B;
}

typename(A) ::= INT(B) . {
	A = B;
}

typename(A) ::= UINT(B) . {
	A = B;
}

typename(A) ::= FLOAT(B) . {
	A = B;
}

typename(A) ::= STRING(B) . {
	A = B;
}

typename(A) ::= CHAR(B) . {
	A = B;
}

typename(A) ::= PVOID(B) . {
	A = B;
}

typename(A) ::= BOOL(B) . {
	A = B;
}

enumlist(A) ::= enumlist(B) enumbody(C) . {
	A = B;
	A->add_child(C);
}

enumlist(A) ::= . {
	A = new node();
	A->type = _EnumList;
}

enumbody(A) ::= SET IDENTIFIER(B) EQ DIGITS(C) LINEBREAK . {
	A = new node();
	A->type = _EnumValue;
	A->name = B.tok;
	A->intvalue = atoi(C.tok);
}

enumbody(A) ::= SET IDENTIFIER(B) EQ BIT bitvalues(C) LINEBREAK . {
	A = new node();
	A->type = _EnumValue;
	A->name = B.tok;
	A->intvalue = C.tokval;
}

bitvalues(A) ::= bitvalues(B) COMMA DIGITS(C) . {
	A.tokval = B.tokval | (1 << atoi(C.tok));
}

bitvalues(A) ::= DIGITS(B) . {
	A.tokval = 1 << atoi(B.tok);
}
