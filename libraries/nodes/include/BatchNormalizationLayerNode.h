////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BatchNormalizationLayerNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/IRMapCompiler.h>
#include <model/include/ModelTransformer.h>
#include "NeuralNetworkLayerNode.h"
#include <model/include/PortElements.h>

#include <predictors/neural/include/BatchNormalizationLayer.h>

#include <string>
#include <type_traits>

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    class BatchNormalizationLayerNode : public NeuralNetworkLayerNode<BatchNormalizationLayerNode<ValueType>, predictors::neural::BatchNormalizationLayer<ValueType>, ValueType>
    {
    public:
        using LayerType = predictors::neural::BatchNormalizationLayer<ValueType>;
        using BaseType = NeuralNetworkLayerNode<BatchNormalizationLayerNode<ValueType>, predictors::neural::BatchNormalizationLayer<ValueType>, ValueType>;

        /// @name Input and Output Ports
        /// @{
        using BaseType::input;
        using BaseType::output;
        /// @}

        BatchNormalizationLayerNode() = default;

        /// <summary> Constructor from a layer. </summary>
        ///
        /// <param name="input"> </param>
        /// <param name="layer"> The batch normalization layer to wrap. </param>
        BatchNormalizationLayerNode(const model::OutputPort<ValueType>& input, const predictors::neural::BatchNormalizationLayer<ValueType>& layer);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("BatchNormalizationLayerNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Indicates if this node is able to compile itself to code. </summary>
        bool IsCompilable(const model::MapCompiler* compiler) const override { return false; }

    protected:
        bool Refine(model::ModelTransformer& transformer) const override;

    private:
        void Copy(model::ModelTransformer& transformer) const override;
    };
} // namespace nodes
} // namespace ell
