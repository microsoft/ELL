////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ValueSelectorNode.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Node.h"
#include "InputPort.h"
#include "OutputPort.h"

#include <vector>
#include <memory>
#include <exception>

/// <summary> model namespace </summary>
namespace model
{
    /// <summary> An example node that selects from one of two input values depending on a third input </summary>
    template <typename ValueType>
    class ValueSelectorNode : public Node
    {
    public:
        /// <summary> Constructor </summary>
        ///
        /// <param name="condition"> An input that returns a single boolean value that selects which input to use as output </param>
        /// <param name="value2"> The input to return if the condition is `true` </param>
        /// <param name="value2"> The input to return if the condition is `false` </param>
        ValueSelectorNode(const OutputPortElementList<bool>& condition, const OutputPortElementList<ValueType>& value1, const OutputPortElementList<ValueType>& value2);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "ValueSelector"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Exposes the output port as a read-only property </summary>
        const OutputPort<ValueType>& output = _output;

    protected:
        virtual void Compute() const override;

    private:
        // Inputs
        InputPort<bool> _condition;
        InputPort<ValueType> _value1;
        InputPort<ValueType> _value2;

        // Output
        OutputPort<ValueType> _output;
    };
}

#include "../tcc/ValueSelectorNode.tcc"
