////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SquaredEuclideanDistanceNode.tcc (nodes)
//  Authors:  Suresh Iyengar
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SquaredEuclideanDistanceNode.h"
#include "DotProductNode.h"
#include "L2NormNode.h"
#include "BinaryOperationNode.h"
#include "MatrixVectorProductNode.h"
#include "UnaryOperationNode.h"

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
        : Node({ &_input }, { &_output }), _input(this, {}, inputPortName), _output(this, outputPortName, 1), _v(0, 0)
    {
    }

    template <typename ValueType, math::MatrixLayout layout>
	SquaredEuclideanDistanceNode<ValueType, layout>::SquaredEuclideanDistanceNode(const model::PortElements<ValueType>& input, const math::Matrix<ValueType, layout>& v)
        : Node({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, v.NumRows()), _v(v)
    {
        assert(input.Size() == v.NumColumns());
    }

    template <typename ValueType, math::MatrixLayout layout>
    void SquaredEuclideanDistanceNode<ValueType, layout>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);

        math::MatrixArchiver::Write(_v, "v", archiver);
        archiver[inputPortName] << _input;
        archiver[outputPortName] << _output;
    }

    template <typename ValueType, math::MatrixLayout layout>
    void SquaredEuclideanDistanceNode<ValueType, layout>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);

        math::MatrixArchiver::Read(_v, "v", archiver);
        archiver[inputPortName] >> _input;
        archiver[outputPortName] >> _output;
    }

    template <typename ValueType, math::MatrixLayout layout>
    void SquaredEuclideanDistanceNode<ValueType, layout>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<SquaredEuclideanDistanceNode<ValueType, layout>>(newPortElements, _v);
        transformer.MapNodeOutput(output, newNode->output);
    }

    // We compute the distance (P - V)^2 as P^2 - 2 * P * V + V^2 where P is the input point and V is the set of vectors
    template <typename ValueType, math::MatrixLayout layout>
    bool SquaredEuclideanDistanceNode<ValueType, layout>::Refine(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());

        auto normNode1 = transformer.AddNode<L2NormNode<double>>(newPortElements);
        auto squareNormNode1 = transformer.AddNode<BinaryOperationNode<double>>(normNode1->output, normNode1->output, emitters::BinaryOperationType::coordinatewiseMultiply);

        auto vectors = _v;
        auto productNode = transformer.AddNode<MatrixVectorProductNode<double, math::MatrixLayout::rowMajor>>(newPortElements, vectors);

        std::vector<double> multiplier(_v.NumRows(), -2.0);
        auto multiplierNode = transformer.AddNode<ConstantNode<ValueType>>(multiplier);
        auto productNodeScaled = transformer.AddNode<BinaryOperationNode<double>>(productNode->output, multiplierNode->output, emitters::BinaryOperationType::coordinatewiseMultiply);

        model::PortElements<ValueType> normNode1Outputs;
        model::PortElements<ValueType> normNode2Outputs;

        for (size_t r = 0; r < _v.NumRows(); r++)
        {
            normNode1Outputs.Append(squareNormNode1->output);

            auto matrixRow = vectors.GetRow(r);
            auto pointNode = transformer.AddNode<ConstantNode<ValueType>>(matrixRow.ToArray());
            auto normNode2 = transformer.AddNode<L2NormNode<double>>(pointNode->output);
            auto squareNormNode2 = transformer.AddNode<BinaryOperationNode<double>>(normNode2->output, normNode2->output, emitters::BinaryOperationType::coordinatewiseMultiply);
            normNode2Outputs.Append(squareNormNode2->output);
        }

        auto distanceNode = transformer.AddNode<BinaryOperationNode<double>>(normNode1Outputs, productNodeScaled->output, emitters::BinaryOperationType::add);
        auto squareDistanceNode = transformer.AddNode<BinaryOperationNode<double>>(normNode2Outputs, distanceNode->output, emitters::BinaryOperationType::add);

        transformer.MapNodeOutput(output, squareDistanceNode->output);

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

        math::ColumnVector<ValueType> result(_v.NumRows());

        auto norm1sq = input.Norm2Squared();

        // result = -2 * _v * input
        math::Multiply(-2.0, _v, input, 0.0, result);

        for (size_t r = 0; r < _v.NumRows(); r++)
        {
            result[r] += norm1sq + _v.GetRow(r).Norm2Squared();
        }

        _output.SetOutput({ result.ToArray() });
    }

    template <typename ValueType, math::MatrixLayout layout>
	SquaredEuclideanDistanceNode<ValueType, layout>* AddNodeToModelTransformer(const model::PortElements<ValueType>& input, math::ConstMatrixReference<ValueType, layout> v, model::ModelTransformer& transformer)
    {
        return transformer.AddNode<SquaredEuclideanDistanceNode>(input, v);
    }
}
}
