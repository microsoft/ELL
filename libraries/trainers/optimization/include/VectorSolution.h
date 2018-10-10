////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     VectorSolution.h (optimization)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "OptimizationExample.h"
#include "Expression.h"
#include "IndexedContainer.h"

// math
#include "Vector.h"
#include "VectorOperations.h"

// stl
#include <type_traits>

namespace ell
{
namespace trainers
{
namespace optimization
{
    /// <summary> An vector solution that applies to vector inputs and scalar outputs. </summary>
    template <typename IOElementType, bool isBiased = false>
    class VectorSolution : public Scalable
    {
    public:
        using InputType = math::ConstRowVectorReference<IOElementType>;
        using OutputType = IOElementType;
        using AuxiliaryDoubleType = double;
        using ExampleType = Example<InputType, OutputType>;
        using ExampleSetType = IndexedContainer<ExampleType>;

        /// <summary> Resize the solution to match the sizes of an input and an output. </summary>
        void Resize(const InputType& inputExample, OutputType);

        /// <summary> Returns the vector. </summary>
        math::ConstColumnVectorReference<double> GetVector() const { return _weights; }

        /// <summary> Returns the bias. </summary>
        template <bool B = isBiased, typename Concept = std::enable_if_t<B>>
        double GetBias() const { return _bias; }

        /// <summary> Assignment operator. </summary>
        void operator=(const VectorSolution<IOElementType, isBiased>& other);

        /// <summary> Adds another scaled solution to a scaled version of this solution. </summary>
        void operator=(SumExpression<ScaledExpression<VectorSolution<IOElementType, isBiased>>, ScaledExpression<VectorSolution<IOElementType, isBiased>>> expression);

        /// <summary> Adds a scaled column vector to a scaled version of this solution. </summary>
        void operator=(SumExpression<ScaledExpression<VectorSolution<IOElementType, isBiased>>, ScaledColumnVectorExpression<IOElementType>> expression);

        /// <summary> Adds a scaled column vector to this solution. </summary>
        void operator+=(ScaledColumnVectorExpression<IOElementType> expression);

        /// <summary> Computes input * weights, or input * weights + bias (if a bias exists). </summary>
        double Multiply(const InputType& input) const;

        /// <summary> Returns the squared 2-norm of a given input. </summary>
        static double GetNorm2SquaredOf(const InputType& input);

        /// <summary> Initializes an auxiliary double variable. </summary>
        void InitializeAuxiliaryVariable(AuxiliaryDoubleType& aux) { aux = 0; }

    private:
        math::ColumnVector<double> _weights;

        struct Nothing{};

        // if the solution is biased, allocate a bias term
        std::conditional_t<isBiased, double, Nothing> _bias = {};

        // if the IO element type is not double, allocate a double row vector
        static constexpr bool isDouble = std::is_same_v<IOElementType, double>;
        mutable std::conditional_t<isDouble, Nothing, math::RowVector<double>> _doubleInput;
    };

    /// <summary> Returns the 1-norm of a VectorSolutionBase. </summary>
    template <typename IOElementType, bool isBiased>
    double Norm1(const VectorSolution<IOElementType, isBiased>& solution);

    /// <summary> Returns the squared 2-norm of a VectorSolutionBase. </summary>
    template <typename IOElementType, bool isBiased>
    double Norm2Squared(const VectorSolution<IOElementType, isBiased>& solution);

    /// <summary> vector-solution product. </summary>
    template <typename IOElementType, bool isBiased>
    double operator*(math::ConstRowVectorReference<IOElementType> input, const VectorSolution<IOElementType, isBiased>& solution);
}
}
}

#include "../tcc/VectorSolution.tcc"
