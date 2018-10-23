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
#include <utility>
#include <algorithm>
#include <stdlib.h> // itoa

#include "conversion.hpp"

namespace c8s
{
	// Creating the finished opcodes using `meta opcodes` produced by the meta generator.
	std::vector<u16> create_opcodes_from_meta(std::vector<std::string> meta_opcodes)
	{
		// Replace labels by their calculated `real` memory-offset.	
		for (unsigned i = 0; i<meta_opcodes.size(); ++i)
		{
			// Find a target-label.
			if (meta_opcodes[i].find("<!") != std::string::npos)
			{
				// Calculate the `real` distance (skipping entries containing `<!`) from target to start.
				auto real_distance = std::count_if(meta_opcodes.begin(), meta_opcodes.begin() + i,
					[](const std::string& str_elem) { return str_elem.find("<!") == std::string::npos; });

				// Get the value of the label between `<!` and `!>`.
				std::string val = meta_opcodes[i].substr(meta_opcodes[i].find("<!") + 2, meta_opcodes[i].find("!>") - 2);

				// Now only target the labels before that and replace them.
				for (unsigned j = 0; j<i; ++j)
				{
					// Find a source-label that should be replaced.
					if (meta_opcodes[j].find("<" + val + ">") != std::string::npos)
					{
						// Calculate the `real` offset in memory by adding the starting address 0x200 
						// for chip-8 ROM's to the `real` distance times the size of each opcode (2 bytes).
						unsigned real_address_offset = (0x200 + (real_distance * 2));

						// Convert the offset from decimal to a hexadecimal string.
						std::string converted_hex_buffer = hex_to_string(real_address_offset);

						// Overwrite the current meta-opcode with the real opcode.
						meta_opcodes[j] = meta_opcodes[j].substr(0, meta_opcodes[j].find('<')) + converted_hex_buffer;
					}
				}
			}
		}

		// Remove `<!..!>`-blocks from `meta_opcodes`.
		meta_opcodes.erase(std::remove_if(
			meta_opcodes.begin(),
			meta_opcodes.end(),
			[](const std::string& str_elem) {
			return str_elem.find('!') != std::string::npos;
		}), meta_opcodes.end());


		// Convert the finished opcodes to u16.
		std::vector<u16> opcodes{};
		for (auto& meta : meta_opcodes)
		{
			if (std::find(meta.begin(), meta.end(), '<') != meta.end())
			{
				std::cerr << "Error! All labels should have been parsed by now\n";
				opcodes.push_back(0x0);
				continue;
			}

			try
			{
				u16 op = 0xFFFF & std::stoul(meta, nullptr, 16);
				opcodes.push_back(op);
			}
			catch (std::invalid_argument ex)
			{
				std::cerr << "Can not convert " << meta << " to hexadecimal in " << ex.what() << '\n';
			}
			catch (...)
			{
				std::cerr << "Unknown error occurred while trying to convert opcode " << meta << '\n';
			}
		}

		return opcodes;
	}
}
