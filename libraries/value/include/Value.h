////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Value.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Emittable.h"
#include "ValueOperations.h"
#include "ValueType.h"

// utilities
#include "Boolean.h"
#include "MemoryLayout.h"
#include "TypeTraits.h"
#include "Exception.h"

// stl
#include <initializer_list>
#include <optional>
#include <type_traits>
#include <variant>
#include <vector>

namespace ell
{
namespace value
{

class Value;
class Scalar;

namespace detail
{
using ConstantData =
    std::variant<
        std::vector<utilities::Boolean>,
        std::vector<char>,
        std::vector<uint8_t>,
        std::vector<int16_t>,
        std::vector<int32_t>,
        std::vector<int64_t>,
        std::vector<float>,
        std::vector<double>
    >;

using Undefined = std::monostate;

Value StoreConstantData(ConstantData);
} // detail

/// <summary> Helper template function that does a mapping from C++ type to ValueType </summary>
/// <typeparam name="Ty"> The C++ type that should be used to get the corresponding enum value </typeparam>
/// <returns> The ValueType enum value that matches the C++ type provided </returns>
template <typename Ty>
constexpr ValueType GetValueType()
{
    using T = std::decay_t<utilities::RemoveAllPointersT<Ty>>;

    if constexpr (std::is_same_v<T, bool> || std::is_same_v<T, utilities::Boolean>)
    {
        return ValueType::Boolean;
    }
    else if constexpr (std::is_same_v<T, char>)
    {
        return ValueType::Char8;
    }
    else if constexpr (std::is_same_v<T, uint8_t>)
    {
        return ValueType::Byte;
    }
    else if constexpr (std::is_same_v<T, short>)
    {
        return ValueType::Int16;
    }
    else if constexpr (std::is_same_v<T, int>)
    {
        return ValueType::Int32;
    }
    else if constexpr (std::is_same_v<T, int64_t>)
    {
        return ValueType::Int64;
    }
    else if constexpr (std::is_same_v<T, float>)
    {
        return ValueType::Float;
    }
    else if constexpr (std::is_same_v<T, double>)
    {
        return ValueType::Double;
    }
}

/// <summary> The basic type in the Value library upon which most operations are based. Wraps either C++ data (constant data) or data that is
/// specific to the EmitterContext, specified by the Emittable type. </summary>
class Value
{
    using Boolean = utilities::Boolean;

    template <typename T>
    inline static constexpr bool IsAcceptableDataType =
        (std::is_arithmetic_v<T> || std::is_same_v<std::decay_t<T>, Boolean>) &&
        std::is_same_v<std::decay_t<T>, T>;

    template <typename T>
    inline static constexpr bool IsAcceptableConstantPointerType =
        !std::is_same_v<std::decay_t<T>, Value> &&
        std::is_pointer_v<T> &&
        IsAcceptableDataType<utilities::RemoveAllPointersT<T>>;

    template <typename T, typename = std::enable_if_t<IsAcceptableDataType<T>>>
    using DataType = T;

    template <typename T, size_t N>
    struct AddPointers
    {
        using Type = std::add_pointer_t<typename AddPointers<T, N - 1>::Type>;
    };

    template <typename T>
    struct AddPointers<T, 0>
    {
        using Type = T;
    };

    template <typename T, size_t N>
    using AddPointersT = typename AddPointers<T, N>::Type;

    template <typename Ty>
    struct ReturnValueHelper
    {
    private:
        template <typename T>
        static constexpr auto Helper()
        {
            using namespace std;
            using namespace utilities;

            if constexpr (is_same_v<decay_t<T>, Emittable> || is_same_v<decay_t<T>, const Emittable>)
            {
                return IdentityType<T>{};
            }
            else if constexpr (is_pointer_v<T>)
            {
                static_assert(IsAcceptableDataType<RemoveAllPointersT<T>>);
                return IdentityType<T>{};
            }
        }

    public:
        using Type = typename decltype(Helper<Ty>())::Type;
    };

    template <typename T>
    using GetT = typename ReturnValueHelper<T>::Type;

