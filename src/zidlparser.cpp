#include <sstream>
#include <fstream>
#include <cassert>
#include <stack>
#include <vector>
#include <algorithm>
#include <functional>
#include <cctype>
#include <iostream>
#include "zidlparser.h"
#include "zidl.h"
#include "zidl_l.h"

using std::endl;

extern void *ParseAlloc(void *(*mallocProc)(size_t));
extern void Parse(void *yyp, int yymajor, token, parsecontext*);
extern void ParseFree(void *p, void (*freeProc)(void*));

std::string stripquotes(const std::string& str) {
	return str.substr(1, str.length() - 2);
}

// "CamelCase" -> "camel_case"
std::string camel_to_posix(std::string camelstr) {
	std::stringstream result;
	for (std::string::iterator i = camelstr.begin(); i != camelstr.end(); ++i) {
		if (isupper(*i)) {
			if (i != camelstr.begin())
				result << "_";
			result << (std::string::value_type)tolower(*i);
		} else
			result << *i;
	}
	return result.str();
}

std::string camel_to_posix(const std::string& camelstr1, const std::string& camelstr2) {
	std::stringstream strm;
	strm << camel_to_posix(camelstr1) << "_" << camel_to_posix(camelstr2);
	return strm.str();
}

std::string camel_to_posix(const std::string& camelstr1, const std::string& camelstr2, const std::string& camelstr3) {
	std::stringstream strm;
	strm << camel_to_posix(camelstr1) << "_" << camel_to_posix(camelstr2) << "_" << camel_to_posix(camelstr3);
	return strm.str();
}

// "posix_to_camel" -> "PosixToCamel"
std::string posix_to_camel(std::string str) {
	std::stringstream result;
	bool upnext = true;
	for (std::string::iterator i = str.begin(); i != str.end(); ++i) {
		if (*i == '_') {
			upnext = true;
		} else {
			if (upnext)
				result << (std::string::value_type)toupper(*i);
			else
				result << *i;
			upnext = false;
		}
	}
	return result.str();
}

void zidl_import_namespace_nodes(node* ns, node* impns) {
	for (std::vector<node*>::iterator i = impns->children.begin(); i != impns->children.end(); ) {
		switch ((*i)->type ) {
			case _Enum:
			case _Class:
			case _CallbackType:
				ns->add_child(*i);
				i = impns->children.erase(i);
				break;
			default:
				++i;
				break;
		}
	}
}

void zidl_merge_namespace_imports(node* program) {
	node* nsnode = 0;
	for (std::vector<node*>::iterator i = program->children.begin(); i != program->children.end(); ++i) {
		if ((*i)->type == _Namespace) {
			nsnode = *i;
			break;
		}
	}

	if (nsnode == 0) return ; // error

	for (std::vector<node*>::iterator i = program->children.begin(); i != program->children.end(); ) {
		if ((*i)->type == _Import) {
			node* importnsnode = (*i)->find_child(_Namespace, nsnode->name);
			if (importnsnode != 0) {
				// copy worthwhile children from the importnsnode into the nsnode and erase the importnsnode
				zidl_import_namespace_nodes(nsnode, importnsnode);
				delete *i;
				i = program->children.erase(i);
			} else {
				++i;
			}
		} else {
			++i;
		}
	}
}

node* zidl_parse_stream(std::istream& strm, int* errorline, std::string scriptpath) {
	token t;
	parsecontext ctx;
	ctx.scriptpath = scriptpath;
	scanner s(strm);
	void* parser = ParseAlloc(malloc);
	ctx.error = 0;
	s.linecounter = 0;
	for (; ctx.error == 0; ) {
		int tok = scan(&s);
		if (tok != LINEBREAK && tok != DEDENT && tok != INDENT) s.linetokens++;
		memset(&t, 0, sizeof(t));
		t.tokval = tok;
		s.extract_token(t.tok);
		if (tok == 0) {
			Parse(parser, 0, t, &ctx); // finalize
			break;
		}
		if (tok == -1) break;
		Parse(parser, tok, t, &ctx);
	}

	ParseFree(parser, free);

	if (ctx.error || t.tokval == -1) {
		if (errorline)
			*errorline = s.linecounter;
		//cout << "Parser context reported error @ " << s.linecounter << endl;
		return 0;
	}

	// merge content from imports into the main namespace
	zidl_merge_namespace_imports(ctx.result);
	return ctx.result;
}

node* zidl_parse_file(const std::string& filename, int* errorline) {
	std::ifstream fs;
	fs.open(filename.c_str(), std::ios::binary | std::ios::in);
	if (!fs) return 0;

	std::string::size_type ls = filename.find_last_of("\\/");
	std::string scriptpath;
	if (ls != std::string::npos)
		scriptpath = filename.substr(0, ls + 1);

	node* result = zidl_parse_stream(fs, errorline, scriptpath);
	fs.close();

	return result;
}
