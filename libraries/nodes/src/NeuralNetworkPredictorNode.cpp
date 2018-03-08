////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     NeuralNetworkPredictorNode.cpp (nodes)
//  Authors:  Chuck Jacobs, Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "NeuralNetworkPredictorNode.h"
#include "ConstantNode.h"
#include "ReorderDataNode.h"

// data
#include "DenseDataVector.h"

// utilities
#include "Exception.h"

// stl
#include <cassert>
#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    namespace
    {
        // Handy helper function
        size_t GetShapeSize(const math::IntegerTriplet& shape)
        {
            return shape[0] * shape[1] * shape[2];
        }
    }

    template <typename ValueType>
    NeuralNetworkPredictorNode<ValueType>::NeuralNetworkPredictorNode()
        : Node({ &_input }, { &_output }), _input(this, {}, defaultInputPortName), _output(this, defaultOutputPortName, 0)
    {
    }

    template <typename ValueType>
    NeuralNetworkPredictorNode<ValueType>::NeuralNetworkPredictorNode(const model::PortElements<ValueType>& input, const PredictorType& predictor)
        : Node({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, GetShapeSize(predictor.GetOutputShape())), _predictor(predictor)
    {
        assert(input.Size() == GetShapeSize(_predictor.GetInputShape()));
    }

    template <typename ValueType>
    void NeuralNetworkPredictorNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver["predictor"] << _predictor;
    }

    template <typename ValueType>
    void NeuralNetworkPredictorNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        PredictorType::RegisterNeuralNetworkPredictorTypes(archiver.GetContext());
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver["predictor"] >> _predictor;
    }

    template <typename ValueType>
    void NeuralNetworkPredictorNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInputElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<NeuralNetworkPredictorNode>(newInputElements, _predictor);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    bool NeuralNetworkPredictorNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        // TODO:
        //
        // Adjust the padding and data ordering depending on what options were set
        //
        // Constraints:
        //   diagonal conv: interleaved order, data padding
        //   unrolled conv: planar order, no padding
        //
        // options:
        //
        // - convolution method
        // - always use regular 'normal' convolution
        // - always use transposed 'normal' convolution
        // - threshold to switch between regular and transposed 'normal' conv (ratio of # memcopies?)
        //
        // - always revert to interleaved order
        // - always revert to planar order
        // - keep existing order as long as possible
        // - choose best order

        // Options
        NetworkCompileOptions options;
        options.useDiagonalConvolution = false; // (true implies rcd order, false implies drc order)
        options.alwaysConvertToInterleaved = true;
        options.transposeReceptiveFieldMatrix = false;

        // State
        NetworkCompileState state;
        state.isInterleavedOrder = true;

        auto newInputElements = transformer.TransformPortElements(_input.GetPortElements());

        const auto& inputLayer = _predictor.GetInputLayer();
        auto inputShape = inputLayer.GetInputShape();
        auto outputPadding = inputLayer.GetLayerParameters().outputPaddingParameters;
        auto padding = outputPadding.paddingSize;

        if (padding != 0)
        {
            // If the input layer wants padding on its output, add a ReorderDataNode to add padding
            model::PortMemoryLayout inputNodeShape({ (int)inputShape.NumRows(), (int)inputShape.NumColumns(), (int)inputShape.NumChannels() });
            model::PortMemoryLayout paddedInputNodeShape({ (int)inputShape.NumRows(), (int)inputShape.NumColumns(), (int)inputShape.NumChannels() }, { (int)padding, (int)padding, 0 });
            auto paddedInputNode = transformer.AddNode<ReorderDataNode<ValueType>>(newInputElements, inputNodeShape, paddedInputNodeShape, predictors::neural::GetPaddingValue<ValueType>(outputPadding.paddingScheme));
            newInputElements = paddedInputNode->output;
        }

        size_t prevOutputSize = GetShapeSize(inputLayer.GetOutputShape()); // With padding
        UNUSED(prevOutputSize);
        auto layerInputs = model::PortElements<ValueType>(newInputElements);
        NeuralNetworkLayerNodeBase<ValueType>* lastNode = nullptr;

        for (const auto& layer : _predictor.GetLayers())
        {
            auto numInputs = GetShapeSize(layer->GetInputShape());
            DEBUG_USED(numInputs);
            assert(prevOutputSize == numInputs);
            auto layerNode = AddLayerNode(transformer, *layer, layerInputs, options, state);

            prevOutputSize = GetShapeSize(layer->GetOutputShape());
            layerInputs = model::PortElements<ValueType>{ *layerNode->GetOutputPort(0) };
            lastNode = layerNode;
        }

        transformer.MapNodeOutput(_output, *lastNode->GetOutputPort(0));
        return true;
    }

    template <typename ValueType>
    void NeuralNetworkPredictorNode<ValueType>::Compute() const
    {
        auto inputDataVector = typename PredictorType::DataVectorType(_input.GetIterator());
        _output.SetOutput(_predictor.Predict(inputDataVector));
    }

    // explicit specialization for float, double
    template class NeuralNetworkPredictorNode<float>;
    template class NeuralNetworkPredictorNode<double>;
}
}
