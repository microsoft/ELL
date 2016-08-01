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
    OutputPortBase::OutputPortBase(const class Node* node, std::string name, PortType type, size_t size) : Port(node, name, type, size), _isReferenced(false) 
    {}

    void OutputPortBase::Serialize(utilities::Serializer& serializer) const
    {
        Port::Serialize(serializer);
        serializer.Serialize("isReferenced", _isReferenced);
    }

    void OutputPortBase::Deserialize(utilities::Deserializer& serializer, utilities::SerializationContext& context)
    {
        throw "Not implemented";
    }
}
