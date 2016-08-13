////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IndentedTextWriter.h (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <sstream>

namespace emll
{
	namespace compiler
	{
		///<summary>A class to emit indented (tabbed) text output</summary>
		class IndentedTextWriter
		{
		public:
			///<summary>Construct a default writer</summary>
			IndentedTextWriter();
			///<summary>Construct a writer with the indent settings of src</summary>
			IndentedTextWriter(const IndentedTextWriter& src);

			///<summary>Configure the string used for indentation - the default is 4 spaces</summary>
			std::string& TabString() { return _tabString; }
			///<summary>Configure the indent count</summary>
			uint32_t& IndentCount() { return _indentCount; }
			///<summary>Increase the indent</summary>
			IndentedTextWriter& IncreaseIndent();
			///<summary>Decrease the indent</summary>
			IndentedTextWriter& DecreaseIndent();

			///<summary>Write a new line</summary>
			IndentedTextWriter& WriteNewLine();
			///<summary>Write a char string</summary>
			IndentedTextWriter& Write(const char* pValue);
			///<summary>Write a string</summary>
			IndentedTextWriter& Write(const std::string& value);
			///<summary>Write some value types</summary>
			template<typename T>
			IndentedTextWriter& Write(T value);

			IndentedTextWriter& WriteRaw(const std::string& value);
			///<summary>Output the current buffer into a string</summary>
			std::string ToString() { return _buffer.str(); }
			
			///<summary>Clear the buffer</summary>
			void Clear() { _buffer.clear(); }

		private:
			///<summary>Emit the tab indent</summary>
			void WriteIndent();

		private:
			std::stringstream _buffer;
			std::string _tabString;
			uint32_t _indentCount = 0;
			bool _needsIndent = true;
		};
	}
}