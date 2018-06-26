////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ConvolutionalLayerNode.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ConvolutionalLayerNode.h"
#include "DiagonalConvolutionNode.h"
#include "SimpleConvolutionNode.h"
#include "UnrolledConvolutionNode.h"
#include "WinogradConvolutionNode.h"

using namespace ell::math;
using namespace ell::math::Blas;

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    ConvolutionalLayerNode<ValueType>::ConvolutionalLayerNode(const model::PortElements<ValueType>& input, const predictors::neural::ConvolutionalLayer<ValueType>& layer)
        : NeuralNetworkLayerNode<ConvolutionalLayerNode<ValueType>, predictors::neural::ConvolutionalLayer<ValueType>, ValueType>(input, layer)
    {
    }

    template <typename ValueType>
    bool ConvolutionalLayerNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        const auto inputLayout = this->GetInputMemoryLayout();
        const auto outputLayout = this->GetOutputMemoryLayout();
        const auto convParams = this->GetLayer().GetConvolutionalParameters();

        auto newInput = transformer.TransformPortElements(this->input.GetPortElements());

        // Terminology:
        // fw: filter width
        // d: # input channels
        // f: # filters (== output channels)
        const auto& weights = this->GetLayer().GetWeights();
        switch (convParams.method)
        {
        case predictors::neural::ConvolutionMethod::simple:
        {
            auto convNode = transformer.AddNode<SimpleConvolutionNode<ValueType>>(newInput, inputLayout, outputLayout, weights, convParams.stride);
            transformer.MapNodeOutput(this->output, convNode->output);
        }
        break;
        case predictors::neural::ConvolutionMethod::unrolled:
        {
            auto convNode = transformer.AddNode<UnrolledConvolutionNode<ValueType>>(newInput, inputLayout, outputLayout, weights, convParams.stride);
            transformer.MapNodeOutput(this->output, convNode->output);
        }
        break;
        case predictors::neural::ConvolutionMethod::diagonal:
        {
            auto convNode = transformer.AddNode<DiagonalConvolutionNode<ValueType>>(newInput, inputLayout, outputLayout, weights, convParams.stride);
            transformer.MapNodeOutput(this->output, convNode->output);
        }
        break;
        case predictors::neural::ConvolutionMethod::winograd:
        {
            using FilterOrder = typename WinogradConvolutionNode<ValueType>::FilterOrder;
            const int winogradTileSize = 2;
            auto numFilterChannels = static_cast<int>(weights.NumChannels());
            const int filtersFirstThreshold = 2;
            const auto order = (numFilterChannels < filtersFirstThreshold) ? FilterOrder::filtersFirst : FilterOrder::tilesFirst;
            auto convNode = transformer.AddNode<WinogradConvolutionNode<ValueType>>(newInput, inputLayout, outputLayout, weights, convParams.stride, winogradTileSize, order);
            transformer.MapNodeOutput(this->output, convNode->output);
        }
        break;
        default:
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }
        return true;
    }

    template <typename ValueType>
    void ConvolutionalLayerNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(this->_input.GetPortElements());
        auto newNode = transformer.AddNode<ConvolutionalLayerNode<ValueType>>(newPortElements, this->_layer);
        transformer.MapNodeOutput(this->_output, newNode->output);
    }

    // Explicit specializations
    template class ConvolutionalLayerNode<float>;
    template class ConvolutionalLayerNode<double>;
} // nodes
} // ell
