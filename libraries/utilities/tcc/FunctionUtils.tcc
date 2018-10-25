////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     FunctionUtils.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace utilities
{
    template <typename Function, typename... Functions>
    void InOrderFunctionEvaluator(Function&& function, Functions&&... functions)
    {
        function();
        InOrderFunctionEvaluator(std::forward<Functions>(functions)...);
    }

    template <typename FunctionType, typename ArgType, typename... ArgTypes>
    void ApplyToEach(FunctionType&& function, ArgType&& arg, ArgTypes&&... args)
    {
        function(std::forward<ArgType>(arg));
        ApplyToEach(std::forward<FunctionType>(function), std::forward<ArgTypes>(args)...);
    }

    template <size_t Index>
    struct IndexTag
    {
        static constexpr size_t index = Index;
    };

    template <size_t Index>
    constexpr size_t GetTagIndex(IndexTag<Index> tag)
    {
        return Index;
    }

    template <typename FunctionType>
    FunctionArgTypes<FunctionType> GetFunctionArgTuple(FunctionType& function)
    {
        return FunctionArgTypes<FunctionType>{};
    }

    template <typename FunctionType, typename... Args>
    auto ApplyFunction(const FunctionType& function, Args... args) -> FunctionReturnType<FunctionType>
    {
        return function(args...);
    }

    template <typename FunctionType, typename... Args, std::size_t... Sequence>
    auto ApplyFunctionHelper(const FunctionType& function, std::tuple<Args...>&& args, std::index_sequence<Sequence...>) -> FunctionReturnType<FunctionType>
    {
        return function(std::get<Sequence>(args)...);
    }

    template <typename FunctionType, typename... Args>
    auto ApplyFunction(const FunctionType& function, std::tuple<Args...>&& args) -> FunctionReturnType<FunctionType>
    {
        return ApplyFunctionHelper(function, args, std::make_index_sequence<sizeof...(Args)>());
    }

    namespace detail
    {
        template <typename... Args, typename T, size_t... I>
        std::tuple<Args...> VectorToTuple(std::vector<T> t, std::index_sequence<I...>)
        {
            return { t[I]... };
        }
    }

    template <typename... Args, typename T>
    std::tuple<Args...> VectorToTuple(std::vector<T> t)
    {
        return detail::VectorToTuple<Args...>(t, std::make_index_sequence<sizeof...(Args)>());
    }

}
}
