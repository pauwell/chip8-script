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

#include <iostream>
#include <string>
#include <vector>

namespace c8s
{
	void print_intro()
	{
		std::cout << "Usage: c8s-compiler.exe [options] file\n";
		std::cout << "Compile the chip-8 script source $file into chip-8 machinecode.\n";
		
		std::cout << "\nOptions:\n";
		std::cout << "  -o, --output <file> output is saved in <file> instead of `out.c8s`\n";
		std::cout << "  -h, --help          display this help and exit\n";
		std::cout << "  -v, --version       print the version\n";
		std::cout << "  -d, --debug         attach debugger after compilation\n";
		std::cout << "  -t, --tests         run standard tests\n";
		std::cout << "  -s, --silent        do not produce any output\n";
		std::cout << "  -m, --steps         print intermediate steps (tokenization, AST creation etc.)\n";

		std::cout << "\nFor more information please visit:\n";
		std::cout << "<https://github.com/pauwell/chip8-script>";
	}

	struct Flag
	{
		char token;
		std::string param;
	};

	std::vector<Flag> parse_flags(int argc, char** const & argv)
	{
		if (argc <= 1)
			return {};

		std::vector<Flag> flags;

		for (int i = 0; i < argc; ++i)
		{
			std::string arg = argv[i];

			// -o file, --output file
			if (arg.find("-o") == 0 || arg.find("--output") == 0)
			{
				// Check if next arg is available and valid.
				if (i + 1 >= (argc - 1) || argv[i + 1][0] == '-') return {};

				// Get `file` param from next arg.
				flags.push_back(Flag{ 'o', argv[i + 1] });
				++i;
			}
			// -v, --version
			else if ((arg[0] == '-' && arg[1] != '-' && arg.find('v') != std::string::npos) || arg.find("--version") == 0)
			{
				return { Flag{ 'v', "" } };
			}
			// -d, --debug
			else if ((arg[0] == '-' && arg[1] != '-' && arg.find('d') != std::string::npos) || arg.find("--debug") == 0)
			{
				flags.push_back(Flag{ 'd', "" });
			}
			// -t, --tests
			else if ((arg[0] == '-' && arg[1] != '-' && arg.find('t') != std::string::npos) || arg.find("--tests") == 0)
			{
				return { Flag{ 't', "" } };
			}
			// -s, --silent
			else if ((arg[0] == '-' && arg[1] != '-' && arg.find('s') != std::string::npos) || arg.find("--silent") == 0)
			{
				flags.push_back(Flag{ 's', "" });
			}
			// -m, --print-steps
			else if ((arg[0] == '-' && arg[1] != '-' && arg.find('m') != std::string::npos) || arg.find("--steps") == 0)
			{
				flags.push_back(Flag{ 'm', "" });
			}
		}

		// The last arg must be the specified input file.
		if (argv[argc - 1][0] != '-')
		{
			flags.push_back(Flag{ 'i', argv[argc - 1] });
			return flags;
		}
		
		return {};
	}
}
