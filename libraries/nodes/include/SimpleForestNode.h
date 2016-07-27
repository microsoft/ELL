////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SimpleForestNode.h (nodes)
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

// stl
#include <string>

namespace nodes
{
    /// <summary>
    /// Implements a forest node, where each tree in the forest uses single-input threshold split
    /// rules and constant outputs on all edges.
    /// </summary>
    class SimpleForestNode : public model::Node
    {
    public:
        /// @name Input and Output Ports
        /// @{
        static constexpr char* inputPortName = "input";
        static constexpr char* outputPortName = "prediction";
        static constexpr char* treeOutputsPortName = "treeOutputs";
        static constexpr char* edgeIndicatorVectorPortName = "edgeIndicatorVector";
        const model::OutputPort<double>& prediction = _prediction;
        const model::OutputPort<double>& treeOutputs = _treeOutputs;
        const model::OutputPort<bool>& edgeIndicatorVector = _edgeIndicatorVector;
        /// @}

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The predictor's input. </param>
        /// <param name="forest"> The simple forest predictor. </param>
        SimpleForestNode(const model::OutputPortElements<double>& input, const predictors::SimpleForestPredictor& forest);

        /// <summary> Gets the name of this type. </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override;

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
        predictors::SimpleForestPredictor _forest;
    };
}