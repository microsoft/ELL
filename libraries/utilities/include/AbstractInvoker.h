////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     AbstractInvoker.h (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace emll
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
    /// Note that this example purposely uses templated functions, which cannot be virtualized. Next, we
    /// have a special function object with two requirements: (1) is specifies its return type with a
    /// using statement, (2) it's operator() takes a const reference to a templated type. Namely, it
    /// could look like this
    ///
    ///     struct MyFunctor
    ///     {
    ///         using ReturnType = double;
    ///
    ///         template&lt;typename ReturnType&gt;
    ///         ReturnType operator()(const FooType& derived) const { return derived.Func&lt;typename ReturnType&gt;(); }
    ///     };
    ///
    /// It is trivial to invoke the function object on an instance of Foo1 or Foo2, as follows:
    ///
    ///     MyFunctor myFunctor;
    ///     Foo1 foo;
    ///     auto result = myFunctor(foo);
    ///
    /// However, it is not trivial to call the function object on a const reference to the base class:
    ///
    ///     MyFunctor myFunctor;
    ///     IFoo& fooRef = Foo1();
    ///     auto result1 = myFunctor(fooRef); // compilation error, because IFoo doesn't have a member Func
    ///
    /// This is where the helper class comes in handy:
    ///
    ///     MyFunctor myFunctor;
    ///     IFoo& fooRef = Foo1();
    ///     auto result1 = AbstractInvoker&lt;IFoo, Foo1, Foo2&gt;::Invoke(functor, fooRef);
    ///
    /// <typeparam name="BaseType"> The base type. </typeparam>
    /// <typeparam name="DerivedTypes"> List of derived types to match against. </typeparam>
    template <typename BaseType, typename... DerivedTypes>
    class AbstractInvoker;

    template <typename BaseType, typename DerivedType, typename... DerivedTypes>
    class AbstractInvoker<BaseType, DerivedType, DerivedTypes...>
    {
    public:
        template <typename FunctorType>
        static auto Invoke(const FunctorType& functor, const BaseType& abstractArgument) -> typename FunctorType::ReturnType;
    };

    template <typename BaseType>
    class AbstractInvoker<BaseType>
    {
    public:
        template <typename FunctorType>
        static auto Invoke(const FunctorType& functor, const BaseType& abstractArgument) -> typename FunctorType::ReturnType;
    };
}
}

#include "../tcc/AbstractInvoker.tcc"