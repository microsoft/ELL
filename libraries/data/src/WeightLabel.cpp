////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     WeightLabel.cpp (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WeightLabel.h"

// utilities
#include  "Exception.h"

// stl
#include <iostream>

namespace ell
{
namespace data
{
    std::ostream& operator<<(std::ostream& os, const WeightLabel& weightLabel)
    {
        os << "(" << weightLabel.weight << ", " << weightLabel.label << ")";
        return os;
    }

    WeightLabel LabelParser::Parse(TextLine& textLine)
    {
        double label = 0.0;

        textLine.TrimLeadingWhitespace();
        textLine.ParseAdvance(label);
        return WeightLabel{ 1.0, label };
    }

    void LabelParser::HandleErrors(utilities::ParseResult result, const std::string& str)
    {
        switch (result)
        {
        case utilities::ParseResult::success:
            return;
        case utilities::ParseResult::endOfString:
        case utilities::ParseResult::beginComment:
            throw utilities::InputException(utilities::InputExceptionErrors::badStringFormat, "premature end-of-std::string or comment in '" + str + "'");
        case utilities::ParseResult::outOfRange:
            throw utilities::InputException(utilities::InputExceptionErrors::badStringFormat, "value out of double precision range in '" + str + "'");
        default:
            throw utilities::InputException(utilities::InputExceptionErrors::badStringFormat, "value out of double precision range in '" + str + "'");
        }
    }

    WeightLabel WeightLabelParser::Parse(TextLine& textLine)
    {
        double label = 0.0;
        double weight = 1.0;

        textLine.TrimLeadingWhitespace();
        textLine.ParseAdvance(weight);

        textLine.TrimLeadingWhitespace();
        textLine.ParseAdvance(label);

        return WeightLabel{ weight, label };
    }
}
}
