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
}
}
