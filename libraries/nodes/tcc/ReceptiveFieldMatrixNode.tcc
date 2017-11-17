////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ReceptiveFieldMatrixNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// model
#include "CompilableNodeUtilities.h"
#include "IRMapCompiler.h"
#include "OutputNode.h"

// utilities
#include "Exception.h"

// stl
#include <cassert>
#include <string>
#include <vector>

#include <iostream>

namespace ell
{
namespace nodes
{
    namespace
    {
        // Useful aliases for operators
        const auto plus = emitters::TypedOperator::add;
        const auto minus = emitters::TypedOperator::subtract;
        const auto times = emitters::TypedOperator::multiply;
        const auto divide = emitters::TypedOperator::divideSigned;
        const auto modulo = emitters::TypedOperator::moduloSigned;

        const auto plusFloat = emitters::TypedOperator::addFloat;
        const auto minusFloat = emitters::TypedOperator::subtractFloat;
        const auto timesFloat = emitters::TypedOperator::multiplyFloat;
        const auto divideFloat = emitters::TypedOperator::divideFloat;

        const auto logicalOr = emitters::TypedOperator::logicalOr;
        const auto logicalAnd = emitters::TypedOperator::logicalAnd;
        const auto shiftLeft = emitters::TypedOperator::shiftLeft;

        // comparisons
        const auto lessThan = emitters::TypedComparison::lessThan;
        const auto greaterThanOrEqual = emitters::TypedComparison::greaterThanOrEquals;
        const auto greaterThanFloat = emitters::TypedComparison::greaterThanFloat;

        //
        // Functions
        //
        llvm::Value* GetValueFromVolume(emitters::IRFunctionEmitter& function,
                                        llvm::Value* inputVolume,
                                        const model::PortMemoryLayout& inputLayout,
                                        std::array<int, 3> dataOrder,
                                        llvm::Value* valueRow, llvm::Value* valueColumn, llvm::Value* valueChannel)
        {
            const auto inputHeight = inputLayout.GetActiveSize(0);
            const auto inputWidth = inputLayout.GetActiveSize(1);
            const auto inputDepth = inputLayout.GetActiveSize(2);
            const auto rowStride = inputLayout.GetStride(0);
            const auto columnStride = inputLayout.GetStride(1);
            const auto channelStride = inputLayout.GetStride(2);

            llvm::Value* index = nullptr;
            // TODO: actually use the entries in dataOrder to compute the indices
            if(dataOrder == std::array<int, 3>({0, 1, 2})) // row, column, channel order
            {
                // index = (valueRow * inputWidth * inputDepth) + (valueColumn * inputDepth) + valueChannel;
                auto index1 = function.Operator(times, valueRow, function.Literal<int>(columnStride * channelStride));
                auto index2 = function.Operator(times, valueColumn, function.Literal<int>(channelStride));
                index = function.Operator(plus, index1, function.Operator(plus, index2, valueChannel));
            }
            else
            {
                // channel, row, column order
                // index = (valueChannel * inputHeight * inputWidth) + (valueRow * inputWidth) + valueColumn;
                auto index2 = function.Operator(times, valueChannel, function.Literal<int>(rowStride * columnStride));
                auto index1 = function.Operator(times, valueRow, function.Literal<int>(columnStride));
                index = function.Operator(plus, index1, function.Operator(plus, index2, valueColumn));
            }

            return function.ValueAt(inputVolume, index);
        }

