////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ValueType.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <utilities/include/Boolean.h>
#include <utilities/include/TypeTraits.h>

#include <cstdint>

namespace ell
{
namespace value
{

    /// <summary> An enumeration of primitive types supported by the value library </summary>
    enum class ValueType
    {
        ///<summary> undefined type </summary>
        Undefined = -1,
        ///<summary> void type </summary>
        Void = 0,
        ///<summary> 1 byte boolean </summary>
        Boolean,
        ///<summary> 1 byte character </summary>
        Char8,
        ///<summary> 1 byte unsigned integer </summary>
        Byte,
        ///<summary> 2 byte signed integer </summary>
        Int16,
        ///<summary> 4 byte signed integer </summary>
        Int32,
        ///<summary> 8 byte signed integer </summary>
        Int64,
        ///<summary> 4 byte floating point </summary>
        Float,
        ///<summary> 8 byte floating point </summary>
        Double,
    };

    /// <summary> An enumeration of unary operations supported by the value library </summary>
    enum class ValueUnaryOperation
    {
        ///<summary> Logical negation </summary>
        LogicalNot,
    };

    /// <summary> An enumeration of binary operations supported by the value library </summary>
    enum class ValueBinaryOperation
    {
        /// <summary> Addition operation </summary>
        add,
        /// <summary> Subtraction operation </summary>
        subtract,
        /// <summary> Multiplication operation </summary>
        multiply,
        /// <summary> Division operation </summary>
        divide,
        /// <summary> Remainder operation </summary>
        modulus,
        logicalAnd,
        logicalOr
    };

    enum class ValueLogicalOperation
    {
        equality,
        inequality,
        lessthan,
        lessthanorequal,
        greaterthan,
        greaterthanorequal
    };

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

} // namespace value
} // namespace ell