////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DiagonalConvolutionNode.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DiagonalConvolutionNode.h"
#include "ConstantNode.h"
#include "MatrixMatrixMultiplyNode.h"

// math
#include "Matrix.h"
#include "MatrixOperations.h"

using namespace std::string_literals;

namespace ell
{
namespace nodes
{
    //
    // DiagonalConvolutionNode
    //

    template <typename ValueType>
    DiagonalConvolutionNode<ValueType>::DiagonalConvolutionNode()
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, defaultInputPortName), _output(this, defaultOutputPortName, 0)
    {
    }

    template <typename ValueType>
    DiagonalConvolutionNode<ValueType>::DiagonalConvolutionNode(const model::PortElements<ValueType>& input,
                                                                const model::PortMemoryLayout& inputMemoryLayout,
                                                                const model::PortMemoryLayout& outputMemoryLayout,
                                                                const ConstTensorReferenceType& filterWeights,
                                                                int stride)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, outputMemoryLayout), _inputMemoryLayout(inputMemoryLayout), _filterWeights(filterWeights), _stride(stride)
    {
    }

    template <typename ValueType>
    void DiagonalConvolutionNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<DiagonalConvolutionNode<ValueType>>(newInput, _inputMemoryLayout, GetOutputMemoryLayout(), _filterWeights, _stride);
        transformer.MapNodeOutput(this->output, newNode->output);
    }

    template <typename ValueType>
    bool DiagonalConvolutionNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(this->input.GetPortElements());

        // row, column, channel order:
        const auto& weightsMatrix = _filterWeights.ReferenceAsMatrix();
        auto weightsTranspose = weightsMatrix.Transpose();
        auto weightsValues = weightsTranspose.ToArray();
        int filterSize = _filterWeights.NumColumns();
        auto weightsNode = transformer.AddNode<ConstantNode<ValueType>>(weightsValues);
        auto convNode = transformer.AddNode<DiagonalConvolutionComputeNode<ValueType>>(newInput, weightsNode->output, _inputMemoryLayout, GetOutputMemoryLayout(), filterSize, _stride);
        transformer.MapNodeOutput(this->output, convNode->output);
        return true;
    }

    template <typename ValueType>
    void DiagonalConvolutionNode<ValueType>::Compute() const
    {
        // Model parameters
        const auto inputLayout = this->GetInputMemoryLayout();
        const auto inputDepth = inputLayout.GetActiveSize(2);
        const auto inputPadding = inputLayout.GetOffset(0);
        const int filterSize = _filterWeights.NumColumns();

        const auto outputLayout = this->GetOutputMemoryLayout();
        const auto outputWidth = outputLayout.GetActiveSize(1);
        const auto outputHeight = outputLayout.GetActiveSize(0);
        const int numFilters = outputLayout.GetActiveSize(2);
        const int batchSize = numFilters;

        const auto paddedWidth = outputWidth + (2 * inputPadding);
        const auto paddedHeight = outputHeight + (2 * inputPadding);
        assert(static_cast<int>(_input.Size()) == paddedWidth * paddedHeight * inputDepth);

        const int numFlattenedMatrixColumns = inputDepth * paddedWidth;

        const int kd = filterSize * inputDepth;
        // const size_t numConvolutions = ((paddedWidth - filterSize) * inputDepth + 1) / inputDepth; // Note: this should be paddedWidth - filterSize + 1?
        const int numConvolutions = paddedWidth - filterSize + 1; // Note: numConvolutions == output width if padding is on

        auto inputData = _input.GetValue();
        auto filterWeightsData = _filterWeights.ToArray();
        assert(static_cast<int>(filterWeightsData.size()) == filterSize * filterSize * inputDepth * numFilters);
        const size_t outputSize = (paddedHeight * paddedWidth) * numFilters;
        std::vector<ValueType> output(outputSize);
        std::vector<ValueType> scratch(paddedHeight * filterSize * batchSize);

        math::RowMatrixReference<ValueType> inputMatrix(inputData.data(), paddedHeight, numFlattenedMatrixColumns);
        math::RowMatrixReference<ValueType> weightsMatrix(filterWeightsData.data(), filterSize * inputDepth, filterSize * numFilters);
        math::RowMatrixReference<ValueType> outputMatrix(output.data(), paddedHeight, paddedWidth * numFilters);

        for (int j = 0; j < numConvolutions; j++) // each pass through this loop computes 1 column of the output image, for all filters
        {
            // Get the sub matrix for Vj
            auto Vj = inputMatrix.GetSubMatrix(0, j * inputDepth, inputMatrix.NumRows(), kd);
            for (int filterStart = 0; filterStart < numFilters; filterStart += batchSize)
            {
                int numFiltersToUse = std::min(batchSize, numFilters - filterStart);

                auto Wl = weightsMatrix.GetSubMatrix(0, filterStart * filterSize, weightsMatrix.NumRows(), numFiltersToUse * filterSize);
                math::RowMatrixReference<ValueType> A(scratch.data(), Vj.NumRows(), Wl.NumColumns());

                math::MultiplyScaleAddUpdate(static_cast<ValueType>(1.0), Vj, Wl, static_cast<ValueType>(0.0), A);

                for (int l = 0; l < numFiltersToUse; l++)
                {
                    for (int startRow = 0; startRow < (paddedHeight - 2 * inputPadding); startRow++) // assumes padding = floor(filterSize/2)
                    {
                        double sum = 0.0;
                        for (int diagonal = 0; diagonal < filterSize; diagonal++)
                        {
                            sum += A(startRow + diagonal, l * filterSize + diagonal);
                        }

                        outputMatrix(startRow + inputPadding, j + inputPadding + paddedWidth * (filterStart + l)) = sum;
                    }
                }
            }
        }

        _output.SetOutput(output);
    }

    template <typename ValueType>
    void DiagonalConvolutionNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        model::CompilableNode::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver["inputLayout"] << _inputMemoryLayout;
        archiver["outputLayout"] << GetOutputMemoryLayout();
        archiver["stride"] << _stride;
        math::TensorArchiver::Write(_filterWeights, "weights", archiver);
    }

    template <typename ValueType>
    void DiagonalConvolutionNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        model::CompilableNode::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver["inputLayout"] >> _inputMemoryLayout;
        model::PortMemoryLayout outputMemoryLayout;
        archiver["outputLayout"] >> outputMemoryLayout;
        _output.SetMemoryLayout(outputMemoryLayout);
        archiver["stride"] >> _stride;
        math::TensorArchiver::Read(_filterWeights, "weights", archiver);
    }

    //
    // DiagonalConvolutionComputeNode
    //

    template <typename ValueType>
    DiagonalConvolutionComputeNode<ValueType>::DiagonalConvolutionComputeNode()
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, defaultInputPortName), _filterWeights(this, {}, filterWeightsPortName), _output(this, defaultOutputPortName, 0)
    {
    }

    template <typename ValueType>
    DiagonalConvolutionComputeNode<ValueType>::DiagonalConvolutionComputeNode(const model::PortElements<ValueType>& input,
                                                                              const model::PortElements<ValueType>& filterWeights,
                                                                              const model::PortMemoryLayout& inputMemoryLayout,
                                                                              const model::PortMemoryLayout& outputMemoryLayout,
                                                                              int filterSize,
                                                                              int stride)
        : CompilableNode({ &_input, &_filterWeights }, { &_output }), _input(this, input, defaultInputPortName), _filterWeights(this, filterWeights, filterWeightsPortName), _output(this, defaultOutputPortName, outputMemoryLayout), _inputMemoryLayout(inputMemoryLayout), _filterSize(filterSize), _stride(stride)
    {
        const int numFilters = outputMemoryLayout.GetActiveSize(2);
        _batchSize = numFilters;
    }

    template <typename ValueType>
    void DiagonalConvolutionComputeNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(_input.GetPortElements());
        auto newFilterWeights = transformer.TransformPortElements(_filterWeights.GetPortElements());
        auto newNode = transformer.AddNode<DiagonalConvolutionComputeNode<ValueType>>(newInput, newFilterWeights, _inputMemoryLayout, GetOutputMemoryLayout(), _filterSize, _stride);
        transformer.MapNodeOutput(this->output, newNode->output);
    }

    template <typename ValueType>
    void DiagonalConvolutionComputeNode<ValueType>::Compute() const
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
    }

    template <typename ValueType>
    void DiagonalConvolutionComputeNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        // input is a d x (w+2p) x (h+2p) array
        // reshaped, it's a d*(w+2p)) x (h+2p) array == d*(w+k-1) x (h+k-1)
        llvm::Value* pInput = compiler.EnsurePortEmitted(this->input);

        // weights is f x k x k x d array
        // reshaped, it's (f*k) x (k*d) or f x k x (k*d)
        llvm::Value* pWeights = compiler.EnsurePortEmitted(this->filterWeights);

        // output is a (w+2p) x (h+2p) x f array
        llvm::Value* pOutput = compiler.EnsurePortEmitted(this->output);

        // Model parameters
        const auto inputLayout = this->GetInputMemoryLayout();
        const auto outputLayout = this->GetOutputMemoryLayout();
        const auto inputHeight = inputLayout.GetActiveSize(0);
        const auto inputWidth = inputLayout.GetActiveSize(1);
        const auto inputDepth = inputLayout.GetActiveSize(2);
        const auto filterSize = _filterSize;
        const auto inputPadding = inputLayout.GetOffset(0);
        assert((inputPadding == filterSize / 2) && "Input padding must be filterSize/2");

        const auto outputTensor = function.LocalTensor(pOutput, outputLayout.GetStride().ToVector(), emitters::RowMajorTensorLayout);

        // input data parameters
        const int paddedWidth = inputLayout.GetStride(1);
        const int paddedHeight = inputLayout.GetStride(0);

        // output data parameters
        const int numFilters = outputLayout.GetActiveSize(2);

        // computation parameters
        const int batchSize = _batchSize;
        const size_t stackSize = inputWidth;

        // TODO: check this carefully to make sure it's valid for stackSize != all and stackSize != 1
        const size_t stackedInputHeight = (inputHeight + inputPadding) * stackSize + inputPadding;
        const size_t columnsPerStack = (inputWidth - 1) / stackSize + 1;
        const size_t stackedInputWidth = (columnsPerStack + 2 * inputPadding);
        const size_t stackedInputSize = stackedInputWidth * stackedInputHeight * inputDepth;

        const auto stackedInputStride = stackedInputWidth * inputDepth;
        const auto inputStride = paddedWidth * inputDepth;
        llvm::Value* pStackedInput = nullptr;
        if (stackSize != 1)
        {
            auto stackedInputVariableName = "stackedInput_" + GetInternalStateIdentifier();
            llvm::GlobalVariable* stackedInput = function.GetModule().GlobalArray(emitters::GetVariableType<ValueType>(), stackedInputVariableName, stackedInputSize);
            pStackedInput = function.PointerOffset(stackedInput, 0); // convert "global variable" to a pointer

            // Fill in input memory
            // First, the top p rows of zeros (padding):
            for (int rowIndex = 0; rowIndex < inputPadding; ++rowIndex)
            {
                function.MemoryCopy<ValueType>(pInput, rowIndex * inputStride, pStackedInput, rowIndex * stackedInputStride, stackedInputStride);
            }

            // Now skip past the first p rows of padding and copy the rest
            auto inputPtr = function.PointerOffset(pInput, inputPadding * inputStride);
            auto stackedInputPtr = function.PointerOffset(pStackedInput, inputPadding * stackedInputStride);
            function.For(stackSize, [=](emitters::IRFunctionEmitter& function, llvm::Value* loopIndex1) {
                auto stackIndex = function.LocalScalar(loopIndex1);
                auto inputColOffset = stackIndex * function.LocalScalar<int>(columnsPerStack * inputDepth);
                auto stackBeginOffset = stackIndex * function.LocalScalar<int>((inputHeight + inputPadding) * stackedInputStride);
                const auto numRows = inputHeight + inputPadding;
                function.For(numRows, [=](emitters::IRFunctionEmitter& function, llvm::Value* loopIndex2) {
                    auto rowIndex = function.LocalScalar(loopIndex2);
                    auto inputOffset = inputColOffset + (rowIndex * function.LocalScalar<int>(inputStride));
                    auto outputOffset = stackBeginOffset + (rowIndex * function.LocalScalar<int>(stackedInputStride));
                    function.MemoryCopy<ValueType>(inputPtr, inputOffset, stackedInputPtr, outputOffset, function.LocalScalar<int>(stackedInputStride));
                });
            });
        }
        else
        {
            pStackedInput = pInput;
        }

        // Allocate scratch memory for 'A' matrix
        // TODO: this is really paddedHeight * filterSize * batchSize * stackSize - inputPadding * filterSize * batchSize
        //              == (inputHeight + inputPadding) * filterSize * batchSize * (stackSize + inputPadding);
        const size_t scratchMemSize = paddedHeight * filterSize * batchSize * stackSize;
        llvm::GlobalVariable* scratch = function.GetModule().GlobalArray(emitters::GetVariableType<ValueType>(), "scratch", scratchMemSize);
        auto scratchPtr = function.PointerOffset(scratch, 0); // Convert LLVM array to pointer

        const size_t numConvolutions = (inputWidth - 1) / stackSize + 1;
        function.For(numConvolutions, [inputDepth, pStackedInput, pWeights, scratchPtr, inputPadding, inputHeight, outputTensor, numFilters, batchSize, filterSize, stackedInputHeight, stackSize, stackedInputWidth, numConvolutions](emitters::IRFunctionEmitter& function, llvm::Value* loopIndex1) {
            auto j = function.LocalScalar(loopIndex1); // j = start column for convolution

            // Get the submatrix for Vj
            auto inputOffset = j * function.LocalScalar<int>(inputDepth);
            llvm::Value* Vj = function.PointerOffset(pStackedInput, inputOffset);

            // now for each batch of filter weights
            for (int filterStart = 0; filterStart < numFilters; filterStart += batchSize)
            {
                int numFiltersToUse = std::min(batchSize, numFilters - filterStart);

                // Get the submatrix for Wl
                auto weightsOffset = filterStart * filterSize;
                llvm::Value* Wl = function.PointerOffset(pWeights, weightsOffset);

                // int m = paddedHeight;
                const int m = stackedInputHeight;
                const int n = filterSize * numFiltersToUse; // this batch
                const int k = inputDepth * filterSize;
                const int lda = stackedInputWidth * inputDepth;
                const int ldb = filterSize * numFilters;
                const int ldc = filterSize * batchSize;

                // Note: Wl is transposed
                function.CallGEMM<ValueType>(false, true, m, n, k, Vj, lda, Wl, ldb, scratchPtr, ldc);

                // S loop here as well
                function.For(stackSize, [inputPadding, j, numFiltersToUse, numConvolutions, filterStart, inputHeight, filterSize, scratchPtr, outputTensor, batchSize](emitters::IRFunctionEmitter& function, llvm::Value* loopIndex2) {
                    auto stackIndex = function.LocalScalar(loopIndex2);
                    auto stackRowOffset = stackIndex * function.LocalScalar<int>(inputHeight + inputPadding);
                    auto outputColumn = (stackIndex * function.LocalScalar<int>(numConvolutions)) + j;

                    function.For(numFiltersToUse, [filterStart, inputHeight, stackRowOffset, filterSize, scratchPtr, outputColumn, outputTensor, batchSize](emitters::IRFunctionEmitter& function, llvm::Value* loopIndex3) {
                        auto l = function.LocalScalar(loopIndex3); // batchFilterIndex
                        auto filterIndex = function.LocalScalar<int>(filterStart) + l;

                        function.For(inputHeight, [stackRowOffset, filterSize, l, scratchPtr, outputColumn, filterIndex, batchSize, outputTensor](emitters::IRFunctionEmitter& function, llvm::Value* loopIndex4) {
                            auto startRow = function.LocalScalar(loopIndex4);
                            auto stackStartRow = stackRowOffset + startRow;
                            auto sum = function.LocalScalar();
                            for (int diagonal = 0; diagonal < filterSize; diagonal++)
                            {
                                auto currRow = stackStartRow + function.LocalScalar<int>(diagonal);
                                auto currRowOffset = currRow * function.LocalScalar<int>(batchSize * filterSize);
                                // auto currColOffset = function.LocalScalar<int>(batchSize * diagonal) + l;
                                // col offset = l*k + diagonal
                                auto currColOffset = (l * function.LocalScalar<int>(filterSize)) + function.LocalScalar<int>(diagonal);

                                auto inputIndex = currRowOffset + currColOffset;
                                auto diagonalValue = function.LocalScalar(function.ValueAt(scratchPtr, inputIndex));
                                // diagonalValue = A[startRow + diagonal, l*k + diagonal]
                                if (sum.value == nullptr)
                                    sum = diagonalValue;
                                else
                                    sum = sum + diagonalValue;
                            }

                            outputTensor({startRow, outputColumn, filterIndex}) = sum;
                        });
                    });
                });
            }
        });
    }

    // Explicit specializations
    template class DiagonalConvolutionNode<float>;
    template class DiagonalConvolutionNode<double>;
} // nodes
} // ell
