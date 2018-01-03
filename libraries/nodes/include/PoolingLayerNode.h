////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PoolingLayerNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "NeuralNetworkLayerNode.h"

// model
#include "IRMapCompiler.h"
#include "ModelTransformer.h"
#include "PortElements.h"
#include "PortMemoryLayout.h"

// predictors
#include "PoolingLayer.h"

// stl
#include <string>
#include <type_traits>

namespace ell
{
namespace nodes
{
    /// <summary> A node that wraps a neural net PoolingLayer. </summary>
    template <typename ValueType, template <typename> class PoolingFunctionType>
    class PoolingLayerNode : public NeuralNetworkLayerNode<PoolingLayerNode<ValueType, PoolingFunctionType>, predictors::neural::PoolingLayer<ValueType, PoolingFunctionType>, ValueType>
    {
    public:
        using LayerType = predictors::neural::PoolingLayer<ValueType, PoolingFunctionType>;
        using BaseType = NeuralNetworkLayerNode<PoolingLayerNode<ValueType, PoolingFunctionType>, LayerType, ValueType>;

        /// @name Input and Output Ports
        /// @{
        using BaseType::input;
        using BaseType::output;
        /// @}

        PoolingLayerNode() = default;

        /// <summary> Constructor from a layer. </summary>
        ///
        /// <param name="input"> The input to the layer. </param>
        /// <param name="layer"> The bias layer to wrap. </param>
        PoolingLayerNode(const model::PortElements<ValueType>& input, const predictors::neural::PoolingLayer<ValueType, PoolingFunctionType>& layer);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType, typename LayerType::PoolingFunction>("PoolingLayerNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Indicates if this node is able to compile itself to code. </summary>
        bool IsCompilable(const model::MapCompiler* compiler) const override { return true; }

    protected:
        template <typename PoolingFunctionT>
        llvm::Value* GetPoolingWindowValue(emitters::IRFunctionEmitter& function,
                                           int windowRowStart,
                                           int windowRowEnd,
                                           int windowColumnStart,
                                           int windowColumnEnd,
                                           llvm::Value* inputRow,
                                           llvm::Value* inputColumn,
                                           llvm::Value* inputChannel,
                                           llvm::Value* inputBuffer,
                                           const model::Shape& inputIncrement,
                                           PoolingFunctionT& poolingFunction);

        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        using BaseType::HasState;
        using BaseType::GetLayer;
    };
}
}
