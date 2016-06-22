////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     InputPort.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Port.h"
#include "OutputPort.h"
#include "Node.h"

#include <vector>
#include <cassert>

/// <summary> model namespace </summary>
namespace model
{
    class InputRange
    {
    public:
        InputRange(const Port& port) : _referencedPort(&port), _startIndex(0), _numValues(port.Size()), _isFixedSize(false) {}
        InputRange(const Port& port, size_t startIndex) : _referencedPort(&port), _startIndex(startIndex), _numValues(1), _isFixedSize(true) {}
        InputRange(const Port& port, size_t startIndex, size_t numValues) : _referencedPort(&port), _startIndex(startIndex), _numValues(numValues), _isFixedSize(true) {}

        Port::PortType Type() const { return _referencedPort->Type(); }

        size_t Size() const
        {
            if (_isFixedSize)
            {
                return _referencedPort->Size();
            }
            else
            {
                return _numValues;
            }
        }

        const Port* ReferencedPort() const { return _referencedPort; }

    private:
        friend class InputPort;
        const Port* _referencedPort;
        size_t _startIndex;
        size_t _numValues;
        bool _isFixedSize;
    };

    template <typename ValueType>
    class TypedRange : public InputRange
    {
    public:
        TypedRange(const OutputPort<ValueType>& port) : InputRange(port) {}
        TypedRange(const OutputPort<ValueType>& port, size_t startIndex) : InputRange(port, startIndex) {}
        TypedRange(const OutputPort<ValueType>& port, size_t startIndex, size_t numValues) : InputRange(port, startIndex, numValues) {}
    };

    class InputRanges
    {
    public:
        InputRanges(const InputRange& range)
        {
            _ranges.push_back(range);
            ComputeSize();
        }
        InputRanges(const std::vector<InputRange>& ranges)
        {
            _ranges.insert(_ranges.end(), ranges.begin(), ranges.end());
            ComputeSize();
        }

        std::vector<InputRange>::const_iterator begin() const { return _ranges.cbegin(); }
        std::vector<InputRange>::const_iterator end() const { return _ranges.cend(); }

        size_t Size() const { return _size; }

    protected:
        InputRanges(){};
        void ComputeSize()
        {
            _size = 0;
            for (const auto& range : _ranges)
            {
                _size += range.Size();
            }
        }
        std::vector<InputRange> _ranges;
        size_t _size = 0;
    };

    // typed version of range
    template <typename ValueType>
    class TypedRanges : public InputRanges
    {
    public:
        TypedRanges(const OutputPort<ValueType>& port) : InputRanges(port) {}
        TypedRanges(const TypedRange<ValueType>& range) : InputRanges(range) {}
        TypedRanges(const std::initializer_list<TypedRange<ValueType>>& ranges)
        {
            for (const auto& range : ranges)
            {
                _ranges.push_back(range);
            }
            ComputeSize();            
        }
        
        TypedRanges(const std::vector<TypedRange<ValueType>>& ranges)
        {
            for (const auto& range : ranges)
            {
                _ranges.push_back(range);
            }
            ComputeSize();
        }
    };

    /// <summary> Class representing an input to a node </summary>
    class InputPort : public Port
    {
    public:
        /// <summary> Constructor </summary>
        ///
        /// <param name="output"> The output elements this port receives values from </param>
        template <typename ValueType>
        InputPort(const class Node* owningNode, size_t portIndex, const TypedRange<ValueType>& input);

        template <typename ValueType>
        InputPort(const class Node* owningNode, size_t portIndex, const TypedRanges<ValueType>& input);

        const std::vector<InputRange>& GetInputRanges() const { return _inputRanges; }

        /// <summary> Returns the (already-computed) output value corresponding to this input </summary>
        template <typename ValueType>
        std::vector<ValueType> GetValue() const;

    private:
        std::vector<InputRange> _inputRanges;
    };

    // TODO: if we want to allow functions/nodes to gather values from arbitrary collections of output elements, then
    //       we'll probablay want to make InputPort not be a subclass of Port, but for it to contain a vector of
    //       (Node*, outputIndex, start, len) items.
    //       We'll have to replace `GetNode()` with `GetNodes()` and maybe just remove `OutputIndex()`.
}

#include "../tcc/InputPort.tcc"
