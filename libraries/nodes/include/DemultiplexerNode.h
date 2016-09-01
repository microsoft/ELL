////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DemultiplexerNode.h (node)
//  Authors:  ChuckJacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ConstantNode.h"
#include "BinaryPredicateNode.h"
#include "TypeCastNode.h"
#include "MultiplexerNode.h"

// model
#include "Node.h"
#include "InputPort.h"
#include "OutputPort.h"

// utilities
#include "TypeName.h"

// stl
#include <vector>
#include <memory>
#include <exception>

/// <summary> model namespace </summary>
namespace nodes
{
    /// <summary> A node that routes its scalar input to one element of its outputs, depending on a separate selector input. The element at the index
    /// provided by `selector` is set to the input value, and the rest are set to a default value. </summary>
    template <typename ValueType, typename SelectorType>
    class DemultiplexerNode : public model::Node
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* inputPortName = "input";
        static constexpr const char* selectorPortName = "selector";
        static constexpr const char* outputPortName = "output";
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        DemultiplexerNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The input value. </param>
        /// <param name="selector"> The index of the chosen element to recieve the value </param>
        DemultiplexerNode(const model::PortElements<ValueType>& input, const model::PortElements<SelectorType>& selector, size_t outputSize, ValueType defaultValue=0);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType, SelectorType>("DemultiplexerNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Adds an object's properties to an ObjectDescription </summary>
        ///
        /// <param name="description"> The ObjectDescription for the object </param>
        virtual void AddProperties(utilities::ObjectDescription& description) const override;

        /// <summary> Sets the internal state of the object according to the description passed in </summary>
        ///
        /// <param name="description"> The `ObjectDescription` to get state from </param>
        virtual void SetObjectState(const utilities::ObjectDescription& description, utilities::SerializationContext& context) override;

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        virtual void Copy(model::ModelTransformer& transformer) const override;

        /// <summary> Refines this node in the model being constructed by the transformer </summary>
        virtual bool Refine(model::ModelTransformer& transformer) const override;

    protected:
        virtual void Compute() const override;

    private:
        // Inputs
        model::InputPort<ValueType> _input;
        model::InputPort<SelectorType> _selector;

        // Output
        model::OutputPort<ValueType> _output;

        // Default value
        ValueType _defaultValue;
    };
}

#include "../tcc/DemultiplexerNode.tcc"
