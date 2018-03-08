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

#include "Unused.h"

// stl
#include <algorithm>
#include <numeric>

namespace ell
{
namespace nodes
{
    namespace
    {
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
                                        emitters::IRLocalArray inputVolume,
                                        const model::PortMemoryLayout& inputLayout,
                                        const predictors::neural::BinaryConvolutionalParameters& convParams,
                                        emitters::IRLocalScalar valueRow, emitters::IRLocalScalar valueColumn, emitters::IRLocalScalar valueChannel)
        {
            const int columnStride = inputLayout.GetStride(1);
            const int channelStride = inputLayout.GetStride(2);

            // row, column, channel order
            auto index = valueRow * (columnStride * channelStride) + valueColumn * channelStride + valueChannel;
            return static_cast<emitters::IRLocalScalar>(inputVolume[index]);
        }

        // TODO: adapt this to work with more generally strided data
        template <typename ValueType>
        llvm::Function* EmitGetValueFromPaddedVolumeFunction(emitters::IRModuleEmitter& moduleEmitter)
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }

        template <typename ValueType>
        llvm::Value* GetValueFromPaddedVolume(emitters::IRFunctionEmitter& function,
                                              emitters::IRLocalArray inputVolume,
                                              const model::PortMemoryLayout& inputLayout,
                                              const predictors::neural::BinaryConvolutionalParameters& convParams,
                                              size_t convPadding,
                                              emitters::IRLocalScalar inputRow, emitters::IRLocalScalar inputCol, emitters::IRLocalScalar inputChannel)
        {
            const auto inputHeight = inputLayout.GetActiveSize(0);
            const auto inputWidth = inputLayout.GetActiveSize(1);
            const auto inputDepth = inputLayout.GetActiveSize(2);
            const auto inputPadding = inputLayout.GetOffset(0);

            const int extraPadding = convPadding - inputPadding; // amount by which the convolution's desired padding exceeds input's
            if (extraPadding > 0) // known at compile-time
            {
                auto getValueFunction = EmitGetValueFromPaddedVolumeFunction<ValueType>(function.GetModule());
                return function.Call(getValueFunction, { inputVolume.value, inputRow, inputCol, inputChannel, function.Literal<int>(inputWidth), function.Literal<int>(inputHeight), function.Literal<int>(inputDepth), function.Literal<int>(extraPadding) });
            }

            if (extraPadding != 0)
            {
                inputRow = inputRow + extraPadding;
                inputCol = inputCol + extraPadding;
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
            const int numChannels = inputLayout.GetActiveSize(2);
            const int outputImageWidth = outputLayout.GetActiveSize(1);
            const int filterSize = static_cast<int>(convParams.receptiveField);
            const int stride = static_cast<int>(convParams.stride);
            const int convPadding = inputLayout.GetOffset(0); // TODO: decouple input data padding from convolution padding

            // compute offset based on outputRowIndex
            auto outputImageRow = function.LocalScalar(outputRowIndex) / outputImageWidth;
            auto outputImageCol = function.LocalScalar(outputRowIndex) % outputImageWidth;
            auto inputRowStart = outputImageRow * stride;
            auto inputColStart = outputImageCol * stride;

            auto input = function.LocalArray(inputVolume);
            auto output = function.LocalArray(realValueRow);

            // For row, column, channel order:
            function.For(filterSize, [input, inputLayout, numChannels, outputImageWidth, filterSize, stride, convParams, convPadding, outputImageRow, outputImageCol, inputRowStart, inputColStart, output](emitters::IRFunctionEmitter& function, llvm::Value* i) {
                auto rowIndex = function.LocalScalar(i);

                function.For(filterSize, [=](emitters::IRFunctionEmitter& function, llvm::Value* j) {
                    auto columnIndex = function.LocalScalar(j);

                    function.For(numChannels, [=](emitters::IRFunctionEmitter& function, llvm::Value* k) {
                        auto channelIndex = function.LocalScalar(k);
                        auto inputRow = inputRowStart + rowIndex;
                        auto inputColumn = inputColStart + columnIndex;
                        auto inputChannel = channelIndex;

                        auto value = GetValueFromPaddedVolume<ValueType>(function, input, inputLayout, convParams, convPadding, inputRow, inputColumn, inputChannel);

                        // The input is a filterSize x filterSize x numChannels image in in row x column x channel order
                        //   so offset = (filterSize*numChannels)*row + numChannels*column + channel
                        auto rowOffset = rowIndex * (filterSize * numChannels);
                        auto colOffset = columnIndex * numChannels;
                        auto channelBeginOffset = rowOffset + colOffset;
                        auto outputOffset = channelBeginOffset + channelIndex;
                        output[outputOffset] = value;
                    });
                });
            });
        }

        template <typename ValueType, typename PackedBitsType>
        void CompressRow(emitters::IRFunctionEmitter& function, llvm::Value* realRow, llvm::Value* packedOutput, int numValues)
        {
            int storedElementSize = sizeof(PackedBitsType);
            int storedElementNumBits = 8 * storedElementSize;
            int numBlocks = (numValues - 1) / storedElementNumBits + 1;
            int numCompleteBlocks = numValues / storedElementNumBits;

            auto input = function.LocalArray(realRow);
            auto output = function.LocalArray(packedOutput);
            function.For(numCompleteBlocks, [storedElementNumBits, input, output](emitters::IRFunctionEmitter& function, llvm::Value* i) {
                auto blockIndex = function.LocalScalar(i);

                // TODO: block-vectorize this:
                auto blockValue = function.LocalScalar<PackedBitsType>(0);
                for (int bitIndex = 0; bitIndex < storedElementNumBits; ++bitIndex)
                {
                    auto realValue = input[(blockIndex * storedElementNumBits) + bitIndex];
                    auto cmp = realValue > static_cast<ValueType>(0);
                    auto bitValue = function.LocalScalar(function.Select(cmp, function.Literal<PackedBitsType>(1), function.Literal<PackedBitsType>(0)));
                    // blockValue = blockValue | ((realValue>0?1:0) << bitIndex);
                    blockValue = blockValue | (bitValue << function.LocalScalar<PackedBitsType>(bitIndex));
                }
                output[blockIndex] = blockValue;
            });

            // now do the last, partial, block
            if (numBlocks > numCompleteBlocks)
            {
                assert(numBlocks == numCompleteBlocks + 1);
                int leftoverBits = numValues % storedElementNumBits;

                auto blockValue = function.LocalScalar<PackedBitsType>(0);
                for (int bitIndex = 0; bitIndex < leftoverBits; ++bitIndex)
                {
                    auto realValue = input[numCompleteBlocks * storedElementNumBits + bitIndex];
                    auto cmp = realValue > static_cast<ValueType>(0);
                    auto bitValue = function.LocalScalar(function.Select(cmp, function.Literal<PackedBitsType>(1), function.Literal<PackedBitsType>(0)));
                    // blockValue = blockValue | ((realValue>0?1:0) << bitIndex);
                    blockValue = blockValue | (bitValue << function.LocalScalar<PackedBitsType>(bitIndex));
                }

                // blockValue = function.Operator(shiftLeft, blockValue, function.Literal<PackedBitsType>(storedElementNumBits - leftoverBits));
                function.SetValueAt(packedOutput, numCompleteBlocks, blockValue);
            }
        }

        void PushPackedBits(std::vector<int64_t>& vec, const std::vector<uint64_t>& bits)
        {
            vec.insert(vec.end(), bits.begin(), bits.end());
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
        auto numPackedBits = compiler != nullptr ? compiler->GetCompilerOptions().targetDevice.numBits : 64;
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
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, defaultInputPortName), _output(this, defaultOutputPortName, 0)
    {
    }

    template <typename ValueType, typename PackedBitsType>
    BinaryReceptiveFieldMatrixNode<ValueType, PackedBitsType>::BinaryReceptiveFieldMatrixNode(const model::PortElements<ValueType>& input,
                                                                                              const predictors::neural::BinaryConvolutionalParameters& convolutionalParameters,
                                                                                              const model::PortMemoryLayout& inputMemoryLayout,
                                                                                              const model::PortMemoryLayout& outputMemoryLayout)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, GetPackedFilterSize<PackedBitsType>(convolutionalParameters, inputMemoryLayout, outputMemoryLayout)), _convolutionalParameters(convolutionalParameters), _inputMemoryLayout(inputMemoryLayout), _outputMemoryLayout(outputMemoryLayout)
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
        // Get LLVM types
        auto& module = function.GetModule();
        auto& context = module.GetLLVMContext();
        auto voidType = llvm::Type::getVoidTy(context);

