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
#include <utilities/include/TupleUtils.h>

#include <functional>
#include <iosfwd>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>

namespace ell
{
namespace value
{

    namespace detail
    {
        Scalar CalculateOffset(const utilities::MemoryLayout& layout, std::vector<Scalar> coordinates);
    } // namespace detail

    class FunctionDeclaration;
    class Vector;
    enum class PrefetchType;
    enum class PrefetchLocality;

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
            return GlobalAllocateImpl(GlobalAllocationScope::Global, name, data, layout.value_or(optionalLayout));
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

        /// <summary> Returns true if function is defined for this context, false otherwise </summary>
        /// <param name="decl"> The function declaration describing the function </param>
        bool IsFunctionDefined(FunctionDeclaration decl) const;

        /// <summary> Stores data known ahead of time in the form of a std::vector of one of the fundamental types </summary>
        /// <param name="data"> The data that is to be stored by the context instance </param>
        /// <returns> An instance of Value that contains a referece to the allocated memory </returns>
        Value StoreConstantData(ConstantData data);

        /// <summary> Creates a for loop over the memory pointed to with the given layout </summary>
        /// <param name="layout"> The layout used to describe the iteration characteristics. Only active elements are iterated over. </param>
        /// <param name="fn"> The function to be called for each coordinate where there is an active element </param>
        void For(MemoryLayout layout, std::function<void(std::vector<Scalar>)> fn);

        /// <summary> Creates a for loop beggining at `start`, ending at `stop`, and incrementing by `step` </summary>
        /// <param name="start"> The value used to initialize the loop counter </param>
        /// <param name="stop"> The terminal value of the loop </param>
        /// <param name="step"> The value by which the loop counter is incremented </param>
        /// <param name="fn"> The function to be called for each coordinate where there is an active element </param>
        void For(Scalar start, Scalar stop, Scalar step, std::function<void(Scalar)> fn);

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

        Value Reference(Value source);

        Value Dereference(Value source);

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

        void Prefetch(Value data, PrefetchType type, PrefetchLocality locality);

        /// <summary> Runs the provided function, in parallel if possible </summary>
        /// <param name="numTasks"> The number of tasks that should be created </param>
        /// <param name="captured"> A list of values to be used inside the function </param>
        /// <param name="fn"> The function that gets run for each task. The first parameter is the task number for that particular call. The second parameter
        /// will be filled in with the values provided within the `captured` parameter. </param>
        void Parallelize(int numTasks, std::vector<Value> captured, std::function<void(Scalar, std::vector<Value>)> fn);

        void DebugDump(Value value, std::string tag, std::ostream* stream) const;
        void DebugDump(FunctionDeclaration fn, std::string tag, std::ostream* stream) const;

        /// <summary> Emit a debug print message.  This assumes the application
        /// on the target platform implements a "void DebugPrint(char* message)" function.  This function will be
        /// defined for you when running in JIT or Compute mode.  </summary>
        void DebugPrint(std::string message);

    protected:
        const std::vector<std::reference_wrapper<FunctionDeclaration>>& GetIntrinsics() const;

    private:
        virtual Value AllocateImpl(ValueType, MemoryLayout) = 0;

        virtual std::optional<Value> GetGlobalValue(GlobalAllocationScope scope, std::string name) = 0;
        virtual Value GlobalAllocateImpl(GlobalAllocationScope scope, std::string name, ConstantData data, MemoryLayout layout) = 0;
        virtual Value GlobalAllocateImpl(GlobalAllocationScope scope, std::string name, ValueType type, MemoryLayout layout) = 0;

        virtual detail::ValueTypeDescription GetTypeImpl(Emittable) = 0;

        virtual DefinedFunction CreateFunctionImpl(FunctionDeclaration decl, DefinedFunction fn) = 0;
        virtual bool IsFunctionDefinedImpl(FunctionDeclaration decl) const = 0;

        virtual Value StoreConstantDataImpl(ConstantData data) = 0;

        virtual void ForImpl(MemoryLayout layout, std::function<void(std::vector<Scalar>)> fn) = 0;
        virtual void ForImpl(Scalar start, Scalar stop, Scalar step, std::function<void(Scalar)> fn) = 0;

        virtual void MoveDataImpl(Value& source, Value& destination) = 0;

        virtual void CopyDataImpl(const Value& source, Value& destination) = 0;

        virtual Value OffsetImpl(Value source, Value offset) = 0;

        virtual Value ReferenceImpl(Value source) = 0;
        virtual Value DereferenceImpl(Value source) = 0;

        virtual Value UnaryOperationImpl(ValueUnaryOperation op, Value destination) = 0;
        virtual Value BinaryOperationImpl(ValueBinaryOperation op, Value destination, Value source) = 0;

        virtual Value LogicalOperationImpl(ValueLogicalOperation op, Value source1, Value source2) = 0;

        virtual Value CastImpl(Value value, ValueType type) = 0;

