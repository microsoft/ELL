////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ParametersGenerator.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>

namespace common
{
    template <typename ParametersType, typename ...ValueTypes>
    class ParametersGenerator
    {
    public:

        using ValueVectorTupleType = std::tuple<std::vector<ParameterValueType>...>; // holds the vectors of parameter values to sweep over
        using ValueTupleType = std::tuple<ParameterValueType...>; // holds a concrete assignment of parameter values

        ParametersGenerator(ParametersType parametersPrototype, std::vector<ParameterValueType>... parameterValues);

        ParametersType GetParameters(size_t index) const; 

    private:
        ParametersType _parametersPrototype;
        ValueVectorTupleType _valueVectorTuple;
    };

}

#include "../tcc/ParameterGenerator.tcc"
