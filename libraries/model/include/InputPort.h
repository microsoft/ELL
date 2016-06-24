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
#include "OutputRef.h"

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
        /// <param name="portIndex"> The index of this port within the owning node <param>
        /// <param name="input"> The input group to fetch input values from </param>
        template <typename ValueType>
        InputPortBase(const class Node* owningNode, size_t portIndex, const OutputRef<ValueType>& input);

        /// <summary> Returns the ElementRefs containing the referenced locations to get values from </summary>
        ///
        /// <returns> The ElementRefs containing the referenced locations to get values from </returns>
        const std::vector<ElementRef>& GetElementRefs() const { return _inputElements; }

        /// <summary> Returns an ElementRef containing the referenced location to get the value for a specific input element from </summary>
        ///
        /// <param name="index"> The index of the element </param>
        /// <returns> The ElementRef containing the referenced location to get the value from </returns>
        const ElementRef& GetElementRef(size_t index) const { return _inputElements[index]; }

        /// <summary> Returns the ElementRefs containing the referenced locations to get values from </summary>
        ///
        /// <returns> The ElementRefs containing the referenced locations to get values from </returns>
        const std::vector<const Node*>& GetInputNodes() const { return _inputNodes; }

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
        std::vector<ElementRef> _inputElements;
        std::vector<const Node*> _inputNodes;
    };

    template <typename ValueType>
    class InputPort : public InputPortBase
    {
    public:
        /// <summary> Creates an input port </summary>
        ///
        /// <param name="owningNode"> The node this port belongs to </param>
        /// <param name="portIneex"> The index of this port within the owning node </param>
        /// <param name="input"> A reference to the output port(s) this input port is consuming from </param>
        InputPort(const class Node* owningNode, size_t portIndex, const OutputRef<ValueType>& input);

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
}

#include "../tcc/InputPort.tcc"
