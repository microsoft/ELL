////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ElementSelectorNode.h (node)
//  Authors:  Chuck Jacobs
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
namespace model
{
    /// <summary> A node that outputs a dynamically specified element from an input array. </summary>
    template <typename ValueType>
    class ElementSelectorNode : public model::Node
    {
    public:
        /// <summary> Input port name. </summary>
        static constexpr char* input1PortName = "input";

        /// <summary> Input port name. </summary>
        static constexpr char* selectorPortName = "selector";

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The input aray of values. </param>
        /// <param name="selector"> The index of the chosen element </param>
        ElementSelectorNode(const model::OutputPortElements<ValueType>& input, const model::OutputPortElements<int>& selector);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("ElementSelectorNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Exposes the output port as a read-only property </summary>
        const OutputPort<ValueType>& output = _output;
        
        /// <summary> Output port name. </summary>
        static constexpr char* outputPortName = "output";

        /// <summary> Makes a copy of this node in the graph being constructed by the transformer </summary>
        virtual void Copy(model::ModelTransformer& transformer) const override;

    protected:
        virtual void Compute() const override;

    private:
        // Inputs
        model::InputPort<ValueType> _input;
        model::InputPort<int> _selector;

        // Output
        model::OutputPort<ValueType> _output;
    };
}

#include "../tcc/ElementSelectorNode.tcc"
