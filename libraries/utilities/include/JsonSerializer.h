////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     JsonSerializer.h (utilities)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../../amalgamated_jsoncpp/json/json.h"

// types
#include "types.h"

// stl
#include <type_traits>
#include <functional>
#include <string>
#include <memory>
#include <vector>
#include <iostream>

namespace utilities
{
    // A datastructure that enables us to read/write objects, which can red/write itself as a json std::string
    //
    class JsonSerializer
    {
    public:

        /// <summary> Static std::function that loads an object from a JSON formatted file. </summary>
        ///
        /// <typeparam name="Type"> Type of the type. </typeparam>
        /// <param name="is"> [in,out] Stream to read data from. </param>
        /// <param name="name"> The name. </param>
        ///
        /// <returns> A Type. </returns>
        template<typename Type>
        static Type Load(std::istream& is, std::string name);

        /// <summary> write shared_ptrs to a class. </summary>
        ///
        /// <typeparam name="KeyType"> Type of the key type. </typeparam>
        /// <typeparam name="ValueType"> Type of the value type. </typeparam>
        /// <param name="key"> The key. </param>
        /// <param name="ptr"> The pointer. </param>
        /// <param name="concept"> [in,out] (Optional) If non-null, the concept. </param>
        template<typename KeyType, typename ValueType>
        void Write(KeyType key, const std::shared_ptr<ValueType>& ptr, typename std::enable_if<std::is_class<ValueType>::value>::type* concept = nullptr);

        template<typename KeyType, typename ValueType>
        void Write(KeyType key, const std::unique_ptr<ValueType>& ptr, typename std::enable_if<std::is_class<ValueType>::value>::type* concept = nullptr);

        /// <summary> write classes. </summary>
        ///
        /// <typeparam name="KeyType"> Type of the key type. </typeparam>
        /// <typeparam name="ValueType"> Type of the value type. </typeparam>
        /// <param name="key"> The key. </param>
        /// <param name="value"> The value. </param>
        /// <param name="concept"> [in,out] (Optional) If non-null, the concept. </param>
        template<typename KeyType, typename ValueType>
        void Write(KeyType key, const ValueType& value, typename std::enable_if<std::is_class<ValueType>::value>::type* concept = nullptr);

        /// <summary> write strings. </summary>
        ///
        /// <typeparam name="KeyType"> Type of the key type. </typeparam>
        /// <param name="key"> The key. </param>
        /// <param name="value"> The value. </param>
        template<typename KeyType>
        void Write(KeyType key, const std::string& value);

        /// <summary> write fundamental types. </summary>
        ///
        /// <typeparam name="KeyType"> Type of the key type. </typeparam>
        /// <typeparam name="ValueType"> Type of the value type. </typeparam>
        /// <param name="key"> The key. </param>
        /// <param name="value"> The value. </param>
        /// <param name="concept"> [in,out] (Optional) If non-null, the concept. </param>
        template<typename KeyType, typename ValueType>
        void Write(KeyType key, const ValueType& value, typename std::enable_if<std::is_fundamental<ValueType>::value>::type* concept = nullptr);

        /// <summary> write vectors. </summary>
        ///
        /// <typeparam name="KeyType"> Type of the key type. </typeparam>
        /// <typeparam name="ValueType"> Type of the value type. </typeparam>
        /// <param name="key"> The key. </param>
        /// <param name="vec"> The vector. </param>
        template<typename KeyType, typename ValueType>
        void Write(KeyType key, const std::vector<ValueType>& vec);

        /// <summary> Read shared_ptrs to a class. </summary>
        ///
        /// <typeparam name="KeyType"> Type of the key type. </typeparam>
        /// <typeparam name="ValueType"> Type of the value type. </typeparam>
        /// <typeparam name="DeserializerType"> Type of the deserializer type. </typeparam>
        /// <param name="key"> The key. </param>
        /// <param name="ptr"> [in,out] The pointer. </param>
        /// <param name="deserializer"> The deserializer. </param>
        template<typename KeyType, typename ValueType, typename DeserializerType>
        void Read(KeyType key, std::shared_ptr<ValueType>& ptr, DeserializerType deserializer) const;

        template<typename KeyType, typename ValueType, typename DeserializerType>
        void Read(KeyType key, std::unique_ptr<ValueType>& ptr, DeserializerType deserializer) const;

