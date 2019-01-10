////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelMaker.h (compile_test)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/InputNode.h>
#include <model/include/OutputNode.h>

#include <nodes/include/AccumulatorNode.h>
#include <nodes/include/BinaryOperationNode.h>
#include <nodes/include/BinaryPredicateNode.h>
#include <nodes/include/ConstantNode.h>
#include <nodes/include/DelayNode.h>
#include <nodes/include/DotProductNode.h>
#include <nodes/include/MultiplexerNode.h>
#include <nodes/include/SumNode.h>
#include <nodes/include/UnaryOperationNode.h>

using namespace ell;

class ModelMaker
{
public:
    ModelMaker();
    ModelMaker(const ModelMaker& src);
    ModelMaker(ModelMaker&& src);

    template <typename T>
    nodes::ConstantNode<T>* Constant(const T value);
    template <typename T>
    nodes::ConstantNode<T>* Constant(const std::vector<T>& values);
    template <typename T>
    model::InputNode<T>* Inputs(size_t count);
    template <typename T>
    model::InputNode<T>* Inputs(std::vector<T>& values);
    template <typename T>
    model::OutputNode<T>* Outputs(const model::OutputPort<T>& x);
    template <typename T>
    nodes::BinaryOperationNode<T>* Add(const model::OutputPort<T>& x, const model::OutputPort<T>& y);
    template <typename T>
    nodes::BinaryOperationNode<T>* Subtract(const model::OutputPort<T>& x, const model::OutputPort<T>& y);
    template <typename T>
    nodes::BinaryOperationNode<T>* Multiply(const model::OutputPort<T>& x, const model::OutputPort<T>& y);
    template <typename T>
    nodes::BinaryOperationNode<T>* Divide(const model::OutputPort<T>& x, const model::OutputPort<T>& y);
    template <typename T>
    nodes::BinaryPredicateNode<T>* Equals(const model::OutputPort<T>& x, const model::OutputPort<T>& y);
    template <typename T>
    nodes::BinaryPredicateNode<T>* Lt(const model::OutputPort<T>& x, const model::OutputPort<T>& y);
    template <typename T>
    nodes::BinaryPredicateNode<T>* Gt(const model::OutputPort<T>& x, const model::OutputPort<T>& y);
    template <typename T, typename S>
    nodes::MultiplexerNode<T, S>* Select(const model::OutputPort<T>& elts, const model::OutputPort<S>& selector);
    template <typename T>
    nodes::AccumulatorNode<T>* Accumulate(const model::OutputPort<T>& x);
    template <typename T>
    nodes::SumNode<T>* Sum(const model::OutputPort<T>& x);
    template <typename T>
    nodes::DelayNode<T>* Delay(const model::OutputPort<T>& x, size_t windowSize);
    template <typename T>
    nodes::DotProductNode<T>* DotProduct(const model::OutputPort<T>& x, const model::OutputPort<T>& y);
    template <typename T>
    nodes::UnaryOperationNode<T>* Sqrt(const model::OutputPort<T>& x);

    template <typename T>
    model::OutputPort<T>* GetOutputPort(model::Node* pNode, size_t portIndex);

    // Expose as a property!
    model::Model& Model = _model;

    const std::string& Name = _name;

    void SetName(const std::string& name)
    {
        // STYLE discrepancy
        _name = name;
    }

private:
    model::Model _model;
    std::string _name;
};

#pragma region implementation

using namespace ell;

template <typename T>
model::InputNode<T>* ModelMaker::Inputs(size_t count)
{
    return _model.AddNode<model::InputNode<T>>(count);
}

template <typename T>
model::InputNode<T>* ModelMaker::Inputs(std::vector<T>& values)
{
    auto node = Inputs<T>(values.size());
    node->SetInput(values);
    return node;
}

template <typename T>
model::OutputNode<T>* ModelMaker::Outputs(const model::OutputPort<T>& x)
{
    return _model.AddNode<model::OutputNode<T>>(x);
}

template <typename T>
nodes::BinaryOperationNode<T>* ModelMaker::Add(const model::OutputPort<T>& x, const model::OutputPort<T>& y)
{
    return _model.AddNode<nodes::BinaryOperationNode<T>>(x, y, nodes::BinaryOperationType::add);
}

