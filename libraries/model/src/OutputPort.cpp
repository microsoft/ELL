////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OutputPort.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "InputPort.h"
#include "OutputPort.h"

#include <utilities/include/Exception.h>

#include <algorithm>

namespace ell
{
namespace model
{
    OutputPortBase::OutputPortBase(const Node* node, std::string name, PortType type, size_t size) :
        Port(node, name, type),
        _layout({ static_cast<int>(size) })
    {
    }

    OutputPortBase::OutputPortBase(const Node* node, std::string name, PortType type, const PortMemoryLayout& layout) :
        Port(node, name, type),
        _layout(layout)
    {
    }

    OutputPortBase::~OutputPortBase()
    {
        for(const InputPortBase* ref: _references)
        {
            const_cast<InputPortBase*>(ref)->ClearReferencedPort();
        }
    }

    void OutputPortBase::AddReference(const InputPortBase* reference) const
    {
        if (HasReference(reference))
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Trying to add an already-existing reference to an output port");
        }
        _references.push_back(reference);
    }

    void OutputPortBase::RemoveReference(const InputPortBase* reference) const
    {
       auto it = std::find(_references.begin(), _references.end(), reference);
        if (it == _references.end())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Trying to remove a nonexistent reference from an output port");
        }
        _references.erase(it);
    }

    bool OutputPortBase::HasReference(const InputPortBase* reference) const
    {
        auto it = std::find(_references.begin(), _references.end(), reference);
        return it != _references.end();
    }

    bool OutputPortBase::IsReferenced() const
    {
        return !_references.empty();
    }

    const std::vector<const InputPortBase*>& OutputPortBase::GetReferences() const
    {
        return _references;
    }

    void OutputPortBase::SetSize(size_t size)
    {
        _layout = PortMemoryLayout({ static_cast<int>(size) });
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
        archiver.OptionalProperty("layout", PortMemoryLayout({ size })) >> _layout;
    }
} // namespace model
} // namespace ell
