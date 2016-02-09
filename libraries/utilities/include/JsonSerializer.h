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
    /// A datastructure that enables us to read/write objects, which can red/write itself as a json std::string
    ///
    class JsonSerializer
    {
    public:

        /// Static std::function that loads an object from a JSON formatted file
        ///
        template<typename Type>
        static Type Load(std::istream& is, std::string name);

        /// write shared_ptrs to a class
        ///
        template<typename KeyType, typename ValueType>
        void Write(KeyType key, const std::shared_ptr<ValueType>& ptr, typename std::enable_if<std::is_class<ValueType>::value>::type* concept = nullptr);
        // TODO there are two styles of adding "concepts" in this solution. One of them appears above. unify.

        /// write classes
        ///
        template<typename KeyType, typename ValueType>
        void Write(KeyType key, const ValueType& value, typename std::enable_if<std::is_class<ValueType>::value>::type* concept = nullptr);

        /// write strings
        ///
        template<typename KeyType>
        void Write(KeyType key, const std::string& value);

        /// write fundamental types 
        ///
        template<typename KeyType, typename ValueType>
        void Write(KeyType key, const ValueType& value, typename std::enable_if<std::is_fundamental<ValueType>::value>::type* concept = nullptr);

        /// write vectors
        ///
        template<typename KeyType, typename ValueType>
        void Write(KeyType key, const std::vector<ValueType>& vec);

        /// read shared_ptrs to a class
        ///
        template<typename KeyType, typename ValueType, typename DeserializerType>
        void Read(KeyType key, std::shared_ptr<ValueType>& ptr, DeserializerType deserializer) const;

        /// read classes
        ///
        template<typename KeyType, typename ValueType>
        void Read(KeyType key, ValueType& value, typename std::enable_if<std::is_class<ValueType>::value>::type* concept = nullptr) const;

        /// read strings
        ///
        template<typename KeyType>
        void Read(KeyType key, std::string& value) const;

        /// read fundamental types 
        ///
        template<typename KeyType, typename ValueType>
        void Read(KeyType key, ValueType& value, typename std::enable_if<std::is_fundamental<ValueType>::value>::type* concept = nullptr) const;
    
        /// read fundamental types with a return value (usage: auto x = JsonSerializer.read<int>("x");) 
        ///
        template<typename ValueType, typename KeyType>
        ValueType Read(KeyType key, typename std::enable_if<std::is_default_constructible<ValueType>::value>::type* concept = nullptr) const;

        /// read vectors of shared_ptrs
        ///
        template<typename KeyType, typename ValueType, typename DeserializerType>
        void Read(KeyType key, std::vector<std::shared_ptr<ValueType>>& vec, DeserializerType deserializer) const;

        /// read vectors
        ///
        template<typename KeyType, typename ValueType>
        void Read(KeyType key, std::vector<ValueType>& vec) const;

        /// convert the serialized objects to a std::string 
        ///
        std::string ToString() const;

        /// convert a std::string to a datastructure from which one can read objects 
        ///
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

