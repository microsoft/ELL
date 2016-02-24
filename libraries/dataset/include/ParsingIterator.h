////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     ParsingIterator.h (dataset)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SupervisedExample.h"

// utilities
#include "IIterator.h"

// stl
#include <vector>
#include <memory>

namespace dataset
{
    using IParsingIterator = utilities::IIterator<SupervisedExample>;

    /// <summary> An input iterator that traverses a textual dataset row by row and applies a parser to
    /// each row into a dense feature std::vector of doubles, a double label, and a double weight. </summary>
    ///
    /// <typeparam name="RowIteratorType"> Type of the row iterator type. </typeparam>
    /// <typeparam name="VectorEntryParserType"> Type of the vector entry parser type. </typeparam>
    template<typename RowIteratorType, typename VectorEntryParserType>
    class ParsingIterator : public IParsingIterator
    {
    public:

        /// <summary> Constructs a parsing iterator \param row_iter An input iterator of strings, that
        /// traverses the textual dataset row by row. </summary>
        ///
        /// <param name="row_iter"> [in,out] The row iterator. </param>
        /// <param name="parser"> The parser. </param>
        ParsingIterator(RowIteratorType&& row_iter, const VectorEntryParserType& parser);

        /// <summary> \returns True if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> true if it succeeds, false if it fails. </returns>
        virtual bool IsValid() const;

        /// <summary> Proceeds to the Next row. </summary>
        virtual void Next();

        /// <summary> \returns The weight of the current example. </summary>
        ///
        /// <returns> A SupervisedExample. </returns>
        virtual SupervisedExample Get();

    private:
        RowIteratorType _rowIterator;
        VectorEntryParserType _instanceParser;
    };

    /// <summary> \returns A std::unique_ptr to a Parsing Iterator. </summary>
    ///
    /// <typeparam name="RowIteratorType"> Type of the row iterator type. </typeparam>
    /// <typeparam name="VectorEntryParserType"> Type of the vector entry parser type. </typeparam>
    /// <param name="row_iter"> The row iterator. </param>
    /// <param name="parser">   The parser. </param>
    ///
    /// <returns> The parsing iterator. </returns>
    template<typename RowIteratorType, typename VectorEntryParserType>
    std::unique_ptr<IParsingIterator> GetParsingIterator(const RowIteratorType& row_iter, const VectorEntryParserType& parser);
}

#include "../tcc/ParsingIterator.tcc"
