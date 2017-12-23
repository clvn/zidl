/*

lua output for zidl

TODO:
	- generate lua classes with type checking, instance maps etc (output in .lua, in a separate generator)
	- setters for zidl classes with data and unions
	- luajit binding generator, generate ffi bindings for dlls with zidl-api
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
#include <set>
#include "zidlparser.h"
#include "zidlintermediate.h"
#include "zidlgenerator.h"
#include "zidlhelper.h"

using std::endl;
using std::cout;
using namespace zidlhelper;

struct luagen : zidlgenerator{

	void push_data(std::ostream& strm, const zidltyperef& t, const std::string& varname) {
		/*if (t.is_array) {
			convert c array to lua array of N dimensionality
			create and call helpers like this:
				void lua_array_from_c_array(lua_State* L, char* data, int length0) {}
				void lua_array_from_c_array(lua_State* L, char* data, int length0, int length1) {}
		} else*/ if (t.classref != 0) {
			strm << "\tif (" << varname << " != NULL)" << endl;
			strm << "\t\tlua_pushlightuserdata(L, " << varname << ");" << endl;
			strm << "\telse" << endl;
			strm << "\t\tlua_pushnil(L);" << endl;
		} else if (t.name == "int" || t.name == "uint") {
			strm << "\tlua_pushnumber(L, ";
			if (t.is_out) strm << "*";
			strm << varname << ");" << endl;
		} else if (t.name == "string") {
			strm << "\tlua_pushstring(L, " << varname << ");" << endl;
		} else if (t.name == "bool") {
			strm << "\tlua_pushnumber(L, ";
			if (t.is_out) strm << "*";
			strm << varname << ");" << endl;
		} else if (t.name == "float" || t.name == "double") {
			strm << "\tlua_pushnumber(L, ";
			if (t.is_out) strm << "*";
			strm << varname << ");" << endl;
		} else if (t.name == "short" || t.name == "ushort") {
			strm << "\tlua_pushnumber(L, ";
			if (t.is_out) strm << "*";
			strm << varname << ");" << endl;
		} else if (t.name == "uchar" || t.name == "char" || t.name == "byte") {
			strm << "\tlua_pushnumber(L, ";
			if (t.is_out) strm << "*";
			strm << varname << ");" << endl;
		} else if (t.name == "pvoid") {
			strm << "\tlua_pushlightuserdata(L, " << varname << ");" << endl;
		} else if (t.name == "void") { 
			; //OK
		} else {
			assert(false);
			strm << "\tlua_pushnumber(L, 0); // TODO: not supported type: " << t.name << endl;
		}
	}

	void get_data(std::ostream& strm, const zidltyperef& t, int index) {
		if (t.classref) {
			strm << "(" << get_c_type_name(t) << ")";
			strm << "lua_touserdata(L, " << index << ");" << endl;
		} else if (t.callbackref) {
			strm << get_c_class_prefix(*t.callbackref) << "_alloc(L, " << index << ");" << endl;
		} else if (t.name == "string") {
			strm << "luaL_tolstring(L, " << index << ", &len);" << endl;
		} else if (t.name == "int" || t.name == "uint") {
			strm << "lua_tonumber(L, " << index << ");" << endl;
		} else if (t.name == "short" || t.name == "ushort") {
			strm << "lua_tonumber(L, " << index << ");" << endl;
		} else if (t.name == "char" || t.name == "uchar" || t.name == "byte") {
			strm << "lua_tonumber(L, " << index << ");" << endl;
		} else if (t.name == "float" || t.name == "double") {
			strm << "lua_tonumber(L, " << index << ");" << endl;
		} else if (t.name == "bool") {
			strm << "lua_toboolean(L, " << index << ");" << endl;
		} else if (t.name == "pvoid") {
			strm << "lua_touserdata(L, " << index << ");" << endl;
		} else
			assert(false);
	}

	void generate_classes_vtbl_memberlist(std::ostream& strm, zidlclass& c, std::vector<zidlclassmember*>& members) {
		for (std::vector<zidlclassmember*>::iterator cmit = members.begin(); cmit != members.end(); ++cmit) {
			zidlclassmember& m = **cmit;
			strm << "static int " << camel_to_posix(c.name) << "_get_" << m.name << "(lua_State *L) {" << endl;

			strm << "\t" << get_c_class_name(c) << "* self = (" << get_c_class_name(c) << "*)";
			strm << "lua_touserdata(L, 2);" << endl;
			strm << "\tif (self == 0) {" << endl;
			strm << "\t\tluaL_error(L, \"Null self argument for " << c.name << ":" << m.name << "\");" << endl;
			strm << "\t\treturn 0;" << endl;
			strm << "\t}" << endl << endl;

			strm << "\t" << get_c_type_name(m.membertype);
			if (m.membertype.no_ref) strm << "*";
			strm << " _luaresult = ";
			if (m.membertype.no_ref) strm << "&";

			std::string membername = camel_to_posix(m.name);
			strm << "self->" << membername << ";" << endl;

			push_data(strm, m.membertype, "_luaresult");

			strm << "\treturn 1;" << endl;
			strm << "};" << endl << endl;
		}
	}

	void generate_classes_vtbl(std::ostream& strm, zidlclass& c) {
		for (std::vector<zidlclass*>::iterator cit = c.classes.begin(); cit != c.classes.end(); ++cit) {
			generate_classes_vtbl(strm, **cit);
		}

		// generate getters and setters for props
		generate_classes_vtbl_memberlist(strm, c, c.members);

		for (std::vector<zidlclassunion*>::iterator cuit = c.unions.begin(); cuit != c.unions.end(); ++cuit) {
			zidlclassunion& u = **cuit;
			generate_classes_vtbl_memberlist(strm, c, u.members);
		}


		for (std::vector<zidlclassmethod*>::iterator cmit = c.methods.begin(); cmit != c.methods.end(); ++cmit) {
			zidlclassmethod& m = **cmit;
			if (m.returntype.no_python) continue;

			// need a "callback callback" for each function taking a callback parameter
			// in order to capture callback parameters in the embedding host, such that 
			// callbacks can be cleaned up with custom code: there is no way to know how
			// a library uninstalls callbacks from the zidl interface.

			if (m.has_callback_arguments()) {
				strm << "void(*luacallback_" << get_c_method_name(m) << ")(lua_State* L, ";
				if (!m.is_static) 
					strm << get_c_class_name(c) << "* " << camel_to_posix(c.name);

				for (std::vector<zidlargument>::const_iterator i = m.arguments.begin(); i != m.arguments.end(); ++i) {
					if (!m.is_static || i != m.arguments.begin())
						strm << ", ";
					strm << get_c_type_name(i->argtype);
					if (!i->argtype.is_array && i->argtype.is_out)
						strm << "*";
					strm << " " << i->name;
				}
				strm << ");" << endl << endl;
			}

			int paramcount = (int)m.arguments.size() + (m.is_static ? 1 : 2);
			strm << "static int " << camel_to_posix(c.name) << "_" << m.name << "(lua_State *L) {" << endl;

			strm << "\tsize_t len;" << endl;
			strm << "\tint argc = lua_gettop(L);" << endl;
			strm << "\tif (argc != " << paramcount << ") {" << endl;
			strm << "\t\tluaL_error(L, \"Invalid argument count for " << c.name << ":" << m.name << "\");" << endl;
			strm << "\t\treturn 0;" << endl;
			strm << "\t}" << endl;

			if (!m.is_static) {
				strm << "\t" << get_c_class_name(c) << "* self = (" << get_c_class_name(c) << "*)";
				strm << "lua_touserdata(L, 2);" << endl;
				strm << "\tif (self == 0) {" << endl;
				strm << "\t\tluaL_error(L, \"Null self argument for " << c.name << ":" << m.name << "\");" << endl;
				strm << "\t\treturn 0;" << endl;
				strm << "\t}" << endl << endl;

			}

			// load lua arguments into c variables
			int outparameters = 0;
			for (std::vector<zidlargument>::iterator i = m.arguments.begin(); i != m.arguments.end(); ++i) {
				int argindex = (int)std::distance(m.arguments.begin(), i) + (m.is_static ? 2 : 3);

				if (i->argtype.is_out) {
					// declare a variable to hold the output, skip the input parameter?
					strm << "\t" << get_c_type_name(i->argtype) << " " << i->name << ";" << endl;
					//outparameters++;
				} else if (i->argtype.is_array) {
					// lua array -> c array (also free the array later, after its been used)
					strm << "\t" << get_c_type_name(i->argtype) << " " << i->name << ";";
				} else {
					strm << "\t" << get_c_type_name(i->argtype) << " " << i->name << " = ";
					get_data(strm, i->argtype, argindex);
				}
			}

			// load lua arrays into c variables, need to parse all nonarray arguments first to get length arguments etc
			for (std::vector<zidlargument>::iterator i = m.arguments.begin(); i != m.arguments.end(); ++i) {
				if (i->argtype.is_array) {

				}
			}

			// call callback callback
			if (m.has_callback_arguments()) {
				strm << "\tif (luacallback_" << get_c_method_name(m) << ") luacallback_" << get_c_method_name(m) << "(L, ";
				if (!m.is_static) 
					strm << "self";

				for (std::vector<zidlargument>::const_iterator i = m.arguments.begin(); i != m.arguments.end(); ++i) {
					if (!m.is_static || i != m.arguments.begin())
						strm << ", ";
					strm << " " << i->name;
				}
				strm << ");" << endl;
			}

			strm << "\t";
			if (m.returntype.name != "void") {
				strm << get_c_type_name(m.returntype) << " ";
				strm << "_luaresult = ";
			}
			
			strm << get_c_method_name(m) << "(";
			if (!m.is_static) {
				strm << "self";
			}

			for (std::vector<zidlargument>::iterator i = m.arguments.begin(); i != m.arguments.end(); ++i) {
				if (i != m.arguments.begin() || !m.is_static)
					strm << ", ";
				if (!i->argtype.is_array && i->argtype.is_out && i->argtype.name != "string")
					strm << "&";
				strm << i->name;
			}

			strm << ");" << endl;

			int returnvalues = 0;
			if (m.returntype.name != "void")
				returnvalues = 1; // TODO: argument outputs

			push_data(strm, m.returntype, "_luaresult");
			
			strm << "\treturn " << returnvalues << ";" << endl;
			strm << "}" << endl << endl;
		}

		if (!c.methods.empty() || !c.members.empty() || !c.unions.empty()) {
			strm << "static const luaL_Reg " << camel_to_posix(c.name) << "_lib[] = {" << endl;

			generate_classes_reg_memberlist(strm, c, c.members);

			for (std::vector<zidlclassunion*>::iterator cuit = c.unions.begin(); cuit != c.unions.end(); ++cuit) {
				zidlclassunion& u = **cuit;
				generate_classes_reg_memberlist(strm, c, u.members);
			}

			for (std::vector<zidlclassmethod*>::iterator i = c.methods.begin(); i != c.methods.end(); ++i) {
				zidlclassmethod* m = *i;
				if (m->returntype.no_python)
					continue;
				strm << "\t{ \"" << m->name << "\", " << camel_to_posix(c.name) << "_" << m->name << " }," << endl;
			}
			strm << "\t{ NULL, NULL }" << endl;
			strm << "};" << endl << endl;
		}
	}

	void generate_classes_reg_memberlist(std::ostream& strm, zidlclass& c, std::vector<zidlclassmember*>& members) {
		for (std::vector<zidlclassmember*>::iterator cmit = members.begin(); cmit != members.end(); ++cmit) {
			zidlclassmember& m = **cmit;
			strm << "\t{ \"get_" << m.name << "\", " << camel_to_posix(c.name) << "_get_" << m.name << " }, " << endl;
		}
	}


	void generate_classes_callback_class(std::ostream& strm, zidlclassmethod& m) {
		int numcallbacks = 500;
		int numreturnvalues = m.returntype.name != "void" ? 1 : 0;

		strm << "struct " << get_c_class_prefix(m) << "_data {" << endl;
		strm << "\tlua_State *L;" << endl;
		strm << "\tint functionref;" << endl;
		strm << "\tconst void* ptr;" << endl;
		strm << "};" << endl << endl;

		strm << get_c_class_prefix(m) << "_data " << get_c_class_prefix(m) << "_datas[" << numcallbacks << "];" << endl << endl;

		strm << "template <int N>" << endl;
		strm << get_c_type_name(m.returntype) << " " << get_c_class_prefix(m) << "_template(";
		for (std::vector<zidlargument>::const_iterator i = m.arguments.begin(); i != m.arguments.end(); ++i) {
			if (!m.is_static || i != m.arguments.begin())
				strm << ", ";
			strm << get_c_type_name(i->argtype);
			if (!i->argtype.is_array && i->argtype.is_out)
				strm << "*";
			strm << " " << i->name;
		}
		strm << ") {" << endl;
		strm << "\tlua_State* L = " << get_c_class_prefix(m) << "_datas[N].L;" << endl;
		strm << "\tlua_rawgeti(L, LUA_REGISTRYINDEX, " << get_c_class_prefix(m) << "_datas[N].functionref);" << endl;

		for (std::vector<zidlargument>::const_iterator i = m.arguments.begin(); i != m.arguments.end(); ++i) {
			push_data(strm, i->argtype, i->name);
		}

		int numparameters = (int)m.arguments.size();
		strm << "\tint status = lua_pcall(L, " << numparameters << ", " << numreturnvalues << ", 0);" << endl;
		strm << "\treport_errors(L, status);" << endl;

		if (numreturnvalues > 0) {
			strm << "\treturn ";
			get_data(strm, m.returntype, -1);
		}

		strm << "}" << endl << endl;


		strm << get_c_class_name(m) << " " << get_c_class_prefix(m) << "_callbacks[] = {" << endl;
		for (int i = 0; i < numcallbacks; i++) {
			strm << "\t&" << get_c_class_prefix(m) << "_template<" << i << ">," << endl;
		}
		strm << "};" << endl << endl;

		strm << get_c_class_name(m) << " " << get_c_class_prefix(m) << "_alloc(lua_State* L, int index) {" << endl;
		strm << "\tconst void* ptr = lua_topointer(L, index);" << endl;
		strm << "\tassert(ptr != 0);" << endl;
		strm << "\tfor (int i = 0; i < " << numcallbacks << "; ++i) {" << endl;
		strm << "\t\tif (ptr == " << get_c_class_prefix(m) << "_datas[i].ptr) {" << endl;
		strm << "\t\t\treturn " << get_c_class_prefix(m) << "_callbacks[i];" << endl;
		strm << "\t\t}" << endl;
		strm << "\t}" << endl;
		strm << "\tlua_pushvalue(L, index);" << endl;
		strm << "\tint functionref = luaL_ref(L, LUA_REGISTRYINDEX);" << endl;
		strm << "\tfor (int i = 0; i < " << numcallbacks << "; ++i) {" << endl;
		strm << "\t\tif (" << get_c_class_prefix(m) << "_datas[i].L == 0) {" << endl;
		strm << "\t\t\t" << get_c_class_prefix(m) << "_datas[i].L = L;" << endl;
		strm << "\t\t\t" << get_c_class_prefix(m) << "_datas[i].ptr = ptr;" << endl;
		strm << "\t\t\t" << get_c_class_prefix(m) << "_datas[i].functionref = functionref;" << endl;
		strm << "\t\t\treturn " << get_c_class_prefix(m) << "_callbacks[i];" << endl;
		strm << "\t\t}" << endl;
		strm << "\t}" << endl;
		strm << "\treturn 0;" << endl;
		strm << "}" << endl << endl;

		strm << "void " << get_c_class_prefix(m) << "_clear(lua_State* L) {" << endl;
		strm << "\tfor (int i = 0; i < " << numcallbacks << "; ++i) {" << endl;
		strm << "\t\tif (L == " << get_c_class_prefix(m) << "_datas[i].L) {" << endl;
		strm << "\t\t\t" << get_c_class_prefix(m) << "_datas[i].L = 0;" << endl;
		strm << "\t\t\t" << get_c_class_prefix(m) << "_datas[i].ptr = 0;" << endl;
		strm << "\t\t\t" << get_c_class_prefix(m) << "_datas[i].functionref = 0;" << endl;
		strm << "\t\t}" << endl;
		strm << "\t}" << endl;
		strm << "}" << endl << endl;
	}

	void generate_classes_lib(std::ostream& strm, zidlclass& c) {
		for (std::vector<zidlclass*>::iterator i = c.classes.begin(); i != c.classes.end(); ++i) {
			generate_classes_lib(strm, **i);
		}

		if (!c.methods.empty() || !c.members.empty() || !c.unions.empty()) {
			strm << "\tluaL_newlib(L, " << camel_to_posix(c.name) << "_lib);" << endl;
			strm << "\tlua_setglobal(L, \"" << get_c_class_prefix(c) << "\");" << endl << endl;
		}
	}

	void generate_classes(std::ostream& strm, zidlnamespace& ns) {

		for (std::vector<zidlclassmethod*>::iterator i = ns.callbacks.begin(); i != ns.callbacks.end(); ++i) {
			generate_classes_callback_class(strm, **i);
		}

		for (std::vector<zidlclass*>::iterator i = ns.classes.begin(); i != ns.classes.end(); ++i) {
			generate_classes_vtbl(strm, **i);
		}

		strm << "int luaopen_" << ns.name << "(lua_State* L) {" << endl;

		for (std::vector<zidlenum*>::iterator i = ns.enums.begin(); i != ns.enums.end(); ++i) {
			zidlenum& e = **i;
			strm << "\tlua_newtable(L);" << endl;
			for (std::vector<zidlenumvalue>::iterator v = e.values.begin(); v != e.values.end(); ++v) {
				strm << "\tlua_pushliteral(L, \"" << v->name << "\"); ";
				strm << "lua_pushnumber(L, " << v->value << "); ";
				strm << "lua_settable(L, -3);" << endl;
			}
			strm << "\tlua_setglobal(L, \"" << get_c_class_prefix(e) << "\");" << endl << endl;
		}

		for (std::vector<zidlclass*>::iterator i = ns.classes.begin(); i != ns.classes.end(); ++i) {
			zidlclass& c = **i;
			generate_classes_lib(strm, c);
			//strm << "\tluaL_openlib(L, \"" << c.name << "\", " << c.name << "_lib, 0);" << endl;
		}
		strm << "\treturn 1;" << endl;
		strm << "}" << endl;

		// generate luaclose_libname() to clear callbacks for a finished context
		strm << "void luaclose_" << ns.name << "(lua_State* L) {" << endl;
		// for each callback
		for (std::vector<zidlclassmethod*>::iterator i = ns.callbacks.begin(); i != ns.callbacks.end(); ++i) {
			strm << "\t" << get_c_class_prefix(**i) << "_clear(L);" << endl;
		}
		strm << "}" << endl;

	}

	void generate(std::ostream& strm, zidlprogram& prog) {
		if (prog.cheaders.empty()) {
			strm << "// ERROR: input zidl did not define a cheader" << endl;
		}
		for (std::vector<std::string>::iterator i = prog.cheaders.begin(); i != prog.cheaders.end(); ++i) {
			strm << "#include <" << *i << ">" << endl;
		}
		strm << "#include <cassert>" << endl;
		strm << "extern \"C\" {" << endl;
		strm << "#include <lua.h>" << endl;
		strm << "#include <lualib.h>" << endl;
		strm << "#include <lauxlib.h>" << endl;
		strm << "}" << endl << endl;

		strm << "extern void report_errors(lua_State *L, int status);" << endl << endl;

		for (std::vector<zidlnamespace*>::const_iterator i = prog.namespaces.begin(); i != prog.namespaces.end(); ++i) {
			zidlnamespace& ns = **i;
			if (ns.is_imported) {
				//strm << "from " << ns.dlname << " import *" << endl;
			} else {

				generate_classes(strm, ns);

			}
		}
	}
};

zidlgenerator* zidlgenerator::create_luagen() {
	return new luagen();
}
