////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ReceptiveFieldMatrixNode.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/CompilableNode.h>
#include <model/include/CompilableNodeUtilities.h>
#include <model/include/IRMapCompiler.h>
#include <model/include/Model.h>
#include <model/include/ModelTransformer.h>
#include <model/include/Node.h>
#include <model/include/OutputNode.h>
#include <model/include/Port.h>

#include <utilities/include/Exception.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary>
    /// A node that reshapes an image into a format suitable for computing convolutions via matrix multiplication.
    /// Also known as Im2Col.
    /// </summary>
    template <typename ValueType>
    class ReceptiveFieldMatrixNode : public model::CompilableNode
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default constructor. </summary>
        ReceptiveFieldMatrixNode();

        /// <summary> Constructor. </summary>
        ///
        /// <param name="input"> The input image. </param>
        /// <param name="inputMemoryLayout"> The memory layout of the input image. </param>
        /// <param name="filterWidth"> The width of the convolution filters. </param>
        /// <param name="stride"> The distance between filter application. </param>
        /// <param name="convolutionPadding"> The amount of padding to use when performing the convolution. </param>
        /// <param name="dataOrder"> Indicates the order of the dimensions in the data. The first entry is the index of the slowest-incrementing dimension, and the last entry is the index of the fastest-moving dimensions. So, the canonical row, column, channel order is {0, 1, 2}. </param>
        /// <param name="outputWidth"> The output image width. </param>
        /// <param name="outputHeight"> The output image height. </param>
        ReceptiveFieldMatrixNode(const model::OutputPort<ValueType>& input,
                                 const model::PortMemoryLayout& inputMemoryLayout,
                                 int filterWidth,
                                 int stride,
                                 int convolutionPadding,
                                 std::array<int, 3> dataOrder, // 0, 1, 2 == rows, columns, channels -- row-major
                                 int outputWidth,
                                 int outputHeight);

        /// <summary> Gets information about the input memory layout </summary>
        const model::PortMemoryLayout& GetInputMemoryLayout() const { return _inputMemoryLayout; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("ReceptiveFieldMatrixNode"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        void Compute() const override;
        void Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function) override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

        bool HasState() const override { return true; } // stored state: input layout, output shape, conv params

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        // Input
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

        model::PortMemoryLayout _inputMemoryLayout;
        int _filterWidth;
        int _stride;
        int _convolutionPadding;
        std::array<int, 3> _dataOrder;
        int _outputWidth;
        int _outputHeight;
    };
} // namespace nodes
} // namespace ell

#pragma region implementation

namespace ell
{
namespace nodes
{
    namespace
    {
        //
        // Functions
        //

        // Note: this function is inline to suppress a compiler warning about it being unneeded
        inline emitters::LLVMValue GetValueFromVolume(emitters::IRFunctionEmitter& function,
                                                      emitters::LLVMValue inputVolume,
                                                      const model::PortMemoryLayout& inputLayout,
                                                      std::array<int, 3> dataOrder,
                                                      emitters::IRLocalScalar valueRow,
                                                      emitters::IRLocalScalar valueColumn,
                                                      emitters::IRLocalScalar valueChannel)
        {
            const auto rowStride = inputLayout.GetExtent(0);
            const auto columnStride = inputLayout.GetExtent(1);
            const auto channelStride = inputLayout.GetExtent(2);

            auto index = function.LocalScalar();
            if (dataOrder == std::array<int, 3>({ 0, 1, 2 }))
            {
                // row, column, channel order
                index = valueRow * (columnStride * channelStride) + (valueColumn * channelStride) + valueChannel;
            }
            else
            {
                // channel, row, column order
                index = valueChannel * (rowStride * columnStride) + (valueRow * columnStride) + valueColumn;
            }

            return function.ValueAt(inputVolume, index);
        }

