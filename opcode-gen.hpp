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
		std::vector<u16> opcodes{};
		for (auto& meta : meta_opcodes)
		{
			//	std::cout << "Created opcode " << meta << '\n';

			if (std::find(meta.begin(), meta.end(), '<') != meta.end())
			{
				// TODO parse labels
				u16 op = 0x0;
				opcodes.push_back(op);
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
