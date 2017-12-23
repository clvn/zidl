AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = zidl

zidl_SOURCES = main.cpp zidl.c zidl.h zidl_l.h zidlhelper.cpp zidlhelper.h zidlintermediate.cpp zidlintermediate.h zidlparser.cpp zidlparser.h zidlgenerator.h

zidl_SOURCES += cdefgen.cpp cheadergen.cpp dotnetgen.cpp luagen.cpp pygen.cpp xmlgen.cpp

EXTRA_DIST = zidl.l zidl.y zidl.vcxproj

zidl_l.h: zidl.l
	re2c -o zidl_l.h zidl.l

zidl.c: zidl.y
	lemon zidl.y

