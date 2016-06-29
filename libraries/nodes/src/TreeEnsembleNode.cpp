////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     TreeEnsembleNode.cpp (nodes)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TreeEnsembleNode.h"

namespace nodes
{
    std::string TreeEnsembleNode::GetRuntimeTypeName() const
    {
        return "TreeEnsembleNode";
    }

    void TreeEnsembleNode::Compute() const
    {

    }
}