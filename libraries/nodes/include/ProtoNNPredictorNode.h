////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ProtoNNPredictorNode.h (nodes)
//  Authors:  Suresh Iyengar
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "Model.h"
#include "ModelTransformer.h"
#include "Node.h"

// predictors
#include "ProtoNNPredictor.h"

// stl
#include <string>

namespace ell
{
namespace nodes
{
    /// <summary> A node that represents a ProtoNN predictor. </summary>
    class ProtoNNPredictorNode : public model::Node
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* inputPortName = "input";
        static constexpr const char* outputScorePortName = "outputScore";
        static constexpr const char* outputLabelPortName = "outputLabel";
        const model::InputPort<double>& input = _input;
        const model::OutputPort<double>& outputScore = _outputScore;
        const model::OutputPort<int>& outputLabel = _outputLabel;

        /// @}

        using ProtoNNPredictor = predictors::ProtoNNPredictor;

        /// <summary> Default Constructor </summary>
        ProtoNNPredictorNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The signal to predict from </param>
        /// <param name="predictor"> The ProtoNN predictor to use when making the prediction. </param>
        ProtoNNPredictorNode(const model::PortElements<double>& input, const ProtoNNPredictor& predictor);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "ProtoNNPredictorNode"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
        virtual void Copy(model::ModelTransformer& transformer) const override;

        /// <summary> Refines this node in the model being constructed by the transformer </summary>
        virtual bool Refine(model::ModelTransformer& transformer) const override;

    protected:
        virtual void Compute() const override;
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        // Inputs
        model::InputPort<double> _input;

        // Output score
        model::OutputPort<double> _outputScore;

        // Output label
        model::OutputPort<int> _outputLabel;

        // ProtoNN predictor
        ProtoNNPredictor _predictor;
    };

    /// <summary> Adds a ProtoNN predictor node to a model transformer. </summary>
    ///
    /// <param name="input"> The input to the predictor. </param>
    /// <param name="predictor"> The ProtoNN predictor. </param>
    /// <param name="transformer"> [in,out] The model transformer. </param>
    ///
    /// <returns> The node added to the model. </returns>
    ProtoNNPredictorNode* AddNodeToModelTransformer(const model::PortElements<double>& input, const predictors::ProtoNNPredictor& predictor, model::ModelTransformer& transformer);
}
}
