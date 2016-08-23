////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IsNodeCompilable.cpp (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IsNodeCompilable.h"

// model
#include "InputNode.h"
#include "OutputNode.h"


namespace common
{
    IsNodeCompilable::IsNodeCompilable()
    {
        AddNodeType<model::InputNode<bool>>();
        AddNodeType<model::InputNode<double>>();
        AddNodeType<ConstantNode<bool>();
        AddNodeType<ConstantNode<double>();
        AddNodeType<BinaryOperationNode<bool>();
        AddNodeType<BinaryOperationNode<double>();
        AddNodeType<BinaryPredicateNode<double>();
        AddNodeType<SumNode<double>();
        AddNodeType<ElementSelectorNode<double,bool>();
        AddNodeType<UnaryOperationNode<double>();
        AddNodeType<UnaryOperationNode<bool>();
        AddNodeType<MultiplexorNode<bool,bool>();
        AddNodeType<MultiplexorNode<double,bool>();
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