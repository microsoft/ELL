////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompiledActivationFunctions.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "BroadcastFunctionNode.h"

// emitters
#include "IRFunctionEmitter.h"

// predictors/neural
#include "HardSigmoidActivation.h"
#include "LeakyReLUActivation.h"
#include "ParametricReLUActivation.h"
#include "ReLUActivation.h"
#include "SigmoidActivation.h"
#include "TanhActivation.h"

// utilities
#include "TypeName.h"

namespace ell
{
namespace nodes
{
    //
    // Activation functions
    //
    template <typename ValueType>
    class ReLUActivationFunction : public BroadcastUnaryFunction<ValueType>
    {
    public:
        /// <summary> Computes the ReLU activation (on the host machine) </summary>
        ///
        /// <param name="x"> The value </param>
        /// <returns> The value of the function ReLU(x) </returns>
        ValueType Compute(ValueType x) const override;
        using BroadcastUnaryFunction<ValueType>::Compute;

        /// <summary> Emits IR to compute the ReLU activation function </summary>
        ///
        /// <param name="function"> The function being compiled. </param>
        /// <param name="x"> The value </param>
        ///
        /// <returns> The value of the function ReLU(x) </returns>
        llvm::Value* Compile(emitters::IRFunctionEmitter& function, llvm::Value* x) const override;
        using BroadcastUnaryFunction<ValueType>::Compile;

