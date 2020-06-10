////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Array.h (value)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "EmitterContext.h"

#include <utilities/include/FunctionUtils.h>
#include <utilities/include/MemoryLayout.h>

#include <functional>
#include <tuple>

namespace ell
{
namespace value
{

    /// <summary> Wraps a Value instance and enforces a memory layout that represents a multidimensional array </summary>
    class Array
    {
    public:
        Array();

        /// <summary> Constructor that wraps the provided instance of Value </summary>
        /// <param name="value"> The Value instance to wrap </param>
        Array(Value value, const std::string& name = "");

        /// <summary> Constructs an instance from a 1D std::vector reshaped into the given array shape </summary>
        /// <typeparam name="T"> Any fundamental type accepted by Value </typeparam>
        /// <param name="data"> The data represented as a std::vector, in canonical row-major layout </param>
        /// <param name="shape"> The shape of the memory </param>
        template <typename T>
        Array(const std::vector<T>& data, const utilities::MemoryShape& shape);

        /// <summary> Constructs an instance from a 1D std::vector reshaped into the given array shape </summary>
        /// <typeparam name="T"> Any fundamental type accepted by Value </typeparam>
        /// <param name="data"> The data represented as a std::vector, in canonical row-major layout </param>
        /// <param name="layout"> The layout of the memory </param>
        template <typename T>
        Array(const std::vector<T>& data, const utilities::MemoryLayout& layout);

        Array(const Array&);
        Array(Array&&) noexcept;
        Array& operator=(const Array&);
        Array& operator=(Array&&);
        ~Array();

        /// <summary> Array element access operator. </summary>
        /// <returns> The Scalar value wrapping the value that is at the specified index within the array </return>
        Scalar operator()(const std::vector<Scalar>& indices);

        /// <summary> Array element access operator. </summary>
        /// <returns> A copy of the Scalar value that is at the specified index within the array </return>
        Scalar operator()(const std::vector<Scalar>& indices) const;

        /// <summary> Array element access operator. </summary>
        /// <returns> The Scalar value wrapping the value that is at the specified index within the array </return>
        template <typename... T>
        Scalar operator()(T... indices);

        /// <summary> Array element access operator. </summary>
        /// <returns> A copy of the Scalar value that is at the specified index within the array </return>
        template <typename... T>
        Scalar operator()(T... indices) const;

        /// <summary> Gets the underlying wrapped Value instance </summary>
        Value GetValue() const;

        /// <summary> Creates a new Array instance that contains the same data as this instance </summary>
        /// <returns> A new Array instance that points to a new, distinct memory that contains the same data as this instance </returns>
        Array Copy() const;

        /// <summary> Returns the number of active elements </summary>
        /// <returns> The size of the array </returns>
        size_t Size() const;

        /// <summary> Retrieves the type of data stored in the wrapped Value instance </summary>
        /// <returns> The type </returns>
        ValueType Type() const;

        void SetName(const std::string& name);
        std::string GetName() const;

    private:
        Value _value;
    };

    /// <summary> Creates a for loop over the array </summary>
    /// <param name="array"> The instance of Array that references the data over which to iterate </param>
    /// <param name="fn"> The function to be called for each coordinate where there is an active element </param>
    void For(Array array, std::function<void(const std::vector<Scalar>&)> fn);

    /// <summary> Constructs an allocated instance with the specified dimensions </summary>
    /// <typeparam name="T"> Any fundamental type accepted by Value </typeparam>
    /// <param name="shape"> The shape of the memory </param>
    template <typename T>
    Array MakeArray(const utilities::MemoryShape& shape)
    {
        return Array(Allocate<T>(utilities::MemoryLayout(shape)));
    }

    /// <summary> Constructs an allocated instance with the specified dimensions </summary>
    /// <typeparam name="T"> Any fundamental type accepted by Value </typeparam>
    /// <param name="shape"> The shape of the memory </param>
    /// <param name="name"> The name of the allocated matrix </param>
    template <typename T>
    Array MakeArray(const utilities::MemoryShape& shape, std::string name)
    {
        auto result = MakeArray<T>(shape);
        result.SetName(name);
        return result;
    }

} // namespace value
} // namespace ell

#pragma region implementation

namespace ell
{
namespace value
{
    template <typename T>
    Array::Array(const std::vector<T>& data, const utilities::MemoryShape& shape)
    {
        using namespace utilities;

        int size = static_cast<int>(data.size());
        if (size != shape.NumElements())
        {
            throw InputException(InputExceptionErrors::invalidSize);
        }
        _value = Value(data, MemoryLayout(shape));
    }

    template <typename T>
    Array::Array(const std::vector<T>& data, const utilities::MemoryLayout& layout)
    {
        using namespace utilities;

        auto size = data.size();
        if (size != layout.GetMemorySize())
        {
            throw InputException(InputExceptionErrors::invalidSize);
        }
        _value = Value(data, layout);
    }

    template <typename... T>
    Scalar Array::operator()(T... indices)
    {
        static_assert(utilities::AllSame<Scalar, std::decay_t<T>...>);
        if (sizeof...(T) != GetValue().GetLayout().NumDimensions())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidSize);
        }
        Value indexedValue = GetContext().Offset(_value, { indices... });
        indexedValue.SetLayout(utilities::ScalarLayout);

        return indexedValue;
    }

    template <typename... T>
    Scalar Array::operator()(T... indices) const
    {
        static_assert(utilities::AllSame<Scalar, std::decay_t<T>...>);
        if (sizeof...(T) != GetValue().GetLayout().NumDimensions())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidSize);
        }
        Value indexedValue = GetContext().Offset(_value, { indices... }); // shouldn't this be "load(offset(...))"?
        indexedValue.SetLayout(utilities::ScalarLayout);

        return Scalar(indexedValue).Copy();
    }
} // namespace value
} // namespace ell

#pragma endregion implementation
