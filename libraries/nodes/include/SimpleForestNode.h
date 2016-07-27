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
#include "InputPort.h"
#include "OutputPort.h"

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
        /// <summary> Input port name. </summary>
        static constexpr char* inputPortName = "input";

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

        /// <summary> Exposes the prediction port as a read-only property </summary>
        const model::OutputPort<double>& output = _prediction;

        /// <summary> Prediction port name. </summary>
        static constexpr char* outputPortName = "prediction";

        /// <summary> Exposes the individual tree outputs port as a read-only property </summary>
        const model::OutputPort<double>& treeOutputs = _treeOutputs;

        /// <summary> Tree outputs port name. </summary>
        static constexpr char* treeOutputsPortName = "treeOutputs";

        /// <summary> Exposes the forest edge indicator vector port as a read-only property </summary>
        const model::OutputPort<bool>& edgeIndicatorVector = _edgeIndicatorVector;

        /// <summary> Edge indicator vector port name. </summary>
        static constexpr char* edgeIndicatorVectorPortName = "edgeIndicatorVector";

    protected:
        virtual void Compute() const override;

    private:
        // Input
        model::InputPort<double> _input;

        // Outputs
        model::OutputPort<double> _prediction;
        model::OutputPort<double> _treeOutputs;
        model::OutputPort<bool> _edgeIndicatorVector;

        // the forest
        predictors::SimpleForestPredictor _forest;
    };
}