    template <typename T>
    static constexpr std::pair<ValueType, int> GetValueTypeAndPointerLevel()
    {
        return { GetValueType<T>(), utilities::CountOfPointers<T> };
    }

    using UnderlyingDataType = std::variant<
        detail::Undefined,
        Emittable,
        Boolean*,
        char*,
        uint8_t*,
        int16_t*,
        int32_t*,
        int64_t*,
        float*,
        double*
    >;

    using MemoryLayout = utilities::MemoryLayout;

public:
    /// <summary> Default constructor </summary>
    Value();

    /// <summary> Destructor </summary>
    ~Value();

    /// <summary> Copy constructor </summary>
    Value(const Value&);

    /// <summary> Move constructor </summary>
    Value(Value&&) noexcept;

    /// <summary> Copy assignment operator </summary>
    /// <param name="other"> The instance whose data should be copied </param>
    /// <remarks> Copy assignment has to be context aware. The terms "defined", "empty", and
    /// "constrained" match their definitions of the respective Value member functions.
    /// Shallow copies of data is done in this function. Deep copies are done by the
    /// global EmitterContext object.
    /// * if this is not defined
    ///   * and other is not defined: don't do anything
    ///   * and other is defined: copy everything
    /// * if this is defined and not constrained and empty
    ///   * and other is not defined: throw
    ///   * and other is defined: if types don't match, throw. copy everything
    /// * if this is defined and constrained and empty
    ///   * and other is not defined: throw
    ///   * and other is defined and not constrained and empty: if types don't match, throw.
    ///   * and other is defined and constrained and empty: if types or layout don't match, throw
    ///   * and other is defined and not constrained and not empty: if types don't match, throw. shallow copy of data
    ///   * and other is defined and constrained and not empty: if types or layout don't match, throw. shallow copy of data
    /// * if this is defined and not constrained and not empty:
    ///   * throw
    /// * if this is defined and constrained and not empty:
    ///   * and other does not match type or layout: throw
    ///   * and other matches type and layout: deep copy of data
    /// </remarks>
    Value& operator=(const Value& other);

    /// <summary> Move assignment operator </summary>
    /// <param name="other"> The instance whose data should be moved </param>
    /// <remarks> Behaves similarly to the copy assignment, except other is reset afterwards </remarks>
    Value& operator=(Value&& other);

    /// <summary> Constructor that creates an instance which serves as a placeholder for data that matches the type and layout specified </summary>
    /// <typeparam name="T"> The C++ fundamental type to be the basis of this instance </typeparam>
    /// <param name="layout"> An optional MemoryLayout instance that describes the memory structure of the eventual data to be stored </param>
    template <typename T>
    Value(std::optional<MemoryLayout> layout = {}) :
        Value(GetValueType<T>(), layout)
    {}

    /// <summary> Constructor that creates an instance which serves as a placeholder for data that matches the type and layout specified </summary>
    /// <param name="type"> The type to be the basis of this instance </typeparam>
    /// <param name="layout"> An optional MemoryLayout instance that describes the memory structure of the eventual data to be stored </param>
    Value(ValueType type, std::optional<MemoryLayout> layout = {});

    /// <summary> Constructor that creates an instance that wraps an Emittable instance </summary>
    /// <param name="emittable"> Context-specific data that is to be wrapped </param>
    /// <param name="layout"> An optional MemoryLayout instance that describes the memory structure of the data </param>
    Value(Emittable emittable, std::optional<MemoryLayout> layout = {});

    /// <summary> Constructor that creates an instance from one C++ fundamental type's value and gives it a scalar layout </summary>
    /// <typeparam name="T"> The C++ fundamental type to be the basis of this instance </typeparam>
    /// <param name="t"> The value to be wrapped </param>
    template <typename T>
    Value(DataType<T> t) : Value(std::vector<T>{t}, MemoryLayout{}) {}

    /// <summary> Constructor that creates an instance wrapping a set of constant values </summary>
    /// <typeparam name="T"> The C++ fundamental type to be the basis of this instance </typeparam>
    /// <param name="data"> The constant data </param>>
    /// <param name="layout"> An optional MemoryLayout instance that describes the memory structure of the data </param>
    template <typename T>
    Value(std::initializer_list<DataType<T>> data, std::optional<MemoryLayout> layout = {}) : Value(std::vector<T>(data), layout) {}

