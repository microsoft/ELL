////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ConvolutionalLayerNode.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ConvolutionalLayerNode.h"
#include "UnrolledConvolutionNode.h"
#include "DiagonalConvolutionNode.h"
#include "SimpleConvolutionNode.h"

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
        const auto inputPadding = this->GetRequestedInputPadding();
        const auto outputPadding = this->GetRequestedOutputPadding();

        auto newInput = transformer.TransformPortElements(this->input.GetPortElements());

        // Terminology:
        // fw: filter width
        // d: # input channels
        // f: # filters (== output channels)        
        
        switch (convParams.method)
        {
            case predictors::neural::ConvolutionMethod::simple:
            {
                // (row, column), channel order:
                const auto& weights = this->GetLayer().GetWeights();
                auto convNode = transformer.AddNode<SimpleConvolutionNode<ValueType>>(newInput, inputLayout, outputLayout, weights, convParams, inputPadding, outputPadding);
                transformer.MapNodeOutput(this->output, convNode->output);
            }
            break;
            case predictors::neural::ConvolutionMethod::unrolled:
            {
                // (row, column), channel order:
                const auto& weights = this->GetLayer().GetWeights();
                auto convNode = transformer.AddNode<UnrolledConvolutionNode<ValueType>>(newInput, inputLayout, outputLayout, weights, convParams, inputPadding, outputPadding);
                transformer.MapNodeOutput(this->output, convNode->output);
            }            
            break;
            case predictors::neural::ConvolutionMethod::diagonal:
            {                
                assert((convParams.stride == 1) && (convParams.receptiveField % 2 == 1));

                // (row, column), channel order:
                const auto& weights = this->GetLayer().GetWeights();
                auto convNode = transformer.AddNode<DiagonalConvolutionNode<ValueType>>(newInput, inputLayout, outputLayout, weights, convParams, inputPadding, outputPadding);
                transformer.MapNodeOutput(this->output, convNode->output);
            }                
            break;
            default:
                throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }
        return true;
    }

    // Explicit specializations
    template class ConvolutionalLayerNode<float>;
    template class ConvolutionalLayerNode<double>;
} // nodes
} // ell
