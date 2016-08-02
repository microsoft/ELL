////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Port.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Port.h"
#include "Node.h"

/// <summary> model namespace </summary>
namespace model
{
    //
    // Specializations of GetPortType
    //
    template <>
    Port::PortType Port::GetPortType<double>()
    {
        return Port::PortType::Real;
    }

    template <>
    Port::PortType Port::GetPortType<int>()
    {
        return Port::PortType::Integer;
    }

    template <>
    Port::PortType Port::GetPortType<bool>()
    {
        return Port::PortType::Boolean;
    }

    void Port::Serialize(utilities::Serializer& serializer) const
    {
        serializer.Serialize("nodeId", _node->GetId());
        serializer.Serialize("name", _name);
        serializer.Serialize("type", static_cast<int>(_type));
        serializer.Serialize("size", _size);
    }

    void Port::Deserialize(utilities::Deserializer& serializer, utilities::SerializationContext& context)
    {
        Node::NodeId id;
        serializer.Deserialize("nodeId", id, context);
        serializer.Deserialize("name", _name, context);
        int typeCode = 0;
        serializer.Deserialize("type", typeCode, context);
        _type = static_cast<PortType>(typeCode);
        serializer.Deserialize("size", _size, context);
    }

    void Port::SetSize(size_t size)
    {
        _size = size;
    }
}
