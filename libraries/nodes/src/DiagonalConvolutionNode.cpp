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
    namespace
    {
        int GetOutputSize(const model::PortMemoryLayout& outputLayout)
        {
            return outputLayout.GetActiveSize(0) * outputLayout.GetActiveSize(1) * outputLayout.GetActiveSize(2);
        }
    } // end anonymous namespace

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
                                                                const predictors::neural::ConvolutionalParameters& convolutionalParameters,
                                                                const predictors::neural::PaddingParameters& inputPaddingParameters,
                                                                const predictors::neural::PaddingParameters& outputPaddingParameters)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, GetOutputSize(outputMemoryLayout)), _inputMemoryLayout(inputMemoryLayout), _outputMemoryLayout(outputMemoryLayout), _filterWeights(filterWeights), _convolutionalParameters(convolutionalParameters), _inputPaddingParameters(inputPaddingParameters), _outputPaddingParameters(outputPaddingParameters)
    {
    }

    template <typename ValueType>
    void DiagonalConvolutionNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<DiagonalConvolutionNode<ValueType>>(newInput, _inputMemoryLayout, _outputMemoryLayout, _filterWeights, _convolutionalParameters, _inputPaddingParameters, _outputPaddingParameters);
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
        auto weightsNode = transformer.AddNode<ConstantNode<ValueType>>(weightsValues);
        auto convNode = transformer.AddNode<DiagonalConvolutionComputeNode<ValueType>>(newInput, weightsNode->output, _inputMemoryLayout, _outputMemoryLayout, _convolutionalParameters, _inputPaddingParameters, _outputPaddingParameters);
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
        const auto filterWidth = static_cast<int>(this->GetConvolutionalParameters().receptiveField);

        const auto outputLayout = this->GetOutputMemoryLayout();
        const auto outputWidth = outputLayout.GetActiveSize(1);
        const auto outputHeight = outputLayout.GetActiveSize(0);
        const auto numFilters = outputLayout.GetActiveSize(2);

        const auto padding = inputPadding;

        // const int batchSize = _convolutionalParameters.numFiltersAtATime;
        const int batchSize = numFilters;

        const int paddedWidth = outputWidth + (2 * padding);
        const int paddedHeight = outputHeight + (2 * padding);
        assert(static_cast<int>(_input.Size()) == paddedWidth * paddedHeight * inputDepth);

        const int numFlattenedMatrixColumns = inputDepth * paddedWidth;

        const int kd = filterWidth * inputDepth;
        // const int numConvolutions = ((paddedWidth - filterWidth) * inputDepth + 1) / inputDepth; // Note: this should be paddedWidth - filterWidth + 1?
        const int numConvolutions = paddedWidth - filterWidth + 1; // Note: numConvolutions == output width if padding is on

        auto inputData = _input.GetValue();
        auto filterWeightsData = _filterWeights.ToArray();
        assert(static_cast<int>(filterWeightsData.size()) == filterWidth * filterWidth * inputDepth * numFilters);
        const int outputSize = (paddedHeight * paddedWidth) * numFilters;
        std::vector<ValueType> output(outputSize);
        std::vector<ValueType> scratch(paddedHeight * filterWidth * batchSize);

        math::RowMatrixReference<ValueType> inputMatrix(inputData.data(), paddedHeight, numFlattenedMatrixColumns);
        math::RowMatrixReference<ValueType> weightsMatrix(filterWeightsData.data(), filterWidth * inputDepth, filterWidth * numFilters);
        math::RowMatrixReference<ValueType> outputMatrix(output.data(), paddedHeight, paddedWidth * numFilters);

        for (int j = 0; j < numConvolutions; j++) // each pass through this loop computes 1 column of the output image, for all filters
        {
            // Get the sub matrix for Vj
            auto Vj = inputMatrix.GetSubMatrix(0, j * inputDepth, inputMatrix.NumRows(), kd);
            for (int filterStart = 0; filterStart < numFilters; filterStart += batchSize)
            {
                int numFiltersToUse = std::min(batchSize, numFilters - filterStart);

                auto Wl = weightsMatrix.GetSubMatrix(0, filterStart * filterWidth, weightsMatrix.NumRows(), numFiltersToUse * filterWidth);
                math::RowMatrixReference<ValueType> A(scratch.data(), Vj.NumRows(), Wl.NumColumns());

                math::MultiplyScaleAddUpdate(static_cast<ValueType>(1.0), Vj, Wl, static_cast<ValueType>(0.0), A);

                for (int l = 0; l < numFiltersToUse; l++)
                {
                    for (int startRow = 0; startRow < (paddedHeight - 2 * padding); startRow++) // assumes padding = floor(filterWidth/2)
                    {
                        double sum = 0.0;
                        for (int diagonal = 0; diagonal < filterWidth; diagonal++)
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
                                                                              const predictors::neural::ConvolutionalParameters& convolutionalParameters,
                                                                              const predictors::neural::PaddingParameters& inputPaddingParameters,
                                                                              const predictors::neural::PaddingParameters& outputPaddingParameters)
        : CompilableNode({ &_input, &_filterWeights }, { &_output }), _input(this, input, defaultInputPortName), _filterWeights(this, filterWeights, filterWeightsPortName), _output(this, defaultOutputPortName, GetOutputSize(outputMemoryLayout)), _inputMemoryLayout(inputMemoryLayout), _outputMemoryLayout(outputMemoryLayout), _convolutionalParameters(convolutionalParameters), _inputPaddingParameters(inputPaddingParameters), _outputPaddingParameters(outputPaddingParameters)
    {
    }

    template <typename ValueType>
    void DiagonalConvolutionComputeNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(_input.GetPortElements());
        auto newFilterWeights = transformer.TransformPortElements(_filterWeights.GetPortElements());
        auto newNode = transformer.AddNode<DiagonalConvolutionComputeNode<ValueType>>(newInput, newFilterWeights, _inputMemoryLayout, _outputMemoryLayout, _convolutionalParameters, _inputPaddingParameters, _outputPaddingParameters);
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
        const auto convParams = this->GetConvolutionalParameters();
        const auto inputHeight = inputLayout.GetActiveSize(0);
        const auto inputWidth = inputLayout.GetActiveSize(1);
        const auto inputDepth = inputLayout.GetActiveSize(2);
        const int filterWidth = static_cast<int>(convParams.receptiveField);
        const int padding = inputLayout.GetOffset(0);
        assert((padding == filterWidth / 2) && "Padding must be filterWidth/2");

        // input data parameters
        const int paddedWidth = inputLayout.GetStride(1);
        const int paddedHeight = inputLayout.GetStride(0);

        // output data parameters
        const int numFilters = outputLayout.GetActiveSize(2);

        // computation parameters
        const int batchSize = numFilters;
        const int stackSize = inputWidth;

        // TODO: check this carefully to make sure it's valid for stackSize != all and stackSize != 1
        const int stackedInputHeight = (inputHeight + padding) * stackSize + padding;
        const int columnsPerStack = (inputWidth - 1) / stackSize + 1;
        const int stackedInputWidth = (columnsPerStack + 2 * padding);
        const int stackedInputSize = stackedInputWidth * stackedInputHeight * inputDepth;

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
            for (int rowIndex = 0; rowIndex < padding; ++rowIndex)
            {
                function.MemoryCopy<ValueType>(pInput, rowIndex * inputStride, pStackedInput, rowIndex * stackedInputStride, stackedInputStride);
            }

            // Now skip past the first p rows of padding and copy the rest
            auto inputPtr = function.PointerOffset(pInput, padding * inputStride);
            auto stackedInputPtr = function.PointerOffset(pStackedInput, padding * stackedInputStride);
            function.For(stackSize, [=](emitters::IRFunctionEmitter& function, llvm::Value* loopIndex1) {
                auto stackIndex = function.LocalScalar(loopIndex1);
                auto inputColOffset = stackIndex * (columnsPerStack * inputDepth);
                auto stackBeginOffset = stackIndex * ((inputHeight + padding) * stackedInputStride);
                const auto numRows = inputHeight + padding;
                function.For(numRows, [=](emitters::IRFunctionEmitter& function, llvm::Value* loopIndex2) {
                    auto rowIndex = function.LocalScalar(loopIndex2);
                    auto inputOffset = inputColOffset + (rowIndex * inputStride);
                    auto outputOffset = stackBeginOffset + (rowIndex * stackedInputStride);
                    function.MemoryCopy<ValueType>(inputPtr, inputOffset, stackedInputPtr, outputOffset, function.LocalScalar<int>(stackedInputStride));
                });
            });
        }
        else
        {
            pStackedInput = pInput;
        }

        // Allocate scratch memory for 'A' matrix
        // TODO: this is really paddedHeight * filterWidth * batchSize * stackSize - padding * filterWidth * batchSize
        //              == (inputHeight + padding) * filterWidth * batchSize * (stackSize + padding);
        const int scratchMemSize = paddedHeight * filterWidth * batchSize * stackSize;
        llvm::GlobalVariable* scratch = function.GetModule().GlobalArray(emitters::GetVariableType<ValueType>(), "scratch", scratchMemSize);
        auto scratchPtr = function.PointerOffset(scratch, 0); // Convert LLVM array to pointer

        const int outputStride = paddedWidth * numFilters;
        const int numConvolutions = (inputWidth - 1) / stackSize + 1;
        function.For(numConvolutions, [=](emitters::IRFunctionEmitter& function, llvm::Value* loopIndex1) {
            auto j = function.LocalScalar(loopIndex1); // j = start column for convolution

            // Get the submatrix for Vj
            auto inputOffset = j * inputDepth;
            llvm::Value* Vj = function.PointerOffset(pStackedInput, inputOffset);

            // now for each batch of filter weights
            for (int filterStart = 0; filterStart < numFilters; filterStart += batchSize)
            {
                int numFiltersToUse = std::min(batchSize, numFilters - filterStart);

                // Get the submatrix for Wl
                auto weightsOffset = filterStart * filterWidth;
                llvm::Value* Wl = function.PointerOffset(pWeights, weightsOffset);

                // int m = paddedHeight;
                const int m = stackedInputHeight;
                const int n = filterWidth * numFiltersToUse; // this batch
                const int k = inputDepth * filterWidth;
                const int lda = stackedInputWidth * inputDepth;
                const int ldb = filterWidth * numFilters;
                const int ldc = filterWidth * batchSize;
                const auto rowStride = ldc;

                // Note: Wl is transposed
                function.CallGEMM<ValueType>(false, true, m, n, k, Vj, lda, Wl, ldb, scratchPtr, ldc);

                // S loop here as well
                // BUG: The explicit list of by-reference captures here is to work around a GCC bug
                function.For(stackSize, [=, &numFilters, &numConvolutions, &filterWidth](emitters::IRFunctionEmitter& function, llvm::Value* loopIndex2) {
                    auto stackIndex = function.LocalScalar(loopIndex2);
                    auto stackRowOffset = stackIndex * (inputHeight + padding);
                    auto outputColumn = (stackIndex * numConvolutions) + j;

                    function.For(numFiltersToUse, [=](emitters::IRFunctionEmitter& function, llvm::Value* loopIndex3) {
                        auto l = function.LocalScalar(loopIndex3); // batchFilterIndex
                        auto filterIndex = filterStart + l;
                        function.For(inputHeight, [=](emitters::IRFunctionEmitter& function, llvm::Value* loopIndex4) {
                            auto startRow = function.LocalScalar(loopIndex4);
                            auto stackStartRow = stackRowOffset + startRow;
                            auto sum = function.LocalScalar();
                            for (int diagonal = 0; diagonal < filterWidth; diagonal++)
                            {
                                auto currRow = stackStartRow + diagonal;
                                auto currRowOffset = currRow * rowStride;

                                // col offset = l*k + diagonal
                                // auto currColOffset = function.LocalScalar<int>(batchSize * diagonal) + l;
                                auto currColOffset = (l * filterWidth) + diagonal;

                                auto inputIndex = currRowOffset + currColOffset;
                                auto diagonalValue = function.LocalScalar(function.ValueAt(scratchPtr, inputIndex));

                                if (sum.value == nullptr)
                                    sum = diagonalValue;
                                else
                                    sum = sum + diagonalValue;
                            }
                            auto outRowOffset = startRow * outputStride;
                            auto outColOffset = outputColumn * numFilters;
                            auto outputIndex = outRowOffset + outColOffset + filterIndex;
                            function.SetValueAt(pOutput, outputIndex, sum);
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
