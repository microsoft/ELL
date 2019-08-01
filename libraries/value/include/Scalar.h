////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Scalar.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Value.h"

namespace ell
{
namespace value
{

    /// <summary> A View type that wraps a Value instance and enforces a memory layout that represents a single value </summary>
    class Scalar
    {
    public:
        Scalar();

        /// <summary> Constructor that wraps the provided instance of Value </summary>
        /// <param name="value"> The Value instance to wrap </param>
        Scalar(Value value);

        /// <summary> Constructs an instance from a fundamental type value </summary>
        /// <typeparam name="T"> Any fundamental type accepted by Value </typeparam>
        /// <param name="t"> The value to wrap </param>
        template <typename T>
        Scalar(T t) :
            Scalar(Value(t))
        {}

        Scalar(const Scalar&);
        Scalar(Scalar&&) noexcept;
        Scalar& operator=(const Scalar&);
        Scalar& operator=(Scalar&&);
        ~Scalar();

        /// <summary> Gets the underlying wrapped Value instance </summary>
        Value GetValue() const;

        /// <summary> Creates a new Scalar instance that contains the same value as this instance </summary>
        /// <returns> A new Scalar instance that points to a new, distinct memory that contains the same value as this instance </returns>
        Scalar Copy() const;

        /// <summary> Arithmetic operators </summary>
        Scalar& operator+=(Scalar);
        Scalar& operator*=(Scalar);
        Scalar& operator-=(Scalar);
        Scalar& operator/=(Scalar);
        Scalar& operator%=(Scalar);

        /// <summary> Retrieve the underlying value as a fundamental type </summary>
        /// <typeparam name="T"> The C++ fundamental type that is being retrieved from the instance </typeparam>
        /// <returns> If the wrapped Value instance's type matches that of the fundamental type, returns the value, otherwise throws </returns>
        template <typename T>
        T Get() const
        {
            return *_value.Get<T*>();
        }

        /// <summary> Retrieves the type of data stored in the wrapped Value instance </summary>
        /// <returns> The type </returns>
        ValueType GetType() const;

    private:
        friend Scalar operator+(Scalar, Scalar);
        friend Scalar operator*(Scalar, Scalar);
        friend Scalar operator-(Scalar, Scalar);
        friend Scalar operator/(Scalar, Scalar);
        friend Scalar operator%(Scalar, Scalar);

        friend Scalar operator-(Scalar);

        friend Scalar operator++(Scalar);
        friend Scalar operator++(Scalar, int);
        friend Scalar operator--(Scalar);
        friend Scalar operator--(Scalar, int);

        friend Scalar operator==(Scalar, Scalar);
        friend Scalar operator!=(Scalar, Scalar);
        friend Scalar operator<(Scalar, Scalar);
        friend Scalar operator<=(Scalar, Scalar);
        friend Scalar operator>(Scalar, Scalar);
        friend Scalar operator>=(Scalar, Scalar);

        friend Scalar operator&&(Scalar, Scalar);
        friend Scalar operator||(Scalar, Scalar);

        Value _value;
    };

} // namespace value
} // namespace ell

#pragma region implementation

namespace ell
{
namespace value
{
    template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, void*>>
    Scalar Cast(T t, ValueType type)
    {
        switch (type)
        {
        case ValueType::Boolean:
            return Scalar(static_cast<utilities::Boolean>(t));
        case ValueType::Byte:
            return Scalar(static_cast<uint8_t>(t));
        case ValueType::Char8:
            return Scalar(static_cast<char>(t));
        case ValueType::Int16:
            return Scalar(static_cast<int16_t>(t));
        case ValueType::Int32:
            return Scalar(static_cast<int32_t>(t));
        case ValueType::Int64:
            return Scalar(static_cast<int64_t>(t));
        case ValueType::Float:
            return Scalar(static_cast<float>(t));
        case ValueType::Double:
            return Scalar(static_cast<double>(t));
        default:
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState);
        }
    }
} // namespace value
} // namespace ell

#pragma endregion implementation
