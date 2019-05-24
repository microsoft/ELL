////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MaskedMatrixSolution.h (optimization)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Common.h"
#include "Expression.h"
#include "IndexedContainer.h"
#include "MatrixSolution.h"
#include "OptimizationExample.h"

#include <math/include/Matrix.h>
#include <math/include/MatrixOperations.h>
#include <math/include/Vector.h>
#include <math/include/VectorOperations.h>

namespace ell
{
namespace optimization
{
    /// <summary> A matrix solution that applies to vector inputs and vector outputs. </summary>
    template <typename MatrixSolutionType>
    class MaskedMatrixSolution : public Scalable
    {
    public:
        using ElementType = typename MatrixSolutionType::ElementType;
        using InputType = typename MatrixSolutionType::InputType;
        using OutputType = typename MatrixSolutionType::OutputType;
        using AuxiliaryDoubleType = typename MatrixSolutionType::AuxiliaryDoubleType;
        using ExampleType = typename MatrixSolutionType::ExampleType;
        using DatasetType = typename MatrixSolutionType::DatasetType;
        using WeightsType = typename MatrixSolutionType::WeightsType;
        using MaskType = math::ColumnMatrix<float>;

        // <summary> The user-settable parameters for this solution type --- the weights which are to remain the same after optimization. </summary>
        struct ParametersType
        {
            MaskType mask = { 0, 0 };
            WeightsType frozenWeights = { 0, 0 };
        };

        /// <summary> Resize the solution to match the sizes of an input and an output. </summary>
        void Resize(const InputType& inputExample, const OutputType& outputExample);

        /// <summary> Resets the solution to zero. </summary>
        void Reset();

        /// <summary> Sets the solution parameters. </summary>
        void SetParameters(const ParametersType& parameters);

        /// <summary> Returns the matrix. </summary>
        math::ConstColumnMatrixReference<double> GetMatrix() const { return _baseSolution.GetMatrix(); }

        /// <summary> Returns the matrix. </summary>
        math::ColumnMatrixReference<double> GetMatrix() { return _baseSolution.GetMatrix(); }

        /// <summary> Returns a vector reference to the matrix. </summary>
        math::ConstColumnVectorReference<double> GetVector() const { return _baseSolution.GetVector(); }

        /// <summary> Returns a vector reference to the matrix. </summary>
        math::ColumnVectorReference<double> GetVector() { return _baseSolution.GetVector(); }

        /// <summary> Returns the bias. </summary>
        template <bool B = MatrixSolutionType::IsBiased, typename Concept = std::enable_if_t<B>>
        const math::RowVector<double>& GetBias() const
        {
            return _baseSolution.GetBias();
        }

        /// <summary> Returns the bias. </summary>
        template <bool B = MatrixSolutionType::IsBiased, typename Concept = std::enable_if_t<B>>
        math::RowVector<double>& GetBias()
        {
            return _baseSolution.GetBias();
        }

        /// <summary> Assignment operator. </summary>
        void operator=(const MaskedMatrixSolution<MatrixSolutionType>& other);

        /// <summary> Adds another scaled solution to a scaled version of this solution. </summary>
        void operator=(SumExpression<ScaledExpression<MaskedMatrixSolution<MatrixSolutionType>>, ScaledExpression<MaskedMatrixSolution<MatrixSolutionType>>> expression);

        /// <summary> Adds a scaled column vector to a scaled version of this solution. </summary>
        void operator=(SumExpression<ScaledExpression<MaskedMatrixSolution<MatrixSolutionType>>, OuterProductExpression<ElementType>> expression);

        /// <summary> Subtracts another solution from this one. </summary>
        void operator-=(const MaskedMatrixSolution<MatrixSolutionType>& other);

        /// <summary> Adds a scaled column vector to this solution. </summary>
        void operator+=(OuterProductExpression<ElementType> expression);

        /// <summary> Computes input * weights, or input * weights + bias (if a bias exists). </summary>
        math::RowVector<double> Multiply(const InputType& input) const;

        /// <summary> Returns the squared 2-norm of a given input. </summary>
        static double GetNorm2SquaredOf(const InputType& input);

        /// <summary> Initializes an auxiliary double variable. </summary>
        void InitializeAuxiliaryVariable(AuxiliaryDoubleType& aux);

        const MaskType& GetMask() const { return _mask; }
        const WeightsType& GetFrozenWeights() const { return _frozenWeights; }
        const MatrixSolutionType& GetBaseSolution() const { return _baseSolution; }

    private:
        void UpdateBaseSolution();

        MatrixSolutionType _baseSolution;
        MaskType _mask = { 0, 0 };
        WeightsType _frozenWeights = { 0, 0 };
    };

    /// <summary> Returns the squared 2-norm of a MatrixSolutionBase. </summary>
    template <typename MatrixSolutionType>
    double Norm2Squared(const MaskedMatrixSolution<MatrixSolutionType>& solution);

    /// <summary> vector-solution product. </summary>
    template <typename MatrixSolutionType>
    math::RowVector<double> operator*(typename MaskedMatrixSolution<MatrixSolutionType>::InputType input, const MaskedMatrixSolution<MatrixSolutionType>& solution);

    /// <summary> An unbiased matrix solution that applies to vector inputs and vector outputs. </summary>
    template <typename IOElementType>
    using UnbiasedMaskedMatrixSolution = MaskedMatrixSolution<UnbiasedMatrixSolution<IOElementType>>;

    /// <summary> A biased matrix solution that applies to vector inputs and vector outputs. </summary>
    template <typename IOElementType>
    using BiasedMaskedMatrixSolution = MaskedMatrixSolution<BiasedMatrixSolution<IOElementType>>;
} // namespace optimization
} // namespace ell

