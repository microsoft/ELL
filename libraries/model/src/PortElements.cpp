////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     PortElements.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PortElements.h"
#include "Model.h"
#include "Node.h"

#include <cassert>

// utilities
#include "Exception.h"

namespace emll
{
namespace model
{
    //
    // PortElementBase::Iterator
    //
    void PortElementsBase::Iterator::Next()
    {
        if (_ranges.size() == 0)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange);
        }

        if (_ranges[0].Size() == 1)
        {
            _ranges.erase(_ranges.begin());
        }
        else
        {
            auto oldRange = _ranges[0];
            _ranges[0] = { *oldRange.ReferencedPort(), oldRange.GetStartIndex() + 1, oldRange.Size() - 1 };
        }

        ++_index;
    }

    linear::IndexValue PortElementsBase::Iterator::Get()
    {
        if(!IsValid())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange);
        }

        return { _index, _ranges[0].ReferencedPort()->GetDoubleOutput(_ranges[0].GetStartIndex()) };
    }

    PortElementsBase::Iterator::Iterator(const std::vector<PortRange>& ranges)
        : _ranges(ranges.begin(), ranges.end())
    {
    }

    //
    // PortElementBase
    //
    PortElementBase::PortElementBase(const OutputPortBase& port, size_t index)
        : _referencedPort(&port), _index(index) {}

    void PortElementsBase::Append(const PortElementsBase& other)
    {
        for (const auto& range : other.GetRanges())
        {
            AddRange(range);
        }
    }

    bool PortElementBase::operator==(const PortElementBase& other) const { return (_referencedPort == other._referencedPort) && (_index == other._index); }

    //
    // PortRange
    //
    PortRange::PortRange(const OutputPortBase& port)
        : _referencedPort(&port), _startIndex(0), _numValues(port.Size()), _isFixedSize(false) {}

    PortRange::PortRange(const OutputPortBase& port, size_t index)
        : _referencedPort(&port), _startIndex(index), _numValues(1), _isFixedSize(true) {}

    PortRange::PortRange(const OutputPortBase& port, size_t startIndex, size_t numValues)
        : _referencedPort(&port), _startIndex(startIndex), _numValues(numValues), _isFixedSize(true) {}

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

    void PortRange::WriteToArchive(utilities::Archiver& archiver) const
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

    void PortRange::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        archiver["startIndex"] >> _startIndex;
        archiver["numValues"] >> _numValues;
        archiver["isFixedSize"] >> _isFixedSize;
        Node::NodeId newId;
        archiver["referencedNodeId"] >> newId;
        std::string portName;
        archiver["referencedPortName"] >> portName;

        auto& context = archiver.GetContext();
        ModelSerializationContext& newContext = dynamic_cast<ModelSerializationContext&>(context);
        Node* newNode = newContext.GetNodeFromSerializedId(newId);
        if (newNode == nullptr)
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Could not find archived node.");
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
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Error unarchiving port.");
        }
        _referencedPort = newPort;
        if (newPort == nullptr)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::nullReference, "Couldn't unarchive PortRange port");
        }
    }

    bool PortRange::IsAdjacent(const PortRange& other) const
    {
        return (ReferencedPort() == other.ReferencedPort()) && (GetStartIndex() + Size() == other.GetStartIndex());
    }

    void PortRange::Append(const PortRange& other)
    {
        if (IsAdjacent(other) && _isFixedSize)
        {
            _numValues += other.Size();
        }
    }

    bool PortRange::operator==(const PortRange& other) const
    {
        return (_referencedPort == other._referencedPort) && (_startIndex == other._startIndex) && (_numValues == other._numValues);
    }

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

    void PortElementsBase::Consolidate()
    {
        if (_ranges.size() > 1)
        {
            auto oldSize = Size();

            // For each range, check if it's adjacent to the next one. If so, combine them
            std::vector<PortRange> newRanges;
            newRanges.push_back(_ranges[0]);
            auto numRanges = _ranges.size();
            for (size_t index = 1; index < numRanges; ++index)
            {
                const auto r = _ranges[index];
                if (newRanges.back().IsAdjacent(r))
                {
                    newRanges.back().Append(r);
                }
                else
                {
                    newRanges.push_back(r);
                }
            }
            _ranges = newRanges;

            ComputeSize();
            assert(oldSize == Size());
        }
    }

    void PortElementsBase::WriteToArchive(utilities::Archiver& archiver) const
    {
        archiver["ranges"] << _ranges;
    }

    void PortElementsBase::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        archiver["ranges"] >> _ranges;
        ComputeSize();
    }
}
}

//
// hash functions for PortElementUntyped and PortRange
//
std::hash<emll::model::PortElementBase>::result_type std::hash<emll::model::PortElementBase>::operator()(argument_type const& element) const
{
    auto hash1 = std::hash<const emll::model::OutputPortBase*>()(element.ReferencedPort());
    auto hash2 = std::hash<size_t>()(element.GetIndex());
    return hash1 ^ (hash2 << 1);
}

std::hash<emll::model::PortRange>::result_type std::hash<emll::model::PortRange>::operator()(argument_type const& range) const
{
    auto hash1 = std::hash<const emll::model::OutputPortBase*>()(range.ReferencedPort());
    auto hash2 = std::hash<size_t>()(range.Size());
    auto hash3 = std::hash<size_t>()(range.GetStartIndex());
    return hash1 ^ ((hash2 ^ (hash3 << 1)) << 1);
}