        // Get port variables
        llvm::Value* inputTemp = compiler.EnsurePortEmitted(input);
        llvm::Value* outputTemp = compiler.EnsurePortEmitted(output);

        // Constants
        auto elementSize = sizeof(PackedBitsType);
        auto numBits = 8 * elementSize;
        const auto inputDepth = _inputMemoryLayout.GetActiveSize(2);
        const auto filterWidth = _convolutionalParameters.receptiveField;
        const auto fieldVolumeSize = filterWidth * filterWidth * inputDepth;

        int packedRowSize = (fieldVolumeSize - 1) / numBits + 1;
        assert(packedRowSize != 0);

        auto argTypes = emitters::GetLLVMTypes({ inputTemp, outputTemp, function.Literal<int32_t>(0), function.Literal<int32_t>(0) });
        emitters::IRFunctionEmitter taskFunction = function.GetModule().BeginFunction(utilities::to_string(GetId()) + "_task", voidType, argTypes);
        {
            auto arguments = taskFunction.Arguments().begin();
            auto pInput = &(*arguments++);
            auto pOutput = &(*arguments++);
            auto begin = &(*arguments++);
            auto end = &(*arguments++);

            // TODO: interleave load/compress more tightly to eliminate need for a scratch variable to hold a whole row
            llvm::AllocaInst* realValueRow = taskFunction.Variable(emitters::GetVariableType<ValueType>(), fieldVolumeSize);
            taskFunction.For(begin, end, [this, pInput, pOutput, packedRowSize, fieldVolumeSize, realValueRow](emitters::IRFunctionEmitter& taskFunction, llvm::Value* i) {
                auto outputRowIndex = taskFunction.LocalScalar(i);
                LoadRow<ValueType>(taskFunction,
                                   pInput,
                                   this->GetInputMemoryLayout(),
                                   outputRowIndex,
                                   this->GetOutputMemoryLayout(),
                                   _convolutionalParameters,
                                   realValueRow);

                auto outputRow = taskFunction.PointerOffset(pOutput, outputRowIndex * packedRowSize);
                CompressRow<ValueType, PackedBitsType>(taskFunction, realValueRow, outputRow, fieldVolumeSize);
            });
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

        const auto& compilerSettings = compiler.GetCompilerOptions();

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
            function.For(numOutputRows, [this, pInput, pOutput, realValueRow, packedRowSize, fieldVolumeSize](emitters::IRFunctionEmitter& function, llvm::Value* i) {
                auto outputRowIndex = function.LocalScalar(i);
                LoadRow<ValueType>(function,
                                   pInput,
                                   this->GetInputMemoryLayout(),
                                   outputRowIndex,
                                   this->GetOutputMemoryLayout(),
                                   _convolutionalParameters,
                                   realValueRow);

                auto outputRow = function.PointerOffset(pOutput, outputRowIndex * static_cast<int>(packedRowSize));
                CompressRow<ValueType, PackedBitsType>(function, realValueRow, outputRow, fieldVolumeSize);
            });
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
        : CompilableNode({ &_input, &_inputPaddingMasks, &_inputPaddingMaskSums, &_filterWeights, &_filterMeans }, { &_output }), _input(this, {}, defaultInputPortName), _inputPaddingMasks(this, {}, inputPaddingMasksPortName), _inputPaddingMaskSums(this, {}, inputPaddingMaskSumsPortName), _filterWeights(this, {}, filterWeightsPortName), _filterMeans(this, {}, filterMeansPortName), _output(this, defaultOutputPortName, 0)
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
        : CompilableNode({ &_input, &_inputPaddingMasks, &_inputPaddingMaskSums, &_filterWeights, &_filterMeans }, { &_output }), _input(this, input, defaultInputPortName), _inputPaddingMasks(this, compressedInputPaddingMasks, inputPaddingMasksPortName), _inputPaddingMaskSums(this, inputPaddingMaskSums, inputPaddingMaskSumsPortName), _filterWeights(this, compressedFilterWeights, filterWeightsPortName), _filterMeans(this, filterMeans, filterMeansPortName), _output(this, defaultOutputPortName, outputMemoryLayout.GetMemorySize()), _convolutionalParameters(convolutionalParameters), _inputPaddingParameters(inputPaddingParameters), _inputMemoryLayout(inputMemoryLayout), _outputMemoryLayout(outputMemoryLayout)
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
                                                                  llvm::Value* reshapedInputPtr,
                                                                  llvm::Value* paddingMaskPtr,
                                                                  llvm::Value* weightsPtr,
                                                                  llvm::Value* xorSumVariable,
                                                                  llvm::Function* popCountFunction,
                                                                  int startBlock,
                                                                  int numBlocks,
                                                                  bool hasZeroPadding)
    {
        auto reshapedInput = function.LocalArray(reshapedInputPtr);
        auto paddingMask = function.LocalArray(paddingMaskPtr);
        auto weights = function.LocalArray(weightsPtr);
        function.For(startBlock, startBlock + numBlocks, [reshapedInput, paddingMask, weights, xorSumVariable, popCountFunction, hasZeroPadding](emitters::IRFunctionEmitter& function, llvm::Value* i) {
            auto blockIndex = function.LocalScalar(i);

            auto inputVal = reshapedInput[blockIndex];
            auto filterVal = weights[blockIndex];
            auto xorVal = inputVal ^ filterVal;

            if (hasZeroPadding)
            {
                // Mask out the bits associated with zero padding from the XOR value
                auto paddingMaskVal = paddingMask[blockIndex];
                xorVal = paddingMaskVal & xorVal;
            }

            auto xorCount = function.Call(popCountFunction, { xorVal });
            const auto plus = emitters::TypedOperator::add;
            function.OperationAndUpdate(xorSumVariable, plus, xorCount);
        });
    }

