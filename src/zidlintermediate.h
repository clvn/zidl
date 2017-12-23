#pragma once

enum zidltype {
	zidltype_program,
	zidltype_namespace,
	zidltype_class,
	zidltype_union,
	zidltype_enum,
	//zidltype_callbackmethod,
	zidltype_classmember,
	zidltype_classmethod,
	zidltype_classunion
};

struct zidlnamespace;
struct zidlclass;
struct zidlclassmethod;
struct zidlclassmember;

struct zidltyperef {
	std::string name;
	zidlclass* classref; // is null for native types or callbacks
	zidlclassmethod* callbackref; // is null for native types or classrefs

	//std::vector<std::string> arraylengthnames; // contains a numeric length or an identifier refering to a sibling class member or argument
	std::string stringlengthname; // for out strings, contains a numeric length or an identifier refering to a sibling argument
	std::string arraylengthname;
	zidltyperef* arraytype;

	bool is_array;
	bool is_out;
	bool no_python;
	bool no_ref;
};

struct zidlargument {
	std::string name;
	zidltyperef argtype;
	bool has_default;
	int default_int;
};

struct zidlunit {
	std::string name;
	zidltype type;
	zidlunit* parent;
	std::vector<std::string> comments;

	zidlunit(zidltype _type) {
		parent = 0;
		type = _type;
	}
	virtual ~zidlunit() {}
	
	zidlunit* find_parent(zidltype findtype) {
		if (parent)
			if (parent->type == findtype)
				return parent;
			else
				return parent->find_parent(findtype);
		return 0;
	}
};

struct zidlclassunion : zidlunit {
	std::vector<zidlclassmember*> members;

	zidlclassunion():zidlunit(zidltype_classunion) {
	}
};

struct zidlclassmethod : zidlunit {
	bool is_static;
	bool is_callback;

	std::vector<zidlargument> arguments;
	zidltyperef returntype;

	zidlclassmethod():zidlunit(zidltype_classmethod) {
	}

	bool has_callback_arguments() {
		for (std::vector<zidlargument>::iterator i = arguments.begin(); i != arguments.end(); ++i) {
			if (!i->argtype.is_out && i->argtype.callbackref != 0) return true;
		}
		return false;
	}
};

struct zidlclassmember : zidlunit {
	zidltyperef membertype;

	zidlclassmember():zidlunit(zidltype_classmember) {
	}
};

struct zidlclass : zidlunit {
	std::vector<zidlclassmethod*> methods;
	std::vector<zidlclassmember*> members;
	std::vector<zidlclassunion*> unions;
	std::vector<zidlclass*> classes;

	zidlclass():zidlunit(zidltype_class) {
	}

	zidlclass* get_class(const std::string& name) {
		for (std::vector<zidlclass*>::iterator i = classes.begin(); i != classes.end(); ++i) {
			if ((*i)->name == name) return *i;
		}
		return 0;
	}
};

struct zidlenumvalue {
	std::string name;
	int value;
};

struct zidlenum : zidlunit {
	std::vector<zidlenumvalue> values;

	zidlenum():zidlunit(zidltype_enum) {
	}
};

struct zidlnamespace : zidlunit {
	bool is_imported;
	std::string dlname;

	std::vector<zidlunit*> content;

	std::vector<zidlenum*> enums;
	std::vector<zidlclass*> classes;
	std::vector<zidlclassmethod*> callbacks;

	zidlnamespace():zidlunit(zidltype_namespace) {
		is_imported = false;
	}

	zidlclass* get_class(const std::string& name) {
		for (std::vector<zidlclass*>::iterator i = classes.begin(); i != classes.end(); ++i) {
			if ((*i)->name == name) return *i;
		}
		return 0;
	}

	zidlclassmethod* get_callback(const std::string& name) {
		for (std::vector<zidlclassmethod*>::iterator i = callbacks.begin(); i != callbacks.end(); ++i) {
			if ((*i)->name == name) return *i;
		}
		return 0;
	}
};


struct zidlprogram : zidlunit {
	std::vector<std::string> cheaders;
	std::vector<zidlnamespace*> namespaces;
	std::vector<std::string> includes;

	zidlprogram():zidlunit(zidltype_program) {
	}

	zidlnamespace* get_namespace(const std::string& name) {
		for (std::vector<zidlnamespace*>::iterator i = namespaces.begin(); i != namespaces.end(); ++i) {
			if ((*i)->name == name) return *i;
		}
		return 0;
	}
};

bool parse_zidl_program(const node* n, zidlprogram* result, bool importing);
