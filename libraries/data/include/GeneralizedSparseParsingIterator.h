////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     GeneralizedSparseParsingIterator.h (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "IndexValue.h"
#include "TextLine.h"

namespace ell
{
namespace data
{
    /// <summary>
    /// A parsing index-value iterator for the generalized sparse format for data vectors (see
    /// doc/GeneralizedSparseFormat.md).
    /// </summary>
    class GeneralizedSparseParsingIterator : public IIndexValueIterator
    {
    public:
        /// <summary> Constructs an instance of GeneralizedSparseParsingIterator for a given line of text. </summary>
        ///
        /// <param name="textLine"> A TextLine object that represents a line of text. </param>
        GeneralizedSparseParsingIterator(TextLine& textLine);

        GeneralizedSparseParsingIterator(GeneralizedSparseParsingIterator&) = default;

        GeneralizedSparseParsingIterator(GeneralizedSparseParsingIterator&&) = default;

        /// <summary> Is the iterator pointing to a valid index-value pair? </summary>
        ///
        /// <returns> True if valid, false if not. </returns>
        bool IsValid() const { return _isValid; }

        /// <summary> Proceeds to the Next iterate. </summary>
        void Next();

        /// <summary> Returns The current index-value pair. </summary>
        ///
        /// <returns> An IndexValue object. </returns>
        IndexValue Get() const { return _currentIndexValue; }

    private:
        void ReadEntry(size_t nextIndex);
        bool _isValid = true;
        TextLine& _textLine;
        IndexValue _currentIndexValue;
    };
}
}
