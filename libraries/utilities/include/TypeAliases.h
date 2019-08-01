////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TypeAliases.h (utilities)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <type_traits>

namespace ell
{
namespace utilities
{
    // macOS seems to alias intptr_t to long, which is different from what they
    // alias for int32_t and int64_t, which are int and long long,
    // respectively. We will now use a custom type to alias to either int32_t
    // or int64_t, depending on the value of sizeof(void*). If neither
    // int32_t nor int64_t match the width of void*, then we will raise a
    // static_assert and fail to compile.

    using IntPtrT = std::conditional_t<sizeof(void*) == sizeof(int32_t), int32_t, int64_t>;
    static_assert(sizeof(IntPtrT) == sizeof(void*), "Unsupported architecture");

    using UIntPtrT = std::make_unsigned_t<IntPtrT>;
    static_assert(sizeof(UIntPtrT) == sizeof(void*), "Unsupported architecture");
} // namespace utilities
} // namespace ell
