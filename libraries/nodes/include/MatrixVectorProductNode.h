////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MatrixVectorProductNode.h (nodes)
//  Authors:  Suresh Iyengar
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ConstantNode.h"
#include "MatrixVectorMultiplyNode.h"

#include <model/include/Model.h>
#include <model/include/ModelTransformer.h>
#include <model/include/Node.h>

#include <math/include/Matrix.h>
#include <math/include/MatrixOperations.h>

#include <utilities/include/Exception.h>

#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary> A node that represents a Matrix-Vector product. </summary>
    ///
    /// <typeparam name="ValueType"> The Matrix element type. </typeparam>
    /// <typeparam name="layout"> The Matrix layout. </typeparam>
    ///
    template <typename ValueType, math::MatrixLayout layout>
    class MatrixVectorProductNode : public model::Node
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        MatrixVectorProductNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The vector to multiply with the matrix </param>
        /// <param name="w"> The matrix </param>
        MatrixVectorProductNode(const model::OutputPort<ValueType>& input, const math::Matrix<ValueType, layout>& w);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName("MatrixVectorProductNode", { utilities::GetTypeName<ValueType>(), std::to_string(int(layout)) }); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Refines this node in the model being constructed by the transformer </summary>
        bool Refine(model::ModelTransformer& transformer) const override;

    protected:
        void Compute() const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
        bool HasState() const override { return true; }

    private:
        void Copy(model::ModelTransformer& transformer) const override;

        // Inputs
        model::InputPort<ValueType> _input;

        // Output
        model::OutputPort<ValueType> _output;

        // Projection matrix
        math::Matrix<ValueType, layout> _w;
    };

    /// <summary> Convenience function for adding a node to a model. </summary>
    ///
    /// <param name="input"> The vector to multiply with the matrix </param>
    /// <param name="w"> The matrix </param>
    ///
    /// <returns> The output of the new node. </returns>
    template <typename ValueType, math::MatrixLayout layout>
    const model::OutputPort<ValueType>& MatrixVectorProduct(const model::OutputPort<ValueType>& input, math::ConstMatrixReference<ValueType, layout> w);

    /// <summary> Adds a Matrix vector product node to a model transformer. </summary>
    ///
    /// <typeparam name="ValueType"> The Matrix element type. </typeparam>
    /// <typeparam name="layout"> The Matrix layout. </typeparam>
    /// <param name="input"> The input to the predictor. </param>
    /// <param name="w"> The Matrix. </param>
    /// <param name="transformer"> [in,out] The model transformer. </param>
    ///
    /// <returns> The node added to the model. </returns>
    template <typename ValueType, math::MatrixLayout layout>
    MatrixVectorProductNode<ValueType, layout>* AddNodeToModelTransformer(const model::PortElements<ValueType>& input, math::ConstMatrixReference<ValueType, layout> w, model::ModelTransformer& transformer);
} // namespace nodes
} // namespace ell

#pragma region implementation

namespace ell
{
namespace nodes
{
    template <typename ValueType, math::MatrixLayout layout>
    MatrixVectorProductNode<ValueType, layout>::MatrixVectorProductNode() :
        Node({ &_input }, { &_output }),
        _input(this, {}, defaultInputPortName),
        _output(this, defaultOutputPortName, 1),
        _w(0, 0)
    {
    }

    template <typename ValueType, math::MatrixLayout layout>
    MatrixVectorProductNode<ValueType, layout>::MatrixVectorProductNode(const model::OutputPort<ValueType>& input, const math::Matrix<ValueType, layout>& w) :
        Node({ &_input }, { &_output }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, w.NumRows()),
        _w(w)
    {
        if (input.Size() != w.NumColumns())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "MatrixVectorProductNode: input size must match the number of columns in the 'w' matrix");
        }
    }

    template <typename ValueType, math::MatrixLayout layout>
    void MatrixVectorProductNode<ValueType, layout>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);

        archiver["w_rows"] << _w.NumRows();
        archiver["w_columns"] << _w.NumColumns();
        std::vector<ValueType> temp;
        temp.assign(_w.GetConstDataPointer(), _w.GetConstDataPointer() + (size_t)(_w.NumRows() * _w.NumColumns()));
        archiver["w"] << temp;

        archiver[defaultInputPortName] << _input;
        archiver[defaultOutputPortName] << _output;
    }

    template <typename ValueType, math::MatrixLayout layout>
    void MatrixVectorProductNode<ValueType, layout>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);

        size_t w_rows = 0;
        size_t w_columns = 0;
        archiver["w_rows"] >> w_rows;
        archiver["w_columns"] >> w_columns;
        std::vector<ValueType> temp;
        archiver["w"] >> temp;
        _w = math::Matrix<ValueType, layout>(w_rows, w_columns, temp);

        archiver[defaultInputPortName] >> _input;
        archiver[defaultOutputPortName] >> _output;
    }

    template <typename ValueType, math::MatrixLayout layout>
    void MatrixVectorProductNode<ValueType, layout>::Copy(model::ModelTransformer& transformer) const
    {
        const auto& newPortElements = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<MatrixVectorProductNode<ValueType, layout>>(newPortElements, _w);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType, math::MatrixLayout layout>
    bool MatrixVectorProductNode<ValueType, layout>::Refine(model::ModelTransformer& transformer) const
    {
        const auto& newInput = transformer.GetCorrespondingInputs(_input);

        // Make sure we have a RowMatrix (because that's what MatrixVectorMultiplyNode wants)
        math::RowMatrix<ValueType> projectionMatrix(_w);
        auto m = projectionMatrix.NumRows();
        auto n = projectionMatrix.NumColumns();
        auto matrixStride = projectionMatrix.GetIncrement();
        if (matrixStride == 0 || matrixStride < n)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::badData, "Matrix has an invalid stride");
        }
        const auto& projectionMatrixOutput = Constant(transformer, projectionMatrix.ToArray());
        const auto& result = MatrixVectorMultiply(projectionMatrixOutput, m, n, matrixStride, newInput);
        transformer.MapNodeOutput(output, result);
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
        math::MultiplyScaleAddUpdate(static_cast<ValueType>(1), _w, input, static_cast<ValueType>(0), result);

        _output.SetOutput(result.ToArray());
    }

    template <typename ValueType, math::MatrixLayout layout>
    const model::OutputPort<ValueType>& MatrixVectorProduct(const model::OutputPort<ValueType>& input, math::ConstMatrixReference<ValueType, layout> w)
    {
        model::Model* model = input.GetNode()->GetModel();
        if (model == nullptr)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Input not part of a model");
        }

        auto node = model->AddNode<MatrixVectorProductNode<ValueType, layout>>(input, w);
        return node->output;
    }

    template <typename ValueType, math::MatrixLayout layout>
    MatrixVectorProductNode<ValueType, layout>* AddNodeToModelTransformer(const model::PortElements<ValueType>& input, math::ConstMatrixReference<ValueType, layout> w, model::ModelTransformer& transformer)
    {
        return transformer.AddNode<MatrixVectorProductNode>(input, w);
    }
} // namespace nodes
} // namespace ell

#pragma endregion implementation
