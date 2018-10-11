#pragma once
#include <iostream>
#include <cstdlib>
#include <cctype>
#include <string>
#include <vector>
#include <functional>
#include <exception>
//#include <algorithm> // std::transform

namespace ch8scr
{
	enum class TokenType { Var, Identifier, Operator, Numerical, ClosingStatement, EndOfProgram };

	// A single token.
	struct Token
	{
		Token(TokenType type, std::string value) : type{ type }, value{ value } {}
		TokenType type; // Possible: declaration, identifier, operator, numerical, closer.
		std::string value;
	};

	// Read characters with the cursor until the validator returns false.
	std::string read_token_string(std::string& input_code, std::string::iterator& cursor, std::function<int(int)> validator)
	{
		std::string token;
		for (; cursor != input_code.end() && validator(*cursor); ++cursor)
		{
			token += *cursor;
		}
		return token;
	}

	// Parse the input code into a list of tokens.
	std::vector<Token> tokenize(std::string input_code)
	{
		// Append newline or semicolon to the end of the code if missing.
		char last_char_in_code = input_code[input_code.length() - 1];
		if (last_char_in_code != ';' && last_char_in_code != '\n')
		{
			input_code += ';';
		}

		// Convert `input_code` into lowercase.
		//std::transform(input_code.begin(), input_code.end(), input_code.begin(), std::tolower);

		std::vector<Token> tokens;
		auto cursor = input_code.begin();

		while (cursor != input_code.end())
		{
			char current_char = *cursor;
			std::cout << current_char << '\n';

			// Newline and semicolon (End of statement).
			if (current_char == '\n' || current_char == ';')
			{
				tokens.push_back(Token{ TokenType::ClosingStatement, std::string{ ';' } });
				++cursor;
			}
			// Tab and whitespace.
			else if (std::isblank(current_char))
			{
				++cursor;
			}
			// Operators.
			else if (current_char == '=' || current_char == '+')
			{
				std::string tok = read_token_string(input_code, cursor, static_cast<int(*)(int)>(std::ispunct));
				tokens.push_back(Token{ TokenType::Operator, tok });
			}
			// Numerical.
			else if (std::isdigit(current_char))
			{
				std::string tok = read_token_string(input_code, cursor, static_cast<int(*)(int)>(std::isdigit));
				tokens.push_back(Token{ TokenType::Numerical, tok });
			}
			// Letters.
			else if (std::isalpha(current_char))
			{
				std::string tok = read_token_string(input_code, cursor, static_cast<int(*)(int)>(std::isalpha));
				if (tok == "var") tokens.push_back(Token{ TokenType::Var, tok });
				else tokens.push_back(Token{ TokenType::Identifier, tok });
			}
			else
			{
				throw std::runtime_error("Unexpected character in input code: " + current_char + '\n');
			}
		}

		tokens.push_back(Token{ TokenType::EndOfProgram, "end" });

		return tokens;
	}

	// -------------------------------------------------------------------------

	enum class ASTNodeType{ Program, EndOfProgram, Error, Statement, Operator, VarDeclaration, VarExpression, Identifier, NumberLiteral };

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
			{
				throw std::runtime_error("Expected `=`!");
			}
			var_decl.params.push_back(walk(++cursor, token_list));

			return var_decl;
		}

		if (tok.type == TokenType::Identifier)
		{
			ASTNode var_expr = ASTNode{ ASTNodeType::VarExpression, tok.value, {} };
			(++cursor);
			if (cursor->value == "+=")
			{
				var_expr.params.push_back(ASTNode{ ASTNodeType::Operator, cursor->value, {} });
				++cursor;
				if (cursor->type == TokenType::Identifier)
				{
					var_expr.params[0].params.push_back(ASTNode{ ASTNodeType::Identifier, cursor->value, {} });
					++cursor;
					if (cursor->type == TokenType::ClosingStatement)
					{
						++cursor;
					}
					else
					{
						throw std::runtime_error("Expected closing statement!");
					}
					return var_expr;
				}
				else if (cursor->type == TokenType::Numerical)
				{
					var_expr.params[0].params.push_back(ASTNode{ ASTNodeType::NumberLiteral, cursor->value, {} });
					++cursor;
					if (cursor->type == TokenType::ClosingStatement)
					{
						++cursor;
					}
					else
					{
						throw std::runtime_error("Expected closing statement!");
					}
					return var_expr;
				}
				else
				{
					throw std::runtime_error("Expected an identifier or number literal!");
				}
			}
			else 
			{
				throw std::runtime_error("Unknown operator!");
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
