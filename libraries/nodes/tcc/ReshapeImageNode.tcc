////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ReshapeImageNode.tcc (nodes)
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
                                        const PortMemoryLayout& inputLayout,
                                        const predictors::neural::ConvolutionalParameters& convParams,
                                        llvm::Value* valueRow, llvm::Value* valueColumn, llvm::Value* valueChannel)
        {
            const auto inputHeight = inputLayout.size[0];
            const auto inputWidth = inputLayout.size[1];
            const auto inputDepth = inputLayout.size[2];
            const auto rowStride = inputLayout.stride[0];
            const auto columnStride = inputLayout.stride[1];
            const auto channelStride = inputLayout.stride[2];

            // row, column, channel order
            // index = (valueRow * inputWidth*inputDepth) + (valueColumn * inputDepth) + valueChannel;
            auto index1 = function.Operator(times, valueRow, function.Literal<int>(columnStride * channelStride));
            auto index2 = function.Operator(times, valueColumn, function.Literal<int>(channelStride));
            auto index = function.Operator(plus, index1, function.Operator(plus, index2, valueChannel));

            return function.ValueAt(inputVolume, index);
        }

        // TODO: adapt this to work with more generally strided data
        template<typename ValueType>
        llvm::Function* EmitGetValueFromPaddedVolumeFunction(emitters::IRModuleEmitter& moduleEmitter)
        {
            const auto functionName = "GetValuefromPaddedVolume";
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
                function.Store(scratch, function.Literal(static_cast<ValueType>(0.0)));
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

        template<typename ValueType>
        llvm::Value* GetValueFromPaddedVolume(emitters::IRFunctionEmitter& function,
                                              llvm::Value* inputVolume,
                                              const PortMemoryLayout& inputLayout,
                                              const predictors::neural::ConvolutionalParameters& convParams,
                                              size_t convPadding,
                                              llvm::Value* inputRow, llvm::Value* inputCol, llvm::Value* inputChannel)
        {
            const auto inputHeight = inputLayout.size[0];
            const auto inputWidth = inputLayout.size[1];
            const auto inputDepth = inputLayout.size[2];
            const auto inputPadding = inputLayout.offset[0]; // a proxy for the padding

            const int extraPadding = convPadding - inputPadding; // amount by which the convolution's desired padding exceeds input's
            if (extraPadding > 0) // known at compile-time
            {
                auto getValueFunction = EmitGetValueFromPaddedVolumeFunction<ValueType>(function.GetModule());
                return function.Call(getValueFunction, { inputVolume, inputRow, inputCol, inputChannel, function.Literal<int>(inputWidth), function.Literal<int>(inputHeight), function.Literal<int>(inputDepth), function.Literal<int>(extraPadding) });
            }

            if (extraPadding != 0)
            {
                inputRow = function.Operator(plus, inputRow, function.Literal<int>(extraPadding));
                inputCol = function.Operator(plus, inputCol, function.Literal<int>(extraPadding));
            }
            return GetValueFromVolume(function, inputVolume, inputLayout, convParams, inputRow, inputCol, inputChannel);
        }

        // TODO: emit this as a function in the module
        template<typename ValueType>
        void EmitReceptiveFieldToColumns(emitters::IRFunctionEmitter& function,
                                         llvm::Value* inputVolume,
                                         const PortMemoryLayout& inputLayout,
                                         const predictors::neural::ConvolutionalParameters& convParams,
                                         size_t outputWidth,
                                         size_t outputHeight,
                                         llvm::Value* outputMatrix)
        {

            auto& context = function.GetLLVMContext();
            auto& emitter = function.GetEmitter();
            auto& irBuilder = emitter.GetIRBuilder();

            // Model parameters
            const auto inputHeight = inputLayout.size[0];
            const auto inputWidth = inputLayout.size[1];
            const auto inputDepth = inputLayout.size[2];
            const auto inputPadding = inputLayout.offset[0]; // a proxy for the padding
            const auto filterWidth = convParams.receptiveField; // sometimes referred to as 'k'
            const auto fieldVolumeSize = filterWidth * filterWidth * inputDepth;

            // Input: h x w x d
            // Output: (d * k * k) x (outputHeight * outputWidth) == fieldVolumeSize x outputImageSize
            const auto stride = convParams.stride;
            const auto padding = inputLayout.offset[0]; // a proxy for the padding
            const auto numOutputColumns = outputWidth * outputHeight;
            const auto numOutputRows = filterWidth * filterWidth * inputDepth;

            const bool useNewReshape = false;

            if (useNewReshape && stride == 1 && (padding == filterWidth / 2))
            {
                assert(false && "Not implemented yet");
                auto depthLoop = function.ForLoop();
                depthLoop.Begin(inputDepth);
                {
                    llvm::Value* channel = depthLoop.LoadIterationVariable();
                    llvm::Value* inputDepthOffset = function.Operator(times, channel, function.Literal<int>(inputWidth * inputHeight));
                    llvm::Value* outputDepthOffset = function.Operator(times, channel, function.Literal<int>(numOutputColumns * filterWidth * filterWidth));

                    // Points to beginning of the current channel in inputVolume
                    llvm::Value* inputChannelPtr = function.PointerOffset(inputVolume, inputDepthOffset);

                    // Points to the beginning of the current channel in the outputMatrix
                    llvm::Value* outputChannelPtr = function.PointerOffset(outputMatrix, outputDepthOffset);

                    for (int fy = 0; fy < filterWidth; ++fy)
                    {
                        for (int fx = 0; fx < filterWidth; ++fx)
                        {
                            int outputRow = fy * filterWidth + fx; // the row within the current channel

                            int outputOffset = inputWidth * (padding - fy) + (padding - fx);
                            int inputOffset = 0;
                            if (outputOffset < 0)
                            {
                                inputOffset = -outputOffset;
                                outputOffset = 0;
                            }
                            int count = inputWidth * inputHeight;
                            if (inputOffset > 0)
                            {
                                assert(count >= inputOffset);
                                count -= inputOffset;
                            }
                            else if (outputOffset + count > numOutputColumns)
                            {
                                count = numOutputColumns - outputOffset;
                            }
                            outputOffset += outputRow * numOutputColumns;

                            // For this output row, we've copied what we need from the input image...
                            function.MemoryCopy<ValueType>(inputChannelPtr, inputOffset, outputChannelPtr, outputOffset, count);
                            uint8_t paddingValue = 0;
                            const int outputRowOffset = outputRow * numOutputColumns;

                            // Zero out the padding areas
                            if (fy < padding)
                            {
                                // zero out full image rows at beginning
                                int count = (padding - fy) * inputWidth; // should this be outputWidth?
                                int begin = 0;
                                assert((outputRowOffset + begin + count < filterWidth * filterWidth * inputDepth * numOutputColumns) && "1");
                                function.MemorySet<ValueType>(outputChannelPtr, outputRowOffset + begin, function.Literal<uint8_t>(paddingValue), count);
                            }
                            else if (fy > padding)
                            {
                                // zero out full image rows at end
                                int count = (fy - padding) * inputWidth;
                                int begin = numOutputColumns - count;
                                assert(outputRowOffset + begin + count < filterWidth * filterWidth * inputDepth * numOutputColumns && "2");
                                assert(begin >= 0);
                                function.MemorySet<ValueType>(outputChannelPtr, outputRowOffset + begin, function.Literal<uint8_t>(paddingValue), count);
                            }

                            // TODO: implement these as real loops... it's spamming out a ton of code
                            if (fx < padding)
                            {
                                // zero out elements at beginning of each row
                                count = padding - fx;
                                for (int index = 0; index < inputHeight; ++index) // if fy < padding, we can start this loop at padding-fy
                                {
                                    int begin = index * inputWidth;
                                    assert(outputRowOffset + begin + count < filterWidth * filterWidth * inputDepth * numOutputColumns && "3");
                                    assert(begin >= 0);
                                    function.MemorySet<ValueType>(outputChannelPtr, outputRowOffset + begin, function.Literal<uint8_t>(paddingValue), count);
                                }
                            }
                            else if (fx > padding)
                            {
                                // zero out elements at end of each row
                                count = fx - padding;
                                for (int index = 0; index < inputHeight; ++index) // if fy > padding, we can end this loop at height-(fy-padding)
                                {
                                    int begin = (index + 1) * inputWidth - count;
                                    assert(outputRowOffset + begin + count < filterWidth * filterWidth * inputDepth * numOutputColumns && "4");
                                    assert(begin >= 0);
                                    function.MemorySet<ValueType>(outputChannelPtr, outputRowOffset + begin, function.Literal<uint8_t>(paddingValue), count);
                                }
                            }
                        }
                    }
                }
                depthLoop.End();
            }
            else
            {
                // Normal, single value-at-a-time method
                auto valueType = emitters::GetVariableType<ValueType>();
                llvm::Value* filterWidthVal = function.Literal<int>(filterWidth);
                llvm::Value* inputDepthVal = function.Literal<int>(inputDepth);
                auto outerLoop = function.ForLoop();
                outerLoop.Begin(fieldVolumeSize);
                {
                    auto f = outerLoop.LoadIterationVariable();

                    // row, column, channel order
                    auto fieldDepth = function.Operator(modulo, f, inputDepthVal);
                    auto fOverDepth = function.Operator(divide, f, inputDepthVal);
                    auto fieldColumn = function.Operator(modulo, fOverDepth, filterWidthVal);
                    auto fieldRow = function.Operator(divide, fOverDepth, filterWidthVal);

                    auto rowLoop = function.ForLoop();
                    rowLoop.Begin(outputHeight);
                    {
                        auto outputImageRow = rowLoop.LoadIterationVariable();
                        auto inputRow = function.Operator(times, outputImageRow, function.Literal<int>(stride));
                        // TODO: we can just globally offset the input memory pointer and get rid of this offset index:
                        auto paddedInputRow = function.Operator(plus, inputRow, function.Literal<int>(inputLayout.offset[0]));
                        auto columnLoop = function.ForLoop();
                        columnLoop.Begin(outputWidth);
                        {
                            auto outputImageColumn = columnLoop.LoadIterationVariable();
                            auto inputColumn = function.Operator(times, outputImageColumn, function.Literal<int>(stride));
                            auto paddedInputColumn = function.Operator(plus, inputColumn, function.Literal<int>(inputLayout.offset[1]));

                            auto outRowOffset = function.Operator(times, f, function.Literal<int>(outputHeight * outputWidth));
                            auto outColRowOffset = function.Operator(times, outputImageRow, function.Literal<int>(outputWidth));
                            auto outputIndex = function.Operator(plus, outRowOffset, function.Operator(plus, outColRowOffset, outputImageColumn));
                            auto entryRow = function.Operator(plus, inputRow, fieldRow);
                            auto entryColumn = function.Operator(plus, inputColumn, fieldColumn);

                            auto volumeValue = GetValueFromPaddedVolume<ValueType>(function, inputVolume, inputLayout, convParams, padding, entryRow, entryColumn, fieldDepth);
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
    // ReshapeImageNode
    //
    template<typename ValueType>
    ReshapeImageNode<ValueType>::ReshapeImageNode()
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, inputPortName), _output(this, outputPortName, 0), _outputWidth(0), _outputHeight(0)
    {
    }

    template<typename ValueType>
    ReshapeImageNode<ValueType>::ReshapeImageNode(const model::PortElements<ValueType>& input, const PortMemoryLayout& inputMemoryLayout, const predictors::neural::ConvolutionalParameters& convolutionalParameters, size_t outputWidth, size_t outputHeight)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, convolutionalParameters.receptiveField * convolutionalParameters.receptiveField * inputMemoryLayout.size[2] * outputWidth * outputHeight), _inputMemoryLayout(inputMemoryLayout), _convolutionalParameters(convolutionalParameters), _outputWidth(outputWidth), _outputHeight(outputHeight)
    {
    }

    template<typename ValueType>
    void ReshapeImageNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<ReshapeImageNode>(newPortElements, GetInputMemoryLayout(), _convolutionalParameters, _outputWidth, _outputHeight);
        transformer.MapNodeOutput(this->output, newNode->output);
    }

    template<typename ValueType>
    void ReshapeImageNode<ValueType>::Compute() const
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
    }

    template<typename ValueType>
    void ReshapeImageNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        llvm::Value* pInput = compiler.EnsurePortEmitted(this->input);
        llvm::Value* pOutput = compiler.EnsurePortEmitted(this->output);

        auto&& inputLayout = this->GetInputMemoryLayout();
        assert(inputLayout.size.size() == 3);

        // Re-shape input
        EmitReceptiveFieldToColumns<ValueType>(function, pInput, inputLayout, _convolutionalParameters, _outputWidth, _outputHeight, pOutput);
    }
} // nodes
} // ell
