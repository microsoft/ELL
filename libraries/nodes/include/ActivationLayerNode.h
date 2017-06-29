////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ActivationLayerNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "BroadcastFunctionNode.h"
#include "NeuralNetworkLayerNode.h"

// model
#include "IRMapCompiler.h"
#include "ModelTransformer.h"
#include "PortElements.h"

// predictors
#include "ActivationLayer.h"

// stl
#include <string>
#include <type_traits>

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
        virtual ~ReLUActivationFunction() {}

        /// <summary> Computes the ReLU activation (on the host machine) </summary>
        ///
        /// <param name="x"> The value </param>
        /// <returns> The value the function ReLU(x) </returns>
        virtual ValueType Compute(ValueType x) const override;        
        using BroadcastUnaryFunction<ValueType>::Compute;

        /// <summary> Emits IR to compute the ReLU activation function </summary>
        ///
        /// <param name="function"> The function being compiled. </param>
        /// <param name="x"> The primary value </param>
        /// <returns> The value the function ReLU(x) </returns>
        virtual llvm::Value* Compile(emitters::IRFunctionEmitter& function, llvm::Value* x) const override;
        using BroadcastUnaryFunction<ValueType>::Compile;

        /// <summary> Indicates if the function can operate on vector types </summary>
        bool CanUseVectorTypes() const { return false; }
    };

    template <typename ValueType>
    class LeakyReLUActivationFunction : public BroadcastUnaryFunction<ValueType>
    {
    public:
        /// <summary> Constructor specifying the leaky factor. </summary>
        ///
        /// <param name="leakyFactor"> The leaky factor. </param>
        LeakyReLUActivationFunction(ValueType leakyFactor)
            : _leakyFactor(leakyFactor) {}

        /// <summary> Computes the leaky ReLU activation (on the host machine) </summary>
        ///
        /// <param name="x"> The value </param>
        /// <returns> The value the function LeakyReLU(x) </returns>
        virtual ValueType Compute(ValueType x) const override;
        using BroadcastUnaryFunction<ValueType>::Compute;

        /// <summary> Emits IR to compute the leaky ReLU activation function </summary>
        ///
        /// <param name="function"> The function being compiled. </param>
        /// <param name="x"> The primary value </param>
        /// <returns> The value the function LeakyReLU(x) </returns>
        virtual llvm::Value* Compile(emitters::IRFunctionEmitter& function, llvm::Value* x) const override;
        using BroadcastUnaryFunction<ValueType>::Compile;

        /// <summary> Indicates if the function can operate on vector types </summary>
        bool CanUseVectorTypes() const { return false; }

        /// <summary> Gets the leaky factor </summary>
        ///
        /// <returns> The leaky factor </returns>
        ValueType GetLeakyFactor() const { return _leakyFactor; }

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
        /// <returns> The value the function Sigmoid(x) </returns>
        virtual ValueType Compute(ValueType x) const override;
        using BroadcastUnaryFunction<ValueType>::Compute;

        /// <summary> Emits IR to compute a the sigmoid activation function </summary>
        ///
        /// <param name="x"> The primary value </param>
        /// <returns> The value the function Sigmoid(x) </returns>
        virtual llvm::Value* Compile(emitters::IRFunctionEmitter& function, llvm::Value* x) const override;
        using BroadcastUnaryFunction<ValueType>::Compile;

        /// <summary> Indicates if the function can operate on vector types </summary>
        bool CanUseVectorTypes() const { return false; }
    };

    //
    // The ActivationLayerNode
    //

    /// <summary> A node that wraps a neural net ActivationLayer. </summary>
    template <typename ValueType, template <typename> class ActivationFunctionType>
    class ActivationLayerNode : public NeuralNetworkLayerNode<ActivationLayerNode<ValueType, ActivationFunctionType>, predictors::neural::ActivationLayer<ValueType, ActivationFunctionType>, ValueType>
    {
    public:
        using LayerType = predictors::neural::ActivationLayer<ValueType, ActivationFunctionType>;
        using BaseType = NeuralNetworkLayerNode<ActivationLayerNode<ValueType, ActivationFunctionType>, predictors::neural::ActivationLayer<ValueType, ActivationFunctionType>, ValueType>;

        /// @name Input and Output Ports
        /// @{
        using BaseType::inputPortName; // "input"
        using BaseType::outputPortName; // "output"
        using BaseType::input;
        using BaseType::output;
        /// @}

        ActivationLayerNode() = default;

        /// <summary> Constructor from a layer. </summary>
        ///
        /// <param name="input"> </param>
        /// <param name="layer"> The bias layer to wrap. </param>
        ActivationLayerNode(const model::PortElements<ValueType>& input, const predictors::neural::ActivationLayer<ValueType, ActivationFunctionType>& layer);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType, typename LayerType::ActivationFunction>("ActivationLayerNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Indicates if this node is able to compile itself to code. </summary>
        virtual bool IsCompilable() const override { return false; }

    protected:
        virtual bool Refine(model::ModelTransformer& transformer) const override;
    };
}
}
