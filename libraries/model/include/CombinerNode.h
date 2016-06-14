////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     CombinerNode.h (model)
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
    /// <summary> An node that selects values from a number of inputs </summary>
    template <typename ValueType>
    class CombinerNode : public Node
    {
    public:
        struct InputRange
        {
            const OutputPort<ValueType>& port;
            size_t startIndex;
            size_t numValues;
        };

        /// <summary> Constructor </summary>
        ///
        /// <param name="inputs"> A vector of port ranges that represent the inputs to this node </param>
        /// <param name="value2"> The input to return if the condition is `true` </param>
        /// <param name="value2"> The input to return if the condition is `false` </param>
        CombinerNode(const std::vector<InputRange>& inputs);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "Combiner"; }

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
        std::vector<InputRange> _inputs;

        // Output
        OutputPort<ValueType> _output;
    };
}

#include "../tcc/CombinerNode.tcc"
