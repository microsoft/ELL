////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SquaredEuclideanDistanceNode.tcc (nodes)
//  Authors:  Suresh Iyengar, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SquaredEuclideanDistanceNode.h"
#include "UnaryOperationNode.h"
#include "ConstantNode.h"
#include "DotProductNode.h"
#include "L2NormSquaredNode.h"
#include "BinaryOperationNode.h"
#include "MatrixVectorProductNode.h"

// math
#include "MatrixOperations.h"

// utilities
#include "Exception.h"

// stl
#include <cassert>
#include <vector>

namespace ell
{
namespace nodes
{
    template <typename ValueType, math::MatrixLayout layout>
    SquaredEuclideanDistanceNode<ValueType, layout>::SquaredEuclideanDistanceNode()
        : Node({ &_input }, { &_output }), _input(this, {}, defaultInputPortName), _output(this, defaultOutputPortName, 1), _vectorsAsMatrix(0, 0)
    {
    }

    template <typename ValueType, math::MatrixLayout layout>
    SquaredEuclideanDistanceNode<ValueType, layout>::SquaredEuclideanDistanceNode(const model::PortElements<ValueType>& input, const math::Matrix<ValueType, layout>& vectorsAsMatrix)
        : Node({ &_input }, { &_output }), _input(this, input, defaultInputPortName), _output(this, defaultOutputPortName, vectorsAsMatrix.NumRows()), _vectorsAsMatrix(vectorsAsMatrix)
    {
        assert(input.Size() == vectorsAsMatrix.NumColumns());
    }

    template <typename ValueType, math::MatrixLayout layout>
    void SquaredEuclideanDistanceNode<ValueType, layout>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);

        math::MatrixArchiver::Write(_vectorsAsMatrix, "vectorsAsMatrix", archiver);
        archiver[defaultInputPortName] << _input;
        archiver[defaultOutputPortName] << _output;
    }

    template <typename ValueType, math::MatrixLayout layout>
    void SquaredEuclideanDistanceNode<ValueType, layout>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);

        math::MatrixArchiver::Read(_vectorsAsMatrix, "vectorsAsMatrix", archiver);
        archiver[defaultInputPortName] >> _input;
        archiver[defaultOutputPortName] >> _output;
    }

    template <typename ValueType, math::MatrixLayout layout>
    void SquaredEuclideanDistanceNode<ValueType, layout>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<SquaredEuclideanDistanceNode<ValueType, layout>>(newPortElements, _vectorsAsMatrix);
        transformer.MapNodeOutput(output, newNode->output);
    }

    // We compute the distance (P - V)^2 as P^2 - 2 * P * V + V^2 where P is the input point and V is the set of vectors
    template <typename ValueType, math::MatrixLayout layout>
    bool SquaredEuclideanDistanceNode<ValueType, layout>::Refine(model::ModelTransformer& transformer) const
    {
        auto inputPortElements = transformer.TransformPortElements(_input.GetPortElements());

        // P^2 => scalar value
        auto inputNorm2SquaredNode = transformer.AddNode<L2NormSquaredNode<double>>(inputPortElements);

        // -2 * P * V => row-wise vector
        auto vectorsAsMatrix = _vectorsAsMatrix;
        vectorsAsMatrix.Transform([](double d) { return -2.0 * d; });
        auto productNode = transformer.AddNode<MatrixVectorProductNode<double, math::MatrixLayout::rowMajor>>(inputPortElements, vectorsAsMatrix);

        // Will hold the scalar value of P^2 for each row in the matrix
        model::PortElements<ValueType> inputNorm2SquaredNodeOutputs;
        // V^2 => row-wise vector of Norm-2 squared values of each vector in _vectorsAsMatrix
        model::PortElements<ValueType> vectorNorm2SquaredConstantNodeOutputs;
        for (size_t index = 0; index < _vectorsAsMatrix.NumRows(); ++index)
        {
            inputNorm2SquaredNodeOutputs.Append(inputNorm2SquaredNode->output);

            auto matrixRow = _vectorsAsMatrix.GetRow(index);
            auto rowNorm2SquaredConstantNode = transformer.AddNode<ConstantNode<ValueType>>(matrixRow.Norm2Squared());
            vectorNorm2SquaredConstantNodeOutputs.Append(rowNorm2SquaredConstantNode->output);
        }

        // Add the three node outputs:
        //   * inputNorm2SquaredNodeOutputs (A)
        //   * vectorNorm2SquaredConstantNodeOutputs (B)
        //   * productNode->output (C)
        // and map it to output node
        auto& A = inputNorm2SquaredNodeOutputs;
        auto& B = vectorNorm2SquaredConstantNodeOutputs;
        auto& C = productNode->output;
        auto aPlusB = transformer.AddNode<BinaryOperationNode<double>>(A, B, emitters::BinaryOperationType::add);
        auto aPlusBPlusC = transformer.AddNode<BinaryOperationNode<double>>(aPlusB->output, C, emitters::BinaryOperationType::add);
        transformer.MapNodeOutput(output, aPlusBPlusC->output);

        return true;
    }

    template <typename ValueType, math::MatrixLayout layout>
    void SquaredEuclideanDistanceNode<ValueType, layout>::Compute() const
    {
        math::ColumnVector<ValueType> input(_input.Size());
        for (size_t index = 0; index < _input.Size(); ++index)
        {
            input[index] = _input[index];
        }

        math::ColumnVector<ValueType> result(_vectorsAsMatrix.NumRows());

        auto norm1sq = input.Norm2Squared();

        // result = -2 * _v * input
        math::MultiplyScaleAddUpdate(-2.0, _vectorsAsMatrix, input, 0.0, result);

        for (size_t r = 0; r < _vectorsAsMatrix.NumRows(); r++)
        {
            result[r] += norm1sq + _vectorsAsMatrix.GetRow(r).Norm2Squared();
        }

        _output.SetOutput(result.ToArray());
    }

    template <typename ValueType, math::MatrixLayout layout>
    SquaredEuclideanDistanceNode<ValueType, layout>* AddNodeToModelTransformer(const model::PortElements<ValueType>& input, math::ConstMatrixReference<ValueType, layout> vectorsAsMatrix, model::ModelTransformer& transformer)
    {
        return transformer.AddNode<SquaredEuclideanDistanceNode>(input, vectorsAsMatrix);
    }
}
}
