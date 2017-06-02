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
    void data::WeightLabel::Print(std::ostream& os) const
    {
        os << "(" << weight << ", " << label << ")";
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
        if (result == utilities::ParseResult::success)
        {
            return;
        }

        if (result == utilities::ParseResult::endOfString || result == utilities::ParseResult::beginComment)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::badStringFormat, "premature end-of-std::string or comment in '" + str + "'");
        }
        else if (result == utilities::ParseResult::outOfRange)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::badStringFormat, "real value out of double precision range in '" + str + "'");
        }

        throw utilities::InputException(utilities::InputExceptionErrors::badStringFormat, "bad format in '" + str + "'");
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
