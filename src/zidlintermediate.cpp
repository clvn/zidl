#include <iostream>
#include <cassert>
#include <fstream>
#include "zidlparser.h"
#include "zidlintermediate.h"

using std::cout;
using std::endl;

zidlclassmember* parse_zidl_classmember(zidlunit* nsparent, const node* n) {
	assert(n->type == _Member);

	zidlclassmember* result = new zidlclassmember();
	result->name = n->name;
	result->parent = nsparent;

	return result;
}

zidlclassmethod* parse_zidl_classmethod(zidlunit* nsparent, const node* n) {
	assert(n->type == _Method || n->type == _CallbackType);

	zidlclassmethod* result = new zidlclassmethod();
	result->name = n->name;
	result->parent = nsparent;
	result->is_static = false;
	result->is_callback = false;
	
	// parse modifiers
	for (std::vector<node*>::const_iterator i = n->modifiers.begin(); i != n->modifiers.end(); ++i) {
		const node* child = *i;
		assert(child->type == _TypeModifier);
		if (child->name == "static") result->is_static = true;
	}

	for (std::vector<node*>::const_iterator i = n->children.begin(); i != n->children.end(); ++i) {
		const node* child = *i;
		assert(child->type == _Argument);

		zidlargument arg;
		arg.name = child->name;
		arg.has_default = false;

		for (std::vector<node*>::const_iterator j = child->modifiers.begin(); j != child->modifiers.end(); ++j) {
			const node* argmod = *j;
			if (argmod->type == _EnumValue) {
				arg.has_default = true;
				arg.default_int = argmod->intvalue;
			}
		}

		result->arguments.push_back(arg);
	}
	return result;
}

zidlclassunion* parse_zidl_union(zidlnamespace* ns, zidlunit* nsparent, const node* n) {
	assert(n->type == _Union);

	zidlclassmember* parsedmember;
	std::vector<std::string> comments;

	zidlclassunion* result = new zidlclassunion();
	result->name = n->name;
	result->parent = nsparent;

	for (std::vector<node*>::const_iterator i = n->children.begin(); i != n->children.end(); ++i) {
		const node* child = *i;
		if (child == 0) continue;

		switch (child->type) {
			//case _String:
			//	comments.push_back(child->name);
			//	break;
			case _Member:
				parsedmember = parse_zidl_classmember(result, child);
				if (parsedmember == 0) return 0;
				result->members.push_back(parsedmember);
				parsedmember->comments = comments;
				comments.clear();
				break;
		}
	}

	return result;
}

zidlclass* parse_zidl_class(zidlnamespace* ns, zidlunit* nsparent, const node* n) {
	assert(n->type == _Class);

	zidlclass* parsedclass;
	zidlclassmethod* parsedmethod;
	zidlclassmember* parsedmember;
	zidlclassunion* parsedunion;
	std::vector<std::string> comments;

	zidlclass* result = new zidlclass();
	result->name = n->name;
	result->parent = nsparent;

	for (std::vector<node*>::const_iterator i = n->children.begin(); i != n->children.end(); ++i) {
		const node* child = *i;
		if (child == 0) continue;

		switch (child->type) {
			case _String:
				comments.push_back(child->name);
				break;
			case _Union:
				parsedunion = parse_zidl_union(ns, result, child);
				if (parsedunion == 0) return 0;
				result->unions.push_back(parsedunion);
				parsedunion->comments = comments;
				comments.clear();
				break;
			case _Class:
				parsedclass = parse_zidl_class(ns, result, child);
				if (parsedclass == 0) return 0;
				result->classes.push_back(parsedclass);
				parsedclass->comments = comments;
				comments.clear();
				break;
			case _Method:
				parsedmethod = parse_zidl_classmethod(result, child);
				if (parsedmethod == 0) return 0;
				result->methods.push_back(parsedmethod);
				parsedmethod->comments = comments;
				comments.clear();
				break;
			case _Member:
				parsedmember = parse_zidl_classmember(result, child);
				if (parsedmember == 0) return 0;
				result->members.push_back(parsedmember);
				parsedmember->comments = comments;
				comments.clear();
				break;
			//default:
			//	return 0;
		}
	}
	return result;
}

