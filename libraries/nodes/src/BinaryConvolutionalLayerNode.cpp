////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BinaryConvolutionalLayerNode.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BinaryConvolutionalLayerNode.h"
#include "ConstantNode.h"

// stl
#include <algorithm>
#include <numeric>

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
        size_t GetFilterVolumeSize(const predictors::neural::BinaryConvolutionalParameters& convolutionalParameters, const PortMemoryLayout& inputMemoryLayout)
        {
            const auto inputDepth = inputMemoryLayout.size[2];
            const auto filterSize = convolutionalParameters.receptiveField;
            return inputDepth * filterSize * filterSize;
        }

        size_t GetMemorySize(const PortMemoryLayout& memoryLayout)
        {
            return std::accumulate(memoryLayout.stride.begin(), memoryLayout.stride.end(), 1, std::multiplies<size_t>());
        }

        template <typename PackedBitsType>
        size_t GetPackedFilterSize(const predictors::neural::BinaryConvolutionalParameters& convolutionalParameters, const PortMemoryLayout& inputMemoryLayout, const PortMemoryLayout& outputMemoryLayout)
        {
            const size_t numOutputPixels = outputMemoryLayout.size[0] * outputMemoryLayout.size[1];
            const size_t filterVolumeSize = GetFilterVolumeSize(convolutionalParameters, inputMemoryLayout);
            return ((filterVolumeSize - 1) / (8 * sizeof(PackedBitsType)) + 1) * numOutputPixels;
        }

        llvm::Value* GetValueFromVolume(emitters::IRFunctionEmitter& function,
                                        llvm::Value* inputVolume,
                                        const PortMemoryLayout& inputLayout,
                                        const predictors::neural::BinaryConvolutionalParameters& convParams,
                                        llvm::Value* valueRow, llvm::Value* valueColumn, llvm::Value* valueChannel)
        {
            const auto rowStride = inputLayout.stride[0];
            const auto columnStride = inputLayout.stride[1];
            const auto channelStride = inputLayout.stride[2];

            // row, column, channel order
            auto index1 = function.Operator(times, valueRow, function.Literal<int>(columnStride * channelStride));
            auto index2 = function.Operator(times, valueColumn, function.Literal<int>(channelStride));
            auto index = function.Operator(plus, index1, function.Operator(plus, index2, valueChannel));

            return function.ValueAt(inputVolume, index);
        }

        // TODO: adapt this to work with more generally strided data
        template <typename ValueType>
        llvm::Function* EmitGetValueFromPaddedVolumeFunction(emitters::IRModuleEmitter& moduleEmitter)
        {
            assert(false && "Not implemented");
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

            // args: {volume, row, col, depth, width, height, padding}
            emitters::IRFunctionEmitter function = moduleEmitter.BeginFunction(functionName, valueType, { valuePtrType, int32Type, int32Type, int32Type, int32Type, int32Type, int32Type });

            llvm::Value* scratch = function.Variable(emitters::GetVariableType<ValueType>(), "scratch");

            auto arguments = function.Arguments().begin();
            auto inputVolume = &(*arguments++);
            auto row = &(*arguments++);
            auto col = &(*arguments++);
            auto depth = &(*arguments++);
            auto width = &(*arguments++);
            auto height = &(*arguments++);
            auto padding = &(*arguments++);

            auto valueRow = function.Operator(minus, row, padding);
            auto valueCol = function.Operator(minus, col, padding);

            auto tooSmallRow = function.Comparison(lessThan, valueRow, function.Literal(0));
            auto tooSmallCol = function.Comparison(lessThan, valueCol, function.Literal(0));
            auto tooBigRow = function.Comparison(greaterThanOrEqual, valueRow, height);
            auto tooBigCol = function.Comparison(greaterThanOrEqual, valueCol, width);

            auto rowBad = function.Operator(emitters::TypedOperator::logicalOr, tooSmallRow, tooBigRow);
            auto colBad = function.Operator(emitters::TypedOperator::logicalOr, tooSmallCol, tooBigCol);
            auto outOfBounds = function.Operator(emitters::TypedOperator::logicalOr, rowBad, colBad);

            auto oobIfEmitter = function.If();
            oobIfEmitter.If(outOfBounds);
            {
                // Error: can't return from within an if/else block
                // function.Return(function.Literal(static_cast<ValueType>(0.0)));
                function.Store(scratch, function.Literal(static_cast<ValueType>(0.0)));
            }
            oobIfEmitter.Else();
            {
                auto index1 = function.Operator(times, depth, function.Operator(times, width, height));
                auto index2 = function.Operator(times, valueRow, width);
                auto index = function.Operator(plus, index1, function.Operator(plus, index2, valueCol));
                auto val = function.ValueAt(inputVolume, index);

                // Error: can't return from within an if/else block
                // function.Return(val);
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
                                              const PortMemoryLayout& inputLayout,
                                              const predictors::neural::BinaryConvolutionalParameters& convParams,
                                              size_t convPadding,
                                              llvm::Value* inputRow, llvm::Value* inputCol, llvm::Value* inputChannel)
        {
            const auto inputHeight = inputLayout.size[0];
            const auto inputWidth = inputLayout.size[1];
            const auto inputDepth = inputLayout.size[2];
            const auto inputPadding = inputLayout.offset[0];

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

        template <typename ValueType>
        void LoadRow(emitters::IRFunctionEmitter& function,
                     llvm::Value* inputVolume,
                     const PortMemoryLayout& inputLayout,
                     llvm::Value* outputRowIndex,
                     const PortMemoryLayout& outputLayout,
                     const predictors::neural::BinaryConvolutionalParameters& convParams,
                     llvm::Value* realValueRow) // realValueRow == output
        {
            const auto numChannels = inputLayout.size[2];
            const auto outputImageWidth = outputLayout.size[1];
            const auto outputImageHeight = outputLayout.size[0];
            const auto filterSize = convParams.receptiveField;
            const auto stride = convParams.stride;
            const auto convPadding = inputLayout.offset[0]; // TODO: decouple input data padding from convolvolution padding

            // compute offset based on outputRowIndex
            auto outputImageRow = function.Operator(divide, outputRowIndex, function.Literal<int>(outputImageWidth));
            auto outputImageCol = function.Operator(modulo, outputRowIndex, function.Literal<int>(outputImageWidth));
            auto inputRowStart = function.Operator(times, outputImageRow, function.Literal<int>(stride));
            auto inputColStart = function.Operator(times, outputImageCol, function.Literal<int>(stride));

            // For row, column, channel order:
            auto rowLoop = function.ForLoop();
            rowLoop.Begin(filterSize);
            {
                auto rowIndex = rowLoop.LoadIterationVariable();

                // TODO: maybe don't inline these if filterSize is big
                for (int colIndex = 0; colIndex < filterSize; ++colIndex)
                {
                    for (int channelIndex = 0; channelIndex < numChannels; ++channelIndex)
                    {
                        auto inputRow = function.Operator(plus, inputRowStart, rowIndex);
                        auto inputColumn = function.Operator(plus, inputColStart, function.Literal(colIndex));
                        auto inputChannel = function.Literal<int>(channelIndex);

                        auto value = GetValueFromPaddedVolume<ValueType>(function, inputVolume, inputLayout, convParams, convPadding, inputRow, inputColumn, inputChannel);

                        // h x w x d:  offset = iy*k*d + ix*d + iz
                        auto outputOffset = function.Operator(plus, function.Operator(times, rowIndex, function.Literal<int>(filterSize * numChannels)), function.Literal<int>(colIndex * numChannels + channelIndex));
                        function.SetValueAt(realValueRow, outputOffset, value);
                    }
                }
            }
            rowLoop.End();
        }

        template <typename ValueType, typename PackedBitsType>
        void CompressRow(emitters::IRFunctionEmitter& function, llvm::Value* realRow, llvm::Value* packedOutput, int numValues)
        {
            int storedElementSize = sizeof(PackedBitsType);
            int storedElementNumBits = 8 * storedElementSize;
            int numBlocks = (numValues - 1) / storedElementNumBits + 1;
            int numCompleteBlocks = numValues / storedElementNumBits;
            auto blockLoop = function.ForLoop();
            blockLoop.Begin(numCompleteBlocks);
            {
                auto blockIndex = blockLoop.LoadIterationVariable();

                // TODO: block-vectorize this:
                llvm::Value* blockValue = function.Literal<PackedBitsType>(0);
                for (int bitIndex = 0; bitIndex < storedElementNumBits; ++bitIndex)
                {
                    auto realValue = function.ValueAt(realRow, function.Operator(plus, function.Operator(times, blockIndex, function.Literal(storedElementNumBits)), function.Literal(bitIndex)));
                    auto cmp = function.Comparison(greaterThanFloat, realValue, function.Literal<ValueType>(0));
                    auto bitValue = function.Select(cmp, function.Literal<PackedBitsType>(1), function.Literal<PackedBitsType>(0));
                    // blockValue = blockValue | ((realValue>0?1:0) << bitIndex);
                    blockValue = function.Operator(logicalOr, blockValue, function.Operator(shiftLeft, bitValue, function.Literal<PackedBitsType>(bitIndex)));
                }
                function.SetValueAt(packedOutput, blockIndex, blockValue);
            }
            blockLoop.End();

            // now do the last, partial, block
            if (numBlocks > numCompleteBlocks)
            {
                assert(numBlocks == numCompleteBlocks + 1);
                int leftoverBits = numValues % storedElementNumBits;

                llvm::Value* blockValue = function.Literal<PackedBitsType>(0);
                for (int bitIndex = 0; bitIndex < leftoverBits; ++bitIndex)
                {
                    auto realValue = function.ValueAt(realRow, function.Literal(numCompleteBlocks * storedElementNumBits + bitIndex));
                    auto cmp = function.Comparison(greaterThanFloat, realValue, function.Literal<ValueType>(0));
                    auto bitValue = function.Select(cmp, function.Literal<PackedBitsType>(1), function.Literal<PackedBitsType>(0));
                    // blockValue = blockValue | ((realValue>0?1:0) << bitIndex);
                    blockValue = function.Operator(logicalOr, blockValue, function.Operator(shiftLeft, bitValue, function.Literal<PackedBitsType>(bitIndex)));
                }

                // blockValue = function.Operator(shiftLeft, blockValue, function.Literal<PackedBitsType>(storedElementNumBits - leftoverBits));
                function.SetValueAt(packedOutput, numCompleteBlocks, blockValue);
            }
        }

    } // end anonymous namespace

    //
    // BinaryConvolutionalLayerNode
    //
    template <typename ValueType>
    BinaryConvolutionalLayerNode<ValueType>::BinaryConvolutionalLayerNode(const model::PortElements<ValueType>& input, const predictors::neural::BinaryConvolutionalLayer<ValueType>& layer)
        : NeuralNetworkLayerNode<BinaryConvolutionalLayerNode<ValueType>, predictors::neural::BinaryConvolutionalLayer<ValueType>, ValueType>(input, layer)
    {
    }

    template <typename ValueType> // TODO: PackedBitsType
    std::vector<int64_t> BinaryConvolutionalLayerNode<ValueType>::GetCompressedFilterWeights() const
    {
        std::vector<int64_t> result;
        auto&& filterWeights = this->GetLayer().GetCompressedFilterWeights();
        for (auto&& w : filterWeights)
        {
            result.insert(result.end(), w.begin(), w.end());
        }
        return result;
    }

    template <typename ValueType>
    std::vector<ValueType> BinaryConvolutionalLayerNode<ValueType>::GetFilterMeans() const
    {
        return this->GetLayer().GetFilterMeans();
    }

    template <typename ValueType>
    bool BinaryConvolutionalLayerNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(this->input.GetPortElements());

        auto&& inputLayout = this->GetInputMemoryLayout();
        auto&& outputLayout = this->GetOutputMemoryLayout();
        auto&& convParams = this->GetLayer().GetConvolutionalParameters();
        const auto inputHeight = inputLayout.size[0];
        const auto inputWidth = inputLayout.size[1];
        const auto inputDepth = inputLayout.size[2];
        const auto filterWidth = convParams.receptiveField;
        const auto fieldVolumeSize = convParams.receptiveField * convParams.receptiveField * inputDepth;
        const auto outputImageHeight = outputLayout.size[0];
        const auto outputImageWidth = outputLayout.size[1];
        const auto numFilters = outputLayout.size[2];
        const auto padding = inputLayout.offset[0];
        const auto shapedInputSize = fieldVolumeSize * outputImageWidth * outputImageHeight;
        const auto outputRows = outputImageWidth * outputImageHeight;

        auto compressedFilterWeights = GetCompressedFilterWeights();
        auto filterMeans = GetFilterMeans();

        using PackedBitsType = int64_t;
        auto reshapeNode = transformer.AddNode<BinarizeAndReshapeImageNode<ValueType, PackedBitsType>>(newInput,
                                                                                                       convParams,
                                                                                                       inputLayout,
                                                                                                       outputLayout);
        auto filterWeightsNode = transformer.AddNode<ConstantNode<PackedBitsType>>(compressedFilterWeights);
        auto filterMeansNode = transformer.AddNode<ConstantNode<ValueType>>(filterMeans);
        auto xnorNode = transformer.AddNode<BinaryXnorNode<ValueType, PackedBitsType>>(reshapeNode->output,
                                                                                       filterWeightsNode->output,
                                                                                       filterMeansNode->output,
                                                                                       convParams,
                                                                                       inputLayout,
                                                                                       outputLayout);

        transformer.MapNodeOutput(this->output, xnorNode->output);
        return true;
    }

    //
    // BinarizeAndReshapeImageNode
    //

    template <typename ValueType, typename PackedBitsType>
    BinarizeAndReshapeImageNode<ValueType, PackedBitsType>::BinarizeAndReshapeImageNode()
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, inputPortName), _output(this, outputPortName, 0)
    {
    }

    template <typename ValueType, typename PackedBitsType>
    BinarizeAndReshapeImageNode<ValueType, PackedBitsType>::BinarizeAndReshapeImageNode(const model::PortElements<ValueType>& input,
                                                                                        const predictors::neural::BinaryConvolutionalParameters& convolutionalParameters,
                                                                                        const PortMemoryLayout& inputMemoryLayout,
                                                                                        const PortMemoryLayout& outputMemoryLayout)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, GetPackedFilterSize<PackedBitsType>(convolutionalParameters, inputMemoryLayout, outputMemoryLayout)), _convolutionalParameters(convolutionalParameters), _inputMemoryLayout(inputMemoryLayout), _outputMemoryLayout(outputMemoryLayout)
    {
    }

    template <typename ValueType, typename PackedBitsType>
    void BinarizeAndReshapeImageNode<ValueType, PackedBitsType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<BinarizeAndReshapeImageNode>(newPortElements, _convolutionalParameters, _inputMemoryLayout, _outputMemoryLayout);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType, typename PackedBitsType>
    void BinarizeAndReshapeImageNode<ValueType, PackedBitsType>::Compute() const
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
    }

    // TODO: Fix this to deal with convParams.stride != 1
    template <typename ValueType, typename PackedBitsType>
    void BinarizeAndReshapeImageNode<ValueType, PackedBitsType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        function.GetModule().DeclarePrintf();

        auto&& inputLayout = this->GetInputMemoryLayout();
        auto&& outputLayout = this->GetOutputMemoryLayout();

        llvm::Value* pInput = compiler.EnsurePortEmitted(this->input);
        llvm::Value* pOutput = compiler.EnsurePortEmitted(this->output);

        // The workspace buffer element sizes are dependent on the processor architecture's bitness
        auto elementSize = sizeof(PackedBitsType);
        auto numBits = 8 * elementSize;
        const auto inputDepth = _inputMemoryLayout.size[2];
        const auto filterWidth = _convolutionalParameters.receptiveField;
        const auto fieldVolumeSize = filterWidth * filterWidth * inputDepth;

        int packedRowSize = (fieldVolumeSize - 1) / numBits + 1;
        assert(packedRowSize != 0);
        int outputImageHeight = _outputMemoryLayout.size[0];
        int outputImageWidth = _outputMemoryLayout.size[1];

        // TODO: interleave load/compress more tightly to eliminate need for a scratch variable to hold the whole row
        // TODO: vectorize
        llvm::AllocaInst* realValueRow = function.Variable(emitters::GetVariableType<ValueType>(), fieldVolumeSize);
        auto outputRowLoop = function.ForLoop();
        outputRowLoop.Begin(outputImageWidth * outputImageHeight);
        {
            auto outputRowIndex = outputRowLoop.LoadIterationVariable();
            LoadRow<ValueType>(function,
                               pInput,
                               this->GetInputMemoryLayout(),
                               outputRowIndex,
                               this->GetOutputMemoryLayout(),
                               _convolutionalParameters,
                               realValueRow);

            auto outputRow = function.PointerOffset(pOutput, function.Operator(times, outputRowIndex, function.Literal<int>(packedRowSize)));
            CompressRow<ValueType, PackedBitsType>(function, realValueRow, outputRow, fieldVolumeSize);
        }
        outputRowLoop.End();
    }

    //
    // BinaryXnorNode
    //
    template <typename ValueType, typename PackedBitsType>
    BinaryXnorNode<ValueType, PackedBitsType>::BinaryXnorNode()
        : CompilableNode({ &_input, &_filterWeights, &_filterMeans }, { &_output }), _input(this, {}, inputPortName), _filterWeights(this, {}, filterWeightsPortName), _filterMeans(this, {}, filterMeansPortName), _output(this, outputPortName, 0)
    {
    }

    template <typename ValueType, typename PackedBitsType>
    BinaryXnorNode<ValueType, PackedBitsType>::BinaryXnorNode(const model::PortElements<PackedBitsType>& input,
                                                              const model::PortElements<PackedBitsType>& compressedFilterWeights,
                                                              const model::PortElements<ValueType>& filterMeans,
                                                              const predictors::neural::BinaryConvolutionalParameters& convolutionalParameters,
                                                              const PortMemoryLayout& inputMemoryLayout,
                                                              const PortMemoryLayout& outputMemoryLayout)
        : CompilableNode({ &_input, &_filterWeights, &_filterMeans }, { &_output }), _input(this, input, inputPortName), _filterWeights(this, compressedFilterWeights, filterWeightsPortName), _filterMeans(this, filterMeans, filterMeansPortName), _output(this, outputPortName, GetMemorySize(outputMemoryLayout)), _convolutionalParameters(convolutionalParameters), _inputMemoryLayout(inputMemoryLayout), _outputMemoryLayout(outputMemoryLayout)
    {
    }

    template <typename ValueType, typename PackedBitsType>
    void BinaryXnorNode<ValueType, PackedBitsType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(_input.GetPortElements());
        auto newFilterWeights = transformer.TransformPortElements(_filterWeights.GetPortElements());
        auto newFilterMeans = transformer.TransformPortElements(_filterMeans.GetPortElements());
        auto newNode = transformer.AddNode<BinaryXnorNode>(newInput, newFilterWeights, newFilterMeans, _convolutionalParameters, _inputMemoryLayout, _outputMemoryLayout);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType, typename PackedBitsType>
    void BinaryXnorNode<ValueType, PackedBitsType>::Compute() const
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
    }

    template <typename ValueType, typename PackedBitsType>
    void BinaryXnorNode<ValueType, PackedBitsType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        const auto packedBitsType = emitters::GetVariableType<PackedBitsType>();
        llvm::Function* popcountFunction = compiler.GetModule().GetIntrinsic(llvm::Intrinsic::ctpop, { packedBitsType });

        llvm::Value* pFilterWeights = compiler.EnsurePortEmitted(filterWeights);
        llvm::Value* pFilterMeans = compiler.EnsurePortEmitted(filterMeans);
        llvm::Value* pInput = compiler.EnsurePortEmitted(input);
        llvm::Value* pOutput = compiler.EnsurePortEmitted(output);

        // Input / output memory layouts
        const auto& inputLayout = this->GetInputMemoryLayout();
        const auto& inputSize = inputLayout.size;
        const auto& inputStride = inputLayout.stride;
        const auto& inputOffset = inputLayout.offset;

        const auto& outputLayout = this->GetOutputMemoryLayout();
        const auto& outputSize = outputLayout.size;
        const auto& outputStride = outputLayout.stride;
        const auto& outputOffset = outputLayout.offset;

        // Get cumulative increment for each dimension
        Shape inputIncrement = inputLayout.GetCumulativeIncrement();
        Shape outputIncrement = outputLayout.GetCumulativeIncrement();

        // The workspace buffer element sizes are dependent on the processor architecture's bitness
        const auto storedElementSize = sizeof(PackedBitsType);
        const auto storedElementNumBits = 8 * storedElementSize;
        const auto numBits = storedElementNumBits; // function.GetModule().GetCompilerParameters().numBits; // for Xnor, use 32 bits in 32-bit environment
        const auto elementSize = numBits / 8;
        assert(elementSize <= storedElementSize);
        const auto filterWidth = _convolutionalParameters.receptiveField;
        const auto numInputChannels = inputSize[2];
        const auto fieldVolumeSize = filterWidth * filterWidth * numInputChannels; // = size*size*numInputChannels
        const auto outputRows = outputSize[0];
        const auto outputColumns = outputSize[1];
        const auto numFilters = outputSize[2];
        const auto numStoredBlocksPerFilter = (fieldVolumeSize - 1) / storedElementNumBits + 1;
        const auto packedRowSize = numStoredBlocksPerFilter; // numStoredBlocksPerFilter * (storedElementSize / elementSize);
        assert(packedRowSize != 0);

        // Create constants for dimension names just to remove magic numbers from code below
        const int rowDimension = 0;
        const int columnDimension = 1;
        const int channelDimension = 2;

        const auto partialBlockSize = fieldVolumeSize % numBits;
        
        // Compute and accumulate xnor counts
        auto rowLoop = function.ForLoop();
        rowLoop.Begin(outputRows);
        {
            auto outputRowIndex = rowLoop.LoadIterationVariable();
            llvm::Value* rowOutputInternalOffset = function.Operator(plus, outputRowIndex, function.Literal<int>(outputOffset[rowDimension]));
            llvm::Value* rowOutputOffset = function.Operator(times, rowOutputInternalOffset, function.Literal<int>(outputIncrement[rowDimension]));

            auto columnLoop = function.ForLoop();
            columnLoop.Begin(outputColumns);
            {
                auto outputColumnIndex = columnLoop.LoadIterationVariable();
                llvm::Value* columnOutputInternalOffset = function.Operator(plus, outputColumnIndex, function.Literal<int>(outputOffset[columnDimension]));
                auto scaledColumnOutputOffset = function.Operator(times, columnOutputInternalOffset, function.Literal<int>(outputIncrement[columnDimension]));
                auto columnOutputOffset = function.Operator(plus, rowOutputOffset, scaledColumnOutputOffset);

                auto inputRow = function.Operator(plus, function.Operator(times, outputRowIndex, function.Literal<int>(outputColumns)), outputColumnIndex);
                auto inputBegin = function.Operator(times, inputRow, function.Literal<int>(numStoredBlocksPerFilter));
                auto channelLoop = function.ForLoop();
                channelLoop.Begin(numFilters); // filters are the output channels, so numFilters is the # of output channels 
                {
                    auto outputChannelIndex = channelLoop.LoadIterationVariable();
                    llvm::Value* channelOutputInternalOffset = function.Operator(plus, outputChannelIndex, function.Literal<int>(outputOffset[channelDimension]));
                    auto scaledChannelOutputOffset = function.Operator(times, channelOutputInternalOffset, function.Literal<int>(outputIncrement[channelDimension]));
                    auto channelOutputOffset = function.Operator(plus, columnOutputOffset, scaledChannelOutputOffset);

                    llvm::Value* filterMean = function.ValueAt(pFilterMeans, outputChannelIndex);
                    auto filterBegin = function.Operator(times, outputChannelIndex, function.Literal<int>(numStoredBlocksPerFilter));

                    llvm::Value* sumVar = function.Variable(packedBitsType, "accum");
                    function.Store(sumVar, function.Literal<PackedBitsType>(0));
                    llvm::Value* outputLocationOffset = channelOutputOffset;
                    
                    // TODO: vectorize this (?)
                    auto blockLoop = function.ForLoop();
                    blockLoop.Begin(packedRowSize);
                    {
                        auto blockIndex = blockLoop.LoadIterationVariable();
                        auto inputIndex = function.Operator(plus, inputBegin, blockIndex);
                        auto inputVal = function.ValueAt(pInput, inputIndex);

                        auto filterIndex = function.Operator(plus, filterBegin, blockIndex);
                        auto filterVal = function.ValueAt(pFilterWeights, filterIndex);
                        auto xorVal = function.Operator(emitters::TypedOperator::logicalXor, filterVal, inputVal);
                        auto count = function.Call(popcountFunction, { xorVal });
                        function.Store(sumVar, function.Operator(plus, count, function.Load(sumVar)));
                    }
                    blockLoop.End();
                    auto sumInt = function.CastValue<PackedBitsType, int>(function.Load(sumVar));
                    auto scaledSum = function.Operator(plus, function.Operator(times, function.Literal<int>(-2), sumInt), function.Literal<int>(numBits * packedRowSize));
                    auto sumFloat = function.CastValue<int, ValueType>(scaledSum);

                    llvm::Value* adjustedSum = sumFloat;
                    if (partialBlockSize != 0)
                    {
                        const auto filterAdjust = numBits - partialBlockSize;
                        adjustedSum = function.Operator(minusFloat, sumFloat, function.Literal<ValueType>(filterAdjust));
                    }
                    auto scaledOutput = function.Operator(timesFloat, adjustedSum, filterMean);
                    function.SetValueAt(pOutput, outputLocationOffset, scaledOutput);
                }
                channelLoop.End();
            }
            columnLoop.End();
        }
        rowLoop.End();
    }

    // Explicit specialization
    template class BinaryConvolutionalLayerNode<float>;
    template class BinaryConvolutionalLayerNode<double>;

} // nodes
} // ell
