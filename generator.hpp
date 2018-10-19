#pragma once
#include "parser.hpp"
#include "types.hpp"

namespace c8s
{
	unsigned find_var_index(std::string name, std::vector<std::string>& variables)
	{
		auto found_at = std::find(variables.begin(), variables.end(), name);
		return (found_at == variables.end()) ? u8(0) : std::distance(variables.begin(), found_at);
	};

	u16 var_decl_to_opcode(const ASTNode& stmt_node, std::vector<std::string>& variables)
	{
		ASTNode source_node = stmt_node;
		ASTNode target_node = source_node.params.front();

		const std::string source_name = source_node.value;
		const std::string target_value = target_node.value;
		std::cout << "Declaring " << source_name << "=" << target_value << "\n";

		if (target_node.type == ASTNodeType::NumberLiteral)
		{
			u8 value_u8 = std::atoi(target_value.c_str());

			if (std::find(variables.begin(), variables.end(), source_name) == variables.end())
			{
				// 6XNN	Const	Vx = NN		Sets VX to NN.
				variables.push_back(source_name);
				u16 op = ((0x6 << 12) | ((variables.size() - 1) << 8) | (value_u8 & 0xFF));
				return op;
			}
			else
				std::cerr << "Variable " << source_name << " already exists!\n";
		}
		else if (target_node.type == ASTNodeType::Identifier)
		{
			if (std::find(variables.begin(), variables.end(), source_name) == variables.end())
			{
				// BUG the following doesnt seem to work with script: var A = B.

				// 8XY0	Assign	Vx=Vy
				variables.push_back(source_name);
				u8 target_v_index = find_var_index(target_node.value, variables);
				u16 op = ((0x8 << 12) | ((variables.size() - 1) << 8) | (target_v_index << 4) | (0x0));
				return op;
			}
		}
		else
			std::cerr << "Expected number literal!\n";

		return 0x0;
	}

	u16 var_expr_to_opcode(const ASTNode& stmt_node, std::vector<std::string>& variables)
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
			std::cout << "Expr: " << source_node.value << operator_node.value << (int)value_u8 << '\n';

			if (operator_node.value == "=")
			{
				// 6XNN	Const	Vx = NN
				return ((0x6 << 12) | (v_index << 8) | (value_u8 & 0xFF));
			}
			else if (operator_node.value == "+=")
			{
				// 7XNN	Const	Vx += NN
				return ((0x7 << 12) | (v_index << 8) | (value_u8 & 0xFF));
			}
			else
				std::cerr << "Unknown operator: " << operator_node.value << '\n';
		}
		else if (target_node.type == ASTNodeType::Identifier)
		{
			u8 source_v_index = find_var_index(source_node.value, variables);
			u8 target_v_index = find_var_index(target_node.value, variables);
			std::cout << "Expr: " << source_node.value << operator_node.value << target_node.value << '\n';

			if (operator_node.value == "=")
			{
				// 8XY0	Assign	Vx=Vy
				return ((0x8 << 12) | (source_v_index << 8) | (target_v_index << 4) | (0x0));
			}
			else if (operator_node.value == "+=")
			{
				// 8XY4	Math	Vx += Vy
				return ((0x8 << 12) | (source_v_index << 8) | (target_v_index << 4) | (0x4));
			}
			else if (operator_node.value == "-=")
			{
				// 8XY5	Math	Vx -= Vy
				return ((0x8 << 12) | (source_v_index << 8) | (target_v_index << 4) | (0x5));
			}
			else if (operator_node.value == "&=")
			{
				// 8XY2	BitOp	Vx=Vx&Vy
				return ((0x8 << 12) | (source_v_index << 8) | (target_v_index << 4) | (0x2));
			}
			else if (operator_node.value == "|=")
			{
				// 8XY1	BitOp	Vx=Vx|Vy
				return ((0x8 << 12) | (source_v_index << 8) | (target_v_index << 4) | (0x1));
			}
			else if (operator_node.value == "^=")
			{
				// 8XY3	BitOp	Vx=Vx^Vy
				return ((0x8 << 12) | (source_v_index << 8) | (target_v_index << 4) | (0x3));
			}
			else
				std::cerr << "Unknown operator: " << operator_node.value << '\n';
		}

		return 0x0;
	}

	u16 ast_node_to_opcode(const ASTNode& node, std::vector<std::string>& variables)
	{
		if (node.params.size() > 1)
			std::cerr << "Warning! Multiple statements in one.\n";
		const auto& stmt_node = node.params.front();

		if (stmt_node.type == ASTNodeType::VarDeclaration)
		{
			return var_decl_to_opcode(stmt_node, variables);
		}
		else if (stmt_node.type == ASTNodeType::VarExpression)
		{
			return var_expr_to_opcode(stmt_node, variables);
		}
		else if (stmt_node.type == ASTNodeType::EndOfProgram)
		{
			return 0x0;
		}
		else
			std::cerr << "Invalid statement: " << stmt_node.value << '\n';
		
		return 0x0;
	}

	// Generate chip-8 assembly-code from AST.
	std::vector<u16> generate_asm(ASTNode program)
	{
		std::cout << "Generating asm:\n";

		std::vector<u16> opcodes;
		std::vector<std::string> variables;

		for (const auto& node : program.params)
		{
			u16 op = ast_node_to_opcode(node, variables);
			if (op != 0x0)
				opcodes.push_back(op);
			else
				return opcodes;
		}

		return opcodes;
	}
}
