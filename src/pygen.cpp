#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <stack>
#include <vector>
#include <algorithm>
#include <functional>
#include <cctype>
#include <iomanip>
#include <set>
#include "zidlparser.h"
#include "zidlintermediate.h"
#include "zidlgenerator.h"
#include "zidlhelper.h"

using std::endl;
using std::cout;
using namespace zidlhelper;

const char* python_header = 
"#!/usr/bin/env python\n"
"# encoding: latin-1\n"
"\n"
"from ctypes import *\n"
"\n"
"import sys, os\n"
"\n"
"def load_library(*names,**kw):\n"
"	\"\"\"\n"
"	searches for a library with given names and returns a ctypes \n"
"	.so/.dll library object if successful. if the library can not\n"
"	be loaded, an assertion error will be thrown.\n"
"	\n"
"	@type  names: list of strings\n"
"	@param names: one or more aliases for required libraries, e.g.\n"
"				  'SDL','SDL-1.2'.\n"
"	@rtype: ctypes CDLL handle\n"
"	\"\"\"\n"
"	import ctypes, os, sys\n"
"	searchpaths = []\n"
"	if os.name in ('posix', 'mac'):\n"
"		if os.environ.has_key('LD_LIBRARY_PATH'):\n"
"			searchpaths += os.environ['LD_LIBRARY_PATH'].split(os.pathsep)\n"
"		searchpaths += [\n"
"			'/usr/local/lib64',\n"
"			'/usr/local/lib',\n"
"			'/usr/lib64',\n"
"			'/usr/lib',\n"
"		]\n"
"	elif os.name == 'nt':\n"
"		searchpaths += ['.']\n"
"		if 'PATH' in os.environ:\n"
"			searchpaths += os.environ['PATH'].split(os.pathsep)\n"
"	else:\n"
"		assert 0, \"Unknown OS: %s\" % os.name\n"
"	if 'paths' in kw:\n"
"		searchpaths += kw['paths']\n"
"	for name in names:\n"
"		if os.name == 'nt':\n"
"			libname = name + '.dll'\n"
"		elif sys.platform == 'darwin':\n"
"			libname = 'lib' + name + '.dylib'\n"
"			if 'version' in kw:\n"
"				libname += '.' + kw['version']\n"
"		else:\n"
"			libname = 'lib' + name + '.so'\n"
"			if 'version' in kw:\n"
"				libname += '.' + kw['version']\n"
"		m = None\n"
"		for path in searchpaths:\n"
"			if os.path.isdir(path):\n"
"				libpath = os.path.join(path,libname)\n"
"				if os.path.isfile(libpath):\n"
"					m = ctypes.CDLL(libpath)\n"
"					break\n"
"				for filename in reversed(sorted(os.listdir(path))):\n"
"					if filename.startswith(libname):\n"
"						m = ctypes.CDLL(os.path.join(path,filename))\n"
"						break\n"
"				if m:\n"
"					break\n"
"		if m:\n"
"			break\n"
"	assert m, \"libraries %s not found in %s\" % (','.join([\"'%s'\" % a for a in names]),','.join(searchpaths))\n"
"	return m\n"
"	\n"
"def dlopen(*args,**kwds):\n"
"	\"\"\"\n"
"	Opens a library by name and returns a handle object. See \n"
"	{library.load} for more information.\n"
"	\"\"\"\n"
"	return load_library(*args,**kwds)\n"
"\n"
"def dlsym(lib, name, restype, *args):\n"
"	\"\"\"\n"
"	Retrieves a symbol from a library loaded by dlopen and\n"
"	assigns correct result and argument types.\n"
"	\n"
"	@param lib: Library object.\n"
"	@type lib: ctypes.CDLL\n"
"	@param name: Name of symbol.\n"
"	@type name: str\n"
"	@param restype: Type of function return value.\n"
"	@param args: Types of function arguments.	\n"
"	\"\"\"\n"
"	if not lib:\n"
"		return None\n"
"	proc = getattr(lib,name)\n"
"	proc.restype = restype\n"
"	proc.argtypes = [argtype for argname,argtype in args]\n"
"	proc.o_restype = restype\n"
"	proc.o_args = args\n"
"	return proc\n"
;

struct pygen : zidlgenerator{

