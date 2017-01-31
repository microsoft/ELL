////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OutputNode.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "OutputNode.h"

namespace ell
{
namespace model
{
    OutputNodeBase::OutputNodeBase(InputPortBase& input, OutputPortBase& output)
        : CompilableNode({ &input }, { &output }), _inputBase(input), _outputBase(output)
    {
    }
}
}
