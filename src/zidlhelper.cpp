#include <iostream>
#include <cassert>
#include <fstream>
#include "zidlparser.h"
#include "zidlintermediate.h"
#include "zidlgenerator.h"
#include "zidlhelper.h"

namespace zidlhelper {

	std::string indent(int c) {
		return std::string(c * 2, ' ');
	}

	// C name generators

	std::string get_c_class_prefix(const zidlunit& c) {
		std::string name = camel_to_posix(c.name);
		
		for (const zidlunit* p = c.parent; p != 0 && p->type != zidltype_program; p = p->parent) {
			std::string parentname = camel_to_posix(p->name);
			if (name.empty())
				name = parentname;
			else if (!parentname.empty())
				name = parentname + "_" + name;
		}
		return name;
	}

	std::string get_c_class_name(const zidlunit& c) {
		return get_c_class_prefix(c) + "_t";
	}

/*	std::string get_builtin_cpp_type_name(const zidltyperef& n) {
		if (n.name == "string") {
			if (n.is_out)
				return "char[1024]";
			else
				return "char*";
		} else
			return get_builtin_c_type_name(n);
	}*/

	std::string get_builtin_c_type_name(const zidltyperef& n) {
		if (n.name == "int64")
			return "long long";
		else if (n.name == "int")
			return "int";
		else if (n.name == "uint")
			return "unsigned int";
		else if (n.name == "short")
			return "short";
		else if (n.name == "ushort")
			return "unsigned short";
		else if (n.name == "char")
			return "char";
		else if (n.name == "byte")		// we could just get rid of this
			return "char";
		else if (n.name == "uchar")
			return "unsigned char";
		else if (n.name == "float")
			return "float";
		else if (n.name == "double")
			return "double";
		else if (n.name == "bool")
			return "int";
		else if (n.name == "string")
			return "char*";
		else if (n.name == "void")
			return "void";
		else if (n.name == "pvoid")
			return "void*";
		else if (n.name == "array")
			return get_builtin_c_type_name(*n.arraytype);

		assert(false);
		return "";
	}

	std::string get_c_type_name(const zidltyperef& t) {
		std::string name;
		if (t.classref != 0) {
			name = get_c_class_name(*t.classref);
			if (!t.no_ref)
				name = name + "*";
		} else if (t.callbackref != 0)
			name = get_c_class_name(*t.callbackref);//t.callbackref->name;
		else if (t.is_array) {
			const zidltyperef* arraybasetype = get_array_base_type(*t.arraytype);
			name = get_c_type_name(*arraybasetype);
			const zidltyperef* arraytype;
			/*if (arraybasetype->classref != 0)
				arraytype = t.arraytype;
			else*/
				arraytype = &t;
			while (arraytype && arraytype->is_array) {
				name += "*";
				arraytype = arraytype->arraytype;
			}
		} else
			name = get_builtin_c_type_name(t);
		
		if ((t.is_array || t.name == "string") && !t.is_out)
			name = "const " + name;
		/*if (t.is_out && t.name != "string") {
			name += "*";
		}*/

		/*
		for (std::vector<std::string>::const_iterator i = t.arraylengthnames.begin(); i != t.arraylengthnames.end(); ++i) {
			// c strings are already pointers, so skip first level of indexing
			if (t.name == "string" && i == t.arraylengthnames.begin()) continue;
			name += "*";
		}*/
		return name;
	}

	std::string get_c_method_name(const zidlclassmethod& m) {
		zidlclass& c = (zidlclass&)*m.parent;
		return get_c_class_prefix(c) + "_" + m.name;
	}


/*	std::string get_cpp_type_name(const zidltyperef& t) {
		std::string name;
		if (t.classref != 0) {
			name = get_c_class_name(*t.classref);
			if (!t.no_ref)
				name = name + "*";
		} else if (t.callbackref != 0)
			name = get_c_class_name(*t.callbackref);//t.callbackref->name;
		else 
			name = get_builtin_c_type_name(t);
		
		if ((t.is_array || t.name == "string") && !t.is_out)
			name = "const " + name;

		for (std::vector<std::string>::const_iterator i = t.arraylengthnames.begin(); i != t.arraylengthnames.end(); ++i) {
			// c strings are already pointers, so skip first level of indexing
			if (t.name == "string" && i == t.arraylengthnames.begin()) continue;
			name = "std::vector<" + name + "> ";
		}
		return name;
	}
	*/
	// Python ctypes name generators

	std::string get_builtin_pyctypes_type_name(const zidltyperef& n) {
		if (n.name == "int64")
			return "c_int64";
		else if (n.name == "int")
			return "c_int";
		else if (n.name == "uint")
			return "c_uint";
		else if (n.name == "short")
			return "c_short";
		else if (n.name == "ushort")
			return "c_ushort";
		else if (n.name == "char")
			return "c_char";
		else if (n.name == "byte")		// we could just get rid of this
			return "c_byte";
		else if (n.name == "uchar")
			return "c_ubyte";
		else if (n.name == "float")
			return "c_float";
		else if (n.name == "double")
			return "c_double";
		else if (n.name == "bool")
			return "c_int";
		else if (n.name == "string" && !n.is_out)
			return "c_char_p";
		else if (n.name == "string" && n.is_out)
			return "c_char_p";
		else if (n.name == "void")
			return "None";
		else if (n.name == "pvoid")
			return "c_void_p";		
		assert(false);
		return "";
	}

