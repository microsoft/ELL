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

namespace dataset
{
    template<typename VectorEntryParserType, typename DefaultDataVectorType>
    SupervisedExampleBuilder<VectorEntryParserType, DefaultDataVectorType>::SupervisedExampleBuilder(VectorEntryParserType parser, bool hasWeight) : _instanceParser(parser), _hasWeight(hasWeight)
    {}

    template<typename VectorEntryParserType, typename DefaultDataVectorType>
    SupervisedExample<> SupervisedExampleBuilder<VectorEntryParserType, DefaultDataVectorType>::Build(std::shared_ptr<const std::string> spExampleString)
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
        return SupervisedExample<>(std::move(instance), label, weight);
    }

    template<typename VectorEntryParserType, typename DefaultDataVectorType>
    void dataset::SupervisedExampleBuilder<VectorEntryParserType, DefaultDataVectorType>::HandleErrors(utilities::ParseResult result, const std::string & str)
    {
        if (result == utilities::ParseResult::badFormat)
        {
            throw std::runtime_error("bad format real value in '" + str + "'");
        }
        else if (result == utilities::ParseResult::endOfString || result == utilities::ParseResult::beginComment)
        {
            throw std::runtime_error("premature end-of-std::string or comment in '" + str + "'");
        }
        else if (result == utilities::ParseResult::outOfRange)
        {
            throw std::runtime_error("real value out of double precision range in '" + str + "'");
        }
    }
}
