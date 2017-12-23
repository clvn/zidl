/*

todo:

	- posix_to_camel -> PosixToCamel
	- stringbuilders for out strings marshaling internally and ref strings publicly
	- array of handles (save_armz(), create_plugininfo(), .. arrays in general?)
	- enums
	- events / structs marshaling
	- callbacks

*/


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
#include "zidlparser.h"
#include "zidlintermediate.h"
#include "zidlgenerator.h"
#include "zidlhelper.h"

using std::endl;
using namespace zidlhelper;

struct dotnetgen : zidlgenerator {

	void generate(std::ostream& strm, zidlprogram& prog) {
		for (std::vector<zidlnamespace*>::const_iterator i = prog.namespaces.begin(); i != prog.namespaces.end(); ++i) {
			zidlnamespace& ns = **i;
			if (ns.is_imported) continue;
			//generate_header(strm);
			generate_flat(strm, ns);
			generate_classes(strm, ns);
			//generate_enums(strm, n);
			//generate_structs(strm, n);
			//generate_footer(strm);
		}
	}

	void generate_flat(std::ostream& strm, zidlnamespace& ns) {
		
		strm << "using System;" << endl;
		strm << "using System.Text;" << endl;
		strm << "using System.Runtime.InteropServices;" << endl;
		strm << "using System.Collections.Generic;" << endl << endl;
		strm << "namespace NArmstrong {" << endl;

		strm << "\tpublic class " << ns.dlname << " {" << endl;

		for (std::vector<zidlunit*>::const_iterator i = ns.content.begin(); i != ns.content.end(); ++i) {
			zidlunit* child = *i;
			switch (child->type) {
				case zidltype_classmethod:
					generate_flat_method(strm, (zidlclassmethod&)**i);
					break;
				case zidltype_class:
					generate_flat_class(strm, (zidlclass&)**i);
					break;
			}
		}

		strm << "\t}" << endl;
		strm << "}" << endl;
	}

	void generate_flat_class(std::ostream& strm, zidlclass& c) {
		// flatten classes
		for (std::vector<zidlclass*>::iterator i = c.classes.begin(); i != c.classes.end(); ++i) {
			generate_flat_class(strm, **i);
		}

		for (std::vector<zidlclassmethod*>::iterator i = c.methods.begin(); i != c.methods.end(); ++i) {
			generate_flat_method(strm, **i);
		}
	}

	void generate_flat_method(std::ostream& strm, zidlclassmethod& m) {

		if (m.is_callback) {
			std::string membername = get_cs_class_name(m);//.name;
			strm << "\t\tpublic delegate " << get_cs_flat_type_name(m.returntype) << " " << membername << "(";
		} else {
			std::string membername = get_c_method_name(m);//.name;
			strm << "\t\t[DllImport(@\"armstrong\", EntryPoint=\"" << membername << "\")]" << endl;
			strm << "\t\tpublic static extern " << get_cs_flat_type_name(m.returntype) << " " << membername << "(";
		}

		// write self argument on non-static member methods
		if (!m.is_static)
			strm << "IntPtr " << m.parent->name;

		for (std::vector<zidlargument>::iterator i = m.arguments.begin(); i != m.arguments.end(); ++i) {
			if (!m.is_static || i != m.arguments.begin())
				strm << ", ";
			if (i->argtype.is_out)
				strm << "out ";
			strm << get_cs_flat_type_name(i->argtype) << " " << i->name ;
		}
		strm << ");" << endl << endl;
	
	}



	void generate_classes(std::ostream& strm, zidlnamespace& ns) {

		strm << "namespace NArmstrong {" << endl;

		for (std::vector<zidlunit*>::iterator i = ns.content.begin(); i != ns.content.end(); ++i) {
			zidlunit* child = *i;
			switch (child->type) {
				case zidltype_classmethod:
					generate_classes_method(strm, (zidlclassmethod&)*child);
					break;
				case zidltype_class:
					generate_classes_class(strm, (zidlclass&)**i);
					break;
			}
		}

		strm << "}" << endl;
	}

	void generate_classes_member(std::ostream& strm, zidlclassmember& m) {
		strm << "\tpublic " << get_cs_flat_type_name(m.membertype) << " " << m.name << ";" << endl;
	}

	void generate_classes_union_member(std::ostream& strm, zidlclassmember& m) {
		strm << "\t[FieldOffset(0)] public " << get_cs_flat_type_name(m.membertype) << " " << m.name << ";" << endl;
	}

	void generate_classes_union(std::ostream& strm, zidlclassunion& u) {
		strm << "[StructLayout(LayoutKind.Explicit)]" << endl;
		strm << "public struct " << u.name << " {" << endl;
		for (std::vector<zidlclassmember*>::iterator i = u.members.begin(); i != u.members.end(); ++i) {
			generate_classes_union_member(strm, **i);
		}
		strm << "}" << endl << endl;
	}