	std::string get_pyctypes_type_name(const zidltyperef& t) {
		std::string name;
		if (t.classref != 0)
			name = "POINTER(" + get_c_class_name(*t.classref) + ")";
		else if (t.callbackref != 0)
			name = get_c_class_name(*t.callbackref);//->name;
		else if (t.is_array) {
			const zidltyperef* arraybasetype = get_array_base_type(t);
			name = get_pyctypes_type_name(*arraybasetype);
		} else
			name = get_builtin_pyctypes_type_name(t);
		
		const zidltyperef* arraytype = &t;
		while (arraytype->is_array) {
			name = "POINTER(" + name + ")";
			arraytype = arraytype->arraytype;
		}
/*		for (std::vector<std::string>::const_iterator i = t.arraylengthnames.begin(); i != t.arraylengthnames.end(); ++i) {
			// c strings are already pointers, so skip first level of indexing
			if (t.name == "string" && i == t.arraylengthnames.begin()) continue;
			name = "POINTER(" + name + ")";
		}*/
		return name;
	}

	std::string get_pyctypes_union_type_name(const zidltyperef& t) {
		std::string name;
		if (t.classref != 0)
			name = get_c_class_name(*t.classref);
		else if (t.callbackref != 0)
			name = t.callbackref->name;
		else if (t.is_array) {
			const zidltyperef* arraytype = get_array_base_type(t);
			name = get_builtin_pyctypes_type_name(*arraytype);
		} else 
			name = get_builtin_pyctypes_type_name(t);
		
		const zidltyperef* arraytype = &t;
		while (arraytype->is_array) {
			name = "POINTER(" + name + ")";
			arraytype = arraytype->arraytype;
		}
/*		for (std::vector<std::string>::const_iterator i = t.arraylengthnames.begin(); i != t.arraylengthnames.end(); ++i) {
			// c strings are already pointers, so skip first level of indexing
			if (t.name == "string" && i == t.arraylengthnames.begin()) continue;
			name = "POINTER(" + name + ")";
		}*/
		return name;
	}

	zidlnamespace* find_parent_namespace(zidlunit& t) {
		if (t.parent && t.parent->type == zidltype_namespace)
			return (zidlnamespace*)t.parent;
		if (t.parent) 
			return find_parent_namespace(*t.parent);
		return 0;
	}


	std::string get_cs_class_prefix(const zidlunit& c) {
		std::string name = c.name;
		
		for (const zidlunit* p = c.parent; p != 0 && p->type != zidltype_program && p->parent->type != zidltype_program; p = p->parent) {
			name = p->name + "" + name;
		}
		return name;
	}

	std::string get_cs_class_name(const zidlunit& c) {
		return get_cs_class_prefix(c) + "";
	}

	std::string get_builtin_cs_type_name(const zidltyperef& n) {
		if (n.name == "int64")
			return "int64";
		else if (n.name == "int")
			return "int";
		else if (n.name == "uint")
			return "uint";
		else if (n.name == "short")
			return "short";
		else if (n.name == "ushort")
			return "ushort";
		else if (n.name == "char")
			return "char";
		else if (n.name == "byte")		// we could just get rid of this
			return "char";
		else if (n.name == "uchar")
			return "byte";
		else if (n.name == "float")
			return "float";
		else if (n.name == "double")
			return "double";
		else if (n.name == "bool")
			return "bool";
		else if (n.name == "string")
			return "string";
		else if (n.name == "void")
			return "void";
		else if (n.name == "pvoid")
			return "IntPtr";		
		assert(false);
		return "";
	}

	std::string get_cs_type_name(const zidltyperef& t) {
		std::string name;
		if (t.classref != 0) {
			name = get_cs_class_name(*t.classref);
			//if (!t.no_ref)
			//	name = name + "*";
		} else if (t.callbackref != 0) {
			zidlnamespace* ns = find_parent_namespace(*t.callbackref);
			name = ns->dlname + "." + get_cs_class_name(*t.callbackref);//t.callbackref->name;
		} else 
			name = get_builtin_cs_type_name(t);
		
		//if ((t.is_array || t.name == "string") && !t.is_out)
		//	name = "const " + name;

		const zidltyperef* arraytype = &t;
		while (arraytype->is_array) {
			name += "[]";
			arraytype = arraytype->arraytype;
		}
/*		for (std::vector<std::string>::const_iterator i = t.arraylengthnames.begin(); i != t.arraylengthnames.end(); ++i) {
			// c strings are already pointers, so skip first level of indexing
			if (t.name == "string" && i == t.arraylengthnames.begin()) continue;
			name += "[]";
		}*/
		return name;
	}

	std::string get_cs_flat_type_name(const zidltyperef& t) {
		std::string name;
		if (t.classref != 0) {
			if (!t.is_array && t.classref->methods.empty())
				name = t.classref->name;
			else
				name = "IntPtr";
			//if (!t.no_ref)
			//	name = name + "*";
		} else if (t.callbackref != 0)
			name = get_cs_class_name(*t.callbackref);//t.callbackref->name;
		else 
			name = get_builtin_cs_type_name(t);
		
		//if ((t.is_array || t.name == "string") && !t.is_out)
		//	name = "const " + name;

		const zidltyperef* arraytype = &t;
		while (arraytype->is_array) {
			name += "[]";
			arraytype = arraytype->arraytype;
		}
/*		for (std::vector<std::string>::const_iterator i = t.arraylengthnames.begin(); i != t.arraylengthnames.end(); ++i) {
			// c strings are already pointers, so skip first level of indexing
			if (t.name == "string" && i == t.arraylengthnames.begin()) continue;
			name += "[]";
		}*/
		return name;
	}

	const zidltyperef* get_array_base_type(const zidltyperef& arrayref) {
		const zidltyperef* arraytype = &arrayref;
		while (arraytype->is_array)
			arraytype = arraytype->arraytype;
		return arraytype;
	}

};
