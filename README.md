# zidl - zzub interface description language

zidl is a language and tool intended for C/C++ library authors to describe
public interfaces, callbacks, data structures and constants in order to 
automate generating headers, documentation, bindings to multiple programming
languages, and more.

zidl was originally conceived and implemented by Leonard Ritter, but has since
been re-implemented and extended with support for several new targets.

Windows

- Requires re2c and lemon. F.ex put re2c.exe, lemon.exe and lemon.c in the
	  project directory.
- Build zidl.sln with Visual Studio for Desktop 2012 (Express or newer)

Posix/Linux

- Install re2c and lemon from your package manager or source
- Run the following commands from the project directory

		autoreconf -vi
		./configure
		make
		sudo make install

The zidl language supports the following constructs:
- top level namespaces
- classes with static and non-static member functions
- classes with data members
- data types: int, uint, short, ushort, char, uchar, float, double, bool, 
  void, pvoid, string
- enums
- unions
- callbacks

The zidl tool supports the following output targets:
- C header
- Python bindings via ctypes
- Lua bindings
- XML
- Linker definition file for Windows DLLs (.DEF)
