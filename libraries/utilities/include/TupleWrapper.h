////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     TupleWrapper.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <tuple>
#include <utility> // for integer_sequence

namespace emll
{
namespace utilities
{
    //
    // General "wrap tuple types" mechanism
    //
    template <template <typename> class WrapperType, typename... Types>
    struct TupleOfWrappedElements
    {
        using type = std::tuple<WrapperType<Types>...>;
    };

    template <typename TupleType, template <typename> class WrapperType, size_t... Sequence>
    static auto MakeWrappedTupleHelper(const TupleType& tuple, std::index_sequence<Sequence...>)
    {
        // fails if Wrapper<T> has no copy constructor
        return typename TupleOfWrappedElements<WrapperType, typename std::tuple_element<Sequence, TupleType>::type...>::type{};
    }

    template <typename TupleType, template <typename> class WrapperType>
    static auto MakeWrappedTuple(const TupleType& tuple)
    {
        // Note: fails if Wrapper<T> has no copy constructor
        return MakeWrappedTupleHelper<TupleType, WrapperType>(tuple, std::make_index_sequence<std::tuple_size<TupleType>::value>());
    }

    template <typename TupleType, template <typename> class WrapperType>
    struct TupleTypeWrapper
    {
        using type = decltype(MakeWrappedTuple<TupleType, WrapperType>(TupleType{}));
    };

    template <typename TupleType, template <typename> class WrapperType>
    using WrappedTuple = typename TupleTypeWrapper<TupleType, WrapperType>::type;

    //
    // Unwrapping tuples
    //

    template <typename WrappedType, template <typename> class WrapperType>
    auto UnwrapType(WrapperType<WrappedType>* x)
    {
        return WrappedType{};
    }

    template <typename WrappedType, template <typename> class WrapperType>
    auto UnwrapType(WrapperType<WrappedType> x)
    {
        return WrappedType{};
    }

    template <typename... WrappedTypes>
    auto UnwrapTuple(const std::tuple<WrappedTypes...>& elements)
    {
        return std::tuple<decltype(UnwrapType(WrappedTypes{}))...>{};
    }

    template <typename WrappedTupleType>
    struct UnwrappedTuple
    {
        using type = decltype(UnwrapTuple(WrappedTupleType{}));
    };

    template <typename WrappedTupleType>
    using UnwrappedTupleType = typename UnwrappedTuple<WrappedTupleType>::type;


    // TODO: move this to someplace else
    void EvalInOrder() {}

    template <typename Lambda, typename... Lambdas>
    void EvalInOrder(Lambda&& lambda, Lambdas&&... lambdas)
    {
        lambda();
        EvalInOrder(std::forward<Lambdas>(lambdas)...);
    }
}
}
