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
#include <fstream>
#include <vector>

#include "test-compiler.hpp"
#include "interface.hpp"
#include "debugger.hpp"

int main(int argc, char** argv)
{
	c8s::run_tests();// XX

	// Parse arguments.
	auto flags = c8s::parse_flags(argc, argv);

	// Just print the introduction if no input is provided.
	if (flags.empty())
	{
		c8s::print_intro();
		return EXIT_SUCCESS;
	}

	// If version flag is set just print the version and exit.
	if (std::find_if(flags.begin(), flags.end(), [](c8s::Flag f) { return f.token == 'v'; }) != flags.end())
	{
		std::cout << "c8s-compiler v0.5\n";
		return EXIT_SUCCESS;
	}

	// If the tests flag is set, run the tests and exit.
	if (std::find_if(flags.begin(), flags.end(), [](c8s::Flag f) { return f.token == 't'; }) != flags.end())
	{
		return c8s::run_tests() ? EXIT_SUCCESS : EXIT_FAILURE;
	}

	// Read the input file.
	if (flags.back().token != 'i' || flags.back().param.empty())
	{
		std::cout << "No input specified!\n";
		return EXIT_FAILURE;
	}
	std::ifstream ifs{ flags.back().param };
	if (!ifs.is_open())
	{
		std::cout << "Could not open input-file!\n";
		return EXIT_FAILURE;
	}
	std::string line{};
	std::string code_input{};
	while (std::getline(ifs, line))
	{
		code_input += (line + '\n');
	}

	// Check which type of output should be produced.
	bool is_silent = std::find_if(flags.begin(), flags.end(), [](c8s::Flag f) { return f.token == 's'; }) != flags.end();
	bool is_print_steps = std::find_if(flags.begin(), flags.end(), [](c8s::Flag f) { return f.token == 'm'; }) != flags.end();

	// Compile.
	std::cout << "Starting to compile..\n";
	auto compiler_output = c8s::compile(code_input, !is_silent, !is_silent && is_print_steps);

	// Check for errors in compiler result.
	if (compiler_output.empty())
	{
		std::cout << "Failed...\n";
		return EXIT_FAILURE;
	}
	else std::cout << "Finished!\n";

	// Write result to output.
	auto out_flag = std::find_if(flags.begin(), flags.end(), [](c8s::Flag f) { return f.token == 'o'; });
	std::string out_file = (out_flag != flags.end() && !out_flag->param.empty()) ? out_flag->param : "out.c8s";
	c8s::write_opcodes_to_file(compiler_output, out_file);
	std::cout << "Output written to `" << out_file << "`\n";

	// Attach debugger to output file.
	bool is_debug = std::find_if(flags.begin(), flags.end(), [](c8s::Flag f) { return f.token == 'd'; }) != flags.end();
	if (is_debug)
	{
		// Load ROM into debugger.
		c8s::Chip8Debugger debugger;
		if (!debugger.loadRom(out_file))
		{
			std::cout << "Debugger unable to load the ROM\n";
			return EXIT_FAILURE;
		}

		// Run debug process.
		std::cout << "Start debugging..\n";
		std::cout << "Press <return> to step to the next instruction\n";
		while (debugger.runCycle());
	}
	
	std::cout << "Success!\n";
	std::cin.get();
	return EXIT_SUCCESS;
}
