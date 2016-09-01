////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     AccumulatorNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "Node.h"
#include "Model.h"
#include "ModelTransformer.h"

// utilities
#include "Exception.h"
#include "TypeName.h"
#include "ObjectDescription.h"

// stl
#include <string>
#include <vector>

namespace nodes
{
    /// <summary> A node that accumulates a running sum of its input. </summary>
    template <typename ValueType>
    class AccumulatorNode : public model::Node
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* inputPortName = "input";
        static constexpr const char* outputPortName = "output";
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        AccumulatorNode();

        /// <summary> Constructor </summary>
        /// <param name="input"> The signal to accumulate </param>
        AccumulatorNode(const model::PortElements<ValueType>& input);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("AccumulatorNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Gets an ObjectDescription for the type </summary>
        ///
        /// <returns> An ObjectDescription for the type </returns>
        static utilities::ObjectDescription GetTypeDescription();

        /// <summary> Gets an ObjectDescription for the object </summary>
        ///
        /// <returns> An ObjectDescription for the object </returns>
        virtual utilities::ObjectDescription GetDescription() const override;

        /// <summary> Sets the internal state of the object according to the description passed in </summary>
        ///
        /// <param name="description"> The `ObjectDescription` to get state from </param>
        virtual void SetObjectState(const utilities::ObjectDescription& description, utilities::SerializationContext& context) override;

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        virtual void Copy(model::ModelTransformer& transformer) const override;

    protected:
        virtual void Compute() const override;

    private:
        // Input
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

        // Buffer
        mutable std::vector<ValueType> _accumulator;
    };
}

#include "../tcc/AccumulatorNode.tcc"