        template <typename ValueType>
        emitters::LLVMValue GetValueFromPaddedVolume(emitters::IRFunctionEmitter& function,
                                                     emitters::LLVMValue inputVolume,
                                                     const model::PortMemoryLayout& inputLayout,
                                                     int convPadding,
                                                     std::array<int, 3> dataOrder,
                                                     emitters::IRLocalScalar inputRow,
                                                     emitters::IRLocalScalar inputColumn,
                                                     emitters::IRLocalScalar inputChannel)
        {
            const int inputHeight = inputLayout.GetActiveSize(0);
            const int inputWidth = inputLayout.GetActiveSize(1);
            const int inputDepth = inputLayout.GetActiveSize(2);
            const int inputPadding = inputLayout.GetOffset(0); // a proxy for the padding

            const int extraPaddingVal = convPadding - inputPadding; // amount by which the convolution's desired padding exceeds input's
            auto extraPadding = function.LocalScalar(extraPaddingVal);
            if (extraPaddingVal > 0) // known at compile-time
            {
                auto valueRow = inputRow - extraPadding;
                auto valueColumn = inputColumn - extraPadding;

                auto rowBad = (valueRow < 0) || (valueRow >= inputHeight);
                auto colBad = (valueColumn < 0) || (valueColumn >= inputWidth);
                auto outOfBounds = rowBad || colBad;

                emitters::LLVMValue returnValue = function.Variable(emitters::GetVariableType<ValueType>(), "returnVal");
                function.If(outOfBounds, [=](emitters::IRFunctionEmitter& function) {
                            function.StoreZero(returnValue);
                        })
                    .Else([=](emitters::IRFunctionEmitter& function) {
                        // channel, row, col order
                        auto index1 = valueRow * (inputWidth * inputDepth);
                        auto index2 = valueColumn * inputDepth;
                        auto index = index1 + index2 + inputChannel;
                        auto val = function.ValueAt(inputVolume, index);

                        // Note: we can't return from within an if/else block, so we store the value in a local variable
                        function.Store(returnValue, val);
                    });

                return function.Load(returnValue);
            }

            if (extraPaddingVal != 0) // negative
            {
                inputRow = inputRow + extraPadding;
                inputColumn = inputColumn + extraPadding;
            }
            return GetValueFromVolume(function, inputVolume, inputLayout, dataOrder, inputRow, inputColumn, inputChannel);
        }

