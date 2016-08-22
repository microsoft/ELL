////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ParametersEnumerator.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include <tuple>

namespace common
{
    /// <summary> Implements a factory class that enumerates all possible combinations of parameter values. </summary>
    ///
    /// <typeparam name="ParametersType"> The parameters struct type. </typeparam>
    /// <typeparam name="ValueTypes"> Types of the individual variables in the parameters struct. </typeparam>
    template <typename ParametersType, typename... ValueTypes>
    class ParametersEnumerator
    {
    public:
        using ValueVectorTupleType = std::tuple<std::vector<ValueTypes>...>; // holds the vectors of parameter values to sweep over
        using ValueTupleType = std::tuple<ValueTypes...>; // holds a concrete assignment of parameter values

        /// <summary> Constructs an instance of ParametersEnumerator. </summary>
        ///
        /// <param name="parameterValues"> Variable number of parameter value vectors. The number and order of these types should match the constructor of ParametersType. </param>
        ParametersEnumerator(std::vector<ValueTypes>... parameterValues);

        /// <summary> Gets the number of different parameter configurations. </summary>
        ///
        /// <returns> The size. </returns>
        size_t Size() const;

        /// <summary> Generates the desired a parameters struct.</summary>
        ///
        /// <param name="index"> Zero-based index of the desired set of parameters. This parameter is interpreted modulo Size(), so all inputs produce valid output.</param>
        ///
        /// <returns> The desired parameters struct. </returns>
        ParametersType GenerateParameters(size_t index) const;

        /// <summary> Generates all possible parameters structs. </summary>
        ///
        /// <returns> A vector of all possible parameters. </returns>
        std::vector<ParametersType> GenerateParametersVector() const;

    private:
        template <size_t Index = 0>
        void SetValueTuple(ValueTupleType& valueTuple, size_t index) const;

        // termination of template recursion
        void SetValueTuple(ValueTupleType& valueTuple, size_t index) const {}

        template <size_t... Sequence>
        ParametersType GenerateParameters(const ValueTupleType& valueTuple, std::index_sequence<Sequence...>) const;

        template <size_t... Sequence>
        size_t Size(std::index_sequence<Sequence...>) const;

        // members
        ValueVectorTupleType _valueVectorTuple;
    };

    /// <summary> Makes a parameters generator. </summary>
    ///
    /// <typeparam name="ParametersType"> The parameters struct type. </typeparam>
    /// <typeparam name="ValueTypes"> Types of the individual variables in the parameters struct. </typeparam>
    /// <param name="parameterValues"> Variable number of parameter value vectors. The number and order of these types should match the constructor of ParametersType. </param>
    /// <returns> A parameters generator. </returns>
    template <typename ParametersType, typename... ValueTypes>
    ParametersEnumerator<ParametersType, ValueTypes...> MakeParametersEnumerator(std::vector<ValueTypes>... parameterValues);
}

#include "../tcc/ParametersEnumerator.tcc"