        virtual IfContext IfImpl(Scalar test, std::function<void()> fn) = 0;

        virtual std::optional<Value> CallImpl(FunctionDeclaration func, std::vector<Value> args) = 0;

        virtual void PrefetchImpl(Value data, PrefetchType type, PrefetchLocality locality) = 0;

        virtual void ParallelizeImpl(int numTasks, std::vector<Value> captured, std::function<void(Scalar, std::vector<Value>)> fn) = 0;

        virtual void DebugDumpImpl(Value value, std::string tag, std::ostream& stream) const = 0;
        virtual void DebugDumpImpl(FunctionDeclaration fn, std::string tag, std::ostream& stream) const = 0;

        virtual void DebugPrintImpl(std::string message) = 0;
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

    namespace detail
    {
        template <bool TakesContext, typename Fn, typename ContextType>
        struct InvokeForContextHelper
        {
            using ResultType = std::invoke_result_t<Fn, ContextType&>;
        };

        template <typename Fn, typename ContextType>
        struct InvokeForContextHelper<false, Fn, ContextType>
        {
            using ResultType = std::invoke_result_t<Fn>;
        };

        template <typename Fn, typename ContextType>
        using InvokeForContextHelperT = typename InvokeForContextHelper<std::is_invocable_v<Fn, ContextType&>, Fn, ContextType>::ResultType;
    } // namespace detail

    /// <summary> Invokes the provided function object if the GlobalContext is of the provided ContextType </summary>
    /// <typeparam name="ContextType"> The specific context derived from EmitterContext </typeparam>
    /// <param name="fn"> The function object to call, which takes a lvalue-reference of ContextType </param>
    /// <returns> The return value of `fn`, wrapped in a `std::optional` </returns>
    template <typename ContextType, typename Fn, typename ReturnType = detail::InvokeForContextHelperT<Fn, ContextType>>
    auto InvokeForContext(Fn&& fn) -> std::conditional_t<std::is_same_v<ReturnType, void>, void, std::optional<ReturnType>>;

    /// <summary> A helper RAII class to set a particular EmitterContext instance as the global context and unset it at the end of scope </summary>
    template <typename T = void, bool b = std::is_base_of_v<EmitterContext, T>>
    struct ContextGuard;

    template <>
    struct ContextGuard<void, false>
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

    private:
        EmitterContext* _oldContext;
    };

    template <typename T, bool b>
    struct ContextGuard : private ContextGuard<>
    {
        template <typename... Args>
        ContextGuard(Args&&... args) :
            ContextGuard<>(_context),
            _context(std::forward<Args>(args)...)
        {}

        T& GetContext() { return _context; }

    private:
        T _context;
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

    void DebugDump(FunctionDeclaration fn, std::string tag = "", std::ostream* stream = nullptr);
    void DebugDump(Value value, std::string tag = "", std::ostream* stream = nullptr);

    template <typename ViewType, std::enable_if_t<std::is_same_v<decltype(std::declval<ViewType>().GetValue()), Value>, void*> = nullptr>
    void DebugDump(ViewType value, std::string tag = "", std::ostream* stream = nullptr)
    {
        return DebugDump(value.GetValue(), tag, stream);
    }

    EmitterContext::IfContext If(Scalar test, std::function<void()> fn);

    void ForRange(Scalar end, std::function<void(Scalar)> fn);
    void ForRange(Scalar start, Scalar end, std::function<void(Scalar)> fn);
    void ForRange(Scalar start, Scalar end, Scalar step, std::function<void(Scalar)> fn);

    /// <summary> Runs the provided function, in parallel if possible </summary>
    /// <typeparam name="Tys..."> The types that represent the captured values. Must be `Value` or types that provide a member
    /// function called `GetValue()` which returns a `Value` instance. </typeparam>
    /// <param name="numTasks"> The number of tasks that should be created </param>
    /// <param name="captured"> A list of values to be used inside the function </param>
    /// <param name="fn"> The function that gets run for each task. The first parameter is the task number for that particular call. Subsequent parameters must match the typelist
    /// `Tys...` and will be filled in with the values provided within the `captured` parameter. </param>
    template <typename... Tys>
    void Parallelize(int numTasks, std::tuple<Tys...> captured, std::function<void(Scalar, Tys...)> fn);

    /// <summary> Runs the provided function, in parallel if possible </summary>
    /// <param name="numTasks"> The number of tasks that should be created </param>
    /// <param name="captured"> A list of values to be used inside the function </param>
    /// <param name="fn"> The function that gets run for each task. The first parameter is the task number for that particular call. The second parameter
    /// will be filled in with the values provided within the `captured` parameter. </param>
    void Parallelize(int numTasks, std::vector<Value> captured, std::function<void(Scalar, std::vector<Value>)> fn);

