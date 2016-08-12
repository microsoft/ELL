////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DelayNode.h (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "Node.h"
#include "ModelTransformer.h"
#include "OutputPortElements.h"
#include "InputPort.h"
#include "OutputPort.h"

// utilities
#include "TypeName.h"

// stl
#include <string>
#include <vector>

namespace nodes
{
    /// <summary> A feature that takes a signal and returns a delayed sample of the signal </summary>
    template <typename ValueType>
    class DelayNode : public model::Node
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* outputPortName = "output";
        static constexpr const char* inputPortName = "input";
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        DelayNode();

        /// <summary> Constructor </summary>
        /// <param name="input"> The signal to delay </param>
        /// <param name="windowSize"> The number of samples to delay the signal </param>
        DelayNode(const model::OutputPortElements<ValueType>& input, size_t windowSize);
        
        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("DelayNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Writes to a Serializer. </summary>
        ///
        /// <param name="serializer"> The serializer. </param>
        virtual void Serialize(utilities::Serializer& serializer) const override;

        /// <summary> Reads from a Deserializer. </summary>
        ///
        /// <param name="deserializer"> The deserializer. </param>
        /// <param name="context"> The serialization context. </param>
        virtual void Deserialize(utilities::Deserializer& serializer, utilities::SerializationContext& context) override;

        /// <summary> Makes a copy of this node in the graph being constructed by the transformer </summary>
        virtual void Copy(model::ModelTransformer& transformer) const override;

    protected:
        virtual void Compute() const override;

    private:
        // Inputs
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

        // Buffer
        mutable std::vector<std::vector<ValueType>> _samples;
        size_t _windowSize;
    };
}

#include "../tcc/DelayNode.tcc"