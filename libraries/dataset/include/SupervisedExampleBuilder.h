// SupervisedExampleBuilder.h

#pragma once

#include "SupervisedExample.h"
#include "SparseEntryParser.h"

// utilities
#include "Parser.h"
using utilities::Parser;

// stl
#include<string>
using std::string;

#include <memory>
using std::shared_ptr;

namespace dataset
{
    template<typename VectorEntryParserType, typename DefaultDataVectorType> 
    class SupervisedExampleBuilder
    {
    public:
        /// Ctor
        ///
        SupervisedExampleBuilder(VectorEntryParserType parser, bool hasWeight = false);

        /// Takes a string that represents a supervised example, and returns a SupervisedExample object
        ///
        SupervisedExample Build(shared_ptr<const string> spExampleString);

    private:
        void handleErrors(Parser::Result result, const string& str);

        VectorEntryParserType _instanceParser;
        bool _hasWeight;
    };
}

#include "../tcc/SupervisedExampleBuilder.tcc"
