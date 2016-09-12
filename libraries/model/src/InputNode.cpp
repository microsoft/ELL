////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     InputNode.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "InputNode.h"

// utilities
#include "IArchivable.h"

namespace emll
{
namespace model
{
    InputNodeBase::InputNodeBase(OutputPortBase& output)
        : Node({}, {&output}), _outputBase(output)
    {
    }
}
}
