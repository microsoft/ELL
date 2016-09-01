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

    void OutputPortBase::GetDescription(utilities::ObjectDescription& description) const
    {
        Port::GetDescription(description);
        description.SetType(*this);
        description["size"] << _size;

    }

    void OutputPortBase::SetObjectState(const utilities::ObjectDescription& description, utilities::SerializationContext& context)
    {
        Port::SetObjectState(description, context);
        description["size"] >> _size;
    }    
}
