////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     OutputPort.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "OutputPort.h"

/// <summary> model namespace </summary>
namespace model
{
    OutputPortBase::OutputPortBase(const class Node* node, std::string name, PortType type, size_t size) : Port(node, name, type), _size(size), _isReferenced(false) 
    {}

    utilities::ObjectDescription OutputPortBase::GetTypeDescription()
    {
        utilities::ObjectDescription description = utilities::MakeObjectDescription<Port, OutputPortBase>("OutputPortBase");
        description.AddProperty<size_t>("size", "Dimension of the output port");
        return description;
    }

    utilities::ObjectDescription OutputPortBase::GetDescription() const
    {
        utilities::ObjectDescription description = GetParentDescription<Port, OutputPortBase>();
        description["size"] = _size;
        return description;
    }

    void OutputPortBase::SetObjectState(const utilities::ObjectDescription& description, utilities::SerializationContext& context)
    {
        Port::SetObjectState(description, context);
        description["size"] >> _size;
    }    
}
