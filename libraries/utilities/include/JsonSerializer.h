/// JsonSerializer.h

#pragma once

#include "../../amalgamated_jsoncpp/json/json.h"

#include "types.h"

#include <type_traits>
using std::enable_if;
using std::is_class;
using std::is_enum;
using std::is_fundamental;
using std::is_default_constructible;

#include <string>
using std::string;

#include <memory>
using std::shared_ptr;

#include <vector>
using std::vector;

namespace utilities
{
    /// A datastructure that enables us to read/write objects, which can red/write itself as a json string
    ///
    class JsonSerializer
    {
    public:

        /// write classes
        ///
        template<typename KeyType, typename ValueType>
        void Write(KeyType key, const ValueType& value, typename enable_if<is_class<ValueType>::value>::type* SFINAE = nullptr);

        /// write strings
        ///
        template<typename KeyType>
        void Write(KeyType key, const string& value);

        /// write unique_ptrs to a classe
        ///
        template<typename KeyType, typename ValueType>
        void Write(KeyType key, const shared_ptr<ValueType>& ptr, typename enable_if<is_class<ValueType>::value>::type* SFINAE = nullptr);

        /// write fundamental types 
        ///
        template<typename KeyType, typename ValueType>
        void Write(KeyType key, const ValueType& value, typename enable_if<is_fundamental<ValueType>::value>::type* SFINAE = nullptr);

        /// write an enum type by casting it to int
        ///
        template<typename KeyType, typename ValueType>
        void Write(KeyType key, const ValueType& value, typename enable_if<is_enum<ValueType>::value>::type* SFINAE = nullptr);

        /// write vectors
        ///
        template<typename KeyType, typename ValueType>
        void Write(KeyType key, const vector<ValueType>& vec);

        /// read subclasses
        ///
        template<typename KeyType, typename ValueType>
        void Read(KeyType key, ValueType& value, typename enable_if<is_class<ValueType>::value>::type* SFINAE = nullptr) const;

        /// read strings
        ///
        template<typename KeyType>
        void Read(KeyType key, string& value) const;

        /// read unique_ptrs to a class
        ///
        template<typename KeyType, typename ValueType>
        void Read(KeyType key, shared_ptr<ValueType>& ptr, typename enable_if<is_class<ValueType>::value>::type* SFINAE = nullptr) const;
    
        /// read fundamental types 
        ///
        template<typename KeyType, typename ValueType>
        void Read(KeyType key, ValueType& value, typename enable_if<is_fundamental<ValueType>::value>::type* SFINAE = nullptr) const;
    
        /// read fundamental types with a return value (usage: auto x = JsonSerializer.read<int>("x");) 
        ///
        template<typename ValueType, typename KeyType>
        ValueType Read(KeyType key, typename enable_if<is_default_constructible<ValueType>::value>::type* SFINAE = nullptr) const;

        /// read an enum type by casting it from int
        ///
        template<typename KeyType, typename ValueType>
        void Read(KeyType key, ValueType& value, typename enable_if<is_enum<ValueType>::value>::type* SFINAE = nullptr) const;
        
        /// read an enum type by casting it from int, with a return value
        ///
        template<typename ValueType, typename KeyType>
        ValueType Read(KeyType key, typename enable_if<is_enum<ValueType>::value>::type* SFINAE = nullptr) const;
    
        /// read vectors
        ///
        template<typename KeyType, typename ValueType>
        void Read(KeyType key, vector<ValueType>& vec) const;

        /// convert the serialized objects to a string 
        ///
        string ToString() const;

        /// convert a string to a datastructure from which one can read objects 
        ///
        void FromString(const string& s);

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
        void Get(KeyType key, string& value) const;
    };
}

#include "../tcc/JsonSerializer.tcc"

