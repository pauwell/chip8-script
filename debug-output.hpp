#pragma once
#include "meta-gen.hpp"

namespace c8s
{
	// Debug output seperator line.
	void print_seperator(bool strong = false)
	{
		for (unsigned i = 0; i < 65; ++i)
			std::cout << ((strong) ? '=' : '-');
		std::cout << '\n';
	}

	// Debug output tokens.
	void print_tokens(const std::vector<c8s::Token>& tokens)
	{
		print_seperator();
		std::cout << "1] Split input code into tokens\n";
		print_seperator();
		for (const auto& e : tokens)
		{
			std::cout << "T[" << e.value << "] ";
			if (e.type == c8s::TokenType::ClosingStatement)
				std::cout << '\n';
		}
		std::cout << '\n';
	}

	// Debug output ast.
	void print_ast(c8s::ASTNode node, unsigned depth = 0)
	{
		if (depth == 0)
		{
			print_seperator();
			std::cout << "2] Parse tokens into abstract syntax tree\n";
			print_seperator();
		}
		for (unsigned i = 0; i < depth; ++i) std::cout << "|`\t";
		//std::cout << "|\n";
		//for (unsigned i = 0; i < depth; ++i) std::cout << "|\t";
		std::cout << "+--[";
		if (node.type == c8s::ASTNodeType::VarExpression) std::cout << "VarExpression, ";
		if (node.type == c8s::ASTNodeType::NumberLiteral) std::cout << "NumberLiteral, ";
		if (node.type == c8s::ASTNodeType::VarDeclaration) std::cout << "VarDeclaration, ";
		if (node.type == c8s::ASTNodeType::Identifier) std::cout << "Identifier, ";
		if (node.type == c8s::ASTNodeType::Operator) std::cout << "Operator, ";
		if (node.type == c8s::ASTNodeType::IfStatement) std::cout << "IfStmt, ";
		if (node.type == c8s::ASTNodeType::ForLoop) std::cout << "ForLoop, ";
		if (node.type == c8s::ASTNodeType::Program) std::cout << "Program, ";
		std::cout << node.value << "]\n";
		for (auto& e : node.params)
		{
			print_ast(e, depth + 1);
		}
	}

	// Debug output meta-opcodes.
	void print_meta(std::vector<std::string> meta_ops)
	{
		print_seperator();
		std::cout << "3] Creating `meta-opcodes` from the AST\n";
		print_seperator();
		std::cout << "(This is the last step before the finished opcodes. Now we only have to ";
		std::cout << "resolve the labels `<1>,<2>..` to their line-number at `<!1!>,<!2!>..`)\n";
		for (auto op : meta_ops)
		{
			std::cout << "0x" << op << '\n';
		}
	}
}
