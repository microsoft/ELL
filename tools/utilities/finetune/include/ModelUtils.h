////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelUtils.h (finetune)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DataUtils.h"
#include "OptimizationUtils.h"

#include <math/include/Matrix.h>
#include <math/include/Vector.h>

#include <model/include/InputNode.h>
#include <model/include/Model.h>
#include <model/include/OutputNode.h>
#include <model/include/OutputPort.h>

#include <nodes/include/BiasLayerNode.h>
#include <nodes/include/ConvolutionalLayerNode.h>
#include <nodes/include/FullyConnectedLayerNode.h>

#include <string>
#include <vector>

// I/O
void SaveModel(const ell::model::Model& model, const ell::model::OutputPortBase& output, std::string filename);

// Querying nodes
bool IsSourceNode(const ell::model::Node* node);
bool IsSinkNode(const ell::model::Node* node);
bool IsClockNode(const ell::model::Node* node);
bool IsNeuralNetworkPredictorNode(const ell::model::Node* node);
bool IsFullyConnectedLayerNode(const ell::model::Node* node);
bool IsBiasLayerNode(const ell::model::Node* node);
bool IsConvolutionalLayerNode(const ell::model::Node* node);

template <typename T>
bool IsSourceNode(const ell::model::Node* node);
template <typename T>
bool IsSinkNode(const ell::model::Node* node);
template <typename T>
bool IsNeuralNetworkPredictorNode(const ell::model::Node* node);
template <typename T>
bool IsFullyConnectedLayerNode(const ell::model::Node* node);
template <typename T>
bool IsBiasLayerNode(const ell::model::Node* node);
template <typename T>
bool IsConvolutionalLayerNode(const ell::model::Node* node);

// Finding things
ell::model::InputNodeBase* GetInputNode(ell::model::Model& model, const ell::model::OutputPortBase& output);

template <typename ElementType>
ell::model::InputNode<ElementType>* GetInputNode(ell::model::Model& model, const ell::model::OutputPortBase& output);

const ell::model::OutputNodeBase* GetOutputNode(const ell::model::Model& model);

template <typename ElementType>
const ell::model::OutputNode<ElementType>* GetOutputNode(const ell::model::Model& model);

template <typename ElementType>
std::vector<const ell::nodes::FullyConnectedLayerNode<ElementType>*> GetFullyConnectedLayerNodes(const ell::model::Model& model, const ell::model::OutputPortBase& output);

template <typename ElementType>
std::vector<const ell::nodes::BiasLayerNode<ElementType>*> GetBiasLayerNodes(const ell::model::Model& model, const ell::model::OutputPortBase& output);

template <typename ElementType>
std::vector<const ell::nodes::ConvolutionalLayerNode<ElementType>*> GetConvolutionalLayerNodes(const ell::model::Model& model, const ell::model::OutputPortBase& output);

// Appending nodes or sets of nodes to models
template <typename ElementType>
const ell::model::OutputPort<ElementType>& AppendSink(ell::model::Model& model, const ell::model::OutputPort<ElementType>& output);

template <typename ElementType>
const ell::model::OutputPort<ElementType>& AppendOutputWithSink(ell::model::Model& model, const ell::model::OutputPort<ElementType>& output);

template <typename ElementType>
const ell::model::OutputPort<ElementType>& AppendBiasLayer(ell::model::Model& model, const ell::model::OutputPort<ElementType>& output, ell::math::ConstColumnVectorReference<ElementType> bias);

template <typename ElementType>
const ell::model::OutputPort<ElementType>& AppendBiasLayer(ell::model::Model& model, const ell::model::OutputPort<ElementType>& output, const std::vector<ElementType>& bias);

template <typename ElementType>
const ell::model::OutputPort<ElementType>& AppendScalingLayer(ell::model::Model& model, const ell::model::OutputPort<ElementType>& output, ell::math::ConstColumnVectorReference<ElementType> scale);

template <typename ElementType>
const ell::model::OutputPort<ElementType>& AppendScalingLayer(ell::model::Model& model, const ell::model::OutputPort<ElementType>& output, const std::vector<ElementType>& scale);

template <typename ElementType>
const ell::model::OutputPort<ElementType>& AppendFullyConnectedLayer(ell::model::Model& model, const ell::model::OutputPort<ElementType>& output, ell::math::ConstRowMatrixReference<ElementType> weights);

template <typename ElementType>
const ell::model::OutputPort<ElementType>& AppendConvolutionalLayer(ell::model::Model& model, const ell::model::OutputPort<ElementType>& output, int filterSize, int stride, int inputPadding, int outputPadding, ell::math::ConstRowMatrixReference<ElementType> weights);

template <typename ElementType>
const ell::model::OutputPort<ElementType>& AppendFineTunedFullyConnectedNodes(ell::model::Model& model, const ell::model::OutputPort<ElementType>& output, const WeightsAndBias<ElementType>& weightsAndBias);

template <typename ElementType>
const ell::model::OutputPort<ElementType>& AppendFineTunedConvolutionalNodes(ell::model::Model& model, const ell::model::OutputPort<ElementType>& output, int filterSize, int stride, int inputPadding, int outputPadding, const WeightsAndBias<ElementType>& weightsAndBias);

// Model I/O node manipulation
const ell::model::OutputPortBase& RemoveSourceAndSinkNodes(ell::model::Model& model, const ell::model::OutputPortBase& output);

template <typename ElementType>
const ell::model::OutputPort<ElementType>& RemoveSourceAndSinkNodes(ell::model::Model& model, const ell::model::OutputPort<ElementType>& output);
