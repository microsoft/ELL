////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OutputPort.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "OutputPort.h"
#include "InputPort.h"

#include <utilities/include/Exception.h>

#include <algorithm>

namespace ell
{
namespace model
{

    OutputPortBase::OutputPortBase(const Node* node, std::string name, PortType type, size_t size) :
        OutputPortBase(node, name, type, PortMemoryLayout({ static_cast<int>(size) }))
    {
    }

    OutputPortBase::OutputPortBase(const Node* node, std::string name, PortType type, const PortMemoryLayout& layout) :
        Port(node, name, type),
        _layout(layout)
    {
        InitializeCachedOutput();
    }

    OutputPortBase::~OutputPortBase()
    {
        for (const InputPortBase* ref : _references)
        {
            const_cast<InputPortBase*>(ref)->ClearReferencedPort();
        }
    }

    std::vector<double> OutputPortBase::GetDoubleOutput() const
    {
        if (auto doubleVector = std::get_if<std::vector<double>>(&_cachedOutput); doubleVector != nullptr)
        {
            return *doubleVector;
        }
        else
        {
            return std::visit(
                [](auto&& value) -> std::vector<double> {
                    return std::vector<double>(value.begin(), value.end());
                },
                _cachedOutput);
        }
    }

    double OutputPortBase::GetDoubleOutput(size_t index) const
    {
        if (auto doubleVector = std::get_if<std::vector<double>>(&_cachedOutput); doubleVector != nullptr)
        {
            return (*doubleVector)[index];
        }
        else
        {
            return std::visit(
                [index](auto&& value) -> double {
                    return static_cast<double>(value[index]);
                },
                _cachedOutput);
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
        InitializeCachedOutput();
    }

    void OutputPortBase::InitializeCachedOutput()
    {
        switch (GetType())
        {
        case PortType::bigInt:
            _cachedOutput = std::vector<int64_t>{};
            break;
        case PortType::boolean:
            _cachedOutput = std::vector<bool>{};
            break;
        case PortType::integer:
            _cachedOutput = std::vector<int32_t>{};
            break;
        case PortType::real:
            _cachedOutput = std::vector<double>{};
            break;
        case PortType::smallReal:
            _cachedOutput = std::vector<float>{};
            break;
        case PortType::categorical:
            [[fallthrough]];
        case PortType::none:
            [[fallthrough]];
        default:
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState);
        }
    }

    static_assert(sizeof(OutputPortBase) == sizeof(OutputPort<double>), "OutputPort<T> must have the same layout as OutputPortBase");
} // namespace model
} // namespace ell
