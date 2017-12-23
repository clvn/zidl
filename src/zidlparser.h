#pragma once

#include <stack>
#include <string>
#include <vector>
#include <istream>
#include <cstring>

enum nodetype {
	_Ignore,
	_Namespace,
	_Class,
	_Method,
	_Member,
	_Comment,
	_DlName,
	_Enum,
	_String,
	_Include,
	_Import,
	_Cheader,
	_ProgramList,
	_ClassList,
	_Union,
	_UnionList,
	_Type,
	_TypeModifierList,
	_TypeModifier,
	_Argument,
	_ArgumentList,
	_EnumList,
	_EnumValue,
	_Indexer,
	_CallbackType,
};

struct node {
	nodetype type;
	std::string name;
	node* metatype;
	node* parent;
	int intvalue;
	std::vector<node*> children;
	std::vector<node*> modifiers;

	node() {
		metatype = 0;
		parent = 0;
	}

	~node() {
		delete metatype;
		for (std::vector<node*>::iterator i = modifiers.begin(); i != modifiers.end(); ++i) 
			delete *i;
		for (std::vector<node*>::iterator i = children.begin(); i != children.end(); ++i) 
			delete *i;
	}

	/*void dump(int indent = 0) {
		for (int i = 0; i < indent; i++) printf(" ");
		printf("%s (%i)\n", name.c_str(), type);
		for (std::vector<node*>::iterator i = children.begin(); i != children.end(); ++i) {
			if ((*i) == 0) continue;
			(*i)->dump(indent + 2);
		}
	}*/

	void add_child(node* n) {
		if (n)
			n->parent = this;
		children.push_back(n);
	}

	void set_parent(std::vector<node*>& childnodes) {
		for (std::vector<node*>::iterator i = childnodes.begin(); i != childnodes.end(); ++i) {
			if (*i != 0)
				(*i)->parent = this;
		}
	}

	node* find_child(nodetype findtype, std::string findname) const {
		for (std::vector<node*>::const_iterator i = children.begin(); i != children.end(); ++i) {
			node* child = *i;
			if (child == 0) continue;
			if (child->type == findtype && child->name == findname) return child;
			if (child->type == _Import) {
				node* importchild = child->find_child(findtype, findname);
				if (importchild != 0) return importchild;
			}
		}
		return 0;
	}
};

struct token {
	int tokval;
	char tok[1024];
};

struct parsecontext {
	int error;
	node* result;
	std::string scriptpath;
};

struct scanner {
	enum {
		buffersize = 1024
	};

	std::istream& strm;
	char buffer[buffersize];
	char* bufferread;
	char* bufferwrite;
	char* bufferline;
	char* token;
	int linetokens;
	std::stack<int> indentstack;
	int linecounter;
	
	scanner(std::istream& _strm):strm(_strm) { 
		bufferread = buffer; 
		bufferwrite = buffer;
		bufferline = buffer;
		indentstack.push(0);
		linetokens = 0;
	}

	void fill(int count) {
		if (bufferwrite + count >= buffer + buffersize) {
			memmove(buffer, bufferline, buffer + buffersize - bufferline);
			int line_to_write = (int)(bufferwrite - bufferline);
			int line_to_read = (int)(bufferread - bufferline);
			int line_to_token = (int)(token - bufferline);
			bufferline = buffer;
			bufferwrite = buffer + line_to_write;
			bufferread = buffer + line_to_read;
			token = buffer + line_to_token;
		}
		strm.read(bufferwrite, count);
		if (!strm) 
			bufferwrite += strm.gcount();
		else 
			bufferwrite += count;
		*bufferwrite = 0;
	}

	char*& limit() {
		return bufferwrite;
	}

	char*& cursor() {
		return bufferread;
	}
	
	void extract_token(char* tok) {
		memcpy(tok, token, bufferread - token);
		tok[bufferread - token] = 0;
	}
	
	int get_token_length() {
		// count number of tabs and/or spaces
		return (int)(bufferread - token);
	}
};
/*
struct zidlgenerator {
	struct modifiers {
		bool is_out;
		bool is_static;
		bool is_noref;
		bool is_nopython;
	};

	virtual ~zidlgenerator() { }
	virtual void generate(std::ostream& strm, const node* n) = 0;

	const node* find_class(const node* classnode, const node* typenode) {
		if (classnode->type == _Union) return find_class(classnode->parent, typenode);

		assert(classnode->type == _Class || classnode->type == _Namespace);
		if (classnode->type == _Class && classnode->name == typenode->name) return classnode;

		for (std::vector<node*>::const_iterator i = classnode->children.begin(); i != classnode->children.end(); ++i) {
			const node* child = *i;
			if (child == 0) continue;
			if (child->type != _Class && child->type != _CallbackType) continue;
			if (child->name == typenode->name) return child;
		}

		if (classnode->parent && (classnode->parent->type == _Class || classnode->parent->type == _Namespace))
			return find_class(classnode->parent, typenode);
		return 0;
	}

	modifiers parse_modifiers(const node* n) {
		modifiers result;
		memset(&result, 0, sizeof(result));

		for (std::vector<node*>::const_iterator i = n->modifiers.begin(); i != n->modifiers.end(); ++i) {
			const node* child = *i;
			assert(child->type == _TypeModifier);
			if (child->name == "out") result.is_out = true;
			if (child->name == "static") result.is_static = true;
			if (child->name == "noref") result.is_noref = true;
		}
		return result;
	}


};

std::string camel_to_posix(std::string camelstr);
std::string camel_to_posix(const std::string& camelstr1, const std::string& camelstr2);
std::string camel_to_posix(const std::string& camelstr1, const std::string& camelstr2, const std::string& camelstr3);
std::string posix_to_camel(std::string str);
*/
extern std::string stripquotes(const std::string& str);
node* zidl_parse_file(const std::string& filename, int* errorline = 0);
node* zidl_parse_stream(std::istream& strm, int* errorline = 0);

std::string camel_to_posix(std::string camelstr);