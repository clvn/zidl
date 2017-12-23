/*
about python lexer indentation
http://docs.python.org/release/2.5.2/ref/indentation.html
*/
#include <iostream>
#include <cassert>
#include <fstream>
#include "zidlparser.h"
#include "zidlintermediate.h"
#include "zidlgenerator.h"

using std::cout;
using std::endl;


int main(int argc, char** argv) {

	if (argc != 4) {
		cout << "usage: zidl [inputfile.zidl] [cheader|cdef|py|xml] [outputfile|.]" << endl;
		return 1;
	}

	std::string inputfile = argv[1]; // "zzub.zidl"
	std::string method = argv[2];
	std::string outputfile = argv[3]; // "c:/code/buze/trunk/buze/src/armstrong/include/zzub/zzub.h"

	int errorline;
	node* zidlroot = zidl_parse_file(inputfile, &errorline);
	if (!zidlroot) {
		cout << "Could not load input or parse error at line " << (errorline + 1) << endl;
		return 4;
	}

	// convert the parse tree into a zidl intermediate representation which can be converted to the various source outputs
	zidlprogram prog;
	if (!parse_zidl_program(zidlroot, &prog, false)) {
		cout << "Could not interpret zidl program." << endl;
		delete zidlroot;
		return 5;
	}
	delete zidlroot;

	std::ofstream fs;
	std::ostream* outstrm;

	if (outputfile == ".") {
		outstrm = &cout;
	} else {
		fs.open(outputfile.c_str(), std::ios::binary | std::ios::in | std::ios::trunc);
		if (!fs) {
			cout << "Cannot open output file." << endl;
			return 2;
		}
		outstrm = &fs;
	}

	zidlgenerator* gen;
	if (method == "cheader")
		gen = zidlgenerator::create_cheadergen();
	else if (method == "cdef")
		gen = zidlgenerator::create_cdefgen();
	else if (method == "xml")
		gen = zidlgenerator::create_xmlgen(); 
	else if (method == "dotnet")
		gen = zidlgenerator::create_dotnetgen(); 
	else if (method == "py")
		gen = zidlgenerator::create_pygen(); 
	else if (method == "lua")
		gen = zidlgenerator::create_luagen(); 
	else {
		cout << "Invalid method, expected cheader, cdef, xml, dotnet, py." << endl;
		return 3;
	}

	gen->generate(*outstrm, prog);

	if (outputfile != ".") fs.close();

	delete gen;
	return 0;
}
