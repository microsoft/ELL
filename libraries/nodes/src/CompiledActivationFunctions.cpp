////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompiledActivationFunctions.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompiledActivationFunctions.h"

#include <predictors/neural/include/Activation.h>

#include <emitters/include/IRLocalValue.h>

namespace ell
{
namespace nodes
{
    //
    // Helper functions to convert predictors::neural activation functions into compilable node activation functions
    //
    template <typename ValueType>
    std::unique_ptr<ActivationFunction<ValueType>> GetNodeActivationFunction(const predictors::neural::Activation<ValueType>& f)
    {
        const predictors::neural::ActivationImpl<ValueType>* ptr = f.GetImpl();

        auto hardSigmoid = dynamic_cast<const predictors::neural::HardSigmoidActivation<ValueType>*>(ptr);
        if (hardSigmoid) return std::make_unique<HardSigmoidActivationFunction<ValueType>>();

        auto leakyReLU = dynamic_cast<const predictors::neural::LeakyReLUActivation<ValueType>*>(ptr);
        if (leakyReLU) return std::make_unique<LeakyReLUActivationFunction<ValueType>>(leakyReLU->GetLeakyFactor());

        auto sigmoid = dynamic_cast<const predictors::neural::SigmoidActivation<ValueType>*>(ptr);
        if (sigmoid) return std::make_unique<SigmoidActivationFunction<ValueType>>();

        auto relu = dynamic_cast<const predictors::neural::ReLUActivation<ValueType>*>(ptr);
        if (relu) return std::make_unique<ReLUActivationFunction<ValueType>>();

        auto tanh = dynamic_cast<const predictors::neural::TanhActivation<ValueType>*>(ptr);
        if (tanh) return std::make_unique<TanhActivationFunction<ValueType>>();

        auto prelu = dynamic_cast<const predictors::neural::ParametricReLUActivation<ValueType>*>(ptr);
        if (prelu)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "GetNodeActivationFunction cannot be used on ParametricReLUActivations");
        }

        throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                                        std::string("GetNodeActivationFunction given a new Activation type it doesn't recognize: ") + typeid(*f.GetImpl()).name());
    }

    //
    // Hard sigmoid activation function
    //
    template <typename ValueType>
    ValueType HardSigmoidActivationFunction<ValueType>::Compute(ValueType x) const
    {
        // y = clip (scale*x + bias) to [0,1]
        //   = scale * (clip x to [a, b]) + bias, where scale*a+bias = 0, scale*b+bias = 1; so, a = -bias/scale, b = (1-bias)/scale
        constexpr auto scale = static_cast<ValueType>(0.2);
        constexpr auto bias = static_cast<ValueType>(0.5);
        constexpr auto lowBound = -bias / scale;
        constexpr auto highBound = (1 - bias) / scale;
        return x < lowBound ? 0 : x > highBound ? 1 : (scale * x) + bias;
    }

    template <typename ValueType>
    emitters::LLVMValue HardSigmoidActivationFunction<ValueType>::Compile(emitters::IRFunctionEmitter& function, emitters::LLVMValue xValue) const
    {
        // y = clip (scale*x + bias) to [0,1]
        //   = scale * (clip x to [a, b]) + bias, where scale*a+bias = 0, scale*b+bias = 1; so, a = -bias/scale, b = (1-bias)/scale
        auto x = function.LocalScalar(xValue);
        const auto zero = function.Literal(ValueType{ 0 });
        const auto one = function.Literal(static_cast<ValueType>(1));
        constexpr auto scale = static_cast<ValueType>(0.2);
        constexpr auto bias = static_cast<ValueType>(0.5);
        constexpr auto lowBound = -bias / scale;
        constexpr auto highBound = (1 - bias) / scale;

        auto result = function.Select(x <= lowBound, zero, function.Select(x >= highBound, one, (scale * x) + bias));

        return result;
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
    emitters::LLVMValue ReLUActivationFunction<ValueType>::Compile(emitters::IRFunctionEmitter& function, emitters::LLVMValue xValue) const
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
    emitters::LLVMValue LeakyReLUActivationFunction<ValueType>::Compile(emitters::IRFunctionEmitter& function, emitters::LLVMValue xValue) const
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
    emitters::LLVMValue SigmoidActivationFunction<ValueType>::Compile(emitters::IRFunctionEmitter& function, emitters::LLVMValue xValue) const
    {
        auto x = function.LocalScalar(xValue);
        return emitters::Sigmoid<ValueType>(x);
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
    emitters::LLVMValue TanhActivationFunction<ValueType>::Compile(emitters::IRFunctionEmitter& function, emitters::LLVMValue xValue) const
    {
        auto x = function.LocalScalar(xValue);
        return emitters::Tanh<ValueType>(x);
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
    emitters::LLVMValue ParametricReLUActivationFunction<ValueType>::Compile(emitters::IRFunctionEmitter& function, emitters::LLVMValue xValue, emitters::LLVMValue aValue) const
    {
        auto x = function.LocalScalar(xValue);
        auto a = function.LocalScalar(aValue);
        return function.Select(x > static_cast<ValueType>(0), x, x * a);
    }

    // Explicit instantiation
    template class HardSigmoidActivationFunction<float>;
    template class HardSigmoidActivationFunction<double>;
    template class LeakyReLUActivationFunction<float>;
    template class LeakyReLUActivationFunction<double>;
    template class ParametricReLUActivationFunction<float>;
    template class ParametricReLUActivationFunction<double>;
    template class ReLUActivationFunction<float>;
    template class ReLUActivationFunction<double>;
    template class SigmoidActivationFunction<float>;
    template class SigmoidActivationFunction<double>;
    template class TanhActivationFunction<float>;
    template class TanhActivationFunction<double>;

    template std::unique_ptr<ActivationFunction<float>> GetNodeActivationFunction<float>(const predictors::neural::Activation<float>& f);
    template std::unique_ptr<ActivationFunction<double>> GetNodeActivationFunction<double>(const predictors::neural::Activation<double>& f);

} // namespace nodes
} // namespace ell
