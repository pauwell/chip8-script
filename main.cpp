/*
* MIT License
*
* Copyright(c) 2018 Paul Bernitz
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files(the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions :
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

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
