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
    /// <summary> A node that represents a linear predictor. </summary>
    class LinearPredictorNode : public model::Node
    {
    public:
        static constexpr char* inputPortName = "input";

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The signal to predict from </param>
        /// <param name="predictor"> The linear predictor to use when making the prediction. </param>
        LinearPredictorNode(const model::OutputPortElements<double>& input, const predictors::LinearPredictor& predictor);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "LinearPredictorNode"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Exposes the output port as a read-only property </summary>
        const model::OutputPort<double>& output = _prediction;
        static constexpr char* outputPortName = "prediction";

        /// <summary> Makes a copy of this node in the graph being constructed by the transformer </summary>
        virtual void Copy(model::ModelTransformer& transformer) const override;

        /// <summary> Refines this node in the graph being constructed by the transformer </summary>
        virtual void Refine(model::ModelTransformer& transformer) const override;

    protected:
        virtual void Compute() const override;

    private:
        // Inputs
        model::InputPort<double> _input;

        // Output
        model::OutputPort<double> _prediction;

        // Parameters
        predictors::LinearPredictor _predictor;
    };

    /// <summary> A struct that represents the outputs of a linear predictor node. </summary>
    struct LinearPredictorNodeOutputs
    {
        const model::OutputPort<double>& output;
    };

    /// <summary> Builds a part of the model that represents a linear predictor. </summary>
    ///
    /// <param name="model"> [in,out] The model being modified. </param>
    /// <param name="outputPortElements"> The output port elements from which the linear predictor takes its inputs. </param>
    /// <param name="predictor"> The linear predictor. </param>
    ///
    /// <returns> The LinearPredictorNodeOutputs. </returns>
    LinearPredictorNodeOutputs BuildSubModel(const predictors::LinearPredictor& predictor, model::Model& model, const model::OutputPortElements<double>& outputPortElements);
}
