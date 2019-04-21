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
#include "conversion.hpp"

namespace c8s
{
	unsigned find_var_index(std::string name, std::vector<std::string>& variables)
	{
		auto found_at = std::find(variables.begin(), variables.end(), name);
		if (found_at == variables.end())
			compiler_log::write_error("Usage of undeclared variable " + name);
		return (found_at == variables.end()) ? u8(0) : std::distance(variables.begin(), found_at);
	};

	std::string var_decl_to_meta(const ASTNode& stmt_node, std::vector<std::string>& variables)
	{
		ASTNode source_node = stmt_node;

		if (source_node.params.size() == 0 || source_node.params.front().params.size() == 0)
		{
			compiler_log::write_error("Error declaring variable on line " + std::to_string(stmt_node.line_number));
			return "";
		}

		ASTNode operator_node = source_node.params.front();
		ASTNode target_node = operator_node.params.front();

		if (operator_node.type != ASTNodeType::Operator && operator_node.value != "=")
		{
			compiler_log::write_error("Expected operator `=` on line " + std::to_string(stmt_node.line_number));
			return "";
		}
		const std::string source_name = source_node.value;
		const std::string target_value = target_node.value;

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
			{
				compiler_log::write_error("Declaring an already existing variable " + source_name + " on line " + std::to_string(stmt_node.line_number));
				return "";
			}
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
		{
			compiler_log::write_error("Expected number literal or identifier on line " + std::to_string(stmt_node.line_number));
			return "";
		}

		return "";
	}

	std::string var_expr_to_meta(const ASTNode& stmt_node, std::vector<std::string>& variables)
	{
		if (stmt_node.params.size() == 0 || stmt_node.params.front().params.size() == 0)
		{
			compiler_log::write_error("Error parsing expression on line " + std::to_string(stmt_node.line_number));
			return "";
		}

		ASTNode source_node = stmt_node;
		ASTNode operator_node = source_node.params.front();
		ASTNode target_node = operator_node.params.front();

		if (operator_node.type != ASTNodeType::Operator)
			compiler_log::write_error("Expected operator on line " + std::to_string(stmt_node.line_number));

		if (target_node.type == ASTNodeType::NumberLiteral)
		{
			u8 value_u8 = std::atoi(target_node.value.c_str());
			u8 v_index = find_var_index(source_node.value, variables);

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
			{
				compiler_log::write_error("Unknown operator: " + operator_node.value + " on line " + std::to_string(stmt_node.line_number));
				return "";
			}
		}
		else if (target_node.type == ASTNodeType::Identifier)
		{
			u8 source_v_index = find_var_index(source_node.value, variables);
			u8 target_v_index = find_var_index(target_node.value, variables);

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
			{
				compiler_log::write_error("Unknown operator " + operator_node.value + " in expression on line " + std::to_string(stmt_node.line_number));
				return "";
			}	
		}

		compiler_log::write_error("Syntax error in expression on line " + std::to_string(stmt_node.line_number));
		return "";
	}

