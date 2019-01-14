////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SquaredEuclideanDistanceNode.h (nodes)
//  Authors:  Suresh Iyengar
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "BinaryOperationNode.h"
#include "ConstantNode.h"
#include "L2NormSquaredNode.h"
#include "MatrixVectorProductNode.h"
#include "SquaredEuclideanDistanceNode.h"
#include "UnaryOperationNode.h"

#include <utilities/include/Exception.h>

#include <model/include/Model.h>
#include <model/include/ModelTransformer.h>
#include <model/include/Node.h>

#include <math/include/Matrix.h>
#include <math/include/MatrixOperations.h>

#include <string>
#include <vector>

namespace ell
{
namespace nodes
{
    /// <summary> A node that represents squared Euclidean distances of input vector to a set of vectors stacked into rows of a matrix. </summary>
    ///
    /// <typeparam name="ValueType"> The Matrix element and input type. </typeparam>
    /// <typeparam name="layout"> The Matrix layout. </typeparam>
    ///
    template <typename ValueType, math::MatrixLayout layout>
    class SquaredEuclideanDistanceNode : public model::Node
    {
    public:
        /// @name Input and Output Ports
        /// @{
        const model::InputPort<ValueType>& input = _input;
        const model::OutputPort<ValueType>& output = _output;
        /// @}

        /// <summary> Default Constructor </summary>
        SquaredEuclideanDistanceNode();

        /// <summary> Constructor </summary>
        ///
        /// <param name="input"> The input vector </param>
        /// <param name="v"> The set of vectors to compute the distance from stacked into a row matrix</param>
        SquaredEuclideanDistanceNode(const model::OutputPort<ValueType>& input, const math::Matrix<ValueType, layout>& v);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "SquaredEuclideanDistanceNode"; }
        // static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType, layout>("SquaredEuclideanDistanceNode"); }

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

        // Set of vectors
        math::Matrix<ValueType, layout> _vectorsAsMatrix;
    };

    /// <summary> Adds a SquaredEuclidean node to a model transformer. </summary>
    ///
    /// <typeparam name="ValueType"> The Matrix element type. </typeparam>
    /// <typeparam name="layout"> The Matrix layout. </typeparam>
    /// <param name="input"> The input to the predictor. </param>
    /// <param name="w"> The Matrix. </param>
    /// <param name="transformer"> [in,out] The model transformer. </param>
    ///
    /// <returns> The node added to the model. </returns>
    template <typename ValueType, math::MatrixLayout layout>
    SquaredEuclideanDistanceNode<ValueType, layout>* AddNodeToModelTransformer(const model::PortElements<ValueType>& input, math::ConstMatrixReference<ValueType, layout> w, model::ModelTransformer& transformer);
} // namespace nodes
} // namespace ell

#pragma region implementation

namespace ell
{
namespace nodes
{
    template <typename ValueType, math::MatrixLayout layout>
    SquaredEuclideanDistanceNode<ValueType, layout>::SquaredEuclideanDistanceNode() :
        Node({ &_input }, { &_output }),
        _input(this, {}, defaultInputPortName),
        _output(this, defaultOutputPortName, 1),
        _vectorsAsMatrix(0, 0)
    {
    }

    template <typename ValueType, math::MatrixLayout layout>
    SquaredEuclideanDistanceNode<ValueType, layout>::SquaredEuclideanDistanceNode(const model::OutputPort<ValueType>& input, const math::Matrix<ValueType, layout>& vectorsAsMatrix) :
        Node({ &_input }, { &_output }),
        _input(this, input, defaultInputPortName),
        _output(this, defaultOutputPortName, vectorsAsMatrix.NumRows()),
        _vectorsAsMatrix(vectorsAsMatrix)
    {
        if (input.Size() != vectorsAsMatrix.NumColumns())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "SquaredEuclideanDistanceNode: input size must match the number of columns in the vectorsAsMatrix");
        }
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
        const auto& newPortElements = transformer.GetCorrespondingInputs(_input);
        auto newNode = transformer.AddNode<SquaredEuclideanDistanceNode<ValueType, layout>>(newPortElements, _vectorsAsMatrix);
        transformer.MapNodeOutput(output, newNode->output);
    }

    // We compute the distance (P - V)^2 as P^2 - 2 * P * V + V^2 where P is the input point and V is the set of vectors
    template <typename ValueType, math::MatrixLayout layout>
    bool SquaredEuclideanDistanceNode<ValueType, layout>::Refine(model::ModelTransformer& transformer) const
    {
        const auto& inputPortElements = transformer.GetCorrespondingInputs(_input);

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
            const auto& rowNorm2Squared = AppendConstant(transformer, static_cast<ValueType>(matrixRow.Norm2Squared()));
            vectorNorm2SquaredConstantNodeOutputs.Append(rowNorm2Squared);
        }

        // Add the three node outputs:
        //   * inputNorm2SquaredNodeOutputs (A)
        //   * vectorNorm2SquaredConstantNodeOutputs (B)
        //   * productNode->output (C)
        // and map it to output node
        auto& A = transformer.SimplifyOutputs(inputNorm2SquaredNodeOutputs);
        auto& B = transformer.SimplifyOutputs(vectorNorm2SquaredConstantNodeOutputs);
        auto& C = productNode->output;
        const auto& aPlusB = AppendBinaryOperation(A, B, BinaryOperationType::add);
        const auto& aPlusBPlusC = AppendBinaryOperation(aPlusB, C, BinaryOperationType::add);
        transformer.MapNodeOutput(output, aPlusBPlusC);

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
} // namespace nodes
} // namespace ell

#pragma endregion implementation
