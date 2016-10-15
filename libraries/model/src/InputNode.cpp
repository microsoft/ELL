////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     InputNode.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "InputNode.h"

namespace emll
{
namespace model
{
    InputNodeBase::InputNodeBase(OutputPortBase& output)
        : Node({}, {&output}), _outputBase(output)
    {
    }

    void InputNodeBase::SetSize(size_t size)
    {
        _outputBase.SetSize(size);
    }

}
}