        // TODO: adapt this to work with more generally strided data
        template <typename ValueType>
        llvm::Function* EmitGetValueFromPaddedVolumeFunction(emitters::IRModuleEmitter& moduleEmitter, std::array<int, 3> dataOrder)
        {
            std::string functionName = std::string("GetValueFromPaddedVolume") + std::to_string(dataOrder[0]) + std::to_string(dataOrder[1]) + std::to_string(dataOrder[2]);
            llvm::Function* getValueFunction = moduleEmitter.GetFunction(functionName);
            if (getValueFunction != nullptr)
            {
                return getValueFunction;
            }

            auto& emitter = moduleEmitter.GetIREmitter();
            auto valueType = emitter.Type(emitters::GetVariableType<ValueType>());
            auto valuePtrType = valueType->getPointerTo();
            auto int32Type = emitter.Type(emitters::VariableType::Int32);

            // args: {volume, row, col, channel, width, height, depth, padding}
            emitters::IRFunctionEmitter function = moduleEmitter.BeginFunction(functionName, valueType, { valuePtrType, int32Type, int32Type, int32Type, int32Type, int32Type, int32Type, int32Type });

            llvm::Value* scratch = function.Variable(emitters::GetVariableType<ValueType>(), "scratch");

            auto arguments = function.Arguments().begin();
            auto inputVolume = &(*arguments++);
            auto row = &(*arguments++);
            auto col = &(*arguments++);
            auto channel = &(*arguments++);
            auto width = &(*arguments++);
            auto height = &(*arguments++);
            auto depth = &(*arguments++);
            auto padding = &(*arguments++);

            auto valueRow = function.Operator(minus, row, padding);
            auto valueColumn = function.Operator(minus, col, padding);

            auto tooSmallRow = function.Comparison(lessThan, valueRow, function.Literal(0));
            auto tooSmallCol = function.Comparison(lessThan, valueColumn, function.Literal(0));
            auto tooBigRow = function.Comparison(greaterThanOrEqual, valueRow, height);
            auto tooBigCol = function.Comparison(greaterThanOrEqual, valueColumn, width);

            auto rowBad = function.Operator(emitters::TypedOperator::logicalOr, tooSmallRow, tooBigRow);
            auto colBad = function.Operator(emitters::TypedOperator::logicalOr, tooSmallCol, tooBigCol);
            auto outOfBounds = function.Operator(emitters::TypedOperator::logicalOr, rowBad, colBad);

            auto oobIfEmitter = function.If();
            oobIfEmitter.If(outOfBounds);
            {
                // Note: we can't return from within an if/else block, so we store the value in a local variable
                function.StoreZero(scratch);
            }
            oobIfEmitter.Else();
            {
                // channel, row, col order
                auto index1 = function.Operator(times, valueRow, function.Operator(times, width, depth));
                auto index2 = function.Operator(times, valueColumn, depth);
                auto index = function.Operator(plus, index1, function.Operator(plus, index2, channel));
                auto val = function.ValueAt(inputVolume, index);

                // Note: we can't return from within an if/else block, so we store the value in a local variable
                function.Store(scratch, val);
            }
            oobIfEmitter.End();

            function.Return(function.Load(scratch));
            moduleEmitter.EndFunction();
            return function.GetFunction();
        }

        template <typename ValueType>
        llvm::Value* GetValueFromPaddedVolume(emitters::IRFunctionEmitter& function,
                                              llvm::Value* inputVolume,
                                              const model::PortMemoryLayout& inputLayout,
                                              size_t convPadding,
                                              std::array<int, 3> dataOrder,
                                              llvm::Value* inputRow, llvm::Value* inputCol, llvm::Value* inputChannel)
        {
            const auto inputHeight = inputLayout.GetActiveSize(0);
            const auto inputWidth = inputLayout.GetActiveSize(1);
            const auto inputDepth = inputLayout.GetActiveSize(2);
            const auto inputPadding = inputLayout.GetOffset(0); // a proxy for the padding

            const int extraPadding = convPadding - inputPadding; // amount by which the convolution's desired padding exceeds input's
            if (extraPadding > 0) // known at compile-time
            {
                auto getValueFunction = EmitGetValueFromPaddedVolumeFunction<ValueType>(function.GetModule(), dataOrder);

                // Offset row and col to account for padding
                return function.Call(getValueFunction, { inputVolume, inputRow, inputCol, inputChannel, function.Literal<int>(inputWidth), function.Literal<int>(inputHeight), function.Literal<int>(inputDepth), function.Literal<int>(extraPadding) });
            }

            if (extraPadding != 0) // negative
            {
                inputRow = function.Operator(plus, inputRow, function.Literal<int>(extraPadding));
                inputCol = function.Operator(plus, inputCol, function.Literal<int>(extraPadding));
            }
            return GetValueFromVolume(function, inputVolume, inputLayout, dataOrder, inputRow, inputCol, inputChannel);
        }

