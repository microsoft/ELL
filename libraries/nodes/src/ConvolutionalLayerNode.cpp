////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ConvolutionalLayerNode.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ConvolutionalLayerNode.h"
#include "ConstantNode.h"
#include "DiagonalConvolutionNode.h"
#include "MatrixMatrixMultiplyNode.h"
#include "ReceptiveFieldMatrixNode.h"
#include "ReorderDataNode.h"

using namespace ell::math;
using namespace ell::math::Blas;

namespace ell
{
namespace nodes
{
    template <typename ValueType>
    ConvolutionalLayerNode<ValueType>::ConvolutionalLayerNode(const model::PortElements<ValueType>& input, const predictors::neural::ConvolutionalLayer<ValueType>& layer)
        : NeuralNetworkLayerNode<ConvolutionalLayerNode<ValueType>, predictors::neural::ConvolutionalLayer<ValueType>, ValueType>(input, layer)
    {
    }

    template <typename ValueType>
    bool ConvolutionalLayerNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        const auto inputLayout = this->GetInputMemoryLayout();
        const auto outputLayout = this->GetOutputMemoryLayout();
        const auto convParams = this->GetLayer().GetConvolutionalParameters();
        const auto inputPaddingParams = this->GetRequestedInputPadding();

        const auto inputHeight = inputLayout.GetActiveSize(0);
        const auto inputWidth = inputLayout.GetActiveSize(1);
        const auto inputDepth = inputLayout.GetActiveSize(2);
        const auto inputDataPadding = inputLayout.GetOffset(0);

        const auto filterWidth = convParams.receptiveField;

        const auto outputImageHeight = outputLayout.GetActiveSize(0);
        const auto outputImageWidth = outputLayout.GetActiveSize(1);
        const auto outputDataPadding = outputLayout.GetOffset(0);
        const auto numFilters = outputLayout.GetActiveSize(2);
        const auto outputRows = outputImageWidth * outputImageHeight;
        const auto stride = convParams.stride;

        const auto padding = inputPaddingParams.paddingSize;
        auto newInput = transformer.TransformPortElements(this->input.GetPortElements());

        bool useDiagonalConvolution = (convParams.method == predictors::neural::ConvolutionMethod::diagonal) && (stride == 1) && (filterWidth % 2 == 1);
        if (!useDiagonalConvolution)
        {
            // Needs (channel, row, column) order and no data padding

            // GEMM method
            const auto& weights = this->GetLayer().GetWeightsMatrix();
            const auto m = weights.NumRows();
            const auto n = outputRows;
            const auto k = weights.NumColumns();
            const auto lda = weights.GetIncrement();
            const auto ldb = n;
            const auto ldc = n;
            const std::array<int, 3> rcdOrder = std::array<int, 3>{ 0, 1, 2 };
            const std::array<int, 3> drcOrder = std::array<int, 3>{ 2, 0, 1 };

            bool useNewMethod = (stride == 1 && padding == filterWidth/2);
            // Input data is in the canonical RCD order
            // `dataOrder` is the order the we're going to generate the receptive field matrix from
            std::array<int, 3> dataOrder = useNewMethod ? drcOrder : rcdOrder;
            auto weightsValues = weights.ToArray();
            auto weightsNode = transformer.AddNode<ConstantNode<ValueType>>(weightsValues);

            assert(outputDataPadding == 0 && "Convolutional node output padding not supported yet");

            // weights: numFilters x fieldVolumeSize == m x k
            // ShapedInput: fieldVolumeSize x outputRows == k x n
            // Matrix multiply output: numFilters x outputRows = m x n

            if (dataOrder == rcdOrder) // don't reorder input -- use old method
            {
                auto receptiveFieldMatrixNode = transformer.AddNode<ReceptiveFieldMatrixNode<ValueType>>(newInput, inputLayout, convParams.receptiveField, convParams.stride, inputPaddingParams.paddingSize, dataOrder, outputImageWidth, outputImageHeight);
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

                auto receptiveFieldMatrixNode = transformer.AddNode<ReceptiveFieldMatrixNode<ValueType>>(reorderInputNode->output, inputLayout, convParams.receptiveField, convParams.stride, inputPaddingParams.paddingSize, dataOrder, outputImageWidth, outputImageHeight);
                auto matrixMultNode = transformer.AddNode<MatrixMatrixMultiplyNode<ValueType>>(weightsNode->output, m, n, k, lda, false, receptiveFieldMatrixNode->output, ldb, false, ldc);

                // Output of matrix multiply is in (f x h x w) order, need to transpose to (h x w x f)
                model::PortMemoryLayout outputShape({ numFilters, outputImageHeight, outputImageWidth });
                model::PortMemoryLayout transposedOutputShape({ outputImageHeight, outputImageWidth, numFilters }, { outputDataPadding, outputDataPadding, 0 });
                auto reorderOutputNode = transformer.AddNode<ReorderDataNode<ValueType>>(matrixMultNode->output, outputShape, transposedOutputShape, std::vector<int>{1, 2, 0});
                transformer.MapNodeOutput(this->output, reorderOutputNode->output);
            }
        }
        else // diagonal method
        {
            // row, column, channel order:
            auto weightsMatrix = this->GetLayer().GetWeights().ReferenceAsMatrix().Transpose();
            auto weightsValues = weightsMatrix.ToArray();
            auto weightsNode = transformer.AddNode<ConstantNode<ValueType>>(weightsValues);
            auto convNode = transformer.AddNode<DiagonalConvolutionNode<ValueType>>(newInput, inputLayout, weightsNode->output, outputLayout, convParams);
            transformer.MapNodeOutput(this->output, convNode->output);
        }
        return true;
    }

    // Explicit specializations
    template class ConvolutionalLayerNode<float>;
    template class ConvolutionalLayerNode<double>;
} // nodes
} // ell
