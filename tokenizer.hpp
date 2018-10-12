#pragma once
#include <cctype>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>

#define INT_FUNC(f) static_cast<int(*)(int)>(f) 

namespace ch8scr
{
	// Different types of tokens.
	enum class TokenType 
	{ 
		Var, 
		Identifier, 
		Operator, 
		Numerical, 
		ClosingStatement, 
		Error, 
		EndOfProgram 
	};

	// A single token.
	struct Token
	{
		Token(TokenType type, std::string value) 
			: type{ type }, value{ value } {}
		TokenType type;
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
		std::transform(input_code.begin(), input_code.end(), input_code.begin(), INT_FUNC(std::tolower));

		std::vector<Token> tokens;
		auto cursor = input_code.begin();

		while (cursor != input_code.end())
		{
			char current_char = *cursor;

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
			else if (std::ispunct(current_char))
			{
				std::string tok = read_token_string(input_code, cursor, INT_FUNC(std::ispunct));
				tokens.push_back(Token{ TokenType::Operator, tok });
			}
			// Numerical.
			else if (std::isdigit(current_char))
			{
				std::string tok = read_token_string(input_code, cursor, INT_FUNC(std::isdigit));
				tokens.push_back(Token{ TokenType::Numerical, tok });
			}
			// Letters.
			else if (std::isalpha(current_char))
			{
				std::string tok = read_token_string(input_code, cursor, INT_FUNC(std::isalpha));
				if (tok == "var") tokens.push_back(Token{ TokenType::Var, tok });
				else tokens.push_back(Token{ TokenType::Identifier, tok });
			}
			else
			{
				// On error, push an error-token onto the token-list and return. 
				tokens.push_back(Token{ TokenType::Error, "Unexpected character: " + std::string{ current_char} });
				return tokens;
			}
				
		}

		tokens.push_back(Token{ TokenType::EndOfProgram, "end" });
		return tokens;
	}
}

#undef INT_FUNC