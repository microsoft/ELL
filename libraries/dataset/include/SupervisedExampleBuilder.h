// SupervisedExampleBuilder.h

#pragma once

#include "SupervisedExample.h"
#include "SparseEntryParser.h"

// utilities
#include "Parser.h"

// stl
#include<string>
#include <memory>

namespace dataset
{
    template<typename VectorEntryParserType, typename DefaultDataVectorType> 
    class SupervisedExampleBuilder
    {
    public:
        /// Ctor
        ///
        SupervisedExampleBuilder(VectorEntryParserType parser, bool hasWeight = false);

        /// Takes a std::string that represents a supervised example, and returns a SupervisedExample object
        ///
        SupervisedExample Build(std::shared_ptr<const std::string> spExampleString);

    private:
        void handleErrors(utilities::Parser::Result result, const std::string& str);

        VectorEntryParserType _instanceParser;
        bool _hasWeight;
    };
}

#include "../tcc/SupervisedExampleBuilder.tcc"
