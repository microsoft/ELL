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
        auto outputValue = Predict(_input);
        _output.SetOutput({ outputValue });
    }
}