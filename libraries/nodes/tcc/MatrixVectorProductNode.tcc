////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MatrixVectorProductNode.tcc (nodes)
//  Authors:  Suresh Iyengar
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MatrixVectorProductNode.h"
#include "MatrixVectorMultiplyNode.h"

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
    MatrixVectorProductNode<ValueType, layout>::MatrixVectorProductNode()
        : Node({ &_input }, { &_output }), _input(this, {}, inputPortName), _output(this, outputPortName, 1), _w(0, 0)
    {
    }

    template <typename ValueType, math::MatrixLayout layout>
    MatrixVectorProductNode<ValueType, layout>::MatrixVectorProductNode(const model::PortElements<ValueType>& input, const math::Matrix<ValueType, layout>& w)
        : Node({ &_input }, { &_output }), _input(this, input, inputPortName), _output(this, outputPortName, w.NumRows()), _w(w)
    {
        assert(input.Size() == w.NumColumns());
    }

    template <typename ValueType, math::MatrixLayout layout>
    void MatrixVectorProductNode<ValueType, layout>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);

        archiver["w_rows"] << _w.NumRows();
        archiver["w_columns"] << _w.NumColumns();
        std::vector<double> temp;
        temp.assign(_w.GetConstDataPointer(), _w.GetConstDataPointer() + (size_t)(_w.NumRows() * _w.NumColumns()));
        archiver["w"] << temp;

        archiver[inputPortName] << _input;
        archiver[outputPortName] << _output;
    }

    template <typename ValueType, math::MatrixLayout layout>
    void MatrixVectorProductNode<ValueType, layout>::ReadFromArchive(utilities::Unarchiver& archiver)
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
    void MatrixVectorProductNode<ValueType, layout>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(_input.GetPortElements());
        auto newNode = transformer.AddNode<MatrixVectorProductNode<double, layout>>(newPortElements, _w);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType, math::MatrixLayout layout>
    bool MatrixVectorProductNode<ValueType, layout>::Refine(model::ModelTransformer& transformer) const
    {
        auto newInput = transformer.TransformPortElements(_input.GetPortElements());

        // Make sure we have a RowMatrix (because that's what MatrixVectorMultiplyNode wants)
        math::RowMatrix<ValueType> projectionMatrix(_w);
        auto projectionMatrixValue = projectionMatrix.ToArray();
        auto projectionMatrixNode = transformer.AddNode<ConstantNode<ValueType>>(projectionMatrixValue);
        auto m = projectionMatrix.NumRows();
        auto n = projectionMatrix.NumColumns();
        auto matrixStride = projectionMatrix.GetIncrement();
        if(matrixStride == 0 || matrixStride < m)
        {
            utilities::InputException(utilities::InputExceptionErrors::badData, "Matrix has an invalid stride");
        }
        auto matrixMultiplyNode = transformer.AddNode<MatrixVectorMultiplyNode<ValueType>>(projectionMatrixNode->output, m, n, matrixStride, newInput);
        transformer.MapNodeOutput(output, matrixMultiplyNode->output);
        return true;
    }

    template <typename ValueType, math::MatrixLayout layout>
    void MatrixVectorProductNode<ValueType, layout>::Compute() const
    {
        math::ColumnVector<ValueType> input(_input.Size());
        for (size_t index = 0; index < _input.Size(); ++index)
        {
            input[index] = _input[index];
        }

        math::ColumnVector<ValueType> result(_w.NumRows());

        // result = _w * data
        math::Multiply(1.0, _w, input, 0.0, result);

        _output.SetOutput(result.ToArray());
    }

    template <typename ValueType, math::MatrixLayout layout>
    MatrixVectorProductNode<ValueType, layout>* AddNodeToModelTransformer(const model::PortElements<ValueType>& input, math::ConstMatrixReference<ValueType, layout> w, model::ModelTransformer& transformer)
    {
        return transformer.AddNode<MatrixVectorProductNode>(input, w);
    }
}
}
