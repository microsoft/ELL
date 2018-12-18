////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     EmitterContext.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Emittable.h"
#include "Scalar.h"
#include "Value.h"
#include "ValueType.h"

#include <utilities/include/Boolean.h>
#include <utilities/include/FunctionUtils.h>
#include <utilities/include/MemoryLayout.h>

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <variant>

namespace ell
{
namespace value
{

    namespace detail
    {
        Scalar CalculateOffset(const utilities::MemoryLayout& layout, std::vector<Scalar> coordinates);
    }

    class FunctionDeclaration;
    class Vector;

    /// <summary> An interface describing the global context that's used by the Value library </summary>
    /// <remarks> This class employs the non-virtual interface pattern to provide an easy to use API while
    /// minimizing the functions needed to be overloaded. </remarks>
    class EmitterContext
    {
    protected:
        using MemoryLayout = utilities::MemoryLayout;
        using MemoryCoordinates = utilities::MemoryCoordinates;

        class IfContextImpl
        {
        public:
            virtual ~IfContextImpl();
            virtual void ElseIf(Scalar, std::function<void()>) = 0;
            virtual void Else(std::function<void()>) = 0;
        };

        enum class GlobalAllocationScope
        {
            Global,
            Function
        };

    public:
        using DefinedFunction = std::function<std::optional<Value>(std::vector<Value>)>;

        class IfContext
        {
        public:
            IfContext(std::unique_ptr<IfContextImpl> impl);
            IfContext(const IfContext&) = delete;
            IfContext(IfContext&&) = delete;

            IfContext&& ElseIf(Scalar, std::function<void()>) &&;
            void Else(std::function<void()>) &&;

        private:
            std::unique_ptr<IfContextImpl> _impl;
        };

        /// <summary> Describes the type that can be used to represent constant C++ data </summary>
        using ConstantData = detail::ConstantData;

        virtual ~EmitterContext();

        /// <summary> Allocates data with the specified type and size </summary>
        /// <param name="type"> The type of the data to allocate </param>
        /// <param name="size"> The size of the allocation, in number of elements </param>
        /// <returns> An instance of Value that contains a referece to the allocated memory </returns>
        Value Allocate(ValueType type, size_t size);

        /// <summary> Allocates data with the specified type and size </summary>
        /// <param name="type"> The type of the data to allocate </param>
        /// <param name="layout"> The memory layout of the allocation, in number of elements </param>
        /// <returns> An instance of Value that contains a referece to the allocated memory </returns>
        Value Allocate(ValueType type, MemoryLayout layout);

        /// <summary> Allocates function static data </summary>
        /// <param name="name"> The name of the variable </param>
        /// <param name="type"> The type of the data </param>
        /// <param name="layout"> The layout of the data </param>
        Value StaticAllocate(std::string name, ValueType type, utilities::MemoryLayout layout);

        /// <summary> Allocates function static data </summary>
        /// <param name="name"> The name of the variable </param>
        /// <param name="data"> The data </param>
        /// <param name="layout"> The layout of the data </param>
        template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, void*> = nullptr>
        Value StaticAllocate(std::string name, const std::vector<T>& data, std::optional<utilities::MemoryLayout> layout = {})
        {
            if (auto globalValue = GetGlobalValue(GlobalAllocationScope::Function, name))
            {
                return globalValue.value();
            }

            auto optionalLayout = utilities::MemoryLayout({ static_cast<int>(data.size()) });
            return GlobalAllocateImpl(GlobalAllocationScope::Function, name, data, layout.value_or(optionalLayout));
        }

        /// <summary> Allocates scalar function static data </summary>
        /// <param name="name"> The name of the variable </param>
        /// <param name="data"> The data </param>
        template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, void*> = nullptr>
        Value StaticAllocate(std::string name, T t)
        {
            return this
                ->template StaticAllocate(name,
                                          std::vector<
                                              std::conditional_t<std::is_same_v<T, bool>, utilities::Boolean, T>>{ t });
        }

        /// <summary> Allocates global data </summary>
        /// <param name="name"> The name of the variable </param>
        /// <param name="type"> The type of the data </param>
        /// <param name="layout"> The layout of the data </param>
        Value GlobalAllocate(std::string name, ValueType type, utilities::MemoryLayout layout);

        /// <summary> Allocates global data </summary>
        /// <param name="name"> The name of the variable </param>
        /// <param name="data"> The data </param>
        /// <param name="layout"> The layout of the data </param>
        template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, void*> = nullptr>
        Value GlobalAllocate(std::string name, const std::vector<T>& data, std::optional<utilities::MemoryLayout> layout = {})
        {
            if (auto globalValue = GetGlobalValue(GlobalAllocationScope::Global, name))
            {
                return globalValue.value();
            }

            auto optionalLayout = utilities::MemoryLayout({ static_cast<int>(data.size()) });
            return GlobalAllocateImpl(GlobalAllocationScope::Function, name, data, layout.value_or(optionalLayout));
        }

