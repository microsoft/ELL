////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelMaker.tcc (compile_test)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

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
    return _model.AddNode<nodes::BinaryOperationNode<T>>(x, y, emitters::BinaryOperationType::add);
}

template <typename T>
nodes::BinaryOperationNode<T>* ModelMaker::Subtract(const model::OutputPort<T>& x, const model::OutputPort<T>& y)
{
    return _model.AddNode<nodes::BinaryOperationNode<T>>(x, y, emitters::BinaryOperationType::subtract);
}

template <typename T>
nodes::BinaryOperationNode<T>* ModelMaker::Multiply(const model::OutputPort<T>& x, const model::OutputPort<T>& y)
{
    return _model.AddNode<nodes::BinaryOperationNode<T>>(x, y, emitters::BinaryOperationType::coordinatewiseMultiply);
}

template <typename T>
nodes::BinaryOperationNode<T>* ModelMaker::Divide(const model::OutputPort<T>& x, const model::OutputPort<T>& y)
{
    return _model.AddNode<nodes::BinaryOperationNode<T>>(x, y, emitters::BinaryOperationType::coordinatewiseDivide);
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
    return _model.AddNode<nodes::UnaryOperationNode<T>>(x, emitters::UnaryOperationType::sqrt);
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
