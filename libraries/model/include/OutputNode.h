////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     OutputNode.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Node.h"
#include "OutputPort.h"
#include "ModelTransformer.h"

// utilities
#include "ObjectDescription.h"

// stl
#include <vector>
#include <memory>
#include <string>

/// <summary> model namespace </summary>
namespace model
{
    /// <summary> A node that represents an output from the system. </summary>
    template <typename ValueType>
    class OutputNode : public Node
    {
    public:
        /// <summary> Default Constructor </summary>
        OutputNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The node to get the input data from </param>
        OutputNode(const model::PortElements<ValueType>& input);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("OutputNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Adds an object's properties to an ObjectDescription </summary>
        ///
        /// <param name="description"> The ObjectDescription for the object </param>
        virtual void AddProperties(utilities::Archiver& description) const override;

        /// <summary> Sets the internal state of the object according to the description passed in </summary>
        ///
        /// <param name="description"> The `ObjectDescription` to get state from </param>
        virtual void SetObjectState(const utilities::Archiver& description, utilities::SerializationContext& context) override;

        /// <summary> Exposes the output port as a read-only property </summary>
        const OutputPort<ValueType>& output = _output;

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        virtual void Copy(ModelTransformer& transformer) const override;

        static constexpr const char* inputPortName = "input";
        static constexpr const char* outputPortName = "output";

    protected:
        virtual void Compute() const override;

    private:
        InputPort<ValueType> _input;
        OutputPort<ValueType> _output;
    };
}

#include "../tcc/OutputNode.tcc"
