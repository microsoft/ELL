////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IsNodeCompilable.cpp (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IsNodeCompilable.h"

namespace common
{
    IsNodeCompilable::IsNodeCompilable()
    {
        _nodeNames.insert("InputNode<double>");
        _nodeNames.insert("ConstantNode<double>");
        _nodeNames.insert("BinaryOperationNode<double>");
        _nodeNames.insert("SumNode<double>");
    }

    bool IsNodeCompilable::operator()(const model::Node * node)
    {
        const auto& nodeName = node->GetRuntimeTypeName();
        if(_nodeNames.find(nodeName) != _nodeNames.end())
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}