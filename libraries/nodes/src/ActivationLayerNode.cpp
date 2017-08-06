////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ActivationLayerNode.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ActivationLayerNode.h"
#include "BinaryFunctionNode.h"
#include "ConstantNode.h"

// predictors
#include "LeakyReLUActivation.h"
#include "ReLUActivation.h"
#include "SigmoidActivation.h"

namespace ell
{
namespace nodes
{
    // Helper functions to convert predictors::neural activation functions into nodes activation functions
    template <typename ValueType>
    auto GetNodeActivationFunction(const predictors::neural::ReLUActivation<ValueType>& f)
    {
        return ReLUActivationFunction<ValueType>{};
    }

    template <typename ValueType>
    auto GetNodeActivationFunction(const predictors::neural::LeakyReLUActivation<ValueType>& f)
    {
        return LeakyReLUActivationFunction<ValueType>{ f.GetLeakyFactor() };
    }

    template <typename ValueType>
    auto GetNodeActivationFunction(const predictors::neural::SigmoidActivation<ValueType>& f)
    {
        return SigmoidActivationFunction<ValueType>{};
    }

    //
    // ReLU activation function
    //
    template <typename ValueType>
    ValueType ReLUActivationFunction<ValueType>::Compute(ValueType x) const
    {
        return x >= 0 ? x : 0;
    }

    template <typename ValueType>
    llvm::Value* ReLUActivationFunction<ValueType>::Compile(emitters::IRFunctionEmitter& function, llvm::Value* x) const
    {
        auto cmp = function.Comparison(emitters::TypedComparison::greaterThanFloat, x, function.Literal<ValueType>(0.0));
        auto result = function.Select(cmp, x, function.Literal<ValueType>(0.0));
        return result;
    }

    //
    // Leaky ReLU activation function
    //
    template <typename ValueType>
    ValueType LeakyReLUActivationFunction<ValueType>::Compute(ValueType x) const
    {
        return x >= 0 ? x : x * GetLeakyFactor();
    }

    template <typename ValueType>
    llvm::Value* LeakyReLUActivationFunction<ValueType>::Compile(emitters::IRFunctionEmitter& function, llvm::Value* x) const
    {
        auto cmp = function.Comparison(emitters::TypedComparison::greaterThanFloat, x, function.Literal<ValueType>(0.0));
        auto result = function.Select(cmp, x, function.Operator(emitters::TypedOperator::multiplyFloat, x, function.Literal<ValueType>(GetLeakyFactor())));
        return result;
    }

    //
    // Sigmoid activation function
    //
    template <typename ValueType>
    ValueType SigmoidActivationFunction<ValueType>::Compute(ValueType x) const
    {
        if (x > 0)
        {
            auto result = static_cast<ValueType>(1) / (std::exp(-x) + static_cast<ValueType>(1));
            return result;
        }
        else
        {
            auto expInput = std::exp(x);
            return expInput / (expInput + static_cast<ValueType>(1));
        }
    }

    template <typename ValueType>
    llvm::Value* SigmoidActivationFunction<ValueType>::Compile(emitters::IRFunctionEmitter& function, llvm::Value* x) const
    {
        auto expFunc = function.GetModule().GetRuntime().GetExpFunction<ValueType>();

        llvm::Value* result = function.Variable(emitters::GetVariableType<ValueType>(), "result");
        auto ifEmitter = function.If();
        ifEmitter.If(emitters::TypedComparison::greaterThanFloat, x, function.Literal(static_cast<ValueType>(0.0)));
        {
            auto negatedInput = function.Operator(emitters::TypedOperator::multiplyFloat, x, function.Literal(static_cast<ValueType>(-1.0)));
            auto expNegInput = function.Call(expFunc, { negatedInput });
            auto expNegPlusOne = function.Operator(emitters::TypedOperator::addFloat, expNegInput, function.Literal(static_cast<ValueType>(1.0)));
            auto sigmoid = function.Operator(emitters::TypedOperator::divideFloat, function.Literal(static_cast<ValueType>(1.0)), expNegPlusOne);
            function.Store(result, sigmoid);
        }
        ifEmitter.Else();
        {
            auto expInput = function.Call(expFunc, { x });
            auto expPlusOne = function.Operator(emitters::TypedOperator::addFloat, expInput, function.Literal(static_cast<ValueType>(1.0)));
            auto sigmoid = function.Operator(emitters::TypedOperator::divideFloat, expInput, expPlusOne);
            function.Store(result, sigmoid);
        }
        ifEmitter.End();
        return function.Load(result);
    }

    //
    // Parametric ReLU activation function
    //
    template <typename ValueType>
    ValueType ParametricReLUActivationFunction<ValueType>::Compute(ValueType x, ValueType a) const
    {
        return ((x > 0) ? x : a * x);
    }