        /// <summary> Allocates scalar global data </summary>
        /// <param name="name"> The name of the variable </param>
        /// <param name="data"> The data </param>
        template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, void*> = nullptr>
        Value GlobalAllocate(std::string name, T t)
        {
            return this
                ->template GlobalAllocate(name,
                                          std::vector<
                                              std::conditional_t<std::is_same_v<T, bool>, utilities::Boolean, T>>{ t });
        }

        /// <summary> Gets the type information contained in an instance of Emittable </summary>
        /// <param name="emittable"> The instance of Emittable to be queried </param>
        /// <returns> A std::pair instance describing the fundamental type of data, along with the number of pointers </returns>
        detail::ValueTypeDescription GetType(Emittable emittable);

        /// <summary> Creates a callable function </summary>
        /// <param name="decl"> The function declaration describing the function </param>
        /// <param name="fn"> The function that defines the function body to be executed when the callable function is called </param>
        /// <returns> A callable function that executes the body described by fn </returns>
        DefinedFunction CreateFunction(FunctionDeclaration decl, DefinedFunction fn);

        /// <summary> Stores data known ahead of time in the form of a std::vector of one of the fundamental types </summary>
        /// <param name="data"> The data that is to be stored by the context instance </param>
        /// <returns> An instance of Value that contains a referece to the allocated memory </returns>
        Value StoreConstantData(ConstantData data);

        /// <summary> Creates a for loop over the memory pointed to with the given layout </summary>
        /// <param name="layout"> The layout used to describe the iteration characteristics. Only active elements are iterated over. </param>
        /// <param name="fn"> The function to be called for each coordinate where there is an active element </param>
        void For(MemoryLayout layout, std::function<void(std::vector<Scalar>)> fn);

        /// <summary> Moves the data from one location to another </summary>
        /// <param name="source"> The source of the memory to be moved </param>
        /// <param name="destination"> The destination of the memory to be moved </param>
        /// <remarks> The source and destination instances must be constrained and have matching MemoryLayouts </remarks>
        void MoveData(Value& source, Value& destination);

        /// <summary> Copies the data from one location to another </summary>
        /// <param name="source"> The source of the memory to be copied </param>
        /// <param name="destination"> The destination of the memory to be copied </param>
        /// <remarks> The source and destination instances must be constrained and have matching MemoryLayouts </remarks>
        void CopyData(const Value& source, Value& destination);

        /// <summary> Returns the offset memory location </summary>
        /// <param name="source"> The source memory location </param>
        /// <param name="offset"> The offset in number of elements by which to move </param>
        /// <returns> A Value instance that refers to the offset memory location </returns>
        Value Offset(Value source, Value offset);

        /// <summary> Returns the offset memory location </summary>
        /// <param name="source"> The source memory location </param>
        /// <param name="offset"> The offset in number of elements by which to move </param>
        /// <returns> A Value instance that refers to the offset memory location </returns>
        /// <remarks> source must be constrained and the number of items in offset must match the degree of source </remarks>
        Value Offset(Value source, std::vector<Scalar> offset);

        /// <summary> Performs a unary operation </summary>
        /// <param name="op"> The unary operation to perform </param>
        /// <param name="destination"> The data over which to perform the operation and store the result </param>
        /// <returns> An instance of Value pointing to the same memory as destination </returns>
        Value UnaryOperation(ValueUnaryOperation op, Value destination);

        /// <summary> Performs a binary operation </summary>
        /// <param name="op"> The binary operation to perform </param>
        /// <param name="destination"> The data which is considered the first operand and destination for the result </param>
        /// <param name="source"> The data which is considered the second operand </param>
        /// <returns> An instance of Value pointing to the same memory as destination </returns>
        Value BinaryOperation(ValueBinaryOperation op, Value destination, Value source);

        Value LogicalOperation(ValueLogicalOperation op, Value source1, Value source2);

        Value Cast(Value value, ValueType type);

        IfContext If(Scalar test, std::function<void()> fn);

        std::optional<Value> Call(FunctionDeclaration func, std::vector<Value> args);

    protected:
        const std::vector<std::reference_wrapper<FunctionDeclaration>>& GetIntrinsics() const;

    private:
        virtual Value AllocateImpl(ValueType, MemoryLayout) = 0;

        virtual std::optional<Value> GetGlobalValue(GlobalAllocationScope scope, std::string name) = 0;
        virtual Value GlobalAllocateImpl(GlobalAllocationScope scope, std::string name, ConstantData data, MemoryLayout layout) = 0;
        virtual Value GlobalAllocateImpl(GlobalAllocationScope scope, std::string name, ValueType type, MemoryLayout layout) = 0;

