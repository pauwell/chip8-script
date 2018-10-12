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

	// Walk the token list.
	ASTNode walk(std::vector<Token>::iterator &cursor, std::vector<Token> &token_list)
	{
		const Token &tok = *cursor;

		if (tok.type == TokenType::Numerical)
		{
			++cursor;
			return ASTNode{ ASTNodeType::NumberLiteral, tok.value, {} };
		}
			
		if (tok.type == TokenType::Var)
		{
			++cursor;
			ASTNode var_decl = ASTNode{ ASTNodeType::VarDeclaration, cursor->value, {} };
			if ((++cursor)->value.front() != '=')
				return ASTNode{ ASTNodeType::Error, ("Expected assignment operator `=`!"),{} };
			var_decl.params.push_back(walk(++cursor, token_list));

			return var_decl;
		}

		if (tok.type == TokenType::Identifier)
		{
			ASTNode var_expr = ASTNode{ ASTNodeType::VarExpression, tok.value, {} };
			++cursor;
			if (cursor->type == TokenType::Operator &&
				std::find(valid_operators.begin(), valid_operators.end(), cursor->value) != valid_operators.end()) 
			{
				var_expr.params.push_back(ASTNode{ ASTNodeType::Operator, cursor->value, {} });
				++cursor;
				if (cursor->type == TokenType::Identifier)
				{
					var_expr.params[0].params.push_back(ASTNode{ ASTNodeType::Identifier, cursor->value, {} });
					if ((++cursor)->type != TokenType::ClosingStatement)
						return ASTNode{ ASTNodeType::Error, ("Expected closing statement!"),{} };
					++cursor;
					return var_expr;
				}
				else if (cursor->type == TokenType::Numerical)
				{
					var_expr.params[0].params.push_back(ASTNode{ ASTNodeType::NumberLiteral, cursor->value, {} });
					if ((++cursor)->type != TokenType::ClosingStatement)
						return ASTNode{ ASTNodeType::Error, ("Expected closing statement!"),{} };
					++cursor;
					return var_expr;
				}
				else 
					return ASTNode{ ASTNodeType::Error, ("Unexpected token " + cursor->value), {} };
			}
			else 
				return ASTNode{ ASTNodeType::Error, ("Unknown operand!"), {} };
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
			if (cursor->type == TokenType::ClosingStatement)
			{
				++cursor;
				continue;
			}

			// Add statements and recursively call `walk()` on their parameters.
			ASTNode stmt = ASTNode{ ASTNodeType::Statement, "stmt", {} };
			stmt.params.push_back(walk(cursor, token_list));
			ast.params.push_back(stmt);
		}

		return ast;
	}
}
