////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DotProductNode.h (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SumNode.h"

// model
#include "Node.h"
#include "ModelTransformer.h"
#include "PortElements.h"
#include "InputPort.h"
#include "OutputPort.h"
#include "BinaryOperationNode.h"

// utilities
#include "TypeName.h"

// stl
#include <string>

namespace nodes
{
    /// <summary> A feature that takes two vector inputs and returns their dot product </summary>
    template <typename ValueType>
    class DotProductNode : public model::Node
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* input1PortName = "input1";
        static constexpr const char* input2PortName = "input2";
        static constexpr const char* outputPortName = "output";
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        DotProductNode();

        /// <summary> Constructor </summary>
        /// <param name="input1"> One of the signals to take the dot product of </param>
        /// <param name="input2"> The other signal to take the dot product of </param>
        DotProductNode(const model::PortElements<ValueType>& input1, const model::PortElements<ValueType>& input2);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("DotProductNode"); }

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
        
        /// <summary> Refines this node in the graph being constructed by the transformer </summary>
        virtual void Refine(model::ModelTransformer& transformer) const override;

    protected:
        virtual void Compute() const override;

    private:
        // Inputs
        model::InputPort<ValueType> _input1;
        model::InputPort<ValueType> _input2;

        // Output
        model::OutputPort<ValueType> _output;
    };
}

#include "../tcc/DotProductNode.tcc"