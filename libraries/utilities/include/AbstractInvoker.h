////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     AbstractInvoker.h (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace ell
{
namespace utilities
{
    /// <summary> A helper class that casts a reference to a polymorphic base class into a reference to
    /// a derived class before sending the refence as the argument of a functor. </summary>
    ///
    /// To use this helper, there has to be a polymorphic base class:
    ///
    ///     struct IFoo { virtual ~IFoo() {} };
    ///
    /// and several derived classes:
    ///
    ///     struct Foo1 : public IFoo
    ///     {
    ///         template &lt;typename ReturnType&gt;
    ///         ReturnType Func() const { ReturnType x = 1;  return x; }
    ///     };
    ///
    ///     struct Foo2 : public IFoo
    ///     {
    ///         template &lt;typename ReturnType&gt;
    ///         ReturnType Func() const { ReturnType x = 2; return x; }
    ///     };
    ///
    /// Note that this example purposely uses templated functions, which cannot be virtualized.
    /// It is easy to invoke the function object on an instance of Foo1 or Foo2, as follows:
    ///
    ///     Foo1 foo1;
    ///     Foo2 foo2;
    ///
    ///     auto result1 = foo1.Func&lt;float&gt;();
    ///     auto result2 = foo2.Func&lt;float&gt;();
    ///
    /// Using the AbstractInvoker, we can call the templated function given a pointer to the base class
    ///
    ///     Foo1 foo1;
    ///     Foo2 foo2;
    ///     const IFoo* baseClassPtr1 = &foo1;
    ///     const IFoo* baseClassPtr2 = &foo2;
    ///
    ///     auto funcCaller = [](const auto* ptr){ return ptr->Func();};
    ///     using Invoker = AbstractInvoker&lt;IFoo, Foo1, Foo2&gt;;
    ///
    ///     auto result1 = Invoke(funcCaller, baseClassPtr1);
    ///     auto result2 = Invoke(funcCaller, baseClassPtr2);
    ///
    /// <typeparam name="BaseType"> The base type. </typeparam>
    /// <typeparam name="DerivedTypes"> List of derived types to match against. </typeparam>
    template <typename BaseType, typename... DerivedTypes>
    class AbstractInvoker;

    template <typename BaseType, typename DerivedType, typename... DerivedTypes>
    class AbstractInvoker<BaseType, DerivedType, DerivedTypes...>
    {
    public:
        /// <summary></summary>
        template <typename ReturnType, typename FunctorType>
        static ReturnType Invoke(const FunctorType& functor, const BaseType* basePointer);
    };

    template <typename BaseType>
    class AbstractInvoker<BaseType>
    {
    public:
        /// <summary></summary>
        template <typename ReturnType, typename FunctorType>
        static ReturnType Invoke(const FunctorType& functor, const BaseType* basePointer);
    };
}
}

#include "../tcc/AbstractInvoker.tcc"