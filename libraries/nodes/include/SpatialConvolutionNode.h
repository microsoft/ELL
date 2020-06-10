////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SpatialConvolutionNode.h (nodes)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/CompilableCodeNode.h>
#include <model/include/IRMapCompiler.h>
#include <model/include/InputPort.h>
#include <model/include/OutputPort.h>

#include <value/include/FunctionDeclaration.h>
#include <value/include/Scalar.h>
#include <value/include/ScalarOperations.h>
#include <value/include/Tensor.h>

#include <predictors/neural/include/ConvolutionalLayer.h>

#include <value/include/loopnests/CodeGenerator.h>
#include <value/include/loopnests/Kernel.h>
#include <value/include/loopnests/LoopNest.h>

#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary> A node that performs the spatial convolution in a depthwise separable
    /// convolutional model. By definition, this node requires:
    /// - Number of input channels per weights filter to be 1
    /// - Number of filters must equal number of input channels
    /// </summary>
    template <typename ValueType>
    class SpatialConvolutionNode : public model::CompilableCodeNode
    {

    public:
        using LayerType = predictors::neural::ConvolutionalLayer<ValueType>;

        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        SpatialConvolutionNode();

        /// <summary> Constructor from a layer. </summary>
        ///
        /// <param name="input"> </param>
        /// <param name="layer"> The convolutional layer to wrap. </param>
        SpatialConvolutionNode(const model::OutputPort<ValueType>& input, const LayerType& layer, const model::PortMemoryLayout& outputMemoryLayout);

        /// <summary> Returns true if the node can accept input with this memory layout order, else false </summary>
        ///
        /// <param name="order"> The memory layout order for all the input ports </summary>
        /// <returns> If the node can accept the input memory layout order, true, else false </returns>
        bool CanAcceptInputLayout(const utilities::DimensionOrder& order) const override
        {
            return true;
        }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("SpatialConvolutionNode"); }

    protected:
        void Define(ell::value::FunctionDeclaration& fn) override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return true; } // stored state: convolutional parameters
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        // Called with output i, j, k
        void spatial_convolutional_kernel(value::Tensor output, value::Tensor input, value::Tensor weights, value::Scalar i, value::Scalar j, value::Scalar k);

        // Inputs
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

        // Convolutional layer
        LayerType _layer;
    };

} // namespace nodes
} // namespace ell

#pragma region implementation

namespace ell
{
namespace nodes
{
    //
    // SpatialConvolutionNode
    //
    template <typename ValueType>
    SpatialConvolutionNode<ValueType>::SpatialConvolutionNode() :
        CompilableCodeNode("SpatialConvolutionNode", { &_input }, { &_output }),
        _input(this, {}, defaultInputPortName),
        _output(this, defaultOutputPortName, 0)
    {}

