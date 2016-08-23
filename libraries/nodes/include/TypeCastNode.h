////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     TypeCastNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Node.h"
#include "OutputPort.h"
#include "ModelTransformer.h"

#include <vector>
#include <memory>
#include <string>

/// <summary> nodes namespace </summary>
namespace nodes
{
    /// <summary> A node that represents an output from the system. </summary>
    template <typename InputValueType, typename OutputValueType>
    class TypeCastNode : public model::Node
    {
    public:
        /// <summary> Default Constructor </summary>
        TypeCastNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The node to get the input data from </param>
        TypeCastNode(const model::PortElements<InputValueType>& input);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<InputValueType, OutputValueType>("TypeCastNode"); }

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

        /// <summary> Exposes the output port as a read-only property </summary>
        const model::OutputPort<OutputValueType>& output = _output;

        /// <summary> Makes a copy of this node in the graph being constructed by the transformer </summary>
        virtual void Copy(model::ModelTransformer& transformer) const override;

        static constexpr const char* inputPortName = "input";
        static constexpr const char* outputPortName = "output";

    protected:
        virtual void Compute() const override;

    private:
        model::InputPort<InputValueType> _input;
        model::OutputPort<OutputValueType> _output;
    };
}

#include "../tcc/TypeCastNode.tcc"
