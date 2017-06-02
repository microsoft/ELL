////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     WeightLabel.h (data)
//  Authors:  Ofer Dekel
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
    /// <summary> A metadata class that contains a weight and a real valued label. </summary>
    struct WeightLabel
    {
        /// <summary> Prints the weight label pair. </summary>
        ///
        /// <param name="os"> [in,out] The output stream. </param>
        void Print(std::ostream& os) const;

        double weight;
        double label;
    };

    /// <summary> Class that parses a textline into a label </summary>
    struct LabelParser
    {
        /// <summary> Parses the given text line. </summary>
        ///
        /// <param name="textLine"> The text line. </param>
        ///
        /// <returns> A WeightLabel. </returns>
        static WeightLabel Parse(TextLine& textLine);

    protected:
        static void HandleErrors(utilities::ParseResult result, const std::string& str);
    };

    /// <summary> Class that parses a textline into a weight and a label </summary>
    struct WeightLabelParser : private LabelParser
    {
        /// <summary> Parses the given text line. </summary>
        ///
        /// <param name="textLine"> The text line. </param>
        ///
        /// <returns> A WeightLabel. </returns>
        static WeightLabel Parse(TextLine& textLine);
    };
}
}
