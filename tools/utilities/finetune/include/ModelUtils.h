////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelUtils.h (finetune)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DataStatistics.h"
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

namespace ell
{
// Types
enum class ConvolutionalNodeType
{
    spatial,
    pointwise,
    full
};

/// <summary> Relevant convolutional parameters we need to pass around. </summary>
struct ConvolutionalParameters
{
    int filterSize;
    int stride;
    bool isDepthwiseSeparable;
    int inputPadding;
    int outputPadding;
};

struct FullyConnectedParameters
{
    // nothing
};

// I/O
void SaveModel(const ell::model::OutputPortBase& output, std::string filename);

// Querying nodes
bool IsInputNode(const ell::model::Node* node);
bool IsSourceNode(const ell::model::Node* node);
bool IsSinkNode(const ell::model::Node* node);
bool IsClockNode(const ell::model::Node* node);
bool IsNeuralNetworkPredictorNode(const ell::model::Node* node);
bool IsFullyConnectedLayerNode(const ell::model::Node* node);
bool IsBiasLayerNode(const ell::model::Node* node);
bool IsConvolutionalLayerNode(const ell::model::Node* node);
ConvolutionalNodeType GetConvolutionalNodeType(const ell::model::Node* node);

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
template <typename T>
ConvolutionalNodeType GetConvolutionalNodeType(const ell::model::Node* node);

// Finding things
ell::model::InputNodeBase* GetInputNode(const ell::model::OutputPortBase& output);

// returns the input port on the first node found that consumes from an InputNode
ell::model::InputPortBase* GetInputNodeReference(const ell::model::OutputPortBase& output);

// returns the input ports on all nodes found that consume from `input`, on the subgraph that generates output
std::vector<const ell::model::InputPortBase*> GetInputReferences(const ell::model::OutputPortBase& input, const ell::model::OutputPortBase& output);

// returns the number input ports on all nodes found that consume from `input`, on the subgraph that generates output
int GetInputFanIn(const ell::model::OutputPortBase& input, const ell::model::OutputPortBase& output);

template <typename T>
const ell::model::InputPort<T>& FindFirstConsumer(const ell::model::OutputPort<T>& port);

const ell::model::OutputPortBase* FindNearestCommonAncestor(const ell::model::OutputPortBase& a, const ell::model::OutputPortBase& b);
const ell::model::OutputPortBase* FindNearestCommonAncestor(std::vector<const ell::model::OutputPortBase*> ports);

// returns true iff a is on every path from p to start
bool IsCompleteAncestor(const ell::model::OutputPortBase& p, const ell::model::OutputPortBase& a);
std::unordered_set<const ell::model::OutputPortBase*> GetCompleteAncestors(const ell::model::OutputPortBase& p);

template <typename ElementType>
ell::model::InputNode<ElementType>* GetInputNode(const ell::model::OutputPortBase& output);

const ell::model::OutputNodeBase* GetOutputNode(const ell::model::Model& model);

template <typename ElementType>
const ell::model::OutputNode<ElementType>* GetOutputNode(const ell::model::Model& model);

template <typename ElementType>
std::vector<const ell::nodes::FullyConnectedLayerNode<ElementType>*> GetFullyConnectedLayerNodes(const ell::model::OutputPortBase& output);

template <typename ElementType>
const ell::nodes::FullyConnectedLayerNode<ElementType>* GetNearestFullyConnectedLayerNode(const ell::model::OutputPortBase& output);

template <typename ElementType>
std::vector<const ell::nodes::BiasLayerNode<ElementType>*> GetBiasLayerNodes(const ell::model::OutputPortBase& output);

template <typename ElementType>
std::vector<const ell::nodes::ConvolutionalLayerNode<ElementType>*> GetConvolutionalLayerNodes(const ell::model::OutputPortBase& output);

template <typename ElementType>
const ell::nodes::ConvolutionalLayerNode<ElementType>* GetNearestConvolutionalLayerNode(const ell::model::OutputPortBase& output);

// Appending nodes or sets of nodes to models
template <typename ElementType>
const ell::model::OutputPort<ElementType>& AppendSink(const ell::model::OutputPort<ElementType>& output);

template <typename ElementType>
const ell::model::OutputPort<ElementType>& AppendOutputWithSink(const ell::model::OutputPort<ElementType>& output);

template <typename ElementType>
const ell::model::OutputPort<ElementType>& Bias(const ell::model::OutputPort<ElementType>& output, ell::math::ConstColumnVectorReference<ElementType> bias);

template <typename ElementType>
const ell::model::OutputPort<ElementType>& Bias(const ell::model::OutputPort<ElementType>& output, const std::vector<ElementType>& bias);

template <typename ElementType>
const ell::model::OutputPort<ElementType>& Scale(const ell::model::OutputPort<ElementType>& output, ell::math::ConstColumnVectorReference<ElementType> scale);

template <typename ElementType>
const ell::model::OutputPort<ElementType>& Scale(const ell::model::OutputPort<ElementType>& output, const std::vector<ElementType>& scale);

template <typename ElementType>
const ell::model::OutputPort<ElementType>& AppendFullyConnectedLayer(const ell::model::OutputPort<ElementType>& output, ell::math::ConstRowMatrixReference<ElementType> weights);

template <typename ElementType>
const ell::model::OutputPort<ElementType>& AppendConvolutionalLayer(const ell::model::OutputPort<ElementType>& output, const ConvolutionalParameters& convParams, ell::math::ConstRowMatrixReference<ElementType> weights);

template <typename ElementType>
const ell::model::OutputPort<ElementType>& Normalize(const ell::model::OutputPort<ElementType>& output, const DataStatistics& stats);

template <typename ElementType>
const ell::model::OutputPort<ElementType>& Unnormalize(const ell::model::OutputPort<ElementType>& output, const DataStatistics& originalOutputStats, const DataStatistics& fineTunedOutputStats);

template <typename ElementType>
const ell::model::OutputPort<ElementType>& AppendFineTunedNodes(const ell::model::OutputPort<ElementType>& output, const FullyConnectedParameters& fcParams, const WeightsAndBias<ElementType>& weightsAndBias);

template <typename ElementType>
const ell::model::OutputPort<ElementType>& AppendFineTunedNodes(const ell::model::OutputPort<ElementType>& output, const ConvolutionalParameters& convParams, const WeightsAndBias<ElementType>& weightsAndBias);

// Model I/O node manipulation
const ell::model::OutputPortBase& RemoveSourceAndSinkNodes(const ell::model::OutputPortBase& output);

template <typename ElementType>
const ell::model::OutputPort<ElementType>& RemoveSourceAndSinkNodes(const ell::model::OutputPort<ElementType>& output);
} // namespace ell