#pragma region implementation

namespace ell
{
namespace optimization
{
    template <typename MatrixSolutionType>
    void MaskedMatrixSolution<MatrixSolutionType>::Resize(const InputType& inputExample, const OutputType& outputExample)
    {
        _baseSolution.Resize(inputExample, outputExample);

        // resize mask matrix
        MaskType mask(inputExample.Size(), outputExample.Size());
        _mask.Swap(mask);

        WeightsType frozenWeights(inputExample.Size(), outputExample.Size());
        _frozenWeights.Swap(frozenWeights);
    }

    template <typename MatrixSolutionType>
    void MaskedMatrixSolution<MatrixSolutionType>::Reset()
    {
        _baseSolution.Reset();
    }

    template <typename MatrixSolutionType>
    void MaskedMatrixSolution<MatrixSolutionType>::SetParameters(const ParametersType& parameters)
    {
        _mask = parameters.mask;
        _frozenWeights = parameters.frozenWeights;
    }

    template <typename MatrixSolutionType>
    void MaskedMatrixSolution<MatrixSolutionType>::operator=(const MaskedMatrixSolution<MatrixSolutionType>& other)
    {
        _baseSolution = other.GetBaseSolution();
        _mask = other.GetMask();
        _frozenWeights = other.GetFrozenWeights();
    }

    template <typename MatrixSolutionType>
    void MaskedMatrixSolution<MatrixSolutionType>::operator=(SumExpression<ScaledExpression<MaskedMatrixSolution<MatrixSolutionType>>, ScaledExpression<MaskedMatrixSolution<MatrixSolutionType>>> expression)
    {
        const auto& thisTerm = expression.lhs; // a ScaledExpression
        const auto& otherTerm = expression.rhs; // a ScaledExpression

        if (&(thisTerm.lhs.get()) != this)
        {
            throw OptimizationException("First term should be a scaled version of this solution");
        }

        double thisScale = thisTerm.rhs;
        double otherScale = otherTerm.rhs;
        const auto& otherSolution = otherTerm.lhs.get();

        _baseSolution = (_baseSolution * thisScale) + (otherSolution * otherScale);
        UpdateBaseSolution();
    }

    template <typename MatrixSolutionType>
    void MaskedMatrixSolution<MatrixSolutionType>::operator=(SumExpression<ScaledExpression<MaskedMatrixSolution<MatrixSolutionType>>, OuterProductExpression<ElementType>> expression)
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
        auto expr = (_baseSolution * thisScale) + (columnVectorReference * rowVectorReference);

        static_assert(std::is_same_v<decltype(expr), SumExpression<ScaledExpression<MatrixSolutionType>, OuterProductExpression<ElementType>>>);
        _baseSolution = expr;
        UpdateBaseSolution();
    }

    template <typename MatrixSolutionType>
    void MaskedMatrixSolution<MatrixSolutionType>::operator-=(const MaskedMatrixSolution<MatrixSolutionType>& other)
    {
        _baseSolution -= other._baseSolution;
        UpdateBaseSolution();
    }

    template <typename MatrixSolutionType>
    void MaskedMatrixSolution<MatrixSolutionType>::operator+=(OuterProductExpression<ElementType> expression)
    {
        _baseSolution += expression;
        UpdateBaseSolution();
    }

    template <typename MatrixSolutionType>
    math::RowVector<double> MaskedMatrixSolution<MatrixSolutionType>::Multiply(const InputType& input) const
    {
        return _baseSolution.Multiply(input);
    }

    template <typename MatrixSolutionType>
    void MaskedMatrixSolution<MatrixSolutionType>::UpdateBaseSolution()
    {
        if (_mask.NumRows() == 0 || _mask.NumColumns() == 0)
        {
            return;
        }
        auto weights = _baseSolution.GetMatrix();
        if (weights.IsContiguous() && _mask.IsContiguous() && _frozenWeights.IsContiguous())
        {
            auto m = _mask.GetConstDataPointer();
            auto end = m + _mask.Size();
            auto f = _frozenWeights.GetConstDataPointer();
            auto w = weights.GetDataPointer();
            for (; m != end; ++m, ++f, ++w)
            {
                if (*m != 0)
                {
                    *w = *f;
                }
            }
        }
        else
        {
            auto numRows = weights.NumRows();
            auto numColumns = weights.NumColumns();
            for (size_t i = 0; i < numRows; ++i)
            {
                for (size_t j = 0; j < numColumns; ++j)
                {
                    if (_mask(i, j) != 0)
                    {
                        weights(i, j) = _frozenWeights(i, j);
                    }
                }
            }
        }
    }

    template <typename MatrixSolutionType>
    double MaskedMatrixSolution<MatrixSolutionType>::GetNorm2SquaredOf(const InputType& input)
    {
        return MatrixSolutionType::GetNorm2SquaredOf(input);
    }

    template <typename MatrixSolutionType>
    void MaskedMatrixSolution<MatrixSolutionType>::InitializeAuxiliaryVariable(AuxiliaryDoubleType& aux)
    {
        _baseSolution.InitializeAuxiliaryVariable(aux);
    }

    template <typename MatrixSolutionType>
    double Norm2Squared(const MaskedMatrixSolution<MatrixSolutionType>& solution)
    {
        return Norm2Squared(solution.GetBaseSolution());
    }

    template <typename MatrixSolutionType>
    math::RowVector<double> operator*(typename MaskedMatrixSolution<MatrixSolutionType>::InputType input, const MaskedMatrixSolution<MatrixSolutionType>& solution)
    {
        return solution.GetBaseSolution().Multiply(input);
    }
} // namespace optimization
} // namespace ell

#pragma endregion implementation
