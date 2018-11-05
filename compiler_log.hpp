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

#include <vector>
#include <string>

namespace c8s
{
	class compiler_log
	{
		static std::vector<std::string> m_messages, m_warnings, m_errors;

	public:
		static void reset_all()
		{
			m_messages.clear();
			m_warnings.clear();
			m_errors.clear();
		}

		static const std::vector<std::string>& read_messages() { return m_messages; }
		static const std::vector<std::string>& read_warnings() { return m_warnings; }
		static const std::vector<std::string>& read_errors() { return m_errors; }

		static void write_message(std::string msg) { m_messages.push_back(msg); }
		static void write_warning(std::string warning) { m_warnings.push_back(warning); }
		static void write_error(std::string error) { m_errors.push_back(error); }
	};

	std::vector<std::string> compiler_log::m_messages{};
	std::vector<std::string> compiler_log::m_warnings{};
	std::vector<std::string> compiler_log::m_errors{};
}