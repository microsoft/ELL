////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SimpleTreeNode.h (nodes)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "Node.h"
#include "InputPort.h"
#include "OutputPort.h"

// predictors
#include "TreePredictor.h"

// stl
#include <string>
#include <vector>

namespace nodes
{
    class SimpleTreeNode : public model::Node, public predictors::SimpleTreePredictor
    {
    public:
        /// <summary> Gets the name of this type. </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override;

        /// <summary> Exposes the tree output as a read-only property </summary>
        const model::OutputPort<double>& output = _output;

        /// <summary> Exposes the path indicator vectors as a read-only property </summary>
        const model::OutputPort<bool>& pathIndicator = _pathIndicator;
    
    protected:
        virtual void Compute() const override;

    private:
        // input ports
        model::InputPort<double> _input;
        
        // output ports
        model::OutputPort<double> _output;
        model::OutputPort<bool> _pathIndicator;
    };
}