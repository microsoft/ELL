////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IndentedTextWriter.h (utilities)
//  Authors:  Umesh Madan, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <sstream>
#include <string>

namespace ell
{
namespace utilities
{
    ///<summary>A class to emit indented (tabbed) text output</summary>
    class IndentedTextWriter
    {
    public:
        /// <summary> Construct a default writer. </summary>
        IndentedTextWriter();

        /// <summary> Construct a writer with the indent settings of another writer. </summary>
        ///
        /// <param name="src"> Source for the. </param>
        IndentedTextWriter(const IndentedTextWriter& other);

        /// <summary> Gets a reference to the string used for indentation - the default is 4 spaces. </summary>
        ///
        /// <returns> The indentation string </returns>
        std::string& TabString() { return _tabString; }

        /// <summary> Gets a reference to the indent level. </summary>
        ///
        /// <returns> The indent count </returns>
        size_t& GetIndentLevel() { return _indentLevel; }

        /// <summary> Increase the indent. </summary>
        ///
        /// <returns> A reference to this writer </returns>
        IndentedTextWriter& IncreaseIndent();

        /// <summary> Decrease the indent. </summary>
        ///
        /// <returns> A reference to this writer </returns>
        IndentedTextWriter& DecreaseIndent();

        /// <summary> Write a new line. </summary>
        ///
        /// <returns> A reference to this writer </returns>
        IndentedTextWriter& WriteNewLine();

        /// <summary> Write a string. </summary>
        ///
        /// <param name="value"> The string to write. </param>
        ///
        /// <returns> A reference to this writer </returns>
        IndentedTextWriter& Write(const std::string& value);

        /// <summary> Write some value types. </summary>
        ///
        /// <typeparam name="T"> The type of the value being written. </typeparam>
        /// <param name="value"> The value to write. </param>
        ///
        /// <returns> A reference to this writer </returns>
        template <typename T>
        IndentedTextWriter& Write(T value);

        /// <summary> Writes a string with a given indent count. </summary>
        ///
        /// <param name="value"> The value to write. </param>
        /// <param name="indentCount"> The indent count. </param>
        ///
        /// <returns> A reference to this writer </returns>
        IndentedTextWriter& WriteRaw(const std::string& value, int indentCount = 0);

        /// <summary> Output the current buffer to a string. </summary>
        ///
        /// <returns> A std::string that represents the contents of this IndentedTextWriter. </returns>
        std::string ToString() const { return _buffer.str(); }

        /// <summary> Clear the buffer. </summary>
        void Clear() { _buffer.clear(); }

        /// <summary> Is the writer empty? </summary>
        ///
        /// <returns> true if empty, false if not. </returns>
        bool IsEmpty() const { return (_buffer.str().size() <= 0); }

        /// <summary> Return the precision with which doubles are serialized. Default is 17. </summary>
        ///
        /// <returns> The precision. </returns>
        size_t GetPrecision() const { return _buffer.precision(); }

        /// <summary> Set the precision with which doubles are serialized. </summary>
        ///
        /// <param name="precision"> The precision. </param>
        void SetPrecision(size_t precision);

    private:
        void WriteIndent();
        void WriteIndent(size_t count);

        std::stringstream _buffer;
        std::string _tabString;
        size_t _indentLevel = 0;
        bool _needsIndent = true;
    };
}
}
