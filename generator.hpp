#pragma once
#include "parser.hpp"
#include "types.hpp"

namespace ch8scr
{
	// Generate chip-8 assembly-code from AST.
	std::vector<u16> generate_asm(ASTNode program)
	{
		std::cout << "Generating asm:\n";

		std::vector<u16> opcodes;
		std::vector<std::string> variables;

		auto find_var_index = [](std::string name, std::vector<std::string> variables) -> u8
		{
			auto found_at = std::find(variables.begin(), variables.end(), name);
			return (found_at == variables.end()) ? u8(0) : std::distance(variables.begin(), found_at);
		};

		for (const auto& node : program.params)
		{
			for (const auto& stmt_node : node.params)
			{
				if (stmt_node.type == ASTNodeType::VarDeclaration)
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
							opcodes.push_back(op);
						}
						else
							std::cerr << "Variable " << source_name << " already exists!\n";
					}
					else if (target_node.type == ASTNodeType::Identifier)
					{
						if (std::find(variables.begin(), variables.end(), source_name) == variables.end())
						{
							// 8XY0	Assign	Vx=Vy	Sets VX to the value of VY.
							variables.push_back(source_name);
							u8 target_v_index = find_var_index(target_node.value, variables);
							u16 op = ((0x8 << 12) | ((variables.size() - 1) << 8) | (target_v_index << 4) | (0x0));
							opcodes.push_back(op);
						}
					}
					else 
						std::cerr << "Expected number literal!\n";
					
				}
				else if (stmt_node.type == ASTNodeType::VarExpression)
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
							// 6XNN	Const	Vx = NN		Sets VX to NN.
							u16 op = ((0x6 << 12) | (v_index << 8) | (value_u8 & 0xFF));
							opcodes.push_back(op);
						}
						else if (operator_node.value == "+=")
						{
							// 7XNN	Const	Vx += NN	Adds NN to VX. (Carry flag is not changed)
							u16 op = ((0x7 << 12) | (v_index << 8) | (value_u8 & 0xFF));
							opcodes.push_back(op);
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
							// 8XY0	Assign	Vx=Vy	Sets VX to the value of VY.
							u16 op = ((0x8 << 12) | (source_v_index << 8) | (target_v_index << 4) | (0x0));
							opcodes.push_back(op);
						}
						else if (operator_node.value == "+=")
						{
							// 8XY4	Math	Vx += Vy	Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
							u16 op = ((0x8 << 12) | (source_v_index << 8) | (target_v_index << 4) | (0x4));
							opcodes.push_back(op);
						}
					}
				}
				else if (stmt_node.type == ASTNodeType::EndOfProgram)
				{
					return opcodes;
				}
				else
					std::cerr << "Invalid statement: " << stmt_node.value << '\n';
			}
		}

		std::cerr << "'EndOfProgram'-node missing!\n";
		return opcodes;
	}
}
