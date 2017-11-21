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

namespace ell
{
namespace nodes
{
    namespace
    {
        const auto plus = emitters::TypedOperator::add;
        const auto times = emitters::TypedOperator::multiply;

        const auto plusFloat = emitters::TypedOperator::addFloat;

        //
        // Functions to emit portions of IR
        //

        size_t GetDiagonalConvolutionOutputSize(const model::PortMemoryLayout& outputLayout)
        {
            return outputLayout.GetActiveSize(0) * outputLayout.GetActiveSize(1) * outputLayout.GetActiveSize(2);
        }
    } // end anonymous namespace

    //
    // DiagonalConvolutionNode
    //

    template <typename ValueType>
    DiagonalConvolutionNode<ValueType>::DiagonalConvolutionNode()
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, inputPortName), _filterWeights(this, {}, filterWeightsPortName), _output(this, outputPortName, 0)
    {
    }

    template <typename ValueType>
    DiagonalConvolutionNode<ValueType>::DiagonalConvolutionNode(const model::PortElements<ValueType>& input, const model::PortMemoryLayout& inputMemoryLayout, const model::PortElements<ValueType>& filterWeights, const model::PortMemoryLayout& outputMemoryLayout, const predictors::neural::ConvolutionalParameters& convolutionalParameters)
        : CompilableNode({ &_input, &_filterWeights }, { &_output }), _input(this, input, inputPortName), _filterWeights(this, filterWeights, filterWeightsPortName), _output(this, outputPortName, GetDiagonalConvolutionOutputSize(outputMemoryLayout)), _inputMemoryLayout(inputMemoryLayout), _outputMemoryLayout(outputMemoryLayout), _convolutionalParameters(convolutionalParameters)
    {
    }

