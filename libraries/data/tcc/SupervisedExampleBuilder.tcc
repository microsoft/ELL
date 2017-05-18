////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SupervisedExampleBuilder.tcc (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// stl
#include <cstdlib>

// utilities
#include "Exception.h"

namespace ell
{
namespace data
{
    template <typename VectorElementParserType, typename DataVectorType>
    SupervisedExampleBuilder<VectorElementParserType, DataVectorType>::SupervisedExampleBuilder(VectorElementParserType parser, bool hasWeight)
        : _instanceParser(parser), _hasWeight(hasWeight)
    {
    }

    template <typename VectorElementParserType, typename DataVectorType>
    AutoSupervisedExample SupervisedExampleBuilder<VectorElementParserType, DataVectorType>::Build(std::shared_ptr<const std::string> pExampleString)
    {
        const char* pStr = pExampleString->c_str();

        double label = 0.0;
        double weight = 1.0;

        // parse weight
        if (_hasWeight)
        {
            auto result = utilities::Parse(pStr, weight);

            if (result != utilities::ParseResult::success)
            {
                HandleErrors(result, *pExampleString);
            }
        }

        // parse label
        auto result = utilities::Parse(pStr, label);

        if (result != utilities::ParseResult::success)
        {
            HandleErrors(result, *pExampleString);
        }

        // create index-value iterator from the rest of the std::string
        auto IndexValueIterator = _instanceParser.GetIterator(pExampleString, pStr);

        // create instance
        auto dataVector = std::make_shared<DataVectorType>(IndexValueIterator);

        // return supervised example
        return AutoSupervisedExample(std::move(dataVector), WeightLabel{ weight, label });
    }

    template <typename VectorElementParserType, typename DataVectorType>
    void SupervisedExampleBuilder<VectorElementParserType, DataVectorType>::HandleErrors(utilities::ParseResult result, const std::string& str)
    {
        if (result == utilities::ParseResult::badFormat)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::badStringFormat, "bad format in '" + str + "'");
        }
        else if (result == utilities::ParseResult::endOfString || result == utilities::ParseResult::beginComment)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::badStringFormat, "premature end-of-std::string or comment in '" + str + "'");
        }
        else if (result == utilities::ParseResult::outOfRange)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::badStringFormat, "real value out of double precision range in '" + str + "'");
        }
    }
}
}
