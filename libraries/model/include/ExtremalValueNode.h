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

// utilities
#include "TypeName.h"

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
        ExtremalValueNode(const OutputPortElements<ValueType>& input);

        /// <summary> Exposes the extremal value port as a read-only property </summary>
        const OutputPort<ValueType>& val = _val;

        /// <summary> Exposes the extremal value index port as a read-only property </summary>
        const OutputPort<int>& argVal = _argVal;

        static constexpr const char* inputPortName = "input";
        static constexpr const char* valPortName = "val";
        static constexpr const char* argValPortName = "argVal";
         
    protected:
        virtual void Compute() const override;

        // My inputs
        InputPort<ValueType> _input;

        // My outputs
        OutputPort<ValueType> _val;
        OutputPort<int> _argVal;
    };

    /// <summary> ArgMin node subclass </summary>
    template <typename ValueType>
    class ArgMinNode : public ExtremalValueNode<ValueType, false>
    {
    public:
        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The node to get the input data from </param>
        ArgMinNode(const OutputPortElements<ValueType>& input) : ExtremalValueNode<ValueType, false>(input) {}

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("ArgMinNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the graph being constructed by the transformer </summary>
        virtual void Copy(ModelTransformer& transformer) const override;
    };

    /// <summary> ArgMax node subclass </summary>
    template <typename ValueType>
    class ArgMaxNode : public ExtremalValueNode<ValueType, true>
    {
    public:
        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The node to get the input data from </param>
        ArgMaxNode(const OutputPortElements<ValueType>& input) : ExtremalValueNode<ValueType, true>(input) {}

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("ArgMaxNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the graph being constructed by the transformer </summary>
        virtual void Copy(ModelTransformer& transformer) const override;
    };
}

#include "../tcc/ExtremalValueNode.tcc"
