////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     WeightLabel.h (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "TextLine.h"

// stl
#include <ostream>

// utilities
#include "CStringParser.h"

namespace ell
{
namespace data
{
    /// <summary> A metadata class that contains a weight and a real valued label. </summary>
    struct WeightLabel
    {
        double weight;
        double label;
    };

    /// <summary> Adds the weight label pair to the output stream. </summary>
    ///
    /// <param name="os"> [in, out] The output stream. </param>
    /// <param name="os"> The weightClassIndex to add to the stream. </param>
    ///
    /// <returns> The output stream. </returns>
    std::ostream& operator<<(std::ostream& os, const WeightLabel& weightLabel);

    /// <summary> Class that parses a text line into a label </summary>
    struct LabelParser
    {
        // The return type of the parser so the example iterator knows how to declare an Example<DataParser::type, MetadataParser::type>
        using type = WeightLabel;

        /// <summary> Parses the given text line. </summary>
        ///
        /// <param name="textLine"> The text line. </param>
        ///
        /// <returns> A WeightLabel. </returns>
        static WeightLabel Parse(TextLine& textLine);

    protected:
        static void HandleErrors(utilities::ParseResult result, const std::string& str);
    };

    /// <summary> Class that parses a text line into a weight and a label </summary>
    struct WeightLabelParser : private LabelParser
    {
        /// <summary> Parses the given text line. </summary>
        ///
        /// <param name="textLine"> The text line. </param>
        ///
        /// <returns> A WeightLabel. </returns>
        static WeightLabel Parse(TextLine& textLine);
    };
} // namespace data
} // namespace ell
