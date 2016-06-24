////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     OutputRef.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "OutputRef.h"

namespace model
{
    //
    // OutputRange
    //
    OutputRange::OutputRange(const Port& port) : _referencedPort(&port), _startIndex(0), _numValues(port.Size()), _isFixedSize(false) {}

    OutputRange::OutputRange(const Port& port, size_t index) : _referencedPort(&port), _startIndex(index), _numValues(1), _isFixedSize(true) {}

    OutputRange::OutputRange(const Port& port, size_t startIndex, size_t numValues) : _referencedPort(&port), _startIndex(startIndex), _numValues(numValues), _isFixedSize(true) {}

    size_t OutputRange::Size() const
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

    size_t OutputRange::GetStartIndex() const { return _startIndex; }

    //
    // UntypedOutputRef
    //
    UntypedOutputRef::UntypedOutputRef(const Port& port)
    {
        _ranges.emplace_back(port);
        ComputeSize();
    }

    UntypedOutputRef::UntypedOutputRef(const Port& port, size_t startIndex)
    {
        _ranges.emplace_back(port, startIndex);
        ComputeSize();
    }

    UntypedOutputRef::UntypedOutputRef(const Port& port, size_t startIndex, size_t numValues)
    {
        _ranges.emplace_back(port, startIndex, numValues);
        ComputeSize();
    }

    UntypedOutputRef::UntypedOutputRef(const OutputRange& range)
    {
        _ranges.push_back(range);
        ComputeSize();
    }

    UntypedOutputRef::UntypedOutputRef(const std::vector<OutputRange>& ranges)
    {
        _ranges.insert(_ranges.end(), ranges.begin(), ranges.end());
        ComputeSize();
    }

    void UntypedOutputRef::ComputeSize()
    {
        _size = 0;
        for (const auto& range : _ranges)
        {
            _size += range.Size();
        }
    }
}
