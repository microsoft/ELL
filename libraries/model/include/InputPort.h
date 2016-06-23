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

        /// <summary> Returns the UntypedOutputRef containing the ranges of referenced locations to get values from </summary>
        ///
        /// <returns> The OutputRef containing the ranges of referenced locations to get values from </returns>
        const UntypedOutputRef& GetInputRanges() const { return _inputRanges; }

        /// <summary> The dimensionality of the output </summary>
        ///
        /// <returns> The dimensionality of the output </returns>
        size_t Size() const { return _inputRanges.Size(); }

        /// <summary> Returns the (already-computed) output value corresponding to this input </summary>
        ///
        /// <returns> The (already-computed) output value corresponding to this input </returns>
        template <typename ValueType>
        std::vector<ValueType> GetTypedValue() const;

    private:
        UntypedOutputRef _inputRanges;
    };

    template <typename ValueType>
    class InputPort : public InputPortBase
    {
    public:
        InputPort(const class Node* owningNode, size_t portIndex, const OutputRef<ValueType>& input);
        std::vector<ValueType> GetValue() const { return GetTypedValue<ValueType>(); }
    };
}

#include "../tcc/InputPort.tcc"
