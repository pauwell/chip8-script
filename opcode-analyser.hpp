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

#include "types.hpp"

namespace c8s
{
	// Prints opcodes in a readable format with additional information.
	void analyse_opcodes(std::vector<u16> opcodes)
	{
		for(auto& op : opcodes)
		{
			// Fetch variables.
			unsigned nnn = op & 0xFFF;    // ----XXXXXXXXXXXX
			unsigned n = op & 0xF;        // ------------XXXX
			unsigned x = (op >> 8) & 0xF; // ----XXXX--------
			unsigned y = (op >> 4) & 0xF; // --------XXXX----
			unsigned kk = op & 0xFF;      // --------XXXXXXXX
			
			switch(op & 0xF000)
			{
			case 0x0000:
				switch(op & 0xFF)
				{
				case 0xE0: // 00E0 - Clear the screen.
					std::cout << "00E0 - Clear the screen\n";
					break;
				case 0xEE: // 00EE - Return from a subroutine.
					std::cout << "00EE - Return from a subroutine\n";
					break;
				default:
					std::cout << "Unknown instruction: " << op << std::endl;
				}
				break;
			case 0x1000: // 1NNN - Jump to location nnn.
				std::cout << "1NNN - Jump to location " << nnn << '\n';
				break;
			case 0x2000: // 2NNN - Call subroutine at nnn.
				std::cout << "2NNN - Call subroutine at " << nnn << '\n';
				break;
			case 0x3000: // 3XNN - Skip next instruction if Vx = kk.
				std::cout << "3XNN - Skip next instruction if V[" << x << "] = " << kk << '\n';
				break;
			case 0x4000: // 4XNN - Skip next instruction if Vx != kk.
				std::cout << "4XNN - Skip next instruction if V[" << x << "] != " << kk << '\n';
				break;
			case 0x5000: // 5XY0 - Skip next instruction if Vx = Vy.
				std::cout << "5XY0 - Skip next instruction if V[" << x << "] = V[" << y << "]\n";
				break;
			case 0x6000: // 6XNN - Set Vx = kk.
				std::cout << "6XNN - Set V[" << x << "] = " << kk << '\n';
				break;
			case 0x7000: // 7XNN - Set Vx = Vx + kk.
				std::cout << "7XNN - Set Vx = V[" << x << "] += " << kk << '\n';
				break;
			case 0x8000: 
				switch(op & 0xF)
				{
				case 0x0: // 8XY0 - Set Vx = Vy.
					std::cout << "8XY0 - Set V[" << x << "] = V[" << y << "]\n";
					break;
				case 0x1: // 8XY1 - Set Vx = Vx OR Vy.
					std::cout << "8XY1 - Set V[" << x << "] |= V[" << y << "]\n";
					break;
				case 0x2: // 8XY2 - Set Vx = Vx AND Vy.
					std::cout << "8XY2 - Set V[" << x << "] &= V[" << y << "]\n";
					break;
				case 0x3: // 8XY3 - Set Vx = Vx XOR Vy.
					std::cout << "8XY3 - Set V[" << x << "] ^= V[" << y << "]\n";
					break;
				case 0x4: // 8XY4 - Set Vx = Vx + Vy, set VF = carry.
					std::cout << "8XY4 - Set V[" << x << "] += V[" << y << "]\n";
					break;
				case 0x5: // 8XY5 - Set Vx = Vx - Vy, set VF = NOT borrow.
					std::cout << "8XY5 - Set V[" << x << "] -= V[" << y << "]\n";
					break;
				case 0x6: // 8XY6 - Set Vx = Vx SHR 1.
					std::cout << "8XY6 - Set V[" << x << "] >>= 1\n";
					break;
				case 0x7: // 8XY7 - Set Vx = Vy - Vx, set VF = NOT borrow.
					std::cout << "8XY7 - Set V[" << x << "] |= V[" << y << "]\n";
					break;
				case 0xE: // 8XYE - Set Vx = Vx SHL 1.
				std::cout << "8XYE - Set V[" << x << "] <<= 1\n";
					break;
				default:
					std::cout << "Unknown instruction: " << op << std::endl;
				}
				break;
			case 0x9000: // 9XY0 - Skip next instruction if Vx != Vy.
				std::cout << "9XY0 - Skip next instruction if V[" << x << "] != V[" << y << "]\n";
				break;
			case 0xA000: // ANNN - Set I = nnn.
				std::cout << "ANN - Set I = " << nnn << '\n';
				break;
			case 0xB000: // BNNN - Jump to location nnn + V0.
				std::cout << "BNNN - Jump to location " << nnn << " + V[0]\n";
				break;
			case 0xC000: // CXNN -  Set Vx = random byte AND kk.
				std::cout << "CXNN - Set V[" << x << "] = rand() & " << kk; 
				break;
			case 0xD000: // DXYN - Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
				std::cout << "DXYN - Display " << n << "-byte sprite from location `I` at (V[" << x << "], V[" << y << "]), set V[F] = collision\n";
				break;
			case 0xE000:
				switch(op & 0xFF)
				{
				case 0x9E: // EX9E - Skip next instruction if key with the value of Vx is pressed. 
					std::cout << "EX9E - Skip next instruction if key with value of V[" << x << "] is pressed\n";
					break;
				case 0xA1: // EXA1 - Skip next instruction if key with the value of Vx is not pressed.
					std::cout << "EXA1 - Skip next instruction if key with value of V[" << x << "] is not pressed\n";
					break;
				default:
					std::cout << "Unknown instruction: " << op << std::endl;
				}
				break;
			case 0xF000:
				switch(op & 0xFF)
				{
				case 0x07: // FX07 - Set Vx = delay timer value.
					std::cout << "FX07 - Set V[" << x << "] = delay timer value\n";
					break;
				case 0x0A: // FX0A - Wait for a key press, store the value of the key in Vx.
					std::cout << "FX0A - Wait for a key press, store the value of the key in V[" << x << "]\n";
					break;
				case 0x15: // FX15 - Set delay timer = Vx.
					std::cout << "FX15 - Set delay timer = V[" << x << "]\n";
					break;
				case 0x18: // FX18 - Set sound timer = Vx.
					std::cout << "FX18 - Set sound timer = V[" << x << "]\n";
					break;
				case 0x1E: // FX1E - Set I = I + Vx.
					std::cout << "FX1E - Set I += V[" << x << "]\n";
					break;
				case 0x29: // FX29 - Set I = location of sprite for digit Vx.
					std::cout << "FX29 - Set I = Location of sprite for digit V[" << x << "]\n";
					break;
				case 0x33: // FX33 - Store BCD representation of Vx in memory locations I, I+1, and I+2.
					std::cout << "FX33 - Store BCD representation of V[" << x << "] in memory locations I, I+1, I+2\n";
					break;  
				case 0x55: // FX55 - Store registers V0 through Vx in memory starting at location I.
					std::cout << "FX55 - Store registers V[0] -> V[" << x << "] in memory starting at location `I`\n";
					break;
				case 0x65: // FX65 - Read registers V0 through Vx from memory starting at location I.
					std::cout << "FX65 - Read registers V[0] -> V[" << x << "] from memory starting at location `I`\n";
					break;
				default:
					std::cout << "Unknown instruction: " << op << std::endl;
				}
				break;
			default:
				std::cout << "Unknown instruction: " << op << std::endl;
			}

			
		}
	}
}