    template <typename ValueType>
    llvm::Value* ParametricReLUActivationFunction<ValueType>::Compile(emitters::IRFunctionEmitter& function, llvm::Value* x, llvm::Value* a) const
    {
        // ((x[i] > 0) ? x[i] : a[i] * x[i])
        llvm::Value* result = function.Variable(emitters::GetVariableType<ValueType>(), "result");
        auto ifEmitter = function.If();
        ifEmitter.If(emitters::GetComparison<ValueType>(emitters::BinaryPredicateType::greater), x, function.Literal<ValueType>(0));
        {
            function.Store(result, x);
        }
        ifEmitter.Else();
        {
            function.Store(result, function.Operator(emitters::GetMultiplyForValueType<ValueType>(), x, a));
        }
        ifEmitter.End();
        return function.Load(result);
    }

    //
    // ActivationLayerNode
    //

    template <typename ValueType, template <typename> class ActivationFunctionType>
    ActivationLayerNode<ValueType, ActivationFunctionType>::ActivationLayerNode(const model::PortElements<ValueType>& input, const predictors::neural::ActivationLayer<ValueType, ActivationFunctionType>& layer)
        : BaseType(input, layer)
    {
        auto&& inputLayout = this->GetInputMemoryLayout();
        auto&& outputLayout = this->GetOutputMemoryLayout();
        if (!ShapesEqual(inputLayout.size, outputLayout.size))
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input and output active area sizes don't match");
        }
    }

    template <typename ValueType, template <typename> class ActivationFunctionType>
    bool ActivationLayerNode<ValueType, ActivationFunctionType>::Refine(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(this->input.GetPortElements());

        auto predictorActivationFunction = this->GetLayer().GetActivationFunction();
        auto nodeActivationFunction = GetNodeActivationFunction(predictorActivationFunction);
        auto computeNode = transformer.AddNode<BroadcastUnaryFunctionNode<ValueType, decltype(nodeActivationFunction)>>(newInput,
                                                                                                                        this->GetInputMemoryLayout(),
                                                                                                                        this->GetOutputMemoryLayout(),
                                                                                                                        nodeActivationFunction);
        transformer.MapNodeOutput(this->output, computeNode->output);
        return true;
    }

    //
    // ParametricReLUActivationLayerNode
    //

    template <typename ValueType>
    ParametricReLUActivationLayerNode<ValueType>::ParametricReLUActivationLayerNode(const model::PortElements<ValueType>& input, const LayerType& layer)
        : BaseType(input, layer)
    {
        auto&& inputLayout = this->GetInputMemoryLayout();
        auto&& outputLayout = this->GetOutputMemoryLayout();
        if (!ShapesEqual(inputLayout.size, outputLayout.size))
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input and output active area sizes don't match");
        }
    }

    template <typename ValueType>
    bool ParametricReLUActivationLayerNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        using ActivationFunction = ParametricReLUActivationFunction<ValueType>;
        auto newInput = transformer.TransformPortElements(this->input.GetPortElements());

        auto predictorActivationFunction = this->GetLayer().GetActivationFunction();
        auto alphaValues = predictorActivationFunction.GetAlpha().ToArray();
        auto alphaValuesNode = transformer.AddNode<ConstantNode<ValueType>>(alphaValues);

        // PReLU is a coordinate-wise operation
        auto computeNode = transformer.AddNode<BinaryFunctionNode<ValueType, ActivationFunction>>(
            newInput,
            alphaValuesNode->output,
            this->GetInputMemoryLayout(),
            this->GetOutputMemoryLayout(),
            ActivationFunction{});

        transformer.MapNodeOutput(this->output, computeNode->output);
        return true;
    }

    // Explicit specialization
    template class ReLUActivationFunction<float>;
    template class ReLUActivationFunction<double>;
    template class LeakyReLUActivationFunction<float>;
    template class LeakyReLUActivationFunction<double>;
    template class SigmoidActivationFunction<float>;
    template class SigmoidActivationFunction<double>;
    template class ActivationLayerNode<float, ell::predictors::neural::ReLUActivation>;
    template class ActivationLayerNode<double, ell::predictors::neural::ReLUActivation>;
    template class ActivationLayerNode<float, ell::predictors::neural::LeakyReLUActivation>;
    template class ActivationLayerNode<double, ell::predictors::neural::LeakyReLUActivation>;
    template class ActivationLayerNode<float, ell::predictors::neural::SigmoidActivation>;
    template class ActivationLayerNode<double, ell::predictors::neural::SigmoidActivation>;

    template class ParametricReLUActivationFunction<float>;
    template class ParametricReLUActivationFunction<double>;
    template class ParametricReLUActivationLayerNode<float>;
    template class ParametricReLUActivationLayerNode<double>;

} // nodes
} // ell
