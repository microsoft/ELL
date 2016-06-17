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
        template <typename ValueType>
        InputRange(const OutputPort<ValueType>& port);

        template <typename ValueType>
        InputRange(const OutputPort<ValueType>& port, size_t index);

        template <typename ValueType>
        InputRange(const OutputPort<ValueType>& port, size_t index, size_t numValues);

        Port::PortType Type() const { return referencedPort->Type(); }

        size_t Size() const
        {
            if (isFixedSize)
            {
                return referencedPort->Size();
            }
            else
            {
                return numValues;
            }
        }

        const Port* referencedPort;
        size_t startIndex;
        size_t numValues;
        bool isFixedSize;
    };

    typedef std::vector<InputRange> InputRangeList;

    /// <summary> Class representing an input to a node </summary>
    class InputPort : public Port
    {
    public:
        /// <summary> Constructor </summary>
        ///
        /// <param name="output"> The output port this port receives values from </param>
        template <typename ValueType>
        InputPort(const class Node* owningNode, size_t portIndex, const OutputPort<ValueType>* output);

        InputPort(const class Node* owningNode, size_t portIndex, const InputRange& inputRange);

        InputPort(const class Node* owningNode, size_t portIndex, const std::vector<InputRange>& inputRanges);

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
