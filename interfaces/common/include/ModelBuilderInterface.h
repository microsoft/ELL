////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelBuilderInterface.h (interfaces)
//  Authors:  Chuck Jacobs, Kirk Olynyk, Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#ifndef SWIG

#include "ModelInterface.h"
#include "NeuralNetworkPredictorInterface.h"

#include <math/include/Tensor.h>

#include <model/include/Model.h>
#include <model/include/ModelBuilder.h>

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#endif

namespace ELL_API
{
//
// ModelBuilder
//
class ModelBuilder
{
public:
    ModelBuilder();

    // Generic AddNode method
    Node AddNode(Model model, const std::string& nodeType, const std::vector<std::string>& args);

    // Specific methods per node type
    Node AddBinaryOperationNode(Model model, PortElements input1, PortElements input2, BinaryOperationType operation);
    Node AddBufferNode(Model model, PortElements input, int windowSize);
    Node AddTypeCastNode(Model model, PortElements input, PortType outputType);
    Node AddClockNode(Model model, PortElements input, double interval, double lagThreshold, const std::string& lagNotificationName);
    Node AddConcatenationNode(Model model, const PortMemoryLayout& outputMemoryLayout, const std::vector<PortElements*>& inputs);
    Node AddSpliceNode(Model model, const std::vector<PortElements*>& inputs);
    Node AddConstantNode(Model model, std::vector<double> values, PortType type);
    Node AddConstantNode(Model model, std::vector<double> values, const PortMemoryLayout& outputMemoryLayout, PortType type);
    Node AddDCTNode(Model model, PortElements input, int numFilters);
    Node AddMatrixMultiplyNode(Model model, PortElements input1, PortElements input2);
    Node AddMatrixMatrixMultiplyNode(Model model, PortElements input1, PortElements input2);
    Node AddMatrixMatrixMultiplyCodeNode(Model model, PortElements input1, PortElements input2, int gemmImpl);
    Node AddMatrixMatrixMultiplyCodeNode(Model model, PortElements input1, PortElements input2, int panelM, int panelN, int panelK, int kernelM, int kernelN, int kernelK, int gemmImpl);
    Node AddDotProductNode(Model model, PortElements input1, PortElements input2);
    Node AddNeuralNetworkPredictorNode(Model model, PortElements input, ell::api::predictors::NeuralNetworkPredictor predictor);
    Node AddFFTNode(Model model, PortElements input, int nfft = 0);
    Node AddHannWindowNode(Model model, PortElements input);
    Node AddHammingWindowNode(Model model, PortElements input);
    Node AddIIRFilterNode(Model model, PortElements input, std::vector<double> bCoeffs, std::vector<double> aCoeffs);
    InputNode AddInputNode(Model model, const PortMemoryLayout& memoryLayout, PortType type);
    Node AddLinearFilterBankNode(Model model, PortElements input, double sampleRate, int numFilters, int numFiltersToUse, double offset = 0);
    Node AddMelFilterBankNode(Model model, PortElements input, double sampleRate, int fftSize, int numFilters, int numFiltersToUse, double offset = 0);
    OutputNode AddOutputNode(Model model, const PortMemoryLayout& memoryLayout, PortElements input);
    Node AddReinterpretLayoutNode(Model model, PortElements input, PortMemoryLayout outputMemoryLayout);
    Node AddReorderDataNode(Model model, PortElements input, PortMemoryLayout inputMemoryLayout, PortMemoryLayout outputMemoryLayout, std::vector<int> order = {}, double outputPaddingValue = 0.0);
    Node AddReorderDataNode(Model model, PortElements input, std::vector<int> order);
    SinkNode AddSinkNode(Model model, PortElements input, const PortMemoryLayout& memoryLayout, const std::string& sinkFunctionName, PortElements trigger = PortElements());
    SourceNode AddSourceNode(Model model, PortElements input, PortType outputType, const PortMemoryLayout& memoryLayout, const std::string& sourceFunctionName);
    Node AddUnaryOperationNode(Model model, PortElements input, UnaryOperationType operation);
    Node AddDTWNode(Model model, std::vector<std::vector<double>> prototype, PortElements input);
    Node AddVoiceActivityDetectorNode(Model model, PortElements input, double sampleRate, double frameDuration, double tauUp, double tauDown, double largeInput, double gainAtt, double thresholdUp, double thresholdDown, double levelThreshold);
    Node AddRNNNode(Model model, PortElements input, PortElements reset, size_t hiddenUnits, PortElements inputWeights, PortElements hiddenWeights, PortElements inputBias, PortElements hiddenBias, ell::api::predictors::neural::ActivationType activation);
    Node AddGRUNode(Model model, PortElements input, PortElements reset, size_t hiddenUnits, PortElements inputWeights, PortElements hiddenWeights, PortElements inputBias, PortElements hiddenBias, ell::api::predictors::neural::ActivationType activation, ell::api::predictors::neural::ActivationType recurrentActivation);
    Node AddFastGRNNNode(Model model, PortElements input, PortElements reset, size_t hiddenUnits, size_t wRank, size_t uRank, PortElements W1, PortElements W2, PortElements U1, PortElements U2, PortElements biasGate, PortElements biasUpdate, PortElements zeta, PortElements nu, ell::api::predictors::neural::ActivationType gateNonlinearity, ell::api::predictors::neural::ActivationType updateNonlinearity);
    Node AddLSTMNode(Model model, PortElements input, PortElements reset, size_t hiddenUnits, PortElements inputWeights, PortElements hiddenWeights, PortElements inputBias, PortElements hiddenBias, ell::api::predictors::neural::ActivationType activation, ell::api::predictors::neural::ActivationType recurrentActivation);

