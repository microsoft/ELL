////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Port.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Port.h"
#include "Node.h"

namespace ell
{
namespace model
{
    //
    // Specializations of GetPortType
    //
    template <>
    Port::PortType Port::GetPortType<float>()
    {
        return Port::PortType::smallReal;
    }

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
    Port::PortType Port::GetPortType<int64_t>()
    {
        return Port::PortType::bigInt;
    }

    template <>
    Port::PortType Port::GetPortType<bool>()
    {
        return Port::PortType::boolean;
    }

    void Port::WriteToArchive(utilities::Archiver& archiver) const
    {
        archiver["nodeId"] << _node->GetId();
        archiver["name"] << _name;
        archiver["type"] << static_cast<int>(_type);
    }

    void Port::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::NodeId id;
        archiver["nodeId"] >> id;
        archiver["name"] >> _name;
        int typeCode;
        archiver["type"] >> typeCode;
        _type = static_cast<PortType>(typeCode);
    }

    std::string GetPortCTypeName(ell::model::Port::PortType type)
    {
        switch (type)
        {
            case ell::model::Port::PortType::none:
                return "void";
            case ell::model::Port::PortType::smallReal:
                return "float";
            case ell::model::Port::PortType::real:
                return "double";
            case ell::model::Port::PortType::integer:
                return "int";
            case ell::model::Port::PortType::bigInt:
                return "int64_t";
            case ell::model::Port::PortType::categorical:
                return "int";
            case ell::model::Port::PortType::boolean:
                return "bool"; // ???
            default:
                return "Unknown";
        };
    }
}
}
