////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     InputPort.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "InputPort.h"

namespace ell
{
namespace model
{
    //
    // InputPortBase
    //
    InputPortBase::InputPortBase(const class Node* owningNode, const PortElementsBase& inputs, const std::string& name)
        : Port(owningNode, name, inputs.GetPortType()), _inputElements(inputs)
    {
    }

    void InputPortBase::ComputeParents()
    {
        _parentNodes.clear();
        for (const auto& range : _inputElements.GetRanges())
        {
            auto port = range.ReferencedPort();
            auto node = port->GetNode();
            _parentNodes.push_back(node);
        }
    }
}
}
