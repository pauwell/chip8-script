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

#include <sstream>

#include "ast-parser.hpp"
#include "types.hpp"

namespace c8s
{
	std::string hex_to_string(u16 hex_input)
	{
		std::stringstream ss;
		ss << std::hex << hex_input;
		return ss.str();
	}

	unsigned find_var_index(std::string name, std::vector<std::string>& variables)
	{
		auto found_at = std::find(variables.begin(), variables.end(), name);
		return (found_at == variables.end()) ? u8(0) : std::distance(variables.begin(), found_at);
	};

	std::string var_decl_to_meta(const ASTNode& stmt_node, std::vector<std::string>& variables)
	{
		ASTNode source_node = stmt_node;
		ASTNode operator_node = source_node.params.front();
		ASTNode target_node = operator_node.params.front();

		if (operator_node.type != ASTNodeType::Operator && operator_node.value != "=")
			std::cerr << "Expected operator `=`!\n";

		const std::string source_name = source_node.value;
		const std::string target_value = target_node.value;
		//std::cout << "Declaring " << source_name << "=" << target_value << "\n";

		if (target_node.type == ASTNodeType::NumberLiteral)
		{
			u8 value_u8 = std::atoi(target_value.c_str());

			if (std::find(variables.begin(), variables.end(), source_name) == variables.end())
			{
				// 6XNN	Const	Vx = NN		Sets VX to NN.
				variables.push_back(source_name);
				u16 op = ((0x6 << 12) | ((u16)(variables.size() - 1) << 8) | (value_u8 & 0xFF));
				return hex_to_string(op);
			}
			else
				std::cerr << "Variable " << source_name << " already exists!\n";
		}
		else if (target_node.type == ASTNodeType::Identifier)
		{
			if (std::find(variables.begin(), variables.end(), source_name) == variables.end())
			{
				// 8XY0	Assign	Vx=Vy
				variables.push_back(source_name);
				u8 target_v_index = find_var_index(target_node.value, variables);
				u16 op = ((0x8 << 12) | ((u16)(variables.size() - 1) << 8) | (target_v_index << 4) | (0x0));
				return hex_to_string(op);
			}
		}
		else
			std::cerr << "Expected number literal!\n";

		return 0x0;
	}

	std::string var_expr_to_meta(const ASTNode& stmt_node, std::vector<std::string>& variables)
	{
		ASTNode source_node = stmt_node;
		ASTNode operator_node = source_node.params.front();
		ASTNode target_node = operator_node.params.front();

		if (operator_node.type != ASTNodeType::Operator)
			std::cerr << "Expected operator!\n";

		if (target_node.type == ASTNodeType::NumberLiteral)
		{
			u8 value_u8 = std::atoi(target_node.value.c_str());
			u8 v_index = find_var_index(source_node.value, variables);
			//std::cout << "Expr: " << source_node.value << operator_node.value << (int)value_u8 << '\n';

			if (operator_node.value == "=")
			{
				// 6XNN	Const	Vx = NN
				return hex_to_string((0x6 << 12) | (v_index << 8) | (value_u8 & 0xFF));
			}
			else if (operator_node.value == "+=")
			{
				// 7XNN	Const	Vx += NN
				return hex_to_string((0x7 << 12) | (v_index << 8) | (value_u8 & 0xFF));
			}
			else
				std::cerr << "Unknown operator: " << operator_node.value << '\n';
		}
		else if (target_node.type == ASTNodeType::Identifier)
		{
			u8 source_v_index = find_var_index(source_node.value, variables);
			u8 target_v_index = find_var_index(target_node.value, variables);
			//std::cout << "Expr: " << source_node.value << operator_node.value << target_node.value << '\n';

			if (operator_node.value == "=")
			{
				// 8XY0	Assign	Vx=Vy
				return hex_to_string((0x8 << 12) | (source_v_index << 8) | (target_v_index << 4) | (0x0));
			}
			else if (operator_node.value == "+=")
			{
				// 8XY4	Math	Vx += Vy
				return hex_to_string((0x8 << 12) | (source_v_index << 8) | (target_v_index << 4) | (0x4));
			}
			else if (operator_node.value == "-=")
			{
				// 8XY5	Math	Vx -= Vy
				return hex_to_string((0x8 << 12) | (source_v_index << 8) | (target_v_index << 4) | (0x5));
			}
			else if (operator_node.value == "&=")
			{
				// 8XY2	BitOp	Vx=Vx&Vy
				return hex_to_string((0x8 << 12) | (source_v_index << 8) | (target_v_index << 4) | (0x2));
			}
			else if (operator_node.value == "|=")
			{
				// 8XY1	BitOp	Vx=Vx|Vy
				return hex_to_string((0x8 << 12) | (source_v_index << 8) | (target_v_index << 4) | (0x1));
			}
			else if (operator_node.value == "^=")
			{
				// 8XY3	BitOp	Vx=Vx^Vy
				return hex_to_string((0x8 << 12) | (source_v_index << 8) | (target_v_index << 4) | (0x3));
			}
			else
				std::cerr << "Unknown operator: " << operator_node.value << '\n';
		}

		return "0x0";
	}

