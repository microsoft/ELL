////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TupleUtils.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <tuple>
#include <utility> // for integer_sequence

namespace ell
{
namespace utilities
{
    //
    // Extracting the tail of a tuple
    //

    template <typename T>
    struct TupleTailImpl; // undefined

    template <typename FirstType, typename... RestTypes>
    struct TupleTailImpl<std::tuple<FirstType, RestTypes...>>
    {
        typedef std::tuple<RestTypes...> type;
    };

    template <typename TupleType>
    using TupleTailType = typename TupleTailImpl<TupleType>::type;

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
        // STYLE discrepancy
        // fails if Wrapper<T> has no copy constructor
        return typename TupleOfWrappedElements<WrapperType, typename std::tuple_element<Sequence, TupleType>::type...>::type{};
    }

    template <typename TupleType, template <typename> class WrapperType>
    static auto MakeWrappedTuple(const TupleType& tuple)
    {
        // STYLE discrepancy
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
        // STYLE discrepancy
        return WrappedType{};
    }

    template <typename WrappedType, template <typename> class WrapperType>
    auto UnwrapType(WrapperType<WrappedType> x)
    {
        // STYLE discrepancy
        return WrappedType{};
    }

    template <typename... WrappedTypes>
    auto UnwrapTuple(const std::tuple<WrappedTypes...>& elements)
    {
        // STYLE discrepancy
        return std::tuple<decltype(UnwrapType(WrappedTypes{}))...>{};
    }

    template <typename WrappedTupleType>
    struct UnwrappedTuple
    {
        using type = decltype(UnwrapTuple(WrappedTupleType{}));
    };

    template <typename WrappedTupleType>
    using UnwrappedTupleType = typename UnwrappedTuple<WrappedTupleType>::type;
}
}