    /// <summary> Constructor that creates an instance wrapping a set of constant values </summary>
    /// <typeparam name="T"> The C++ fundamental type to be the basis of this instance </typeparam>
    /// <param name="data"> The constant data </param>>
    /// <param name="layout"> An optional MemoryLayout instance that describes the memory structure of the data </param>
    template <typename T>
    Value(std::vector<DataType<T>> data, std::optional<MemoryLayout> layout = {}) noexcept :
        Value(detail::StoreConstantData(std::move(data)))
    {
        if (layout)
        {
            SetLayout(layout.value());
        }
    }

    /// <summary> Constructor that creates an instance wrapping a set of constant values </summary>
    /// <typeparam name="T"> The C++ fundamental type to be the basis of this instance </typeparam>
    /// <param name="data"> Pointer to the constant data </param>>
    /// <param name="layout"> An optional MemoryLayout instance that describes the memory structure of the data </param>
    template <typename T,
        std::enable_if_t<IsAcceptableConstantPointerType<T>, void*> = nullptr>
    Value(T t, std::optional<MemoryLayout> layout = {}) noexcept :
        _data(reinterpret_cast<std::add_pointer_t<utilities::RemoveAllPointersT<T>>>(t)),
        _type(GetValueTypeAndPointerLevel<T>()),
        _layout(layout)
    {
    }

    /// <summary> Sets the data on an empty Value instance </summary>
    /// <param name="value"> The Value instance from which to get the data </param>
    void SetData(Value value);

    /// <summary> Resets the instance to an undefined, unconstrained, and empty state </summary>
    void Reset();

    /// <summary> Gets the underlying data as the specified type, if possible </summary>
    /// <typeparam name="T"> The type of the data that is being retrieved </typeparam>
    /// <returns> An instance of T if it matches the type stored within the instance, else an exception is thrown </returns>
    template <typename T>
    GetT<std::add_const_t<T>> Get() const
    {
        auto ptr = TryGet<T>();
        if (!ptr.has_value())
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }

        return *ptr;
    }

    /// <summary> Gets the underlying data as the specified type, if possible </summary>
    /// <typeparam name="T"> The type of the data that is being retrieved </typeparam>
    /// <returns> An instance of T if it matches the type stored within the instance, else an exception is thrown </returns>
    template <typename T>
    GetT<T> Get()
    {
        auto ptr = TryGet<T>();
        if (!ptr.has_value())
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }

        return *ptr;
    }

    /// <summary> Gets the underlying data as the specified type, if possible </summary>
    /// <typeparam name="T"> The type of the data that is being retrieved </typeparam>
    /// <returns> A std::optional, holding an instance of T if it matches the type stored within the instance, otherwise empty </returns>
    template <typename T>
    std::optional<GetT<std::add_const_t<T>>> TryGet() const noexcept
    {
        auto ptr = std::get_if<T>(&_data);
        return ptr != nullptr ? std::optional<GetT<std::add_const_t<T>>>{ *ptr } : std::optional<GetT<std::add_const_t<T>>>{};
    }

    /// <summary> Gets the underlying data as the specified type, if possible </summary>
    /// <typeparam name="T"> The type of the data that is being retrieved </typeparam>
    /// <returns> A std::optional, holding an instance of T if it matches the type stored within the instance, otherwise empty </returns>
    template <typename T>
    std::optional<GetT<T>> TryGet() noexcept
    {
        auto ptr = std::get_if<T>(&_data);
        return ptr != nullptr ? std::optional<GetT<T>>{ *ptr } : std::optional<GetT<T>>{};
    }

    /// <summary> Returns true if the instance is defined </summmary>
    bool IsDefined() const;

    /// <summary> Returns true if the instance holds data </summary>
    bool IsEmpty() const;

    /// <summary> Returns true if the instance holds constant data </summary>
    bool IsConstant() const;

