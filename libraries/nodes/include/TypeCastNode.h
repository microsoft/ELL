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

        /// <summary> Adds an object's properties to an `Archiver` </summary>
        ///
        /// <param name="archiver"> The `Archiver` to add the values from the object to </param>
        virtual void AddProperties(utilities::Archiver& archiver) const override;

        /// <summary> Sets the internal state of the object according to the archiver passed in </summary>
        ///
        /// <param name="archiver"> The `Archiver` to get state from </param>
        virtual void SetObjectState(const utilities::Archiver& archiver, utilities::SerializationContext& context) override;

        /// <summary> Exposes the output port as a read-only property </summary>
        const model::OutputPort<OutputValueType>& output = _output;

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
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
