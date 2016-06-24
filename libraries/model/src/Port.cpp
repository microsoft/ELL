////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Port.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Port.h"

/// <summary> model namespace </summary>
namespace model
{
    void Port::SetType(PortType type) 
    { 
        _type = type; 
    }

    void Port::SetSize(size_t size) 
    { 
        _size = size; 
    }

    //
    // Specializations of GetTypeCode
    //
    template <>
    Port::PortType Port::GetTypeCode<double>()
    {
        return Port::PortType::Real;
    }

    template <>
    Port::PortType Port::GetTypeCode<int>()
    {
        return Port::PortType::Integer;
    }

    template <>
    Port::PortType Port::GetTypeCode<bool>()
    {
        return Port::PortType::Boolean;
    }

}
