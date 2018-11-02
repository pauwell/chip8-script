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

#include <iostream>
#include <cstdlib>
#include <exception>
#include <array>

#include "token-parser.hpp"

namespace c8s
{
	// Different types of AST-nodes.
	enum class ASTNodeType
	{ 
		Program,	// Root node of the AST.
		EndOfProgram,	// Must be the last statement in the program.
		Error,		// Gets inserted to show where an error happened.
		Deletable,	// Nodes that are `Deletable` should be removed.
		Statement,	// A single statement.
		Operator,	// E.g: ==, +=, + ...
		VarDeclaration,	// var XYZ
		VarExpression,	// X += Y
		IfStatement,	// if A==B:
		EndifStatement,	// End marker for if-statement bodies.
		ForLoop,	// for I=0 to 10:
		To,		// 0 to 10.		
		Step,	// 0 to 10 step 1
		EndforLoop,	// End marker for for-loop bodies.
		Identifier,	// Name of a (valid) variable.
		NumberLiteral	// Any number (1,2,3 ...)
	};

	// List of all supported operators.
	const std::array<std::string, 11> valid_operators = {
		":", "=", "==", "!=", "+=", "-=", "<<=", ">>=", "|=", "&=", "^="
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

	// Remove every node from the AST that is marked as `Deletable`.
	void remove_deletables(ASTNode& ast)
	{
		ast.params.erase(std::remove_if(ast.params.begin(), ast.params.end(),
			[](const ASTNode& node) { return node.type == ASTNodeType::Deletable; }),
			ast.params.end()
		);
	}

	// Create a new node of a given type and continue `walking` the tree.
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

		if (parent.type == ASTNodeType::Statement)
		{
			if (tok.type == TokenType::Var)
			{
				ASTNode var_decl = ASTNode{ ASTNodeType::VarDeclaration, (++cursor)->value,{} };
				var_decl.params.push_back(walk(++cursor, var_decl));
				return var_decl;
			}
			if (tok.type == TokenType::Identifier)
			{
				ASTNode var_expr = ASTNode{ ASTNodeType::VarExpression, tok.value, {} };
				var_expr.params.push_back(walk(++cursor, var_expr));
				return var_expr;
				
			}
			if (tok.type == TokenType::If)
			{
				return create_node_and_walk(ASTNodeType::IfStatement, tok, cursor, walk);
			}
			if (tok.type == TokenType::For)
			{
				return create_node_and_walk(ASTNodeType::ForLoop, tok, cursor, walk);
			}
		}	
		else if (parent.type == ASTNodeType::Identifier)
		{
			if (tok.type == TokenType::Operator)
			{
				return create_node_and_walk(ASTNodeType::Operator, tok, cursor, walk);
			}
			if (tok.type == TokenType::To)
			{
				return create_node_and_walk(ASTNodeType::To, tok, cursor, walk);
			}
		}
		else if (parent.type == ASTNodeType::Operator)
		{
			if (tok.type == TokenType::Identifier)
			{
				return create_node_and_walk(ASTNodeType::Identifier, tok, cursor, walk);
			}
			if (tok.type == TokenType::Numerical)
			{
				return create_node_and_walk(ASTNodeType::NumberLiteral, tok, cursor, walk);
			}
		}
		else if (parent.type == ASTNodeType::VarDeclaration)
		{
			if (tok.type == TokenType::Identifier)
			{
				return create_node_and_walk(ASTNodeType::Identifier, tok, cursor, walk);
			}
			if (tok.type == TokenType::Operator)
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
		else if (parent.type == ASTNodeType::IfStatement)
		{
			if (tok.type == TokenType::Identifier)
			{
				return create_node_and_walk(ASTNodeType::Identifier, tok, cursor, walk);
			}
			//return create_node_and_walk(ASTNodeType::IfStatement, tok, cursor, walk);
		}
		else if (parent.type == ASTNodeType::ForLoop)
		{
			if (tok.type == TokenType::Identifier)
			{
				return create_node_and_walk(ASTNodeType::Identifier, tok, cursor, walk);
			}
			//return create_node_and_walk(ASTNodeType::ForLoop, tok, cursor, walk);
		}
		else if (parent.type == ASTNodeType::NumberLiteral)
		{
			if (tok.type == TokenType::To)
			{
				return create_node_and_walk(ASTNodeType::To, tok, cursor, walk);
			}
			if (tok.type == TokenType::Step)
			{
				return create_node_and_walk(ASTNodeType::Step, tok, cursor, walk);
			}
			if (tok.type == TokenType::Colon)
			{
				return create_node_and_walk(ASTNodeType::Operator, tok, cursor, walk);
			}
		}
		else if (parent.type == ASTNodeType::To)
		{
			if (tok.type == TokenType::Numerical)
			{
				return create_node_and_walk(ASTNodeType::NumberLiteral, tok, cursor, walk);
			}
		}
		else if (parent.type == ASTNodeType::Step)
		{
			if (tok.type == TokenType::Numerical)
			{
				return create_node_and_walk(ASTNodeType::NumberLiteral, tok, cursor, walk);
			}
		}
		
		if (tok.type == TokenType::Endif)
		{
			++cursor;
			return ASTNode{ ASTNodeType::EndifStatement, tok.value, {} };
		}

		if (tok.type == TokenType::Endfor)
		{
			++cursor;
			return ASTNode{ ASTNodeType::EndforLoop, tok.value,{} };
		}

		if (tok.type == TokenType::Numerical)
		{
			++cursor;
			return ASTNode{ ASTNodeType::NumberLiteral, tok.value,{} };
		}

		if (tok.type == TokenType::EndOfProgram)
		{
			++cursor;
			return ASTNode{ ASTNodeType::EndOfProgram, "end", {} };
		}

		++cursor;
		return ASTNode{ ASTNodeType::Error, "@no_impl", {} };
	}


	// The `bodies` of if-statements and for-loops is moved into the `params` of
	// the parent. 
	void move_bodies_to_params(ASTNode& ast)
	{
		for (;;) 
		{
			// Find the last/innermost statement node that does not yet have more than one parent.
			unsigned innermost_stmt_index = 0;
			ASTNodeType from_type = ASTNodeType::Error;
			ASTNodeType to_type = ASTNodeType::Error;
			for (unsigned i = 0; i < ast.params.size(); ++i)
			{
				const auto& params = ast.params[i].params;
				const auto type = params.front().type;

				if ((type == ASTNodeType::IfStatement || type == ASTNodeType::ForLoop) && params.size() <= 1)
				{
					innermost_stmt_index = i;
					from_type = type;
					to_type = (type == ASTNodeType::ForLoop) ? ASTNodeType::EndforLoop : ASTNodeType::EndifStatement;
				}
			}

			// Finish the loop if no nodes were left to find.
			if (from_type == ASTNodeType::Error || to_type == ASTNodeType::Error)
			{
				break;
			}

			// Push the nodes that follow onto the statement's `params` until `to_type` is reached.
			for (unsigned i = innermost_stmt_index + 1; ast.params[i].params.front().type != to_type; ++i)
			{
				// Copy the node over to the params of the statement.
				ast.params[innermost_stmt_index].params.push_back(ast.params[i]);

				// Mark the old node to be deleted.
				ast.params[i].type = ASTNodeType::Deletable;
				ast.params[i].value = "del";
				ast.params[i].params.clear();

				// Remove the `to_type`-node.
				if (ast.params[i + 1].params.front().type == to_type)
				{
					ast.params[innermost_stmt_index].params.push_back(ast.params[i + 1]);
					ast.params.erase(ast.params.begin() + i + 1);
					break;
				}
			}

			// Clean up nodes that were marked as `deletable`.
			remove_deletables(ast);
		}
	}

	// Parse the list of tokens into an AST.
	ASTNode parse_tokens_to_ast(std::vector<Token> &token_list)
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

		move_bodies_to_params(ast);

		return ast;
	}
}
