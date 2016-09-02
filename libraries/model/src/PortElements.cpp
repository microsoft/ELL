////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     PortElements.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PortElements.h"
#include "Node.h"
#include "Model.h"

#include <cassert>

// utilities
#include "Exception.h"

namespace model
{
    //
    // PortElementBase
    //
    PortElementBase::PortElementBase(const OutputPortBase& port, size_t index) : _referencedPort(&port), _index(index) {}

    bool PortElementBase::operator==(const PortElementBase& other) const { return (_referencedPort == other._referencedPort) && (_index == other._index); }

    //
    // PortRange
    //
    PortRange::PortRange(const OutputPortBase& port) : _referencedPort(&port), _startIndex(0), _numValues(port.Size()), _isFixedSize(false) {}

    PortRange::PortRange(const OutputPortBase& port, size_t index) : _referencedPort(&port), _startIndex(index), _numValues(1), _isFixedSize(true) {}

    PortRange::PortRange(const OutputPortBase& port, size_t startIndex, size_t numValues) : _referencedPort(&port), _startIndex(startIndex), _numValues(numValues), _isFixedSize(true) {}

    size_t PortRange::Size() const
    {
        if (_isFixedSize)
        {
            return _numValues;
        }
        else
        {
            return _referencedPort->Size();
        }
    }

    bool PortRange::IsFullPortRange() const { return GetStartIndex() == 0 && Size() == ReferencedPort()->Size(); }

    void PortRange::AddProperties(utilities::Archiver& archiver) const
    {
        archiver["startIndex"] << _startIndex;
        archiver["numValues"] << _numValues;
        archiver["isFixedSize"] << _isFixedSize;
        if (_referencedPort != nullptr)
        {
            archiver["referencedNodeId"] << _referencedPort->GetNode()->GetId();
            archiver["referencedPortName"] << _referencedPort->GetName();
        }
        else
        {
            archiver["referencedNodeId"] << utilities::UniqueId();
            archiver["referencedPortName"] << std::string{ "" };
        }
    }

    void PortRange::SetObjectState(const utilities::Archiver& archiver, utilities::SerializationContext& context)
    {
        archiver["startIndex"] >> _startIndex;
        archiver["numValues"] >> _numValues;
        archiver["isFixedSize"] >> _isFixedSize;
        Node::NodeId newId;
        archiver["referencedNodeId"] >> newId;
        std::string portName;
        archiver["referencedPortName"] >> portName;

        model::ModelSerializationContext& newContext = dynamic_cast<model::ModelSerializationContext&>(context);
        Node* newNode = newContext.GetNodeFromId(newId);
        if (newNode == nullptr)
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Could not find deserialized node.");
        }

        auto ports = newNode->GetOutputPorts();
        OutputPortBase* newPort = nullptr;
        for (auto port : ports)
        {
            if (port->GetName() == portName)
            {
                newPort = port;
                break;
            }
        }
        if (_referencedPort == newPort)
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Error deserializing port.");
        }
        _referencedPort = newPort;
        if (newPort == nullptr)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::nullReference, "Couldn't deserialize model::PortRange port");
        }
    }

    bool PortRange::operator==(const PortRange& other) const { return (_referencedPort == other._referencedPort) && (_startIndex == other._startIndex) && (_numValues == other._numValues); }

    //
    // PortElementsBase
    //
    PortElementsBase::PortElementsBase(const OutputPortBase& port)
    {
        _ranges.emplace_back(port);
        ComputeSize();
    }

    PortElementsBase::PortElementsBase(const OutputPortBase& port, size_t startIndex)
    {
        _ranges.emplace_back(port, startIndex);
        ComputeSize();
    }

    PortElementsBase::PortElementsBase(const OutputPortBase& port, size_t startIndex, size_t numValues)
    {
        _ranges.emplace_back(port, startIndex, numValues);
        ComputeSize();
    }

    PortElementsBase::PortElementsBase(const PortRange& range)
    {
        _ranges.push_back(range);
        _size += range.Size();
    }

    PortElementsBase::PortElementsBase(const std::vector<PortRange>& ranges)
    {
        _ranges.insert(_ranges.end(), ranges.begin(), ranges.end());
        ComputeSize();
    }

    void PortElementsBase::Reserve(size_t numRanges) { _ranges.reserve(numRanges); }

    void PortElementsBase::AddRange(const PortRange& range)
    {
        _ranges.push_back(range);
        _size += range.Size();
    }

    PortElementBase PortElementsBase::GetElement(size_t index) const
    {
        size_t sumRangeSizesSoFar = 0;
        for (const auto& range : _ranges)
        {
            if (index < sumRangeSizesSoFar + range.Size())
            {
                return PortElementBase(*range.ReferencedPort(), range.GetStartIndex() + index - sumRangeSizesSoFar);
            }
            sumRangeSizesSoFar += range.Size();
        }
        throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index exceeds PortElements range");
    }

    void PortElementsBase::ComputeSize()
    {
        _size = 0;
        for (const auto& range : _ranges)
        {
            _size += range.Size();
        }
    }

    void PortElementsBase::AddProperties(utilities::Archiver& archiver) const
    {
        archiver["ranges"] << _ranges;
    }

    void PortElementsBase::SetObjectState(const utilities::Archiver& archiver, utilities::SerializationContext& context)
    {
        archiver["ranges"] >> _ranges;
        ComputeSize();
    }
}

//
// hash functions for PortElementUntyped and PortRange
//
std::hash<model::PortElementBase>::result_type std::hash<model::PortElementBase>::operator()(argument_type const& element) const
{
    auto hash1 = std::hash<const model::OutputPortBase*>()(element.ReferencedPort());
    auto hash2 = std::hash<size_t>()(element.GetIndex());
    return hash1 ^ (hash2 << 1);
}

std::hash<model::PortRange>::result_type std::hash<model::PortRange>::operator()(argument_type const& range) const
{
    auto hash1 = std::hash<const model::OutputPortBase*>()(range.ReferencedPort());
    auto hash2 = std::hash<size_t>()(range.Size());
    auto hash3 = std::hash<size_t>()(range.GetStartIndex());
    return hash1 ^ ((hash2 ^ (hash3 << 1)) << 1);
}
