////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     LinearPredictorNode.h (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Node.h"
#include "ModelGraph.h"
#include "ModelTransformer.h"

// predictors
#include "LinearPredictor.h"

// stl
#include <string>

namespace nodes
{
    /// <summary> A feature that takes a vector input and returns its mean over some window of time </summary>
    class LinearPredictorNode : public model::Node
    {
    public:
        /// <summary> Constructor </summary>
        LinearPredictorNode(const model::OutputPortElementList<double>& input, const predictors::LinearPredictor& predictor);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "LinearNode"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Exposes the output port as a read-only property </summary>
        const model::OutputPort<double>& output = _output;

        virtual void Copy(model::ModelTransformer& transformer) const override;

    protected:
        virtual void Compute() const override;

    private:

        // Inputs
        model::InputPort<double> _input;

        // Output
        model::OutputPort<double> _output;

        // Parameters
        predictors::LinearPredictor _predictor;
    };
}
