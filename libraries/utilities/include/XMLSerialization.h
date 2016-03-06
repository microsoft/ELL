////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     XMLSerialization.h (utilities)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Format.h"

// types
#include "types.h"

// stl
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <type_traits>
#include <memory>

namespace utilities
{
    /// <summary> An XML serializer. </summary>
    class XMLSerializer
    {
    public:

        /// <summary> Serialize fundamental types. </summary>
        ///
        /// <typeparam name="ValueType"> The type being serialized. </typeparam>
        /// <param name="name"> Name of the variable being serialized. </param>
        /// <param name="value"> The variable being serialized. </param>
        template<typename ValueType>
        void Serialize(const char* name, const ValueType& value, typename std::enable_if_t<std::is_fundamental<ValueType>::value>* concept = nullptr);

        /// <summary> Serialize a positive-size vector. </summary>
        ///
        /// <typeparam name="ElementType"> The type of vector elements being serialized. </typeparam>
        /// <param name="name"> Name of the variable being serialized. </param>
        /// <param name="value"> The vector being serialized, must have size>0. </param>
        template<typename ElementType>
        void Serialize(const char* name, const std::vector<ElementType>& value);

        /// <summary> Serialize a shared pointer to a polymorphic class. </summary>
        ///
        /// <typeparam name="ValueType"> The type pointed to, must be a polymorphic class. </typeparam>
        /// <param name="name"> Name of the variable being serialized. </param>
        /// <param name="value"> The variable being serialized. </param>
        template<typename ValueType>
        void Serialize(const char* name, const std::shared_ptr<ValueType>& spValue);

        /// <summary> Serialize class types. </summary>
        ///
        /// <typeparam name="ValueType"> Type being serialized. </typeparam>
        /// <param name="name"> Name of the variable being serialized. </param>
        /// <param name="value"> The variable being serialized. </param>
        template<typename ValueType>
        void Serialize(const char* name, const ValueType& value, typename std::enable_if_t<std::is_class<ValueType>::value>* concept = nullptr);

        /// <summary> Writes the serialization to a stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        void WriteToStream(std::ostream& os) const;
        
    private:

        void Indent();

        uint64 _indentation = 0;
        std::stringstream _stream;
    };

    /// <summary> An XML deserializer. </summary>
    class XMLDeserializer
    {
    public:

        /// <summary> Constructs a XMLDeserializer. </summary>
        ///
        /// <param name="is"> [in,out] The input stream. </param>
        XMLDeserializer(std::istream& is);

        /// <summary> Deserialize fundamental types. </summary>
        ///
        /// <typeparam name="ValueType"> The type being deserialized. </typeparam>
        /// <param name="name"> Name of the variable being deserialized, which is compared to the serialized version. </param>
        /// <param name="value"> [in,out] Reference to the variable being deserialized. </param>
        template<typename ValueType>
        void Deserialize(const char* name, ValueType& value, typename std::enable_if_t<std::is_fundamental<ValueType>::value>* concept = nullptr);

        /// <summary> Deserialize vector types. </summary>
        ///
        /// <typeparam name="ElementType"> The type of vector elements being deserialized. </typeparam>
        /// <param name="name"> Name of the variable being deserialized, which is compared to the serialized version. </param>
        /// <param name="value"> [in,out] Reference to the variable being deserialized. </param>
        template<typename ElementType>
        void Deserialize(const char* name, std::vector<ElementType>& value);

        /// <summary> Deserialize a shared pointer to a polymorphic class. </summary>
        ///
        /// <typeparam name="ValueType"> The type pointed to, must be a polymorphic class. </typeparam>
        /// <param name="name"> Name of the variable being deserialized, which is compared to the serialized version. </param>
        /// <param name="value"> [in,out] Reference to the variable being deserialized. </param>
        template<typename ValueType>
        void Deserialize(const char* name, std::shared_ptr<ValueType>& spValue);

        /// <summary> Deserialize class types. </summary>
        ///
        /// <typeparam name="ValueType"> The type being deserialized. </typeparam>
        /// <param name="name"> Name of the variable being deserialized, which is compared to the serialized version. </param>
        /// <param name="value"> [in,out] Reference to the variable being deserialized. </param>
        template<typename ValueType>
        void Deserialize(const char* name, ValueType& value, typename std::enable_if_t<std::is_class<ValueType>::value>* concept = nullptr);

    private:
        std::string _string;
        const char* _pStr;
    };
}

#include "../tcc/XMLSerialization.tcc"
