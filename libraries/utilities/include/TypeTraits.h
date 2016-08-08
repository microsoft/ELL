////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     TypeTraits.h (utilities)
//  Authors:  Chuck Jacobs, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <type_traits>

namespace utilities
{
    class ISerializable;

    template <typename ValueType>
    class IsVectorType
    {
        template <typename VectorType>
        static constexpr bool IsVectorChecker(typename VectorType::value_type*, typename std::enable_if_t<std::is_base_of<VectorType, typename std::vector<typename VectorType::value_type>>::value, int> = 0)
        {
            return true;
        }

        template <typename VectorType>
        static constexpr bool IsVectorChecker(...)
        {
            return false;
        }

    public:
        static const bool value = IsVectorChecker<ValueType>(0);
    };

    template <typename ValueType>
    using IsFundamental = typename std::enable_if_t<std::is_fundamental<typename std::decay<ValueType>::type>::value, int>;

    template <typename ValueType>
    using IsNotFundamental = typename std::enable_if_t<!std::is_fundamental<typename std::decay<ValueType>::type>::value, int>;

    template <typename ValueType>
    using IsClass = typename std::enable_if_t<std::is_class<ValueType>::value, int>;

    template <typename ValueType>
    using IsSerializable = typename std::enable_if_t<std::is_base_of<ISerializable, typename std::decay<ValueType>::type>::value, int>;

    template <typename ValueType>
    using IsNotSerializable = typename std::enable_if_t<(!std::is_base_of<ISerializable, typename std::decay<ValueType>::type>::value) && (!std::is_fundamental<typename std::decay<ValueType>::type>::value), int>;

    template <typename ValueType>
    using IsVector = typename std::enable_if_t<IsVectorType<typename std::decay<ValueType>::type>::value, int>;

    template <typename ValueType>
    using IsNotVector = typename std::enable_if_t<!IsVectorType<typename std::decay<ValueType>::type>::value, int>;
}
