////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MatrixSolution.tcc (optimization)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

// utilities
#include "Common.h"

// math
#include "VectorOperations.h"
#include "MatrixOperations.h"

namespace ell
{
namespace trainers
{
namespace optimization
{
    template <typename IOElementType, bool isBiased>
    void MatrixSolution<IOElementType, isBiased>::Resize(const InputType& inputExample, const OutputType& outputExample)
    {
        math::ColumnMatrix<double> matrix(inputExample.Size(), outputExample.Size());
        _weights.Swap(matrix);

        if constexpr (!isDouble)
        {
            _doubleInput.Resize(inputExample.Size());
        }

        if constexpr (isBiased)
        {
            _bias.Resize(outputExample.Size());
        }
    }

    template <typename IOElementType, bool isBiased>
    void MatrixSolution<IOElementType, isBiased>::operator=(const MatrixSolution<IOElementType, isBiased>& other)
    {
        _weights.CopyFrom(other._weights);
        
        if constexpr (isBiased)
        {
            _bias.CopyFrom(other._bias);
        }
    }

    template <typename IOElementType, bool isBiased>
    void MatrixSolution<IOElementType, isBiased>::operator=(SumExpression<ScaledExpression<MatrixSolution<IOElementType, isBiased>>, ScaledExpression<MatrixSolution<IOElementType, isBiased>>> expression)
    {
        const auto& thisTerm = expression.lhs;
        const auto& otherTerm = expression.rhs;

        if (&(thisTerm.lhs.get()) != this)
        {
            throw OptimizationException("First term should be a scaled version of this solution");
        }

        double thisScale = thisTerm.rhs;
        const auto& otherSolution = otherTerm.lhs.get();
        double otherScale = otherTerm.rhs;
        math::ScaleAddUpdate(otherScale, otherSolution._weights, thisScale, _weights);

        if constexpr (isBiased)
        {
            math::ScaleAddUpdate(otherScale, otherSolution.GetBias(), thisScale, _bias);
        }
    }

    template <typename IOElementType, bool isBiased>
    void MatrixSolution<IOElementType, isBiased>::operator=(SumExpression<ScaledExpression<MatrixSolution<IOElementType, isBiased>>, OuterProductExpression<IOElementType>> expression)
    {
        const auto& thisTerm = expression.lhs;
        const auto& updateTerm = expression.rhs;

        if (&(thisTerm.lhs.get()) != this)
        {
            throw OptimizationException("The first term should be a scaled version of this solution");
        }

        double thisScale = thisTerm.rhs;
        const auto& columnVectorReference = updateTerm.lhs;
        const auto& rowVectorReference = updateTerm.rhs;
        _weights *= thisScale;

        if constexpr (isDouble)
        {
            math::RankOneUpdate(1.0, columnVectorReference, rowVectorReference, _weights);
        }
        else
        {
            auto doubleColumnVector = _doubleInput.Transpose();
            doubleColumnVector.CopyFrom(columnVectorReference);
            math::RankOneUpdate(1.0, doubleColumnVector, rowVectorReference, _weights);
        }

        if constexpr (isBiased)
        {
            math::ScaleAddUpdate(1.0, rowVectorReference, thisScale, _bias);
        }
    }

    template <typename IOElementType, bool isBiased>
    void MatrixSolution<IOElementType, isBiased>::operator-=(const MatrixSolution<IOElementType, isBiased>& other)
    {
        _weights -= other._weights;
        if constexpr (isBiased)
        {
            _bias -= other._bias;
        }
    }

    template <typename IOElementType, bool isBiased>
    void MatrixSolution<IOElementType, isBiased>::operator+=(OuterProductExpression<IOElementType> expression)
    {
        const auto& columnVectorReference = expression.lhs;
        const auto& rowVectorReference = expression.rhs;

        if constexpr (isDouble)
        {
            math::RankOneUpdate(1.0, columnVectorReference, rowVectorReference, _weights);
        }
        else
        {
            auto doubleColumnVector = _doubleInput.Transpose();
            doubleColumnVector.CopyFrom(columnVectorReference);
            math::RankOneUpdate(1.0, doubleColumnVector, rowVectorReference, _weights);
        }

        if constexpr (isBiased)
        {
            math::ScaleAddUpdate(1.0, rowVectorReference, 1.0, _bias);
        }
    }

    template <typename IOElementType, bool isBiased>
    math::RowVector<double> MatrixSolution<IOElementType, isBiased>::Multiply(const InputType& input) const
    {
        math::RowVector<double> result(_weights.NumColumns());

        if constexpr (isBiased)
        {
            result.CopyFrom(_bias);
        }

        if constexpr (isDouble)
        {
            math::MultiplyScaleAddUpdate(1.0, input, _weights, 1.0, result);
        }
        else
        {
            _doubleInput.CopyFrom(input);
            math::MultiplyScaleAddUpdate(1.0, _doubleInput, _weights, 1.0, result);
        }

        return result;
    }

    template <typename IOElementType, bool isBiased>
    double MatrixSolution<IOElementType, isBiased>::GetNorm2SquaredOf(const InputType& input) 
    { 
        double result = input.Norm2Squared(); 

        if constexpr (isBiased)
        {
            result += 1.0;
        }

        return result;
    }

    template <typename IOElementType, bool isBiased>
    void MatrixSolution<IOElementType, isBiased>::InitializeAuxiliaryVariable(AuxiliaryDoubleType& aux) 
    { 
        aux.Resize(_weights.NumColumns()); aux.Reset(); 
    }

    template <typename IOElementType, bool isBiased>
    double Norm2Squared(const MatrixSolution<IOElementType, isBiased>& solution)
    {
        double result = solution.GetMatrix().ReferenceAsVector().Norm2Squared();

        if constexpr (isBiased)
        {
            result += solution.GetBias().Norm2Squared();
        }

        return result;
    }

    template <typename IOElementType, bool isBiased>
    math::RowVector<double> operator*(math::ConstRowVectorReference<IOElementType> input, const MatrixSolution<IOElementType, isBiased>& solution)
    {
        return solution.Multiply(input);
    }
}
}
}
