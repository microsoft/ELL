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
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, defaultInputPortName), _output(this, defaultOutputPortName, 0)
    {
        _parameters.includePaddingInInputData = true;
    }

    template <typename ValueType>
    NeuralNetworkLayerNodeBase<ValueType>::NeuralNetworkLayerNodeBase(const model::PortElements<ValueType>& input, const NeuralNetworkLayerNodeParameters& parameters, size_t outputSize)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, outputSize), _parameters(parameters)
    {
    }

    template <typename ValueType>
    void NeuralNetworkLayerNodeBase<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        CompilableNode::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
    }

    template <typename ValueType>
    void NeuralNetworkLayerNodeBase<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        CompilableNode::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
    }

    //
    // NeuralNetworkLayerNode
    //
    template <typename DerivedType, typename LayerType, typename ValueType>
    NeuralNetworkLayerNode<DerivedType, LayerType, ValueType>::NeuralNetworkLayerNode()
        : NeuralNetworkLayerNodeBase<ValueType>(), _inputShape(0,0,0)
    {
    }

    template <typename DerivedType, typename LayerType, typename ValueType>
    NeuralNetworkLayerNode<DerivedType, LayerType, ValueType>::NeuralNetworkLayerNode(const model::PortElements<ValueType>& input, const LayerType& layer)
        : NeuralNetworkLayerNodeBase<ValueType>(input, {}, layer.GetOutput().Size()), _inputTensor(layer.GetInputShape()), _layer(layer), _inputShape(layer.GetInputShape())
    {
        _layer.GetLayerParameters().input = _inputTensor;

        const auto& layerParameters = this->GetLayerParameters();

        // Calculate input dimension parameters
        size_t inputPaddingSize = layerParameters.inputPaddingParameters.paddingSize;
        auto inputShape = this->GetLayer().GetInputShape();
        _inputLayout = CalculateMemoryLayout(inputPaddingSize, inputShape);

        // Calculate output dimension parameters
        size_t outputPaddingSize = layerParameters.outputPaddingParameters.paddingSize;
        auto outputShape = this->_layer.GetOutputShape();
        _outputLayout = CalculateMemoryLayout(outputPaddingSize, outputShape);
    }

    template <typename DerivedType, typename LayerType, typename ValueType>
    model::PortMemoryLayout NeuralNetworkLayerNode<DerivedType, LayerType, ValueType>::CalculateMemoryLayout(size_t padding, typename predictors::neural::Layer<ValueType>::Shape dataBufferSize)
    {
        // Calculate dimension parameters
        math::IntegerTriplet dataSizeArray = dataBufferSize;
        model::Shape stride{ dataSizeArray.begin(), dataSizeArray.end() };
        model::Shape offset{ static_cast<int>(padding), static_cast<int>(padding), 0 };
        model::Shape size(stride.size());
        for (size_t dimensionIndex = 0; dimensionIndex < offset.size(); ++dimensionIndex)
        {
            if(stride[dimensionIndex] < (2 * offset[dimensionIndex]))
            {
                throw utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "Data size not large enough to accommodate padding");
            }
            size[dimensionIndex] = stride[dimensionIndex] - (2 * offset[dimensionIndex]);
        }

        return { size, stride, offset };
    }

    template <typename DerivedType, typename LayerType, typename ValueType>
    utilities::ArchiveVersion NeuralNetworkLayerNode<DerivedType, LayerType, ValueType>::GetArchiveVersion() const
    {
        constexpr utilities::ArchiveVersion archiveVersion = { utilities::ArchiveVersionNumbers::v5_refined_nodes };

        return archiveVersion;
    }

    template <typename DerivedType, typename LayerType, typename ValueType>
    bool NeuralNetworkLayerNode<DerivedType, LayerType, ValueType>::CanReadArchiveVersion(const utilities::ArchiveVersion& version) const
    {
        constexpr utilities::ArchiveVersion archiveVersion = { utilities::ArchiveVersionNumbers::v5_refined_nodes };

        return version >= archiveVersion;
    }

    template <typename DerivedType, typename LayerType, typename ValueType>
    void NeuralNetworkLayerNode<DerivedType, LayerType, ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        NeuralNetworkLayerNodeBase<ValueType>::WriteToArchive(archiver);
        archiver["inputLayout"] << _inputLayout;
        archiver["outputLayout"] << _outputLayout;

        std::vector<size_t> inputShape = _inputShape;
        archiver["inputShape"] << inputShape;

        archiver["layer"] << _layer;
    }

    template <typename DerivedType, typename LayerType, typename ValueType>
    void NeuralNetworkLayerNode<DerivedType, LayerType, ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        NeuralNetworkLayerNodeBase<ValueType>::ReadFromArchive(archiver);
        archiver["inputLayout"] >> _inputLayout;
        archiver["outputLayout"] >> _outputLayout;

        std::vector<size_t> inputShape;
        archiver["inputShape"] >> inputShape;
        _inputShape = math::TensorShape{ inputShape };

        _inputTensor = typename LayerType::TensorType(_inputShape);
        _layer.GetLayerParameters().input = _inputTensor;
        archiver["layer"] >> _layer;
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
        auto inputTensor = typename LayerType::ConstTensorReferenceType{ inputVector.data(), _inputTensor.GetShape() };
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