	void generate_ctypes_enum(std::ostream& strm, zidlenum& e) {
		std::string enumprefix = e.parent->name;

		strm << endl;
		if (e.name.empty()) {
			strm << "# nameless enum" << endl;
		} else {
			enumprefix += "_" + camel_to_posix(e.name);
			strm << "# enum " << enumprefix << endl;
		}

		for (std::vector<zidlenumvalue>::const_iterator i = e.values.begin(); i != e.values.end(); ++i) {
			strm << enumprefix << "_" << camel_to_posix(i->name) << " = " << (unsigned int)i->value << endl;
		}

		strm << endl;
	}

	void generate_ctypes_enums(std::ostream& strm, zidlnamespace& ns) {
		for (std::vector<zidlenum*>::iterator i = ns.enums.begin(); i != ns.enums.end(); ++i) {
			generate_ctypes_enum(strm, **i);
		}
	}
	
	void generate_ctypes_classmember(std::ostream& strm, zidlclassmember& m) {
		std::string membername = camel_to_posix(m.name);
		strm << indent(2) << "(\"" << membername << "\", ";
		strm << get_pyctypes_type_name(m.membertype);
		strm << ")," << endl;
	}
		
	void generate_ctypes_unionmember(std::ostream& strm, zidlclassmember& m) {
		std::string membername = camel_to_posix(m.name);
		strm << indent(2) << "(\"" << membername << "\", ";
		strm << get_pyctypes_union_type_name(m.membertype);
		strm << ")," << endl;
	}

	void generate_ctypes_classunion(std::ostream& strm, zidlclassunion& u) {
		// before we write this union, make sure we have written all types this union needs etc recursively
		for (std::vector<zidlclassmember*>::iterator i = u.members.begin(); i != u.members.end(); ++i) {
			if ((*i)->membertype.classref)
				generate_ctypes_class(strm, *(*i)->membertype.classref);
		}

		strm << "class " << get_c_class_name(u) << "(Union):" << endl;
		for (std::vector<zidlclassmember*>::iterator i = u.members.begin(); i != u.members.end(); ++i) {
			generate_ctypes_unionmember(strm, **i);
		}
		//strm << "}" << endl;
	}

	std::set<zidlclass*> written_classes;

	void generate_ctypes_class(std::ostream& strm, zidlclass& c) {
		std::set<zidlclass*>::iterator it = written_classes.find(&c);
		if (it != written_classes.end()) return ;

		written_classes.insert(&c);

		zidlnamespace* classns = find_parent_namespace(c);
		assert(classns != 0);
		if (classns->is_imported) 
			return ;

		// before we write this class, make sure we have written all types this class needs etc recursively
		for (std::vector<zidlclassmember*>::iterator i = c.members.begin(); i != c.members.end(); ++i) {
			if ((*i)->membertype.classref) {
				generate_ctypes_class(strm, *(*i)->membertype.classref);
			}
		}


		strm << "class " << get_c_class_name(c) << "(Structure):" << endl;
		strm << indent(1) << "_fields_ = [" << endl;
		for (std::vector<zidlclassmember*>::iterator i = c.members.begin(); i != c.members.end(); ++i) {
			generate_ctypes_classmember(strm, **i);
		}
		strm << indent(1) << "]" << endl;
		strm << indent(1) << "_anonymous_ = [" << endl;
		strm << indent(1) << "]" << endl;
		strm << endl;

		for (std::vector<zidlclass*>::iterator i = c.classes.begin(); i != c.classes.end(); ++i) {
			generate_ctypes_class(strm, **i);
		}

		for (std::vector<zidlclassunion*>::iterator i = c.unions.begin(); i != c.unions.end(); ++i) {
			generate_ctypes_classunion(strm, **i);
		}
	}

	void generate_ctypes_classes(std::ostream& strm, zidlnamespace& ns) {
		for (std::vector<zidlclass*>::iterator i = ns.classes.begin(); i != ns.classes.end(); ++i) {
			generate_ctypes_class(strm, **i);
		}
	}
	