    template <typename ValueType>
    void DiagonalConvolutionNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(_input.GetPortElements());
        auto newFilterWeights = transformer.TransformPortElements(_filterWeights.GetPortElements());
        auto newNode = transformer.AddNode<DiagonalConvolutionNode<ValueType>>(newInput, _inputMemoryLayout, newFilterWeights, _outputMemoryLayout, _convolutionalParameters);
        transformer.MapNodeOutput(this->output, newNode->output);
    }

    template <typename ValueType>
    void DiagonalConvolutionNode<ValueType>::Compute() const
    {
        // TODO: Deal with pre-padded input

        // Model parameters
        auto&& inputLayout = this->GetInputMemoryLayout();
        auto&& outputLayout = this->GetOutputMemoryLayout();
        auto&& convParams = this->GetConvolutionalParameters();
        const auto inputDepth = inputLayout.GetActiveSize(2);
        const auto inputPadding = inputLayout.GetOffset(0);
        const auto filterWidth = convParams.receptiveField;

        const auto outputWidth = outputLayout.GetActiveSize(1);
        const auto outputHeight = outputLayout.GetActiveSize(0);
        const auto numFilters = outputLayout.GetActiveSize(2);

        const auto padding = inputPadding; // ?

        // const size_t batchSize = _convolutionalParameters.numFiltersAtATime;
        const size_t batchSize = numFilters;

        const size_t paddedWidth = outputWidth + (2 * padding);
        const size_t paddedHeight = outputHeight + (2 * padding);
        assert(_input.Size() == paddedWidth * paddedHeight * inputDepth);

        const size_t numFlattenedMatrixColumns = inputDepth * paddedWidth;

        const size_t kd = filterWidth * inputDepth;
        // const size_t numConvolutions = ((paddedWidth - filterWidth) * inputDepth + 1) / inputDepth; // Note: this should be paddedWidth - filterWidth + 1?
        const size_t numConvolutions = paddedWidth - filterWidth + 1; // Note: numConvolutions == output width if padding is on

        auto inputData = _input.GetValue();
        auto filterWeightsData = _filterWeights.GetValue();
        assert(filterWeightsData.size() == filterWidth * filterWidth * inputDepth * numFilters);
        const size_t outputSize = (paddedHeight * paddedWidth) * numFilters;
        std::vector<ValueType> output(outputSize);
        std::vector<ValueType> scratch(paddedHeight * filterWidth * batchSize);

        math::RowMatrixReference<ValueType> inputMatrix(inputData.data(), paddedHeight, numFlattenedMatrixColumns);
        math::RowMatrixReference<ValueType> weightsMatrix(filterWeightsData.data(), filterWidth * inputDepth, filterWidth * numFilters);
        math::RowMatrixReference<ValueType> outputMatrix(output.data(), paddedHeight, paddedWidth * numFilters); //

        for (size_t j = 0; j < numConvolutions; j++) // each pass through this loop computes 1 column of the output image, for all filters
        {
            auto Vj = inputMatrix.GetSubMatrix(0, j * inputDepth, inputMatrix.NumRows(), kd);
            for (size_t filterStart = 0; filterStart < numFilters; filterStart += batchSize)
            {
                size_t numFiltersToUse = std::min(batchSize, numFilters - filterStart);

                auto Wl = weightsMatrix.GetSubMatrix(0, filterStart * filterWidth, weightsMatrix.NumRows(), numFiltersToUse * filterWidth);
                math::RowMatrixReference<ValueType> A(scratch.data(), Vj.NumRows(), Wl.NumColumns());

                math::Multiply(static_cast<ValueType>(1.0), Vj, Wl, static_cast<ValueType>(0.0), A);

                for (size_t l = 0; l < numFiltersToUse; l++)
                {
                    for (size_t startRow = 0; startRow < (paddedHeight - 2 * padding); startRow++) // assumes padding = floor(filterWidth/2)
                    {
                        double sum = 0.0;
                        for (size_t diagonal = 0; diagonal < filterWidth; diagonal++)
                        {
                            sum += A(startRow + diagonal, l * filterWidth + diagonal);
                        }

                        outputMatrix(startRow + padding, j + padding + paddedWidth * (filterStart + l)) = sum;
                    }
                }
            }
        }

        _output.SetOutput(output);
    }

    template <typename ValueType>
    void DiagonalConvolutionNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
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
        auto&& inputLayout = this->GetInputMemoryLayout();
        auto&& outputLayout = this->GetOutputMemoryLayout();
        auto&& convParams = this->GetConvolutionalParameters();
        const auto inputHeight = inputLayout.GetActiveSize(0);
        const auto inputWidth = inputLayout.GetActiveSize(1);
        const auto inputDepth = inputLayout.GetActiveSize(2);
        const auto filterWidth = convParams.receptiveField;
        const auto padding = inputLayout.GetOffset(0);
        assert((padding == filterWidth / 2) && "Padding must be filterWidth/2");

        // input data parameters
        const size_t paddedWidth = inputLayout.GetStride(1);
        const size_t paddedHeight = inputLayout.GetStride(0);

        // output data parameters
        const auto numFilters = outputLayout.GetActiveSize(2);

        // computation parameters
        const size_t batchSize = numFilters;
        const size_t stackSize = inputWidth;

        // TODO: check this carefully to make sure it's valid for stackSize != all and stackSize != 1
        const size_t stackedInputHeight = (inputHeight + padding) * stackSize + padding;
        const size_t columnsPerStack = (inputWidth - 1) / stackSize + 1;
        const size_t stackedInputWidth = (columnsPerStack + 2 * padding);
        const size_t stackedInputSize = stackedInputWidth * stackedInputHeight * inputDepth;

        llvm::Value* pStackedInput = nullptr;
        const size_t inputStride = paddedWidth * inputDepth;
        const size_t stackedInputStride = stackedInputWidth * inputDepth;
        if (stackSize != 1)
        {
            llvm::GlobalVariable* stackedInput = function.GetModule().GlobalArray(emitters::GetVariableType<ValueType>(), "stackedInput", stackedInputSize);
            pStackedInput = function.PointerOffset(stackedInput, 0); // convert "global variable" to a pointer

            // Fill in input memory
            // First, the top p rows of zeros (padding):
            for (int rowIndex = 0; rowIndex < padding; ++rowIndex)
            {
                function.MemoryCopy<ValueType>(pInput, rowIndex * inputStride, pStackedInput, rowIndex * stackedInputStride, stackedInputStride);
            }

            // Now skip past the first p rows of padding and copy the rest
            auto inputPtr = function.PointerOffset(pInput, padding * inputStride);
            auto stackedInputPtr = function.PointerOffset(pStackedInput, padding * stackedInputStride);
            auto stackLoop = function.ForLoop();
            stackLoop.Begin(stackSize); // foreach stack
            {
                auto stackIndex = stackLoop.LoadIterationVariable();
                auto inputColOffset = function.Operator(times, stackIndex, function.Literal<int>(columnsPerStack * inputDepth));
                auto stackBeginOffset = function.Operator(times, stackIndex, function.Literal<int>((inputHeight + padding) * stackedInputStride));
                auto copyLoop = function.ForLoop();
                copyLoop.Begin(inputHeight + padding); // foreach row
                {
                    auto rowIndex = copyLoop.LoadIterationVariable();
                    auto inputOffset = function.Operator(plus, inputColOffset, function.Operator(times, rowIndex, function.Literal<int>(inputStride)));
                    auto outputOffset = function.Operator(plus, stackBeginOffset, function.Operator(times, rowIndex, function.Literal<int>(stackedInputStride)));
                    function.MemoryCopy<ValueType>(inputPtr, inputOffset, stackedInputPtr, outputOffset, function.Literal<int>(stackedInputStride));
                }
                copyLoop.End();
            }
            stackLoop.End();
        }
        else
        {
            pStackedInput = pInput;
        }

        // Allocate scratch memory for 'A' matrix
        // TODO: this is really paddedHeight * filterWidth * batchSize * stackSize - padding * filterWidth * batchSize
        //              == (inputHeight + padding) * filterWidth * batchSize * (stackSize + padding);
        const size_t scratchMemSize = paddedHeight * filterWidth * batchSize * stackSize;
        llvm::GlobalVariable* scratch = function.GetModule().GlobalArray(emitters::GetVariableType<ValueType>(), "scratch", scratchMemSize);
        auto scratchPtr = function.PointerOffset(scratch, 0); // Convert LLVM array to pointer

        const int outputStride = paddedWidth * numFilters;
        const size_t numConvolutions = (inputWidth - 1) / stackSize + 1;
        auto convLoop = function.ForLoop();
        convLoop.Begin(numConvolutions);
        {
            auto j = convLoop.LoadIterationVariable(); // j = start column for convolution

            // Get the submatrix for Vj
            auto inputOffset = function.Operator(times, j, function.Literal<int>(inputDepth));
            llvm::Value* Vj = function.PointerOffset(pStackedInput, inputOffset);

            // now for each batch of filter weights
            for (size_t filterStart = 0; filterStart < numFilters; filterStart += batchSize)
            {
                size_t numFiltersToUse = std::min(batchSize, numFilters - filterStart);

                // Get the submatrix for Wl
                auto weightsOffset = filterStart * filterWidth;
                llvm::Value* Wl = function.PointerOffset(pWeights, weightsOffset);

                // int m = paddedHeight;
                int m = stackedInputHeight;
                int n = filterWidth * numFiltersToUse; // this batch
                int k = inputDepth * filterWidth;
                int lda = stackedInputWidth * inputDepth;
                int ldb = filterWidth * numFilters;
                int ldc = filterWidth * batchSize;

                // Note: Wl is transposed
                function.CallGEMM<ValueType>(false, true, m, n, k, Vj, lda, Wl, ldb, scratchPtr, ldc);

                // S loop here as well
                auto stackLoop = function.ForLoop();
                stackLoop.Begin(stackSize);
                {
                    auto stackIndex = stackLoop.LoadIterationVariable();
                    auto stackRowOffset = function.Operator(times, stackIndex, function.Literal<int>(inputHeight + padding));
                    auto outputColumn = function.Operator(plus, j, function.Operator(times, stackIndex, function.Literal<int>(numConvolutions)));

                    auto lLoop = function.ForLoop();
                    lLoop.Begin(numFiltersToUse);
                    {
                        auto l = lLoop.LoadIterationVariable(); // batchFilterIndex
                        auto filterIndex = function.Operator(plus, function.Literal<int>(filterStart), l);
                        auto startRowLoop = function.ForLoop();
                        startRowLoop.Begin(inputHeight);
                        {
                            auto startRow = startRowLoop.LoadIterationVariable();
                            auto stackStartRow = function.Operator(plus, stackRowOffset, startRow);
                            llvm::Value* sum = nullptr;
                            for (size_t diagonal = 0; diagonal < filterWidth; diagonal++)
                            {
                                auto currRow = function.Operator(plus, stackStartRow, function.Literal<int>(diagonal));
                                auto currRowOffset = function.Operator(times, currRow, function.Literal<int>(batchSize * filterWidth));
                                // auto currColOffset = function.Operator(plus, function.Literal<int>(batchSize * diagonal), l);
                                // col offset = l*k + diagonal
                                auto currColOffset = function.Operator(plus, function.Operator(times, l, function.Literal<int>(filterWidth)), function.Literal<int>(diagonal));

                                auto inputIndex = function.Operator(plus, currRowOffset, currColOffset);
                                llvm::Value* diagonalValue = function.ValueAt(scratchPtr, inputIndex);
                                // diagonalValue = A[startRow + diagonal, l*k + diagonal]
                                if (sum == nullptr)
                                    sum = diagonalValue;
                                else
                                    sum = function.Operator(plusFloat, sum, diagonalValue);
                            }
                            auto outRowOffset = function.Operator(times, startRow, function.Literal<int>(outputStride));
                            auto outColOffset = function.Operator(times, outputColumn, function.Literal<int>(numFilters));
                            auto outputIndex = function.Operator(plus, function.Operator(plus, outRowOffset, outColOffset), filterIndex);
                            function.SetValueAt(pOutput, outputIndex, sum);
                        }
                        startRowLoop.End();
                    }
                    lLoop.End();
                }
                stackLoop.End();
            }
        }
        convLoop.End();
    }

    // Explicit specializations
    template class DiagonalConvolutionNode<float>;
    template class DiagonalConvolutionNode<double>;
} // nodes
} // ell
