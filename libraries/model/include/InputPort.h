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
#include "OutputPortElementList.h"

#include <vector>
#include <cassert>

/// <summary> model namespace </summary>
namespace model
{
    /// <summary> Class representing an input to a node </summary>
    class InputPortBase : public Port
    {
    public:
        /// <summary> Constructor </summary>
        ///
        /// <param name="owningNode"> The node that contains this port </param>
        /// <param name="input"> The input group to fetch input values from </param>
        template <typename ValueType>
        InputPortBase(const class Node* owningNode, const OutputPortElementList<ValueType>& input);

        /// <summary> Returns the OutputPortElements containing the referenced locations to get values from </summary>
        ///
        /// <returns> The OutputPortElements containing the referenced locations to get values from </returns>
        const std::vector<OutputPortElement>& GetOutputPortElements() const { return _inputElements; }

        /// <summary> Returns an OutputPortElement containing the referenced location to get the value for a specific input element from </summary>
        ///
        /// <param name="index"> The index of the element </param>
        /// <returns> The OutputPortElement containing the referenced location to get the value from </returns>
        const OutputPortElement& GetOutputPortElement(size_t index) const { return _inputElements[index]; }

        /// <summary> Returns the list nodes this input port gets values from </summary>
        ///
        /// <returns> The list nodes this input port gets values from </returns>
        const std::vector<const Node*>& GetInputNodes() const { return _inputNodes; }

        const OutputPortElementListUntyped& GetInputRanges() const { return _inputRanges; }

        /// <summary> The dimensionality of the output </summary>
        ///
        /// <returns> The dimensionality of the output </returns>
        size_t Size() const { return _inputElements.size(); }

        /// <summary> Returns the (already-computed) output value corresponding to this input </summary>
        ///
        /// <returns> The (already-computed) output value corresponding to this input </returns>
        template <typename ValueType>
        std::vector<ValueType> GetTypedValue() const;

        /// <summary> Returns an element from the (already-computed) output value corresponding to this input </summary>
        ///
        /// <param name="index"> The index of the element to return </param>
        /// <returns> The output value at the corresponding index </returns>
        template <typename ValueType>
        ValueType GetTypedValue(size_t index) const;

    private:
        OutputPortElementListUntyped _inputRanges;
        std::vector<OutputPortElement> _inputElements;
        std::vector<const Node*> _inputNodes;
    };

    template <typename ValueType>
    class InputPort : public InputPortBase
    {
    public:
        /// <summary> Creates an input port </summary>
        ///
        /// <param name="owningNode"> The node this port belongs to </param>
        /// <param name="input"> A reference to the output port(s) this input port is consuming from </param>
        InputPort(const class Node* owningNode, const OutputPortElementList<ValueType>& input);

        /// <summary> Returns the (already-computed) output value corresponding to this input </summary>
        ///
        /// <returns> The (already-computed) output value corresponding to this input </returns>
        std::vector<ValueType> GetValue() const { return GetTypedValue<ValueType>(); }

        /// <summary> Returns an element from the (already-computed) output value corresponding to this input </summary>
        ///
        /// <param name="index"> The index of the element to return </param>
        /// <returns> The output value at the corresponding index </returns>
        ValueType GetValue(size_t index) const;

        /// <summary> Returns an element from the (already-computed) output value corresponding to this input </summary>
        ///
        /// <param name="index"> The index of the element to return </param>
        /// <returns> The output value at the corresponding index </returns>
        ValueType operator[](size_t index) const;
    };

    template <typename ValueType>
    OutputPortElementList<ValueType> CopyInputPort(const InputPort<ValueType>& input, std::unordered_map<const Port*, Port*>& portMap)
    {
        const auto& ranges = input.GetInputRanges();
        std::vector<OutputPortElementList<ValueType>> newRanges;
        for (const auto& range : ranges)
        {
            auto oldPort = range.ReferencedPort();
            auto newPort = portMap[oldPort];
            auto outputPort = dynamic_cast<const OutputPort<ValueType>*>(newPort);
            auto start = range.GetStartIndex();
            auto size = range.Size();
            newRanges.emplace_back(*outputPort, start, size);
        }
        return OutputPortElementList<ValueType>(newRanges);
    }
}

#include "../tcc/InputPort.tcc"
