////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TypeTraits.h (utilities)
//  Authors:  Chuck Jacobs, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <type_traits>
#include <vector>

namespace ell
{
namespace utilities
{
    /// <summary> Utility class to test if a type is a specialization of std::vector. Used by IsVector/IsNotVector, below. </summary>
    template <typename ValueType>
    class IsVectorType
    {
    private:
        template <typename VectorType>
        static constexpr bool IsVectorChecker(typename VectorType::value_type*, typename std::enable_if_t<std::is_base_of<VectorType, typename std::vector<typename VectorType::value_type>>::value, int> = 0);

        template <typename VectorType>
        static constexpr bool IsVectorChecker(...);

    public:
        static const bool value = IsVectorChecker<ValueType>(0);
    };

    /// <summary> Enabled if ValueType is a boolean. </summary>
    template <typename ValueType>
    using IsBoolean = typename std::enable_if_t<std::is_same<std::decay_t<ValueType>, bool>::value, bool>;

    /// <summary> Enabled if ValueType is a class. </summary>
    template <typename ValueType>
    using IsClass = typename std::enable_if_t<std::is_class<ValueType>::value, bool>;

    /// <summary> Enabled if the two classes are different. </summary>
    template <typename T, typename S>
    using IsDifferent = typename std::enable_if_t<!std::is_same<T, S>::value, bool>;

    /// <summary> Enabled if ValueType is an enum. </summary>
    template <typename ValueType>
    using IsEnum = typename std::enable_if_t<std::is_enum<ValueType>::value, bool>;

    /// <summary> Enabled if ValueType is an floating point value. </summary>
    template <typename ValueType>
    using IsFloatingPoint = typename std::enable_if_t<std::is_floating_point<std::decay_t<ValueType>>::value, bool>;

    /// <summary> Enabled if ValueType is a fundamental value. </summary>
    template <typename ValueType>
    using IsFundamental = typename std::enable_if_t<std::is_fundamental<std::decay_t<ValueType>>::value, int>;

    /// <summary> Enabled if ValueType is an integer value. </summary>
    template <typename ValueType>
    using IsIntegral = typename std::enable_if_t<std::is_integral<std::decay_t<ValueType>>::value, bool>;

    /// <summary> Enabled if ValueType is an integer value that's not a boolean. </summary>
    template <typename ValueType>
    using IsNonBooleanIntegral = typename std::enable_if_t<std::is_integral<std::decay_t<ValueType>>::value && !std::is_same<std::decay_t<ValueType>, bool>::value, bool>;

    /// <summary> Enabled if ValueType is not a fundamental value. </summary>
    template <typename ValueType>
    using IsNotFundamental = typename std::enable_if_t<!std::is_fundamental<std::decay_t<ValueType>>::value, int>;

    /// <summary> Enabled if ValueType is not a specialization of std::vector. </summary>
    template <typename ValueType>
    using IsNotVector = typename std::enable_if_t<!IsVectorType<std::decay_t<ValueType>>::value, int>;

    /// <summary> Enabled if the two classes are the same. </summary>
    template <typename T, typename S>
    using IsSame = typename std::enable_if_t<std::is_same<T, S>::value, bool>;

    /// <summary> Enabled if ValueType is a specialization of std::vector. </summary>
    template <typename ValueType>
    using IsVector = typename std::enable_if_t<IsVectorType<std::decay_t<ValueType>>::value, int>;
}
}

#include "../tcc/TypeTraits.tcc"