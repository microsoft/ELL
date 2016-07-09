////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BinaryOperationNode.h (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Node.h"
#include "ModelGraph.h"
#include "ModelTransformer.h"

// utilities
#include "Exception.h"

// stl
#include <string>
#include <vector>

namespace nodes
{
    /// <summary> A node that performs a binary arithmetic operation on its inputs </summary>
    template <typename ValueType>
    class BinaryOperationNode : public model::Node
    {
    public:
        enum class OperationType
        {
            add,
            subtract,
            multiply,
            divide
        };

        /// <summary> Constructor </summary>
        /// <param name="input1"> The left-hand input of the arithmetic expression </param>
        /// <param name="input2"> The right-hand input of the arithmetic expression </param>
        BinaryOperationNode(const model::OutputPortElementList<ValueType>& input1, const model::OutputPortElementList<ValueType>& input2, OperationType operation);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "BinaryOperationNode"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Exposes the output port as a read-only property </summary>
        const model::OutputPort<ValueType>& output = _output;

        virtual void Copy(model::ModelTransformer& transformer) const override;

    protected:
        virtual void Compute() const override;

    private:
        template <typename Operation>
        std::vector<ValueType> ComputeOutput(Operation&& fn) const;

        OperationType _operation;

        // Inputs
        model::InputPort<ValueType> _input1;
        model::InputPort<ValueType> _input2;

        // Output
        model::OutputPort<ValueType> _output;
    };
}

#include "../tcc/BinaryOperationNode.tcc"