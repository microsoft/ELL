////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     EuclidianDistanceNode.tcc (nodes)
//  Authors:  Suresh Iyengar
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "EuclideanDistanceNode.h"
#include "DotProductNode.h"

// math
#include "Operations.h"

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
    EuclideanDistanceNode<ValueType, layout>::EuclideanDistanceNode()
        : Node({ &_input }, { &_output }), _input(this, {}, inputPortName), _output(this, outputPortName, 1), _w(0, 0)
    {
    }

    template <typename ValueType, math::MatrixLayout layout>
    EuclideanDistanceNode<ValueType, layout>::EuclideanDistanceNode(const model::PortElements<ValueType>& input, const math::Matrix<ValueType, layout>& w)
        : Node({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, w.NumRows()), _w(w)
    {
        assert(input.Size() == w.NumColumns());
    }

    template <typename ValueType, math::MatrixLayout layout>
    void EuclideanDistanceNode<ValueType, layout>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);

        archiver["w_rows"] << _w.NumRows();
        archiver["w_columns"] << _w.NumColumns();
        std::vector<double> temp;
        temp.assign(_w.GetDataPointer(), _w.GetDataPointer() + (size_t)(_w.NumRows() * _w.NumColumns()));
        archiver["w"] << temp;

        archiver[inputPortName] << _input;
        archiver[outputPortName] << _output;
    }

    template <typename ValueType, math::MatrixLayout layout>
    void EuclideanDistanceNode<ValueType, layout>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);

        size_t w_rows = 0;
        size_t w_columns = 0;
        archiver["w_rows"] >> w_rows;
        archiver["w_columns"] >> w_columns;
        std::vector<double> temp;
        archiver["w"] >> temp;
        _w = math::Matrix<ValueType, layout>(w_rows, w_columns, temp);

        archiver[inputPortName] >> _input;
        archiver[outputPortName] >> _output;
    }

    template <typename ValueType, math::MatrixLayout layout>
    void EuclideanDistanceNode<ValueType, layout>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<EuclideanDistanceNode<ValueType, layout>>(newPortElements, _w);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType, math::MatrixLayout layout>
    bool EuclideanDistanceNode<ValueType, layout>::Refine(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());

        auto normNode1 = transformer.AddNode<L2NormNode<double>>(newPortElements);
        auto squareNormNode1 = transformer.AddNode<BinaryOperationNode<double>>(normNode1->output, normNode1->output, emitters::BinaryOperationType::coordinatewiseMultiply);

        auto pointsMatrix = _w;
        auto productNode = transformer.AddNode<MatrixVectorProductNode<double, math::MatrixLayout::rowMajor>>(newPortElements, pointsMatrix);

        std::vector<double> multiplier(_w.NumRows(), -2.0);
        auto multiplierNode = transformer.AddNode<ConstantNode<ValueType>>(multiplier);
        auto productNodeScaled = transformer.AddNode<BinaryOperationNode<double>>(productNode->output, multiplierNode->output, emitters::BinaryOperationType::coordinatewiseMultiply);

        model::PortElements<ValueType> normNode1Outputs;
        model::PortElements<ValueType> normNode2Outputs;

        for (size_t r = 0; r < _w.NumRows(); r++)
        {
            normNode1Outputs.Append(squareNormNode1->output);

            auto matrixRow = pointsMatrix.GetRow(r);
            auto pointNode = transformer.AddNode<ConstantNode<ValueType>>(matrixRow.ToArray());
            auto normNode2 = transformer.AddNode<L2NormNode<double>>(pointNode->output);
            auto squareNormNode2 = transformer.AddNode<BinaryOperationNode<double>>(normNode2->output, normNode2->output, emitters::BinaryOperationType::coordinatewiseMultiply);
            normNode2Outputs.Append(squareNormNode2->output);
        }

        auto distNode1 = transformer.AddNode<BinaryOperationNode<double>>(normNode1Outputs, productNodeScaled->output, emitters::BinaryOperationType::add);
        auto distNode = transformer.AddNode<BinaryOperationNode<double>>(normNode2Outputs, distNode1->output, emitters::BinaryOperationType::add);

        transformer.MapNodeOutput(output, distNode->output);

        return true;
    }

    template <typename ValueType, math::MatrixLayout layout>
    void EuclideanDistanceNode<ValueType, layout>::Compute() const
    {
        math::ColumnVector<ValueType> input(_input.Size());
        for (size_t index = 0; index < _input.Size(); ++index)
        {
            input[index] = _input[index];
        }

        math::ColumnVector<ValueType> result(_w.NumRows());

        auto norm1sq = input.Norm2Squared();

        // result = _w * input
        math::Operations::Multiply(-2.0, _w, input, 0.0, result);

        for (size_t r = 0; r < _w.NumRows(); r++)
        {
            result[r] += norm1sq + _w.GetRow(r).Norm2Squared();
        }

        _output.SetOutput({ result.ToArray() });
    }

    template <typename ValueType, math::MatrixLayout layout>
    EuclideanDistanceNode<ValueType, layout>* AddNodeToModelTransformer(const model::PortElements<ValueType>& input, math::ConstMatrixReference<ValueType, layout> w, model::ModelTransformer& transformer)
    {
        return transformer.AddNode<EuclideanDistanceNode>(input, w);
    }
}
}
