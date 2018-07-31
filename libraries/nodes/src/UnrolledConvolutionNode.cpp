////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     UnrolledConvolutionNode.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "UnrolledConvolutionNode.h"
#include "ConstantNode.h"
#include "MatrixMatrixMultiplyNode.h"
#include "ReceptiveFieldMatrixNode.h"
#include "ReorderDataNode.h"

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    UnrolledConvolutionNode<ValueType>::UnrolledConvolutionNode()
        : CompilableNode({ &_input }, { &_output }), _input(this, {}, defaultInputPortName), _output(this, defaultOutputPortName, 0), _filterWeights(0, 0)
    {
    }

    template <typename ValueType>
    UnrolledConvolutionNode<ValueType>::UnrolledConvolutionNode(const model::PortElements<ValueType>& input,
                                                                const model::PortMemoryLayout& inputMemoryLayout,
                                                                const model::PortMemoryLayout& outputMemoryLayout,
                                                                const ConstTensorReferenceType& filterWeights,
                                                                int stride)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, outputMemoryLayout), _inputMemoryLayout(inputMemoryLayout), _filterWeights(0, 0), _filterSize(filterWeights.NumColumns()), _stride(stride)
    {
        _isDepthwiseSeparable = (filterWeights.NumChannels() == 1) && (inputMemoryLayout.GetActiveSize()[2] > 1);
        _filterWeights = GetWeightsMatrix(filterWeights);
    }

    template <typename ValueType>
    UnrolledConvolutionNode<ValueType>::UnrolledConvolutionNode(const model::PortElements<ValueType>& input,
                                                                const model::PortMemoryLayout& inputMemoryLayout,
                                                                const model::PortMemoryLayout& outputMemoryLayout,
                                                                ConstMatrixReferenceType filterWeights,
                                                                int filterSize,
                                                                int stride)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, outputMemoryLayout), _inputMemoryLayout(inputMemoryLayout), _filterWeights(filterWeights), _filterSize(filterSize), _stride(stride)
    {
        _isDepthwiseSeparable = (static_cast<int>(filterWeights.NumColumns()) == (filterSize * filterSize)) && (inputMemoryLayout.GetActiveSize()[2] > static_cast<int>(1));
    }

    template <typename ValueType>
    typename UnrolledConvolutionNode<ValueType>::MatrixType UnrolledConvolutionNode<ValueType>::GetWeightsMatrix(const ConstTensorReferenceType& weightsTensor) const
    {
        const size_t filterWidth = weightsTensor.NumColumns();
        const size_t inputDepth = _isDepthwiseSeparable ? 1 : GetInputMemoryLayout().GetActiveSize(2);
        const size_t numFilters = GetOutputMemoryLayout().GetActiveSize(2);

        // Reshape the weights
        MatrixType weightsMatrix{ numFilters, filterWidth * filterWidth * inputDepth };
        auto flattened = weightsTensor.ReferenceAsMatrix();

        for (size_t startRow = 0; startRow < flattened.NumRows() / filterWidth; startRow++)
        {
            for (size_t row = 0; row < filterWidth; row++)
            {
                auto weightsVector = flattened.GetMajorVector(startRow * filterWidth + row);
                for (size_t i = 0; i < weightsVector.Size(); i++)
                {
                    const size_t columnOffset = row * weightsVector.Size();
                    weightsMatrix(startRow, columnOffset + i) = weightsVector[i];
                }
            }
        }

        return weightsMatrix;
    }

    template <typename ValueType>
    void UnrolledConvolutionNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<UnrolledConvolutionNode<ValueType>>(newInput, _inputMemoryLayout, GetOutputMemoryLayout(), _filterWeights, _filterSize, _stride);
        transformer.MapNodeOutput(this->output, newNode->output);
    }

    template <typename ValueType>
    void UnrolledConvolutionNode<ValueType>::Compute() const
    {
        throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
    }

    template <typename ValueType>
    bool UnrolledConvolutionNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        if (_isDepthwiseSeparable)
        {
            return false;
        }
        // Add the weights as matrix inside a ConstantNode in (row, column), channel order:
        auto weightsValues = _filterWeights.ToArray();
        auto weightsNode = transformer.AddNode<ConstantNode<ValueType>>(weightsValues);

        const auto inputLayout = this->GetInputMemoryLayout();
        const auto outputLayout = this->GetOutputMemoryLayout();

        const auto inputHeight = inputLayout.GetActiveSize(0);
        const auto inputWidth = inputLayout.GetActiveSize(1);
        const auto inputDepth = inputLayout.GetActiveSize(2);
        const auto inputPadding = inputLayout.GetOffset(0);

        const auto outputImageHeight = outputLayout.GetActiveSize(0);
        const auto outputImageWidth = outputLayout.GetActiveSize(1);
        const auto outputPadding = outputLayout.GetOffset(0);
        const auto numFilters = outputLayout.GetActiveSize(2);
        const auto outputRows = outputImageWidth * outputImageHeight;
        const auto filterSize = _filterSize;
        auto newInput = transformer.TransformPortElements(this->input.GetPortElements());

        // Needs (channel, row, column) order and no data padding
        const auto m = _filterWeights.NumRows();
        const auto n = outputRows;
        const auto k = _filterWeights.NumColumns();
        const auto lda = _filterWeights.NumColumns();
        const auto ldb = n;
        const auto ldc = n;
        const std::array<int, 3> rcdOrder = { 0, 1, 2 };
        const std::array<int, 3> drcOrder = { 2, 0, 1 };

        // Input data is in the canonical RCD order
        // `dataOrder` is the order the we're going to generate the receptive field matrix from
        bool useNewMethod = (_stride == 1 && inputPadding == filterSize / 2);
        std::array<int, 3> dataOrder = useNewMethod ? drcOrder : rcdOrder;
        assert(outputPadding == 0 && "Unrolled convolution node output padding not supported yet");

        // weights: numFilters x fieldVolumeSize == m x k
        // ShapedInput: fieldVolumeSize x outputRows == k x n
        // Matrix multiply output: numFilters x outputRows = m x n

        if (dataOrder == rcdOrder) // don't reorder input -- use old method
        {
            auto receptiveFieldMatrixNode = transformer.AddNode<ReceptiveFieldMatrixNode<ValueType>>(newInput, inputLayout, filterSize, _stride, inputPadding, dataOrder, outputImageWidth, outputImageHeight);
            auto matrixMultNode = transformer.AddNode<MatrixMatrixMultiplyNode<ValueType>>(weightsNode->output, m, n, k, lda, false, receptiveFieldMatrixNode->output, ldb, false, ldc);

            // Output of matrix multiply is in (f x h x w) order, need to transpose to the canonical (h x w x f) order
            model::PortMemoryLayout outputShape(model::MemoryShape{ numFilters, outputImageHeight, outputImageWidth }, model::DimensionOrder{ { 2, 0, 1 } }); // Note: memory layout constructor takes the sizes in physical dimension order
            model::PortMemoryLayout transposedOutputShape(model::MemoryShape{ outputImageHeight, outputImageWidth, numFilters }, model::MemoryShape{ outputPadding, outputPadding, 0 }, model::DimensionOrder{ { 0, 1, 2 } });
            auto reorderOutputNode = transformer.AddNode<ReorderDataNode<ValueType>>(matrixMultNode->output, outputShape, transposedOutputShape);
            transformer.MapNodeOutput(this->output, reorderOutputNode->output);
        }
        else // reorder input to be channels x rows x columns (then we can use the 'new' receptive field matrix generation)
        {
            assert(dataOrder == drcOrder);
            // Remove padding and transpose to DRC order
            model::PortMemoryLayout inputShape(model::MemoryShape{ inputHeight, inputWidth, inputDepth }, model::MemoryShape{ inputPadding, inputPadding, 0 });
            model::PortMemoryLayout transposedInputShape(model::MemoryShape{ inputDepth, inputHeight, inputWidth },  model::DimensionOrder{ 2, 0, 1 });
            auto reorderInputNode = transformer.AddNode<ReorderDataNode<ValueType>>(newInput, inputShape, transposedInputShape);

            auto receptiveFieldMatrixNode = transformer.AddNode<ReceptiveFieldMatrixNode<ValueType>>(reorderInputNode->output, inputLayout, _filterSize, _stride, inputPadding, dataOrder, outputImageWidth, outputImageHeight);
            auto matrixMultNode = transformer.AddNode<MatrixMatrixMultiplyNode<ValueType>>(weightsNode->output, m, n, k, lda, false, receptiveFieldMatrixNode->output, ldb, false, ldc);

            // Output of matrix multiply is in (f x h x w) order, need to transpose to the canonical (h x w x f) order
            model::PortMemoryLayout outputShape(model::MemoryShape{ numFilters, outputImageHeight, outputImageWidth }, model::DimensionOrder{ { 2, 0, 1 } }); // Note: memory layout constructor takes the sizes in physical dimension order
            model::PortMemoryLayout transposedOutputShape(model::MemoryShape{ outputImageHeight, outputImageWidth, numFilters }, model::MemoryShape{ outputPadding, outputPadding, 0 }, model::DimensionOrder{ { 0, 1, 2 } });
            auto reorderOutputNode = transformer.AddNode<ReorderDataNode<ValueType>>(matrixMultNode->output, outputShape, transposedOutputShape);
            transformer.MapNodeOutput(this->output, reorderOutputNode->output);
        }
        return true;
    }

    template <typename ValueType>
    void UnrolledConvolutionNode<ValueType>::Compile(model::IRMapCompiler& compiler, emitters::IRFunctionEmitter& function)
    {
        llvm::Value* pInput = compiler.EnsurePortEmitted(this->input);
        llvm::Value* pOutput = compiler.EnsurePortEmitted(this->output);

        // Input / output memory layouts
        const auto& inputLayout = this->GetInputMemoryLayout();

        const auto& outputLayout = this->GetOutputMemoryLayout();
        const auto numFilters = outputLayout.GetActiveSize(2);
        const auto& outputSize = outputLayout.GetActiveSize();
        const auto& outputOffset = outputLayout.GetOffset();

        // Calculate cumulative increment for each dimension
        model::MemoryShape inputIncrement = inputLayout.GetCumulativeIncrement();
        model::MemoryShape outputIncrement = outputLayout.GetCumulativeIncrement();

        // Weights matrix
        auto pVarWeights = function.GetModule().Variables().AddVariable<emitters::LiteralVectorVariable<ValueType>>(_filterWeights.ToArray());
        auto weights = function.GetModule().EnsureEmitted(*pVarWeights);

        // Calculate input dimension parameters
        const int outputRows = outputSize[0];
        const int outputColumns = outputSize[1];
        const int outputElements = outputRows * outputColumns;
        const int depth = outputSize[2];
        //const int fieldSize = _filterSize;
        const int fieldArea = _filterSize * _filterSize;

        // Pointers to beginning of 'active' area of input and output
        const auto outputBufferOffset = (outputIncrement[0] * outputOffset[0]) + (outputIncrement[1] * outputOffset[1]) + (outputIncrement[2] * outputOffset[2]);
        auto inputBuffer = function.PointerOffset(pInput, 0);
        auto outputBuffer = function.PointerOffset(pOutput, outputBufferOffset);

        // Create temporary space for a matrix which is (output rows * output columns, _filterSize * _filterSize)
        llvm::AllocaInst* reshapedInputMatrix = function.Variable(emitters::GetVariableType<ValueType>(), outputElements * fieldArea);

        // Loop over all input channels.
        // The large capture parameters for the lambda are to work around a bug in gcc 5.4
        function.For(numFilters, [this, inputBuffer, outputBuffer, reshapedInputMatrix, inputIncrement, outputIncrement, weights, fieldArea, outputRows,
                                  outputColumns, outputElements, depth]
            (emitters::IRFunctionEmitter& function, llvm::Value* fValue) 
        {
            auto f = function.LocalScalar(fValue);

            llvm::Value* inputPtr = function.PointerOffset(inputBuffer, f);
            llvm::Value* shapedInputPtr = function.PointerOffset(reshapedInputMatrix, 0);
            llvm::Value* outputPtr = function.PointerOffset(outputBuffer, f);

            auto input = function.LocalArray(inputPtr);
            auto shapedInput = function.LocalArray(shapedInputPtr);
            auto shapedInputRowIncrement = function.Literal(outputColumns * fieldArea);

            // ReceptiveFieldToRows for this channel/filter.
            // Iterate over all h * w locations in the output image
            function.For(outputRows, [&] (emitters::IRFunctionEmitter& function, llvm::Value* outputImageRowValue) 
            {
                auto outputImageRow = function.LocalScalar(outputImageRowValue);
                auto inputRow = outputImageRow * _stride;
                function.For(outputColumns, [&]
                    (emitters::IRFunctionEmitter& function, llvm::Value* outputImageColumnValue)
                {
                    auto outputImageColumn = function.LocalScalar(outputImageColumnValue);
                    auto inputColumn = outputImageColumn * _stride;
                    auto fieldOffset = (outputImageRow * shapedInputRowIncrement) + (outputImageColumn * fieldArea);

                    // Unroll this inner loop since _filterSize is generally small
                    for (int fieldIndex = 0; fieldIndex < fieldArea; ++fieldIndex)
                    {
                        auto fieldRow = function.Literal(fieldIndex / _filterSize);
                        auto fieldColumn = function.Literal(fieldIndex % _filterSize);
                        auto shapedInputOffset = fieldOffset + fieldIndex;

                        auto inputOffset = ((inputRow + fieldRow) * inputIncrement[0]) + ((inputColumn + fieldColumn) * inputIncrement[1]);

                        shapedInput[shapedInputOffset] = input[inputOffset];
                    }
                });
            });
            auto weightsPtr = function.PointerOffset(weights, f * fieldArea);
            function.CallGEMV<ValueType>(outputElements, fieldArea, shapedInput, (int)fieldArea, weightsPtr, (int)1, outputPtr, (int)depth);
        });
    }

    template <typename ValueType>
    void UnrolledConvolutionNode<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        model::CompilableNode::WriteToArchive(archiver);
        archiver[defaultInputPortName] << _input;
        archiver["inputLayout"] << _inputMemoryLayout;
        archiver["outputLayout"] << GetOutputMemoryLayout();
        archiver["filterSize"] << _filterSize;
        archiver["stride"] << _stride;
        math::MatrixArchiver::Write(_filterWeights, "weights", archiver);
    }

    template <typename ValueType>
    void UnrolledConvolutionNode<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        model::CompilableNode::ReadFromArchive(archiver);
        archiver[defaultInputPortName] >> _input;
        archiver["inputLayout"] >> _inputMemoryLayout;
        model::PortMemoryLayout outputMemoryLayout;
        archiver["outputLayout"] >> outputMemoryLayout;
        _output.SetMemoryLayout(outputMemoryLayout);
        archiver["filterSize"] >> _filterSize;
        archiver["stride"] >> _stride;
        math::MatrixArchiver::Read(_filterWeights, "weights", archiver);
        _isDepthwiseSeparable = (static_cast<int>(_filterWeights.NumColumns()) == (_filterSize * _filterSize));
    }

    // Explicit specializations
    template class UnrolledConvolutionNode<float>;
    template class UnrolledConvolutionNode<double>;
} // nodes
} // ell