        template <typename ValueType>
        void EmitReceptiveFieldToColumns(emitters::IRFunctionEmitter& function,
                                         llvm::Value* inputVolume,
                                         const model::PortMemoryLayout& inputLayout,
                                         size_t filterWidth,
                                         size_t stride,
                                         size_t convPadding, // amount of padding to assume around the image -- determines output size
                                         std::array<int, 3> dataOrder,
                                         size_t outputWidth,
                                         size_t outputHeight,
                                         llvm::Value* outputMatrix)
        {
            // Model parameters
            const auto inputHeight = inputLayout.GetActiveSize(0);
            const auto inputWidth = inputLayout.GetActiveSize(1);
            const auto inputDepth = inputLayout.GetActiveSize(2);
            const auto inputPadding = inputLayout.GetOffset(0); // a proxy for the padding provided in the input
            const auto fieldVolumeSize = filterWidth * filterWidth * inputDepth;
            const auto numOutputColumns = outputWidth * outputHeight;
            const auto numOutputRows = filterWidth * filterWidth * inputDepth;

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
            // Note that the middle 2 rows of S are the entire image, linearized:
            // A B C D E F G H I J K L M N O P a b c d e f g h i j k l m n o p

            // const int extraPadding = (int)convPadding - (int)inputPadding; // extraPadding is the amount of extra padding we need to do, on top of what's in the input data
            const int extraPadding = (int)convPadding;
            const bool useNewReshape = dataOrder == std::array<int, 3>({2, 0, 1}); // channel, row, column order
            if (useNewReshape && stride == 1 && extraPadding >= 0)
            {
                // assert(inputPadding == 0 && "Input data must not be padded");
                // Points to the beginning of the input volume
                llvm::Value* inputPtr = function.PointerOffset(inputVolume, 0);

                // Points to the beginning of the outputMatrix
                llvm::Value* outputPtr = function.PointerOffset(outputMatrix, 0);

                for (int fy = 0; fy < filterWidth; ++fy)
                {
                    for (int fx = 0; fx < filterWidth; ++fx)
                    {
                        int outputRow = (fy * filterWidth + fx) * inputDepth; // The row of the output matrix to start writing to. Multiplied by inputDepth, because
                        // we're going to memcpy `inputDepth` rows at once

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

                        // For this output row, copy what we need from the input image...
                        function.MemoryCopy<ValueType>(inputPtr, inputOffset, outputPtr, outputOffset, count);
                        const int outputRowOffset = outputRow * numOutputColumns;

                        // Zero out the padding areas
                        auto depthLoop = function.ForLoop();
                        depthLoop.Begin(inputDepth);
                        {
                            llvm::Value* channel = depthLoop.LoadIterationVariable();
                            llvm::Value* outputDepthOffset = function.Operator(times, channel, function.Literal<int>(numOutputColumns));

                            // Points to the beginning of the current channel in the outputMatrix
                            llvm::Value* outputChannelPtr = function.PointerOffset(outputMatrix, outputDepthOffset);

                            uint8_t paddingValue = 0;
                            if (fy < extraPadding)
                            {
                                // zero out full image rows at beginning
                                int count = (extraPadding - fy) * outputWidth;
                                int begin = 0;
                                // assert((outputRowOffset + begin + count < filterWidth * filterWidth * inputDepth * numOutputColumns) && "1");
                                function.MemorySet<ValueType>(outputChannelPtr, outputRowOffset + begin, function.Literal<uint8_t>(paddingValue), count);
                            }
                            else if (fy > extraPadding)
                            {
                                // zero out full image rows at end
                                int count = (fy - extraPadding) * outputWidth;
                                int begin = numOutputColumns - count;
                                // assert(outputRowOffset + begin + count < filterWidth * filterWidth * inputDepth * numOutputColumns && "2");
                                assert(begin >= 0);
                                function.MemorySet<ValueType>(outputChannelPtr, outputRowOffset + begin, function.Literal<uint8_t>(paddingValue), count);
                            }

                            if (fx < extraPadding)
                            {
                                // zero out elements at beginning of each row
                                count = extraPadding - fx;
                                auto loop = function.ForLoop();
                                loop.Begin(inputHeight);
                                {
                                    // int begin = index * inputWidth;
                                    // assert(outputRowOffset + begin + count < filterWidth * filterWidth * inputDepth * numOutputColumns && "3");
                                    // assert(begin >= 0);
                                    // auto offset = (outputRowOffset + begin) * inputDepth;
                                    auto index = loop.LoadIterationVariable();
                                    auto begin = function.Operator(times, index, function.Literal<int>(inputWidth));
                                    auto offset = function.Operator(plus, begin, function.Literal<int>(outputRowOffset));
                                    function.MemorySet<ValueType>(outputChannelPtr, offset, function.Literal<uint8_t>(paddingValue), count);
                                }
                                loop.End();
                            }
                            else if (fx > extraPadding)
                            {
                                // zero out elements at end of each row
                                count = fx - extraPadding;
                                auto loop = function.ForLoop();
                                loop.Begin(inputHeight);
                                {
                                    // int begin = (index + 1) * inputWidth - count;
                                    // assert(outputRowOffset + begin + count < filterWidth * filterWidth * inputDepth * numOutputColumns && "4");
                                    // assert(begin >= 0);
                                    // auto offset = (outputRowOffset + begin) * inputDepth;
                                    auto index = loop.LoadIterationVariable();
                                    auto begin = function.Operator(minus, function.Operator(times, function.Operator(plus, index, function.Literal<int>(1)), function.Literal<int>(inputWidth)), function.Literal<int>(count));
                                    auto offset = function.Operator(plus, begin, function.Literal<int>(outputRowOffset));
                                    function.MemorySet<ValueType>(outputChannelPtr, offset, function.Literal<uint8_t>(paddingValue), count);
                                }
                                loop.End();
                            }
                        }
                        depthLoop.End();
                    }
                }
            }
            else // Normal, single value-at-a-time method
            {
                llvm::Value* filterWidthVal = function.Literal<int>(filterWidth);
                llvm::Value* inputDepthVal = function.Literal<int>(inputDepth);

                // The outer loop iterates over all d * k * k entries in the receptive field
                auto outerLoop = function.ForLoop();
                outerLoop.Begin(fieldVolumeSize);
                {
                    auto f = outerLoop.LoadIterationVariable();

                    llvm::Value* fieldChannel = nullptr;
                    llvm::Value* fieldColumn = nullptr;
                    llvm::Value* fieldRow = nullptr;

                    // TODO: use the entries of dataOrder to compute the indices
                    if (dataOrder == std::array<int, 3>({0, 1, 2})) // row, column, channel order
                    {
                        fieldChannel = function.Operator(modulo, f, inputDepthVal);
                        auto fDivDepth = function.Operator(divide, f, inputDepthVal);
                        fieldColumn = function.Operator(modulo, fDivDepth, filterWidthVal);
                        fieldRow = function.Operator(divide, fDivDepth, filterWidthVal);
                    }
                    else // channel, row, column order
                    {

                        fieldColumn = function.Operator(modulo, f, filterWidthVal);
                        auto fDivColumns = function.Operator(divide, f, filterWidthVal);
                        fieldRow = function.Operator(modulo, fDivColumns, filterWidthVal);
                        fieldChannel = function.Operator(divide, fDivColumns, inputDepthVal);
                    }

                    // Now for each receptive field entry, iterate over all h * w locations in the output image
                    auto rowLoop = function.ForLoop();
                    rowLoop.Begin(outputHeight);
                    {
                        auto outputImageRow = rowLoop.LoadIterationVariable();
                        auto inputRow = function.Operator(times, outputImageRow, function.Literal<int>(stride));

                        auto columnLoop = function.ForLoop();
                        columnLoop.Begin(outputWidth);
                        {
                            auto outputImageColumn = columnLoop.LoadIterationVariable();
                            auto inputColumn = function.Operator(times, outputImageColumn, function.Literal<int>(stride));

                            // outRowOffset is the offset to the f'th row in the output S matrix
                            auto outRowOffset = function.Operator(times, f, function.Literal<int>(outputHeight * outputWidth));

                            // outColRowOffset is the offset to the column of the S matrix where `outputImageRow` begins
                            auto outColRowOffset = function.Operator(times, outputImageRow, function.Literal<int>(outputWidth));
                            // outputIndex is the index of the entry in S to write to
                            auto outputIndex = function.Operator(plus, outRowOffset, function.Operator(plus, outColRowOffset, outputImageColumn));

                            // input row and column in the input image
                            auto entryRow = function.Operator(plus, inputRow, fieldRow);
                            auto entryColumn = function.Operator(plus, inputColumn, fieldColumn);

                            auto volumeValue = GetValueFromPaddedVolume<ValueType>(function, inputVolume, inputLayout, extraPadding, dataOrder, entryRow, entryColumn, fieldChannel);
                            function.SetValueAt(outputMatrix, outputIndex, volumeValue);
                        }
                        columnLoop.End();
                    }
                    rowLoop.End();
                }
                outerLoop.End();
            }
        }
    }

