////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ForestPredictorNode.h (nodes)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "Model.h"
#include "ModelTransformer.h"
#include "Node.h"

// predictors
#include "ConstantNode.h"
#include "ForestPredictor.h"
#include "SingleElementThresholdPredictor.h"

// stl
#include <string>

namespace ell
{
namespace nodes
{
    /// <summary> Implements a forest node, which wraps the forest predictor. </summary>
    ///
    /// <typeparam name="SplitRuleType"> The split rule type. </typeparam>
    /// <typeparam name="EdgePredictorType"> The edge predictor type. </typeparam>
    template <typename SplitRuleType, typename EdgePredictorType>
    class ForestPredictorNode : public model::Node
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* inputPortName = "input";
        static constexpr const char* outputPortName = "output";
        static constexpr const char* treeOutputsPortName = "treeOutputs";
        static constexpr const char* edgeIndicatorVectorPortName = "edgeIndicatorVector";
        const model::InputPort<double>& input = _input;
        const model::OutputPort<double>& output = _output;
        const model::OutputPort<double>& treeOutputs = _treeOutputs;
        const model::OutputPort<bool>& edgeIndicatorVector = _edgeIndicatorVector;
        /// @}

        using ForestPredictor = predictors::ForestPredictor<SplitRuleType, EdgePredictorType>;

        /// <summary> Default Constructor </summary>
        ForestPredictorNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The predictor's input. </param>
        /// <param name="forest"> The forest predictor. </param>
        ForestPredictorNode(const model::PortElements<double>& input, const ForestPredictor& forest);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<SplitRuleType, EdgePredictorType>("ForestPredictorNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the model being constructed by the transformer. </summary>
        ///
        /// <param name="transformer"> [in,out] The transformer. </param>
        virtual void Copy(model::ModelTransformer& transformer) const override;

        /// <summary> Refines this node in the model being constructed by the transformer </summary>
        virtual bool Refine(model::ModelTransformer& transformer) const override;

    protected:
        virtual void Compute() const override;
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        // Input
        model::InputPort<double> _input;

        // Outputs
        model::OutputPort<double> _output;
        model::OutputPort<double> _treeOutputs;
        model::OutputPort<bool> _edgeIndicatorVector;

        // Forest
        ForestPredictor _forest;
    };

    /// <summary> Defines an alias representing a simple forest node, which holds a forest with a SingleElementThresholdPredictor as the split rule and ConstantPredictors on the edges. </summary>
    typedef ForestPredictorNode<predictors::SingleElementThresholdPredictor, predictors::ConstantPredictor> SimpleForestPredictorNode;
}
}

#include "../tcc/ForestPredictorNode.tcc"