    //
    // SpatialConvolutionNode
    //
    template <typename ValueType>
    SpatialConvolutionNode<ValueType>::SpatialConvolutionNode(const model::OutputPort<ValueType>& input,
                                                              const LayerType& layer,
                                                              const model::PortMemoryLayout& outputMemoryLayout) :
        CompilableCodeNode("SpatialConvolutionNode", { &_input }, { &_output }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, outputMemoryLayout),
        _layer(layer)
    {
        const auto& weights = _layer.GetWeights();
        if (weights.NumChannels() != 1)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                                            "Error: weights for Spatial Convolution must have single channel");
        }
        if (_input.GetMemoryLayout().GetLogicalDimensionExtent(2) != _output.GetMemoryLayout().GetLogicalDimensionExtent(2))
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
                                            "Error: input and output number of channels must match for Spatial Convolution");
        }
    }

    //
    // A spatial convolution kernel
    //
    template <typename ValueType>
    void SpatialConvolutionNode<ValueType>::spatial_convolutional_kernel(value::Tensor output, value::Tensor input, value::Tensor weights, value::Scalar row, value::Scalar column, value::Scalar channel)
    {
        const auto& parameters = _layer.GetConvolutionalParameters();
        const int receptiveFieldRows = (int)parameters.receptiveField;
        const int receptiveFieldColumns = (int)parameters.receptiveField;
        const int rowStride = (int)parameters.stride;
        const int columnStride = (int)parameters.stride;

        value::Scalar temp = value::Allocate(output.GetValue().GetBaseType(), ell::utilities::ScalarLayout);
        temp = static_cast<ValueType>(0.0);

        // Unroll the calculations for the receptive field size in row and column dimensions
        for (int k_r = 0; k_r < receptiveFieldRows; ++k_r)
        {
            for (int k_c = 0; k_c < receptiveFieldColumns; ++k_c)
            {
                // Weight filters are stacked in the row dimension. For spatial convolutions, the weights channel index is always 0
                // since there is only one channel per filter.
                temp += input(row * rowStride + k_r, column * columnStride + k_c, channel) * weights(channel * receptiveFieldRows + k_r, k_c, 0);
            }
        }
        output(row, column, channel) = temp;
    }

    template <typename ValueType>
    void SpatialConvolutionNode<ValueType>::Define(ell::value::FunctionDeclaration& fn)
    {
        (void)fn.Define([this](const value::Tensor input_tensor, value::Tensor output) {
            namespace loopnests = ell::value::loopnests;

            value::Value input_value(input_tensor.GetValue());
            input_value.SetLayout(utilities::MemoryLayout(input_value.GetLayout().GetExtent(), input_value.GetLayout().GetLogicalDimensionOrder()));
            value::Tensor input(input_value);

            // Declare constants
            const auto& w = _layer.GetWeights();
            std::vector<ValueType> data = w.ToArray();
            value::Tensor weights({ data,
                                    utilities::MemoryLayout({ static_cast<int>(w.NumRows()), static_cast<int>(w.NumColumns()), static_cast<int>(w.NumChannels()) },
                                                            utilities::DimensionOrder(utilities::RowMajorTensorOrder)) });

            // Declare the indexes
            loopnests::IndexRange i("i", { 0, (int)(output.Rows()) });
            loopnests::IndexRange j("j", { 0, (int)(output.Columns()) });
            loopnests::IndexRange k("k", { 0, (int)(output.Channels()) });

            auto kernel = loopnests::Kernel("kernel")
                              .Inputs(output.GetValue(), input.GetValue(), weights.GetValue())
                              .Indices(i.GetIndex(), j.GetIndex(), k.GetIndex())
                              .Define([this](value::Tensor output, value::Tensor input, value::Tensor weights, value::Scalar row, value::Scalar column, value::Scalar channel) {
                                  spatial_convolutional_kernel(output, input, weights, row, column, channel);
                              });

            loopnests::LoopNest loop(std::vector<loopnests::IndexRange>{ i, j, k });
            loop.AddKernel(kernel);
            loop.SetLoopOrder({ k.GetIndex(), i.GetIndex(), j.GetIndex() });

            loopnests::CodeGenerator generator;
            generator.Run(loop);
        });
    }

    template <typename ValueType>
    void SpatialConvolutionNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver["outputLayout"] << _output.GetMemoryLayout();
        archiver["layer"] << _layer;
    }

    template <typename ValueType>
    void SpatialConvolutionNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        model::PortMemoryLayout outputMemoryLayout;
        archiver["outputLayout"] >> outputMemoryLayout;
        _output.SetMemoryLayout(outputMemoryLayout);
        archiver["layer"] >> _layer;
    }

    template <typename ValueType>
    void SpatialConvolutionNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newInputs = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<SpatialConvolutionNode<ValueType>>(newInputs, _layer, _output.GetMemoryLayout());
        transformer.MapNodeOutput(output, newNode->output);
    }

} // namespace nodes
} // namespace ell

#pragma endregion implementation
