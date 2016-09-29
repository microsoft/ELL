////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     FunctionUtils.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace emll
{
namespace utilities
{
    template <typename Function, typename... Functions>
    void InOrderFunctionEvaluator(Function&& function, Functions&&... functions)
    {
        function();
        InOrderFunctionEvaluator(std::forward<Functions>(functions)...);
    }
}
}
