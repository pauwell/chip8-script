#pragma once
#include <iostream>
#include <cstdlib>
#include <exception>
#include <array>

#include "tokenizer.hpp"

namespace ch8scr
{
	// Different types of AST-nodes.
	enum class ASTNodeType
	{ 
		Program, 
		EndOfProgram, 
		Error, 
		Statement, 
		Operator, 
		VarDeclaration, 
		VarExpression, 
		IfStatement,
		Identifier, 
		NumberLiteral 
	};

	// List of all supported operators.
	const std::array<std::string, 10> valid_operators = {
		"=", "==", "!=", "+=", "-=", "<<=", ">>=", "|=", "&=", "^="
	};

	// A node in the abstract syntax tree.
	struct ASTNode
	{
		ASTNode(ASTNodeType type, std::string value, std::vector<ASTNode> params)
			: type{ type }, value{ value }, params{ params } {}
		ASTNodeType type;
		std::string value;
		std::vector<ASTNode> params;
	};

	template<typename T>
	auto create_node_and_walk(ASTNodeType node_type, Token tok, std::vector<Token>::iterator &cursor,  T walk)
	{
		ASTNode node = ASTNode{ node_type, tok.value,{} };
		if ((++cursor)->type != TokenType::ClosingStatement)
			node.params.push_back(walk(cursor, node));
		return node;
	}

	// Walk the token list.
	ASTNode walk(std::vector<Token>::iterator &cursor, const ASTNode& parent)
	{
		const Token &tok = *cursor;

		if (tok.type == TokenType::Numerical)
		{
			++cursor;
			return ASTNode{ ASTNodeType::NumberLiteral, tok.value, {} };
		}

		if (parent.type == ASTNodeType::Statement)
		{
			if (tok.type == TokenType::Var)
			{
				ASTNode var_decl = ASTNode{ ASTNodeType::VarDeclaration, (++cursor)->value,{} };
				var_decl.params.push_back(walk(++cursor, var_decl));
				return var_decl;
			}
			else if (tok.type == TokenType::Identifier)
			{
				ASTNode var_expr = ASTNode{ ASTNodeType::VarExpression, tok.value, {} };
				var_expr.params.push_back(walk(++cursor, var_expr));
				return var_expr;
				
			}
			else if (tok.type == TokenType::If)
			{
				// TODO implement if-statement.
			}
		}	
		else if (parent.type == ASTNodeType::Identifier)
		{
			if (tok.type == TokenType::Operator)
			{
				return create_node_and_walk(ASTNodeType::Operator, tok, cursor, walk);
			}
		}
		else if (parent.type == ASTNodeType::Operator)
		{
			if (tok.type == TokenType::Identifier)
			{
				return create_node_and_walk(ASTNodeType::Identifier, tok, cursor, walk);
			}
		}
		else if (parent.type == ASTNodeType::VarDeclaration)
		{
			if (tok.type == TokenType::Identifier)
			{
				return create_node_and_walk(ASTNodeType::Identifier, tok, cursor, walk);
			}
			else if (tok.type == TokenType::Operator)
			{
				return create_node_and_walk(ASTNodeType::Operator, tok, cursor, walk);
			}
		}
		else if (parent.type == ASTNodeType::VarExpression)
		{
			if (tok.type == TokenType::Operator)
			{
				return create_node_and_walk(ASTNodeType::Operator, tok, cursor, walk);
			}
		}
		
		if (tok.type == TokenType::EndOfProgram)
		{
			++cursor;
			return ASTNode{ ASTNodeType::EndOfProgram, "end", {} };
		}

		++cursor;
		return ASTNode{ ASTNodeType::Error, "@no_impl", {} };
	}

	// Parse the list of tokens into an AST.
	ASTNode parse(std::vector<Token> &token_list)
	{
		auto cursor = token_list.begin();
		auto ast = ASTNode{ ASTNodeType::Program, "", {} };

		// Check for errors in the input token-list.
		if (token_list.back().type == TokenType::Error)
		{
			ast.value = "error";
			return ast;
		}

		while (cursor != token_list.end())
		{
			// Skip closing-statements.
			if (cursor->type == TokenType::ClosingStatement)
			{
				++cursor;
				continue;
			}

			// Add statements and recursively call `walk()` on their parameters.
			ASTNode stmt = ASTNode{ ASTNodeType::Statement, "stmt", {} };
			stmt.params.push_back(walk(cursor, stmt));
			ast.params.push_back(stmt);
		}

		return ast;
	}
}
