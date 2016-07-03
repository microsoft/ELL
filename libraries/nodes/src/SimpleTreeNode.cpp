////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SimpleTreeNode.cpp (nodes)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SimpleTreeNode.h"

namespace nodes
{
    std::string SimpleTreeNode::GetRuntimeTypeName() const
    {
        return "SimpleTreeNode";
    }

    void SimpleTreeNode::Compute() const
    {
        _output.SetOutput({ _tree.Compute(_input) });
        _edgePathIndicatorVector.SetOutput(_tree.GetEdgePathIndicatorVector(_input));
    }
}