	std::vector<std::string> open_if_statement_to_meta(const ASTNode& stmt_node, std::vector<std::string>& variables, unsigned& if_label_counter)
	{
		if (stmt_node.params.size() == 0 || stmt_node.params.front().params.size() == 0)
		{
			compiler_log::write_error("Error parsing if-statement on line " + std::to_string(stmt_node.line_number));
			return {};
		}

		ASTNode source_node = stmt_node.params.front();
		ASTNode operator_node = source_node.params.front();
		ASTNode target_node = operator_node.params.front();

		if (operator_node.type != ASTNodeType::Operator)
		{
			compiler_log::write_error("Expected operator in if-statement on line " + std::to_string(stmt_node.line_number));
			return {};
		}

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
					"1<" + std::to_string(if_label_counter++) + ">"
				};
			}
			if (operator_node.value == "!=")
			{
				// 4XNN	Cond	if(Vx!=NN)
				// 1NNN	Flow	goto NNN;
				return { 
					hex_to_string((u16)((0x4 << 12) | (v_index << 8) | (value_u8 & 0xFF))),
					"1<" + std::to_string(if_label_counter++) + ">"
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
					"1<" + std::to_string(if_label_counter++) + ">"
				};
			}
			if (operator_node.value == "!=")
			{
				// 9XY0	Cond	if(Vx!=Vy)
				// 1NNN	Flow	goto NNN;
				return {
					hex_to_string((u16)((0x9 << 12) | (source_v_index << 8) | (target_v_index << 4) | (0x0))),
					"1<" + std::to_string(if_label_counter++) + ">"
				};
			}
		}

		return { 0x0 };
	}

	std::vector<std::string> close_if_statement_to_meta(unsigned& if_label_counter)
	{
		return { "<!" + std::to_string(--if_label_counter) + "!>" };
	}

	std::vector<std::string> open_for_loop_to_meta(const ASTNode& stmt_node, std::vector<std::string>& variables, unsigned& for_label_counter)
	{
		// Fetch nodes.
		if (stmt_node.params.size() == 0)
		{
			compiler_log::write_error("Error creating index variable in for-loop on line " + std::to_string(stmt_node.line_number));
			return {};
		}
		ASTNode var_node = stmt_node.params.front();
		if (var_node.params.size() == 0 || var_node.params.front().params.size() == 0 || var_node.params.front().params.front().params.size() == 0)
		{
			compiler_log::write_error("Error creating range value in for-loop on line " + std::to_string(stmt_node.line_number));
			return {};
		}
		ASTNode to_node = var_node.params.front().params.front().params.front();
		if (to_node.params.size() == 0 || to_node.params.front().params.size() == 0)
		{
			compiler_log::write_error("Error creating step value in for-loop on line " + std::to_string(stmt_node.line_number));
			return {};
		}
		ASTNode step_node = to_node.params.front().params.front();

		// These are dummy ASTNodes to use the `var_decl_to_meta`-function to 
		// create the additional variables neccessary for the loop.
		ASTNode ito_dummy{ ASTNodeType::VarDeclaration, var_node.value + "to", var_node.line_number, {
			ASTNode{ ASTNodeType::Operator, "=", var_node.line_number, { to_node.params.front() } } } };
		ASTNode istep_dummy{ ASTNodeType::VarDeclaration, var_node.value + "step", var_node.line_number, {
			ASTNode{ ASTNodeType::Operator, "=", var_node.line_number, { step_node.params.front() } } } };

		// Declare the loop variables.
		std::string index_var = var_decl_to_meta(var_node, variables);
		std::string index_to_var = var_decl_to_meta(ito_dummy, variables);
		std::string index_istep_var = var_decl_to_meta(istep_dummy, variables);
		std::string loop_start_label = "<!" + std::to_string(for_label_counter++) + "!>";

		// Handle errors in the loop-variables declarations.
		if (index_var.size() == 0 || index_to_var.size() == 0 || index_istep_var.size() == 0 || compiler_log::read_errors().size() != 0)
		{
			compiler_log::write_error("Error creating variable " + var_node.value + " on line " + std::to_string(stmt_node.line_number));
			return {};
		}

		return { index_var, index_to_var, index_istep_var, loop_start_label };
	}

	std::vector<std::string> close_for_loop_to_meta(std::vector<std::string>& variables, unsigned& for_label_counter)
	{
		// The last `x, xto, xstep` triplet in the variables stack must be the corresponding one.
		unsigned var_idx = 0;
		for (unsigned i = variables.size() - 1; i > 1; --i)
			if (variables[i].find("step") != std::string::npos && variables[i - 1].find("to") != std::string::npos)
			{
				var_idx = i - 2;
				break;
			}
		
		std::vector<std::string> endfor_ops{
			hex_to_string((u16)((0x8 << 12) | (var_idx << 8) | ((var_idx + 2) << 4) | (0x4))), /* 8[i][istep]4 - Vx += Vy  */
			hex_to_string((u16)((0x5 << 12) | (var_idx << 8) | ((var_idx + 1) << 4) | (0x4))), /* 5[i][ito]0 - if(Vx==Vy) */
			"1<" + std::to_string(--for_label_counter) + ">"	/* Jmp to loop-start. */
		};

		return endfor_ops;
	}

	std::vector<std::string> func_call_to_meta(const ASTNode& stmt_node)
	{
		if (stmt_node.params.size() == 0 || stmt_node.params.front().params.size() == 0)
		{
			compiler_log::write_error("Error parsing function-call on line " + std::to_string(stmt_node.line_number));
			return {};
		}

		ASTNode source_node = stmt_node;
		ASTNode func_def_node = stmt_node.params.front();
		ASTNode opening_brace_node = func_def_node.params.front();
		ASTNode closing_brace_node = opening_brace_node.params.front();
		
		if(func_def_node.type != ASTNodeType::FunctionCall)
			compiler_log::write_error("Expected function-call on line " + std::to_string(stmt_node.line_number));

		if (opening_brace_node.type != ASTNodeType::OpenBrace)
			compiler_log::write_error("Expected open brace on line " + std::to_string(stmt_node.line_number));

		if (closing_brace_node.type != ASTNodeType::ClosingBrace)
		{
			// TODO Parse parameters and reparse closing brace node.
		}

		if (func_def_node.value == "cls")
		{
			return { "00E0" }; // 00E0	Display	cls()
		}

		compiler_log::write_error("Invalid function call on line " + std::to_string(stmt_node.line_number));
		return {};
	}

	std::vector<std::string> ast_node_to_meta(const ASTNode& node, std::vector<std::string>& variables, unsigned& if_label_counter, unsigned& for_label_counter)
	{
		if (node.params.size() > 1)
			compiler_log::write_warning("Multiple statements in one on line " + std::to_string(node.line_number));
		
		if (node.params.size() == 0)
		{
			compiler_log::write_error("Empty statement on line " + std::to_string(node.line_number)); 
			return {};
		}

		const auto& stmt_node = node.params.front();

		if (node.type == ASTNodeType::IfStatement)
		{
			return open_if_statement_to_meta(node, variables, if_label_counter);
		}
		if (stmt_node.type == ASTNodeType::EndifStatement)
		{
			return close_if_statement_to_meta(if_label_counter);
		}
		if (node.type == ASTNodeType::ForLoop)
		{
			return open_for_loop_to_meta(node, variables, for_label_counter);
		}
		if (stmt_node.type == ASTNodeType::EndforLoop)
		{
			return close_for_loop_to_meta(variables, for_label_counter);
		}
		if (stmt_node.type == ASTNodeType::FunctionCall)
		{
			return func_call_to_meta(node);
		}
		if (stmt_node.type == ASTNodeType::VarDeclaration)
		{
			auto var_decl = var_decl_to_meta(stmt_node, variables);
			if (var_decl.length() == 0) return {};
			return { var_decl };
		}
		if (stmt_node.type == ASTNodeType::VarExpression)
		{
			auto var_expr = var_expr_to_meta(stmt_node, variables);
			if (var_expr.length() == 0) return {};
			return { var_expr };
		}
		if (stmt_node.type == ASTNodeType::Raw)
		{
			if(stmt_node.params.size() != 0)
				return { stmt_node.params.front().value };
		}
		if (stmt_node.type == ASTNodeType::EndOfProgram)
		{
			return { "0" };
		}
		
		compiler_log::write_error("Invalid statement " + stmt_node.value + " in expression on line " + std::to_string(stmt_node.line_number));
		return { };
	}

	std::vector<std::string> walk_statements_and_convert_to_meta(
		const ASTNode& root_node,
		std::vector<std::string>& variables,
		unsigned& if_label_counter,
		unsigned& for_label_counter,
		unsigned line=1
	){
		if (root_node.params.size() == 0)
		{
			compiler_log::write_error("Empty program!");
			return {};
		}

		std::vector<std::string> meta_opcodes;

		for (const auto& node : root_node.params)
		{
			// Call this function again recursively, if there are nested statements.
			if (node.params.size() > 1)
			{
				std::vector<std::string> nested_opcodes = walk_statements_and_convert_to_meta(node, variables, if_label_counter, for_label_counter, line);
				meta_opcodes.insert(meta_opcodes.end(), nested_opcodes.begin(), nested_opcodes.end());
				
				// Add real distance to line counter. That means ignore meta-opcodes containing '<!'.
				line += std::count_if(nested_opcodes.begin(), nested_opcodes.end(), [](std::string s) {
					return s.find("<!") == std::string::npos;
				});
			}
			else
			{
				std::cout << "src [" << node.line_number << "] dest[" << line << "]\n";

				std::vector<std::string> new_opcodes = ast_node_to_meta(node, variables, if_label_counter, for_label_counter);
				if (new_opcodes.size() == 0 && compiler_log::read_errors().size() != 0) return {};
				meta_opcodes.insert(meta_opcodes.end(), new_opcodes.begin(), new_opcodes.end());

				// Add real distance to line counter. That means ignore meta-opcodes containing '<!'.
				line += std::count_if(new_opcodes.begin(), new_opcodes.end(), [](std::string s) {
					return s.find("<!") == std::string::npos;
				});
			}
		}

		return meta_opcodes;
	}

	// Generate `meta-code` from the AST.
	std::vector<std::string> generate_meta_opcodes(ASTNode program)
	{
		if (program.type == ASTNodeType::Error || compiler_log::read_errors().size() != 0)
			return {};

		std::vector<std::string> meta_opcodes;
		std::vector<std::string> variables;

		// Is used for pulling unique numbers for jumping blocks.
		unsigned label_counter_if = 1;
		unsigned label_counter_for = 500; // The if-label counter should never reach this value.

		// Walk through all the statements and convert them to opcodes.
		meta_opcodes = walk_statements_and_convert_to_meta(program, variables, label_counter_if, label_counter_for);

		return meta_opcodes;
	}
}
