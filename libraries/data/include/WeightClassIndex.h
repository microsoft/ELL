////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     WeightClassIndex.h (data)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "TextLine.h"

// utilities
#include "CStringParser.h"

namespace ell
{
namespace data
{
    /// <summary> A metadata class that contains a weight and a class index. </summary>
    struct WeightClassIndex
    {
        double weight;
        size_t classIndex;
    };

    /// <summary> Adds the weight class index pair to the output stream. </summary>
    ///
    /// <param name="os"> [in, out] The output stream. </param>
    /// <param name="os"> The weightClassIndex to add to the stream. </param>
    ///
    /// <returns> The output stream. </returns>
    std::ostream& operator<<(std::ostream& os, const WeightClassIndex& weightClassIndex);

    /// <summary> Class that parses a text line into a label </summary>
    struct ClassIndexParser
    {
        // The return type of the parser so the example iterator knows how to declare an Example<DataParser::type, MetadataParser::type>
        using type = WeightClassIndex;

        /// <summary> Parses the given text line. </summary>
        ///
        /// <param name="textLine"> The text line. </param>
        ///
        /// <returns> A WeightClassIndex. </returns>
        static WeightClassIndex Parse(TextLine& textLine);

    protected:
        static void HandleErrors(utilities::ParseResult result, const std::string& str);
    };

    /// <summary> Class that parses a text line into a weight and a class index </summary>
    struct WeightClassIndexParser : private ClassIndexParser
    {
        /// <summary> Parses the given text line. </summary>
        ///
        /// <param name="textLine"> The text line. </param>
        ///
        /// <returns> A WeightClassIndex. </returns>
        static WeightClassIndex Parse(TextLine& textLine);
    };
}
}
