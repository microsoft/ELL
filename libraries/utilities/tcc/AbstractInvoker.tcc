////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     AbstractInvoker.tcc (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Exception.h"

namespace ell
{
namespace utilities
{
    template <typename BaseType, typename DerivedType, typename... DerivedTypes>
    template <typename ReturnType, typename FunctorType>
    ReturnType AbstractInvoker<BaseType, DerivedType, DerivedTypes...>::Invoke(const FunctorType& functor, const BaseType* basePointer)
    {
        const DerivedType* ptr = dynamic_cast<const DerivedType*>(basePointer);
        if (ptr != nullptr)
        {
            return functor(ptr);
        }
        else
        {
            return AbstractInvoker<BaseType, DerivedTypes...>::template Invoke<ReturnType>(functor, basePointer);
        }
    }

    template <typename BaseType>
    template <typename ReturnType, typename FunctorType>
    ReturnType AbstractInvoker<BaseType>::Invoke(const FunctorType& /*functor*/, const BaseType* /*basePointer*/)
    {
        throw LogicException(LogicExceptionErrors::illegalState, "base type reference could not be matched with a derived type");
    }
}
}
