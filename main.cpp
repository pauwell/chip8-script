#include <iostream>
#include <vector>
#include "parser.hpp"
#include "variable.hpp"

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
	std::string code = "var A = 30; var B = 40\nB += 20;B += A";
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

	// Test variables.
	/*std::vector<ch8scr::u16> ops;
	ch8scr::create_variable("A");
	ch8scr::create_variable("B");
	ops.push_back(ch8scr::variable_set_value("A", 12));
	ops.push_back(ch8scr::variable_set_value("B", 34));
	ops.push_back(ch8scr::variable_set_value("B", 221));*/

	/*for (const auto& e : ops) {
		std::cout << std::hex << e << '\n';
	}*/

	//std::cout << (int)ch8scr::variables["Test"].value;
	//std::cout << ch8scr::variables.size();

	return 0;
}