    //
    // ReceptiveFieldMatrixNode
    //
    template <typename ValueType>
    ReceptiveFieldMatrixNode<ValueType>::ReceptiveFieldMatrixNode()
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, inputPortName), _output(this, outputPortName, 0), _filterWidth(0), _stride(0), _convolutionPadding(0), _dataOrder({0, 1, 2}), _outputWidth(0), _outputHeight(0)
    {
    }

    template <typename ValueType>
    ReceptiveFieldMatrixNode<ValueType>::ReceptiveFieldMatrixNode(const model::PortElements<ValueType>& input, const model::PortMemoryLayout& inputMemoryLayout, size_t filterWidth, size_t stride, size_t convolutionPadding, std::array<int, 3> dataOrder, size_t outputWidth, size_t outputHeight)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, filterWidth * filterWidth * inputMemoryLayout.GetActiveSize(2) * outputWidth * outputHeight), _inputMemoryLayout(inputMemoryLayout), _filterWidth(filterWidth), _stride(stride), _convolutionPadding(convolutionPadding), _dataOrder(dataOrder), _outputWidth(outputWidth), _outputHeight(outputHeight)
    {
    }

    template <typename ValueType>
    void ReceptiveFieldMatrixNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<ReceptiveFieldMatrixNode>(newPortElements, GetInputMemoryLayout(), _filterWidth, _stride, _convolutionPadding, _dataOrder, _outputWidth, _outputHeight);
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
        llvm::Value* pInput = compiler.EnsurePortEmitted(this->input);
        llvm::Value* pOutput = compiler.EnsurePortEmitted(this->output);

        auto&& inputLayout = this->GetInputMemoryLayout();
        assert(inputLayout.NumDimensions() == 3);

        // Re-shape input
        EmitReceptiveFieldToColumns<ValueType>(function, pInput, inputLayout, _filterWidth, _stride, _convolutionPadding, _dataOrder, _outputWidth, _outputHeight, pOutput);
    }
} // nodes
} // ell
