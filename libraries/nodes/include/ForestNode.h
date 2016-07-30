////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ForestNode.h (nodes)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "Node.h"
#include "ModelGraph.h"
#include "ModelTransformer.h"

// predictors
#include "ForestPredictor.h"
#include "SingleElementThresholdRule.h"
#include "ConstantNode.h"

// stl
#include <string>

namespace nodes
{
    /// <summary> Implements a forest node, where each tree in the forest uses single-input threshold
    /// split rules and constant outputs on all edges. </summary>
    ///
    /// <typeparam name="SplitRuleType"> The split rule type. </typeparam>
    /// <typeparam name="EdgePredictorType"> The edge predictor type. </typeparam>
    template<typename SplitRuleType, typename EdgePredictorType>
    class ForestNode : public model::Node
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr const char* inputPortName = "input";
        static constexpr const char* outputPortName = "prediction";
        static constexpr const char* treeOutputsPortName = "treeOutputs";
        static constexpr const char* edgeIndicatorVectorPortName = "edgeIndicatorVector";
        const model::OutputPort<double>& prediction = _prediction;
        const model::OutputPort<double>& treeOutputs = _treeOutputs;
        const model::OutputPort<bool>& edgeIndicatorVector = _edgeIndicatorVector;
        /// @}

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The predictor's input. </param>
        /// <param name="forest"> The forest predictor. </param>
        ForestNode(const model::OutputPortElements<double>& input, const predictors::ForestPredictor<SplitRuleType, EdgePredictorType>& forest);
        
        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<SplitRuleType, EdgePredictorType>("ForestNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Makes a copy of this node in the graph being constructed by the transformer. </summary>
        ///
        /// <param name="transformer"> [in,out] The transformer. </param>
        virtual void Copy(model::ModelTransformer& transformer) const override;

        /// <summary> Refines this node in the graph being constructed by the transformer </summary>
        virtual void Refine(model::ModelTransformer& transformer) const;

    protected:
        virtual void Compute() const override;

    private:
        // Input
        model::InputPort<double> _input;

        // Outputs
        model::OutputPort<double> _prediction;
        model::OutputPort<double> _treeOutputs;
        model::OutputPort<bool> _edgeIndicatorVector;

        // Forest
        predictors::ForestPredictor<SplitRuleType, EdgePredictorType> _forest;
    };

    typedef ForestNode<predictors::SingleElementThresholdRule, predictors::ConstantPredictor>  SimpleForestNode;

    /// <summary> A struct that represents the outputs of a linear predictor sub-model. </summary>
    struct ForestSubModelOutputs
    {
        const model::OutputPort<double>& prediction;
        //const model::OutputPort<double>& treeOutputs;   // TODO: waiting for OutputPortElements changes
        //const model::OutputPort<bool>& edgeIndicatorVector;
    };

    /// <summary> Builds a part of the model that represents a refined forest predictor. </summary>
    ///
    /// <typeparam name="SplitRuleType"> The split rule type. </typeparam>
    /// <typeparam name="EdgePredictorType"> The edge predictor type. </typeparam>
    /// <param name="predictor"> The forest predictor. </param>
    /// <param name="model"> [in,out] The model being modified. </param>
    /// <param name="outputPortElements"> The output port elements from which the linear predictor takes its inputs. </param>
    ///
    /// <returns> The ForestSubModelOutputs. </returns>
    template<typename SplitRuleType, typename EdgePredictorType>
    ForestSubModelOutputs BuildSubModel(const predictors::ForestPredictor<SplitRuleType, EdgePredictorType>& predictor, model::Model& model, const model::OutputPortElements<double>& outputPortElements);
}

#include "../tcc/ForestNode.tcc"
