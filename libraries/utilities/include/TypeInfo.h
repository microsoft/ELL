////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     TypeInfo.h (utilities)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "types.h"

// stl
#include <vector>
#include <memory>

namespace utilities
{
    template<typename T>
    struct TypeInfo
    {
        static const char* GetSerializationName() { return T::GetSerializationName(); }
    };

    template<typename T>
    struct TypeInfo<std::shared_ptr<T>>
    {
        static const char* GetSerializationName() { return "*"; }
    };

    template<typename T>
    struct TypeInfo<std::vector<T>>
    {
        static const char* GetSerializationName() { return "Vector"; }
    };

    template<>
    struct TypeInfo<int>
    {
        static const char* GetSerializationName() { return "int"; }
    };

    template<>
    struct TypeInfo<uint64>
    {
        static const char* GetSerializationName() { return "uint64"; }
    };

    template<>
    struct TypeInfo<float>
    {
        static const char* GetSerializationName() { return "float"; }
    };

    template<>
    struct TypeInfo<double>
    {
        static const char* GetSerializationName() { return "double"; }
    };
}