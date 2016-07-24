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
#include <memory>

namespace nodes
{
    SimpleForestNode::SimpleForestNode(const model::OutputPortElementList<double>& input, const predictors::SimpleForestPredictor& forest) : Node({ &_input }, { &_output, &_treeOutputs, &_edgeIndicatorVector }), _input(this, input), _output(this, 1), _treeOutputs(this, forest.NumTrees()), _edgeIndicatorVector(this, forest.NumEdges()), _forest(forest)
    {}

    std::string SimpleForestNode::GetRuntimeTypeName() const
    {
        return "SimpleForestNode";
    }

    void SimpleForestNode::Copy(model::ModelTransformer & transformer) const
    {
    }

    void SimpleForestNode::Refine(model::ModelTransformer & transformer) const
    {
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
        _treeOutputs.SetOutput(std::move(treeOutputs));

        // path indicator
        auto edgeIndicator = _forest.GetEdgeIndicatorVector(_input);
        _edgeIndicatorVector.SetOutput(std::move(edgeIndicator));
    }
}