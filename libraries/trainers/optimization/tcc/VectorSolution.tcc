////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     VectorSolution.tcc (optimization)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

// utilities
#include "Common.h"

// math
#include "VectorOperations.h"

namespace ell
{
namespace trainers
{
    namespace optimization
    {
        template <typename IOElementType, bool isBiased>
        void VectorSolution<IOElementType, isBiased>::Resize(const InputType& inputExample, OutputType)
        {
            _weights.Resize(inputExample.Size());

            if constexpr (!isDouble)
            {
                _doubleInput.Resize(inputExample.Size());
            }
        }

        template <typename IOElementType, bool isBiased>
        void VectorSolution<IOElementType, isBiased>::operator=(const VectorSolution<IOElementType, isBiased>& other)
        {
            _weights.CopyFrom(other._weights);

            if constexpr (isBiased)
            {
                _bias = other._bias;
            }
        }

        template <typename IOElementType, bool isBiased>
        void VectorSolution<IOElementType, isBiased>::operator=(SumExpression<ScaledExpression<VectorSolution<IOElementType, isBiased>>, ScaledExpression<VectorSolution<IOElementType, isBiased>>> expression)
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
            math::ScaleAddUpdate(otherScale, otherSolution.GetVector(), thisScale, _weights);

            if constexpr (isBiased)
            {
                _bias = thisScale * _bias + otherScale * otherSolution.GetBias();
            }
        }

        template <typename IOElementType, bool isBiased>
        void VectorSolution<IOElementType, isBiased>::operator=(SumExpression<ScaledExpression<VectorSolution<IOElementType, isBiased>>, ScaledColumnVectorExpression<IOElementType>> expression)
        {
            const auto& thisTerm = expression.lhs;
            const auto& updateTerm = expression.rhs;

            if (&(thisTerm.lhs.get()) != this)
            {
                throw OptimizationException("One of the terms should be a scaled version of this solution");
            }

            double thisScale = thisTerm.rhs;
            auto updateVector = updateTerm.lhs;
            double updateScale = updateTerm.rhs;

            if constexpr (isDouble)
            {
                math::ScaleAddUpdate(updateScale, updateVector, thisScale, _weights);
            }
            else
            {
                auto doubleColumnVector = _doubleInput.Transpose();
                doubleColumnVector.CopyFrom(updateVector);
                math::ScaleAddUpdate(updateScale, doubleColumnVector, thisScale, _weights);
            }

            if constexpr (isBiased)
            {
                _bias = thisScale * _bias + updateScale;
            }
        }

        template <typename IOElementType, bool isBiased>
        void VectorSolution<IOElementType, isBiased>::operator-=(const VectorSolution<IOElementType, isBiased>& other)
        {
            _weights -= other._weights;
            if constexpr (isBiased)
            {
                _bias -= other._bias;
            }
        }

        template <typename IOElementType, bool isBiased>
        void VectorSolution<IOElementType, isBiased>::operator+=(ScaledColumnVectorExpression<IOElementType> expression)
        {
            const auto& updateVector = expression.lhs;
            double updateScale = expression.rhs;

            if constexpr (isDouble)
            {
                math::ScaleAddUpdate(updateScale, updateVector, 1.0, _weights);
            }
            else
            {
                auto doubleColumnVector = _doubleInput.Transpose();
                doubleColumnVector.CopyFrom(updateVector);
                math::ScaleAddUpdate(updateScale, doubleColumnVector, 1.0, _weights);
            }

            if constexpr (isBiased)
            {
                _bias += updateScale;
            }
        }

        template <typename IOElementType, bool isBiased>
        double VectorSolution<IOElementType, isBiased>::Multiply(const InputType& input) const
        {
            double result;

            if constexpr (isDouble)
            {
                result = math::Dot(input, _weights);
            }
            else
            {
                _doubleInput.CopyFrom(input);
                result = math::Dot(_doubleInput, _weights);
            }

            if constexpr (isBiased)
            {
                result += _bias;
            }

            return result;
        }

        template <typename IOElementType, bool isBiased>
        double VectorSolution<IOElementType, isBiased>::GetNorm2SquaredOf(const InputType& input)
        {
            double result = input.Norm2Squared();

            if constexpr (isBiased)
            {
                result += 1.0;
            }

            return result;
        }

        template <typename IOElementType, bool isBiased>
        double Norm2Squared(const VectorSolution<IOElementType, isBiased>& solution)
        {
            double result = solution.GetVector().Norm2Squared();

            if constexpr (isBiased)
            {
                result += solution.GetBias() * solution.GetBias();
            }

            return result;
        }

        template <typename IOElementType, bool isBiased>
        double operator*(math::ConstRowVectorReference<IOElementType> input, const VectorSolution<IOElementType, isBiased>& solution)
        {
            return solution.Multiply(input);
        }
    } // namespace optimization
} // namespace trainers
} // namespace ell
