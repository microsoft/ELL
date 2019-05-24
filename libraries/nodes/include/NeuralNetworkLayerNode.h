////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     NeuralNetworkLayerNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/CompilableNode.h>
#include <model/include/IRMapCompiler.h>
#include <model/include/Model.h>
#include <model/include/ModelTransformer.h>
#include <model/include/Node.h>
#include <model/include/PortMemoryLayout.h>

#include <predictors/neural/include/Layer.h>

#include <string>
#include <type_traits>

namespace ell
{
namespace nodes
{
    /// <summary> Parameters to influence how neural network layers behave when embedded as nodes in a graph </summary>
    struct NeuralNetworkLayerNodeParameters
    {
        bool includePaddingInInputData;
    };

    /// <summary> Base class for neural network layer nodes. </summary
    template <typename ValueType>
    class NeuralNetworkLayerNodeBase : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Gets information about the input memory layout </summary>
        virtual model::PortMemoryLayout& GetInputMemoryLayout() = 0;

        /// <summary> Gets information about the input memory layout </summary>
        virtual const model::PortMemoryLayout& GetInputMemoryLayout() const = 0;

        /// <summary> Gets information about the output memory layout </summary>
        virtual model::PortMemoryLayout GetOutputMemoryLayout() const = 0;

        /// <summary> Gets the LayerParameters from the layer wrapped by this node </summary>
        virtual typename predictors::neural::Layer<ValueType>::LayerParameters GetLayerParameters() const = 0;

        /// <summary> Gets the neural network base class Layer from the actual layer wrapped by this node </summary>
        virtual typename predictors::neural::Layer<ValueType>& GetBaseLayer() const = 0;

        /// <summary> Get the input padding requested by the layer </summary>
        predictors::neural::PaddingParameters GetRequestedInputPadding() const { return GetLayerParameters().inputPaddingParameters; }

        /// <summary> Get the output padding requested by the layer </summary>
        predictors::neural::PaddingParameters GetRequestedOutputPadding() const { return GetLayerParameters().outputPaddingParameters; }

        /// <summary> Get the size of the output port </summary>
        size_t GetOutputSize() const { return _output.Size(); }

    protected:
        NeuralNetworkLayerNodeBase();
        NeuralNetworkLayerNodeBase(const model::OutputPort<ValueType>& input, const NeuralNetworkLayerNodeParameters& parameters, size_t outputSize);

        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

        // Input
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

        NeuralNetworkLayerNodeParameters _parameters;
    };

    /// <summary> Base class for neural network layer nodes. </summary
    template <typename DerivedType, typename NodeLayerType, typename ValueType>
    class NeuralNetworkLayerNode : public NeuralNetworkLayerNodeBase<ValueType>
    {
    public:
        using LayerType = NodeLayerType;

        /// @name Input and Output Ports
        /// @{
        using NeuralNetworkLayerNodeBase<ValueType>::input;
        using NeuralNetworkLayerNodeBase<ValueType>::output;
        /// @}

        /// <summary> Default constructor. </summary>
        NeuralNetworkLayerNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The input to the layer (typically the output of the previous layer). </param>
        /// <param name="layer"> The neural network layer to wrap. </param>
        NeuralNetworkLayerNode(const model::OutputPort<ValueType>& input, const LayerType& layer);

        /// <summary> Gets the layer being wrapped </summary>
        const LayerType& GetLayer() const { return _layer; }

        /// <summary> Gets information about the input memory layout </summary>
        model::PortMemoryLayout& GetInputMemoryLayout() override { return _inputLayout; }

        /// <summary> Gets information about the input memory layout </summary>
        const model::PortMemoryLayout& GetInputMemoryLayout() const override { return _inputLayout; }

        /// <summary> Gets information about the output memory layout </summary>
        model::PortMemoryLayout GetOutputMemoryLayout() const override { return output.GetMemoryLayout(); }

        /// <summary> Returns true if the node can accept input with this memory layout order, else false </summary>
        ///
        /// <param name="order"> The memory layout order for all the input ports </summary>
        /// <returns> If the node can accept the input memory layout order, true, else false </returns>
        bool CanAcceptInputLayout(const utilities::DimensionOrder& order) const override
        {
            return GetInputMemoryLayout().GetLogicalDimensionOrder() == order;
        }

        /// <summary> Gets the LayerParameters from the layer wrapped by this node </summary>
        typename predictors::neural::Layer<ValueType>::LayerParameters GetLayerParameters() const override { return _layer.GetLayerParameters(); }

        /// <summary> Gets the neural network base class Layer from the actual layer wrapped by this node </summary>
        typename predictors::neural::Layer<ValueType>& GetBaseLayer() const override { return _layer; }

    protected:
        size_t NumInputDimensions() const { return _inputLayout.NumDimensions(); }
        model::PortMemoryLayout CalculateMemoryLayout(size_t padding, typename predictors::neural::Layer<ValueType>::Shape dataBufferSize);
        void Compute() const override;
        utilities::ArchiveVersion GetArchiveVersion() const override;
        bool CanReadArchiveVersion(const utilities::ArchiveVersion& version) const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

