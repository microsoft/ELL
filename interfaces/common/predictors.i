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
        numAnchors: Number of anchors per region. Typically this is 4
        applySoftmax: Specifies whether softmax should be applied to class probabilites
%}

// Include the C++ code to be wrapped
%include "NeuralLayersInterface.h"

%template(LayerVector) std::vector<ell::api::predictors::neural::Layer*>;
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
