////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ParsingIterator.h (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Example.h"

// utilities
#include "IIterator.h"

// stl
#include <memory>
#include <vector>

namespace dataset
{
    using IParsingIterator = typename utilities::IIterator<GenericSupervisedExample>;

    /// <summary> An input iterator that traverses a textual dataset row by row and applies a parser to
    /// each row into a dense feature std::vector of doubles, a double label, and a double weight. </summary>
    ///
    /// <typeparam name="RowIteratorType"> Type of the row iterator type. </typeparam>
    /// <typeparam name="VectorEntryParserType"> Type of the vector entry parser type. </typeparam>
    template <typename RowIteratorType, typename VectorEntryParserType>
    class ParsingIterator : public IParsingIterator
    {
    public:
        /// <summary> Constructs a parsing iterator \param row_iter An input iterator of strings, that
        /// traverses the textual dataset row by row. </summary>
        ///
        /// <param name="row_iter"> [in,out] The row iterator. </param>
        /// <param name="parser"> The parser. </param>
        ParsingIterator(RowIteratorType row_iter, VectorEntryParserType parser);

        /// <summary> Returns true if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> true if it succeeds, false if it fails. </returns>
        virtual bool IsValid() const override { return _rowIterator.IsValid(); }

        /// <summary> Proceeds to the Next row. </summary>
        virtual void Next() override { _rowIterator.Next(); }

        /// <summary> Get the current example. </summary>
        ///
        /// <returns> A SupervisedExample. </returns>
        virtual GenericSupervisedExample Get() const override;

    private:
        RowIteratorType _rowIterator;
        VectorEntryParserType _instanceParser;
    };

    /// <summary> Returns a std::unique_ptr to a Parsing Iterator. </summary>
    ///
    /// <typeparam name="RowIteratorType"> Type of the row iterator type. </typeparam>
    /// <typeparam name="VectorEntryParserType"> Type of the vector entry parser type. </typeparam>
    /// <param name="row_iter"> The row iterator. </param>
    /// <param name="parser">   The parser. </param>
    ///
    /// <returns> The parsing iterator. </returns>
    template <typename RowIteratorType, typename VectorEntryParserType>
    std::unique_ptr<IParsingIterator> GetParsingIterator(RowIteratorType row_iter, VectorEntryParserType parser);
}

#include "../tcc/ParsingIterator.tcc"
