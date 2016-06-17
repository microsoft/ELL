////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     InputPort.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "InputPort.h"

InputPort::InputPort(const class Node* owningNode, size_t portIndex, const InputRange& inputRange) : Port(owningNode, portIndex, inputRange.Type(), inputRange.Size())
{
}

InputPort::InputPort(const class Node* owningNode, size_t portIndex, const std::vector<InputRange>& inputRanges) : Port(owningNode, portIndex, Port::PortType::None, 0)
{
    // TODO: throw if inputRanges.size() == 0

    Port::PortType portType = inputRanges[0].Type();
    size_t size = 0;
    for (const auto& range : inputRanges)
    {
        size += range.Size();
        if (range.Type() != portType)
        {
            throw std::runtime_error("Error, all ranges not of same type");
        }
    }

    SetSize(size);
    Port::SetType(portType);
}