        template <typename ValueType>
        void EmitReceptiveFieldToColumns(emitters::IRFunctionEmitter& function,
                                         emitters::LLVMValue inputVolume,
                                         const model::PortMemoryLayout& inputLayout,
                                         int filterWidth,
                                         int stride,
                                         int convPadding, // amount of padding to assume around the image -- determines output size
                                         std::array<int, 3> dataOrder,
                                         int outputWidth,
                                         int outputHeight,
                                         emitters::LLVMValue outputMatrix)
        {
            // Model parameters
            const auto inputHeight = inputLayout.GetLogicalDimensionActiveSize(0);
            const auto inputWidth = inputLayout.GetLogicalDimensionActiveSize(1);
            const auto inputDepth = inputLayout.GetLogicalDimensionActiveSize(2);
            const auto fieldVolumeSize = filterWidth * filterWidth * inputDepth;
            const auto numOutputColumns = static_cast<int>(outputWidth * outputHeight);

            // Input (I): d x h x w (planar)
            // Output (S): (d * k * k) x (outputHeight * outputWidth) ==  fieldVolumeSize x outputImageSize

            // Example
            // k = 3, d = 2
            //
            //      A B C D    a b c d
            // I =  E F G H    e f g h
            //      I J K L    i j k l
            //      M N O P    m n o p
            //
            //      . . . .  . A B C  D E F G  H I J K
            //      . . . .  . a b c  d e f g  h i j k
            //      . . . .  A B C D  E F G H  I J K L
            //      . . . .  a b c d  e f g h  i j k l
            //      . . . .  B C D E  F G H I  J K L M
            //      . . . .  b c d e  f g h i  j k l m
            //
            //      . A B C  D E F G  H I J K  L M N O
            //      . a b c  d e f g  h i j k  l m n o
            // S =  A B C D  E F G H  I J K L  M N O P
            //      a b c d  e f g h  i j k l  m n o p
            //      B C D E  F G H I  J K L M  N O P .
            //      b c d e  f g h i  j k l m  n o p .
            //
            //      D E F G  H I J K  L M N O  . . . .
            //      d e f g  h i j k  l m n o  . . . .
            //      E F G H  I J K L  M N O P  . . . .
            //      e f g h  i j k l  m n o p  . . . .
            //      F G H I  J K L M  N O P .  . . . .
            //      f g h i  j k l m  n o p .  . . . .
            //
            // Note that the middle d=2 rows of S are the entire image, linearized:
            // A B C D E F G H I J K L M N O P a b c d e f g h i j k l m n o p

            // const int extraPadding = (int)convPadding - (int)inputPadding; // extraPadding is the amount of extra padding we need to do, on top of what's in the input data
            const int extraPadding = convPadding;
            const bool useContiguousReshape = (dataOrder == std::array<int, 3>({ { 2, 0, 1 } })) && (stride == 1); // channel, row, column order, unit stride
            if (useContiguousReshape)
            {
                // assert(inputPadding == 0 && "Input data must not be padded");
                // Points to the beginning of the input volume
                emitters::LLVMValue inputPtr = function.PointerOffset(inputVolume, 0);

                // Points to the beginning of the outputMatrix
                emitters::LLVMValue outputPtr = function.PointerOffset(outputMatrix, 0);

                // Unroll outer loops
                for (int fy = 0; fy < filterWidth; ++fy)
                {
                    for (int fx = 0; fx < filterWidth; ++fx)
                    {
                        // `outputRow` is the row of the output matrix to start writing to. Multiplied by `inputDepth`, because
                        // we're going to memcpy `inputDepth` rows at once
                        int outputRow = (fy * filterWidth + fx) * inputDepth;

                        int outputOffset1 = inputWidth * (extraPadding - fy); // where to start writing this row in the output
                        int outputOffset2 = (extraPadding - fx); // where to start writing this row in the output
                        int inputOffset = 0; // where to start reading from for this row
                        if (outputOffset1 < 0)
                        {
                            inputOffset -= outputOffset1;
                            outputOffset1 = 0;
                        }
                        if (outputOffset2 < 0)
                        {
                            inputOffset -= outputOffset2;
                            outputOffset2 = 0;
                        }
                        int outputOffset = outputOffset1 + outputOffset2;
                        int count = (inputWidth * inputHeight * inputDepth) - inputOffset - outputOffset;
                        outputOffset += outputRow * numOutputColumns;

                        // For this output row, copy what we need from the input image
                        function.MemoryCopy<ValueType>(inputPtr, inputOffset, outputPtr, outputOffset, count);
                        const int outputRowOffset = outputRow * numOutputColumns;

                        // Zero out the padding areas
                        // BUG: explicit capture-by-ref entries are here to work around a GCC bug
                        function.For(inputDepth, [=, &fx, &fy, &extraPadding, &inputWidth, &inputHeight, &outputWidth, &numOutputColumns](emitters::IRFunctionEmitter& function, emitters::LLVMValue channelValue) {
                            auto channel = function.LocalScalar(channelValue);
                            auto outputDepthOffset = channel * numOutputColumns;

                            // Points to the beginning of the current channel in the outputMatrix
                            auto outputChannelPtr = function.PointerOffset(outputMatrix, outputDepthOffset);

                            uint8_t paddingValue = 0;
                            if (fy < extraPadding)
                            {
                                // zero out full image rows at beginning of image
                                int count = (extraPadding - fy) * outputWidth;
                                int begin = 0;
                                function.MemorySet<ValueType>(outputChannelPtr, outputRowOffset + begin, function.Literal<uint8_t>(paddingValue), count);
                            }
                            else if (fy > extraPadding)
                            {
                                // zero out full image rows at end of image
                                int count = (fy - extraPadding) * outputWidth;
                                int begin = numOutputColumns - count;
                                assert(begin >= 0);
                                function.MemorySet<ValueType>(outputChannelPtr, outputRowOffset + begin, function.Literal<uint8_t>(paddingValue), count);
                            }

                            if (fx < extraPadding)
                            {
                                // zero out elements at beginning of each row
                                int count = extraPadding - fx;
                                // BUG: explicit capture-by-ref entries are here to work around a GCC bug
                                function.For(inputHeight, [=, &inputWidth, &outputRowOffset](emitters::IRFunctionEmitter& function, emitters::LLVMValue indexValue) {
                                    auto index = function.LocalScalar(indexValue);
                                    auto begin = index * inputWidth;
                                    auto offset = begin + outputRowOffset;
                                    function.MemorySet<ValueType>(outputChannelPtr, offset, function.Literal<uint8_t>(paddingValue), count);
                                });
                            }
                            else if (fx > extraPadding)
                            {
                                // zero out elements at end of each row
                                int count = fx - extraPadding;
                                // BUG: explicit capture-by-ref entries are here to work around a GCC bug
                                function.For(inputHeight, [=, &inputWidth, &outputRowOffset](emitters::IRFunctionEmitter& function, emitters::LLVMValue indexValue) {
                                    auto index = function.LocalScalar(indexValue);
                                    auto begin = ((index + 1) * inputWidth) - count;
                                    auto offset = begin + outputRowOffset;
                                    function.MemorySet<ValueType>(outputChannelPtr, offset, function.Literal<uint8_t>(paddingValue), count);
                                });
                            }
                        });
                    }
                }
            }
            else // Normal, single value-at-a-time method
            {
                // The outer loop iterates over all d * k * k entries in the receptive field
                function.For(fieldVolumeSize, [=](emitters::IRFunctionEmitter& function, emitters::LLVMValue fValue) {
                    auto f = function.LocalScalar(fValue);
                    auto fieldChannel = function.LocalScalar();
                    auto fieldColumn = function.LocalScalar();
                    auto fieldRow = function.LocalScalar();

                    // TODO: use the entries of dataOrder to compute the indices
                    if (dataOrder == std::array<int, 3>({ { 0, 1, 2 } })) // row, column, channel order
                    {
                        fieldChannel = f % inputDepth;
                        auto fDivDepth = f / inputDepth;
                        fieldColumn = fDivDepth % filterWidth;
                        fieldRow = fDivDepth / filterWidth;
                    }
                    else // channel, row, column order
                    {
                        fieldColumn = f % filterWidth;
                        auto fDivColumns = f / filterWidth;
                        fieldRow = fDivColumns % filterWidth;
                        fieldChannel = fDivColumns / filterWidth;
                    }

                    // Now for each receptive field entry, iterate over all h * w locations in the output image
                    function.For(outputHeight, [=, &fieldRow, &fieldColumn](emitters::IRFunctionEmitter& function, emitters::LLVMValue outputImageRowValue) {
                        auto outputImageRow = function.LocalScalar(outputImageRowValue);
                        auto inputRow = outputImageRow * stride;
                        function.For(outputWidth, [=, &fieldRow, &fieldColumn, &inputRow](emitters::IRFunctionEmitter& function, emitters::LLVMValue outputImageColumnValue) {
                            auto outputImageColumn = function.LocalScalar(outputImageColumnValue);
                            auto inputColumn = outputImageColumn * stride;

                            // outRowOffset is the offset to the f'th row in the output S matrix
                            auto outRowOffset = f * (outputHeight * outputWidth);

                            // outColRowOffset is the offset to the column of the S matrix where `outputImageRow` begins
                            auto outColRowOffset = outputImageRow * outputWidth;
                            // outputIndex is the index of the entry in S to write to
                            auto outputIndex = outRowOffset + (outColRowOffset + outputImageColumn);

                            // input row and column in the input image
                            auto entryRow = inputRow + fieldRow;
                            auto entryColumn = inputColumn + fieldColumn;
                            auto volumeValue = GetValueFromPaddedVolume<ValueType>(function, inputVolume, inputLayout, extraPadding, dataOrder, entryRow, entryColumn, fieldChannel);
                            function.SetValueAt(outputMatrix, outputIndex, volumeValue);
                        });
                    });
                });
            }
        }
    } // namespace

