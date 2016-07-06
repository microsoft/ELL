////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SimpleForestNode.cpp (nodes)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SimpleForestNode.h"

namespace nodes
{
    std::string SimpleForestNode::GetRuntimeTypeName() const
    {
        return "SimpleForestNode";
    }

    void SimpleForestNode::Compute() const
    {
//        _output.SetOutput({ _tree.Compute(_input) });
//        _edgePathIndicatorVector.SetOutput(_tree.GetEdgePathIndicatorVector(_input));
    }
}