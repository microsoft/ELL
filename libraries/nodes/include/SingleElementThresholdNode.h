////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SingleElementThresholdNode.h (nodes)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "Model.h"
#include "ModelTransformer.h"
#include "Node.h"

// predictors
#include "SingleElementThresholdPredictor.h"

namespace ell
{
namespace nodes
{
    /// <summary> A node that represents a single-element threshold predictor. </summary>
    class SingleElementThresholdNode : public model::Node
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* inputPortName = "input";
        static constexpr const char* outputPortName = "output";
        const model::InputPort<double>& input = _input;
        const model::OutputPort<bool>& output = _output;
        /// @}

        using SingleElementThresholdPredictor = predictors::SingleElementThresholdPredictor;

        /// <summary> Default Constructor </summary>
        SingleElementThresholdNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The signal to predict from </param>
        /// <param name="predictor"> The linear predictor to use when making the prediction. </param>
        SingleElementThresholdNode(const model::PortElements<double>& input, const SingleElementThresholdPredictor& predictor);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "SingleElementThresholdNode"; }

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

        // Output
        model::OutputPort<bool> _output;

        // Linear predictor
        SingleElementThresholdPredictor _predictor;
    };

    /// <summary> Adds a SingleElementThreshold predictor node to a model transformer. </summary>
    ///
    /// <param name="input"> The input to the predictor. </param>
    /// <param name="predictor"> The SingleElementThreshold predictor. </param>
    /// <param name="transformer"> [in,out] The model transformer. </param>
    ///
    /// <returns> The node added to the model. </returns>
    SingleElementThresholdNode* AddNodeToModelTransformer(const model::PortElements<double>& input, const predictors::SingleElementThresholdPredictor& predictor, model::ModelTransformer& transformer);
}
}
