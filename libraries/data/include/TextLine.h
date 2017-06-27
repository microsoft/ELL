////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TextLine.h (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include <memory>

// utilities
#include "CStringParser.h"

namespace ell
{
namespace data 
{
    /// <summary> Represents a constant line of text with a cursor that enables incremental parsing. </summary>
    class TextLine
    {
    public:
        TextLine() = default;

        /// <summary> Constructs an instance of TextLine from a string </summary>
        ///
        /// <param name="string"> The string. </param>
        TextLine(std::string string);

        /// <summary> Gets a const reference to the underlying string. </summary>
        ///
        /// <returns> The underlying string. </returns>
        const std::string& GetString() const { return *_string; }

        /// <summary> Query if this TextLine contains a valid string. </summary>
        ///
        /// <returns> True if valid, false if not. </returns>
        bool IsValid() { return _string != nullptr; }

        /// <summary> Returns a character relative to the current position of the cursor. </summary>
        ///
        /// <param name="offset"> The offset relative to the cursor. </param>
        ///
        /// <returns> The character at the specified position. </returns>
        char Peek(size_t offset = 0) const;

        /// <summary> Query if the cursor is at he end of the content. Specifically, it could be at the end of the string, or pointing to an end-of-string comment. </summary>
        ///
        /// <returns> True if end of content, false if not. </returns>
        bool IsEndOfContent() const;

        /// <summary> Advances the cursor past whitespace. </summary>
        void TrimLeadingWhitespace() { utilities::TrimLeadingWhitespace(_currentChar); }

        /// <summary> Parses a value at the current cursor position and advances the cursor to the character that appears right after this value. </summary>
        ///
        /// <typeparam name="ValueType"> Value type to parse. </typeparam>
        /// <param name="value"> The value. </param>
        template <typename ValueType>
        void ParseAdvance(ValueType& value);

        /// <summary> Tries to parse a value at the current cursor position without advancing the cursor. </summary>
        ///
        /// <typeparam name="ValueType"> The value type. </typeparam>
        /// <param name="value"> The value. </param>
        ///
        /// <returns> The number of characters occupied by the parsed value, or zero if parsing fails. </returns>
        template <typename ValueType>
        size_t TryParse(ValueType& value) const;

        /// <summary> Gets the current cursor position. </summary>
        ///
        /// <returns> The current cursor position. </returns>
        size_t GetCurrentPosition() const;

        /// <summary> Advance the cursor position. </summary>
        ///
        /// <param name="increment"> Amount to advance the cursor by, 1 by default. </param>
        void AdvancePosition(size_t increment = 1);

        /// <summary> Gets the total number of characters in the line. </summary>
        ///
        /// <returns> The text line size. </returns>
        size_t Size() const { return _string->length(); }

    private:
        std::shared_ptr<const std::string> _string = nullptr; 
        const char* _currentChar = nullptr;
    };
}
}

#include "../tcc/TextLine.tcc"