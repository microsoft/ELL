////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IndentedTextWriter.cpp (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "IndentedTextWriter.h"

namespace emll
{
	namespace compiler
	{
		static const std::string c_TabStringDefault = "    ";

		IndentedTextWriter::IndentedTextWriter()
			: _tabString(c_TabStringDefault)
		{
			_buffer.precision(17);
		}

		IndentedTextWriter::IndentedTextWriter(const IndentedTextWriter& src)
			: _tabString(src._tabString),
			  _indentCount(src._indentCount)
		{
			_buffer.precision(_buffer.precision());
		}

		IndentedTextWriter& IndentedTextWriter::WriteNewLine()
		{
			_buffer << std::endl;
			_needsIndent = true;
			return *this;
		}

		IndentedTextWriter& IndentedTextWriter::Write(const char* pValue)
		{
			WriteIndent();
			_buffer << pValue;
			return *this;
		}

		IndentedTextWriter& IndentedTextWriter::Write(const std::string& value)
		{
			WriteIndent();
			_buffer << value;
			return *this;
		}

		template<>
		IndentedTextWriter& IndentedTextWriter::Write<uint8_t>(uint8_t value)
		{
			WriteIndent();
			_buffer << value;
			return *this;
		}

		template<>
		IndentedTextWriter& IndentedTextWriter::Write<double>(double value)
		{
			WriteIndent();
			_buffer << value;
			return *this;
		}

		template<>
		IndentedTextWriter& IndentedTextWriter::Write<int>(int value)
		{
			WriteIndent();
			_buffer << value;
			return *this;
		}

		template<>
		IndentedTextWriter& IndentedTextWriter::Write<int64_t>(int64_t value)
		{
			WriteIndent();
			_buffer << value;
			return *this;
		}

		template<>
		IndentedTextWriter& IndentedTextWriter::Write<char>(char value)
		{
			WriteIndent();
			_buffer << value;
			return *this;
		}

		IndentedTextWriter& IndentedTextWriter::WriteRaw(const std::string& value, int indentCount)
		{
			if (indentCount > 0)
			{
				WriteIndent(indentCount);
			}
			_buffer << value;
			return *this;
		}

		IndentedTextWriter& IndentedTextWriter::IncreaseIndent()
		{
			++_indentCount;
			return *this;
		}

		IndentedTextWriter& IndentedTextWriter::DecreaseIndent()
		{
			if (_indentCount > 0)
			{
				_indentCount--;
			}
			return *this;
		}

		void IndentedTextWriter::WriteIndent()
		{
			if (_needsIndent)
			{
				WriteIndent(_indentCount);
				_needsIndent = false;
			}
		}

		void IndentedTextWriter::WriteIndent(int count)
		{
			for (uint32_t i = 0; i < count; ++i)
			{
				_buffer << _tabString;
			}
		}
	}
}