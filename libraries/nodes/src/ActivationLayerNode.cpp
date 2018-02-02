////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ActivationLayerNode.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ActivationLayerNode.h"
#include "BinaryFunctionNode.h"
#include "BroadcastFunctionNode.h"
#include "CompiledActivationFunctions.h"
#include "ConstantNode.h"

// predictors
#include "LeakyReLUActivation.h"
#include "ReLUActivation.h"
#include "SigmoidActivation.h"

namespace ell
{
namespace nodes
{
    //
    // ActivationLayerNode
    //

    template <typename ValueType, template <typename> class ActivationFunctionType>
    ActivationLayerNode<ValueType, ActivationFunctionType>::ActivationLayerNode(const model::PortElements<ValueType>& input, const predictors::neural::ActivationLayer<ValueType, ActivationFunctionType>& layer)
        : BaseType(input, layer)
    {
        auto&& inputLayout = this->GetInputMemoryLayout();
        auto&& outputLayout = this->GetOutputMemoryLayout();
        if (!model::ShapesEqual(inputLayout.GetActiveSize(), outputLayout.GetActiveSize()))
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input and output active area sizes don't match");
        }
    }

    template <typename ValueType, template <typename> class ActivationFunctionType>
    bool ActivationLayerNode<ValueType, ActivationFunctionType>::Refine(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(this->input.GetPortElements());

        auto predictorActivationFunction = this->GetLayer().GetActivationFunction();
        auto nodeActivationFunction = GetNodeActivationFunction(predictorActivationFunction);
        auto computeNode = transformer.AddNode<BroadcastUnaryFunctionNode<ValueType, decltype(nodeActivationFunction)>>(newInput,
                                                                                                                        this->GetInputMemoryLayout(),
                                                                                                                        this->GetOutputMemoryLayout(),
                                                                                                                        nodeActivationFunction);
        transformer.MapNodeOutput(this->output, computeNode->output);
        return true;
    }

    //
    // ParametricReLUActivationLayerNode
    //

    template <typename ValueType>
    ParametricReLUActivationLayerNode<ValueType>::ParametricReLUActivationLayerNode(const model::PortElements<ValueType>& input, const LayerType& layer)
        : BaseType(input, layer)
    {
        auto&& inputLayout = this->GetInputMemoryLayout();
        auto&& outputLayout = this->GetOutputMemoryLayout();
        if (!model::ShapesEqual(inputLayout.GetActiveSize(), outputLayout.GetActiveSize()))
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input and output active area sizes don't match");
        }
    }

    template <typename ValueType>
    bool ParametricReLUActivationLayerNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        using ActivationFunction = ParametricReLUActivationFunction<ValueType>;
        auto newInput = transformer.TransformPortElements(this->input.GetPortElements());

        auto predictorActivationFunction = this->GetLayer().GetActivationFunction();
        auto alphaValues = predictorActivationFunction.GetAlpha().ToArray();
        auto alphaValuesNode = transformer.AddNode<ConstantNode<ValueType>>(alphaValues);

        // PReLU is a coordinate-wise operation
        auto computeNode = transformer.AddNode<BinaryFunctionNode<ValueType, ActivationFunction>>(
            newInput,
            alphaValuesNode->output,
            this->GetInputMemoryLayout(),
            this->GetOutputMemoryLayout(),
            ActivationFunction{});

        transformer.MapNodeOutput(this->output, computeNode->output);
        return true;
    }

    // Explicit specialization
    template class ActivationLayerNode<float, ell::predictors::neural::HardSigmoidActivation>;
    template class ActivationLayerNode<double, ell::predictors::neural::HardSigmoidActivation>;
    template class ActivationLayerNode<float, ell::predictors::neural::LeakyReLUActivation>;
    template class ActivationLayerNode<double, ell::predictors::neural::LeakyReLUActivation>;
    template class ActivationLayerNode<float, ell::predictors::neural::ReLUActivation>;
    template class ActivationLayerNode<double, ell::predictors::neural::ReLUActivation>;
    template class ActivationLayerNode<float, ell::predictors::neural::SigmoidActivation>;
    template class ActivationLayerNode<double, ell::predictors::neural::SigmoidActivation>;
    template class ActivationLayerNode<float, ell::predictors::neural::TanhActivation>;
    template class ActivationLayerNode<double, ell::predictors::neural::TanhActivation>;
    template class ParametricReLUActivationLayerNode<float>;
    template class ParametricReLUActivationLayerNode<double>;

} // nodes
} // ell
