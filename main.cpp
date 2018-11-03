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

	// FIX If-bodies cant be empty and using of undeclared variable produces no warning

	// BUG?? Loop exit-conditions are only tested for equality not greater or less then.
	//		 This can result in an infinite loop if e.g: FOR i=5 TO 10 STEP 2 (i: 5,7,9, 11 ! )

	// Running tests.
	if (!c8s::run_tests())
		std::cout << "\n\nTests have failed!\n";

	std::cin.get();
	return 0;
}
