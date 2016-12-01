////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     OutputNode.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "OutputNode.h"

namespace emll
{
namespace model
{
    OutputNodeBase::OutputNodeBase(InputPortBase& input, OutputPortBase& output)
        : Node({ &input }, { &output }), _inputBase(input), _outputBase(output)
    {
    }
}
}
