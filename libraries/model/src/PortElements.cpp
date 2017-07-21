////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PortElements.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PortElements.h"
#include "Model.h"
#include "Node.h"

// utilities
#include "Exception.h"
#include "Tokenizer.h"

// stl
#include <cassert>
#include <sstream>

namespace ell
{
namespace model
{
    namespace
    {
        //
        // Helper functions for parsing and stringifying proxies
        //
        std::string GetRangeString(const PortRangeProxy& range)
        {
            using std::to_string;
            std::string prefix = to_string(range.GetNodeId()) + "." + range.GetPortName();
            if (!range.IsFixedSize())
            {
                return prefix;
            }
            else if (range.Size() == 1)
            {
                return prefix + "[" + to_string(range.GetStartIndex()) + "]";
            }
            return prefix + "[" + to_string(range.GetStartIndex()) + ":" + to_string(range.GetStartIndex() + range.Size()) + "]";
        }

        PortRangeProxy ParseRange(utilities::Tokenizer& tokenizer)
        {
            auto nodeIdStr = tokenizer.ReadNextToken();
            auto nodeId = utilities::UniqueId(nodeIdStr);
            tokenizer.MatchToken(".");
            auto portName = tokenizer.ReadNextToken();
            // now check for element/element slice
            if (tokenizer.PeekNextToken() == "[")
            {
                tokenizer.MatchToken("[");
                auto token = tokenizer.ReadNextToken();
                size_t startIndex = std::stoi(token);
                size_t size = 1;
                if (tokenizer.PeekNextToken() == ":")
                {
                    tokenizer.MatchToken(":");
                    auto endIndex = std::stoi(tokenizer.ReadNextToken());
                    size = endIndex - startIndex;
                }

                tokenizer.MatchToken("]");
                return { nodeId, portName, startIndex, size };
            }
            else
            {
                return { nodeId, portName };
            }
        }

        PortRangeProxy ParseRange(const std::string& str)
        {
            std::stringstream stream(str);
            std::string delimiters = "{}[],.:";
            utilities::Tokenizer tokenizer(stream, delimiters);
            return ParseRange(tokenizer);
        }

        std::vector<PortRangeProxy> ParseRangeList(utilities::Tokenizer& tokenizer)
        {
            std::vector<PortRangeProxy> result;
            while (true)
            {
                // read a range
                result.push_back(ParseRange(tokenizer));
                if (tokenizer.PeekNextToken() != ",")
                {
                    break;
                }
                tokenizer.MatchToken(",");
            }
            return result;
        }

        std::string GetPortElementsString(const PortElementsProxy& elements)
        {
            if (elements.NumRanges() == 0)
            {
                return "{}";
            }

            if (elements.NumRanges() == 1)
            {
                return GetRangeString(elements.GetRanges()[0]);
            }

            std::string valueString = "{";
            bool first = true;
            for (const auto& range : elements.GetRanges())
            {
                if (first)
                {
                    first = false;
                }
                else
                {
                    valueString += ", ";
                }
                valueString += GetRangeString(range);
            }

            valueString += "}";
            return valueString;
        }

        // Create a PortElementsProxy from a tokenizer
        PortElementsProxy ParsePortElements(utilities::Tokenizer& tokenizer)
        {
            auto t = tokenizer.PeekNextToken();
            if (t == "{")
            {
                tokenizer.MatchToken(t);
                auto ranges = ParseRangeList(tokenizer);
                tokenizer.MatchToken("}");
                return PortElementsProxy(ranges);
            }
            else
            {
                return PortElementsProxy(ParseRange(tokenizer));
            }
        }

        // Create a PortElementsProxy from a string
        PortElementsProxy ParsePortElements(const std::string& str)
        {
            std::stringstream stream(str);
            std::string delimiters = "{}[],.:";
            utilities::Tokenizer tokenizer(stream, delimiters);
            return ParsePortElements(tokenizer);
        }

        PortRange PortRangeFromArchivedProxy(utilities::Unarchiver& archiver, const PortRangeProxy& proxy)
        {
            auto& context = archiver.GetContext();
            ModelSerializationContext& modelContext = dynamic_cast<ModelSerializationContext&>(context);
            Node* node = modelContext.GetNodeFromSerializedId(proxy.GetNodeId());
            if (node == nullptr)
            {
                throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Could not find archived node.");
            }

            OutputPortBase* newPort = node->GetOutputPort(proxy.GetPortName());
            if (newPort == nullptr)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::nullReference, "Couldn't unarchive PortRange port -- no port of that name");
            }

