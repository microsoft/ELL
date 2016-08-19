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
        _nodeNames.insert("InputNode<bool>");
        _nodeNames.insert("InputNode<double>");
        _nodeNames.insert("ConstantNode<bool>");
        _nodeNames.insert("ConstantNode<double>");
        _nodeNames.insert("BinaryOperationNode<bool>");
        _nodeNames.insert("BinaryOperationNode<double>");
        _nodeNames.insert("BinaryPredicateNode<double>");
        _nodeNames.insert("SumNode<double>");
        _nodeNames.insert("ElementSelectorNode<double>");
        _nodeNames.insert("UnaryOperationNode<double>");
        _nodeNames.insert("UnaryOperationNode<bool>");
        _nodeNames.insert("MultiplexorNode<bool,bool>");
        _nodeNames.insert("MultiplexorNode<double,bool>");
    }

    bool IsNodeCompilable::operator()(const model::Node& node)
    {
        const auto& nodeName = node.GetRuntimeTypeName();
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