	void generate_ctypes_class_function(std::ostream& strm, zidlclassmethod& m) {
		zidlnamespace* rootns = (zidlnamespace*)m.find_parent(zidltype_namespace);
		assert(rootns != 0);
		
		if (m.is_callback) {
			strm << get_c_class_name(m) << " = ";
			strm << "CFUNCTYPE(";
			strm << get_pyctypes_type_name(m.returntype);
			for (std::vector<zidlargument>::iterator i = m.arguments.begin(); i != m.arguments.end(); ++i) {
				strm << ", " << get_pyctypes_type_name(i->argtype);
			}
			strm << ")" << endl;
		} else {
			std::string membername = get_c_method_name(m);
			strm << membername << " = ";
			strm << "dlsym(lib" << rootns->dlname << ", \"" << membername << "\", ";
			strm << get_pyctypes_type_name(m.returntype);
			if (!m.is_static)
				strm << ", (\"" << m.parent->name << "\", POINTER(" << get_c_class_name(*m.parent) << "))";

			for (std::vector<zidlargument>::iterator i = m.arguments.begin(); i != m.arguments.end(); ++i) {
				strm << ", (\"" << i->name << "\", " << get_pyctypes_type_name(i->argtype) << ")";
			}
			strm << ")" << endl;
		}

	}

	void generate_ctypes_class_functions(std::ostream& strm, zidlclass& c) {
		for (std::vector<zidlclassmethod*>::iterator i = c.methods.begin(); i != c.methods.end(); ++i) {
			generate_ctypes_class_function(strm, **i);
		}
	}

	void generate_ctypes_classes_functions(std::ostream& strm, zidlnamespace& ns) {
		for (std::vector<zidlunit*>::iterator i = ns.content.begin(); i != ns.content.end(); ++i) {
			switch ((*i)->type) {
				case zidltype_class:
					//generate_class(strm, 0, (zidlclass&)**i);
					generate_ctypes_class_functions(strm, (zidlclass&)**i);
					break;
				case zidltype_classmethod:
					generate_ctypes_class_function(strm, (zidlclassmethod&)**i);
					break;
			}
		}
		//for (std::vector<zidlclass*>::iterator i = ns.classes.begin(); i != ns.classes.end(); ++i) {
		//	generate_ctypes_class_functions(strm, **i);
		//}
	}


