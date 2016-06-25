////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     OutputPortRef.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "OutputPortRef.h"

namespace model
{
    //
    // OutputPortElementRef
    //

    OutputPortElementRef::OutputPortElementRef(const Port* port, size_t index) : _referencedPort(port), _index(index) {}

    //
    // OutputPortRange
    //
    OutputPortRange::OutputPortRange(const Port& port) : _referencedPort(&port), _startIndex(0), _numValues(port.Size()), _isFixedSize(false) {}

    OutputPortRange::OutputPortRange(const Port& port, size_t index) : _referencedPort(&port), _startIndex(index), _numValues(1), _isFixedSize(true) {}

    OutputPortRange::OutputPortRange(const Port& port, size_t startIndex, size_t numValues) : _referencedPort(&port), _startIndex(startIndex), _numValues(numValues), _isFixedSize(true) {}

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

    //
    // OutputPortRefUntyped
    //
    OutputPortRefUntyped::OutputPortRefUntyped(const Port& port)
    {
        _ranges.emplace_back(port);
        ComputeSize();
    }

    OutputPortRefUntyped::OutputPortRefUntyped(const Port& port, size_t startIndex)
    {
        _ranges.emplace_back(port, startIndex);
        ComputeSize();
    }

    OutputPortRefUntyped::OutputPortRefUntyped(const Port& port, size_t startIndex, size_t numValues)
    {
        _ranges.emplace_back(port, startIndex, numValues);
        ComputeSize();
    }

    OutputPortRefUntyped::OutputPortRefUntyped(const OutputPortRange& range)
    {
        _ranges.push_back(range);
        ComputeSize();
    }

    OutputPortRefUntyped::OutputPortRefUntyped(const std::vector<OutputPortRange>& ranges)
    {
        _ranges.insert(_ranges.end(), ranges.begin(), ranges.end());
        ComputeSize();
    }

    void OutputPortRefUntyped::AddRange(const OutputPortRange& range)
    {
        _ranges.push_back(range);
    }

    void OutputPortRefUntyped::ComputeSize()
    {
        _size = 0;
        for (const auto& range : _ranges)
        {
            _size += range.Size();
        }
    }
}
