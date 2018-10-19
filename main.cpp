#include <iostream>
#include <vector>
#include "generator.hpp"

// Debug output ast.
void print_ast(c8s::ASTNode node, unsigned depth=0)
{
	for (unsigned i = 0; i < depth; ++i) std::cout << "|\t";
	std::cout << "|\n";
	for (unsigned i = 0; i < depth; ++i) std::cout << "|\t";
	std::cout << "|__[";
	if (node.type == c8s::ASTNodeType::VarExpression) std::cout << "VarExpression, ";
	if (node.type == c8s::ASTNodeType::NumberLiteral) std::cout << "NumberLiteral, ";
	if (node.type == c8s::ASTNodeType::VarDeclaration) std::cout << "VarDeclaration, ";
	if (node.type == c8s::ASTNodeType::Identifier) std::cout << "Identifier, ";
	if (node.type == c8s::ASTNodeType::Operator) std::cout << "Operator, ";
	if (node.type == c8s::ASTNodeType::Program) std::cout << "Program, ";
	std::cout << node.value << "]\n";
	for (auto& e : node.params)
	{
		print_ast(e, depth + 1);
	}
}

int main()
{
	// Test parser.
	/*std::string code =
		"var A = 10\n"\
		"var B = A;\n"\
		"A += 4";*/
	std::string code =
		"var A = 6;;\n"\
		"if A == 1:\n"\
		"	A+=11\n"\
		"	if A==2:\n"\
		"		A+=21\n"\
		"		A+=22\n"\
		"		A+=23\n"\
		"	endif\n"
		"	A+=12\n"\
		"endif";

	auto tokens = c8s::tokenize(code);
	auto ast = c8s::parse(tokens);
	for (auto& e : tokens)
	{
		std::cout << "Token[";
		if (e.type == c8s::TokenType::Numerical)
			std::cout << "Numerical, ";
		if (e.type == c8s::TokenType::Var)
			std::cout << "Var, ";
		if (e.type == c8s::TokenType::Identifier)
			std::cout << "Identifier, ";
		if (e.type == c8s::TokenType::ClosingStatement)
			std::cout << "Closing statement, ";
		std::cout << e.value << "]\n";
	}
	print_ast(ast);

	auto ops = c8s::generate_asm(ast);
	for (auto op : ops)
	{
		std::cout << "0x" << std::hex << op << '\n';
	}

	std::cin.get();
	return 0;
}
