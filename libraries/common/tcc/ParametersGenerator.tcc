#include "..\include\ParametersGenerator.h"
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ParameterGenerator.tcc (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace common
{
    template<typename ParametersType, typename ...ValueTypes>
    ParametersGenerator<ParametersType, ValueTypes...>::ParametersGenerator(ParametersType parametersPrototype, std::vector<ValueTypes>... valueVectors) :
        _parametersPrototype(parametersPrototype), _parameterValueTuple(std::make_tuple(valueVectors...))
    {}

    template<typename ParametersType, typename ...ValueTypes>
    ParametersType ParametersGenerator<ParametersType, ...ValueTypes>::GetParameters(size_t index) const
    {
        ValueTupleType valueTuple;
//        SetValueTuple<0>(valueTuple, index);
//       return MakeParameters(valueTuple, std::make_index_sequence<tuple_size<ValueTupleType>::value>());
        return ParametersType();
    }

}
