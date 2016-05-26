////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ParametersGenerator.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include <tuple>

namespace common
{
    template <typename ParametersType, typename ...ValueTypes>
    class ParametersGenerator
    {
    public:

        using ValueVectorTupleType = std::tuple<std::vector<ValueTypes>...>; // holds the vectors of parameter values to sweep over
        using ValueTupleType = std::tuple<ValueTypes...>; // holds a concrete assignment of parameter values

        ParametersGenerator(std::vector<ValueTypes>... parameterValues);

        size_t Size() const;

        ParametersType GenerateParameters(size_t index) const; 

        std::vector<ParametersType> GenerateParametersVector() const;

    private:
        template <size_t Index = 0>
        void SetValueTuple(ValueTupleType& valueTuple, size_t index) const;

        // termination of template recursion
        template <>
        void SetValueTuple<std::tuple_size<ValueTupleType>::value>(ValueTupleType& valueTuple, size_t index) const {}

        template <size_t... Sequence>
        ParametersType GenerateParameters(const ValueTupleType& valueTuple, std::index_sequence<Sequence...>) const;

        template <size_t... Sequence>
        size_t Size(std::index_sequence<Sequence...>) const;

        // members
        ValueVectorTupleType _valueVectorTuple;
    };

    template <typename ParametersType, typename... ValueTypes>
    ParametersGenerator<ParametersType, ValueTypes...> MakeParametersGenerator(std::vector<ValueTypes>... parameterValues);
}


#include "../tcc/ParametersGenerator.tcc"
