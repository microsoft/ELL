////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MultiplexorNode.h (node)
//  Authors:  ChuckJacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

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
    /// <summary> A node that routes its input to one of its outputs, depending on a separate selector input. When `selector` is false, the input is routed
    /// to output1, and when `selector` is true, it is routed to output2. </summary>
    template <typename ValueType>
    class BinaryMultiplexorNode : public model::Node
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* inputPortName = "input";
        static constexpr const char* selectorPortName = "selector";
        static constexpr const char* output1PortName = "output1";
        static constexpr const char* output2PortName = "output2";
        const model::OutputPort<ValueType>& output1 = _output1;
        const model::OutputPort<ValueType>& output2 = _output2;
        /// @}

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The input value. </param>
        /// <param name="selector"> The index of the chosen element to recieve the value </param>
        BinaryMultiplexorNode(const model::PortElements<ValueType>& input, const model::PortElements<bool>& selector, ValueType defaultValue=0);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("BinaryMultiplexorNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the graph being constructed by the transformer </summary>
        virtual void Copy(model::ModelTransformer& transformer) const override;

    protected:
        virtual void Compute() const override;

    private:
        // Inputs
        model::InputPort<ValueType> _input;
        model::InputPort<bool> _selector;

        // Output
        model::OutputPort<ValueType> _output1;
        model::OutputPort<ValueType> _output2;

        // Default value
        std::vector<ValueType> _defaultValue;
    };
}

#include "../tcc/BinaryMultiplexorNode.tcc"
