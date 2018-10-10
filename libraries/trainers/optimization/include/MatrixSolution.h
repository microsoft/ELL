////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MatrixSolution.h (optimization)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "OptimizationExample.h"
#include "Expression.h"
#include "IndexedContainer.h"

// math
#include "Matrix.h"
#include "Vector.h"
#include "VectorOperations.h"

namespace ell
{
namespace trainers
{
namespace optimization
{
    /// <summary> An unbiased matrix solution that applies to vector inputs and vector outputs. </summary>
    template <typename IOElementType, bool isBiased = false>
    class MatrixSolution : public Scalable
    {
    public:
        using InputType = math::ConstRowVectorReference<IOElementType>;
        using OutputType = math::ConstRowVectorReference<IOElementType>;
        using AuxiliaryDoubleType = math::RowVector<double>;
        using ExampleType = Example<InputType, OutputType>;
        using ExampleSetType = IndexedContainer<ExampleType>;

        /// <summary> Resize the solution to match the sizes of an input and an output. </summary>
        void Resize(const InputType& inputExample, const OutputType& outputExample);

        /// <summary> Returns the matrix. </summary>
        math::ConstColumnMatrixReference<double> GetMatrix() const { return _weights; }

        /// <summary> Returns the bias. </summary>
        template <bool B = isBiased, typename Concept = std::enable_if_t<B>>
        const math::RowVector<double>& GetBias() const { return _bias; }

        /// <summary> Assignment operator. </summary>
        void operator=(const MatrixSolution<IOElementType, isBiased>& other);

        /// <summary> Adds another scaled solution to a scaled version of this solution. </summary>
        void operator=(SumExpression<ScaledExpression<MatrixSolution<IOElementType, isBiased>>, ScaledExpression<MatrixSolution<IOElementType, isBiased>>> expression);

        /// <summary> Adds a scaled column vector to a scaled version of this solution. </summary>
        void operator=(SumExpression<ScaledExpression<MatrixSolution<IOElementType, isBiased>>, OuterProductExpression<IOElementType>> expression);

        /// <summary> Adds a scaled column vector to this solution. </summary>
        void operator+=(OuterProductExpression<IOElementType> expression);

        /// <summary> Computes input * weights, or input * weights + bias (if a bias exists). </summary>
        math::RowVector<double> Multiply(const InputType& input) const;

        /// <summary> Returns the squared 2-norm of a given input. </summary>
        static double GetNorm2SquaredOf(const InputType& input);

        /// <summary> Initializes an auxiliary double variable. </summary>
        void InitializeAuxiliaryVariable(AuxiliaryDoubleType& aux);

    private:
        math::ColumnMatrix<double> _weights = { 0,0 };

        struct Nothing {};

        // if the solution is biased, allocate a bias term
        std::conditional_t<isBiased, math::RowVector<double>, Nothing> _bias = {};

        // if the IO element type is not double, allocate a double row vector
        static constexpr bool isDouble = std::is_same_v<IOElementType, double>;
        mutable std::conditional_t<isDouble, Nothing, math::RowVector<double>> _doubleInput;
    };

    /// <summary> Returns the 1-norm of a MatrixSolutionBase. </summary>
    template <typename IOElementType, bool isBiased>
    double Norm1(const MatrixSolution<IOElementType, isBiased>& solution);

    /// <summary> Returns the squared 2-norm of a MatrixSolutionBase. </summary>
    template <typename IOElementType, bool isBiased>
    double Norm2Squared(const MatrixSolution<IOElementType, isBiased>& solution);

    /// <summary> vector-solution product. </summary>
    template <typename IOElementType, bool isBiased>
    math::RowVector<double> operator*(math::ConstRowVectorReference<IOElementType> input, const MatrixSolution<IOElementType, isBiased>& solution);
}
}
}

#include "../tcc/MatrixSolution.tcc"


