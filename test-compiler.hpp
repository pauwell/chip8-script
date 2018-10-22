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
	void test_compiler(std::string what, std::string code,
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

		std::cout << "\n\n";
	}
}
