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
    OutputPortBase::OutputPortBase(const Node* node, std::string name, PortType type, size_t size)
        : Port(node, name, type), _layout({static_cast<int>(size)}), _isReferenced(false)
    {
    }

    OutputPortBase::OutputPortBase(const Node* node, std::string name, PortType type, const PortMemoryLayout& layout)
        : Port(node, name, type), _layout(layout),_isReferenced(false)
    {
    }

    void OutputPortBase::SetSize(size_t size)
    {
        _layout = PortMemoryLayout({static_cast<int>(size)});
    }
    
    void OutputPortBase::SetMemoryLayout(const PortMemoryLayout& layout)
    {
        _layout = layout;
    }
    
    utilities::ArchiveVersion OutputPortBase::GetArchiveVersion() const
    {
        return utilities::ArchiveVersionNumbers::v8_port_memory_layout;
    }

    bool OutputPortBase::CanReadArchiveVersion(const utilities::ArchiveVersion& version) const
    {
        return version <= utilities::ArchiveVersionNumbers::v8_port_memory_layout;
    }

    void OutputPortBase::WriteToArchive(utilities::Archiver& archiver) const
    {
        Port::WriteToArchive(archiver);
        archiver["layout"] << GetMemoryLayout();
    }

    void OutputPortBase::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Port::ReadFromArchive(archiver);
        int size = 0;
        archiver.OptionalProperty("size", 0) >> size;
        archiver.OptionalProperty("layout", PortMemoryLayout({size})) >> _layout;
    }
}
}
