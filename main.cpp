#include <iostream>
#include <vector>
#include "generator.hpp"

// Debug output ast.
void print_ast(ch8scr::ASTNode node, unsigned depth=0)
{
	for (unsigned i = 0; i < depth; ++i) std::cout << "|\t";
	std::cout << "|\n";
	for (unsigned i = 0; i < depth; ++i) std::cout << "|\t";
	std::cout << "|__[";
	if (node.type == ch8scr::ASTNodeType::VarExpression) std::cout << "VarExpression, ";
	if (node.type == ch8scr::ASTNodeType::NumberLiteral) std::cout << "NumberLiteral, ";
	if (node.type == ch8scr::ASTNodeType::VarDeclaration) std::cout << "VarDeclaration, ";
	if (node.type == ch8scr::ASTNodeType::Identifier) std::cout << "Identifier, ";
	if (node.type == ch8scr::ASTNodeType::Operator) std::cout << "Operator, ";
	if (node.type == ch8scr::ASTNodeType::Program) std::cout << "Program, ";
	std::cout << node.value << "]\n";
	for (auto& e : node.params)
	{
		print_ast(e, depth + 1);
	}
}

int main()
{
	// Test parser.
	std::string code =
		"var A = 8\n"\
		"var B = 16\n"\
		"A = 10\n"\
		"A = B\n"\
		"A += B\n"\
		"A += 8\n"\
		"A |= B\n"\
		"A &= B\n"\
		"A ^= B\n";

	auto tokens = ch8scr::tokenize(code);
	auto ast = ch8scr::parse(tokens);
	for (auto& e : tokens)
	{
		std::cout << "Token[";
		if (e.type == ch8scr::TokenType::Numerical)
			std::cout << "Numerical, ";
		if (e.type == ch8scr::TokenType::Var)
			std::cout << "Var, ";
		if (e.type == ch8scr::TokenType::Identifier)
			std::cout << "Identifier, ";
		if (e.type == ch8scr::TokenType::ClosingStatement)
			std::cout << "Closing statement, ";
		std::cout << e.value << "]\n";
	}
	print_ast(ast);

	auto ops = ch8scr::generate_asm(ast);
	for (auto op : ops)
	{
		std::cout << "0x" << std::hex << op << '\n';
	}

	std::cin.get();
	return 0;
}
