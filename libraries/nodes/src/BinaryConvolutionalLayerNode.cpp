////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BinaryConvolutionalLayerNode.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BinaryConvolutionalLayerNode.h"
#include "ConstantNode.h"
#include "ReorderDataNode.h"

// emitters
#include "IRAsyncTask.h"
#include "IREmitter.h"
#include "IRThreadPool.h"
#include "IRVectorUtilities.h"
#include "LLVMUtilities.h"

#include "IRHeaderWriter.h"

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

        const auto minusFloat = emitters::TypedOperator::subtractFloat;
        const auto timesFloat = emitters::TypedOperator::multiplyFloat;

        const auto logicalOr = emitters::TypedOperator::logicalOr;
        const auto logicalAnd = emitters::TypedOperator::logicalAnd;
        const auto logicalXor = emitters::TypedOperator::logicalXor;
        const auto shiftLeft = emitters::TypedOperator::shiftLeft;

        // comparisons
        const auto lessThan = emitters::TypedComparison::lessThan;
        const auto greaterThanOrEqual = emitters::TypedComparison::greaterThanOrEquals;
        const auto greaterThanFloat = emitters::TypedComparison::greaterThanFloat;

        // convolution parameters
        const auto scaleOutputByFilterMeans = ell::predictors::neural::BinaryWeightsScale::mean;

        //
        // Functions
        //

        // computes ceil(a/b)
        int CeilDiv(int a, int b)
        {
            return (a - 1) / b + 1;
        }

        size_t GetFilterVolumeSize(const predictors::neural::BinaryConvolutionalParameters& convolutionalParameters, const model::PortMemoryLayout& inputMemoryLayout)
        {
            const auto inputDepth = inputMemoryLayout.GetActiveSize(2);
            const auto filterSize = convolutionalParameters.receptiveField;
            return inputDepth * filterSize * filterSize;
        }

        template <typename PackedBitsType>
        size_t GetPackedFilterSize(const predictors::neural::BinaryConvolutionalParameters& convolutionalParameters, const model::PortMemoryLayout& inputMemoryLayout, const model::PortMemoryLayout& outputMemoryLayout)
        {
            const size_t numOutputPixels = outputMemoryLayout.GetActiveSize(0) * outputMemoryLayout.GetActiveSize(1);
            const size_t filterVolumeSize = GetFilterVolumeSize(convolutionalParameters, inputMemoryLayout);
            return ((filterVolumeSize - 1) / (8 * sizeof(PackedBitsType)) + 1) * numOutputPixels;
        }

        llvm::Value* GetValueFromVolume(emitters::IRFunctionEmitter& function,
                                        llvm::Value* inputVolume,
                                        const model::PortMemoryLayout& inputLayout,
                                        const predictors::neural::BinaryConvolutionalParameters& convParams,
                                        llvm::Value* valueRow, llvm::Value* valueColumn, llvm::Value* valueChannel)
        {
            const auto columnStride = inputLayout.GetStride(1);
            const auto channelStride = inputLayout.GetStride(2);

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

            emitters::IRFunctionEmitter function = moduleEmitter.BeginFunction(functionName, valueType, { valuePtrType, int32Type, int32Type, int32Type, int32Type, int32Type, int32Type });
            llvm::Value* returnValue = function.Variable(emitters::GetVariableType<ValueType>(), "returnValue");

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

            auto rowBad = function.Operator(logicalOr, tooSmallRow, tooBigRow);
            auto colBad = function.Operator(logicalOr, tooSmallCol, tooBigCol);
            auto outOfBounds = function.Operator(logicalOr, rowBad, colBad);

            auto oobIfEmitter = function.If();
            oobIfEmitter.If(outOfBounds);
            {
                function.StoreZero(returnValue);
            }
            oobIfEmitter.Else();
            {
                auto index1 = function.Operator(times, depth, function.Operator(times, width, height));
                auto index2 = function.Operator(times, valueRow, width);
                auto index = function.Operator(plus, index1, function.Operator(plus, index2, valueCol));
                auto val = function.ValueAt(inputVolume, index);

                function.Store(returnValue, val);
            }
            oobIfEmitter.End();

            function.Return(function.Load(returnValue));
            moduleEmitter.EndFunction();
            return function.GetFunction();
        }

        template <typename ValueType>
        llvm::Value* GetValueFromPaddedVolume(emitters::IRFunctionEmitter& function,
                                              llvm::Value* inputVolume,
                                              const model::PortMemoryLayout& inputLayout,
                                              const predictors::neural::BinaryConvolutionalParameters& convParams,
                                              size_t convPadding,
                                              llvm::Value* inputRow, llvm::Value* inputCol, llvm::Value* inputChannel)
        {
            const auto inputHeight = inputLayout.GetActiveSize(0);
            const auto inputWidth = inputLayout.GetActiveSize(1);
            const auto inputDepth = inputLayout.GetActiveSize(2);
            const auto inputPadding = inputLayout.GetOffset(0);

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
                     const model::PortMemoryLayout& inputLayout,
                     llvm::Value* outputRowIndex,
                     const model::PortMemoryLayout& outputLayout,
                     const predictors::neural::BinaryConvolutionalParameters& convParams,
                     llvm::Value* realValueRow) // realValueRow == output
        {
            const auto numChannels = inputLayout.GetActiveSize(2);
            const auto outputImageWidth = outputLayout.GetActiveSize(1);
            const auto filterSize = convParams.receptiveField;
            const auto stride = convParams.stride;
            const auto convPadding = inputLayout.GetOffset(0); // TODO: decouple input data padding from convolution padding

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

                auto colLoop = function.ForLoop();
                colLoop.Begin(filterSize);
                {
                    auto colIndex = colLoop.LoadIterationVariable();

                    auto channelLoop = function.ForLoop();
                    channelLoop.Begin(numChannels);
                    {
                        auto channelIndex = channelLoop.LoadIterationVariable();
                        auto inputRow = function.Operator(plus, inputRowStart, rowIndex);
                        auto inputColumn = function.Operator(plus, inputColStart, colIndex);
                        auto inputChannel = channelIndex;

                        auto value = GetValueFromPaddedVolume<ValueType>(function, inputVolume, inputLayout, convParams, convPadding, inputRow, inputColumn, inputChannel);

                        // The input is a filterSize x filterSize x numChannels image in in row x column x channel order
                        //   so offset = (filterSize*numChannels)*row + numChannels*column + channel
                        auto rowOffset = function.Operator(times, rowIndex, function.Literal<int>(filterSize * numChannels));
                        auto colOffset = function.Operator(times, colIndex, function.Literal<int>(numChannels));
                        auto channelBeginOffset = function.Operator(plus, rowOffset, colOffset);
                        auto outputOffset = function.Operator(plus, channelBeginOffset, channelIndex);
                        function.SetValueAt(realValueRow, outputOffset, value);
                    }
                    channelLoop.End();
                }
                colLoop.End();
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

        void PushPackedBits(std::vector<uint64_t>& vec, const std::vector<uint64_t>& bits)
        {
            vec.insert(vec.end(), bits.begin(), bits.end());
        }

        void PushPackedBits(std::vector<int64_t>& vec, const std::vector<uint64_t>& bits)
        {
            vec.insert(vec.end(), bits.begin(), bits.end());
        }

        void PushPackedBits(std::vector<uint32_t>& vec, const std::vector<uint64_t>& bits)
        {
            // Push each half separately
            for (auto b : bits)
            {
                vec.push_back(static_cast<uint32_t>(b & 0xffffffff));
                vec.push_back(static_cast<uint32_t>((b >> 32) & 0xffffffff));
            }
        }

        void PushPackedBits(std::vector<int32_t>& vec, const std::vector<uint64_t>& bits)
        {
            // Push each half separately
            for (auto b : bits)
            {
                vec.push_back(static_cast<uint32_t>(b & 0xffffffff));
                vec.push_back(static_cast<uint32_t>((b >> 32) & 0xffffffff));
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

    template <typename ValueType>
    template <typename PackedBitsType>
    std::vector<PackedBitsType> BinaryConvolutionalLayerNode<ValueType>::GetCompressedFilterWeights() const
    {
        std::vector<PackedBitsType> result;
        auto&& filterWeights = this->GetLayer().GetCompressedFilterWeights(); // a vector of vectors of uint64_t
        static_assert(sizeof(PackedBitsType) <= sizeof(filterWeights[0][0]), "PackedBitsType must not be larger than 64 bits");

        // Really, we should just use a buffer of bytes
        for (auto&& w : filterWeights)
        {
            PushPackedBits(result, w);
        }
        return result;
    }

    template <typename ValueType>
    std::vector<ValueType> BinaryConvolutionalLayerNode<ValueType>::GetFilterMeans() const
    {
        return this->GetLayer().GetFilterMeans();
    }

    template <typename ValueType>
    template <typename PackedBitsType>
    std::vector<PackedBitsType> BinaryConvolutionalLayerNode<ValueType>::GetCompressedInputPaddingMask() const
    {
        std::vector<PackedBitsType> result;
        auto&& masks = this->GetLayer().GetCompressedInputPaddingMasks();
        static_assert(sizeof(PackedBitsType) <= sizeof(masks[0][0]), "PackedBitsType must not be larger than 64 bits");
        for (auto&& m : masks)
        {
            PushPackedBits(result, m);
        }
        return result;
    }

    template <typename ValueType>
    std::vector<int> BinaryConvolutionalLayerNode<ValueType>::GetInputPaddingMaskSums() const
    {
        return this->GetLayer().GetInputPaddingMaskSums();
    }

    template <typename ValueType>
    bool BinaryConvolutionalLayerNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        auto compiler = dynamic_cast<const model::IRMapCompiler*>(transformer.GetContext().GetCompiler());
        auto numPackedBits = compiler != nullptr ? compiler->GetCompilerParameters().targetDevice.numBits : 64;
        if (numPackedBits == 0)
        {
            numPackedBits = 64;
        }

        auto newInput = transformer.TransformPortElements(this->input.GetPortElements());

        auto&& outputLayout = this->GetOutputMemoryLayout();
        const auto outputImageHeight = outputLayout.GetActiveSize(0);
        const auto outputImageWidth = outputLayout.GetActiveSize(1);
        const auto numFilters = outputLayout.GetActiveSize(2);
        const auto outputDataPadding = outputLayout.GetOffset(0);

        assert(outputDataPadding == 0 && "Convolutional node output padding not supported yet");

        model::PortElements<ValueType> xnorOutput;
        if (numPackedBits == 32)
        {
            xnorOutput = AddRefinedNodes<int32_t>(transformer, newInput);
        }
        else
        {
            xnorOutput = AddRefinedNodes<int64_t>(transformer, newInput);
        }

        // Output of xnor is in (f x h x w) order, need to transpose to (h x w x f)
        model::PortMemoryLayout outputShape({ numFilters, outputImageHeight, outputImageWidth });
        model::PortMemoryLayout transposedOutputShape({ outputImageHeight, outputImageWidth, numFilters }, { outputDataPadding, outputDataPadding, 0 });
        auto reorderOutputNode = transformer.AddNode<ReorderDataNode<ValueType>>(xnorOutput, outputShape, transposedOutputShape, std::vector<int>{ 1, 2, 0 });
        transformer.MapNodeOutput(this->output, reorderOutputNode->output);
        return true;
    }

    template <typename ValueType>
    template <typename PackedBitsType>
    model::PortElements<ValueType> BinaryConvolutionalLayerNode<ValueType>::AddRefinedNodes(model::ModelTransformer& transformer, const model::PortElements<ValueType>& input) const
    {
        auto&& inputLayout = this->GetInputMemoryLayout();
        auto&& outputLayout = this->GetOutputMemoryLayout();
        auto&& convParams = this->GetLayer().GetConvolutionalParameters();
        auto&& layerParams = this->GetLayer().GetLayerParameters();

        auto compressedFilterWeights = GetCompressedFilterWeights<PackedBitsType>();
        auto compressedPaddingMasks = GetCompressedInputPaddingMask<PackedBitsType>();
        auto paddingMaskSums = GetInputPaddingMaskSums();
        auto filterMeans = GetFilterMeans();

        auto reshapeNode = transformer.AddNode<BinaryReceptiveFieldMatrixNode<ValueType, PackedBitsType>>(input,
                                                                                                          convParams,
                                                                                                          inputLayout,
                                                                                                          outputLayout);
        auto paddingMasksNode = transformer.AddNode<ConstantNode<PackedBitsType>>(compressedPaddingMasks);
        auto paddingMaskSumsNode = transformer.AddNode<ConstantNode<int>>(paddingMaskSums);
        auto filterWeightsNode = transformer.AddNode<ConstantNode<PackedBitsType>>(compressedFilterWeights);
        auto filterMeansNode = transformer.AddNode<ConstantNode<ValueType>>(filterMeans);
        auto xnorNode = transformer.AddNode<BinaryXnorNode<ValueType, PackedBitsType>>(reshapeNode->output,
                                                                                       paddingMasksNode->output,
                                                                                       paddingMaskSumsNode->output,
                                                                                       filterWeightsNode->output,
                                                                                       filterMeansNode->output,
                                                                                       convParams,
                                                                                       layerParams.inputPaddingParameters,
                                                                                       inputLayout,
                                                                                       outputLayout);

        return { xnorNode->output };
    }

    //
    // BinaryReceptiveFieldMatrixNode
    //

    template <typename ValueType, typename PackedBitsType>
    BinaryReceptiveFieldMatrixNode<ValueType, PackedBitsType>::BinaryReceptiveFieldMatrixNode()
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, inputPortName), _output(this, outputPortName, 0)
    {
    }

    template <typename ValueType, typename PackedBitsType>
    BinaryReceptiveFieldMatrixNode<ValueType, PackedBitsType>::BinaryReceptiveFieldMatrixNode(const model::PortElements<ValueType>& input,
                                                                                              const predictors::neural::BinaryConvolutionalParameters& convolutionalParameters,
                                                                                              const model::PortMemoryLayout& inputMemoryLayout,
                                                                                              const model::PortMemoryLayout& outputMemoryLayout)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, GetPackedFilterSize<PackedBitsType>(convolutionalParameters, inputMemoryLayout, outputMemoryLayout)), _convolutionalParameters(convolutionalParameters), _inputMemoryLayout(inputMemoryLayout), _outputMemoryLayout(outputMemoryLayout)
    {
    }

    template <typename ValueType, typename PackedBitsType>
    void BinaryReceptiveFieldMatrixNode<ValueType, PackedBitsType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<BinaryReceptiveFieldMatrixNode>(newPortElements, _convolutionalParameters, _inputMemoryLayout, _outputMemoryLayout);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType, typename PackedBitsType>
    void BinaryReceptiveFieldMatrixNode<ValueType, PackedBitsType>::Compute() const
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
    }

    // TODO: make new function that emits the contents of the task function (which is also the stuff emitted in the serial case), and call this from
    // GetTaskFunction as well as the serial part. Removes code duplication and makes it so that we don't accidentally use 'function' inside definition of 'taskFunction'
    template <typename ValueType, typename PackedBitsType>
    emitters::IRFunctionEmitter BinaryReceptiveFieldMatrixNode<ValueType, PackedBitsType>::GetTaskFunction(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        // Get port variables
        llvm::Value* pInputTemp = compiler.EnsurePortEmitted(input);
        llvm::Value* pOutputTemp = compiler.EnsurePortEmitted(output);

        // Get LLVM types
        auto& module = function.GetModule();
        auto& context = module.GetLLVMContext();
        auto voidType = llvm::Type::getVoidTy(context);

        // Constants
        auto elementSize = sizeof(PackedBitsType);
        auto numBits = 8 * elementSize;
        const auto inputDepth = _inputMemoryLayout.GetActiveSize(2);
        const auto filterWidth = _convolutionalParameters.receptiveField;
        const auto fieldVolumeSize = filterWidth * filterWidth * inputDepth;

        int packedRowSize = (fieldVolumeSize - 1) / numBits + 1;
        assert(packedRowSize != 0);

        auto argTypes = emitters::GetLLVMTypes({ pInputTemp, pOutputTemp, function.Literal<int32_t>(0), function.Literal<int32_t>(0) });
        emitters::IRFunctionEmitter taskFunction = function.GetModule().BeginFunction(utilities::to_string(GetId()) + "_task", voidType, argTypes);
        {
            auto arguments = taskFunction.Arguments().begin();
            auto pInput = &(*arguments++);
            auto pOutput = &(*arguments++);
            auto begin = &(*arguments++);
            auto end = &(*arguments++);

            // TODO: interleave load/compress more tightly to eliminate need for a scratch variable to hold a whole row
            llvm::AllocaInst* realValueRow = taskFunction.Variable(emitters::GetVariableType<ValueType>(), fieldVolumeSize);
            auto outputRowLoop = taskFunction.ForLoop();
            outputRowLoop.Begin(begin, end, taskFunction.Literal<int>(1));
            {
                auto outputRowIndex = outputRowLoop.LoadIterationVariable();
                LoadRow<ValueType>(taskFunction,
                                   pInput,
                                   this->GetInputMemoryLayout(),
                                   outputRowIndex,
                                   this->GetOutputMemoryLayout(),
                                   _convolutionalParameters,
                                   realValueRow);

                auto outputRow = taskFunction.PointerOffset(pOutput, taskFunction.Operator(times, outputRowIndex, taskFunction.Literal<int>(packedRowSize)));
                CompressRow<ValueType, PackedBitsType>(taskFunction, realValueRow, outputRow, fieldVolumeSize);
            }
            outputRowLoop.End();
            taskFunction.Return();
        }
        function.GetModule().EndFunction();

        return taskFunction;
    }

    // TODO: Fix this to deal with convParams.stride != 1
    template <typename ValueType, typename PackedBitsType>
    void BinaryReceptiveFieldMatrixNode<ValueType, PackedBitsType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        // Get port variables
        llvm::Value* pInput = compiler.EnsurePortEmitted(this->input);
        llvm::Value* pOutput = compiler.EnsurePortEmitted(this->output);

        const auto& compilerSettings = compiler.GetCompilerParameters();

        // The workspace buffer element sizes are dependent on the processor architecture's bitness
        auto elementSize = sizeof(PackedBitsType);
        auto numBits = 8 * elementSize;
        const auto inputDepth = _inputMemoryLayout.GetActiveSize(2);
        const auto filterWidth = _convolutionalParameters.receptiveField;
        const auto fieldVolumeSize = filterWidth * filterWidth * inputDepth;

        const auto packedRowSize = (fieldVolumeSize - 1) / numBits + 1;
        assert(packedRowSize != 0);
        const auto outputImageHeight = _outputMemoryLayout.GetActiveSize(0);
        const auto outputImageWidth = _outputMemoryLayout.GetActiveSize(1);
        const auto numOutputRows = outputImageWidth * outputImageHeight;

        const auto numDesiredTasks = compilerSettings.maxThreads;
        const int taskSize = CeilDiv(numOutputRows, numDesiredTasks);
        const int numTasks = CeilDiv(numOutputRows, taskSize);
        if (compilerSettings.parallelize && numTasks > 1)
        {
            auto taskFunction = GetTaskFunction(compiler, function);
            std::vector<std::vector<llvm::Value*>> taskArgs;
            for (int taskIndex = 0; taskIndex < numTasks; ++taskIndex)
            {
                auto start = taskIndex * taskSize;
                auto end = std::min((taskIndex + 1) * taskSize, numOutputRows);
                taskArgs.push_back({ pInput, pOutput, function.Literal<int32_t>(start), function.Literal<int32_t>(end) });
            }
            auto tasks = function.StartTasks(taskFunction, taskArgs);
            tasks.WaitAll(function);
        }
        else
        {
            // TODO: interleave load/compress more tightly to eliminate need for a scratch variable to hold the whole row
            llvm::AllocaInst* realValueRow = function.Variable(emitters::GetVariableType<ValueType>(), fieldVolumeSize);
            auto outputRowLoop = function.ForLoop();
            outputRowLoop.Begin(numOutputRows);
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
    }

    template <typename ValueType, typename PackedBitsType>
    void BinaryReceptiveFieldMatrixNode<ValueType, PackedBitsType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
    }

    template <typename ValueType, typename PackedBitsType>
    void BinaryReceptiveFieldMatrixNode<ValueType, PackedBitsType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
    }

    //
    // BinaryXnorNode
    //
    template <typename ValueType, typename PackedBitsType>
    BinaryXnorNode<ValueType, PackedBitsType>::BinaryXnorNode()
        : CompilableNode({ &_input, &_inputPaddingMasks, &_inputPaddingMaskSums, &_filterWeights, &_filterMeans }, { &_output }), _input(this, {}, inputPortName), _inputPaddingMasks(this, {}, inputPaddingMasksPortName), _inputPaddingMaskSums(this, {}, inputPaddingMaskSumsPortName), _filterWeights(this, {}, filterWeightsPortName), _filterMeans(this, {}, filterMeansPortName), _output(this, outputPortName, 0)
    {
    }

    template <typename ValueType, typename PackedBitsType>
    BinaryXnorNode<ValueType, PackedBitsType>::BinaryXnorNode(const model::PortElements<PackedBitsType>& input,
                                                              const model::PortElements<PackedBitsType>& compressedInputPaddingMasks,
                                                              const model::PortElements<int>& inputPaddingMaskSums,
                                                              const model::PortElements<PackedBitsType>& compressedFilterWeights,
                                                              const model::PortElements<ValueType>& filterMeans,
                                                              const predictors::neural::BinaryConvolutionalParameters& convolutionalParameters,
                                                              const predictors::neural::PaddingParameters& inputPaddingParameters,
                                                              const model::PortMemoryLayout& inputMemoryLayout,
                                                              const model::PortMemoryLayout& outputMemoryLayout)
        : CompilableNode({ &_input, &_inputPaddingMasks, &_inputPaddingMaskSums, &_filterWeights, &_filterMeans }, { &_output }), _input(this, input, inputPortName), _inputPaddingMasks(this, compressedInputPaddingMasks, inputPaddingMasksPortName), _inputPaddingMaskSums(this, inputPaddingMaskSums, inputPaddingMaskSumsPortName), _filterWeights(this, compressedFilterWeights, filterWeightsPortName), _filterMeans(this, filterMeans, filterMeansPortName), _output(this, outputPortName, outputMemoryLayout.GetMemorySize()), _convolutionalParameters(convolutionalParameters), _inputPaddingParameters(inputPaddingParameters), _inputMemoryLayout(inputMemoryLayout), _outputMemoryLayout(outputMemoryLayout)
    {
    }

    template <typename ValueType, typename PackedBitsType>
    void BinaryXnorNode<ValueType, PackedBitsType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(_input.GetPortElements());
        auto newInputPaddingMasks = transformer.TransformPortElements(_inputPaddingMasks.GetPortElements());
        auto newInputPaddingMaskSums = transformer.TransformPortElements(_inputPaddingMaskSums.GetPortElements());
        auto newFilterWeights = transformer.TransformPortElements(_filterWeights.GetPortElements());
        auto newFilterMeans = transformer.TransformPortElements(_filterMeans.GetPortElements());
        auto newNode = transformer.AddNode<BinaryXnorNode>(newInput, newInputPaddingMasks, newInputPaddingMaskSums, newFilterWeights, newFilterMeans, _convolutionalParameters, _inputPaddingParameters, _inputMemoryLayout, _outputMemoryLayout);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType, typename PackedBitsType>
    void BinaryXnorNode<ValueType, PackedBitsType>::Compute() const
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
    }

    template <typename ValueType, typename PackedBitsType>
    void BinaryXnorNode<ValueType, PackedBitsType>::EmitInnerLoop(emitters::IRFunctionEmitter& function,
                                                                  llvm::Value* reshapedInput,
                                                                  llvm::Value* paddingMask,
                                                                  llvm::Value* weights,
                                                                  llvm::Value* xorSumVariable,
                                                                  llvm::Function* popCountFunction,
                                                                  int startBlock,
                                                                  int numBlocks,
                                                                  bool hasZeroPadding)
    {
        auto blockLoop = function.ForLoop();
        blockLoop.Begin(startBlock, startBlock + numBlocks, 1);
        {
            auto blockIndex = blockLoop.LoadIterationVariable();
            auto inputVal = function.ValueAt(reshapedInput, blockIndex);
            auto filterVal = function.ValueAt(weights, blockIndex);
            auto xorVal = function.Operator(logicalXor, filterVal, inputVal);

            if (hasZeroPadding)
            {
                // Mask out the bits associated with zero padding from the XOR value
                auto paddingMaskVal = function.ValueAt(paddingMask, blockIndex);
                xorVal = function.Operator(logicalAnd, paddingMaskVal, xorVal);
            }

            auto xorCount = function.Call(popCountFunction, { xorVal });
            function.OperationAndUpdate(xorSumVariable, plus, xorCount);
        }
        blockLoop.End();
    }

    template <typename ValueType, typename PackedBitsType>
    void BinaryXnorNode<ValueType, PackedBitsType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        // Get compiler settings
        const auto& compilerSettings = compiler.GetCompilerParameters();
        const int vectorSize = compilerSettings.vectorWidth;

        // Get port variables
        llvm::Value* pInput = compiler.EnsurePortEmitted(input);
        llvm::Value* pFilterWeights = compiler.EnsurePortEmitted(filterWeights);
        llvm::Value* pFilterMeans = compiler.EnsurePortEmitted(filterMeans);
        llvm::Value* pInputPaddingMask = compiler.EnsurePortEmitted(inputPaddingMasks);
        llvm::Value* pInputPaddingMaskSums = compiler.EnsurePortEmitted(inputPaddingMaskSums);
        llvm::Value* pOutput = compiler.EnsurePortEmitted(output);

        // Input / output memory layouts (of the original node)
        const auto& inputLayout = this->GetInputMemoryLayout();
        const auto& inputSize = inputLayout.GetActiveSize();

        const auto& outputLayout = this->GetOutputMemoryLayout();
        // const auto& outputLayout = this->GetOutputMemoryLayout().Reorder({2,0,1}); // TODO: reorder from r,c,d -> d,r,c
        const auto& outputSize = outputLayout.GetActiveSize();

        // The workspace buffer element sizes are dependent on the processor architecture's bitness
        const auto storedElementSize = sizeof(PackedBitsType);
        const auto storedElementNumBits = 8 * storedElementSize;
        const auto numBits = storedElementNumBits; // function.GetModule().GetCompilerParameters().numBits; // for Xnor, use 32 bits in 32-bit environment
        const auto elementSize = numBits / 8;
        assert(elementSize <= storedElementSize);
        const auto filterWidth = _convolutionalParameters.receptiveField;
        const auto numInputChannels = inputSize[2]; // inputSize is the dimensions of the input to the original layer node
        const auto fieldVolumeSize = filterWidth * filterWidth * numInputChannels; // = size*size*numInputChannels

        // TODO: Put this back once we're using the transposed output layout
        // const auto numFilters = outputSize[0]; // == # output rows
        // const auto outputColumns = outputSize[1] * outputSize[2];
        const auto numFilters = outputSize[2]; // == # output rows
        const auto outputColumns = outputSize[0] * outputSize[1];
        const auto numStoredBlocksPerFilter = (fieldVolumeSize - 1) / storedElementNumBits + 1;
        const auto packedRowSize = numStoredBlocksPerFilter; // numStoredBlocksPerFilter * (storedElementSize / elementSize);
        assert(packedRowSize != 0);

        // Need to compute the stride between rows of the filters and input image, if they've been compressed with a different stride
        const int rowStrideBits = 64;
        const int rowStrideElementSize = rowStrideBits / 8;
        const auto numStrideBlocks = (fieldVolumeSize - 1) / rowStrideBits + 1;
        const int packedRowStride = numStrideBlocks * (rowStrideElementSize / elementSize);
        const bool hasZeroPadding = predictors::neural::HasPadding(_inputPaddingParameters, predictors::neural::PaddingScheme::zeros);

        bool useVectorInstructions = compilerSettings.allowVectorInstructions;
        const int numVectorBlocks = useVectorInstructions ? packedRowSize / vectorSize : 0;
        if (numVectorBlocks == 0)
        {
            useVectorInstructions = false;
        }

        const int numDesiredTasks = compilerSettings.maxThreads;
        const int taskSize = CeilDiv(numFilters, numDesiredTasks);
        const int numTasks = CeilDiv(numFilters, taskSize);
        if (compilerSettings.parallelize && numTasks > 1)
        {
            auto taskFunction = GetTaskFunction(compiler, function);
            std::vector<std::vector<llvm::Value*>> taskArgs;
            for (int taskIndex = 0; taskIndex < numTasks; ++taskIndex)
            {
                auto start = taskIndex * taskSize;
                auto end = std::min((taskIndex + 1) * taskSize, numFilters);
                std::vector<llvm::Value*> args = { pInput, pFilterWeights, pFilterMeans, pInputPaddingMask, pInputPaddingMaskSums, pOutput, function.Literal<int32_t>(start), function.Literal<int32_t>(end) };
                taskArgs.push_back(args);
            }
            auto tasks = function.StartTasks(taskFunction, taskArgs);
            tasks.WaitAll(function);
        }
        else // single-threaded
        {
            auto filterLoop = function.ForLoop();
            filterLoop.Begin(numFilters);
            {
                auto filterIndex = filterLoop.LoadIterationVariable();

                ComputeFilterOutput(compiler,
                                    function,
                                    pInput,
                                    pFilterWeights,
                                    pFilterMeans,
                                    pInputPaddingMask,
                                    pInputPaddingMaskSums,
                                    pOutput,
                                    filterIndex,
                                    hasZeroPadding,
                                    outputColumns,
                                    packedRowSize,
                                    packedRowStride,
                                    useVectorInstructions,
                                    vectorSize,
                                    numVectorBlocks);
            }
            filterLoop.End();
        }
    }

    template <typename ValueType, typename PackedBitsType>
    emitters::IRFunctionEmitter BinaryXnorNode<ValueType, PackedBitsType>::GetTaskFunction(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        // Get port variables
        llvm::Value* pInput = compiler.EnsurePortEmitted(input);
        llvm::Value* pFilterWeights = compiler.EnsurePortEmitted(filterWeights);
        llvm::Value* pFilterMeans = compiler.EnsurePortEmitted(filterMeans);
        llvm::Value* pInputPaddingMask = compiler.EnsurePortEmitted(inputPaddingMasks);
        llvm::Value* pInputPaddingMaskSums = compiler.EnsurePortEmitted(inputPaddingMaskSums);
        llvm::Value* pOutput = compiler.EnsurePortEmitted(output);

        const auto& compilerSettings = compiler.GetCompilerParameters();

        // Get LLVM types
        auto& module = function.GetModule();
        auto& context = module.GetLLVMContext();
        auto voidType = llvm::Type::getVoidTy(context);

        // Constants
        // Input / output memory layouts (of the original node)
        const auto& inputLayout = this->GetInputMemoryLayout();
        const auto& inputSize = inputLayout.GetActiveSize();

        const auto& outputLayout = this->GetOutputMemoryLayout();
        // const auto& outputLayout = this->GetOutputMemoryLayout().Reorder({2,0,1}); // TODO: reorder from r,c,d -> d,r,c
        const auto& outputSize = outputLayout.GetActiveSize();

        // The workspace buffer element sizes are dependent on the processor architecture's bitness
        const auto storedElementSize = sizeof(PackedBitsType);
        const auto storedElementNumBits = 8 * storedElementSize;
        const auto numBits = storedElementNumBits; // function.GetModule().GetCompilerParameters().numBits; // for Xnor, use 32 bits in 32-bit environment
        const auto elementSize = numBits / 8;
        assert(elementSize <= storedElementSize);
        const auto filterWidth = _convolutionalParameters.receptiveField;
        const auto numInputChannels = inputSize[2]; // inputSize is the dimensions of the input to the original layer node
        const auto fieldVolumeSize = filterWidth * filterWidth * numInputChannels; // = size*size*numInputChannels

        // TODO: Put this back once we're using the transposed output layout
        // const auto outputColumns = outputSize[1] * outputSize[2];
        const auto outputColumns = outputSize[0] * outputSize[1];
        const auto numStoredBlocksPerFilter = (fieldVolumeSize - 1) / storedElementNumBits + 1;
        const auto packedRowSize = numStoredBlocksPerFilter; // numStoredBlocksPerFilter * (storedElementSize / elementSize);
        assert(packedRowSize != 0);

        // Need to compute the stride between rows of the filters and input image, if they've been compressed with a different stride
        const int rowStrideBits = 64;
        const int rowStrideElementSize = rowStrideBits / 8;
        const auto numStrideBlocks = (fieldVolumeSize - 1) / rowStrideBits + 1;
        const int packedRowStride = numStrideBlocks * (rowStrideElementSize / elementSize);
        const bool hasZeroPadding = predictors::neural::HasPadding(_inputPaddingParameters, predictors::neural::PaddingScheme::zeros);

        bool useVectorInstructions = compilerSettings.allowVectorInstructions;
        const int vectorSize = compilerSettings.vectorWidth;
        const int numVectorBlocks = useVectorInstructions ? packedRowSize / vectorSize : 0;
        if (numVectorBlocks == 0)
        {
            useVectorInstructions = false;
        }

        // TODO: get types in a way that doesn't require emitting these variables
        auto argTypes = emitters::GetLLVMTypes({ pInput, pFilterWeights, pFilterMeans, pInputPaddingMask, pInputPaddingMaskSums, pOutput, function.Literal<int32_t>(0), function.Literal<int32_t>(0) });
        emitters::IRFunctionEmitter taskFunction = function.GetModule().BeginFunction(utilities::to_string(GetId()) + "_task", voidType, argTypes);
        {
            auto arguments = taskFunction.Arguments().begin();
            auto pInput = &(*arguments++);
            auto pFilterWeights = &(*arguments++);
            auto pFilterMeans = &(*arguments++);
            auto pInputPaddingMask = &(*arguments++);
            auto pInputPaddingMaskSums = &(*arguments++);
            auto pOutput = &(*arguments++);
            auto blockStartVal = &(*arguments++);
            auto blockEndVal = &(*arguments++);

            auto filterLoop = taskFunction.ForLoop();
            filterLoop.Begin(blockStartVal, blockEndVal, taskFunction.Literal<int>(1));
            {
                auto filterIndex = filterLoop.LoadIterationVariable();
                ComputeFilterOutput(compiler,
                                    taskFunction,
                                    pInput,
                                    pFilterWeights,
                                    pFilterMeans,
                                    pInputPaddingMask,
                                    pInputPaddingMaskSums,
                                    pOutput,
                                    filterIndex,
                                    hasZeroPadding,
                                    outputColumns,
                                    packedRowSize,
                                    packedRowStride,
                                    useVectorInstructions,
                                    vectorSize,
                                    numVectorBlocks);
            }
            filterLoop.End();

            taskFunction.Return();
        }
        function.GetModule().EndFunction();
        return taskFunction;
    }

    template <typename ValueType, typename PackedBitsType>
    void BinaryXnorNode<ValueType, PackedBitsType>::ComputeFilterOutput(model::IRMapCompiler& compiler,
                                                                        emitters::IRFunctionEmitter& function,
                                                                        llvm::Value* pInput,
                                                                        llvm::Value* pFilterWeights,
                                                                        llvm::Value* pFilterMeans,
                                                                        llvm::Value* pInputPaddingMask,
                                                                        llvm::Value* pInputPaddingMaskSums,
                                                                        llvm::Value* pOutput,
                                                                        llvm::Value* filterIndex,
                                                                        bool hasZeroPadding,
                                                                        int outputColumns,
                                                                        int packedRowSize,
                                                                        int packedRowStride,
                                                                        bool useVectorInstructions,
                                                                        int vectorSize,
                                                                        int numVectorBlocks)
    {
        // Input / output memory layouts (of the original node)
        const auto& inputLayout = this->GetInputMemoryLayout();
        const auto& inputSize = inputLayout.GetActiveSize();

        const auto storedElementSize = sizeof(PackedBitsType);
        const auto storedElementNumBits = 8 * storedElementSize;
        const auto numBits = storedElementNumBits; // function.GetModule().GetCompilerParameters().numBits; // for Xnor, use 32 bits in 32-bit environment
        const auto elementSize = numBits / 8;
        assert(elementSize <= storedElementSize);
        const auto filterWidth = _convolutionalParameters.receptiveField;
        const auto numInputChannels = inputSize[2]; // inputSize is the dimensions of the input to the original layer node
        const auto fieldVolumeSize = filterWidth * filterWidth * numInputChannels; // = size*size*numInputChannels

        const auto partialBlockSize = fieldVolumeSize % numBits;

        // Get LLVM types
        auto& emitter = function.GetEmitter();
        auto packedBitsType = emitter.Type(emitters::GetVariableType<PackedBitsType>());
        assert(llvm::VectorType::isValidElementType(packedBitsType) && "Invalid element type for LLVM vector");
        auto vectorType = emitter.VectorType(packedBitsType, vectorSize);
        auto vectorPointerType = vectorType->getPointerTo();

        llvm::Function* popcountFunction = function.GetModule().GetIntrinsic(llvm::Intrinsic::ctpop, { packedBitsType });
        llvm::Function* vecPopcountFunction = function.GetModule().GetIntrinsic(llvm::Intrinsic::ctpop, { vectorType });

        // The start of the binarized weights matrix for this filter
        auto weightsBegin = function.Operator(times, filterIndex, function.Literal<int>(packedRowStride));
        auto weightsBeginPtr = function.PointerOffset(pFilterWeights, weightsBegin);
        auto weightsVector = function.CastPointer(weightsBeginPtr, vectorPointerType);

        llvm::Value* filterMean = nullptr;
        if (_convolutionalParameters.weightsScale == scaleOutputByFilterMeans)
        {
            filterMean = function.ValueAt(pFilterMeans, filterIndex);
        }

        const int numScalarBlocks = packedRowSize - (vectorSize * numVectorBlocks);

        // Variables to hold the running sum of xor values
        llvm::Value* vectorSumVar = useVectorInstructions ? function.Variable(vectorType, "vecXorSum") : nullptr;
        llvm::Value* sumVar = numScalarBlocks > 0 ? function.Variable(packedBitsType, "xorSum") : nullptr;

        // Compute and accumulate xnor counts

        auto columnLoop = function.ForLoop();
        columnLoop.Begin(outputColumns);
        {
            auto outputColumnIndex = columnLoop.LoadIterationVariable(); // outputColumnIndex == input row index

            // The start of the binarized receptive field matrix for this output image pixel
            auto inputBegin = function.Operator(times, outputColumnIndex, function.Literal<int>(packedRowSize));
            auto paddingBegin = function.Operator(times, outputColumnIndex, function.Literal<int>(packedRowStride));

            auto inputBeginPtr = function.PointerOffset(pInput, inputBegin);
            auto paddingMaskBeginPtr = function.PointerOffset(pInputPaddingMask, paddingBegin);

            llvm::Value* vectorXorSum = nullptr;
            if (numVectorBlocks > 0)
            {
                assert(vectorSumVar != nullptr);

                // cast to vector pointer
                auto inputVector = function.CastPointer(inputBeginPtr, vectorPointerType);
                auto paddingMaskVector = function.CastPointer(paddingMaskBeginPtr, vectorPointerType);

                // If vector instructions are enabled, create a variable to store the running vector sum
                function.Store(vectorSumVar, emitters::FillVector<PackedBitsType>(function, vectorType, 0));
                EmitInnerLoop(function, inputVector, paddingMaskVector, weightsVector, vectorSumVar, vecPopcountFunction, 0, numVectorBlocks, hasZeroPadding);

                // Accumulate horizontal sum into output
                vectorXorSum = emitters::HorizontalVectorSum<PackedBitsType>(function, function.Load(vectorSumVar));
                assert(vectorXorSum->getType() == packedBitsType);
            }

            // Now compute the non-vectorized values
            const int numScalarBlocks = packedRowSize - (vectorSize * numVectorBlocks);
            if (numScalarBlocks > 0)
            {
                assert(sumVar != nullptr);
                function.StoreZero(sumVar);
                auto start = vectorSize * numVectorBlocks;
                EmitInnerLoop(function, inputBeginPtr, paddingMaskBeginPtr, weightsBeginPtr, sumVar, popcountFunction, start, numScalarBlocks, hasZeroPadding);
            }

            llvm::Value* xorSum = (sumVar == nullptr) ? nullptr : function.Load(sumVar);
            if (vectorXorSum != nullptr)
            {
                xorSum = (xorSum == nullptr) ? vectorXorSum : function.Operator(plus, xorSum, vectorXorSum);
            }
            assert(xorSum != nullptr);

            // Output scaling
            auto sumInt = function.CastValue<PackedBitsType, int>(xorSum);
            auto scaledSum = function.Operator(plus, function.Operator(times, function.Literal<int>(-2), sumInt), function.Literal<int>(numBits * packedRowSize));

            auto scaledSumWithPadding = scaledSum;
            if (hasZeroPadding)
            {
                // Add back the zero padding, if any (since the scaled sum is made negative, use the minus operation)
                llvm::Value* paddingSum = function.ValueAt(pInputPaddingMaskSums, outputColumnIndex);
                scaledSumWithPadding = function.Operator(minus, scaledSum, paddingSum);
            }
            auto sumFloat = function.CastValue<int, ValueType>(scaledSumWithPadding);

            llvm::Value* adjustedSum = sumFloat;
            if (partialBlockSize != 0)
            {
                const auto filterAdjust = numBits - partialBlockSize;
                adjustedSum = function.Operator(minusFloat, sumFloat, function.Literal<ValueType>(filterAdjust));
            }

            auto outIndex = function.Operator(plus, function.Operator(times, filterIndex, function.Literal((int)outputColumns)), outputColumnIndex);
            if (_convolutionalParameters.weightsScale == scaleOutputByFilterMeans)
            {
                // Scale output by the filters mean
                assert(filterMean != nullptr);
                auto scaledOutput = function.Operator(timesFloat, adjustedSum, filterMean);
                function.SetValueAt(pOutput, outIndex, scaledOutput);
            }
            else
            {
                // No output scaling
                function.SetValueAt(pOutput, outIndex, adjustedSum);
            }
        }
        columnLoop.End();
    }

    template <typename ValueType, typename PackedBitsType>
    void BinaryXnorNode<ValueType, PackedBitsType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
    }

    template <typename ValueType, typename PackedBitsType>
    void BinaryXnorNode<ValueType, PackedBitsType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
    }

    // Explicit specialization
    template class BinaryConvolutionalLayerNode<float>;
    template class BinaryConvolutionalLayerNode<double>;

} // nodes
} // ell
