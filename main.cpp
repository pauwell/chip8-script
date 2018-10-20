#include <iostream>
#include <vector>
#include "generator.hpp"

// Debug output tokens.
void print_tokens(const std::vector<c8s::Token>& tokens)
{
	std::cout << "\n\nCreated tokens =>\n";
	for (const auto& e : tokens)
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
}

// Debug output ast.
void print_ast(c8s::ASTNode node, unsigned depth=0)
{
	if(depth==0)std::cout << "\n\nCreated AST =>\n";
	for (unsigned i = 0; i < depth; ++i) std::cout << "|`\t";
	//std::cout << "|\n";
	//for (unsigned i = 0; i < depth; ++i) std::cout << "|\t";
	std::cout << "+--[";
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

// Debug output opcodes.
void print_asm(std::vector<c8s::u16> ops)
{
	std::cout << "\n\nCreated opcodes =>\n";
	for (auto op : ops)
	{
		std::cout << "0x" << std::hex << op << '\n';
	}
}

void test(std::string what, std::string code, 
	bool is_print_asm = true, bool is_print_ast = true, bool is_print_tokens = false)
{
	std::cout << "\n\n#######\nRunning test: " << what << '\n';
	
	// Tokenize.
	auto tokens = c8s::tokenize(code);
	if (is_print_tokens) print_tokens(tokens);
	
	// Parse.
	auto ast = c8s::parse(tokens);
	if (is_print_ast) print_ast(ast);
	
	// Generate.
	auto ops = c8s::generate_asm(ast);
	if (is_print_asm) print_asm(ops);
}

int main()
{
	system("mode 150");

	test(
		"Nested if-statements",

		"var A = 6;;\n"\
		"if A == 1:\n"\
		"	A+=11\n"\
		"	if A==2:\n"\
		"		A+=21\n"\
		"		A+=22\n"\
		"		A+=23\n"\
		"	endif\n"
		"	A+=12\n"\
		"endif"
	);

	test(
		"Nested for-loops and if-statements",
		
		"VAR x = 0\n"\
		"IF x == 0:\n"\
		"	FOR I = 0 TO 10:\n"\
		"		VAR a = 10\n"\
		"		VAR b = A;\n"\
		"		a += 4\n"\
		"	ENDFOR\n"\
		"ENDIF"
	);

	std::cin.get();
	return 0;
}