        using NeuralNetworkLayerNodeBase<ValueType>::_input;
        using NeuralNetworkLayerNodeBase<ValueType>::_output;

        mutable typename LayerType::TensorType _inputTensor;
        mutable LayerType _layer; // mutable to get around Compute being non-const
        bool HasState() const override { return true; } // stored state: inputLayout, outputLayout

    private:
        model::PortMemoryLayout _inputLayout;
        math::TensorShape _inputShape;
    };

    // helper:
    template <typename LayerType>
    typename LayerType::LayerParameters GetLayerNodeParameters(const typename LayerType::TensorType& inputTensor, const typename LayerType::LayerParameters& layerParameters);
} // namespace nodes
} // namespace ell

#pragma region implementation

namespace ell
{
namespace nodes
{
    //
    // NeuralNetworkLayerNodeBase
    //

    template <typename ValueType>
    NeuralNetworkLayerNodeBase<ValueType>::NeuralNetworkLayerNodeBase() :
        CompilableNode({ &_input }, { &_output }),
        _input(this, {}, defaultInputPortName),
        _output(this, defaultOutputPortName, 0)
    {
        _parameters.includePaddingInInputData = true;
    }

    template <typename ValueType>
    NeuralNetworkLayerNodeBase<ValueType>::NeuralNetworkLayerNodeBase(const model::OutputPort<ValueType>& input, const NeuralNetworkLayerNodeParameters& parameters, size_t outputSize) :
        CompilableNode({ &_input }, { &_output }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, outputSize),
        _parameters(parameters)
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
    NeuralNetworkLayerNode<DerivedType, LayerType, ValueType>::NeuralNetworkLayerNode() :
        NeuralNetworkLayerNodeBase<ValueType>(),
        _inputShape(0, 0, 0)
    {
    }

    template <typename DerivedType, typename LayerType, typename ValueType>
    NeuralNetworkLayerNode<DerivedType, LayerType, ValueType>::NeuralNetworkLayerNode(const model::OutputPort<ValueType>& input, const LayerType& layer) :
        NeuralNetworkLayerNodeBase<ValueType>(input, {}, layer.GetOutput().Size()),
        _inputTensor(layer.GetInputShape()),
        _layer(layer),
        _inputShape(layer.GetInputShape())
    {
        _layer.GetLayerParameters().input = _inputTensor;

        const auto& layerParameters = _layer.GetLayerParameters();

        // Calculate input dimension parameters
        size_t inputPaddingSize = layerParameters.inputPaddingParameters.paddingSize;
        auto inputShape = this->GetLayer().GetInputShape();
        _inputLayout = CalculateMemoryLayout(inputPaddingSize, inputShape);

        // Calculate output dimension parameters
        size_t outputPaddingSize = layerParameters.outputPaddingParameters.paddingSize;
        auto outputShape = this->_layer.GetOutputShape();
        _output.SetMemoryLayout(CalculateMemoryLayout(outputPaddingSize, outputShape));
    }

    template <typename DerivedType, typename LayerType, typename ValueType>
    model::PortMemoryLayout NeuralNetworkLayerNode<DerivedType, LayerType, ValueType>::CalculateMemoryLayout(size_t padding, typename predictors::neural::Layer<ValueType>::Shape dataBufferSize)
    {
        // Calculate dimension parameters
        math::IntegerTriplet dataSizeArray = dataBufferSize;
        model::MemoryShape stride{ { static_cast<int>(dataSizeArray[0]), static_cast<int>(dataSizeArray[1]), static_cast<int>(dataSizeArray[2]) } };
        model::MemoryShape offset{ static_cast<int>(padding), static_cast<int>(padding), 0 };
        model::MemoryShape size{};
        size.Resize(stride.NumDimensions());
        for (int dimensionIndex = 0; dimensionIndex < offset.NumDimensions(); ++dimensionIndex)
        {
            if (stride[dimensionIndex] < (2 * offset[dimensionIndex]))
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
        archiver["outputLayout"] << GetOutputMemoryLayout();

        std::vector<size_t> inputShape = _inputShape;
        archiver["inputShape"] << inputShape;

        archiver["layer"] << _layer;
    }

    template <typename DerivedType, typename LayerType, typename ValueType>
    void NeuralNetworkLayerNode<DerivedType, LayerType, ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        NeuralNetworkLayerNodeBase<ValueType>::ReadFromArchive(archiver);
        archiver["inputLayout"] >> _inputLayout;
        model::PortMemoryLayout outputLayout;
        archiver["outputLayout"] >> outputLayout;
        _output.SetMemoryLayout(outputLayout);

        std::vector<size_t> inputShape;
        archiver["inputShape"] >> inputShape;
        _inputShape = math::TensorShape{ inputShape };

        _inputTensor = typename LayerType::TensorType(_inputShape);
        _layer.GetLayerParameters().input = _inputTensor;
        archiver["layer"] >> _layer;
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
} // namespace nodes
} // namespace ell

#pragma endregion implementation