    extern FunctionDeclaration AbsFunctionDeclaration;
    extern FunctionDeclaration CosFunctionDeclaration;
    extern FunctionDeclaration CopySignFunctionDeclaration;
    extern FunctionDeclaration ExpFunctionDeclaration;
    extern FunctionDeclaration LogFunctionDeclaration;
    extern FunctionDeclaration Log10FunctionDeclaration;
    extern FunctionDeclaration Log2FunctionDeclaration;
    extern FunctionDeclaration MaxNumFunctionDeclaration;
    extern FunctionDeclaration MinNumFunctionDeclaration;
    extern FunctionDeclaration PowFunctionDeclaration;
    extern FunctionDeclaration SinFunctionDeclaration;
    extern FunctionDeclaration SqrtFunctionDeclaration;
    extern FunctionDeclaration TanhFunctionDeclaration;
    extern FunctionDeclaration RoundFunctionDeclaration;
    extern FunctionDeclaration FloorFunctionDeclaration;
    extern FunctionDeclaration CeilFunctionDeclaration;

    Scalar Abs(Scalar s);
    Scalar Cos(Scalar s);
    Scalar CopySign(Scalar s1, Scalar s2);
    Scalar Exp(Scalar s);
    Scalar Log(Scalar s);
    Scalar Log10(Scalar s);
    Scalar Log2(Scalar s);
    Scalar Max(Scalar s1, Scalar s2);
    Scalar Min(Scalar s1, Scalar s2);
    Scalar Pow(Scalar base, Scalar exp);
    Scalar Sin(Scalar s);
    Scalar Sqrt(Scalar s);
    Scalar Tanh(Scalar s);
    Scalar Round(Scalar s);
    Scalar Floor(Scalar s);
    Scalar Ceil(Scalar s);
    Scalar Sign(Scalar s);
    Scalar Square(Scalar s);
    Scalar LogicalNot(Scalar v);

    Vector Abs(Vector v);
    Vector Cos(Vector v);
    Vector Exp(Vector v);
    Vector Log(Vector v);
    Vector Log10(Vector v);
    Vector Log2(Vector v);
    Scalar Max(Vector v);
    Scalar Min(Vector v);
    Vector Pow(Vector bases, Scalar exp);
    Vector Sin(Vector v);
    Vector Sqrt(Vector v);
    Vector Tanh(Vector v);
    Vector Round(Vector v);
    Vector Floor(Vector v);
    Vector Ceil(Vector v);

    /// <summary> Specifier determining if the fetch should be for a read or a write </summary>
    enum class PrefetchType
    {
        Read = 0,
        Write
    };

    /// <summary> Temporal locality specifier. Data with temporal locality, or persistence,
    /// is expected to be accessed multiple times and so should be left in a cache when it
    /// is prefetched so it will continue to be readily accessible. Accesses to data with
    /// no temporal locality are transient; the data is unlikely to be accessed multiple times
    /// and, if possible, should not be left in a cache where it would displace other data that
    /// might be needed soon. </summary>
    enum class PrefetchLocality
    {
        None = 0,
        Low,
        Moderate,
        Extreme
    };

    template <typename ViewType>
    void Prefetch(ViewType view, PrefetchType type = PrefetchType::Read, PrefetchLocality locality = PrefetchLocality::None);

    /// <summary> Returns the passed in View type with the memory layout representative of the full view of the memory, i.e., no padding. </summary>
    template <typename ViewType>
    ViewType AsFullView(ViewType view);

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
            if constexpr (std::is_invocable_v<Fn, decltype(*ptr)>)
            {
                return fn(*ptr);
            }
            else
            {
                return fn();
            }
        }

        if constexpr (!std::is_same_v<ReturnType, void>)
        {
            return std::nullopt;
        }
    }

    template <typename... Tys>
    void Parallelize(int numTasks, std::tuple<Tys...> captured, std::function<void(Scalar, Tys...)> fn)
    {
        auto capturedValues = utilities::TupleToVector<Value>([](auto view) { return detail::GetValue(view); }, captured);

        Parallelize(
            numTasks,
            capturedValues,
            [fn = std::move(fn)](Scalar threadIndex, std::vector<Value> captures) {
                auto fnArgsTuple = [&] {
                    std::tuple scalarTuple{ threadIndex };
                    if constexpr (sizeof...(Tys) > 0)
                    {
                        auto capturesTuple = utilities::VectorToTuple<Tys...>(captures);
                        return std::tuple_cat(scalarTuple, capturesTuple);
                    }
                    else
                    {
                        return scalarTuple;
                    }
                }();
                std::apply(fn, fnArgsTuple);
            });
    }

    template <typename ViewType>
    void Prefetch(ViewType view, PrefetchType type, PrefetchLocality locality)
    {
        GetContext().Prefetch(detail::GetValue(view), type, locality);
    }

    template <typename ViewType>
    ViewType AsFullView(ViewType view)
    {
        auto value = GetValue(view);
        value.SetLayout(utilities::MemoryLayout{ value.GetLayout().GetExtent() });
        return value;
    }

} // namespace value
} // namespace ell

#pragma endregion implementation
