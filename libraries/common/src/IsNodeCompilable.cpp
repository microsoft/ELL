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
        _nodeNames.insert("OutputNode<double>");
        _nodeNames.insert("ConstantNode<bool>");
        _nodeNames.insert("ConstantNode<int32>");
        _nodeNames.insert("ConstantNode<double>");
        _nodeNames.insert("BinaryOperationNode<bool>");
        _nodeNames.insert("BinaryOperationNode<double>");
        _nodeNames.insert("BinaryPredicateNode<int32>");
        _nodeNames.insert("BinaryPredicateNode<double>");
        _nodeNames.insert("SumNode<double>");
        _nodeNames.insert("MultiplexerNode<bool,bool>");
        _nodeNames.insert("MultiplexerNode<double,bool>");
        _nodeNames.insert("UnaryOperationNode<double>");
        _nodeNames.insert("UnaryOperationNode<bool>");
        _nodeNames.insert("TypeCastNode<bool,int32>");
        _nodeNames.insert("DemultiplexerNode<bool,bool>");
        _nodeNames.insert("DemultiplexerNode<double,bool>");
        _nodeNames.insert("ValueSelectorNode<bool>");
        _nodeNames.insert("ValueSelectorNode<double>");
    }

    bool IsNodeCompilable::operator()(const model::Node& node)
    {
        const auto& nodeName = node.GetRuntimeTypeName();
        if (_nodeNames.find(nodeName) != _nodeNames.end())
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}