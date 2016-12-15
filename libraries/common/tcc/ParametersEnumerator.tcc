////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ParameterGenerator.tcc (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace common
{
    template <typename ParametersType, typename... ValueTypes>
    ParametersType ParametersEnumerator<ParametersType, ValueTypes...>::GenerateParameters(size_t index) const
    {
        ValueTupleType valueTuple;
        SetValueTuple(valueTuple, index);
        return GenerateParameters(valueTuple, std::make_index_sequence<std::tuple_size<ValueTupleType>::value>());
    }

    template <typename ParametersType, typename... ValueTypes>
    std::vector<ParametersType> ParametersEnumerator<ParametersType, ValueTypes...>::GenerateParametersVector() const
    {
        std::vector<ParametersType> vector;
        auto size = Size();
        for (size_t index = 0; index < size; ++index)
        {
            vector.push_back(GenerateParameters(index));
        }
        return vector;
    }

    template <typename ParametersType, typename... ValueTypes>
    template <size_t Index>
    void ParametersEnumerator<ParametersType, ValueTypes...>::SetValueTuple(ValueTupleType& valueTuple, size_t index) const
    {
        const auto& values = std::get<Index>(_valueVectorTuple);
        std::get<Index>(valueTuple) = values[index % values.size()];
        SetValueTuple<Index + 1>(valueTuple, index / values.size());
    }

    template <typename ParametersType, typename... ValueTypes>
    template <size_t... Sequence>
    ParametersType ParametersEnumerator<ParametersType, ValueTypes...>::GenerateParameters(const ValueTupleType& valueTuple, std::index_sequence<Sequence...>) const
    {
        return ParametersType{ std::get<Sequence>(valueTuple)... };
    }
}
}
