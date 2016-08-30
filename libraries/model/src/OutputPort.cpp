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

    void OutputPortBase::Serialize(utilities::Serializer& serializer) const
    {
        Port::Serialize(serializer);
        serializer.Serialize("size", _size);
    }

    void OutputPortBase::Deserialize(utilities::Deserializer& serializer, utilities::SerializationContext& context)
    {
        Port::Deserialize(serializer, context);
        serializer.Deserialize("size", _size, context);
    }

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

    void OutputPortBase::SetObjectState(const utilities::ObjectDescription& description)
    {
        Port::SetObjectState(description);
        _size = description["size"].GetValue<size_t>();
    }    
}
