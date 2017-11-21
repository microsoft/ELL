////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     WeightClassIndex.cpp (data)
//  Authors:  Ofer Dekel, Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WeightClassIndex.h"

// utilities
#include  "Exception.h"

// stl
#include <iostream>

namespace ell
{
namespace data
{
    std::ostream& operator<<(std::ostream& os, const WeightClassIndex& weightClassIndex)
    {
        os << "(" << weightClassIndex.weight << ", " << weightClassIndex.classIndex << ")";
        return os;
    }

    WeightClassIndex ClassIndexParser::Parse(TextLine& textLine)
    {
        size_t classIndex = 0;

        textLine.TrimLeadingWhitespace();
        textLine.ParseAdvance(classIndex);
        return WeightClassIndex{ 1.0, classIndex };
    }

    void ClassIndexParser::HandleErrors(utilities::ParseResult result, const std::string& str)
    {
        switch (result)
        {
        case utilities::ParseResult::success:
            return;
        case utilities::ParseResult::endOfString:
        case utilities::ParseResult::beginComment:
            throw utilities::InputException(utilities::InputExceptionErrors::badStringFormat, "premature end-of-std::string or comment in '" + str + "'");
        case utilities::ParseResult::outOfRange:
            throw utilities::InputException(utilities::InputExceptionErrors::badStringFormat, "value out of size_t precision range in '" + str + "'");
        default:
            throw utilities::InputException(utilities::InputExceptionErrors::badStringFormat, "value out of size_t precision range in '" + str + "'");
        }
    }

    WeightClassIndex WeightClassIndexParser::Parse(TextLine& textLine)
    {
        size_t classIndex = 0;
        double weight = 1.0;

        textLine.TrimLeadingWhitespace();
        textLine.ParseAdvance(weight);

        textLine.TrimLeadingWhitespace();
        textLine.ParseAdvance(classIndex);

        return WeightClassIndex{ weight, classIndex };
    }
}
}