template <typename T>
nodes::BinaryOperationNode<T>* ModelMaker::Subtract(const model::OutputPort<T>& x, const model::OutputPort<T>& y)
{
    return _model.AddNode<nodes::BinaryOperationNode<T>>(x, y, nodes::BinaryOperationType::subtract);
}

template <typename T>
nodes::BinaryOperationNode<T>* ModelMaker::Multiply(const model::OutputPort<T>& x, const model::OutputPort<T>& y)
{
    return _model.AddNode<nodes::BinaryOperationNode<T>>(x, y, nodes::BinaryOperationType::multiply);
}

template <typename T>
nodes::BinaryOperationNode<T>* ModelMaker::Divide(const model::OutputPort<T>& x, const model::OutputPort<T>& y)
{
    return _model.AddNode<nodes::BinaryOperationNode<T>>(x, y, nodes::BinaryOperationType::divide);
}

template <typename T>
nodes::DotProductNode<T>* ModelMaker::DotProduct(const model::OutputPort<T>& x, const model::OutputPort<T>& y)
{
    return _model.AddNode<nodes::DotProductNode<T>>(x, y);
}

template <typename T>
nodes::BinaryPredicateNode<T>* ModelMaker::Equals(const model::OutputPort<T>& x, const model::OutputPort<T>& y)
{
    return _model.AddNode<nodes::BinaryPredicateNode<T>>(x, y, emitters::BinaryPredicateType::equal);
}

template <typename T>
nodes::BinaryPredicateNode<T>* ModelMaker::Lt(const model::OutputPort<T>& x, const model::OutputPort<T>& y)
{
    return _model.AddNode<nodes::BinaryPredicateNode<T>>(x, y, emitters::BinaryPredicateType::less);
}

template <typename T>
nodes::BinaryPredicateNode<T>* ModelMaker::Gt(const model::OutputPort<T>& x, const model::OutputPort<T>& y)
{
    return _model.AddNode<nodes::BinaryPredicateNode<T>>(x, y, emitters::BinaryPredicateType::greater);
}

template <typename T, typename S>
nodes::MultiplexerNode<T, S>* ModelMaker::Select(const model::OutputPort<T>& elts, const model::OutputPort<S>& selector)
{
    auto node = _model.AddNode<nodes::MultiplexerNode<T, S>>(elts, selector);
    return node;
}

template <typename T>
nodes::UnaryOperationNode<T>* ModelMaker::Sqrt(const model::OutputPort<T>& x)
{
    return _model.AddNode<nodes::UnaryOperationNode<T>>(x, nodes::UnaryOperationType::sqrt);
}

template <typename T>
nodes::SumNode<T>* ModelMaker::Sum(const model::OutputPort<T>& x)
{
    return _model.AddNode<nodes::SumNode<T>>(x);
}

template <typename T>
nodes::DelayNode<T>* ModelMaker::Delay(const model::OutputPort<T>& x, size_t windowSize)
{
    return _model.AddNode<nodes::DelayNode<T>>(x, windowSize);
}

template <typename T>
nodes::AccumulatorNode<T>* ModelMaker::Accumulate(const model::OutputPort<T>& x)
{
    return _model.AddNode<nodes::AccumulatorNode<T>>(x);
}

template <typename T>
nodes::ConstantNode<T>* ModelMaker::Constant(const T value)
{
    return _model.AddNode<nodes::ConstantNode<T>>(value);
}

template <typename T>
nodes::ConstantNode<T>* ModelMaker::Constant(const std::vector<T>& values)
{
    auto* pNode = _model.AddNode<nodes::ConstantNode<T>>(values);
    // Work around a bug. Make sure literal values are propagated to outputs
    _model.ComputeOutput<T>(pNode->output);
    return pNode;
}

template <typename T>
model::OutputPort<T>* ModelMaker::GetOutputPort(model::Node* pNode, size_t portIndex)
{
    auto pPort = pNode->GetOutputPorts()[portIndex];
    return static_cast<model::OutputPort<T>*>(pPort);
}

#pragma endregion implementation
