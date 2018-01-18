////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BiasLayerNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "IRMapCompiler.h"
#include "ModelTransformer.h"
#include "NeuralNetworkLayerNode.h"
#include "PortElements.h"

// predictors
#include "BiasLayer.h"

// stl
#include <string>
#include <type_traits>

namespace ell
{
namespace nodes
{
    /// <summary> A node that wraps a neural net BiasLayer. </summary>
    template <typename ValueType>
    class BiasLayerNode : public NeuralNetworkLayerNode<BiasLayerNode<ValueType>, predictors::neural::BiasLayer<ValueType>, ValueType>
    {
    public:
        using LayerType = predictors::neural::BiasLayer<ValueType>;
        using BaseType = NeuralNetworkLayerNode<BiasLayerNode<ValueType>, predictors::neural::BiasLayer<ValueType>, ValueType>;

        /// @name Input and Output Ports
        /// @{
        using BaseType::input;
        using BaseType::output;
        /// @}

        BiasLayerNode() = default;

        /// <summary> Constructor from a layer. </summary>
        ///
        /// <param name="input"> </param>
        /// <param name="layer"> The bias layer to wrap. </param>
        BiasLayerNode(const model::PortElements<ValueType>& input, const predictors::neural::BiasLayer<ValueType>& layer);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("BiasLayerNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Indicates if this node is able to compile itself to code. </summary>
        bool IsCompilable(const model::MapCompiler* compiler) const override { return false; }

    protected:
        bool Refine(model::ModelTransformer& transformer) const override;
    };
}
}
