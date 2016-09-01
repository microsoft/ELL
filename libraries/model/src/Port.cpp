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
        return Port::PortType::real;
    }

    template <>
    Port::PortType Port::GetPortType<int>()
    {
        return Port::PortType::integer;
    }

    template <>
    Port::PortType Port::GetPortType<bool>()
    {
        return Port::PortType::boolean;
    }

    utilities::ObjectDescription Port::GetTypeDescription()
    {
        utilities::ObjectDescription description = utilities::MakeObjectDescription<Port>("Port");
        description.AddProperty<utilities::UniqueId>("nodeId", "ID of port's node");
        description.AddProperty<std::string>("name", "Name of this port");
        description.AddProperty<int>("type", "Type code for the port");
        return description;
    }

    utilities::ObjectDescription Port::GetDescription() const
    {
        utilities::ObjectDescription description = GetTypeDescription();
        description["nodeId"] << _node->GetId();
        description["name"] << _name;
        description["type"] << static_cast<int>(_type);
        return description;
    }

    void Port::SetObjectState(const utilities::ObjectDescription& description, utilities::SerializationContext& context)
    {
        auto nodeId = description["nodeId"].GetValue<utilities::UniqueId>(); // ignore it
        description["name"] >> _name;
        int typeCode;
        description["type"] >> typeCode;
        _type = static_cast<PortType>(typeCode);
    }
}
