////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     NeuralNetworkPredictorNode.h (nodes)
//  Authors:  Chuck Jacobs, Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "Model.h"
#include "ModelTransformer.h"
#include "Node.h"

// nodes
#include "ActivationLayerNode.h"
#include "BatchNormalizationLayerNode.h"
#include "BiasLayerNode.h"
#include "BinaryConvolutionalLayerNode.h"
#include "ConvolutionalLayerNode.h"
#include "FullyConnectedLayerNode.h"
#include "GRULayerNode.h"
#include "LSTMLayerNode.h"
#include "PoolingLayerNode.h"
#include "RegionDetectionLayerNode.h"
#include "ScalingLayerNode.h"
#include "SoftmaxLayerNode.h"

// predictors
#include "NeuralNetworkPredictor.h"

// activation and pooling functions
#include "LeakyReLUActivation.h"
#include "MaxPoolingFunction.h"
#include "MeanPoolingFunction.h"
#include "ParametricReLUActivation.h"
#include "ReLUActivation.h"
#include "SigmoidActivation.h"

// utilities
#include "TypeName.h"

// stl
#include <functional>
#include <string>
#include <type_traits>

namespace ell
{
namespace nodes
{
/// <summary> A node that represents a neural network. </summary>
template <typename ValueType>
class NeuralNetworkPredictorNode : public model::Node
{
public:
    /// @name Input and Output Ports
    /// @{
    const model::InputPort<ValueType>& input = _input;
    const model::OutputPort<ValueType>& output = _output;
    /// @}

    using PredictorType = typename predictors::NeuralNetworkPredictor<ValueType>;
    using Layer = typename predictors::neural::Layer<ValueType>;

    /// <summary> Default Constructor </summary>
    NeuralNetworkPredictorNode();

    /// <summary> Constructor </summary>
    ///
    /// <param name="input"> The signal to predict from </param>
    /// <param name="predictor"> The predictor to use when making the prediction. </param>
    NeuralNetworkPredictorNode(const model::PortElements<ValueType>& input, const PredictorType& predictor);

    /// <summary> Returns the underlying predictor </summary>
    ///
    /// <returns> The predictor wrapped by this node </returns>
    const PredictorType& GetPredictor() const { return _predictor; }

    /// <summary> Returns the underlying predictor </summary>
    ///
    /// <returns> The predictor wrapped by this node </returns>
    PredictorType& GetPredictor() { return _predictor; }

    /// <summary> Gets the name of this type (for serialization). </summary>
    ///
    /// <returns> The name of this type. </returns>
    static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("NeuralNetworkPredictorNode"); }

    /// <summary> Gets the name of this type (for serialization). </summary>
    ///
    /// <returns> The name of this type. </returns>
    std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    /// <summary> Makes a copy of this node in the model being constructed by the transformer </summary>
    void Copy(model::ModelTransformer& transformer) const override;

    /// <summary> Options to control how the network is compiled into nodes </summary>
    struct NetworkCompileOptions
    {
        /// <summary> Use diagonal convolution (vs. im2col-based convolution)
        bool useDiagonalConvolution;

        /// <summary> Ensure the output of the nodes implementing a layer is in the canonical row, column, channel order </summary>
        bool alwaysConvertToInterleaved;

        /// <summary> When using im2col-based convolution, construct the transpose of the receptive field matrix </summary>
        bool transposeReceptiveFieldMatrix;
    };

    struct NetworkCompileState
    {
        /// <summary> Indicates the current order of input data. If `true`, it's in the canonical row, column, channel order. </summary>
        bool isInterleavedOrder;
    };

protected:
    void Compute() const override;
    bool Refine(model::ModelTransformer& transformer) const override;
    void WriteToArchive(utilities::Archiver& archiver) const override;
    void ReadFromArchive(utilities::Unarchiver& archiver) override;

private:
    NeuralNetworkLayerNodeBase<ValueType>* AddLayerNode(model::ModelTransformer& transformer, Layer& layer, const model::PortElements<ValueType>& layerInputs, const NetworkCompileOptions& options, NetworkCompileState& state) const;

    // Input
    model::InputPort<ValueType> _input;

    // Output
    model::OutputPort<ValueType> _output;

    // Pointer to the predictor
    PredictorType _predictor;
};
}
}

#include "../tcc/NeuralNetworkPredictorNode.tcc"