zidlenum* parse_zidl_enum(zidlunit* nsparent, const node* n) {
	assert(n->type == _Enum);

	zidlenum* result = new zidlenum();
	result->name = n->name;
	result->parent = nsparent;

	for (std::vector<node*>::const_iterator i = n->children.begin(); i != n->children.end(); ++i) {
		const node* child = *i;
		if (child == 0) continue;

		zidlenumvalue value;
		value.name = child->name;
		value.value = child->intvalue;
		result->values.push_back(value);
	}
	return result;
}

bool is_builtin_type(std::string name) {
	return 
		(name == "int64") ||
		(name == "int") ||
		(name == "uint") ||
		(name == "short") ||
		(name == "ushort") ||
		(name == "char") ||
		(name == "byte") ||
		(name == "uchar") ||
		(name == "float") ||
		(name == "double") ||
		(name == "bool") ||
		(name == "string") ||
		(name == "void") ||
		(name == "pvoid") ||
		(name == "array")
	;
}

zidlclass* find_class(zidlunit* nscontext, std::string classref) {
	// split name on '.', find first part, continue scanning for remainder
	std::string::size_type fd = classref.find_first_of('.');
	std::string name = classref.substr(0, fd);
	std::string membername;
	if (fd != std::string::npos)
		membername = classref.substr(fd + 1);

	if (nscontext->type == zidltype_class) {
		zidlclass* result = ((zidlclass*)nscontext)->get_class(name);
		if (result != 0)
			if (fd == std::string::npos)
				return result;
			else 
				return find_class(result, membername);
	} else
	if (nscontext->type == zidltype_namespace) {
		zidlclass* result = ((zidlnamespace*)nscontext)->get_class(name);
		if (result != 0)
			if (fd == std::string::npos)
				return result;
			else 
				return find_class(result, membername);
	} else
	if (nscontext->type == zidltype_program) {
		zidlnamespace* ns = ((zidlprogram*)nscontext)->get_namespace(name);
		if (ns != 0)
			return find_class(ns, membername);
		else
			return 0;
	}
	if (nscontext->parent == 0) return 0;

	return find_class(nscontext->parent, classref);
}

zidlclassmethod* find_callback(zidlunit* nscontext, std::string name) {
	if (nscontext->type == zidltype_namespace) {
		zidlclassmethod* result = ((zidlnamespace*)nscontext)->get_callback(name);
		if (result != 0) return result;
	}
	if (nscontext->parent == 0) return 0;

	return find_callback(nscontext->parent, name);
}

bool resolve_zidl_typeref(zidlunit* nscontext, const node* metatype, zidltyperef* result) {
	
	result->classref = 0;
	result->callbackref = 0;
	result->arraytype = 0;
	result->is_out = false;
	result->no_python = false;
	result->is_array = false;
	result->no_ref = false;

	if (metatype == 0) {
		result->name = "void";
	} else {

		for (std::vector<node*>::const_iterator i = metatype->modifiers.begin(); i != metatype->modifiers.end(); ++i) {
			const node* child = *i;
			assert(child->type == _TypeModifier);
			if (child->name == "out") result->is_out = true;
			else if (child->name == "no_python") result->no_python = true;
			else if (child->name == "noref") 
				result->no_ref = true;
		}

		zidltyperef* arraytyperef = 0;
		for (std::vector<node*>::const_iterator i = metatype->children.begin(); i != metatype->children.end(); ++i) {
			const node* child = *i;
			assert(child->type == _Indexer); // could be a default value too - but is it parsed?
			
			// handle out string vs array inconsistency.
			// example #1: returns a string in a parameter:
			//     def myfunction(out string[maxlen] result, int maxlen)
			//  example #2: returns an array of strings in a parameter:
			//     def myfunction(out string[maxlen][maxstrings] result, int maxlen, int maxstrings)
			//  example #3: takes an array of strings in a parameter:
			//     def myfunction(string[maxstrings] result, int maxstrings)
			//  example #5: takes an array of array of ints as parameter
			//     def myfunction(int[maxlen][maxlen] result, int maxlen)
			if (result->is_out && metatype->name == "string" && i == metatype->children.begin()) {
				if (metatype->children.size() == 1) {
					// its a regular out string w/length
					result->stringlengthname = child->name;
					break;
				} 
			}
			
			// its an array of somethings
			if (arraytyperef == 0) {
				result->name = "array";
				result->is_array = true;

				arraytyperef = new zidltyperef();
				arraytyperef->name = metatype->name;
				arraytyperef->arraytype = 0;
				arraytyperef->callbackref = 0;
				arraytyperef->classref = 0;
				arraytyperef->is_array = false;
				arraytyperef->is_out = false;
				arraytyperef->no_python = false;
				arraytyperef->no_ref = false;
				result->arraylengthname = child->name;
				result->arraytype = arraytyperef;
			} else {
				zidltyperef* subarray = new zidltyperef();					
				subarray->name = "array";
				subarray->arraytype = 0;
				subarray->callbackref = 0;
				subarray->classref = 0;
				subarray->is_array = true;
				subarray->is_out = false;
				subarray->no_python = false;
				subarray->no_ref = false;
				subarray->arraytype = arraytyperef;
				subarray->arraylengthname = child->name;
				result->arraytype = subarray;
			}

		}

		if (arraytyperef == 0) {
			result->name = metatype->name;
		}

		zidltyperef* resolvetype = result;
		while (resolvetype->is_array) resolvetype = resolvetype->arraytype;

		if (!is_builtin_type(resolvetype->name)) {
			resolvetype->classref = find_class(nscontext, resolvetype->name);
			if (resolvetype->classref == 0) 
				resolvetype->callbackref = find_callback(nscontext, resolvetype->name);
			if (resolvetype->callbackref == 0 && resolvetype->classref == 0) 
				return false;
		}
	}
	return true;
}

