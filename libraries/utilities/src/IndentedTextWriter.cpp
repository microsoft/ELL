////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IndentedTextWriter.cpp (utilities)
//  Authors:  Umesh Madan, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IndentedTextWriter.h"

namespace ell
{
namespace utilities
{
    static const std::string tabStringDefault = "    ";

    IndentedTextWriter::IndentedTextWriter()
        : _tabString(tabStringDefault)
    {
        SetPrecision(17);
    }

    IndentedTextWriter::IndentedTextWriter(const IndentedTextWriter& other)
        : _tabString(other._tabString), _indentLevel(other._indentLevel)
    {
        SetPrecision(other.GetPrecision());
    }

    IndentedTextWriter& IndentedTextWriter::WriteNewLine()
    {
        _buffer << std::endl;
        _needsIndent = true;
        return *this;
    }

    IndentedTextWriter& IndentedTextWriter::Write(const std::string& value)
    {
        WriteIndent();
        _buffer << value;
        return *this;
    }

    template <>
    IndentedTextWriter& IndentedTextWriter::Write<const char*>(const char* pValue)
    {
        WriteIndent();
        _buffer << pValue;
        return *this;
    }

    template <>
    IndentedTextWriter& IndentedTextWriter::Write<uint8_t>(uint8_t value)
    {
        WriteIndent();
        _buffer << value;
        return *this;
    }

    template <>
    IndentedTextWriter& IndentedTextWriter::Write<double>(double value)
    {
        WriteIndent();
        _buffer << value;
        return *this;
    }

    template <>
    IndentedTextWriter& IndentedTextWriter::Write<int>(int value)
    {
        WriteIndent();
        _buffer << value;
        return *this;
    }

    template <>
    IndentedTextWriter& IndentedTextWriter::Write<int64_t>(int64_t value)
    {
        WriteIndent();
        _buffer << value;
        return *this;
    }

    template <>
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
        ++_indentLevel;
        return *this;
    }

    IndentedTextWriter& IndentedTextWriter::DecreaseIndent()
    {
        if (_indentLevel > 0)
        {
            _indentLevel--;
        }
        return *this;
    }

    void IndentedTextWriter::SetPrecision(size_t precision)
    {
        _buffer.precision(precision);
    }

    void IndentedTextWriter::WriteIndent()
    {
        if (_needsIndent)
        {
            WriteIndent(_indentLevel);
            _needsIndent = false;
        }
    }

    void IndentedTextWriter::WriteIndent(size_t count)
    {
        for (size_t i = 0; i < count; ++i)
        {
            _buffer << _tabString;
        }
    }
}
}