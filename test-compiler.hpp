#pragma once
#include "debug-output.hpp"

namespace c8s
{
	void test_compiler(std::string what, std::string code,
		bool is_print_meta = true, bool is_print_ast = true, bool is_print_tokens = true)
	{
		print_seperator(true);
		std::cout << "Test => " << what << '\n';
		print_seperator(true);

		// Tokenize.
		auto tokens = c8s::split_code_into_tokens(code);
		if (is_print_tokens) print_tokens(tokens);

		// Parse.
		auto ast = c8s::parse_tokens_to_ast(tokens);
		if (is_print_ast) print_ast(ast);

		// Generate meta.
		auto meta_ops = c8s::generate_meta_opcodes(ast);
		if (is_print_meta) print_meta(meta_ops);

		std::cout << "\n\n";
	}
}
