////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     NeuralNetworkLayerNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace nodes
{
    //
    // NeuralNetworkLayerNodeBase
    //

    template <typename ValueType>
    NeuralNetworkLayerNodeBase<ValueType>::NeuralNetworkLayerNodeBase()
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, inputPortName), _output(this, outputPortName, 0)
    {
    }

    template <typename ValueType>
    NeuralNetworkLayerNodeBase<ValueType>::NeuralNetworkLayerNodeBase(const model::PortElements<ValueType>& input, size_t outputSize)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, outputSize)
    {
    }

    template <typename ValueType>
    void NeuralNetworkLayerNodeBase<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
    }

    template <typename ValueType>
    void NeuralNetworkLayerNodeBase<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
    }

    //
    // NeuralNetworkLayerNode
    //
    template <typename DerivedType, typename LayerType, typename ValueType>
    NeuralNetworkLayerNode<DerivedType, LayerType, ValueType>::NeuralNetworkLayerNode()
        : NeuralNetworkLayerNodeBase<ValueType>()
    {
    }

    template <typename DerivedType, typename LayerType, typename ValueType>
    NeuralNetworkLayerNode<DerivedType, LayerType, ValueType>::NeuralNetworkLayerNode(const model::PortElements<ValueType>& input, const LayerType& layer)
        : NeuralNetworkLayerNodeBase<ValueType>(input, GetShapeSize(layer.GetOutputShape())), _inputTensor(layer.GetInputShape()), _layer(layer)
    {
        _layer.GetLayerParameters().input = _inputTensor;
    }

    template <typename DerivedType, typename LayerType, typename ValueType>
    void NeuralNetworkLayerNode<DerivedType, LayerType, ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<DerivedType>(newPortElements, _layer);
        transformer.MapNodeOutput(_output, newNode->output);
    }

    template <typename DerivedType, typename LayerType, typename ValueType>
    void NeuralNetworkLayerNode<DerivedType, LayerType, ValueType>::Compute() const
    {
        // typename predictors::neural::Layer<ValueType>::LayerVector inputVector(ConvertVectorTo<double>(_input.GetValue()));
        auto inputVector = _input.GetValue();
        auto inputTensor = typename LayerType::ConstTensorReferenceType{ _inputTensor.GetShape(), inputVector.data() };
        _inputTensor.CopyFrom(inputTensor);
        _layer.Compute();
        auto&& outputTensor = _layer.GetOutput();
        _output.SetOutput(outputTensor.ToArray());
    }

    template <typename DerivedType, typename LayerType, typename ValueType>
    size_t NeuralNetworkLayerNode<DerivedType, LayerType, ValueType>::GetShapeSize(const math::Triplet& shape)
    {
        return shape[0] * shape[1] * shape[2];
    }

    template <typename LayerType>
    typename LayerType::LayerParameters GetLayerNodeParameters(const typename LayerType::TensorType& inputTensor, const typename LayerType::LayerParameters& layerParameters)
    {
        return { inputTensor, layerParameters.inputPaddingParameters, layerParameters.outputShape, layerParameters.outputPaddingParameters };
    }
}
}
