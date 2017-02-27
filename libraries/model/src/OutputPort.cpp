////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OutputPort.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "OutputPort.h"

namespace ell
{
namespace model
{
    OutputPortBase::OutputPortBase(const class Node* node, std::string name, PortType type, size_t size)
        : Port(node, name, type), _size(size), _isReferenced(false)
    {
    }

    void OutputPortBase::SetSize(size_t size)
    {
        _size = size;
    }
    
    void OutputPortBase::WriteToArchive(utilities::Archiver& archiver) const
    {
        Port::WriteToArchive(archiver);
        archiver["size"] << _size;
    }

    void OutputPortBase::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Port::ReadFromArchive(archiver);
        archiver["size"] >> _size;
    }
}
}
