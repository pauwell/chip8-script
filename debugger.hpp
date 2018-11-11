/*
*	MIT License
*
*	Copyright(c) 2018 Paul Bernitz
*
*	Permission is hereby granted, free of charge, to any person obtaining a copy
*	of this software and associated documentation files(the "Software"), to deal
*	in the Software without restriction, including without limitation the rights
*	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*	copies of the Software, and to permit persons to whom the Software is
*	furnished to do so, subject to the following conditions :
*
*	The above copyright notice and this permission notice shall be included in all
*	copies or substantial portions of the Software.
*
*	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
*	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*	SOFTWARE.
*/

#pragma once

#include <iostream>
#include <cstdlib>
#include <ctime>  
#include <array>
#include <fstream>
#include <iomanip>
#include <vector>

#include "types.hpp"

#define MEMORY_SIZE 0x1000
#define V_REGS_TOTAL 0x10
#define STACK_SIZE 0x10
#define KEYPAD_SIZE 0x10
#define DISPLAY_W 0x40
#define DISPLAY_H 0x20
#define FONTSET_SIZE 0x50
#define PROGRAM_START 0x200
#define PIXEL_SIZE 0xC

namespace c8s
{
	const u8 FONTSET[FONTSET_SIZE] =
	{
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	class Chip8Debugger
	{
		u8  m_memory[MEMORY_SIZE];
		u8  m_v[V_REGS_TOTAL];
		u16 m_i;
		u8  m_delayTimer;
		u8  m_soundTimer;
		u16 m_pc;
		u8  m_sp;
		u16 m_stack[STACK_SIZE];
		u8  m_keypad[KEYPAD_SIZE];
		u8  m_display[DISPLAY_W * DISPLAY_H];
		unsigned m_debug_counter;

		std::array<bool, (DISPLAY_W * DISPLAY_H)> m_pixels;

	public:
		Chip8Debugger()
		{
			initialize();
		}

		void initialize()
		{
			// Seed for random.
			srand((unsigned)time(NULL));

			// Reset registers.
			m_pc = 0;
			m_sp = 0;
			m_i = 0;
			m_delayTimer = 0;
			m_soundTimer = 0;
			for (unsigned j = 0; j < MEMORY_SIZE; ++j) m_memory[j] = 0;
			for (unsigned j = 0; j < V_REGS_TOTAL; ++j) m_v[j] = 0;
			for (unsigned j = 0; j < STACK_SIZE; ++j) m_stack[j] = 0;
			for (unsigned j = 0; j < KEYPAD_SIZE; ++j) m_keypad[j] = 0;
			for (unsigned j = 0; j < (DISPLAY_W * DISPLAY_H); ++j) m_display[j] = 0;

			m_debug_counter = 0;

			// Set program-counter to the start of most Chip-8 programs (0x200). 
			m_pc = PROGRAM_START;

			// Load fontset into memory (0x0 - 0x50).
			for (unsigned j = 0; j < FONTSET_SIZE; ++j) m_memory[j] = FONTSET[j];

		}

		bool loadRom(const std::string fileName)
		{
			initialize();
			std::ifstream fileIn(fileName, std::ifstream::binary);

			if (!fileIn.is_open())
				return false;

			if (fileIn)
			{
				// Get length of file.
				fileIn.seekg(0, fileIn.end);
				int length = (int)fileIn.tellg();
				fileIn.seekg(0, fileIn.beg);

				// Allocate memory.
				char* buffer = new char[length];

				// Read data as a block.
				fileIn.read(buffer, length);
				fileIn.close();

				// Copy buffer to chip8-memory.
				for (int i = 0; i < length; ++i)
					m_memory[i + PROGRAM_START] = buffer[i];

				delete[] buffer;
			}
			return true;
		}

		bool runCycle()
		{
			updateTimers();
			return executeInstruction();
		}

	private:
		void clearScreen()
		{
			for (unsigned j = 0; j < (DISPLAY_W * DISPLAY_H); ++j) m_display[j] = 0;
			for (unsigned i = 0; i < m_pixels.size(); ++i)
			{
				m_pixels[i] = false;
			}
		}
		void updateTimers()
		{
			m_delayTimer -= (m_delayTimer > 0 ? 1 : 0);
			m_soundTimer -= (m_soundTimer > 0 ? 1 : 0);
		}
		bool executeInstruction()
		{
			u16 instruction = m_memory[m_pc] << 8 | m_memory[m_pc + 1];	// Read instruction.

			u16 nnn = instruction & 0xFFF;		// nnn or addr - A 12-bit value, the lowest 12 bits of the instruction		----XXXXXXXXXXXX
			u8 n = instruction & 0xF;			// n or nibble - A 4-bit value, the lowest 4 bits of the instruction		------------XXXX
			u8 x = (instruction >> 8) & 0xF;	// x - A 4-bit value, the lower 4 bits of the high byte of the instruction	----XXXX--------
			u8 y = (instruction >> 4) & 0xF;	// y - A 4-bit value, the upper 4 bits of the low byte of the instruction	--------XXXX----
			u8 kk = instruction & 0xFF;			// kk or byte - An 8-bit value, the lowest 8 bits of the instruction		--------XXXXXXXX

			if (instruction == 0x0)
			{
				std::cout << "<EOP> Press `return` to quit debugging.." << std::endl;
				std::cin.get();
				return false;
			}

			u16 op = instruction;
			std::ostringstream behavior_oss;

			switch (instruction & 0xF000)
			{
			case 0x0000:
				switch (instruction & 0xF)
				{
				case 0x0: // Clear the screen.
					clearScreen();
					m_pc += 2;
					behavior_oss << std::hex << op << " - 00E0 - Clear the screen";
					break;
				case 0xE: // Return from subroutine.
					--m_sp;
					m_pc = m_stack[m_sp];
					m_pc += 2;
					behavior_oss << std::hex << op << " - 00EE - Return from a subroutine";
					break;
				default:
					std::cout << "Unknown instruction: " << instruction << std::endl;
				}
				break;
			case 0x1000: // Jump to location nnn.
				m_pc = nnn;
				behavior_oss << std::hex << op << " - 1NNN - Jump to location " << nnn;
				break;
			case 0x2000: // Call subroutine at nnn.
				m_stack[m_sp] = m_pc;
				++m_sp;
				m_pc = nnn;
				behavior_oss << std::hex << op << " - 2NNN - Call subroutine at " << nnn;
				break;
			case 0x3000: // Skip next instruction if Vx = kk.
				m_pc += (m_v[x] == kk ? 4 : 2);
				behavior_oss << std::hex << op << " - 3XNN - Skip next instruction if V[" << std::hex << (int)x << "] = " << std::hex << (int)kk;
				break;
			case 0x4000: // Skip next instruction if Vx != kk.
				m_pc += (m_v[x] != kk ? 4 : 2);
				behavior_oss << std::hex << op << " - 4XNN - Skip next instruction if V[" << std::hex << (int)x << "] != " << std::hex << (int)kk;
				break;
			case 0x5000: // Skip next instruction if Vx = Vy.
				m_pc += (m_v[x] == m_v[y] ? 4 : 2);
				behavior_oss << std::hex << op << " - 5XY0 - Skip next instruction if V[" << std::hex << (int)x << "] = V[" << std::hex << (int)y << "]";
				break;
			case 0x6000: // Set Vx = kk.
				m_v[x] = kk;
				m_pc += 2;
				behavior_oss << std::hex << op << " - 6XNN - Set V[" << std::hex << (int)x << "] = " << std::hex << (int)kk;
				break;
			case 0x7000: // Set Vx = Vx + kk.
				m_v[x] += kk;
				m_pc += 2;
				behavior_oss << std::hex << op << " - 7XNN - Set Vx = V[" << std::hex << (int)x << "] += " << std::hex << (int)kk;
				break;
			case 0x8000:
				switch (instruction & 0xF)
				{
				case 0x0: // Set Vx = Vy.
					m_v[x] = m_v[y];
					m_pc += 2;
					behavior_oss << std::hex << op << " - 8XY0 - Set V[" << std::hex << (int)x << "] = V[" << std::hex << (int)y << "]";
					break;
				case 0x1: // Set Vx = Vx OR Vy.
					m_v[x] |= m_v[y];
					m_pc += 2;
					behavior_oss << std::hex << op << " - 8XY1 - Set V[" << std::hex << (int)x << "] |= V[" << std::hex << (int)y << "]";
					break;
				case 0x2: // Set Vx = Vx AND Vy.
					m_v[x] &= m_v[y];
					m_pc += 2;
					behavior_oss << std::hex << op << " - 8XY2 - Set V[" << std::hex << (int)x << "] &= V[" << std::hex << (int)y << "]";
					break;
				case 0x3: // Set Vx = Vx XOR Vy.
					m_v[x] ^= m_v[y];
					m_pc += 2;
					behavior_oss << std::hex << op << " - 8XY3 - Set V[" << std::hex << (int)x << "] ^= V[" << std::hex << (int)y << "]";
					break;
				case 0x4: // Set Vx = Vx + Vy, set VF = carry.
					m_v[0xF] = (m_v[x] + m_v[y] > 0xFF) ? 1 : 0;
					m_v[x] = (m_v[x] + m_v[y]) & 0xFF;
					m_pc += 2;
					behavior_oss << std::hex << op << " - 8XY4 - Set V[" << std::hex << (int)x << "] += V[" << std::hex << (int)y << "]";
					break;
				case 0x5: // Set Vx = Vx - Vy, set VF = NOT borrow.
					m_v[0xF] = (m_v[x] > m_v[y]) ? 1 : 0;
					m_v[x] -= m_v[y];
					m_pc += 2;
					behavior_oss << std::hex << op << " - 8XY5 - Set V[" << std::hex << (int)x << "] -= V[" << std::hex << (int)y << "]";
					break;
				case 0x6: // Set Vx = Vx SHR 1.
					m_v[0xF] = (m_v[x] & 0x1) != 0 ? 1 : 0;
					m_v[x] /= 2;
					m_pc += 2;
					behavior_oss << std::hex << op << " - 8XY6 - Set V[" << std::hex << (int)x << "] >>= 1";
					break;
				case 0x7: // Set Vx = Vy - Vx, set VF = NOT borrow.
					m_v[0xF] = m_v[y] > m_v[x] ? 1 : 0;
					m_v[x] = m_v[y] - m_v[x];
					m_pc += 2;
					behavior_oss << std::hex << op << " - 8XY7 - Set V[" << std::hex << (int)x << "] |= V[" << std::hex << (int)y << "]";
					break;
				case 0xE: // Set Vx = Vx SHL 1.
					m_v[0xF] = (m_v[x] & 0x80) != 0 ? 1 : 0;
					m_v[x] *= 2;
					m_pc += 2;
					behavior_oss << std::hex << op << " - 8XYE - Set V[" << std::hex << (int)x << "] <<= 1";
					break;
				default:
					std::cout << "Unknown instruction: " << instruction << std::endl;
				}
				break;
			case 0x9000: // Skip next instruction if Vx != Vy.
				m_pc += m_v[x] != m_v[y] ? 4 : 2;
				behavior_oss << std::hex << op << " - 9XY0 - Skip next instruction if V[" << std::hex << (int)x << "] != V[" << std::hex << (int)y << "]";
				break;
			case 0xA000: // Set I = nnn.
				m_i = nnn;
				m_pc += 2;
				behavior_oss << std::hex << op << " - ANNN - Set I = " << nnn;
				break;
			case 0xB000: // Jump to location nnn + V0.
				m_pc = nnn + m_v[0];
				behavior_oss << std::hex << op << " - BNNN - Jump to location " << nnn << " + V[0]";
				break;
			case 0xC000: // Set Vx = random byte AND kk.
				m_v[x] = kk & u8(rand() % 255);
				m_pc += 2;
				behavior_oss << std::hex << op << " - CXNN - Set V[" << std::hex << (int)x << "] = rand() & " << std::hex << (int)kk;
				break;
			case 0xD000: //  Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
				u16 pixel;
				m_v[0xF] = 0x0;

				for (unsigned line = 0x0; line < n; line++)
				{
					pixel = m_memory[m_i + line];
					for (unsigned bitCount = 0x0; bitCount < 0x8; ++bitCount)
					{
						if ((pixel & (0x80 >> bitCount)) != 0x0)
						{
							if (m_display[(m_v[x] + bitCount + ((m_v[y] + line) * DISPLAY_W))] == 0x1)
								m_v[0xF] = 0x1;
							m_display[m_v[x] + bitCount + ((m_v[y] + line) * DISPLAY_W)] ^= 0x1;
						}
					}
				}

				for (unsigned j = 0; j < (DISPLAY_W * DISPLAY_H); ++j)
				{
					if (m_display[j] != 0)
						m_pixels[j] = true;
					else
						m_pixels[j] = false;
				}
				m_pc += 2;
				behavior_oss << std::hex << op << " - DXYN - Display " << n << "-byte from `I` at (V[" << std::hex << (int)x << "], V[" << std::hex << (int)y << "]), set V[F] = collision";
				break;
			case 0xE000:
				switch (instruction & 0xFF)
				{
				case 0x9E: // Skip next instruction if key with the value of Vx is pressed. 
					m_pc += 2;
					behavior_oss << std::hex << op << " - EX9E - Skip next instruction if key with value of V[" << std::hex << (int)x << "] is pressed";
					break;
				case 0xA1: // Skip next instruction if key with the value of Vx is not pressed.
					m_pc += 2;
					behavior_oss << std::hex << op << " - EXA1 - Skip next instruction if key with value of V[" << std::hex << (int)x << "] is not pressed";
					break;
				default:
					std::cout << "Unknown instruction: " << instruction << std::endl;
				}
				break;
			case 0xF000:
				switch (instruction & 0xFF)
				{
				case 0x07: // Set Vx = delay timer value.
					m_v[x] = m_delayTimer;
					m_pc += 2;
					behavior_oss << std::hex << op << " - FX07 - Set V[" << std::hex << (int)x << "] = delay timer value";
					break;
				case 0x0A: // Wait for a key press, store the value of the key in Vx.
					for (unsigned i = 0; i < 0x10; ++i)
					{
						if (m_keypad[i] != 0)
						{
							m_v[x] = i;
							m_pc += 2;
						}
					}
					behavior_oss << std::hex << op << " - FX0A - Wait for key press, store key-value in V[" << std::hex << (int)x << "]";
					break;
				case 0x15: // Set delay timer = Vx.
					m_delayTimer = m_v[x];
					m_pc += 2;
					behavior_oss << std::hex << op << " - FX15 - Set delay timer = V[" << std::hex << (int)x << "]";
					break;
				case 0x18: // Set sound timer = Vx.
					m_soundTimer = m_v[x];
					m_pc += 2;
					behavior_oss << std::hex << op << " - FX18 - Set sound timer = V[" << std::hex << (int)x << "]";
					break;
				case 0x1E: // Set I = I + Vx.
					m_i += m_v[x];
					m_pc += 2;
					behavior_oss << std::hex << op << " - FX1E - Set I += V[" << std::hex << (int)x << "]";
					break;
				case 0x29: // Set I = location of sprite for digit Vx.
					m_i = m_v[x] * 5; // Multiplied by sprite-size (5). 
					m_pc += 2;
					behavior_oss << std::hex << op << " - FX29 - Set I = Location of sprite for digit V[" << std::hex << (int)x << "]";
					break;
				case 0x33: // Store BCD representation of Vx in memory locations I, I+1, and I+2.
						   // Ref: http://www.multigesture.net/wp-content/uploads/mirror/goldroad/chip8.shtml
					m_memory[m_i] = m_v[x] / 100;
					m_memory[m_i + 1] = (m_v[x] / 10) % 10;
					m_memory[m_i + 2] = (m_v[x] % 100) % 10;
					m_pc += 2;
					behavior_oss << std::hex << op << " - FX33 - Store BCD representation of V[" << std::hex << (int)x << "] in memory locations I, I+1, I+2";
					break;
				case 0x55: // Store registers V0 through Vx in memory starting at location I.
					for (unsigned j = 0; j < x; ++j) m_memory[m_i + j] = m_v[j];
					m_pc += 2;
					behavior_oss << std::hex << op << " - FX55 - Store registers V[0] -> V[" << std::hex << (int)x << "] in memory starting at location `I`";
					break;
				case 0x65: // Read registers V0 through Vx from memory starting at location I.
					for (unsigned j = 0; j < x; ++j) m_v[j] = m_memory[m_i + j];
					m_pc += 2;
					behavior_oss << std::hex << op << " - FX65 - Read registers V[0] -> V[" << std::hex << (int)x << "] from memory starting at location `I`";
					break;
				default:
					std::cout << "Unknown instruction: " << instruction << std::endl;
				}
				break;
			default:
				std::cout << "Unknown instruction: " << instruction << std::endl;
			}

			// Output.
			std::cout << "\n--[ step #" << ++m_debug_counter << " ]\n";

			std::cout << "\n+-[ instruction ]-+-[ behavior ]------------------------------------------------+\n";
			std::cout << "| 0x" << std::left << std::setw(14) << std::hex << instruction;
			std::cout << "| " << std::left << std::setw(60) << behavior_oss.str() << "|\n";
			std::cout << "+-------------------------------------------------------------------------------+\n";

			std::cout << "+[program counter]+[stack pointer]+[I register]+\n";
			std::cout << "| 0x" << std::left << std::setw(14) << std::hex << m_pc;
			std::cout << "| 0x" << std::left << std::setw(12) << std::hex << (int)m_sp;
			std::cout << "| 0x" << std::left << std::setw(9) << std::hex << m_i << "|\n";
			std::cout << "+-----------------+---------------+------------+\n";

			std::cout << "+-[ V registers ]---+----+----+----+----+----+----+----+----+----+----+----+----+\n";
			std::cout << "| 0  | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8  | 9  | a  | b  | c  | d  | e  | f  |\n";
			for (unsigned j = 0; j < 0x10; ++j) std::cout << "|0x" << std::left << std::setw(2) << (unsigned)m_v[j];
			std::cout << "|\n+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+\n";
			std::cin.get();
			return true;
		}
	};
}
