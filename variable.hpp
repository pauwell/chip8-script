#pragma once
#include <iostream>
#include <string>
#include <map>
#include "types.hpp"

namespace ch8scr
{
	// Basic struct for a variable. Consists of a unique name, its position 
	// in the V-register and a byte-sized value.
	struct Variable
	{
		Variable() : name{ "" }, v_index{ 0 }, value{ 0 } {};
		Variable(std::string name, unsigned v_index, u8 value)
			: name{ name }, v_index{ v_index }, value{ value } {}
		std::string name;
		unsigned v_index;
		u8 value;
	};

	// Create a map to store all active variables.
	typedef std::map<std::string, Variable> VariableMap;
	typedef std::pair<std::string, Variable> VariableMapItem;
	VariableMap variables;
	unsigned v_index_counter = 0;

	bool create_variable(std::string name)
	{
		if (variables.find(name) != variables.end()) {
			std::cerr << "Variable with name " << name << " already exists!\n";
			return false;
		}
		variables.insert(VariableMapItem{ name, Variable{ name, v_index_counter++, 0 } });
		return true;
	}

	u16 variable_set_value(std::string name, u8 value)
	{
		auto var = variables.find(name);
		if (var == variables.end()) {
			std::cerr << "Couldnt find variable with name " << name << "!\n";
			return 0x0;
		}
		
		var->second.value = value;
		u16 op = ((0x6 << 12) | (var->second.v_index << 8) | ((int)value & 0xFF));
		return op;
	}
}
