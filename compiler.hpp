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

#include "meta-gen.hpp"
#include "opcode-gen.hpp"
#include "compiler_log.hpp"

namespace c8s
{
	// Compiles chip-8 script into chip-8 machinecode.
	std::vector<u16> compile(std::string c8s_input_code, bool print_errors=false)
	{
		// Reset the log.
		compiler_log::reset_all();

		// Parse.
		auto tokens = c8s::split_code_into_tokens(c8s_input_code);
		auto ast = c8s::parse_tokens_to_ast(tokens);
		
		// Generate.
		auto meta = c8s::generate_meta_opcodes(ast);
		auto ops = c8s::create_opcodes_from_meta(meta);

		// Evaluate the log.
		if(print_errors && compiler_log::read_errors().size() != 0)
		{
			for (const auto& err_line : compiler_log::read_errors())
				std::cerr << err_line << '\n';
		}

		return ops;
	}
}