    // Layer nodes (going away...)
    Node AddActivationLayerNode(Model model, PortElements input, const ell::api::predictors::neural::ActivationLayer& layer);
    Node AddBatchNormalizationLayerNode(Model model, PortElements input, const ell::api::predictors::neural::BatchNormalizationLayer& layer);
    Node AddBiasLayerNode(Model model, PortElements input, const ell::api::predictors::neural::BiasLayer& layer);
    Node AddBinaryConvolutionalLayerNode(Model model, PortElements input, const ell::api::predictors::neural::BinaryConvolutionalLayer& layer);
    Node AddConvolutionalLayerNode(Model model, PortElements input, const ell::api::predictors::neural::ConvolutionalLayer& layer);
    Node AddFullyConnectedLayerNode(Model model, PortElements input, const ell::api::predictors::neural::FullyConnectedLayer& layer);
    Node AddRegionDetectionLayerNode(Model model, PortElements input, const ell::api::predictors::neural::RegionDetectionLayer& layer);
    Node AddPoolingLayerNode(Model model, PortElements input, const ell::api::predictors::neural::PoolingLayer& layer);
    Node AddScalingLayerNode(Model model, PortElements input, const ell::api::predictors::neural::ScalingLayer& layer);
    Node AddSoftmaxLayerNode(Model model, PortElements input, const ell::api::predictors::neural::SoftmaxLayer& layer);

private:
#ifndef SWIG
    template <typename ElementType>
    Node AddNeuralNetworkPredictorNode(Model model, PortElements input, const ell::predictors::NeuralNetworkPredictor<ElementType>& predictor);

    template <typename ElementType>
    Node AddActivationLayerNode(Model model, PortElements input, const ell::api::predictors::neural::ActivationLayer& layer);

    template <typename ElementType>
    Node AddBatchNormalizationLayerNode(Model model, PortElements input, const ell::api::predictors::neural::BatchNormalizationLayer& layer);

    template <typename ElementType>
    Node AddBiasLayerNode(Model model, PortElements input, const ell::api::predictors::neural::BiasLayer& layer);

    template <typename ElementType>
    Node AddBinaryConvolutionalLayerNode(Model model, PortElements input, const ell::api::predictors::neural::BinaryConvolutionalLayer& layer);

    template <typename ElementType>
    Node AddConvolutionalLayerNode(Model model, PortElements input, const ell::api::predictors::neural::ConvolutionalLayer& layer);

    template <typename ElementType>
    Node AddFullyConnectedLayerNode(Model model, PortElements input, const ell::api::predictors::neural::FullyConnectedLayer& layer);

    template <typename ElementType>
    Node AddRegionDetectionLayerNode(Model model, PortElements input, const ell::api::predictors::neural::RegionDetectionLayer& layer);

    template <typename ElementType>
    Node AddPoolingLayerNode(Model model, PortElements input, const ell::api::predictors::neural::PoolingLayer& layer);

    template <typename ElementType>
    Node AddScalingLayerNode(Model model, PortElements input, const ell::api::predictors::neural::ScalingLayer& layer);

    template <typename ElementType>
    Node AddSoftmaxLayerNode(Model model, PortElements input, const ell::api::predictors::neural::SoftmaxLayer& layer);

    template <typename ElementType>
    Node AddRNNNode(Model model, PortElements input, PortElements reset, size_t hiddenUnits, PortElements inputWeights, PortElements hiddenWeights, PortElements inputBias, PortElements hiddenBias, ell::api::predictors::neural::ActivationType activation);

    template <typename ElementType>
    Node AddLSTMNode(Model model, PortElements input, PortElements reset, size_t hiddenUnits, PortElements inputWeights, PortElements hiddenWeights, PortElements inputBias, PortElements hiddenBias, ell::api::predictors::neural::ActivationType activation, ell::api::predictors::neural::ActivationType recurrentActivation);

    template <typename ElementType>
    Node AddGRUNode(Model model, PortElements input, PortElements reset, size_t hiddenUnits, PortElements inputWeights, PortElements hiddenWeights, PortElements inputBias, PortElements hiddenBias, ell::api::predictors::neural::ActivationType activation, ell::api::predictors::neural::ActivationType recurrentActivation);

    template <typename ElementType>
    Node AddFastGRNNNode(Model model, PortElements input, PortElements reset, size_t hiddenUnits, size_t wRank, size_t uRank, PortElements W1, PortElements W2, PortElements U1, PortElements U2, PortElements biasGate, PortElements biasUpdate, PortElements zeta, PortElements nu, ell::api::predictors::neural::ActivationType gateNonlinearity, ell::api::predictors::neural::ActivationType updateNonlinearity);

    template <typename ElementType>
    void InternalResetInput(Node node, PortElements input, std::string input_port_name);

#endif

    ell::model::ModelBuilder _modelBuilder;
};

#ifndef SWIG
template <typename OutputType, typename InputType>
std::vector<OutputType> CastVector(const std::vector<InputType>& vector);
#endif

} // namespace ELL_API