bool resolve_zidl_classmember(const node* n, zidlclassmember* result) {
	if (!resolve_zidl_typeref(result->parent, n->metatype, &result->membertype))
		return false;
	return true;
}

bool resolve_zidl_classmethod(const node* n, zidlclassmethod* result) {
	// use typeinfo from n->metatype and look up non-native types in the methods namespace, recursively down the ns hierarchy

	if (!resolve_zidl_typeref(result->parent, n->metatype, &result->returntype))
		return false;

	for (std::vector<zidlargument>::iterator i = result->arguments.begin(); i != result->arguments.end(); ++i) {
		const node* child = n->find_child(_Argument, i->name);
		if (!resolve_zidl_typeref(result->parent, child->metatype, &i->argtype))
			return false;
	}

	return true;
}

bool resolve_zidl_classunion(const node* n, zidlclassunion* result) {

	for (std::vector<zidlclassmember*>::iterator i = result->members.begin(); i != result->members.end(); ++i) {
		zidlclassmember* parsedmember = *i;
		const node* child = n->find_child(_Member, parsedmember->name);
		if (!resolve_zidl_classmember(child, parsedmember)) return false;
	}

	return true;
}

bool resolve_zidl_class(const node* n, zidlclass* result) {
	for (std::vector<zidlclassmethod*>::iterator i = result->methods.begin(); i != result->methods.end(); ++i) {
		zidlclassmethod* parsedmethod = *i;
		const node* child = n->find_child(_Method, parsedmethod->name);
		if (!resolve_zidl_classmethod(child, parsedmethod)) 
			return false;
	}

	for (std::vector<zidlclassmember*>::iterator i = result->members.begin(); i != result->members.end(); ++i) {
		zidlclassmember* parsedmember = *i;
		const node* child = n->find_child(_Member, parsedmember->name);
		if (!resolve_zidl_classmember(child, parsedmember)) 
			return false;
	}

	for (std::vector<zidlclassunion*>::iterator i = result->unions.begin(); i != result->unions.end(); ++i) {
		zidlclassunion* parsedunion = *i;
		const node* child = n->find_child(_Union, parsedunion->name);
		if (!resolve_zidl_classunion(child, parsedunion)) 
			return false;
	}

	for (std::vector<zidlclass*>::iterator i = result->classes.begin(); i != result->classes.end(); ++i) {
		zidlclass* parsedclass = *i;
		const node* child = n->find_child(_Class, parsedclass->name);
		if (!resolve_zidl_class(child, parsedclass)) 
			return false;
	}

	return true;
}