	void generate_classes_class_function(std::ostream& strm, zidlclassmethod& m) {
		zidlnamespace* rootns = (zidlnamespace*)m.find_parent(zidltype_namespace);
		assert(rootns != 0);

		if (m.is_static)
			strm << indent(1) << "@staticmethod" << endl;
		strm << indent(1) << "def " << m.name << "(";
		if (!m.is_static)
			strm << "self";

		std::vector<const zidlargument*> outargs;
		int argcount = 0;
		for (std::vector<zidlargument>::const_iterator i = m.arguments.begin(); i != m.arguments.end(); ++i) {
			if (i->argtype.is_out) {
				outargs.push_back(&*i);
			} else {
				if (!m.is_static || argcount > 0) 
					strm << ", ";
				strm << i->name;
				if (i->has_default) {
					if (i->argtype.classref != 0) {
						if (i->default_int != 0) {
							cout << "WARNING: non-zero default value specified on class type argument " << i->name << endl;
						} 
						strm << " = None";
					} else {
					// check type, only 0=None for objcts
						strm << " = " << i->default_int;
					}
				}
				argcount++;
			}
		}
		strm << "):" << endl;

		generate_comments(strm, 2, m.comments);

		if (!m.is_static)
			strm << indent(2) << "assert self._as_parameter_" << endl;

		for (std::vector<zidlargument>::iterator i = m.arguments.begin(); i != m.arguments.end(); ++i) {
			if (i->argtype.is_out) {
				// handle out parameter
				std::string declname;
				if (i->argtype.name == "string") {
					declname = "c_char";
					/*if (i->argtype.arraylengthnames.empty())
						cout << "WARNING: no out size modifier on " << m.name << endl;*/
				} else if (i->argtype.is_array) {
					const zidltyperef* arraybasetype = get_array_base_type(*i->argtype.arraytype);
					declname = get_pyctypes_type_name(*arraybasetype);
				} else {
					declname = get_pyctypes_type_name(i->argtype);
				}
				
				strm << indent(2) << i->name << " = (" << declname;
				// loop array dimensions
				const zidltyperef* arraytype = &i->argtype;
				while (arraytype->is_array) {
					//name = "POINTER(" + name + ")";
					strm << "*" << arraytype->arraylengthname;
					arraytype = arraytype->arraytype;
				}

				if (i->argtype.name == "string") {
					strm << "*" << i->argtype.stringlengthname;
				}
				/*for (std::vector<std::string>::iterator j = i->argtype.arraylengthnames.begin(); j != i->argtype.arraylengthnames.end(); ++j) {
					std::string lengthname = i->argtype.arraylengthnames[0];
					if (!lengthname.empty())
						strm << "*" << lengthname;
				} */
				strm << ")()" << endl;
			} else if (i->argtype.callbackref != 0) {
				strm << indent(2) << "# wrap callback" << endl;
				strm << indent(2) << "cb_" << i->name << " = " << i->argtype.callbackref->name << "._from_function(" << i->name << ")" << endl;
			}
		}

		strm << indent(2);

		if (outargs.empty()) {
			if (m.returntype.name != "void") {
				strm << "return ";
			}
			if (m.returntype.classref != 0)
				strm << m.returntype.classref->name << "._new_from_handle(";
		} else {
			if (m.returntype.name != "void")
				strm << "_ret_val = ";
			if (m.returntype.classref != 0)
				strm << m.returntype.classref->name << "._new_from_handle(";
		}

		strm << get_c_method_name(m) << "(";
		if (!m.is_static)
			strm << "self";
		for (std::vector<zidlargument>::iterator i = m.arguments.begin(); i != m.arguments.end(); ++i) {
			if (!m.is_static || i != m.arguments.begin()) 
				strm << ", ";
			if (i->argtype.callbackref)
				strm << "cb_" << i->name << "._function_handle";
			else
				strm << i->name;
		}
		strm << ")";
		if (m.returntype.classref != 0)
			strm << ")";

		strm << endl;
		if (!outargs.empty()) {
			strm << indent(2) << "return ";
			if (m.returntype.name != "void")
				strm << "_ret_val, ";
			for (std::vector<const zidlargument*>::const_iterator i = outargs.begin(); i != outargs.end(); ++i) {
				const zidlargument* outarg = *i;
				if (i != outargs.begin())
					strm << ", ";

				if (outarg->argtype.is_array && outarg->argtype.name != "string")
					strm << "[v for v in " << outarg->name << "]";
				else
					strm << outarg->name << ".value";
			}
			strm << endl;
		}

		strm << endl;
	}

	void generate_comments(std::ostream& strm, int indents, const std::vector<std::string>& comments) {
		if (comments.size() > 0) {
			for (std::vector<std::string>::const_iterator i = comments.begin(); i != comments.end(); ++i) {
				strm << indent(indents);
				if (i == comments.begin())
					strm << "\"\"\"";
				strm << *i;
				if (&*i == &comments.back())
					strm << "\"\"\"";
				strm << endl;
			}
		}
	}

	void generate_classes_class_body(std::ostream& strm) {
		strm << indent(1) << "_as_parameter_ = None" << endl;
		strm << indent(1) << "_hash = 0" << endl;
		strm << endl;
		strm << indent(1) << "def __init__(self, handle):" << endl;
		strm << indent(2) << "self._as_parameter_ = handle" << endl;
		strm << indent(2) << "self._hash = cast(self._as_parameter_, c_void_p).value" << endl;
		strm << endl;
		strm << indent(1) << "@classmethod" << endl;
		strm << indent(1) << "def _new_from_handle(cls,handle):" << endl;
		strm << indent(2) << "if not handle:" << endl;
		strm << indent(3) << "return None" << endl;
		strm << indent(2) << "return cls(handle)" << endl;
		strm << endl;
		strm << indent(1) << "def __hash__(self):" << endl;
		strm << indent(2) << "return self._hash" << endl;
		strm << endl;
		strm << indent(1) << "def __eq__(self,other):" << endl;
		strm << indent(2) << "return self._hash == hash(other)" << endl;
		strm << endl;
		strm << indent(1) << "def __ne__(self,other):" << endl;
		strm << indent(2) << "return self._hash != hash(other)" << endl;
		strm << endl;
	}

