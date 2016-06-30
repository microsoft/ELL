////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BinaryDecisionTreeNode.h (nodes)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "Node.h"
#include "InputPort.h"
#include "OutputPort.h"

// stl
#include <string>
#include <vector>

namespace nodes
{
    class BinaryDecisionTreeNode : public model::Node
    {
    public:
        /// <summary> Gets the name of this type. </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override;

        /// <summary> Exposes the ensemble prediction as a read-only property </summary>
        const model::OutputPort<double>& prediction = _prediction;

        /// <summary> Exposes the path indicator vectors of individual trees as a read-only property </summary>
        const model::OutputPort<bool>& pathIndicator = _pathIndicator;
    
    protected:
        virtual void Compute() const override;

    private:
        // input ports
        model::InputPort<double> _featureValues;
        
        // output ports
        model::OutputPort<double> _prediction;
        model::OutputPort<bool> _pathIndicator;
    };
}