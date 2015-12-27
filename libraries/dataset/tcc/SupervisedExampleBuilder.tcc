// SupervisedExampleBuilder.tcc

#include "DatavectorBuilder.h"

#include "Parser.h"
using utilities::Parser;

#include <cstdlib>
using std::strtod;

namespace dataset
{
    template<typename VectorEntryParserType, typename DefaultDataVectorType>
    SupervisedExampleBuilder<VectorEntryParserType, DefaultDataVectorType>::SupervisedExampleBuilder(VectorEntryParserType parser, bool hasWeight) : _instanceParser(parser), _hasWeight(hasWeight)
    {}

    void handleErrors(Parser::Result result, const string& str)
    {
        if(result == Parser::Result::badFormat)
        {
            throw runtime_error("bad format real value in '" + str + "'");
        }
        else if(result == Parser::Result::endOfString || result == Parser::Result::beginComment)
        {
            throw runtime_error("premature end-of-string or comment in '" + str + "'");
        }
        else if(result == Parser::Result::outOfRange)
        {
            throw runtime_error("real value out of double precision range in '" + str + "'");
        }
    }

    template<typename VectorEntryParserType, typename DefaultDataVectorType>
    SupervisedExample SupervisedExampleBuilder<VectorEntryParserType, DefaultDataVectorType>::Build(shared_ptr<const string> spExampleString)
    {
        const char* pStr = spExampleString->c_str(); 

        double label = 0.0;
        double weight = 1.0;

        // parse weight
        if(_hasWeight)
        {
            auto result = Parser::Parse(pStr, /* out */ weight);

            if(result != Parser::Result::success)
            {
                handleErrors(result, *spExampleString);
            }
        }

        // parse label
        auto result = Parser::Parse(pStr, /* out */ label);

        if(result != Parser::Result::success)
        {
            handleErrors(result, *spExampleString);
        }

        // create index-value iterator from the rest of the string
        auto indexValueIterator = _instanceParser.GetIterator(spExampleString, pStr);

        // create instance
        auto upInstance = DataVectorBuilder<DefaultDataVectorType>::Build(indexValueIterator);

        // return supervised example
        return SupervisedExample(move(upInstance), label, weight);
    }
}