	void generate_classes_class(std::ostream& strm, zidlclass& c) {

		// TODO: data vs interface
		if (c.methods.empty()) {

			strm << "[StructLayout(LayoutKind.Sequential)]" << endl;
			strm << "public struct " << c.name << " {" << endl;

			for (std::vector<zidlclass*>::iterator i = c.classes.begin(); i != c.classes.end(); ++i) {
				generate_classes_class(strm, **i);
			}

			for (std::vector<zidlclassunion*>::iterator i = c.unions.begin(); i != c.unions.end(); ++i) {
				generate_classes_union(strm, **i);
			}

			for (std::vector<zidlclassunion*>::iterator i = c.unions.begin(); i != c.unions.end(); ++i) {
				strm << "\t" << (*i)->name << " " << (*i)->name << "union;" << endl;
			}

			for (std::vector<zidlclassmember*>::iterator i = c.members.begin(); i != c.members.end(); ++i) {
				generate_classes_member(strm, **i);
			}
			// 
			strm << "}" << endl << endl;
		} else {

			strm << "public class " << c.name << " {" << endl << endl;
			strm << "\tinternal IntPtr handle;" << endl;
			strm << "\tinternal static Dictionary<IntPtr, " << c.name << "> handles = new Dictionary<IntPtr, " << c.name << ">();" << endl << endl;

			strm << "\tinternal " << c.name << "(IntPtr key) {" << endl;
			strm << "\t\thandle = key;" << endl;
			strm << "\t}" << endl << endl;

			strm << "\tinternal static " << c.name << " GetInstance(IntPtr key) {" << endl;
			strm << "\t\t" << c.name << " inst;" << endl;
			strm << "\t\tif (handles.ContainsKey(key)) return handles[key];" << endl;
			strm << "\t\thandles.Add(key, inst = new " << c.name << "(key));" << endl;
			strm << "\t\treturn inst;" << endl;
			strm << "\t}" << endl << endl;

			// flatten classes
			for (std::vector<zidlclass*>::iterator i = c.classes.begin(); i != c.classes.end(); ++i) {
				generate_classes_class(strm, **i);
			}

			for (std::vector<zidlclassmethod*>::iterator i = c.methods.begin(); i != c.methods.end(); ++i) {
				generate_classes_method(strm, **i);
			}

			strm << "}" << endl << endl;
		}
	}

	void generate_classes_method(std::ostream& strm, zidlclassmethod& m) {
		std::string membername = m.name;

		strm << "\t";
		
		bool isuserrettype = m.returntype.classref != 0 || m.returntype.callbackref != 0;
		std::string typname = get_cs_type_name(m.returntype); //format_dotnet_type_name(n->metatype, n->parent, true, &isuserrettype);

		if (m.is_callback) {
			// use the flat - or generate one that converts
			return ;
			//strm << "public delegate " << typname << " " << membername << "(";
		} else {
			if (m.is_static)
				strm << "static ";
			strm << "public " << typname << " " << membername << "(";
		}
		// write self argument on non-static member methods
		//if (!mods.is_static)
		//	strm << "IntPtr " << camel_to_posix(classname);

		for (std::vector<zidlargument>::iterator i = m.arguments.begin(); i != m.arguments.end(); ++i) {
			if (i != m.arguments.begin())
				strm << ", ";
			if (i->argtype.is_out)
				strm << "out ";
			strm << get_cs_type_name(i->argtype) << " " << i->name;
		}
		strm << ")";
		
		if (m.is_callback) {
			strm << ";" << endl;
		} else {
			strm << "{" << endl;

			// TODO: must convert object array arguments to arrays of intptr handles
			// TODO: fix stringbuilder->ref string arguments

			// any arguments that need conversion first?

			for (std::vector<zidlargument>::iterator i = m.arguments.begin(); i != m.arguments.end(); ++i) {
				if (i->argtype.classref != 0 && i->argtype.is_array) {
					strm << "\t\tIntPtr[] arg_" << i->name << ";" << endl;
				}
			}

			strm << "\t\t";
			if (typname != "void")
				strm << "return ";

			std::string callmembername = get_c_method_name(m); //camel_to_posix(dlname, classname, n->name);

			if (isuserrettype) strm << m.returntype.name << ".GetInstance(";

			zidlnamespace* ns = find_parent_namespace(m);
			std::string dlname = ns->dlname;
			strm << dlname << "." << callmembername << "(";
			if (!m.is_static)
				strm << "handle";

			for (std::vector<zidlargument>::iterator i = m.arguments.begin(); i != m.arguments.end(); ++i) {
				if (!m.is_static || i != m.arguments.begin())
					strm << ", ";
				//bool isuserarg = i->argtype.callbackref != 0 || i->argtype.classref != 0;

				//strm << format_dotnet_type_modifier(child->metatype, n->parent) << child->name;
				if (i->argtype.is_out)
					strm << "out ";

				if (i->argtype.classref != 0 && i->argtype.is_array)
					strm << "arg_" << i->name;
				else
					strm << i->name;

				if (i->argtype.classref != 0 && !i->argtype.is_array && !i->argtype.classref->methods.empty())
					strm << ".handle";
			}

			strm << ")";
			if (isuserrettype) strm << ")";
			strm << ";" << endl;

			strm << "\t}" << endl << endl;
		}
	}
};


zidlgenerator* zidlgenerator::create_dotnetgen() {
	return new dotnetgen();
}


