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
        /// <summary> Gets the name of this type. </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override;

        /// <summary> Exposes the forest output as a read-only property </summary>
        const model::OutputPort<double>& output = _output;

        /// <summary> Exposes the individual tree outputs as a read-only property </summary>
        const model::OutputPort<double>& treeOutputs = _treeOutputs;

        /// <summary> Exposes the forest edge indicator vector as a read-only property </summary>
        const model::OutputPort<bool>& edgeIndicatorVector = _edgeIndicatorVector;
    
    protected:
        virtual void Compute() const override;

    private:
        // input ports
        model::InputPort<double> _input;
        
        // output ports
        model::OutputPort<double> _output;
        model::OutputPort<double> _treeOutputs;
        model::OutputPort<bool> _edgeIndicatorVector;

        // the tree
        predictors::SimpleForestPredictor _forest;
    };
}