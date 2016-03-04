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
    // constants that are only visible from within this file
    namespace
    {
        const std::string lt = "(";
        const std::string rt = ")";
    }

    /// <summary> Class used to get information about class types. </summary>
    ///
    /// <typeparam name="T"> Generic type parameter. </typeparam>
    template<typename T>
    struct TypeInfo
    {
        /// <summary> Gets the serialization name of the type. </summary>
        ///
        /// <returns> The serialization name. </returns>
        static std::string GetSerializationName() { return std::string(T::GetSerializationName()); }
    };

    /// <summary> Class used to get information about shared_ptr types. </summary>
    ///
    /// <typeparam name="T"> Generic type parameter. </typeparam>
    template<typename T>
    struct TypeInfo<std::shared_ptr<T>>
    {
        /// <summary> Gets the serialization name of the type. </summary>
        ///
        /// <returns> The serialization name. </returns>
        static std::string GetSerializationName() { return "ptr" + lt + TypeInfo<T>::GetSerializationName() + rt; }
    };

    /// <summary> Class used to get information about std::vector types. </summary>
    ///
    /// <typeparam name="T"> Generic type parameter. </typeparam>
    template<typename T>
    struct TypeInfo<std::vector<T>>
    {
        /// <summary> Gets the serialization name of the type. </summary>
        ///
        /// <returns> The serialization name. </returns>
        static std::string GetSerializationName() { return "vector" + lt + TypeInfo<T>::GetSerializationName() + rt; }
    };

    /// <summary> Class used to get information about the char type. </summary>
    ///
    /// <typeparam name="T"> Generic type parameter. </typeparam>
    template<>
    struct TypeInfo<char>
    {
        /// <summary> Gets the serialization name of the type. </summary>
        ///
        /// <returns> The serialization name. </returns>
        static std::string GetSerializationName() { return "char"; }
    };

    /// <summary> Class used to get information about the short type. </summary>
    ///
    /// <typeparam name="T"> Generic type parameter. </typeparam>
    template<>
    struct TypeInfo<short>
    {
        /// <summary> Gets the serialization name of the type. </summary>
        ///
        /// <returns> The serialization name. </returns>
        static std::string GetSerializationName() { return "short"; }
    };

    /// <summary> Class used to get information about the unsigned short type. </summary>
    ///
    /// <typeparam name="T"> Generic type parameter. </typeparam>
    template<>
    struct TypeInfo<unsigned short>
    {
        /// <summary> Gets the serialization name of the type. </summary>
        ///
        /// <returns> The serialization name. </returns>
        static std::string GetSerializationName() { return "ushort"; }
    };

    /// <summary> Class used to get information about the int type. </summary>
    ///
    /// <typeparam name="T"> Generic type parameter. </typeparam>
    template<>
    struct TypeInfo<int>
    {
        /// <summary> Gets the serialization name of the type. </summary>
        ///
        /// <returns> The serialization name. </returns>
        static std::string GetSerializationName() { return "int"; }
    };

    /// <summary> Class used to get information about the unsigned int type. </summary>
    ///
    /// <typeparam name="T"> Generic type parameter. </typeparam>
    template<>
    struct TypeInfo<unsigned int>
    {
        /// <summary> Gets the serialization name of the type. </summary>
        ///
        /// <returns> The serialization name. </returns>
        static std::string GetSerializationName() { return "uint"; }
    };

    /// <summary> Class used to get information about the long type. </summary>
    ///
    /// <typeparam name="T"> Generic type parameter. </typeparam>
    template<>
    struct TypeInfo<long>
    {
        /// <summary> Gets the serialization name of the type. </summary>
        ///
        /// <returns> The serialization name. </returns>
        static std::string GetSerializationName() { return "long"; }
    };

    /// <summary> Class used to get information about the unsigned long type. </summary>
    ///
    /// <typeparam name="T"> Generic type parameter. </typeparam>
    template<>
    struct TypeInfo<unsigned long>
    {
        /// <summary> Gets the serialization name of the type. </summary>
        ///
        /// <returns> The serialization name. </returns>
        static std::string GetSerializationName() { return "ulong"; }
    };

    /// <summary> Class used to get information about the unit64 type. </summary>
    ///
    /// <typeparam name="T"> Generic type parameter. </typeparam>
    template<>
    struct TypeInfo<uint64>
    {
        /// <summary> Gets the serialization name of the type. </summary>
        ///
        /// <returns> The serialization name. </returns>
        static std::string GetSerializationName() { return "uint64"; }
    };

    /// <summary> Class used to get information about the float type. </summary>
    ///
    /// <typeparam name="T"> Generic type parameter. </typeparam>
    template<>
    struct TypeInfo<float>
    {
        /// <summary> Gets the serialization name of the type. </summary>
        ///
        /// <returns> The serialization name. </returns>
        static std::string GetSerializationName() { return "float"; }
    };

    /// <summary> Class used to get information about the double type. </summary>
    ///
    /// <typeparam name="T"> Generic type parameter. </typeparam>
    template<>
    struct TypeInfo<double>
    {
        /// <summary> Gets the serialization name of the type. </summary>
        ///
        /// <returns> The serialization name. </returns>
        static std::string GetSerializationName() { return "double"; }
    };
}