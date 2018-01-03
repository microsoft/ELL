////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SoftmaxLayerNode.h (nodes)
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
#include "PortMemoryLayout.h"

// predictors
#include "SoftmaxLayer.h"

// stl
#include <string>
#include <type_traits>

namespace ell
{
namespace nodes
{
    /// <summary> A node that wraps a neural net SoftmaxLayer. </summary>
    template <typename ValueType>
    class SoftmaxLayerNode : public NeuralNetworkLayerNode<SoftmaxLayerNode<ValueType>, predictors::neural::SoftmaxLayer<ValueType>, ValueType>
    {
    public:
        using LayerType = predictors::neural::SoftmaxLayer<ValueType>;
        using BaseType = NeuralNetworkLayerNode<SoftmaxLayerNode<ValueType>, predictors::neural::SoftmaxLayer<ValueType>, ValueType>;

        /// @name Input and Output Ports
        /// @{
        using BaseType::input;
        using BaseType::output;
        /// @}

        SoftmaxLayerNode() = default;
        
        /// <summary> Constructor from a layer. </summary>
        ///
        /// <param name="input"> </param>
        /// <param name="layer"> The bias layer to wrap. </param>
        SoftmaxLayerNode(const model::PortElements<ValueType>& input, const predictors::neural::SoftmaxLayer<ValueType>& layer);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("SoftmaxLayerNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Indicates if this node is able to compile itself to code. </summary>
        bool IsCompilable(const model::MapCompiler* compiler) const override { return true; }

    protected:
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        using BaseType::HasState;

    private:
        // Helper for generating nested loops to visit all input/output values
        template <typename FunctionType>
        void EmitComputeDimensionLoop(model::IRMapCompiler& compiler,
                                      emitters::IRFunctionEmitter& function,
                                      size_t dimension,
                                      const model::PortMemoryLayout& inputLayout,
                                      const model::PortMemoryLayout& outputLayout,
                                      llvm::Value* pInput,
                                      llvm::Value* pOutput,
                                      llvm::Value* prevInputDimensionOffset,
                                      llvm::Value* prevOutputDimensionOffset,
                                      FunctionType& f) const;

        // in-place version
        template <typename FunctionType>
        void EmitComputeDimensionLoop(model::IRMapCompiler& compiler,
                                      emitters::IRFunctionEmitter& function,
                                      size_t dimension,
                                      const model::PortMemoryLayout& inputLayout,
                                      llvm::Value* pInput,
                                      llvm::Value* prevInputDimensionOffset,
                                      FunctionType& f) const;
    };
}
}