        virtual detail::ValueTypeDescription GetTypeImpl(Emittable) = 0;

        virtual DefinedFunction CreateFunctionImpl(FunctionDeclaration decl, DefinedFunction fn) = 0;

        virtual Value StoreConstantDataImpl(ConstantData data) = 0;

        virtual void ForImpl(MemoryLayout layout, std::function<void(std::vector<Scalar>)> fn) = 0;

        virtual void MoveDataImpl(Value& source, Value& destination) = 0;

        virtual void CopyDataImpl(const Value& source, Value& destination) = 0;

        virtual Value OffsetImpl(Value source, Value offset) = 0;

        virtual Value UnaryOperationImpl(ValueUnaryOperation op, Value destination) = 0;
        virtual Value BinaryOperationImpl(ValueBinaryOperation op, Value destination, Value source) = 0;

        virtual Value LogicalOperationImpl(ValueLogicalOperation op, Value source1, Value source2) = 0;

        virtual Value CastImpl(Value value, ValueType type) = 0;

        virtual IfContext IfImpl(Scalar test, std::function<void()> fn) = 0;

        virtual std::optional<Value> CallImpl(FunctionDeclaration func, std::vector<Value> args) = 0;
    };

    /// <summary> Returns the global instance of EmitterContext </summary>
    /// <remarks> This is the instance used by all operations in the Value library. As such, it must be set before using
    /// any of the APIs provided in the Value library </summary>
    EmitterContext& GetContext();

    /// <summary> Sets the global instance of EmitterContext </summary>
    /// <param name="context"> The context to set as the global instance </param>
    void SetContext(EmitterContext& context);

    /// <summary> Clears the global instance of EmitterContext </summary>
    void ClearContext() noexcept;

    /// <summary> Invokes the provided function object if the GlobalContext is of the provided ContextType </summary>
    /// <typeparam name="ContextType"> The specific context derived from EmitterContext </typeparam>
    /// <param name="fn"> The function object to call, which takes a lvalue-reference of ContextType </param>
    /// <returns> The return value of `fn`, wrapped in a `std::optional` </returns>
    template <typename ContextType, typename Fn, typename ReturnType = std::invoke_result_t<Fn, ContextType&>>
    auto InvokeForContext(Fn&& fn) -> std::conditional_t<std::is_same_v<ReturnType, void>, void, std::optional<ReturnType>>;

    /// <summary> A helper RAII class to set a particular EmitterContext instance as the global context and unset it at the end of scope </summary>
    struct ContextGuard
    {
        /// <summary> Constructor </summary>
        /// <param name="context"> The instance of EmitterContext to set as the global context </param>
        ContextGuard(EmitterContext& context);

        /// <summary> Destructor for the instance. Sets the global context to nullptr </summary>
        ~ContextGuard();

        ContextGuard(const ContextGuard&) = delete;
        ContextGuard(ContextGuard&&) = delete;
        ContextGuard& operator=(const ContextGuard&) = delete;
        ContextGuard& operator=(ContextGuard&&) = delete;
    };

    /// <summary> Allocates data with the specified type and size </summary>
    /// <param name="type"> The type of the data to allocate </param>
    /// <param name="size"> The size of the allocation, in number of elements </param>
    /// <returns> An instance of Value that contains a referece to the allocated memory </returns>
    Value Allocate(ValueType type, size_t size);

    /// <summary> Allocates data with the specified type and size </summary>
    /// <param name="type"> The type of the data to allocate </param>
    /// <param name="layout"> The memory layout of the allocation, in number of elements </param>
    /// <returns> An instance of Value that contains a referece to the allocated memory </returns>
    Value Allocate(ValueType type, utilities::MemoryLayout layout);

    /// <summary> Allocates data with the specified type and size </summary>
    /// <typeparam name="T"> The type of the data to allocate </param>
    /// <param name="size"> The size of the allocation, in number of elements </param>
    /// <returns> An instance of Value that contains a referece to the allocated memory </returns>
    template <typename T>
    Value Allocate(size_t size)
    {
        return Allocate(GetValueType<T>(), size);
    }

    /// <summary> Allocates data with the specified type and size </summary>
    /// <typeparam name="T"> The type of the data to allocate </param>
    /// <param name="layout"> The memory layout of the allocation, in number of elements </param>
    /// <returns> An instance of Value that contains a referece to the allocated memory </returns>
    template <typename T>
    Value Allocate(utilities::MemoryLayout layout)
    {
        return Allocate(GetValueType<T>(), layout);
    }

