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
#include <numeric>

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
		Raw,	// raw 6001.
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
		NumberLiteral,	// Any number (1,2,3 ...)
		FunctionCall,	// cls()
		OpenBrace,		// (
		ClosingBrace	// )
	};

	// List of all supported operators.
	const std::array<std::string, 11> valid_operators = {
		":", "=", "==", "!=", "+=", "-=", "<<=", ">>=", "|=", "&=", "^="
	};

	// A node in the abstract syntax tree.
	struct ASTNode
	{
		ASTNode(ASTNodeType type, std::string value, unsigned line_number, std::vector<ASTNode> params)
			: type{ type }, value{ value }, line_number{ line_number }, params{ params } {}
		ASTNodeType type;
		std::string value;
		unsigned line_number;
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
		ASTNode node = ASTNode{ node_type, tok.value, tok.line_number, {} };
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
				ASTNode var_decl = ASTNode{ ASTNodeType::VarDeclaration, (++cursor)->value, tok.line_number, {} };
				var_decl.params.push_back(walk(++cursor, var_decl));
				return var_decl;
			}
			if (tok.type == TokenType::Identifier)
			{
				ASTNode var_expr = ASTNode{ ASTNodeType::VarExpression, tok.value, tok.line_number, {} };
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
			if (tok.type == TokenType::Raw)
			{
				ASTNode raw_expr{ ASTNodeType::Raw, tok.value, tok.line_number, {} };
				raw_expr.params.push_back(walk(++cursor, raw_expr));
				return raw_expr;
			}
			if (tok.type == TokenType::FunctionCall)
			{
				return create_node_and_walk(ASTNodeType::FunctionCall, tok, cursor, walk);
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
			if (tok.type == TokenType::Colon)
			{
				return create_node_and_walk(ASTNodeType::Operator, tok, cursor, walk);
			}
		}
		else if (parent.type == ASTNodeType::FunctionCall)
		{
			if (tok.type == TokenType::OpenBrace)
			{
				return create_node_and_walk(ASTNodeType::OpenBrace, tok, cursor, walk);
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
			// TODO if a==b: does not work

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
		else if (parent.type == ASTNodeType::Raw)
		{
			if (tok.type == TokenType::Numerical)
			{
				++cursor;
				return ASTNode{ ASTNodeType::NumberLiteral, tok.value, tok.line_number, {} };
			}
		}
		
		if (tok.type == TokenType::Endif)
		{
			++cursor;
			return ASTNode{ ASTNodeType::EndifStatement, tok.value, tok.line_number, {} };
		}

		if (tok.type == TokenType::Endfor)
		{
			++cursor;
			return ASTNode{ ASTNodeType::EndforLoop, tok.value, tok.line_number, {} };
		}

		if (tok.type == TokenType::ClosingBrace)
		{
			++cursor;
			return ASTNode{ ASTNodeType::ClosingBrace, tok.value, tok.line_number, {} };
		}

		if (tok.type == TokenType::Numerical)
		{
			++cursor;
			return ASTNode{ ASTNodeType::NumberLiteral, tok.value, tok.line_number, {} };
		}

		if (tok.type == TokenType::EndOfProgram)
		{
			++cursor;
			return ASTNode{ ASTNodeType::EndOfProgram, "end", tok.line_number, {} };
		}


		compiler_log::write_error("Syntax error on line " + std::to_string(tok.line_number));
		++cursor;
		return ASTNode{ ASTNodeType::Error, "error", tok.line_number, {} };
	}


	// The `bodies` of if-statements and for-loops is moved into the `params` of
	// the parent. 
	bool move_bodies_to_params(ASTNode& ast)
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

			// Finish the loop if no nodes are left.
			if (from_type == ASTNodeType::Error || to_type == ASTNodeType::Error)
				break;

			// Log error and return false if the body of the condition is empty.
			if (ast.params[innermost_stmt_index + 1].params.front().type == to_type)
			{
				compiler_log::write_error("Bodies of if-statements can not be empty!");
				return false;
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
		return true;
	}

	// Parse the list of tokens into an AST.
	ASTNode parse_tokens_to_ast(std::vector<Token> &token_list)
	{
		if (token_list.size() == 0 || compiler_log::read_errors().size() > 0)
		{
			return ASTNode{ ASTNodeType::Error, "error", 0, {} };
		}

		// Check for missing endif/endfor statements.
		auto open_if_for = std::count_if(token_list.begin(), token_list.end(), [](Token t) { 
			return t.type == TokenType::If || t.type == TokenType::For; 
		});
		auto closed_if_for = std::count_if(token_list.begin(), token_list.end(), [](Token t) { 
			return t.type == TokenType::Endif || t.type == TokenType::Endfor; 
		});
		if (open_if_for != closed_if_for)
		{
			compiler_log::write_error("Missing endif/endfor\n");
			return ASTNode{ ASTNodeType::Error, "error", 0,{} };
		}


		auto cursor = token_list.begin();
		auto ast = ASTNode{ ASTNodeType::Program, "", 0, {} };

		while (cursor != token_list.end())
		{
			// Skip closing-statements.
			if (cursor->type == TokenType::ClosingStatement)
			{
				++cursor;
				continue;
			}

			// Add statements and recursively call `walk()` on their parameters.
			ASTNode stmt = ASTNode{ ASTNodeType::Statement, "stmt", cursor->line_number, {} };
			stmt.params.push_back(walk(cursor, stmt));
			ast.params.push_back(stmt);
		}

		move_bodies_to_params(ast);

		// Check for empty if-bodies.
		if (!move_bodies_to_params(ast) || compiler_log::read_errors().size() != 0)
			return ASTNode{ ASTNodeType::Error, "error", 0, {} };

		return ast;
	}
}