    /// <summary> Returns true if the instance's type is an integral type (non-floating point) </summary>
    bool IsIntegral() const;

    /// <summary> Returns true if the instance's type is a boolean </summary>
    bool IsBoolean() const;

    /// <summary> Returns true if the instance's type is a short </summary>
    bool IsInt16() const;

    /// <summary> Returns true if the instance's type is a 32-bit int </summary>
    bool IsInt32() const;

    /// <summary> Returns true if the instance's type is a 64-bit int </summary>
    bool IsInt64() const;

    /// <summary> Returns true if the instance's type is a floating point type </summary>
    bool IsFloatingPoint() const;

    /// <summary> Returns true if the instance's type is a 32-bit float </summary>
    bool IsFloat32() const;

    /// <summary> Returns true if the instance's type is a double </summary>
    bool IsDouble() const;

    /// <summary> Returns true if the instance's type is a pointer </summary>
    bool IsPointer() const;

    /// <summary> Returns true if the instance's pointer level is greater than one and
    /// type is an integral type (non-floating point) </summary>
    bool IsIntegralPointer() const;

    /// <summary> Returns true if the instance's pointer level is greater than one and
    /// type is a boolean </summary>
    bool IsBooleanPointer() const;

    /// <summary> Returns true if the instance's pointer level is greater than one and
    /// type is a short </summary>
    bool IsShortPointer() const;

    /// <summary> Returns true if the instance's pointer level is greater than one and
    /// type is a 32-bit int </summary>
    bool IsInt32Pointer() const;

    /// <summary> Returns true if the instance's pointer level is greater than one and
    /// type is a 64-bit int </summary>
    bool IsInt64Pointer() const;

    /// <summary> Returns true if the instance's pointer level is greater than one and
    /// type is a floating point type </summary>
    bool IsFloatingPointPointer() const;

    /// <summary> Returns true if the instance's pointer level is greater than one and
    /// type is a 32-bit float </summary>
    bool IsFloat32Pointer() const;

    /// <summary> Returns true if the instance's pointer level is greater than one and
    /// type is a double </summary>
    bool IsDoublePointer() const;

    /// <summary> Returns true if the instance has a MemoryLayout </summary>
    bool IsConstrained() const;

    /// <summary> Returns the MemoryLayout if the instance has one, throws otherwise </summary>
    const MemoryLayout& GetLayout() const;

    /// <summary> Returns a new Value instance that's offset by the specified index </summary>
    /// <param name="index"> The value to offset by </param>
    /// <returns> A new Value instance offset from this instance's memory location by the specified index </returns>
    /// <remarks> This operation is not bounds checked against the layout </remarks>
    Value Offset(Value index) const;

    /// <summary> Returns a new Value instance that's offset by the specified index </summary>
    /// <param name="index"> The value to offset by </param>
    /// <returns> A new Value instance offset from this instance's memory location by the specified index </returns>
    /// <remarks> This operation is not bounds checked against the layout </remarks>
    Value Offset(Scalar index) const;

    /// <summary> Returns the type of data represented by this instance </summary>
    ValueType GetBaseType() const;

    /// <summary> Sets the MemoryLayout for this instance </summary>
    /// <param name="layout"> The MemoryLayout to be set on this instance </param>
    void SetLayout(MemoryLayout layout);

    /// <summary> Clear the MemoryLayout, if any, on this instance </summary>
    void ClearLayout();

    /// <summary> Returns the number of pointer indirections on the data referred to by this instance </summary>
    /// <returns> The number of pointer indirections </returns>
    int PointerLevel() const;

    /// <summary> Gets a reference to the underlying data storage </summary>
    /// <returns> The underlying data storage </returns>
    UnderlyingDataType& GetUnderlyingData();

    /// <summary> Gets a reference to the underlying data storage </summary>
    /// <returns> The underlying data storage </returns>
    const UnderlyingDataType& GetUnderlyingData() const;

private:
    UnderlyingDataType _data;

    std::pair<ValueType, int> _type { ValueType::Undefined, 0 };
    std::optional<MemoryLayout> _layout = {};
};

}
}
