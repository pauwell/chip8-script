#include <iostream>
#include <vector>
#include "test-compiler.hpp"

int main()
{
	system("mode 150");

	// Running tests.
	c8s::test_compiler(
		" {1} Nested if-statements",

		"var A = 6;;\n"\
		"if A == 1:\n"\
		"	A+=11\n"\
		"	if A==2:\n"\
		"		A+=21\n"\
		"		A+=22\n"\
		"		A+=23\n"\
		"	endif\n"
		"	A+=12\n"\
		"endif"
	);
	c8s::test_compiler(
		" {2} Nested for-loops and if-statements",
		
		"VAR x = 0\n"\
		"IF x == 0:\n"\
		"	FOR I = 0 TO 10:\n"\
		"		VAR a = 10\n"\
		"		VAR b = A;\n"\
		"		a += 4\n"\
		"	ENDFOR\n"\
		"ENDIF"
	);

	std::cin.get();
	return 0;
}