    //
    // ReceptiveFieldMatrixNode
    //
    template <typename ValueType>
    ReceptiveFieldMatrixNode<ValueType>::ReceptiveFieldMatrixNode() :
        CompilableNode({ &_input }, { &_output }),
        _input(this, {}, defaultInputPortName),
        _output(this, defaultOutputPortName, 0),
        _filterWidth(0),
        _stride(0),
        _convolutionPadding(0),
        _dataOrder({ { 0, 1, 2 } }),
        _outputWidth(0),
        _outputHeight(0)
    {
    }

    template <typename ValueType>
    ReceptiveFieldMatrixNode<ValueType>::ReceptiveFieldMatrixNode(const model::OutputPort<ValueType>& input, const model::PortMemoryLayout& inputMemoryLayout, int filterWidth, int stride, int convolutionPadding, std::array<int, 3> dataOrder, int outputWidth, int outputHeight) :
        CompilableNode({ &_input }, { &_output }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, model::PortMemoryLayout(model::MemoryShape{ outputWidth * outputHeight, filterWidth * filterWidth * inputMemoryLayout.GetLogicalDimensionActiveSize(2) }, model::DimensionOrder{ dataOrder })),
        _inputMemoryLayout(inputMemoryLayout),
        _filterWidth(filterWidth),
        _stride(stride),
        _convolutionPadding(convolutionPadding),
        _dataOrder(dataOrder),
        _outputWidth(outputWidth),
        _outputHeight(outputHeight)
    {
        if (inputMemoryLayout.NumDimensions() != 3)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "ReceptiveFieldMatrixNode: inputMemoryLayout must have 3 dimensions");
        }
    }

    template <typename ValueType>
    void ReceptiveFieldMatrixNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newInputs = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<ReceptiveFieldMatrixNode>(newInputs, GetInputMemoryLayout(), _filterWidth, _stride, _convolutionPadding, _dataOrder, _outputWidth, _outputHeight);
        transformer.MapNodeOutput(this->output, newNode->output);
    }

    template <typename ValueType>
    void ReceptiveFieldMatrixNode<ValueType>::Compute() const
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
    }

    template <typename ValueType>
    void ReceptiveFieldMatrixNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        emitters::LLVMValue pInput = compiler.EnsurePortEmitted(this->input);
        emitters::LLVMValue pOutput = compiler.EnsurePortEmitted(this->output);

        const auto& inputLayout = this->GetInputMemoryLayout();
        assert(inputLayout.NumDimensions() == 3);

        // Re-shape input
        EmitReceptiveFieldToColumns<ValueType>(function, pInput, inputLayout, _filterWidth, _stride, _convolutionPadding, _dataOrder, _outputWidth, _outputHeight, pOutput);
    }

    template <typename ValueType>
    void ReceptiveFieldMatrixNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver[defaultOutputPortName] << _output;
        archiver["inputLayout"] << _inputMemoryLayout;

        archiver["filterWidth"] << _filterWidth;
        archiver["stride"] << _stride;

        archiver["convolutionPadding"] << _convolutionPadding;

        std::vector<int> dataOrder(_dataOrder.begin(), _dataOrder.end());
        archiver["dataOrder"] << dataOrder;

        archiver["outputWidth"] << _outputWidth;
        archiver["outputHeight"] << _outputHeight;
    }

    template <typename ValueType>
    void ReceptiveFieldMatrixNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver[defaultOutputPortName] >> _output;
        archiver["inputLayout"] >> _inputMemoryLayout;

        archiver["filterWidth"] >> _filterWidth;
        archiver["stride"] >> _stride;
        archiver["convolutionPadding"] >> _convolutionPadding;

        std::vector<int> dataOrder;
        archiver["dataOrder"] >> dataOrder;
        std::copy(dataOrder.begin(), dataOrder.end(), _dataOrder.begin());

        archiver["outputWidth"] >> _outputWidth;
        archiver["outputHeight"] >> _outputHeight;
    }
} // namespace nodes
} // namespace ell

#pragma endregion implementation
