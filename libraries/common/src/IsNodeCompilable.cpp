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

// nodes
#include "ConstantNode.h"
#include "BinaryOperationNode.h"
#include "BinaryPredicateNode.h"
#include "SumNode.h"
#include "UnaryOperationNode.h"
#include "DemultiplexerNode.h"
#include "MultiplexerNode.h"
#include "ValueSelectorNode.h"

namespace common
{
    IsNodeCompilable::IsNodeCompilable()
    {
        AddNodeType<model::InputNode<bool>>();
        AddNodeType<model::InputNode<double>>();
        AddNodeType<model::OutputNode<double>>();
        AddNodeType<nodes::ConstantNode<bool>>();
        AddNodeType<nodes::ConstantNode<int>>();
        AddNodeType<nodes::ConstantNode<double>>();
        AddNodeType<nodes::BinaryOperationNode<bool>>();
        AddNodeType<nodes::BinaryOperationNode<double>>();
        AddNodeType<nodes::BinaryPredicateNode<int>>();
        AddNodeType<nodes::BinaryPredicateNode<double>>();
        AddNodeType<nodes::DemultiplexerNode<bool,bool>>();
        AddNodeType<nodes::DemultiplexerNode<double,bool>>();
        AddNodeType<nodes::SumNode<double>>();
        AddNodeType<nodes::TypeCastNode<bool,int>>();
        AddNodeType<nodes::UnaryOperationNode<bool>>();
        AddNodeType<nodes::UnaryOperationNode<double>>();
        AddNodeType<nodes::MultiplexerNode<bool,bool>>();
        AddNodeType<nodes::MultiplexerNode<double,bool>>();
        AddNodeType<model::ValueSelectorNode<bool>>();
        AddNodeType<model::ValueSelectorNode<double>>();
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