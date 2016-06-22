////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ExtremalValueNode.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Node.h"
#include "InputPort.h"
#include "OutputPort.h"

#include <vector>
#include <memory>
#include <algorithm>

/// <summary> model namespace </summary>
namespace model
{
    /// <summary> An example node that computes an extremal value (min or max) of its input, as well as the index of the extremal value. </summary>
    template <typename ValueType, bool max>
    class ExtremalValueNode : public Node
    {
    public:
        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The node to get the input data from </param>
        //ExtremalValueNode(const OutputPort<ValueType>& input);
        ExtremalValueNode(const TypedOutputRef<ValueType>& input);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return max ? "ArgMax" : "ArgMin"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Exposes the extremal value port as a read-only property </summary>
        const OutputPort<ValueType>& val = _val;

        /// <summary> Exposes the extremal value index port as a read-only property </summary>
        const OutputPort<int>& argVal = _argVal;

    protected:
        virtual void Compute() const override;

    private:
        // My inputs
        InputPort _input;

        // My outputs
        OutputPort<ValueType> _val;
        OutputPort<int> _argVal;
    };

    /// <summary> Type alias for argmin node </summary>
    template <typename ValueType>
    using ArgMinNode = ExtremalValueNode<ValueType, false>;

    /// <summary> Type alias for argmax node </summary>
    template <typename ValueType>
    using ArgMaxNode = ExtremalValueNode<ValueType, true>;
}

#include "../tcc/ExtremalValueNode.tcc"
