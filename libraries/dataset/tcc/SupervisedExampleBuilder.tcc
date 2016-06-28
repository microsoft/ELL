////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SupervisedExampleBuilder.tcc (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataVectorBuilder.h"

// stl
#include <cstdlib>

// utilities
#include "Exception.h"

namespace dataset
{
    template<typename VectorEntryParserType, typename DefaultDataVectorType>
    SupervisedExampleBuilder<VectorEntryParserType, DefaultDataVectorType>::SupervisedExampleBuilder(VectorEntryParserType parser, bool hasWeight) : _instanceParser(parser), _hasWeight(hasWeight)
    {}

    template<typename VectorEntryParserType, typename DefaultDataVectorType>
    GenericSupervisedExample SupervisedExampleBuilder<VectorEntryParserType, DefaultDataVectorType>::Build(std::shared_ptr<const std::string> spExampleString)
    {
        const char* pStr = spExampleString->c_str(); 

        double label = 0.0;
        double weight = 1.0;

        // parse weight
        if(_hasWeight)
        {
            auto result = utilities::Parse(pStr, weight);

            if(result != utilities::ParseResult::success)
            {
                HandleErrors(result, *spExampleString);
            }
        }

        // parse label
        auto result = utilities::Parse(pStr, label);

        if(result != utilities::ParseResult::success)
        {
            HandleErrors(result, *spExampleString);
        }

        // create index-value iterator from the rest of the std::string
        auto IndexValueIterator = _instanceParser.GetIterator(spExampleString, pStr);

        // create instance
        auto instance = DataVectorBuilder<DefaultDataVectorType>::Build(IndexValueIterator);

        // return supervised example
        return GenericSupervisedExample(std::move(instance), label, weight);
    }

    template<typename VectorEntryParserType, typename DefaultDataVectorType>
    void dataset::SupervisedExampleBuilder<VectorEntryParserType, DefaultDataVectorType>::HandleErrors(utilities::ParseResult result, const std::string& str)
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
