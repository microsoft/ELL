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
        CompilableNode::WriteToArchive(archiver);
        archiver[inputPortName] << _input;
    }

    template <typename ValueType>
    void NeuralNetworkLayerNodeBase<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        CompilableNode::ReadFromArchive(archiver);
        archiver[inputPortName] >> _input;
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
        : NeuralNetworkLayerNodeBase<ValueType>(input, layer.GetOutput().Size()), _inputTensor(layer.GetInputShape()), _layer(layer)
    {
        _layer.GetLayerParameters().input = _inputTensor;

        const auto& layerParameters = this->GetLayer().GetLayerParameters();

        // Calculate input dimension parameters
        size_t inputPaddingSize = layerParameters.inputPaddingParameters.paddingSize;
        auto inputShapeArray = this->GetLayer().GetInputShape();
        Shape inputStride{ inputShapeArray.begin(), inputShapeArray.end() };
        Shape inputOffset{ inputPaddingSize, inputPaddingSize, 0 };
        Shape inputSize(inputStride.size());
        for (int dimensionIndex = 0; dimensionIndex < inputOffset.size(); ++dimensionIndex)
        {
            if(inputStride[dimensionIndex] < (2 * inputOffset[dimensionIndex]))
            {
                throw utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "Input size not large enough to accomodate padding");
            }
            inputSize[dimensionIndex] = inputStride[dimensionIndex] - (2 * inputOffset[dimensionIndex]);
        }

        _inputLayout = { inputSize, inputStride, inputOffset };

        // Calculate output dimension parameters
        size_t outputPaddingSize = layerParameters.outputPaddingParameters.paddingSize;
        auto outputShapeArray = this->_layer.GetOutputShape();
        Shape outputStride{ outputShapeArray.begin(), outputShapeArray.end() };
        Shape outputOffset = { outputPaddingSize, outputPaddingSize, 0 };
        Shape outputSize(outputStride.size());
        for (int dimensionIndex = 0; dimensionIndex < outputOffset.size(); ++dimensionIndex)
        {
            if(outputStride[dimensionIndex] < (2 * outputOffset[dimensionIndex]))
            {
                throw utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "Output size not large enough to accomodate padding");
            }
            outputSize[dimensionIndex] = outputStride[dimensionIndex] - (2 * outputOffset[dimensionIndex]);
        }

        _outputLayout = { outputSize, outputStride, outputOffset };
    }

    template <typename DerivedType, typename LayerType, typename ValueType>
    void NeuralNetworkLayerNode<DerivedType, LayerType, ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        NeuralNetworkLayerNodeBase<ValueType>::WriteToArchive(archiver);
        math::TensorArchiver::Write(_inputTensor, "inputTensor", archiver);
        archiver["layer"] << _layer;
        archiver["inputLayout"] << _inputLayout;
        archiver["outputLayout"] << _outputLayout;
    }

    template <typename DerivedType, typename LayerType, typename ValueType>
    void NeuralNetworkLayerNode<DerivedType, LayerType, ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        NeuralNetworkLayerNodeBase<ValueType>::ReadFromArchive(archiver);
        math::TensorArchiver::Read(_inputTensor, "inputTensor", archiver);
        archiver["layer"] >> _layer;
        _inputTensor = typename LayerType::TensorType(_layer.GetInputShape());
        _layer.GetLayerParameters().input = _inputTensor;
        archiver["inputLayout"] >> _inputLayout;
        archiver["outputLayout"] >> _outputLayout;
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
        auto inputVector = _input.GetValue();
        auto inputTensor = typename LayerType::ConstTensorReferenceType{ _inputTensor.GetShape(), inputVector.data() };
        _inputTensor.CopyFrom(inputTensor);
        _layer.Compute();
        const auto& outputTensor = _layer.GetOutput();
        _output.SetOutput(outputTensor.ToArray());
    }

    template <typename LayerType>
    typename LayerType::LayerParameters GetLayerNodeParameters(const typename LayerType::TensorType& inputTensor, const typename LayerType::LayerParameters& layerParameters)
    {
        return { inputTensor, layerParameters.inputPaddingParameters, layerParameters.outputShape, layerParameters.outputPaddingParameters };
    }
}
}
