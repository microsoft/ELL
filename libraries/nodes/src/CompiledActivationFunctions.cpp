////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompiledActivationFunctions.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompiledActivationFunctions.h"
#include "ActivationLayerNode.h"
#include "BinaryFunctionNode.h"
#include "ConstantNode.h"

// predictors
#include "LeakyReLUActivation.h"
#include "ReLUActivation.h"
#include "SigmoidActivation.h"

// emitters
#include "IRLocalValue.h"

namespace ell
{
namespace nodes
{
    //
    // Helper functions to convert predictors::neural activation functions into node activation functions
    //
    template <typename ValueType>
    ReLUActivationFunction<ValueType> GetNodeActivationFunction(const predictors::neural::ReLUActivation<ValueType>& f)
    {
        return ReLUActivationFunction<ValueType>{};
    }

    template <typename ValueType>
    LeakyReLUActivationFunction<ValueType> GetNodeActivationFunction(const predictors::neural::LeakyReLUActivation<ValueType>& f)
    {
        return LeakyReLUActivationFunction<ValueType>{ f.GetLeakyFactor() };
    }

    template <typename ValueType>
    SigmoidActivationFunction<ValueType> GetNodeActivationFunction(const predictors::neural::SigmoidActivation<ValueType>& f)
    {
        return SigmoidActivationFunction<ValueType>{};
    }

    template <typename ValueType>
    HardSigmoidActivationFunction<ValueType> GetNodeActivationFunction(const predictors::neural::HardSigmoidActivation<ValueType>& f)
    {
        return HardSigmoidActivationFunction<ValueType>{};
    }

    template <typename ValueType>
    TanhActivationFunction<ValueType> GetNodeActivationFunction(const predictors::neural::TanhActivation<ValueType>& f)
    {
        return TanhActivationFunction<ValueType>{};
    }

    template <typename ValueType>
    ParametricReLUActivationFunction<ValueType> GetNodeActivationFunction(const predictors::neural::ParametricReLUActivation<ValueType>& f)
    {
        return ParametricReLUActivationFunction<ValueType>{};
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
    llvm::Value* ReLUActivationFunction<ValueType>::Compile(emitters::IRFunctionEmitter& function, llvm::Value* xValue) const
    {
        auto x = function.LocalScalar(xValue);
        auto zero = function.LocalScalar<ValueType>(0);
        auto result = function.Select(x >= zero, x, zero);
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
    llvm::Value* LeakyReLUActivationFunction<ValueType>::Compile(emitters::IRFunctionEmitter& function, llvm::Value* xValue) const
    {
        auto x = function.LocalScalar(xValue);
        auto zero = function.LocalScalar<ValueType>(0.0);
        auto result = function.Select(x >= zero, x, x * function.LocalScalar(GetLeakyFactor()));
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
    llvm::Value* SigmoidActivationFunction<ValueType>::Compile(emitters::IRFunctionEmitter& function, llvm::Value* xValue) const
    {
        auto x = function.LocalScalar(xValue);

        return Sigmoid(x);
    }

    //
    // Tanh activation function
    //
    template <typename ValueType>
    ValueType TanhActivationFunction<ValueType>::Compute(ValueType x) const
    {
        return std::tanh(x);
    }

    template <typename ValueType>
    llvm::Value* TanhActivationFunction<ValueType>::Compile(emitters::IRFunctionEmitter& function, llvm::Value* xValue) const
    {
        auto x = function.LocalScalar(xValue);
        auto one = function.LocalScalar<ValueType>(1.0);
        auto two = function.LocalScalar<ValueType>(2.0);
        // tanh = (exp(x) - exp(-x)) / (exp(x) + exp(-x))
        //      = 2*sigmoid(2*x) - 1
        auto sigmoidFunction = SigmoidActivationFunction<ValueType>();
        auto sig2x = function.LocalScalar(sigmoidFunction.Compile(function, two * x));
        return (two * sig2x) - one;
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
    llvm::Value* ParametricReLUActivationFunction<ValueType>::Compile(emitters::IRFunctionEmitter& function, llvm::Value* xValue, llvm::Value* aValue) const
    {
        auto x = function.LocalScalar(xValue);
        auto a = function.LocalScalar(aValue);
        auto zero = function.LocalScalar<ValueType>(0.0);

        // ((x[i] > 0) ? x[i] : a[i] * x[i])
        llvm::Value* result = function.Variable(emitters::GetVariableType<ValueType>(), "result");
        auto ifEmitter = function.If();
        ifEmitter.If(x > zero);
        {
            function.Store(result, x);
        }
        ifEmitter.Else();
        {
            function.Store(result, x * a);
        }
        ifEmitter.End();
        return function.Load(result);
    }

    // Explicit instantiation
    template class ReLUActivationFunction<float>;
    template class ReLUActivationFunction<double>;
    template class LeakyReLUActivationFunction<float>;
    template class LeakyReLUActivationFunction<double>;
    template class SigmoidActivationFunction<float>;
    template class SigmoidActivationFunction<double>;
    template class TanhActivationFunction<float>;
    template class TanhActivationFunction<double>;
    template class ParametricReLUActivationFunction<float>;
    template class ParametricReLUActivationFunction<double>;

    template ReLUActivationFunction<float> GetNodeActivationFunction(const predictors::neural::ReLUActivation<float>& f);
    template ReLUActivationFunction<double> GetNodeActivationFunction(const predictors::neural::ReLUActivation<double>& f);
    template LeakyReLUActivationFunction<float> GetNodeActivationFunction(const predictors::neural::LeakyReLUActivation<float>& f);
    template LeakyReLUActivationFunction<double> GetNodeActivationFunction(const predictors::neural::LeakyReLUActivation<double>& f);
    template SigmoidActivationFunction<float> GetNodeActivationFunction(const predictors::neural::SigmoidActivation<float>& f);
    template SigmoidActivationFunction<double> GetNodeActivationFunction(const predictors::neural::SigmoidActivation<double>& f);
    template TanhActivationFunction<float> GetNodeActivationFunction(const predictors::neural::TanhActivation<float>& f);
    template TanhActivationFunction<double> GetNodeActivationFunction(const predictors::neural::TanhActivation<double>& f);

} // nodes
} // ell
