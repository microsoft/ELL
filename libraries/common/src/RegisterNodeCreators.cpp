////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     RegisterNodeCreators.cpp (common)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "RegisterNodeCreators.h"

// model
#include "InputNode.h"
#include "ModelBuilder.h"
#include "OutputNode.h"

// nodes
#include "BinaryOperationNode.h"
#include "BinaryPredicateNode.h"
#include "DCTNode.h"
#include "BufferNode.h"
#include "DelayNode.h"
#include "DotProductNode.h"
#include "ExtremalValueNode.h"
#include "FFTNode.h"
#include "ForestPredictorNode.h"
#include "HammingWindowNode.h"
#include "IIRFilterNode.h"
#include "L2NormSquaredNode.h"
#include "LinearPredictorNode.h"
#include "MovingAverageNode.h"
#include "MovingVarianceNode.h"
#include "MultiplexerNode.h"
#include "UnaryOperationNode.h"
#include "ValueSelectorNode.h"

namespace ell
{
namespace common
{
    void RegisterNodeCreators(model::ModelBuilder& builder)
    {
        builder.RegisterNodeCreator<model::InputNode<bool>, size_t>();
        builder.RegisterNodeCreator<model::InputNode<int>, size_t>();
        builder.RegisterNodeCreator<model::InputNode<float>, size_t>();
        builder.RegisterNodeCreator<model::InputNode<double>, size_t>();

        builder.RegisterNodeCreator<model::OutputNode<bool>, const model::PortElements<bool>&>();
        builder.RegisterNodeCreator<model::OutputNode<int>, const model::PortElements<int>&>();
        builder.RegisterNodeCreator<model::OutputNode<float>, const model::PortElements<float>&>();
        builder.RegisterNodeCreator<model::OutputNode<double>, const model::PortElements<double>&>();

        builder.RegisterNodeCreator<nodes::ArgMaxNode<int>, const model::PortElements<int>&>();
        builder.RegisterNodeCreator<nodes::ArgMaxNode<double>, const model::PortElements<double>&>();
        builder.RegisterNodeCreator<nodes::ArgMinNode<int>, const model::PortElements<int>&>();
        builder.RegisterNodeCreator<nodes::ArgMinNode<double>, const model::PortElements<double>&>();

        builder.RegisterNodeCreator<nodes::AccumulatorNode<int>, const model::PortElements<int>&>();
        builder.RegisterNodeCreator<nodes::AccumulatorNode<double>, const model::PortElements<double>&>();

        builder.RegisterNodeCreator<nodes::BinaryOperationNode<bool>, const model::PortElements<bool>&, const model::PortElements<bool>&, emitters::BinaryOperationType>();
        builder.RegisterNodeCreator<nodes::BinaryOperationNode<int>, const model::PortElements<int>&, const model::PortElements<int>&, emitters::BinaryOperationType>();
        builder.RegisterNodeCreator<nodes::BinaryOperationNode<double>, const model::PortElements<double>&, const model::PortElements<double>&, emitters::BinaryOperationType>();

        builder.RegisterNodeCreator<nodes::BinaryPredicateNode<int>, const model::PortElements<int>&, const model::PortElements<int>&, emitters::BinaryPredicateType>();
        builder.RegisterNodeCreator<nodes::BinaryPredicateNode<double>, const model::PortElements<double>&, const model::PortElements<double>&, emitters::BinaryPredicateType>();

        builder.RegisterNodeCreator<nodes::BufferNode<float>, const model::PortElements<float>&, size_t>();
        builder.RegisterNodeCreator<nodes::BufferNode<double>, const model::PortElements<double>&, size_t>();

        builder.RegisterNodeCreator<nodes::ConstantNode<bool>, const std::vector<bool>&>();
        builder.RegisterNodeCreator<nodes::ConstantNode<int>, const std::vector<int>&>();
        builder.RegisterNodeCreator<nodes::ConstantNode<float>, const std::vector<float>&>();
        builder.RegisterNodeCreator<nodes::ConstantNode<double>, const std::vector<double>&>();

        builder.RegisterNodeCreator<nodes::DelayNode<bool>, const model::PortElements<bool>&, size_t>();
        builder.RegisterNodeCreator<nodes::DelayNode<int>, const model::PortElements<int>&, size_t>();
        builder.RegisterNodeCreator<nodes::DelayNode<float>, const model::PortElements<float>&, size_t>();
        builder.RegisterNodeCreator<nodes::DelayNode<double>, const model::PortElements<double>&, size_t>();

        builder.RegisterNodeCreator<nodes::DCTNode<float>, const model::PortElements<float>&, size_t>();
        builder.RegisterNodeCreator<nodes::DCTNode<double>, const model::PortElements<double>&, size_t>();

        builder.RegisterNodeCreator<nodes::DemultiplexerNode<bool, bool>, const model::PortElements<bool>&, const model::PortElements<bool>&, size_t>();

        builder.RegisterNodeCreator<nodes::DotProductNode<int>, const model::PortElements<int>&, const model::PortElements<int>&>();
        builder.RegisterNodeCreator<nodes::DotProductNode<double>, const model::PortElements<double>&, const model::PortElements<double>&>();

        builder.RegisterNodeCreator<nodes::FFTNode<float>, const model::PortElements<float>&>();
        builder.RegisterNodeCreator<nodes::FFTNode<double>, const model::PortElements<double>&>();
        
        builder.RegisterNodeCreator<nodes::HammingWindowNode<float>, const model::PortElements<float>&>();
        builder.RegisterNodeCreator<nodes::HammingWindowNode<double>, const model::PortElements<double>&>();

        builder.RegisterNodeCreator<nodes::IIRFilterNode<float>, const model::PortElements<float>&, const std::vector<float>&, const std::vector<float>&>();
        builder.RegisterNodeCreator<nodes::IIRFilterNode<double>, const model::PortElements<double>&, const std::vector<double>&, const std::vector<double>&>();

        builder.RegisterNodeCreator<nodes::L2NormSquaredNode<float>, const model::PortElements<float>&>();
        builder.RegisterNodeCreator<nodes::L2NormSquaredNode<double>, const model::PortElements<double>&>();

        builder.RegisterNodeCreator<nodes::MovingAverageNode<double>, const model::PortElements<double>&, size_t>();
        builder.RegisterNodeCreator<nodes::MovingVarianceNode<double>, const model::PortElements<double>&, size_t>();

        builder.RegisterNodeCreator<nodes::MultiplexerNode<bool, bool>, const model::PortElements<bool>&, const model::PortElements<bool>&>();
        builder.RegisterNodeCreator<nodes::MultiplexerNode<int, bool>, const model::PortElements<int>&, const model::PortElements<bool>&>();
        builder.RegisterNodeCreator<nodes::MultiplexerNode<double, bool>, const model::PortElements<double>&, const model::PortElements<bool>&>();

        builder.RegisterNodeCreator<nodes::MultiplexerNode<bool, int>, const model::PortElements<bool>&, const model::PortElements<int>&>();
        builder.RegisterNodeCreator<nodes::MultiplexerNode<int, int>, const model::PortElements<int>&, const model::PortElements<int>&>();
        builder.RegisterNodeCreator<nodes::MultiplexerNode<double, int>, const model::PortElements<double>&, const model::PortElements<int>&>();

        builder.RegisterNodeCreator<nodes::SumNode<int>, const model::PortElements<int>&>();
        builder.RegisterNodeCreator<nodes::SumNode<double>, const model::PortElements<double>&>();

        builder.RegisterNodeCreator<nodes::TypeCastNode<bool, int>, const model::PortElements<bool>&>();
        builder.RegisterNodeCreator<nodes::TypeCastNode<int, double>, const model::PortElements<int>&>();

        builder.RegisterNodeCreator<nodes::UnaryOperationNode<bool>, const model::PortElements<bool>&, emitters::UnaryOperationType>();
        builder.RegisterNodeCreator<nodes::UnaryOperationNode<double>, const model::PortElements<double>&, emitters::UnaryOperationType>();

        builder.RegisterNodeCreator<nodes::ValueSelectorNode<bool>, const model::PortElements<bool>&, const model::PortElements<bool>&, const model::PortElements<bool>&>();
        builder.RegisterNodeCreator<nodes::ValueSelectorNode<int>, const model::PortElements<bool>&, const model::PortElements<int>&, const model::PortElements<int>&>();
        builder.RegisterNodeCreator<nodes::ValueSelectorNode<double>, const model::PortElements<bool>&, const model::PortElements<double>&, const model::PortElements<double>&>();
    }
}
}
