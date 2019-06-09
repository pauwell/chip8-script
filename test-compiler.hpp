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

#pragma once

#include "debug-output.hpp" 
#include "compiler.hpp"

namespace c8s
{
	// Run all tests.
	bool run_tests()
	{
		if (
			build_opcode("8XY3", 0, 0, 0x1, 0x2) != "8123" ||
			build_opcode("1NNN", 0xF4) != "10f4" ||
			build_opcode("3XNN", 0, 0, 0xA, 0, 0, 0xBB) != "3abb",
			build_opcode("DXYN", 0, 0x3, 0x1, 0x2) != "d123"
		) {
			std::cout << "Failed building opcodes via `build_opcode(mask..)`\n";
			return false;
		}

		auto raw_test_output = compile(
			"VAR a = 10\n"\
			"VAR b = 10\n"\
			"RAW 6001\n",
			true, true
		);

		if (compiler_log::read_errors().size() != 0
			|| raw_test_output.size() != 3
			|| raw_test_output[0] != 0x600A
			|| raw_test_output[1] != 0x610A
			|| raw_test_output[2] != 0x6001
			) {
			std::cout << "\n\raw_test_output failed!\n";
			return false;
		}

		auto for_test_output = compile(
			"VAR a = 1\n"\
			"FOR i=4 TO 10 STEP 2:\n"\
			"	IF a==1:\n"\
			"		a+=2\n"\
			"	ENDIF\n"\
			"	a += 1\n"\
			"ENDFOR\n"\
			"VAR z=10;",
			true, true
		);

		if ( compiler_log::read_errors().size() != 0
			|| for_test_output.size() != 12
			|| for_test_output[0] != 0x6001
			|| for_test_output[1] != 0x6104
			|| for_test_output[2] != 0x620A
			|| for_test_output[3] != 0x6302
			|| for_test_output[4] != 0x3001
			|| for_test_output[5] != 0x120E
			|| for_test_output[6] != 0x7002
			|| for_test_output[7] != 0x7001
			|| for_test_output[8] != 0x8134
			|| for_test_output[9] != 0x5120
			|| for_test_output[10]!= 0x1208
			|| for_test_output[11]!= 0x640A
			) {
			std::cout << "\n\nfor_test_output failed!\n";
			return false;
		}

		auto test_output = compile(
			"VAR a = 4\n"\
			"VAR b = 2\n"\
			"IF a == 4:\n"\
			"	IF a != 4:\n"\
			"		a = 8\n"\
			"	ENDIF\n"\
			"ENDIF\n"\
			"IF a == 3:\n"\
			"	IF a != b:\n"\
			"		a = b\n"\
			"	ENDIF\n"\
			"ENDIF\n"\
			"a = 1\n"\
			"b = a\n"\
			"a |= b\n"\
			"a &= b\n"\
			"a ^= b\n"\
			"a -= b\n"\
			"a <<=2\n"\
			"a >>=1\n",
			true, true
		);

		if (compiler_log::read_errors().size() != 0
			|| test_output.size() != 21
			|| test_output[0] != 0x6004
			|| test_output[1] != 0x6102
			|| test_output[2] != 0x3004
			|| test_output[3] != 0x120E
			|| test_output[4] != 0x4004
			|| test_output[5] != 0x120E
			|| test_output[6] != 0x6008
			|| test_output[7] != 0x3003
			|| test_output[8] != 0x1218
			|| test_output[9] != 0x9010
			|| test_output[10]!= 0x1218
			|| test_output[11]!= 0x8010
			|| test_output[12]!= 0x6001
			|| test_output[13]!= 0x8100
			|| test_output[14]!= 0x8011
			|| test_output[15]!= 0x8012
			|| test_output[16]!= 0x8013
			|| test_output[17]!= 0x8015
			|| test_output[18]!= 0x800E
			|| test_output[19]!= 0x800E
			|| test_output[20]!= 0x8006
		){
			std::cout << "\n\ntest_output failed!\n";
			return false;
		}
			
		std::cout << "\n\nAll tests passed!\n";
		return true;
	}
}
