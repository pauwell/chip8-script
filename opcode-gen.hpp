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
#include <algorithm>

#include "types.hpp"

namespace c8s
{
	// Creating the finished opcodes using `meta opcodes` produced by the meta generator.
	std::vector<u16> create_opcodes_from_meta(std::vector<std::string> meta_opcodes)
	{
		// 1. Parse all the labels.
		// 1.1 Search for an element in `meta_opcodes` that contain '<!'.
		// 1.1.1 If none was found. Continue with 2.
		// 1.2 Take their respective address (0x200 + (idx*2)?)
		// 1.3 Search for all elements in `meta_opcodes` that contain '<'.
		// 1.4 Replace their `<value>` with the address we got from before.
		// 1.5 Remove the element from the `meta_opcodes` vector.
		// 1.6 Start again by 1.1.

		// Okay by now all labels should be replaced by their respective 

		// 2. Convert the finished opcodes to u16.
		std::vector<u16> opcodes{};
		for (auto& meta : meta_opcodes)
		{
			//	std::cout << "Created opcode " << meta << '\n';

			if (std::find(meta.begin(), meta.end(), '<') != meta.end())
			{
				std::cerr << "Error! All labels should have been parsed by now\n";
				opcodes.push_back(0x0);
			}
			else
			{
				u16 op = std::stoul(meta, nullptr, 16);
				opcodes.push_back(op);
			}
		}

		return opcodes;
	}
}