    /// <summary> Allocates function static data </summary>
    /// <param name="name"> The name of the variable </param>
    /// <param name="type"> The type of the data </param>
    /// <param name="layout"> The layout of the data </param>
    Value StaticAllocate(std::string name, ValueType type, utilities::MemoryLayout layout);

    /// <summary> Allocates function static data </summary>
    /// <param name="name"> The name of the variable </param>
    /// <param name="data"> The data </param>
    /// <param name="layout"> The layout of the data </param>
    template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, void*> = nullptr>
    Value StaticAllocate(std::string name, const std::vector<T>& data, std::optional<utilities::MemoryLayout> layout = {})
    {
        return GetContext().StaticAllocate(name, data, layout);
    }

    /// <summary> Allocates scalar function static data </summary>
    /// <param name="name"> The name of the variable </param>
    /// <param name="data"> The data </param>
    template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, void*> = nullptr>
    Scalar StaticAllocate(std::string name, T t)
    {
        return StaticAllocate(name,
                              std::vector<std::conditional_t<std::is_same_v<T, bool>, utilities::Boolean, T>>{ t },
                              utilities::ScalarLayout);
    }

    /// <summary> Allocates global data </summary>
    /// <param name="name"> The name of the variable </param>
    /// <param name="type"> The type of the data </param>
    /// <param name="layout"> The layout of the data </param>
    Value GlobalAllocate(std::string name, ValueType type, utilities::MemoryLayout layout);

    template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, void*> = nullptr>
    Value GlobalAllocate(std::string name, utilities::MemoryLayout layout)
    {
        return GlobalAllocate(name, GetValueType<T>(), layout);
    }

    /// <summary> Allocates global data </summary>
    /// <param name="name"> The name of the variable </param>
    /// <param name="data"> The data </param>
    /// <param name="layout"> The layout of the data </param>
    template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, void*> = nullptr>
    Value GlobalAllocate(std::string name, const std::vector<T>& data, std::optional<utilities::MemoryLayout> layout = {})
    {
        return GetContext().GlobalAllocate(name, data, layout);
    }

    /// <summary> Allocates scalar global data </summary>
    /// <param name="name"> The name of the variable </param>
    /// <param name="data"> The data </param>
    template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, void*> = nullptr>
    Scalar GlobalAllocate(std::string name, T t)
    {
        return GlobalAllocate(name,
                              std::vector<std::conditional_t<std::is_same_v<T, bool>, utilities::Boolean, T>>{ t },
                              utilities::ScalarLayout);
    }

    EmitterContext::IfContext If(Scalar test, std::function<void()> fn);

    extern FunctionDeclaration AbsFunctionDeclaration;
    extern FunctionDeclaration CosFunctionDeclaration;
    extern FunctionDeclaration ExpFunctionDeclaration;
    extern FunctionDeclaration LogFunctionDeclaration;
    extern FunctionDeclaration MaxNumFunctionDeclaration;
    extern FunctionDeclaration MinNumFunctionDeclaration;
    extern FunctionDeclaration PowFunctionDeclaration;
    extern FunctionDeclaration SinFunctionDeclaration;
    extern FunctionDeclaration SqrtFunctionDeclaration;
    extern FunctionDeclaration TanhFunctionDeclaration;

    Scalar Abs(Scalar s);
    Scalar Cos(Scalar s);
    Scalar Exp(Scalar s);
    Scalar Log(Scalar s);
    Scalar Max(Scalar s1, Scalar s2);
    Scalar Min(Scalar s1, Scalar s2);
    Scalar Pow(Scalar base, Scalar exp);
    Scalar Sin(Scalar s);
    Scalar Sqrt(Scalar s);
    Scalar Tanh(Scalar s);

    Vector Abs(Vector v);
    Vector Cos(Vector v);
    Vector Exp(Vector v);
    Vector Log(Vector v);
    Scalar Max(Vector v);
    Scalar Min(Vector v);
    Vector Pow(Vector bases, Scalar exp);
    Vector Sin(Vector v);
    Vector Sqrt(Vector v);
    Vector Tanh(Vector v);

} // namespace value
} // namespace ell

#pragma region implementation

namespace ell
{
namespace value
{

    template <typename ContextType, typename Fn, typename ReturnType>
    auto InvokeForContext(Fn&& fn) -> std::conditional_t<std::is_same_v<ReturnType, void>, void, std::optional<ReturnType>>
    {
        static_assert(std::is_base_of_v<EmitterContext, std::decay_t<ContextType>>,
                      "ContextType must be derived from EmitterContext");

        if (auto ptr = dynamic_cast<ContextType*>(&GetContext()); ptr != nullptr)
        {
            return fn(*ptr);
        }

        if constexpr (!std::is_same_v<ReturnType, void>)
        {
            return std::nullopt;
        }
    }

} // namespace value
} // namespace ell

#pragma endregion implementation
