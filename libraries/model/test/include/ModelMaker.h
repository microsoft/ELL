////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelMaker.h (compile_test)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "InputNode.h"
#include "OutputNode.h"

// nodes
#include "AccumulatorNode.h"
#include "BinaryOperationNode.h"
#include "BinaryPredicateNode.h"
#include "ConstantNode.h"
#include "DelayNode.h"
#include "DotProductNode.h"
#include "MultiplexerNode.h"
#include "SumNode.h"
#include "UnaryOperationNode.h"

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

#include "../tcc/ModelMaker.tcc"