bool resolve_zidl_namespace(const node* n, zidlnamespace* result) {
	assert(n->type == _Namespace);
	assert(result->name == n->name);

	for (std::vector<zidlclass*>::iterator i = result->classes.begin(); i != result->classes.end(); ++i) {
		zidlclass* parsedclass = *i;
		const node* child = n->find_child(_Class, parsedclass->name);
		if (!resolve_zidl_class(child, parsedclass))
			return false;
	}
	for (std::vector<zidlclassmethod*>::iterator i = result->callbacks.begin(); i != result->callbacks.end(); ++i) {
		zidlclassmethod* parsedmethod = *i;
		const node* child = n->find_child(_CallbackType, parsedmethod->name);
		if (!resolve_zidl_classmethod(child, parsedmethod)) 
			return false;
	}
	return true;

}

zidlnamespace* parse_zidl_namespace(zidlunit* nsparent, const node* n) {
	assert(n->type == _Namespace);

	zidlenum* parsedenum;
	zidlclass* parsedclass;
	zidlclassmethod* parsedmethod;
	std::vector<std::string> comments;

	zidlnamespace* result = new zidlnamespace();
	result->parent = nsparent;
	result->name = n->name;

	for (std::vector<node*>::const_iterator i = n->children.begin(); i != n->children.end(); ++i) {
		const node* child = *i;
		if (child == 0) continue;

		switch (child->type) {
			case _String:
				comments.push_back(child->name);
				break;
			case _DlName:
				if (result->dlname.length() != 0) return false;
				result->dlname = child->name;
				comments.clear(); // unused
				break;
			case _Enum:
				parsedenum = parse_zidl_enum(result, child);
				if (parsedenum == 0) return false;
				result->content.push_back(parsedenum);
				result->enums.push_back(parsedenum);
				parsedenum->comments = comments;
				comments.clear();
				break;
			case _Class:
				parsedclass = parse_zidl_class(result, result, child);
				if (parsedclass == 0) return false;
				result->content.push_back(parsedclass);
				result->classes.push_back(parsedclass);
				parsedclass->comments = comments;
				comments.clear();
				break;
			case _CallbackType:
				parsedmethod = parse_zidl_classmethod(result, child);
				if (parsedmethod == 0) return false;
				parsedmethod->is_callback = true;
				parsedmethod->is_static = true;
				result->content.push_back(parsedmethod);
				result->callbacks.push_back(parsedmethod);
				parsedmethod->comments = comments;
				comments.clear();
				break;
		}
	}
	return result;
}

// parses out a single namespace
// needs two pass: 1 to find all types, 2 to resolve types
bool parse_zidl_program(const node* n, zidlprogram* result, bool importing) {
	assert(n->type == _ProgramList || n->type == _Import);

	std::vector<std::string> comments;

	// pass 1: build tree without resolved type references
	bool gotnamespace = false;
	zidlnamespace* parsednamespace;
	for (std::vector<node*>::const_iterator i = n->children.begin(); i != n->children.end(); ++i) {
		const node* child = *i;
		if (child == 0) continue;

		switch (child->type) {
			case _Include:
				result->includes.push_back(child->name);
				break;
			case _Import:
				parse_zidl_program(child, result, true);
				break;
			case _Cheader:
				result->cheaders.push_back(child->name);
				break;
			case _String:
				if (gotnamespace) return false;
				comments.push_back(child->name);
				break;
			case _Namespace:
				if (gotnamespace) return false;
				gotnamespace = true;
				parsednamespace = parse_zidl_namespace(result, child);
				if (!parsednamespace) return false;
				parsednamespace->is_imported = importing;
				parsednamespace->comments = comments;
				result->namespaces.push_back(parsednamespace);
				//if (!parse_zidl_namespace(child, result)) return false;
				//result->comments = comments;
				break;
		}
	}


	// pass 2: resolve type references in the tree

	for (std::vector<zidlnamespace*>::iterator i = result->namespaces.begin(); i != result->namespaces.end(); ++i) {
		zidlnamespace* parsedentity = *i;
		const node* child = n->find_child(_Namespace, parsedentity->name);
		if (!resolve_zidl_namespace(child, parsedentity))
			return false;
	}
/*	for (std::vector<node*>::const_iterator i = n->children.begin(); i != n->children.end(); ++i) {
		const node* child = *i;
		if (child == 0) continue;

		switch (child->type) {
			case _Namespace:
				if (!resolve_zidl_namespace(child, result))
					return false;
				break;
		}
	}*/

	return true;
}
