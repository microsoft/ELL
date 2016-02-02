// ParsingIterator.h

#pragma once

#include "SupervisedExample.h"

// utilities
#include "IIterator.h"

/// stl
#include <vector>
#include <memory>

namespace dataset
{
    using IParsingIterator = utilities::IIterator<SupervisedExample>;
    
    /// An input iterator that traverses a textual dataset row by row and applies a parser to each row into a dense feature std::vector of doubles, 
    /// a double label, and a double weight.
    template<typename RowIteratorType, typename VectorEntryParserType>
    class ParsingIterator : public IParsingIterator
    {
    public:
        /// Constructs a parsing iterator
        /// \param row_iter An input iterator of strings, that traverses the textual dataset row by row
        ParsingIterator(RowIteratorType&& row_iter, const VectorEntryParserType& parser);

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

    /// \returns A std::unique_ptr to a Parsing Iterator
    ///
    template<typename RowIteratorType, typename VectorEntryParserType>
    std::unique_ptr<IParsingIterator> GetParsingIterator(const RowIteratorType& row_iter, const VectorEntryParserType& parser);
}

#include "../tcc/ParsingIterator.tcc"