    template <typename ValueType, typename PackedBitsType>
    void BinaryXnorNode<ValueType, PackedBitsType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        // Get compiler settings
        const auto& compilerSettings = compiler.GetCompilerOptions();
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
        const auto numBits = storedElementNumBits; // function.GetModule().GetCompilerOptions().numBits; // for Xnor, use 32 bits in 32-bit environment
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
            function.For(numFilters, [=, &compiler](emitters::IRFunctionEmitter& function, llvm::Value* i) {
                auto filterIndex = function.LocalScalar(i);
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
            });
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

        const auto& compilerSettings = compiler.GetCompilerOptions();

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
        const auto numBits = storedElementNumBits; // function.GetModule().GetCompilerOptions().numBits; // for Xnor, use 32 bits in 32-bit environment
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
                                                                        llvm::Value* filterIndexPtr,
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

        const int storedElementSize = sizeof(PackedBitsType);
        const int storedElementNumBits = 8 * storedElementSize;
        const int numBits = storedElementNumBits; // function.GetModule().GetCompilerOptions().numBits; // for Xnor, use 32 bits in 32-bit environment
        const int elementSize = numBits / 8;
        DEBUG_USED(elementSize);
        assert(elementSize <= storedElementSize);
        const int filterWidth = static_cast<int>(_convolutionalParameters.receptiveField);
        const int numInputChannels = inputSize[2]; // inputSize is the dimensions of the input to the original layer node
        const int fieldVolumeSize = filterWidth * filterWidth * numInputChannels; // = size*size*numInputChannels

