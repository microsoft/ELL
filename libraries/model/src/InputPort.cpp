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
    InputPortBase::InputPortBase(Port::PortType portType)
        : Port(nullptr, "", portType)
    {
    }

    InputPortBase::InputPortBase(const Node* owningNode, PortElementsBase& input, const std::string& name)
        : Port(owningNode, name, input.GetPortType()), _referencedPort(input.GetRanges()[0].ReferencedPort())
    {
        if (!input.IsFullPortOutput())
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Input port elements must be full port outputs.");
        }
    }

    std::vector<const Node*> InputPortBase::GetParentNodes() const 
    { 
        return _referencedPort == nullptr ? std::vector<const Node*>() : std::vector<const Node*>{ _referencedPort->GetNode() }; 
    }

    PortElementsBase InputPortBase::GetInputElements() const
    {
        return _referencedPort == nullptr ? PortElementsBase() : PortElementsBase(*_referencedPort);
    }

    PortMemoryLayout InputPortBase::GetMemoryLayout() const
    {
        return _referencedPort->GetMemoryLayout();
    }

    const OutputPortBase& InputPortBase::GetReferencedPort() const
    {
        if (!IsValid())
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Error: empty input port.");
        }

        return *_referencedPort;
    }

    size_t InputPortBase::Size() const
    { 
        if (!IsValid())
        {
            return 0;
        }
        return GetInputElements().Size(); 
    }

    bool InputPortBase::IsValid() const
    {
        return _referencedPort != nullptr;
    }

    void InputPortBase::SetInput(const OutputPortBase* input)
    {
        _referencedPort = input;
    }
}
}
