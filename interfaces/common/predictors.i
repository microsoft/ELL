////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Predictiors.i (interfaces)
//  Authors:  Chuck Jacobs, Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%ignore ell::predictors::LinearPredictor::GetWeights() const;
%ignore ell::predictors::LinearPredictor::GetBias() const;

%{
#include "NeuralLayersInterface.h"
using PaddingParameters = ell::predictors::neural::PaddingParameters;
#include "NeuralNetworkPredictorInterface.h"
%}

// Include language specific SWIG definitions that must be declared before the
// C++ code to be wrapped
#ifdef SWIGPYTHON
    %include "predictors_python_pre.i"
#elif SWIGJAVASCRIPT
    %include "predictors_javascript_pre.i"
#endif

// Documentation
%feature("docstring") LayerParameters::inputShape "TensorShape representing input dimensions"
%feature("docstring") LayerParameters::outputShape "TensorShape representing output dimensions"
%feature("docstring") PaddingParameters::PaddingParameters %{
    PaddingParameters(scheme, size)
        scheme: one of PaddingScheme values
        size: size of the padding on both sides of row and column dimensions
%}
%feature("docstring") BinaryConvolutionalParameters::BinaryConvolutionalParameters %{
    BinaryConvolutionalParameters(field, stride, binaryConvolutionMethod, binaryWeightsScale)
        field: size of the receptive field in row and column dimensions
        stride: size of stride in row and column dimensions
        binaryConvolutionMethod: one of BinaryConvolutionMethod values
        binaryWeightsScale: scale to apply to the binarized weights
%}
%feature("docstring") ConvolutionalParameters::ConvolutionalParameters %{
    ConvolutionalParameters(field, stride, convolutionMethod, filterBatchSize)
        field: size of the receptive field in row and column dimensions
        stride: size of stride in row and column dimensions
        convolutionMethod: one of ConvolutionMethod values
        filterBatchSize: number of filters to use at a time when using the diagonal method, from 1 to total number of filters
%}
%feature("docstring") PoolingParameters::PoolingParameters %{
    PoolingParameters(poolingSize, stride)
        poolingSize: size of the pooling field in row and column dimensions
        stride: size of stride in row and column dimensions
%}
%feature("docstring") RegionDetectionParameters::RegionDetectionParameters %{
    RegionDetectionParameters(width, height, numBoxesPerCell, numClasses, numCoordinates)
        width: width of the input
        height: height of the input
        numBoxesPerCell: number of possible bounding boxes per cell
        numClasses: number of classes that can be detected
        numCoordinates: number of coordinates per region. Currently, only supported value is 4
%}

// Include the C++ code to be wrapped
%include "NeuralLayersInterface.h"
%include "NeuralNetworkPredictorInterface.h"

#ifndef SWIGXML
// Include C++ cheaders that will be ignored, except for specific classes we will bring in
// for wrapping by SWIG. This is to share POD types, enums and so on between Api and core code
%rename($ignore, %$isclass) ""; // Ignore all classes by default, we will expose specific ones we need
%rename("%s") BinaryConvolutionalParameters; // Expose BinaryConvolutionalParameters
%rename("%s") BinaryConvolutionMethod; // Expose BinaryConvolutionMethod
%rename("%s") ConvolutionalParameters; // Expose ConvolutionalParameters
%rename("%s") ConvolutionMethod; // Expose ConvolutionMethod
%rename("%s") LayerType; // Expose LayerType
%rename("%s") PaddingParameters; // Expose PaddingParameters
%rename("%s") PaddingScheme; // Expose PaddingScheme
%rename("%s") PoolingParameters; // Expose PoolingParameters
%rename("%s") RegionDetectionParameters; // Expose RegionDetectionParameters
%ignore ell::predictors::neural::Layer::LayerParameters;
%include <Layer.h>
%include <BinaryConvolutionalLayer.h>
%include <ConvolutionalLayer.h>
%include <PoolingLayer.h>
%include <RegionDetectionLayer.h>

// Template instantiations

// These need to be defined before the other layer types
%template(FloatLayer) ell::api::predictors::neural::Layer<float>;
%template(FloatLayerVector) std::vector<ell::api::predictors::neural::Layer<float>*>;
%template(DoubleLayer) ell::api::predictors::neural::Layer<double>;
%template(DoubleLayerVector) std::vector<ell::api::predictors::neural::Layer<double>*>;

%template(FloatActivationLayer) ell::api::predictors::neural::ActivationLayer<float>;
%template(FloatBatchNormalizationLayer) ell::api::predictors::neural::BatchNormalizationLayer<float>;
%template(FloatBiasLayer) ell::api::predictors::neural::BiasLayer<float>;
%template(FloatBinaryConvolutionalLayer) ell::api::predictors::neural::BinaryConvolutionalLayer<float>;
%template(FloatConvolutionalLayer) ell::api::predictors::neural::ConvolutionalLayer<float>;
%template(FloatFullyConnectedLayer) ell::api::predictors::neural::FullyConnectedLayer<float>;
%template(FloatGRULayer) ell::api::predictors::neural::GRULayer<float>;
%template(FloatLSTMLayer) ell::api::predictors::neural::LSTMLayer<float>;
%template(FloatNeuralNetworkPredictor) ell::api::predictors::NeuralNetworkPredictor<float>;
%template(FloatPoolingLayer) ell::api::predictors::neural::PoolingLayer<float>;
%template(FloatPReLUActivationLayer) ell::api::predictors::neural::PReLUActivationLayer<float>;
%template(FloatRegionDetectionLayer) ell::api::predictors::neural::RegionDetectionLayer<float>;
%template(FloatScalingLayer) ell::api::predictors::neural::ScalingLayer<float>;
%template(FloatSoftmaxLayer) ell::api::predictors::neural::SoftmaxLayer<float>;

%template(DoubleActivationLayer) ell::api::predictors::neural::ActivationLayer<double>;
%template(DoubleBatchNormalizationLayer) ell::api::predictors::neural::BatchNormalizationLayer<double>;
%template(DoubleBiasLayer) ell::api::predictors::neural::BiasLayer<double>;
%template(DoubleBinaryConvolutionalLayer) ell::api::predictors::neural::BinaryConvolutionalLayer<double>;
%template(DoubleConvolutionalLayer) ell::api::predictors::neural::ConvolutionalLayer<double>;
%template(DoubleFullyConnectedLayer) ell::api::predictors::neural::FullyConnectedLayer<double>;
%template(DoubleGRULayer) ell::api::predictors::neural::GRULayer<double>;
%template(DoubleLSTMLayer) ell::api::predictors::neural::LSTMLayer<double>;
%template(DoubleNeuralNetworkPredictor) ell::api::predictors::NeuralNetworkPredictor<double>;
%template(DoublePoolingLayer) ell::api::predictors::neural::PoolingLayer<double>;
%template(DoublePReLUActivationLayer) ell::api::predictors::neural::PReLUActivationLayer<double>;
%template(DoubleRegionDetectionLayer) ell::api::predictors::neural::RegionDetectionLayer<double>;
%template(DoubleScalingLayer) ell::api::predictors::neural::ScalingLayer<double>;
%template(DoubleSoftmaxLayer) ell::api::predictors::neural::SoftmaxLayer<double>;

// Restore environment
%rename("%s") ""; // Unignore everything

#endif

// Include language specific SWIG definitions that must be declared after the
// C++ code has been wrapped by SWIG
#ifdef SWIGPYTHON
    %include "predictors_python_post.i"
#elif SWIGJAVASCRIPT
    %include "predictors_javascript_post.i"
#endif
