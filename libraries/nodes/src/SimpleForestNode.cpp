////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SimpleForestNode.cpp (nodes)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SimpleForestNode.h"

// stl
#include <vector>

namespace nodes
{
    std::string SimpleForestNode::GetRuntimeTypeName() const
    {
        return "SimpleForestNode";
    }

    void SimpleForestNode::Compute() const
    {
        // forest output
        _output.SetOutput({ _forest.Compute(_input) });

        // individual tree outputs
        std::vector<double> treeOutputs(_forest.NumTrees());
        for(size_t i=0; i<_forest.NumTrees(); ++i)
        {
            treeOutputs[i] = _forest.Compute(_input, _forest.GetRootIndex(i));
        }
        _treeOutputs.SetOutput(treeOutputs);

        // path indicator
        std::vector<bool> edgeIndicator(_forest.NumEdges());
        _forest.GetEdgeIndicatorVector(_input, edgeIndicator);
        _edgeIndicatorVector.SetOutput(edgeIndicator);
    }
}