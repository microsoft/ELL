////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     OutputPortElementList.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "OutputPortElementList.h"

namespace model
{
    //
    // OutputPortElement
    //

    OutputPortElement::OutputPortElement(const OutputPortBase& port, size_t index) : _referencedPort(&port), _index(index) {}

    //
    // OutputPortRange
    //
    OutputPortRange::OutputPortRange(const OutputPortBase& port) : _referencedPort(&port), _startIndex(0), _numValues(port.Size()), _isFixedSize(false) {}

    OutputPortRange::OutputPortRange(const OutputPortBase& port, size_t index) : _referencedPort(&port), _startIndex(index), _numValues(1), _isFixedSize(true) {}

    OutputPortRange::OutputPortRange(const OutputPortBase& port, size_t startIndex, size_t numValues) : _referencedPort(&port), _startIndex(startIndex), _numValues(numValues), _isFixedSize(true) {}

    size_t OutputPortRange::Size() const
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

    size_t OutputPortRange::GetStartIndex() const { return _startIndex; }

    bool OutputPortRange::IsFullPortRange() const
    {
        return GetStartIndex() == 0 && Size() == ReferencedPort()->Size();
    }

    //
    // OutputPortElementListUntyped
    //
    OutputPortElementListUntyped::OutputPortElementListUntyped(const OutputPortBase& port)
    {
        _ranges.emplace_back(port);
        ComputeSize();
    }

    OutputPortElementListUntyped::OutputPortElementListUntyped(const OutputPortBase& port, size_t startIndex)
    {
        _ranges.emplace_back(port, startIndex);
        ComputeSize();
    }

    OutputPortElementListUntyped::OutputPortElementListUntyped(const OutputPortBase& port, size_t startIndex, size_t numValues)
    {
        _ranges.emplace_back(port, startIndex, numValues);
        ComputeSize();
    }

    OutputPortElementListUntyped::OutputPortElementListUntyped(const OutputPortRange& range)
    {
        _ranges.push_back(range);
        ComputeSize();
    }

    OutputPortElementListUntyped::OutputPortElementListUntyped(const std::vector<OutputPortRange>& ranges)
    {
        _ranges.insert(_ranges.end(), ranges.begin(), ranges.end());
        ComputeSize();
    }
    
    void OutputPortElementListUntyped::AddRange(const OutputPortRange& range)
    {
        _ranges.push_back(range);
    }

    void OutputPortElementListUntyped::ComputeSize()
    {
        _size = 0;
        for (const auto& range : _ranges)
        {
            _size += range.Size();
        }
    }
}
