#pragma once

#include "meta-gen.hpp"
#include "opcode-gen.hpp"
#include "compiler_log.hpp"

namespace c8s
{
	// Compiles chip-8 script into chip-8 machinecode.
	std::vector<u16> compile(std::string c8s_input_code, bool print_errors=false)
	{
		// Reset the log.
		compiler_log::reset_all();

		// Parse.
		auto tokens = c8s::split_code_into_tokens(c8s_input_code);
		auto ast = c8s::parse_tokens_to_ast(tokens);
		
		// Generate.
		auto meta = c8s::generate_meta_opcodes(ast);
		auto ops = c8s::create_opcodes_from_meta(meta);

		// Evaluate the log.
		if(print_errors && compiler_log::read_errors().size() != 0)
		{
			for (const auto& err_line : compiler_log::read_errors())
				std::cerr << err_line << '\n';
		}

		return ops;
	}
}