////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     AbstractInvoker.tcc (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Exception.h"

namespace emll
{
namespace utilities
{
    template <typename BaseType, typename DerivedType, typename... DerivedTypes>
    template <typename FunctorType>
    auto AbstractInvoker<BaseType, DerivedType, DerivedTypes...>::Invoke(const FunctorType& functor, const BaseType& abstractArgument) -> typename FunctorType::ReturnType
    {
        const DerivedType* ptr = dynamic_cast<const DerivedType*>(&abstractArgument);
        if (ptr != nullptr)
        {
            return functor(*ptr);
        }
        else
        {
            return AbstractInvoker<BaseType, DerivedTypes...>::Invoke(functor, abstractArgument);
        }
    }

    template <typename BaseType>
    template <typename FunctorType>
    auto AbstractInvoker<BaseType>::Invoke(const FunctorType& functor, const BaseType& abstractArgument) -> typename FunctorType::ReturnType
    {
        throw LogicException(LogicExceptionErrors::illegalState, "base type reference could not be matched with a derived type");
    }
}
}
