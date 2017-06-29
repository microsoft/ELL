////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ScalingLayerNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "NeuralNetworkLayerNode.h"

// model
#include "IRMapCompiler.h"
#include "ModelTransformer.h"
#include "PortElements.h"

// predictors
#include "ScalingLayer.h"

// stl
#include <string>
#include <type_traits>

namespace ell
{
namespace nodes
{
    /// <summary> A node that wraps a neural net ScalingLayer. </summary>
    template <typename ValueType>
    class ScalingLayerNode : public NeuralNetworkLayerNode<ScalingLayerNode<ValueType>, predictors::neural::ScalingLayer<ValueType>, ValueType>
    {
    public:
        using LayerType = predictors::neural::ScalingLayer<ValueType>;
        using BaseType = NeuralNetworkLayerNode<ScalingLayerNode<ValueType>, predictors::neural::ScalingLayer<ValueType>, ValueType>;

        /// @name Input and Output Ports
        /// @{
        using BaseType::inputPortName; // "input"
        using BaseType::outputPortName; // "output"
        using BaseType::input;
        using BaseType::output;
        /// @}

        ScalingLayerNode() = default;
        
        /// <summary> Constructor from a layer. </summary>
        ///
        /// <param name="input"> </param>
        /// <param name="layer"> The layer to wrap. </param>
        ScalingLayerNode(const model::PortElements<ValueType>& input, const predictors::neural::ScalingLayer<ValueType>& layer);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("ScalingLayerNode"); }

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
