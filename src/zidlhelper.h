#pragma once

namespace zidlhelper {
	std::string indent(int c);
	std::string get_c_class_prefix(const zidlunit& c);
	std::string get_c_class_name(const zidlunit& c);
	std::string get_builtin_c_type_name(const zidltyperef& n);
	std::string get_c_type_name(const zidltyperef& t);
	std::string get_cpp_type_name(const zidltyperef& t);
	std::string get_c_method_name(const zidlclassmethod& m);
	std::string get_pyctypes_type_name(const zidltyperef& t);
	std::string get_pyctypes_union_type_name(const zidltyperef& t);
	zidlnamespace* find_parent_namespace(zidlunit& t);
	std::string get_cs_type_name(const zidltyperef& t);
	std::string get_cs_flat_type_name(const zidltyperef& t);
	std::string get_cs_class_name(const zidlunit& c);

	const zidltyperef* get_array_base_type(const zidltyperef& arrayref);
};
