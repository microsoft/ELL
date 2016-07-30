////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     UnaryOperationNode.h (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Node.h"
#include "ModelTransformer.h"
#include "OutputPortElements.h"
#include "InputPort.h"
#include "OutputPort.h"

// utilities
#include "TypeName.h"

// stl
#include <string>
#include <cmath>
#include <vector>

namespace nodes
{
    /// <summary> A node that represents a unary function of its input </summary>
    template <typename ValueType>
    class UnaryOperationNode : public model::Node
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* inputPortName = "input";
        static constexpr const char* outputPortName = "output";
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Types of unary operations supported by this node type. </summary>
        enum class OperationType
        {
            sqrt
        };

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The signal to process. </param>
        /// <param name="operation"> The function to use to process the signal. </param>
        UnaryOperationNode(const model::OutputPortElements<ValueType>& input, OperationType operation);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("UnaryOperationNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the graph being constructed by the transformer </summary>
        virtual void Copy(model::ModelTransformer& transformer) const override;

    protected:
        virtual void Compute() const override;

    private:
        template <typename Operation>
        std::vector<ValueType> ComputeOutput(Operation&& fn) const;
 
        // Inputs
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

        // Operation
        OperationType _operation;

    };
}

#include "../tcc/UnaryOperationNode.tcc"