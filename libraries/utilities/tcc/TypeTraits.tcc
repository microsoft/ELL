////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TypeTraits.tcc (utilities)
//  Authors:  Chuck Jacobs, Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace utilities
{
    //
    // IsVectorType<>
    //
    template <typename ValueType>
    template <typename VectorType>
    constexpr bool IsVectorType<ValueType>::IsVectorChecker(typename VectorType::value_type*, typename std::enable_if_t<std::is_base_of<VectorType, typename std::vector<typename VectorType::value_type>>::value, int>)
    {
        return true;
    }

    template <typename ValueType>
    template <typename VectorType>
    constexpr bool IsVectorType<ValueType>::IsVectorChecker(...)
    {
        return false;
    }
}
}