        /// <summary> Indicates if the function can operate on vector types </summary>
        ///
        /// <returns> true if the function can operate on vector types </returns>
        bool CanUseVectorTypes() const { return false; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("ReLUActivationFunction"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const { return GetTypeName(); }
    };

    template <typename ValueType>
    class LeakyReLUActivationFunction : public BroadcastUnaryFunction<ValueType>
    {
    public:
        LeakyReLUActivationFunction() = default;

        /// <summary> Constructor specifying the leaky factor. </summary>
        ///
        /// <param name="leakyFactor"> The leaky factor. </param>
        LeakyReLUActivationFunction(ValueType leakyFactor)
            : _leakyFactor(leakyFactor) {}

        /// <summary> Computes the leaky ReLU activation (on the host machine) </summary>
        ///
        /// <param name="x"> The value </param>
        /// <returns> The value of the function LeakyReLU(x) </returns>
        ValueType Compute(ValueType x) const override;
        using BroadcastUnaryFunction<ValueType>::Compute;

        /// <summary> Emits IR to compute the leaky ReLU activation function </summary>
        ///
        /// <param name="function"> The function being compiled. </param>
        /// <param name="x"> The value </param>
        ///
        /// <returns> The value of the function LeakyReLU(x) </returns>
        llvm::Value* Compile(emitters::IRFunctionEmitter& function, llvm::Value* x) const override;
        using BroadcastUnaryFunction<ValueType>::Compile;

        /// <summary> Indicates if the function can operate on vector types </summary>
        ///
        /// <returns> true if the function can operate on vector types </returns>
        bool CanUseVectorTypes() const { return false; }

        /// <summary> Gets the leaky factor </summary>
        ///
        /// <returns> The leaky factor </returns>
        ValueType GetLeakyFactor() const { return _leakyFactor; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("LeakyReLUActivationFunction"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const { return GetTypeName(); }

    private:
        ValueType _leakyFactor = 0;
    };

    template <typename ValueType>
    class SigmoidActivationFunction : public BroadcastUnaryFunction<ValueType>
    {
    public:
        /// <summary> Computes the sigmoid activation (on the host machine) </summary>
        ///
        /// <param name="function"> The function being compiled. </param>
        /// <param name="x"> The value </param>
        ///
        /// <returns> The value of the function Sigmoid(x) </returns>
        ValueType Compute(ValueType x) const override;
        using BroadcastUnaryFunction<ValueType>::Compute;

        /// <summary> Emits IR to compute the sigmoid activation function </summary>
        ///
        /// <param name="x"> The value </param>
        /// <returns> The value of the function Sigmoid(x) </returns>
        ///
        llvm::Value* Compile(emitters::IRFunctionEmitter& function, llvm::Value* x) const override;
        using BroadcastUnaryFunction<ValueType>::Compile;

        /// <summary> Indicates if the function can operate on vector types </summary>
        ///
        /// <returns> true if the function can operate on vector types </returns>
        bool CanUseVectorTypes() const { return false; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("SigmoidActivationFunction"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const { return GetTypeName(); }
    };

    template <typename ValueType>
    class HardSigmoidActivationFunction : public BroadcastUnaryFunction<ValueType>
    {
    public:
        /// <summary> Computes the hard sigmoid activation (on the host machine) </summary>
        ///
        /// <param name="function"> The function being compiled. </param>
        /// <param name="x"> The value </param>
        ///
        /// <returns> The value of the hard sigmoid function) </returns>
        ValueType Compute(ValueType x) const override;
        using BroadcastUnaryFunction<ValueType>::Compute;

        /// <summary> Emits IR to compute the hard sigmoid activation function </summary>
        ///
        /// <param name="x"> The value </param>
        ///
        /// <returns> The value of the function Sigmoid(x) </returns>
        llvm::Value* Compile(emitters::IRFunctionEmitter& function, llvm::Value* x) const override;
        using BroadcastUnaryFunction<ValueType>::Compile;

        /// <summary> Indicates if the function can operate on vector types </summary>
        ///
        /// <returns> true if the function can operate on vector types </returns>
        bool CanUseVectorTypes() const { return false; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("HardSigmoidActivationFunction"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const { return GetTypeName(); }
    };

    template <typename ValueType>
    class TanhActivationFunction : public BroadcastUnaryFunction<ValueType>
    {
    public:
        /// <summary> Computes the tanh activation function (on the host machine) </summary>
        ///
        /// <param name="x"> The value </param>
        ///
        /// <returns> The value of the function tanh(x) </returns>
        ValueType Compute(ValueType x) const override;
        using BroadcastUnaryFunction<ValueType>::Compute;

        /// <summary> Emits IR to compute the tanh activation function </summary>
        ///
        /// <param name="x"> The value </param>
        ///
        /// <returns> The value of the function tanh(x) </returns>
        llvm::Value* Compile(emitters::IRFunctionEmitter& function, llvm::Value* x) const override;
        using BroadcastUnaryFunction<ValueType>::Compile;

        /// <summary> Indicates if the function can operate on vector types </summary>
        ///
        /// <returns> true if the function can operate on vector types </returns>
        bool CanUseVectorTypes() const { return false; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("TanhActivationFunction"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const { return GetTypeName(); }
    };

    template <typename ValueType>
    class ParametricReLUActivationFunction
    {
    public:
        /// <summary> Computes the Parametric ReLU activation function (on the host machine) </summary>
        ///
        /// <param name="x"> The value </param>
        /// <param name="a"> The learned alpha parameter </param>
        ///
        /// <returns> The value of the function PReLU(x, a) </returns>
        ValueType Compute(ValueType x, ValueType a) const;

        /// <summary> Emits IR to compute the Parametric ReLU activation function </summary>
        ///
        /// <param name="x"> The value </param>
        /// <param name="a"> The learned alpha parameter. </param>
        ///
        /// <returns> The value of the function PReLU(x, a) </returns>
        llvm::Value* Compile(emitters::IRFunctionEmitter& function, llvm::Value* x, llvm::Value* a) const;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("ParametricReLUActivationFunction"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const { return GetTypeName(); }
    };

    //
    // Helper functions
    //
    template <typename ValueType>
    ReLUActivationFunction<ValueType> GetNodeActivationFunction(const predictors::neural::ReLUActivation<ValueType>& f);

    template <typename ValueType>
    LeakyReLUActivationFunction<ValueType> GetNodeActivationFunction(const predictors::neural::LeakyReLUActivation<ValueType>& f);

    template <typename ValueType>
    HardSigmoidActivationFunction<ValueType> GetNodeActivationFunction(const predictors::neural::HardSigmoidActivation<ValueType>& f);

    template <typename ValueType>
    ParametricReLUActivationFunction<ValueType> GetNodeActivationFunction(const predictors::neural::ParametricReLUActivation<ValueType>& f);

    template <typename ValueType>
    SigmoidActivationFunction<ValueType> GetNodeActivationFunction(const predictors::neural::SigmoidActivation<ValueType>& f);

    template <typename ValueType>
    TanhActivationFunction<ValueType> GetNodeActivationFunction(const predictors::neural::TanhActivation<ValueType>& f);
}
}
