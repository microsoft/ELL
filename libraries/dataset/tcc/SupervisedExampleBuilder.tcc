////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     SupervisedExampleBuilder.tcc (dataset)
//  Authors:  Ofer Dekel
//
//  [copyright]
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
    SupervisedExample SupervisedExampleBuilder<VectorEntryParserType, DefaultDataVectorType>::Build(std::shared_ptr<const std::string> spExampleString)
    {
        const char* pStr = spExampleString->c_str(); 

        double label = 0.0;
        double weight = 1.0;

        // parse weight
        if(_hasWeight)
        {
            auto result = utilities::Parser::Parse(pStr, /* out */ weight);

            if(result != utilities::Parser::Result::success)
            {
                handleErrors(result, *spExampleString);
            }
        }

        // parse label
        auto result = utilities::Parser::Parse(pStr, /* out */ label);

        if(result != utilities::Parser::Result::success)
        {
            handleErrors(result, *spExampleString);
        }

        // create index-value iterator from the rest of the std::string
        auto IndexValueIterator = _instanceParser.GetIterator(spExampleString, pStr);

        // create instance
        auto upInstance = DataVectorBuilder<DefaultDataVectorType>::Build(IndexValueIterator);

        // return supervised example
        return SupervisedExample(move(upInstance), label, weight);
    }

    template<typename VectorEntryParserType, typename DefaultDataVectorType>
    void dataset::SupervisedExampleBuilder<VectorEntryParserType, DefaultDataVectorType>::handleErrors(utilities::Parser::Result result, const std::string & str)
    {
        if (result == utilities::Parser::Result::badFormat)
        {
            throw std::runtime_error("bad format real value in '" + str + "'");
        }
        else if (result == utilities::Parser::Result::endOfString || result == utilities::Parser::Result::beginComment)
        {
            throw std::runtime_error("premature end-of-std::string or comment in '" + str + "'");
        }
        else if (result == utilities::Parser::Result::outOfRange)
        {
            throw std::runtime_error("real value out of double precision range in '" + str + "'");
        }
    }
}
