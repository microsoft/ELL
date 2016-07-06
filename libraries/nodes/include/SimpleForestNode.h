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
#include <vector>

namespace nodes
{
    /// <summary>
    /// Implements a simple binary decision/regression tree node, with single-input threshold split
    /// rules in interior nodes and constant outputs on all edges.
    /// </summary>
    class SimpleForestNode : public model::Node
    {
    public:
        /// <summary> Gets the name of this type. </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override;

        /// <summary> Exposes the tree output as a read-only property </summary>
        const model::OutputPort<double>& output = _output;

        /// <summary> Exposes the edge-path indicator vectors as a read-only property </summary>
        //const model::OutputPort<bool>& edgePathIndicatorVector = _edgePathIndicatorVector;
    
    protected:
        virtual void Compute() const override;

    private:
        // input ports
        model::InputPort<double> _input;
        
        // output ports
        model::OutputPort<double> _output;
        model::OutputPort<double> _treeOutputs;
        std::vector<model::OutputPort<bool>> _edgePathIndicatorVectors;

        // the tree
        predictors::SimpleForestPredictor _forest;
    };
}