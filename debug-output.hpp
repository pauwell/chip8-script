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

#include "meta-gen.hpp"
#include "opcode-analyser.hpp"

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
		std::cout << "(This is the last step before the finished opcodes.\n";
		std::cout << "Now we only have to resolve the labels `<1>,<2>..`\n"; 
		std::cout << "to their line - number at `<!1!> , <!2!>..`)\n";
		for (auto op : meta_ops)
		{
			std::cout << "0x" << op << '\n';
		}
	}

	// Debug output opcodes.
	void print_opcodes(std::vector<u16> opcodes)
	{
		print_seperator();
		std::cout << "4] Creating finished opcodes from `meta`\n";
		print_seperator();
		analyse_opcodes(opcodes);
	}
}
