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
    namespace
    {
        size_t GetOutputSize(const model::PortMemoryLayout& outputLayout)
        {
            return outputLayout.GetActiveSize(0) * outputLayout.GetActiveSize(1) * outputLayout.GetActiveSize(2);
        }
    } // end anonymous namespace

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
                                                                    const predictors::neural::ConvolutionalParameters& convolutionalParameters,
                                                                    const predictors::neural::PaddingParameters& inputPaddingParameters,
                                                                    const predictors::neural::PaddingParameters& outputPaddingParameters)
    : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, GetOutputSize(outputMemoryLayout)), _inputMemoryLayout(inputMemoryLayout), _outputMemoryLayout(outputMemoryLayout), _filterWeights(0,0), _convolutionalParameters(convolutionalParameters), _inputPaddingParameters(inputPaddingParameters), _outputPaddingParameters(outputPaddingParameters)
    {
        _filterWeights = GetWeightsMatrix(filterWeights);
    }
    
    template <typename ValueType>
    UnrolledConvolutionNode<ValueType>::UnrolledConvolutionNode(const model::PortElements<ValueType>& input, 
                                                                    const model::PortMemoryLayout& inputMemoryLayout,
                                                                    const model::PortMemoryLayout& outputMemoryLayout,
                                                                    ConstMatrixReferenceType filterWeights,
                                                                    const predictors::neural::ConvolutionalParameters& convolutionalParameters,
                                                                    const predictors::neural::PaddingParameters& inputPaddingParameters,
                                                                    const predictors::neural::PaddingParameters& outputPaddingParameters)
        : CompilableNode({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, GetOutputSize(outputMemoryLayout)), _inputMemoryLayout(inputMemoryLayout), _outputMemoryLayout(outputMemoryLayout), _filterWeights(filterWeights), _convolutionalParameters(convolutionalParameters), _inputPaddingParameters(inputPaddingParameters), _outputPaddingParameters(outputPaddingParameters)
    {
    }
    
    template <typename ValueType>
    typename UnrolledConvolutionNode<ValueType>::MatrixType UnrolledConvolutionNode<ValueType>::GetWeightsMatrix(const ConstTensorReferenceType& weightsTensor) const
    {
        const size_t filterWidth = _convolutionalParameters.receptiveField;
        const size_t inputDepth = GetInputMemoryLayout().GetActiveSize(2);
        const size_t numFilters = GetOutputMemoryLayout().GetActiveSize(2);

        // Reshape the weights
        MatrixType weightsMatrix {numFilters, filterWidth * filterWidth * inputDepth};
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
        auto newNode = transformer.AddNode<UnrolledConvolutionNode<ValueType>>(newInput, _inputMemoryLayout, _outputMemoryLayout, _filterWeights, _convolutionalParameters, _inputPaddingParameters, _outputPaddingParameters);
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
        // Add the weights as matrix inside a ConstantNode in (row, column), channel order:
        auto weightsValues = _filterWeights.ToArray();
        auto weightsNode = transformer.AddNode<ConstantNode<ValueType>>(weightsValues);

        const auto inputLayout = this->GetInputMemoryLayout();
        const auto outputLayout = this->GetOutputMemoryLayout();

        const auto inputHeight = inputLayout.GetActiveSize(0);
        const auto inputWidth = inputLayout.GetActiveSize(1);
        const auto inputDepth = inputLayout.GetActiveSize(2);
        const auto inputDataPadding = inputLayout.GetOffset(0);

        const auto filterWidth = _convolutionalParameters.receptiveField;

        const auto outputImageHeight = outputLayout.GetActiveSize(0);
        const auto outputImageWidth = outputLayout.GetActiveSize(1);
        const auto outputDataPadding = outputLayout.GetOffset(0);
        const auto numFilters = outputLayout.GetActiveSize(2);
        const auto outputRows = outputImageWidth * outputImageHeight;
        const auto stride = _convolutionalParameters.stride;

        const auto padding = _inputPaddingParameters.paddingSize;
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
        bool useNewMethod = (stride == 1 && padding == filterWidth/2);
        std::array<int, 3> dataOrder = useNewMethod ? drcOrder : rcdOrder;
        assert(outputDataPadding == 0 && "Convolutional node output padding not supported yet");

        // weights: numFilters x fieldVolumeSize == m x k
        // ShapedInput: fieldVolumeSize x outputRows == k x n
        // Matrix multiply output: numFilters x outputRows = m x n

        if (dataOrder == rcdOrder) // don't reorder input -- use old method
        {
            auto receptiveFieldMatrixNode = transformer.AddNode<ReceptiveFieldMatrixNode<ValueType>>(newInput, inputLayout, _convolutionalParameters.receptiveField, _convolutionalParameters.stride, _inputPaddingParameters.paddingSize, dataOrder, outputImageWidth, outputImageHeight);
            auto matrixMultNode = transformer.AddNode<MatrixMatrixMultiplyNode<ValueType>>(weightsNode->output, m, n, k, lda, false, receptiveFieldMatrixNode->output, ldb, false, ldc);

            // Output of matrix multiply is in (f x h x w) order, need to transpose to (h x w x f)
            model::PortMemoryLayout outputShape({ numFilters, outputImageHeight, outputImageWidth });
            model::PortMemoryLayout transposedOutputShape({ outputImageHeight, outputImageWidth, numFilters }, { outputDataPadding, outputDataPadding, 0 });
            auto reorderOutputNode = transformer.AddNode<ReorderDataNode<ValueType>>(matrixMultNode->output, outputShape, transposedOutputShape, std::vector<int>{ 1, 2, 0 });
            transformer.MapNodeOutput(this->output, reorderOutputNode->output);
        }
        else // reorder input to be channels x rows x columns (then we can use the 'new' receptive field matrix generation)
        {
            assert(dataOrder == drcOrder);

            // Remove padding and transpose to DRC order
            model::PortMemoryLayout inputShape({ inputHeight, inputWidth, inputDepth }, { inputDataPadding, inputDataPadding, 0 });
            model::PortMemoryLayout transposedInputShape({ inputDepth, inputHeight, inputWidth});
            auto reorderInputNode = transformer.AddNode<ReorderDataNode<ValueType>>(newInput, inputShape, transposedInputShape, std::vector<int>{ 2, 0, 1 });

            auto receptiveFieldMatrixNode = transformer.AddNode<ReceptiveFieldMatrixNode<ValueType>>(reorderInputNode->output, inputLayout, _convolutionalParameters.receptiveField, _convolutionalParameters.stride, _inputPaddingParameters.paddingSize, dataOrder, outputImageWidth, outputImageHeight);
            auto matrixMultNode = transformer.AddNode<MatrixMatrixMultiplyNode<ValueType>>(weightsNode->output, m, n, k, lda, false, receptiveFieldMatrixNode->output, ldb, false, ldc);

            // Output of matrix multiply is in (f x h x w) order, need to transpose to (h x w x f)
            model::PortMemoryLayout outputShape({ numFilters, outputImageHeight, outputImageWidth });
            model::PortMemoryLayout transposedOutputShape({ outputImageHeight, outputImageWidth, numFilters }, { outputDataPadding, outputDataPadding, 0 });
            auto reorderOutputNode = transformer.AddNode<ReorderDataNode<ValueType>>(matrixMultNode->output, outputShape, transposedOutputShape, std::vector<int>{1, 2, 0});
            transformer.MapNodeOutput(this->output, reorderOutputNode->output);
        }
        return true;
    }

    // Explicit specializations
    template class UnrolledConvolutionNode<float>;
    template class UnrolledConvolutionNode<double>;
} // nodes
} // ell
