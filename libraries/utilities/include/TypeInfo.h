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
#include <string>

namespace utilities
{

    const std::string lt = "(";
    const std::string rt = ")";

    template<typename T>
    struct TypeInfo
    {
        static std::string GetSerializationName() { return std::string(T::GetSerializationName()); }
    };

    template<typename T>
    struct TypeInfo<std::shared_ptr<T>>
    {
        static std::string GetSerializationName() { return "ptr" + lt + TypeInfo<T>::GetSerializationName() + rt; }
    };

    template<typename T>
    struct TypeInfo<std::vector<T>>
    {
        static std::string GetSerializationName() { return "vector" + lt + TypeInfo<T>::GetSerializationName() + rt; }
    };

    template<>
    struct TypeInfo<char>
    {
        static std::string GetSerializationName() { return "char"; }
    };

    template<>
    struct TypeInfo<short>
    {
        static std::string GetSerializationName() { return "short"; }
    };

    template<>
    struct TypeInfo<unsigned short>
    {
        static std::string GetSerializationName() { return "ushort"; }
    };

    template<>
    struct TypeInfo<int>
    {
        static std::string GetSerializationName() { return "int"; }
    };

    template<>
    struct TypeInfo<unsigned int>
    {
        static std::string GetSerializationName() { return "uint"; }
    };

    template<>
    struct TypeInfo<long>
    {
        static std::string GetSerializationName() { return "long"; }
    };

    template<>
    struct TypeInfo<unsigned long>
    {
        static std::string GetSerializationName() { return "ulong"; }
    };

    template<>
    struct TypeInfo<uint64>
    {
        static std::string GetSerializationName() { return "uint64"; }
    };

    template<>
    struct TypeInfo<float>
    {
        static std::string GetSerializationName() { return "float"; }
    };

    template<>
    struct TypeInfo<double>
    {
        static std::string GetSerializationName() { return "double"; }
    };
}