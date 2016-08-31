////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     InputNode.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Node.h"
#include "OutputPort.h"
#include "ModelTransformer.h"

// utilities
#include "TypeName.h"
#include "ObjectDescription.h"

#include <vector>
#include <memory>
#include <string>

/// <summary> model namespace </summary>
namespace model
{
    /// <summary> A node that represents an input to the system. </summary>
    template <typename ValueType>
    class InputNode : public Node
    {
    public:
        /// <summary> Default Constructor </summary>
        InputNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="dimension"> The input dimension </param>
        InputNode(size_t dimension);

        /// <summary> Sets the value output by this scalar node </summary>
        ///
        /// <param name="inputValues"> The value for this node to output </param>
        void SetInput(ValueType inputValue);

        /// <summary> Sets the value output by this node </summary>
        ///
        /// <param name="inputValues"> The values for this node to output </param>
        void SetInput(std::vector<ValueType> inputValues);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("InputNode"); }

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
        virtual void Copy(ModelTransformer& transformer) const override;

        /// <summary> Exposes the output port as a read-only property </summary>
        const OutputPort<ValueType>& output = _output;

        static constexpr const char* outputPortName = "output";

    protected:
        virtual void Compute() const override;

    private:
        std::vector<ValueType> _inputValues;
        OutputPort<ValueType> _output;
    };
}

#include "../tcc/InputNode.tcc"
