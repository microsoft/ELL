// ParsingIterator.h

#pragma once

#include "SupervisedExample.h"

#include <vector>
using std::vector;

namespace dataset
{
    /// An input iterator that traverses a textual dataset row by row and applies a parser to each row into a dense feature vector of doubles, 
    /// a double label, and a double weight.
    template<typename RowIteratorType, typename VectorEntryParserType>
    class ParsingIterator 
    {
    public:

        /// Constructs a parsing iterator
        /// \param row_iter An input iterator of strings, that traverses the textual dataset row by row
        ParsingIterator(RowIteratorType row_iter, VectorEntryParserType parser);

        /// \returns True if the iterator is currently pointing to a valid iterate
        ///
        bool IsValid() const;

        /// Proceeds to the Next row
        ///
        void Next();

        /// \returns The weight of the current example
        ///
        SupervisedExample Get();

    private:
        RowIteratorType _rowIterator;
        VectorEntryParserType _instanceParser;
    };
}

#include "../tcc/ParsingIterator.tcc"
