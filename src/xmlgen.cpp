#include <iostream>
#include <cassert>
#include <fstream>
#include "zidlparser.h"
#include "zidlintermediate.h"
#include "zidlgenerator.h"
#include "zidlhelper.h"

using std::cout;
using std::endl;
using namespace zidlhelper;

struct xmlgen : zidlgenerator {

	void generate_method(std::ostream& strm, int level, zidlclassmethod& m) {
		strm << indent(level) << "<method>" << endl;
		strm << indent(level + 1) << "<name>" << get_c_method_name(m) << "</name>" << endl;
		for (std::vector<std::string>::const_iterator i = m.comments.begin(); i != m.comments.end(); ++i) {
			strm << indent(level + 1) << "<description>" << *i << "</description>" << endl;
		}
		strm << indent(level + 1) << "<returns>" << get_c_type_name(m.returntype) << "</returns>" << endl;
		if (m.arguments.size() > 0 || !m.is_static) {
			strm << indent(level + 1) << "<arguments>" << endl;
			if (!m.is_static) {
				strm << indent(level + 2) << "<argument>" << endl;
				strm << indent(level + 3) << "<type>" << get_c_class_name(*m.parent) << "*</type>" << endl;
				strm << indent(level + 3) << "<name>" << camel_to_posix(m.parent->name) << "</name>" << endl;
				strm << indent(level + 2) << "</argument>" << endl;
			}
			for (std::vector<zidlargument>::const_iterator i = m.arguments.begin(); i != m.arguments.end(); ++i) {
				strm << indent(level + 2) << "<argument>" << endl;
				strm << indent(level + 3) << "<type>" << get_c_type_name(i->argtype) << "</type>" << endl;
				strm << indent(level + 3) << "<name>" << i->name << "</name>" << endl;
				strm << indent(level + 2) << "</argument>" << endl;
			}
			strm << indent(level + 1) << "</arguments>" << endl;
		} else {
			strm << indent(level + 1) << "<arguments />" << endl;
		}
		strm << indent(level) << "</method>" << endl;
	}

	void generate_class(std::ostream& strm, int level, zidlclass& c) {
		strm << indent(level) << "<class>" << endl;
		strm << indent(level + 1) << "<name>" << get_c_class_name(c) << "</name>" << endl;
		for (std::vector<std::string>::const_iterator i = c.comments.begin(); i != c.comments.end(); ++i) {
			strm << indent(level + 1) << "<description>" << *i << "</description>" << endl;
		}
		for (std::vector<zidlclass*>::const_iterator i = c.classes.begin(); i != c.classes.end(); ++i) {
			generate_class(strm, level + 1, **i);
		}
		for (std::vector<zidlclassmethod*>::const_iterator i = c.methods.begin(); i != c.methods.end(); ++i) {
			generate_method(strm, level + 1, **i);
		}
		strm << indent(level) << "</class>" << endl;
	}

	void generate_enum(std::ostream& strm, int level, zidlenum& c) {
		strm << indent(level) << "<enum>" << endl;
		if (!c.name.empty())
			strm << indent(level + 1) << "<name>" << get_c_class_name(c) << "</name>" << endl;
		for (std::vector<std::string>::const_iterator i = c.comments.begin(); i != c.comments.end(); ++i) {
			strm << indent(level + 1) << "<description>" << *i << "</description>" << endl;
		}
		for (std::vector<zidlenumvalue>::const_iterator i = c.values.begin(); i != c.values.end(); ++i) {
			strm << indent(level + 1) << "<enumvalue>" << endl;
			strm << indent(level + 2) << "<name>" << endl;
			strm << indent(level + 3) << get_c_class_prefix(c) << "_" << i->name << endl;
			strm << indent(level + 2) << "</name>" << endl;
			strm << indent(level + 2) << "<value>" << endl;
			strm << indent(level + 3) << i->value << endl;
			strm << indent(level + 2) << "</value>" << endl;
			strm << indent(level + 1) << "</enumvalue>" << endl;
		}
		strm << indent(level) << "</enum>" << endl;
	}

	void generate_namespace(std::ostream& strm, const zidlnamespace& ns) {
		strm << indent(1) << "<namespace>" << endl;
		for (std::vector<std::string>::const_iterator i = ns.comments.begin(); i != ns.comments.end(); ++i) {
			strm << indent(2) << "<description>" << *i << "</description>" << endl;
		}
		strm << indent(2) << "<dlname>" << ns.dlname << "</dlname>" << endl;
		for (std::vector<zidlunit*>::const_iterator i = ns.content.begin(); i != ns.content.end(); ++i) {

			switch ((*i)->type) {
				case zidltype_classmethod:
					// TODO
					break;
				case zidltype_enum:
					generate_enum(strm, 2, (zidlenum&)**i);
					break;
				case zidltype_class:
					generate_class(strm, 2, (zidlclass&)**i);
					break;
			}

		}
		strm << indent(1) << "</namespace>" << endl;
	}

	void generate(std::ostream& strm, zidlprogram& prog) {
		strm << "<zidl>" << endl;
		for (std::vector<zidlnamespace*>::const_iterator i = prog.namespaces.begin(); i != prog.namespaces.end(); ++i) {
			zidlnamespace& ns = **i;
			if (ns.is_imported) continue;
			generate_namespace(strm, ns);
		}
		strm << "</zidl>" << endl;
	}

};

zidlgenerator* zidlgenerator::create_xmlgen() {
	return new xmlgen();
}