	std::vector<std::string> open_if_statement_to_meta(const ASTNode& stmt_node, std::vector<std::string>& variables, unsigned& label_counter)
	{
		ASTNode source_node = stmt_node.params.front();
		ASTNode operator_node = source_node.params.front();
		ASTNode target_node = operator_node.params.front();

		if (operator_node.type != ASTNodeType::Operator)
			std::cerr << "Expected operator!\n";

		//std::cout << "if " << source_node.value << operator_node.value << target_node.value << ":\n";

		if (target_node.type == ASTNodeType::NumberLiteral)
		{
			u8 value_u8 = std::atoi(target_node.value.c_str());
			u8 v_index = find_var_index(source_node.value, variables);

			if (operator_node.value == "==")
			{
				// 3XNN	Cond	if(Vx==NN)
				// 1NNN	Flow	goto NNN;
				return { 
					hex_to_string((u16)((0x3 << 12) | (v_index << 8) | (value_u8 & 0xFF))),
					"1<" + std::to_string(label_counter++) + ">"
				};
			}
			if (operator_node.value == "!=")
			{
				// 4XNN	Cond	if(Vx!=NN)
				// 1NNN	Flow	goto NNN;
				return { 
					hex_to_string((u16)((0x4 << 12) | (v_index << 8) | (value_u8 & 0xFF))),
					"1<" + std::to_string(label_counter++) + ">"
				};
			}
		}

		if (target_node.type == ASTNodeType::Identifier)
		{
			u8 source_v_index = find_var_index(source_node.value, variables);
			u8 target_v_index = find_var_index(target_node.value, variables);

			if (operator_node.value == "==")
			{
				// 5XY0	Cond	if(Vx==Vy)
				// 1NNN	Flow	goto NNN;
				return {
					hex_to_string((u16)((0x5 << 12) | (source_v_index << 8) | (target_v_index << 4) | (0x0))),
					"1<" + std::to_string(label_counter++) + ">"
				};
			}
			if (operator_node.value == "!=")
			{
				// 9XY0	Cond	if(Vx!=Vy)
				// 1NNN	Flow	goto NNN;
				return {
					hex_to_string((u16)((0x9 << 12) | (source_v_index << 8) | (target_v_index << 4) | (0x0))),
					"1<" + std::to_string(label_counter++) + ">"
				};
			}
		}

		return { 0x0 };
	}

	std::vector<std::string> close_if_statement_to_meta(unsigned& label_counter)
	{
		return { "<!" + std::to_string(--label_counter) + "!>" };
	}

	std::vector<std::string> ast_node_to_meta(const ASTNode& node, std::vector<std::string>& variables, unsigned& label_counter)
	{
		if (node.params.size() > 1)
			std::cerr << "Warning! Multiple statements in one.\n";
		const auto& stmt_node = node.params.front();

		if (node.type == ASTNodeType::IfStatement)
		{
			return open_if_statement_to_meta(node, variables, label_counter);
		}

		if (stmt_node.type == ASTNodeType::VarDeclaration)
		{
			return { var_decl_to_meta(stmt_node, variables) };
		}
		else if (stmt_node.type == ASTNodeType::VarExpression)
		{
			return { var_expr_to_meta(stmt_node, variables) };
		}
		else if (stmt_node.type == ASTNodeType::EndifStatement)
		{
			return { close_if_statement_to_meta(label_counter) };
		}
		else if (stmt_node.type == ASTNodeType::EndOfProgram)
		{
			return { "0x0" };
		}
		else
			std::cerr << "Invalid statement: " << stmt_node.value << '\n';

		return { "0x0" };
	}

	std::vector<std::string> walk_statements_and_convert_to_meta(const ASTNode& root_node, std::vector<std::string>& variables, unsigned& label_counter)
	{
		std::vector<std::string> meta_opcodes;

		for (const auto& node : root_node.params)
		{
			// Call this function again recursively, if there are nested statements.
			if (node.params.size() > 1)
			{
				std::vector<std::string> nested_opcodes = walk_statements_and_convert_to_meta(node, variables, label_counter);
				meta_opcodes.insert(meta_opcodes.end(), nested_opcodes.begin(), nested_opcodes.end());
			}
			else
			{
				std::vector<std::string> new_opcodes = ast_node_to_meta(node, variables, label_counter);
				meta_opcodes.insert(meta_opcodes.end(), new_opcodes.begin(), new_opcodes.end());
			}
		}

		return meta_opcodes;
	}

	// Generate `meta-code` from the AST.
	std::vector<std::string> generate_meta_opcodes(ASTNode program)
	{
		std::vector<std::string> meta_opcodes;
		std::vector<std::string> variables;
		unsigned label_counter = 1; // Is used for pulling unique numbers for jumping blocks.

		// Walk through all the statements and convert them to opcodes.
		meta_opcodes = walk_statements_and_convert_to_meta(program, variables, label_counter);

		return meta_opcodes;
	}
}
