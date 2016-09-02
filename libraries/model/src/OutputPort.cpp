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

    void OutputPortBase::AddProperties(utilities::Archiver& archiver) const
    {
        Port::AddProperties(archiver);
        archiver.SetType(*this);
        archiver["size"] << _size;

    }

    void OutputPortBase::SetObjectState(const utilities::Archiver& archiver, utilities::SerializationContext& context)
    {
        Port::SetObjectState(archiver, context);
        archiver["size"] >> _size;
    }    
}
