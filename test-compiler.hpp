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
#include "opcode-gen.hpp"

namespace c8s
{
	std::vector<u16> test_compiler(std::string what, std::string code,
		bool is_print_meta = true, 
		bool is_print_ast = true, 
		bool is_print_tokens = true, 
		bool is_print_ops = true)
	{
		print_seperator(true);
		std::cout << "Test => " << what << '\n';
		print_seperator(true);

		// Tokenize.
		auto tokens = c8s::split_code_into_tokens(code);
		if (is_print_tokens) print_tokens(tokens);

		// Parse.
		auto ast = c8s::parse_tokens_to_ast(tokens);
		if (is_print_ast) print_ast(ast);

		// Generate meta.
		auto meta_ops = c8s::generate_meta_opcodes(ast);
		if (is_print_meta) print_meta(meta_ops);

		// Generate opcodes.
		auto ops = c8s::create_opcodes_from_meta(meta_ops);
		if (is_print_ops) print_opcodes(ops);

		return ops;
	}

	// Run all tests.
	bool run_tests()
	{
		auto for_test_output = test_compiler(
			"Test for-loop",
			"VAR a = 1\n"\
			"FOR i=5 TO 10 STEP 2:\n"\
			"	a += 1\n"\
			"ENDFOR\n"\
			"VAR a=10;"
		);

		auto test_output = test_compiler(
			"Full set of features",

			"VAR a = 4\n"\
			"VAR b = 2\n"\
			"IF a == 4:\n"\
			"IF a != 4:\n"\
			"	a = 8\n"\
			"ENDIF\n"\
			"ENDIF\n"\
			"IF a == b:\n"\
			"	IF a != b:\n"\
			"		a = b\n"\
			"	ENDIF\n"\
			"ENDIF\n"\
			"a = 1\n"\
			"b = a\n"\
			"a |= b\n"\
			"a &= b\n"\
			"a ^= b\n"\
			"a -= b"
		);

		if (   test_output[0] != 0x6004
			|| test_output[1] != 0x6102
			|| test_output[2] != 0x3004
			|| test_output[3] != 0x120E
			|| test_output[4] != 0x4004
			|| test_output[5] != 0x120E
			|| test_output[6] != 0x6008
			|| test_output[7] != 0x5010
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
		){
			std::cout << "\n\nTest failed!\n";
			return false;
		}
			
		std::cout << "\n\nTest passed!\n";

		// Output opcodes to ROM file.
		c8s::write_opcodes_to_file(test_output, "OUT_ROM");
		std::cout << "\n\nOpcodes written to `OUT_ROM`..\nDone!\n\n";

		return true;
	}
}
