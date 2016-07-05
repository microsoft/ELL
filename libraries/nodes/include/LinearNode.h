////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     LinearNode.h (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Node.h"
#include "ModelGraph.h"
#include "ModelTransformer.h"

// stl
#include <string>

namespace nodes
{
    /// <summary> A feature that takes a vector input and returns its mean over some window of time </summary>
    template <typename ValueType>
    class LinearNode : public model::Node
    {
    public:
        /// <summary> Constructor </summary>
        LinearNode(const model::OutputPortElementList<ValueType>& input);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "LinearNode"; }

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

        // Inputs
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

        // Parameters
        std::vector<ValueType> _weights;
        ValueType _bias;
    };
}

#include "../tcc/LinearNode.tcc"