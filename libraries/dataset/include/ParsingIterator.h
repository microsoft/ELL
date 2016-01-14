// ParsingIterator.h

#pragma once

#include "SupervisedExample.h"

/// stl
#include <vector>
using std::vector;

#include <memory>
using std::unique_ptr;

namespace dataset
{
    class IParsingIterator
    {
    public:
        /// \returns True if the iterator is currently pointing to a valid iterate
        ///
        virtual bool IsValid() const = 0;

        /// Proceeds to the Next row
        ///
        virtual void Next() = 0;

        /// \returns The weight of the current example
        ///
        virtual SupervisedExample Get() = 0;
    };

    /// An input iterator that traverses a textual dataset row by row and applies a parser to each row into a dense feature vector of doubles, 
    /// a double label, and a double weight.
    template<typename RowIteratorType, typename VectorEntryParserType>
    class ParsingIterator : public IParsingIterator
    {
    public:
        /// Constructs a parsing iterator
        /// \param row_iter An input iterator of strings, that traverses the textual dataset row by row
        ParsingIterator(RowIteratorType row_iter, VectorEntryParserType parser);

        /// \returns True if the iterator is currently pointing to a valid iterate
        ///
        virtual bool IsValid() const;

        /// Proceeds to the Next row
        ///
        virtual void Next();

        /// \returns The weight of the current example
        ///
        virtual SupervisedExample Get();

    private:
        RowIteratorType _rowIterator;
        VectorEntryParserType _instanceParser;
    };

    /// \returns A unique_ptr to a Parsing Iterator
    ///
    template<typename RowIteratorType, typename VectorEntryParserType>
    unique_ptr<IParsingIterator> GetParsingIterator(RowIteratorType row_iter, VectorEntryParserType parser);
}

#include "../tcc/ParsingIterator.tcc"
