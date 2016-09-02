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

    void OutputPortBase::Serialize(utilities::Archiver& archiver) const
    {
        Port::Serialize(archiver);
        archiver["size"] << _size;

    }

    void OutputPortBase::Deserialize(utilities::Unarchiver& archiver)
    {
        Port::Deserialize(archiver);
        archiver["size"] >> _size;
    }    
}
