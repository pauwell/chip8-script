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

#include <cctype>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>

#include "compiler_log.hpp"

#define INT_FUNC(f) static_cast<int(*)(int)>(f) 
#define UCHAR(c) static_cast<unsigned char>(c)

namespace c8s
{
	// Different types of tokens.
	enum class TokenType 
	{ 
		Var, 
		If,
		Endif,
		For,
		To,
		Step,
		Endfor,
		Colon, 
		Identifier, 
		Operator, 
		Numerical,
		Raw,
		ClosingStatement,  
		EndOfProgram,

		OpenBrace,
		ClosingBrace,
		FunctionCall
	};

	// A single token.
	struct Token
	{
		Token(TokenType type, std::string value, unsigned line_number)
			: type{ type }, value{ value }, line_number{ line_number }{}
		TokenType type;
		std::string value;
		unsigned line_number;
	};

	// Read characters with the cursor until the validator returns false.
	std::string read_token_string(std::string& input_code, std::string::iterator& cursor, std::function<int(int)> validator)
	{
		std::string token;
		for (; cursor != input_code.end() && validator(static_cast<unsigned char>(*cursor)); ++cursor)
		{
			token += *cursor;
		}
		return token;
	}

	// Read characters without changing the cursor's position.
	std::string peek_next_token_string(std::string& input_code, std::string::iterator& cursor, std::function<int(int)> validator)
	{
		std::string::iterator cursor_copy = cursor;
		return read_token_string(input_code, cursor_copy, validator);
	}

	// Parse the input code into a list of tokens.
	std::vector<Token> split_code_into_tokens(std::string input_code)
	{
		// Check if the input is empty.
		if (input_code.size() == 0)
		{
			return {};
		}

		// Append newline or semicolon to the end of the code if missing.
		char last_char_in_code = input_code[input_code.length() - 1];
		if (last_char_in_code != ';' && last_char_in_code != '\n')
		{
			input_code += ';';
		}

		// Convert `input_code` into lowercase.
		std::transform(input_code.begin(), input_code.end(), input_code.begin(), INT_FUNC(std::tolower));

		std::vector<Token> tokens;
		unsigned line_number = 1;
		auto cursor = input_code.begin();

		
		while (cursor != input_code.end())
		{
			char current_char = *cursor;

			// Increase line-counter on newline.
			if (current_char == '\n') ++line_number;

			// Newline and semicolon (End of statement).
			if (current_char == '\n' || current_char == ';')
			{
				if(tokens.back().type != TokenType::ClosingStatement)
					tokens.push_back(Token{ TokenType::ClosingStatement, ";", line_number });
				++cursor;
			}
			// Colons (that introduce if-statements).
			else if (current_char == ':')
			{
				tokens.push_back(Token{ TokenType::Colon, ":", line_number });
				++cursor;
			}
			// Opening brace '('.
			else if (current_char == '(')
			{
				tokens.push_back(Token{ TokenType::OpenBrace, "(", line_number });
				++cursor;
			}
			// Closing brace ')'.
			else if (current_char == ')')
			{
				tokens.push_back(Token{ TokenType::ClosingBrace, ")", line_number });
				++cursor;
			}
			// Tab and whitespace.
			else if (std::isblank(static_cast<unsigned char>(current_char)))
			{
				++cursor;
			}
			// Operators.
			else if (std::ispunct(static_cast<unsigned char>(current_char)))
			{
				std::string tok = read_token_string(input_code, cursor, INT_FUNC(std::ispunct));
				tokens.push_back(Token{ TokenType::Operator, tok, line_number });
			}
			// Numerical.
			else if (std::isdigit(static_cast<unsigned char>(current_char)))
			{
				std::string tok = read_token_string(input_code, cursor, INT_FUNC(std::isdigit));
				tokens.push_back(Token{ TokenType::Numerical, tok, line_number });
			}
			// Letters.
			else if (std::isalpha(static_cast<unsigned char>(current_char)))
			{
				std::string tok = read_token_string(input_code, cursor, INT_FUNC(std::isalpha));
				std::string next_tok = peek_next_token_string(input_code, cursor, [](int e)-> int { return (char)e == '('; });
				if (tok == "var") tokens.push_back(Token{ TokenType::Var, tok, line_number });
				else if (tok == "if") tokens.push_back(Token{ TokenType::If, tok, line_number });
				else if (tok == "endif") tokens.push_back(Token{ TokenType::Endif, tok, line_number });
				else if (tok == "for") tokens.push_back(Token{ TokenType::For, tok, line_number });
				else if (tok == "to") tokens.push_back(Token{ TokenType::To, tok, line_number });
				else if (tok == "step") tokens.push_back(Token{ TokenType::Step, tok, line_number });
				else if (tok == "endfor") tokens.push_back(Token{ TokenType::Endfor, tok, line_number });
				else if (tok == "raw") tokens.push_back(Token{ TokenType::Raw, tok, line_number });
				else if (next_tok[0] == '(') tokens.push_back(Token{ TokenType::FunctionCall, tok, line_number });
				else tokens.push_back(Token{ TokenType::Identifier, tok, line_number });
			}
			else
			{
				// On error, log the line number and the character and return an empty list. 
				compiler_log::write_error("Unexpected character " + std::string{ current_char } +" on line " + std::to_string(line_number));
				return {};
			}
		}

		tokens.push_back(Token{ TokenType::EndOfProgram, "end", line_number });
		return tokens;
	}
}

#undef INT_FUNC
