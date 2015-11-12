// ParsingIterator.h

#pragma once

#include "SupervisedExample.h"

#include <iterator>
using std::iterator;

#include <vector>
using std::vector;

namespace dataset
{
    /// An input iterator that traverses a textual dataset Row by Row and applies a parser to each Row into a dense feature vector of doubles, 
    /// a double label, and a double weight.
    template<typename RowIteratorType, typename ParserType>
    class ParsingIterator : public iterator<std::input_iterator_tag, vector<double>>
    {
    public:

        /// Constructs a parsing iterator
        /// \param row_iter An input iterator of strings, that traverses the textual dataset Row by Row
        ParsingIterator(RowIteratorType& row_iter, ParserType& parser);

        /// \returns True if the iterator is currently pointing to a valid iterate
        ///
        bool IsValid() const;

        /// Proceeds to the Next Row
        ///
        void Next();

        /// \returns The weight of the current example
        ///
        SupervisedExample GetValue();

    private:
        RowIteratorType& _row_iter;
        ParserType& _parser;
    };
}

#include "../tcc/ParsingIterator.tcc"
