////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     EmitterContext.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Emittable.h"
#include "ValueType.h"
#include "Value.h"

// utilities
#include "Boolean.h"
#include "FunctionUtils.h"
#include "MemoryLayout.h"

// stl
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <variant>

namespace ell
{
namespace value
{

    class Scalar;
    class Value;

    namespace detail
    {
        Scalar CalculateOffset(const utilities::MemoryLayout& layout, std::vector<Scalar> coordinates);
    }

    /// <summary> An interface describing the global context that's used by the Value library </summary>
    /// <remarks> This class employs the non-virtual interface pattern to provide an easy to use API while
    /// minimizing the functions needed to be overloaded. </remarks>
    class EmitterContext
    {
    protected:
        using MemoryLayout = utilities::MemoryLayout;
        using MemoryCoordinates = utilities::MemoryCoordinates;

    public:
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

        /// <summary> Gets the type information contained in an instance of Emittable </summary>
        /// <param name="emittable"> The instance of Emittable to be queried </param>
        /// <returns> A std::pair instance describing the fundamental type of data, along with the number of pointers </returns>
        std::pair<ValueType, int> GetType(Emittable emittable);

        /// <summary> Creates a callable function </summary>
        /// <param name="fnName"> The string identifier for this particular function </param>
        /// <param name="fn"> The function that defines the function body to be executed when the callable function is called </param>
        /// <returns> A callable function that executes the body described by fn </returns>
        std::function<void()> CreateFunction(std::string fnName, std::function<void()> fn);

        /// <summary> Creates a callable function </summary>
        /// <param name="fnName"> The string identifier for this particular function </param>
        /// <param name="argValues"> A vector of Values describing the types of the arguments and their memory layout expected by the function </param>
        /// <param name="fn"> The function that defines the function body to be executed when the callable function is called </param>
        /// <returns> A callable function that executes the body described by fn </returns>
        std::function<void(std::vector<Value>)> CreateFunction(std::string fnName, std::vector<Value> argValues,
                                                               std::function<void(std::vector<Value>)> fn);

        /// <summary> Creates a callable function </summary>
        /// <param name="fnName"> The string identifier for this particular function </param>
        /// <param name="returnValue"> A Value instance describing type of the value that is expected and its memory layout to be returned by the function </param>
        /// <param name="fn"> The function that defines the function body to be executed when the callable function is called </param>
        /// <returns> A callable function that executes the body described by fn </returns>
        std::function<Value()> CreateFunction(std::string fnName, Value returnValue, std::function<Value()> fn);

        /// <summary> Creates a callable function </summary>
        /// <param name="fnName"> The string identifier for this particular function </param>
        /// <param name="returnValue"> A Value instance describing type of the value that is expected and its memory layout to be returned by the function </param>
        /// <param name="argValues"> A vector of Values describing the types of the arguments and their memory layout expected by the function </param>
        /// <param name="fn"> The function that defines the function body to be executed when the callable function is called </param>
        /// <returns> A callable function that executes the body described by fn </returns>
        std::function<Value(std::vector<Value>)> CreateFunction(std::string fnName, Value returnValue,
                                                                std::vector<Value> argValues,
                                                                std::function<Value(std::vector<Value>)> fn);

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

    private:
        virtual Value AllocateImpl(ValueType, MemoryLayout) = 0;

        virtual std::pair<ValueType, int> GetTypeImpl(Emittable) = 0;

        virtual std::function<void()> CreateFunctionImpl(std::string fnName, std::function<void()> fn) = 0;
        virtual std::function<Value()> CreateFunctionImpl(std::string fnName, Value returnValue,
                                                          std::function<Value()> fn) = 0;
        virtual std::function<void(std::vector<Value>)> CreateFunctionImpl(
            std::string fnName, std::vector<Value> argValues, std::function<void(std::vector<Value>)> fn) = 0;
        virtual std::function<Value(std::vector<Value>)> CreateFunctionImpl(
            std::string fnName, Value returnValue, std::vector<Value> argValues,
            std::function<Value(std::vector<Value>)> fn) = 0;

        virtual Value StoreConstantDataImpl(ConstantData data) = 0;

        virtual void ForImpl(MemoryLayout layout, std::function<void(std::vector<Scalar>)> fn) = 0;

        virtual void MoveDataImpl(Value& source, Value& destination) = 0;

        virtual void CopyDataImpl(const Value& source, Value& destination) = 0;

        virtual Value OffsetImpl(Value source, Value offset) = 0;

        virtual Value UnaryOperationImpl(ValueUnaryOperation op, Value destination) = 0;
        virtual Value BinaryOperationImpl(ValueBinaryOperation op, Value destination, Value source) = 0;
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
    /// <param name="nn"> The function object to call, which takes a lvalue-reference of ContextType </param>
    template <typename ContextType, typename Fn>
    void InvokeForContext(Fn&& fn);

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

    /// <summary> Creates a callable function </summary>
    /// <param name="fnName"> The string identifier for this particular function </param>
    /// <param name="fn"> The function that defines the function body to be executed when the callable function is called </param>
    /// <returns> A callable function that executes the body described by fn </returns>
    template <typename Fn>
    auto CreateFunction(std::string fnName, Fn&& fn);

    /// <summary> Creates a callable function </summary>
    /// <param name="fnName"> The string identifier for this particular function </param>
    /// <param name="returnValue"> A Value instance describing type of the value that is expected and its memory layout to be returned by the function </param>
    /// <param name="fn"> The function that defines the function body to be executed when the callable function is called </param>
    /// <returns> A callable function that executes the body described by fn </returns>
    template <typename Fn>
    auto CreateFunction(std::string fnName, Value returnValue, Fn&& fn);

    /// <summary> Creates a callable function </summary>
    /// <param name="fnName"> The string identifier for this particular function </param>
    /// <param name="argValues"> A vector of Values describing the types of the arguments and their memory layout expected by the function </param>
    /// <param name="fn"> The function that defines the function body to be executed when the callable function is called </param>
    /// <returns> A callable function that executes the body described by fn </returns>
    template <typename Fn>
    auto CreateFunction(std::string fnName, std::vector<Value> argValues, Fn&& fn);

    /// <summary> Creates a callable function </summary>
    /// <param name="fnName"> The string identifier for this particular function </param>
    /// <param name="returnValue"> A Value instance describing type of the value that is expected and its memory layout to be returned by the function </param>
    /// <param name="argValues"> A vector of Values describing the types of the arguments and their memory layout expected by the function </param>
    /// <param name="fn"> The function that defines the function body to be executed when the callable function is called </param>
    /// <returns> A callable function that executes the body described by fn </returns>
    template <typename Fn>
    auto CreateFunction(std::string fnName, Value returnValue, std::vector<Value> argValues, Fn&& fn);

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

} // namespace value
} // namespace ell

#include "../tcc/EmitterContext.tcc"
