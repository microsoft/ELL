// SupervisedExampleBuilder.h

#pragma once

#include "SupervisedExample.h"
#include "SparseEntryParser.h"

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
        VectorEntryParserType _instanceParser;
        bool _hasWeight;
        char _delim;
    };
}

#include "../tcc/SupervisedExampleBuilder.tcc"
