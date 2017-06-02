////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SequentialLineIterator.h (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TextLine.h"

// stl
#include <fstream>
#include <memory>
#include <string>

namespace ell
{
namespace data
{
    /// <summary> An iterator that reads a long text line by line. </summary>
    class SequentialLineIterator
    {
    public:
        /// <summary> Constructs a sequential line iterator. </summary>
        ///
        /// <param name="stream"> The input stream. </param>
        /// <param name="delim"> The delimiter. </param>
        SequentialLineIterator(std::istream& stream, char delim = '\n');

        SequentialLineIterator(SequentialLineIterator&&) = default;

        SequentialLineIterator(const SequentialLineIterator&) = delete; // this ctor is deleted because a private member of this class cannot be copied

        /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> true if it succeeds, false if it fails. </returns>
        bool IsValid() const { return _isValid; }

        /// <summary> Proceeds to the next row. </summary>
        void Next();

        /// <summary> Returns a TextLine that contains the current line. </summary>
        ///
        /// <returns> A TextLine </returns>
        TextLine GetTextLine() const { return _currentLine; } 

    private:
        std::istream& _stream;
        bool _isValid = true;
        TextLine _currentLine;
        char _delim;
    };
}
}
