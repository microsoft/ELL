////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ConvolutionalLayerNode.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ConvolutionalLayerNode.h"
#include "DiagonalConvolutionNode.h"
#include "ReorderDataNode.h"
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
        using predictors::neural::ConvolutionMethod;

        auto originalInputLayout = this->GetInputMemoryLayout();
        const auto originalOutputLayout = this->GetOutputMemoryLayout();
        const auto convParams = this->GetLayer().GetConvolutionalParameters();

        auto newInput = transformer.TransformPortElements(this->input.GetPortElements());

        // Terminology:
        // fw: filter width
        // d: # input channels
        // f: # filters (== output channels)
        const auto& weights = this->GetLayer().GetWeights();

        auto isDepthwiseSeparable = (weights.NumChannels() == 1);
        auto shouldReorderToChannelMajor = isDepthwiseSeparable && (convParams.method == ConvolutionMethod::simple || convParams.method == ConvolutionMethod::winograd);

        auto convInputLayout = originalInputLayout.ReorderedCopy({ shouldReorderToChannelMajor ? utilities::ChannelMajorTensorOrder : utilities::RowMajorTensorOrder });
        auto convOutputLayout = originalOutputLayout.ReorderedCopy({ shouldReorderToChannelMajor ? utilities::ChannelMajorTensorOrder : utilities::RowMajorTensorOrder });

        auto preConvReorderNode = transformer.AddNode<ReorderDataNode<ValueType>>(newInput, originalInputLayout, convInputLayout);
        newInput = preConvReorderNode->output;

        model::PortElements<ValueType> convOutput;

        switch (convParams.method)
        {
        case ConvolutionMethod::simple:
        {
            auto convNode = transformer.AddNode<SimpleConvolutionNode<ValueType>>(newInput, convInputLayout, convOutputLayout, weights, convParams.stride);
            convOutput = convNode->output;
        }
        break;
        case ConvolutionMethod::unrolled:
        {
            auto convNode = transformer.AddNode<UnrolledConvolutionNode<ValueType>>(newInput, convInputLayout, convOutputLayout, weights, convParams.stride);
            convOutput = convNode->output;
        }
        break;
        case ConvolutionMethod::diagonal:
        {
            auto convNode = transformer.AddNode<DiagonalConvolutionNode<ValueType>>(newInput, convInputLayout, convOutputLayout, weights, convParams.stride);
            convOutput = convNode->output;
        }
        break;
        case ConvolutionMethod::winograd:
        {
            using FilterOrder = typename WinogradConvolutionNode<ValueType>::FilterOrder;
            const int winogradTileSize = 2;
            auto numFilterChannels = static_cast<int>(weights.NumChannels());
            const int filtersFirstThreshold = 2;
            const auto order = (numFilterChannels < filtersFirstThreshold) ? FilterOrder::filtersFirst : FilterOrder::tilesFirst;
            if (order == FilterOrder::filtersFirst)
            {
                // add a ReorderDataNode to convert to channel-major, which is more efficient in this case
                auto orderArr = utilities::ChannelMajorTensorOrder;
                auto reorderNode = transformer.AddNode<ReorderDataNode<ValueType>>(newInput, convInputLayout, convInputLayout, std::vector<int>{ orderArr.begin(), orderArr.end() });
                auto outputLayout = reorderNode->GetOutputMemoryLayout();
                newInput = reorderNode->output;
                convInputLayout = reorderNode->GetOutputMemoryLayout();
            }

            auto convNode = transformer.AddNode<WinogradConvolutionNode<ValueType>>(newInput, convInputLayout, convOutputLayout, weights, convParams.stride, winogradTileSize, order);
            convOutput = convNode->output;
        }
        break;
        default:
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }

        auto postConvReorderNode = transformer.AddNode<ReorderDataNode<ValueType>>(convOutput, convOutputLayout, originalOutputLayout);
        transformer.MapNodeOutput(this->output, postConvReorderNode->output);

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
