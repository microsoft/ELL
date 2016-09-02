////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ExtremalValueNode.h (nodes)
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

/// <summary> nodes namespace </summary>
namespace nodes
{
    /// <summary> An example node that computes an extremal value (min or max) of its input, as well as the index of the extremal value. </summary>
    template <typename ValueType, bool max>
    class ExtremalValueNode : public model::Node
    {
    public:
        /// <summary> Default Constructor </summary>    
        ExtremalValueNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The node to get the input data from </param>
        ExtremalValueNode(const model::PortElements<ValueType>& input);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName();

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Adds an object's properties to an `Archiver` </summary>
        ///
        /// <param name="archiver"> The `Archiver` to add the values from the object to </param>
        virtual void Serialize(utilities::Archiver& archiver) const override;

        /// <summary> Sets the internal state of the object according to the archiver passed in </summary>
        ///
        /// <param name="archiver"> The `Archiver` to get state from </param>
        virtual void Deserialize(utilities::Unarchiver& archiver) override;

        /// <summary> Exposes the extremal value port as a read-only property </summary>
        const model::OutputPort<ValueType>& val = _val;

        /// <summary> Exposes the extremal value index port as a read-only property </summary>
        const model::OutputPort<int>& argVal = _argVal;

        static constexpr const char* inputPortName = "input";
        static constexpr const char* valPortName = "val";
        static constexpr const char* argValPortName = "argVal";
         
    protected:
        virtual void Compute() const override;

        // My inputs
        model::InputPort<ValueType> _input;

        // My outputs
        model::OutputPort<ValueType> _val;
        model::OutputPort<int> _argVal;
    };

    /// <summary> ArgMin node subclass </summary>
    template <typename ValueType>
    class ArgMinNode : public ExtremalValueNode<ValueType, false>
    {
    public:
        /// <summary> Default Constructor </summary>    
        ArgMinNode() : ExtremalValueNode<ValueType, false>() {}

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The node to get the input data from </param>
        ArgMinNode(const model::PortElements<ValueType>& input) : ExtremalValueNode<ValueType, false>(input) {}

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("ArgMinNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        virtual void Copy(model::ModelTransformer& transformer) const override;
    };

    /// <summary> ArgMax node subclass </summary>
    template <typename ValueType>
    class ArgMaxNode : public ExtremalValueNode<ValueType, true>
    {
    public:
        /// <summary> Default Constructor </summary>    
        ArgMaxNode() : ExtremalValueNode<ValueType, true>() {}

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The node to get the input data from </param>
        ArgMaxNode(const model::PortElements<ValueType>& input) : ExtremalValueNode<ValueType, true>(input) {}

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("ArgMaxNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        virtual void Copy(model::ModelTransformer& transformer) const override;

    };
}

#include "../tcc/ExtremalValueNode.tcc"
