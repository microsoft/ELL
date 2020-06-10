////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     EnumFlagHelpers.h (utilities)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#define ELL_DEFINE_ENUM_FLAG_OPERATORS(ENUMTYPE)                                                             \
    inline ENUMTYPE operator|(ENUMTYPE a, ENUMTYPE b)                                                        \
    {                                                                                                        \
        return ENUMTYPE(((std::underlying_type_t<ENUMTYPE>)a) | ((std::underlying_type_t<ENUMTYPE>)b));      \
    }                                                                                                        \
    inline ENUMTYPE& operator|=(ENUMTYPE& a, ENUMTYPE b)                                                     \
    {                                                                                                        \
        return (ENUMTYPE&)(((std::underlying_type_t<ENUMTYPE>&)a) |= ((std::underlying_type_t<ENUMTYPE>)b)); \
    }                                                                                                        \
    inline ENUMTYPE operator&(ENUMTYPE a, ENUMTYPE b)                                                        \
    {                                                                                                        \
        return ENUMTYPE(((std::underlying_type_t<ENUMTYPE>)a) & ((std::underlying_type_t<ENUMTYPE>)b));      \
    }                                                                                                        \
    inline ENUMTYPE& operator&=(ENUMTYPE& a, ENUMTYPE b)                                                     \
    {                                                                                                        \
        return (ENUMTYPE&)(((std::underlying_type_t<ENUMTYPE>&)a) &= ((std::underlying_type_t<ENUMTYPE>)b)); \
    }                                                                                                        \
    inline ENUMTYPE operator~(ENUMTYPE a)                                                                    \
    {                                                                                                        \
        return ENUMTYPE(~((std::underlying_type_t<ENUMTYPE>)a));                                             \
    }                                                                                                        \
    inline ENUMTYPE operator^(ENUMTYPE a, ENUMTYPE b)                                                        \
    {                                                                                                        \
        return ENUMTYPE(((std::underlying_type_t<ENUMTYPE>)a) ^ ((std::underlying_type_t<ENUMTYPE>)b));      \
    }                                                                                                        \
    inline ENUMTYPE& operator^=(ENUMTYPE& a, ENUMTYPE b)                                                     \
    {                                                                                                        \
        return (ENUMTYPE&)(((std::underlying_type_t<ENUMTYPE>&)a) ^= ((std::underlying_type_t<ENUMTYPE>)b)); \
    }