            if (proxy.IsFixedSize())
            {
                return PortRange(*newPort, proxy.GetStartIndex(), proxy.Size());
            }
            else
            {
                return PortRange(*newPort);
            }
        }

    } // end anonymous namespace

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

    data::IndexValue PortElementsBase::Iterator::Get()
    {
        if (!IsValid())
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
    // PortRange
    //
    PortRange::PortRange(const OutputPortBase& port)
        : _referencedPort(&port), _startIndex(0), _numValues(port.Size()), _isFixedSize(false) {}

    PortRange::PortRange(const OutputPortBase& port, size_t index)
        : _referencedPort(&port), _startIndex(index), _numValues(1), _isFixedSize(true) {}

    PortRange::PortRange(const OutputPortBase& port, size_t startIndex, size_t numValues)
        : _referencedPort(&port), _startIndex(startIndex), _numValues(numValues), _isFixedSize(true) {}

    PortRange::PortRange(const PortElementBase& element)
        : _referencedPort(element.ReferencedPort()), _startIndex(element.GetIndex()), _numValues(1), _isFixedSize(true) {}

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
        PortRangeProxy proxy(*this);
        archiver << GetRangeString(proxy);
    }

    void PortRange::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        std::string str;
        archiver >> str;
        auto proxy = ParseRange(str);
        auto portRange = PortRangeFromArchivedProxy(archiver, proxy);
        *this = portRange;
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

    PortElementsBase::PortElementsBase(const PortElementBase& element)
    {
        _ranges.push_back(PortRange{ element });
        _size = 1;
    }

    PortElementsBase::PortElementsBase(const PortRange& range)
    {
        _ranges.emplace_back(range);
        _size = range.Size();
    }

    PortElementsBase::PortElementsBase(const std::vector<PortRange>& ranges)
    {
        _ranges.insert(_ranges.end(), ranges.begin(), ranges.end());
        ComputeSize();
    }

    bool PortElementBase::operator==(const PortElementBase& other) const
    {
        return (_referencedPort == other._referencedPort) && (_index == other._index);
    }

    Port::PortType PortElementsBase::GetPortType() const
    {
        if (Size() == 0)
        {
            return Port::PortType::none;
        }

        return _ranges[0].GetPortType();
    }

    void PortElementsBase::Reserve(size_t numRanges)
    {
        _ranges.reserve(numRanges);
    }

    void PortElementsBase::AddRange(const PortRange& range)
    {
        // Check if range is contiguous with _ranges.back(), and if so, just add range.Size() to ranges.back()
        if (_ranges.size() > 0 && _ranges.back().IsAdjacent(range))
        {
            _ranges.back().Append(range);
        }
        else
        {
            _ranges.push_back(range);
        }
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
        }
    }

    void PortElementsBase::WriteToArchive(utilities::Archiver& archiver) const
    {
        PortElementsProxy proxy(*this);
        archiver << GetPortElementsString(proxy);
    }

    void PortElementsBase::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        std::string str;
        archiver >> str;
        auto proxy = ParsePortElements(str);
        for (const auto& rangeProxy : proxy.GetRanges())
        {
            _ranges.push_back(PortRangeFromArchivedProxy(archiver, rangeProxy));
        }
        ComputeSize();
    }

    //
    // PortRangeProxy
    //

    PortRangeProxy::PortRangeProxy(Node::NodeId nodeId, std::string portName)
        : _nodeId(nodeId), _portName(portName), _portType(Port::PortType::none), _startIndex(0), _numValues(0), _isFixedSize(false)
    {
    }

    PortRangeProxy::PortRangeProxy(Node::NodeId nodeId, std::string portName, size_t startIndex)
        : _nodeId(nodeId), _portName(portName), _portType(Port::PortType::none), _startIndex(startIndex), _numValues(0), _isFixedSize(true)
    {
    }

    PortRangeProxy::PortRangeProxy(Node::NodeId nodeId, std::string portName, size_t startIndex, size_t numValues)
        : _nodeId(nodeId), _portName(portName), _portType(Port::PortType::none), _startIndex(startIndex), _numValues(numValues), _isFixedSize(true)
    {
    }

    PortRangeProxy::PortRangeProxy(Node::NodeId nodeId, std::string portName, Port::PortType portType, size_t startIndex, size_t numValues)
        : _nodeId(nodeId), _portName(portName), _portType(portType), _startIndex(startIndex), _numValues(numValues), _isFixedSize(true)
    {
    }

    PortRangeProxy::PortRangeProxy(const PortRange& range)
    {
        if (range.ReferencedPort() == nullptr || range.ReferencedPort()->GetNode() == nullptr)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::nullReference);
        }

        _nodeId = range.ReferencedPort()->GetNode()->GetId();
        _portName = range.ReferencedPort()->GetName();
        _portType = range.GetPortType();
        _startIndex = range.GetStartIndex();
        _numValues = range.Size();
        _isFixedSize = range.IsFixedSize();
    }

    void PortRangeProxy::WriteToArchive(utilities::Archiver& archiver) const
    {
        archiver << GetRangeString(*this);
    }

    void PortRangeProxy::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        std::string str;
        archiver >> str;
        auto proxy = ParseRange(str);
        *this = proxy;
    }

    //
    // PortElementsProxy
    //

    PortElementsProxy::PortElementsProxy()
        : _portType(Port::PortType::none)
    {
    }

    PortElementsProxy::PortElementsProxy(Port::PortType portType)
        : _portType(portType)
    {
    }

    PortElementsProxy::PortElementsProxy(const PortElementsBase& elements)
    {
        for (auto r : elements.GetRanges())
        {
            Append(r);
        }
    }

    PortElementsProxy::PortElementsProxy(const PortRangeProxy& range)
    {
        Append(range);
    }

    PortElementsProxy::PortElementsProxy(const std::vector<PortRangeProxy>& ranges)
    {
        for (auto r : ranges)
        {
            Append(r);
        }
    }

    void PortElementsProxy::Append(const PortRangeProxy& range)
    {
        if (_portType == Port::PortType::none)
        {
            _portType = range.GetPortType();
        }
        else if (_portType != range.GetPortType())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }
        _ranges.push_back(range);
    }

    void PortElementsProxy::WriteToArchive(utilities::Archiver& archiver) const
    {
        archiver << GetPortElementsString(*this);
    }

    void PortElementsProxy::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        std::string str;
        archiver >> str;
        auto proxy = ParsePortElements(str);
        *this = proxy;
    }

    //
    // Helper functions
    //

    PortElementsProxy ParsePortElementsProxy(std::string str)
    {
        return ParsePortElements(str);
    }

    PortElementsBase ProxyToPortElements(const Model& model, const PortElementsProxy& proxy)
    {
        PortElementsBase elements;
        for (auto rangeProxy : proxy.GetRanges())
        {
            const Node* node = model.GetNode(rangeProxy.GetNodeId());
            if (node == nullptr)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::nullReference, "Couldn't unarchive PortRange port -- bad node id");
            }

            const OutputPortBase* port = node->GetOutputPort(rangeProxy.GetPortName());
            if (port == nullptr)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::nullReference, "Couldn't unarchive PortRange port -- bad port name");
            }

            if (rangeProxy.GetPortType() != Port::PortType::none && port->GetType() != rangeProxy.GetPortType())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
            }

            // Transform to real range
            auto size = rangeProxy.Size();
            if (!rangeProxy.IsFixedSize())
            {
                size = port->Size();
            }
            elements.Append(PortRange(*port, rangeProxy.GetStartIndex(), size));
        }
        return elements;
    }
}
}

//
// hash functions for PortElementUntyped and PortRange
//
std::hash<ell::model::PortElementBase>::result_type std::hash<ell::model::PortElementBase>::operator()(argument_type const& element) const
{
    auto hash1 = std::hash<const ell::model::OutputPortBase*>()(element.ReferencedPort());
    auto hash2 = std::hash<size_t>()(element.GetIndex());
    return hash1 ^ (hash2 << 1);
}

std::hash<ell::model::PortRange>::result_type std::hash<ell::model::PortRange>::operator()(argument_type const& range) const
{
    auto hash1 = std::hash<const ell::model::OutputPortBase*>()(range.ReferencedPort());
    auto hash2 = std::hash<size_t>()(range.Size());
    auto hash3 = std::hash<size_t>()(range.GetStartIndex());
    return hash1 ^ ((hash2 ^ (hash3 << 1)) << 1);
}
