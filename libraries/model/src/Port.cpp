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

    void Port::AddProperties(utilities::Archiver& archiver) const
    {
        archiver["nodeId"] << _node->GetId();
        archiver["name"] << _name;
        archiver["type"] << static_cast<int>(_type);
    }

    void Port::SetObjectState(const utilities::Archiver& archiver, utilities::SerializationContext& context)
    {
        auto nodeId = archiver["nodeId"].GetValue<utilities::UniqueId>(); // ignore it
        archiver["name"] >> _name;
        int typeCode;
        archiver["type"] >> typeCode;
        _type = static_cast<PortType>(typeCode);
    }
}