        const auto partialBlockSize = fieldVolumeSize % numBits;

        auto filterIndex = function.LocalScalar(filterIndexPtr);

        // Get LLVM types
        auto& emitter = function.GetEmitter();
        auto packedBitsType = emitter.Type(emitters::GetVariableType<PackedBitsType>());
        assert(llvm::VectorType::isValidElementType(packedBitsType) && "Invalid element type for LLVM vector");
        auto vectorType = emitter.VectorType(packedBitsType, vectorSize);
        auto vectorPointerType = vectorType->getPointerTo();

        llvm::Function* popcountFunction = function.GetModule().GetIntrinsic(llvm::Intrinsic::ctpop, { packedBitsType });
        llvm::Function* vecPopcountFunction = function.GetModule().GetIntrinsic(llvm::Intrinsic::ctpop, { vectorType });

        // The start of the binarized weights matrix for this filter
        auto weightsBegin = filterIndex * packedRowStride;
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

        function.For(outputColumns, [=](emitters::IRFunctionEmitter& function, llvm::Value* i) {
            auto outputColumnIndex = function.LocalScalar(i);

            // The start of the binarized receptive field matrix for this output image pixel
            auto inputBegin = outputColumnIndex * packedRowSize;
            auto paddingBegin = outputColumnIndex * packedRowStride;

            auto inputBeginPtr = function.PointerOffset(pInput, inputBegin);
            auto paddingMaskBeginPtr = function.PointerOffset(pInputPaddingMask, paddingBegin);

            auto vectorXorSum = function.LocalScalar();
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
                assert(vectorXorSum.value->getType() == packedBitsType);
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
            if (vectorXorSum.value != nullptr)
            {
                xorSum = (xorSum == nullptr) ? vectorXorSum : xorSum + vectorXorSum;
            }
            assert(xorSum != nullptr);

            // Output scaling
            auto sumInt = function.CastValue<PackedBitsType, int>(xorSum);
            auto scaledSum = (function.LocalScalar<int>(-2) * sumInt) + (numBits * packedRowSize);

            auto scaledSumWithPadding = scaledSum;
            if (hasZeroPadding)
            {
                // Add back the zero padding, if any (since the scaled sum is made negative, use the minus operation)
                llvm::Value* paddingSum = function.ValueAt(pInputPaddingMaskSums, outputColumnIndex);
                scaledSumWithPadding = scaledSum - paddingSum;
            }
            auto sumFloat = function.CastValue<int, ValueType>(scaledSumWithPadding);

            auto adjustedSum = function.LocalScalar(sumFloat);
            if (partialBlockSize != 0)
            {
                const auto filterAdjust = numBits - partialBlockSize;
                adjustedSum = sumFloat - function.LocalScalar<ValueType>(filterAdjust);
            }

            auto outIndex = (filterIndex * outputColumns) + outputColumnIndex;
            if (_convolutionalParameters.weightsScale == scaleOutputByFilterMeans)
            {
                // Scale output by the filters mean
                assert(filterMean != nullptr);
                auto scaledOutput = adjustedSum * filterMean;
                function.SetValueAt(pOutput, outIndex, scaledOutput);
            }
            else
            {
                // No output scaling
                function.SetValueAt(pOutput, outIndex, adjustedSum);
            }
        });
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
