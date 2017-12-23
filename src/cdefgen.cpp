#include "zidlparser.h"
#include "zidlintermediate.h"
#include "zidlgenerator.h"
#include "zidlhelper.h"

using std::endl;
using namespace zidlhelper;

struct cdefgen : zidlgenerator{

	void generate_method(std::ostream& strm, zidlclassmethod& m) {
		strm << indent(1) << get_c_method_name(m) << endl;
	}

	void generate_class(std::ostream& strm, zidlclass& c) {
		for (std::vector<zidlclass*>::const_iterator i = c.classes.begin(); i != c.classes.end(); ++i) {
			generate_class(strm, **i);
		}
		for (std::vector<zidlclassmethod*>::const_iterator i = c.methods.begin(); i != c.methods.end(); ++i) {
			generate_method(strm, **i);
		}
	}

	void generate_namespace(std::ostream& strm, const zidlnamespace& ns) {
		for (std::vector<zidlunit*>::const_iterator i = ns.content.begin(); i != ns.content.end(); ++i) {
			switch ((*i)->type) {
				case zidltype_class:
					generate_class(strm, (zidlclass&)**i);
					break;
			}
		}
	}

	void generate(std::ostream& strm, zidlprogram& prog) {
		strm << "EXPORTS" << endl;
		for (std::vector<zidlnamespace*>::const_iterator i = prog.namespaces.begin(); i != prog.namespaces.end(); ++i) {
			zidlnamespace& ns = **i;
			if (ns.is_imported) continue;
			generate_namespace(strm, ns);
		}
	}
};

zidlgenerator* zidlgenerator::create_cdefgen() {
	return new cdefgen();
}
