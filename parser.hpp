#pragma once
#include <iostream>
#include <cstdlib>
#include <exception>
#include <array>

#include "tokenizer.hpp"

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
		Operator,	// ==, +=, + ...
		VarDeclaration,	// var XYZ
		VarExpression,	// X += Y
		IfStatement,	// if A==B:
		EndifStatement,	// End marker for if-statement bodies.
		Identifier,	// Name of a (valid) variable.
		NumberLiteral	// Any number (1,2,3 ...)
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
			return create_node_and_walk(ASTNodeType::IfStatement, tok, cursor, walk);
		}
		
		if (tok.type == TokenType::Endif)
		{
			++cursor;
			return ASTNode{ ASTNodeType::EndifStatement, tok.value, {} };
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

	// Every statement between `if` and `endif` is moved into the `params` 
	// of the `if`statement. The `endif` statement gets removed.
	void move_condition_bodies_to_params(ASTNode& ast)
	{
		for (;;) 
		{
			// Find the last/innermost if-statement node that does not yet have more than one parent.
			unsigned innermost_if_stmt_index = 0;
			for (unsigned i = 0; i < ast.params.size(); ++i)
			{
				if (ast.params[i].params.front().type == ASTNodeType::IfStatement && ast.params[i].params.size() <= 1)
				{
					innermost_if_stmt_index = i;
				}
			}

			// Finish the loop if no if-nodes were left to find.
			if (innermost_if_stmt_index == 0)
			{
				break;
			}

			// Push the nodes that follow onto the if-statement's `params` until `endif` is reached.
			for (unsigned i = innermost_if_stmt_index + 1; ast.params[i].params.front().type != ASTNodeType::EndifStatement; ++i) // does `i<params.size()` even make sense!?
			{
				// Copy the node over to the params of the if-statement.
				ast.params[innermost_if_stmt_index].params.push_back(ast.params[i]);

				// Mark the old node to be deleted.
				ast.params[i].type = ASTNodeType::Deletable;
				ast.params[i].value = "del";
				ast.params[i].params.clear();

				// Remove the endif-node.
				if (ast.params[i + 1].params.front().type == ASTNodeType::EndifStatement)
				{
					ast.params.erase(ast.params.begin() + i + 1);
					std::cout << "Found endif: " << ast.params[i + 1].params.front().value << '\n';
					break;
				}
			}

			// Clean up nodes that were marked as `deletable`.
			remove_deletables(ast);
		}
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

		move_condition_bodies_to_params(ast);

		return ast;
	}
}