        /// <summary> Read classes. </summary>
        ///
        /// <typeparam name="KeyType"> Type of the key type. </typeparam>
        /// <typeparam name="ValueType"> Type of the value type. </typeparam>
        /// <param name="key"> The key. </param>
        /// <param name="value"> [in,out] The value. </param>
        /// <param name="concept"> [in,out] (Optional) If non-null, the concept. </param>
        template<typename KeyType, typename ValueType>
        void Read(KeyType key, ValueType& value, typename std::enable_if<std::is_class<ValueType>::value>::type* concept = nullptr) const;

        /// <summary> Read strings. </summary>
        ///
        /// <typeparam name="KeyType"> Type of the key type. </typeparam>
        /// <param name="key"> The key. </param>
        /// <param name="value"> [in,out] The value. </param>
        template<typename KeyType>
        void Read(KeyType key, std::string& value) const;

        /// <summary> Read fundamental types. </summary>
        ///
        /// <typeparam name="KeyType"> Type of the key type. </typeparam>
        /// <typeparam name="ValueType"> Type of the value type. </typeparam>
        /// <param name="key"> The key. </param>
        /// <param name="value"> [in,out] The value. </param>
        /// <param name="concept"> [in,out] (Optional) If non-null, the concept. </param>
        template<typename KeyType, typename ValueType>
        void Read(KeyType key, ValueType& value, typename std::enable_if<std::is_fundamental<ValueType>::value>::type* concept = nullptr) const;

        /// <summary> Read fundamental types with a return value (usage: auto x =
        /// JsonSerializer.read<int>("x");) </summary>
        ///
        /// <typeparam name="ValueType"> Type of the value type. </typeparam>
        /// <typeparam name="KeyType"> Type of the key type. </typeparam>
        /// <param name="key"> The key. </param>
        /// <param name="concept"> The concept. </param>
        ///
        /// <returns> A ValueType. </returns>
        template<typename ValueType, typename KeyType>
        ValueType Read(KeyType key, typename std::enable_if<std::is_default_constructible<ValueType>::value>::type* concept = nullptr) const;

        /// <summary> Read vectors of shared_ptrs. </summary>
        ///
        /// <typeparam name="KeyType"> Type of the key type. </typeparam>
        /// <typeparam name="ValueType"> Type of the value type. </typeparam>
        /// <typeparam name="DeserializerType"> Type of the deserializer type. </typeparam>
        /// <param name="key"> The key. </param>
        /// <param name="vec"> [in,out] The vector. </param>
        /// <param name="deserializer"> The deserializer. </param>
        template<typename KeyType, typename ValueType, typename DeserializerType>
        void Read(KeyType key, std::vector<std::shared_ptr<ValueType>>& vec, DeserializerType deserializer) const;

        template<typename KeyType, typename ValueType, typename DeserializerType>
        void Read(KeyType key, std::vector<std::unique_ptr<ValueType>>& vec, DeserializerType deserializer) const;


        /// <summary> Read vectors. </summary>
        ///
        /// <typeparam name="KeyType"> Type of the key type. </typeparam>
        /// <typeparam name="ValueType"> Type of the value type. </typeparam>
        /// <param name="key"> The key. </param>
        /// <param name="vec"> [in,out] The vector. </param>
        template<typename KeyType, typename ValueType>
        void Read(KeyType key, std::vector<ValueType>& vec) const;

        /// <summary> Convert the serialized objects to a std::string. </summary>
        ///
        /// <returns> A std::string that represents this object. </returns>
        std::string ToString() const;

        /// <summary> Convert a std::string to a datastructure from which one can read objects. </summary>
        ///
        /// <param name="s"> The std::string to process. </param>
        void FromString(const std::string& s);

    private:
        Json::Value _json_value;

        template<typename KeyType>
        void Get(KeyType key, bool& value) const;

        template<typename KeyType>
        void Get(KeyType key, int& value) const;

        template<typename KeyType>
        void Get(KeyType key, unsigned int& value) const;

        template<typename KeyType>
        void Get(KeyType key, uint64& value) const;

        template<typename KeyType>
        void Get(KeyType key, float& value) const;

        template<typename KeyType>
        void Get(KeyType key, double& value) const;

        template<typename KeyType>
        void Get(KeyType key, std::string& value) const;
    };
}

#include "../tcc/JsonSerializer.tcc"

