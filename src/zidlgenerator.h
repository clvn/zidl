#pragma once

struct zidlgenerator {
	virtual ~zidlgenerator() { }
	virtual void generate(std::ostream& strm, zidlprogram& prog) = 0;

	static zidlgenerator* create_xmlgen();
	static zidlgenerator* create_cdefgen();
	static zidlgenerator* create_cheadergen();
	static zidlgenerator* create_dotnetgen();
	static zidlgenerator* create_pygen();
	static zidlgenerator* create_luagen();

};