	void generate_classes_class(std::ostream& strm, zidlclass& c) {
		strm << "class " << c.name << "(object):" << endl;
		generate_comments(strm, 1, c.comments);
		generate_classes_class_body(strm);
		for (std::vector<zidlclassmethod*>::iterator i = c.methods.begin(); i != c.methods.end(); ++i) {
			generate_classes_class_function(strm, **i);
		}
		strm << get_c_class_name(c) << "._wrapper_ = " << c.name << endl;
		strm << endl;
	}

	void generate_classes_callback_class(std::ostream& strm, zidlclassmethod& c) {
		strm << "class " << c.name << "(object):" << endl;
		generate_comments(strm, 1, c.comments);
		strm << indent(1) << "_function = None" << endl;
		strm << indent(1) << "_function_handle = None" << endl;
		strm << indent(1) << "_hash = 0" << endl;
		strm << endl;
		strm << indent(1) << "def __init__(self, function):" << endl;
		strm << indent(2) << "self._function = function" << endl;
		strm << indent(2) << "self._function_handle = " << get_c_class_name(c) << "(self.wrapper_function)" << endl;
		strm << indent(2) << "self._hash = cast(self._function_handle, c_void_p).value" << endl;
		strm << endl;
		strm << indent(1) << "@classmethod" << endl;
		strm << indent(1) << "def _from_function(cls, function):" << endl;
		strm << indent(2) << "functionhash = hash(function)" << endl;
		strm << indent(2) << "if functionhash in Callback._instances_:" << endl;
		strm << indent(3) << "return Callback._instances_[functionhash]" << endl;
		strm << indent(2) << "instance = cls(function)" << endl;
		strm << indent(2) << "Callback._instances_[functionhash] = instance" << endl;
		strm << indent(2) << "return instance" << endl;
		strm << endl;
		strm << indent(1) << "def __hash__(self):" << endl;
		strm << indent(2) << "return self._hash" << endl;
		strm << endl;
		strm << indent(1) << "def __eq__(self,other):" << endl;
		strm << indent(2) << "return self._hash == hash(other)" << endl;
		strm << endl;
		strm << indent(1) << "def __ne__(self,other):" << endl;
		strm << indent(2) << "return self._hash != hash(other)" << endl;
		strm << endl;

		strm << indent(1) << "def wrapper_function(self";
		for (std::vector<zidlargument>::iterator i =  c.arguments.begin(); i != c.arguments.end(); ++i) {
			strm << ", " << i->name ;
		}
		strm << "):" << endl;
		strm << indent(2) << "self._function(";
		for (std::vector<zidlargument>::iterator i =  c.arguments.begin(); i != c.arguments.end(); ++i) {
			if (i != c.arguments.begin())
				strm << ", ";
			if (i->argtype.classref) {
				strm << i->argtype.classref->name << "._new_from_handle(" << i->name << ")";
			} else
				strm << i->name ;
		}
		strm << ")" << endl;
		strm << endl;
		strm << c.name << "._instances_ = {}" << endl;
		//strm << get_c_class_name(c) << "._wrapper_ = " << c.name << endl;
		strm << endl;
	}

	void generate_classes(std::ostream& strm, zidlnamespace& ns) {

		for (std::vector<zidlclass*>::iterator i = ns.classes.begin(); i != ns.classes.end(); ++i) {
			generate_classes_class(strm, **i);
		}

		for (std::vector<zidlclassmethod*>::iterator i = ns.callbacks.begin(); i != ns.callbacks.end(); ++i) {
			generate_classes_callback_class(strm, **i);
		}

	}

	void generate(std::ostream& strm, zidlprogram& prog) {
		// TODO: parse zidl version
		strm << python_header << endl;
		for (std::vector<zidlnamespace*>::const_iterator i = prog.namespaces.begin(); i != prog.namespaces.end(); ++i) {
			zidlnamespace& ns = **i;
			if (ns.is_imported) {
				strm << "from " << ns.dlname << " import *" << endl;
			} else {

				strm << "lib" << ns.dlname << " = dlopen(\"" << ns.dlname << "\", \"0.3\")" << endl;

				generate_ctypes_enums(strm, ns);
				generate_ctypes_classes(strm, ns);
				generate_ctypes_classes_functions(strm, ns);

				generate_classes(strm, ns);
			}
		}
	}
};

zidlgenerator* zidlgenerator::create_pygen() {
	return new pygen();
}
