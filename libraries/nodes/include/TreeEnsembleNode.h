////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     TreeEnsembleNode.h (nodes)
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
    class TreeEnsembleNode : public model::Node
    {
    public:
        /// <summary> Gets the name of this type. </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override;

    protected:
        virtual void Compute() const override;

    private:

        // input ports
        model::InputPort<double> featureValues;
        
        // output ports
        model::OutputPort<double> ensemblePrediction;
        model::OutputPort<double> treePredictions;
        std::vector<model::OutputPort<bool>> treePathIndicators;
        std::vector<model::OutputPort<bool>> treeLeafIndicators;
    };
}