////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Serialization.h (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TypeFactory.h"
#include "TypeName.h"
#include "Exception.h"
#include "ISerializable.h" // For IsFundamental, etc.

// stl
#include <cstdint>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <type_traits>
#include <memory>

namespace utilities
{
    class Serializer
    {
    public:
        /// <summary> Serialize fundamental types. </summary>
        ///
        /// <typeparam name="ValueType"> The type being serialized. </typeparam>
        /// <param name="name"> Name of the variable being serialized. </param>
        /// <param name="value"> The variable being serialized. </param>
        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void Serialize(const char* name, const ValueType& value);

        /// <summary> Serialize a pointer to a polymorphic class. </summary>
        ///
        /// <typeparam name="ValueType"> The type pointed to, must be a polymorphic class. </typeparam>
        /// <param name="name"> Name of the variable being serialized. </param>
        /// <param name="value"> The variable being serialized. </param>
        template<typename ValueType>
        void Serialize(const char* name, const std::unique_ptr<ValueType>& value);

        /// <summary> Serialize a vector. </summary>
        ///
        /// <typeparam name="ElementType"> The type of vector elements being serialized. </typeparam>
        /// <param name="name"> Name of the variable being serialized. </param>
        /// <param name="value"> The vector being serialized. </param>
        template<typename ElementType>
        void Serialize(const char* name, const std::vector<ElementType>& value);

        /// <summary> Serialize a std::string. </summary>
        ///
        /// <param name="name"> Name of the string being serialized. </param>
        /// <param name="value"> The string being serialized. </param>
        void Serialize(const char* name, const std::string& value);

        /// <summary> Serialize class types. </summary>
        ///
        /// <typeparam name="ValueType"> Type being serialized. </typeparam>
        /// <param name="name"> Name of the variable being serialized. </param>
        /// <param name="value"> The variable being serialized. </param>
        template <typename ValueType, IsClass<ValueType> concept = 0>
        void Serialize(const char* name, const ValueType& value);

        // /// <summary> Serialize unnamed fundamental types. </summary>
        // ///
        // /// <typeparam name="ValueType"> The type being serialized. </typeparam>
        // /// <param name="value"> The variable being serialized. </param>
        // template <typename ValueType, IsFundamental<ValueType> concept = 0>
        // void Serialize(const ValueType& value);

        // /// <summary> Serialize an unnamed unique pointer to a polymorphic class. </summary>
        // ///
        // /// <typeparam name="ValueType"> The type pointed to, must be a polymorphic class. </typeparam>
        // /// <param name="value"> The variable being serialized. </param>
        // template<typename ValueType>
        // void Serialize(const std::unique_ptr<ValueType>& value);

        // /// <summary> Serialize an unnamed vector. </summary>
        // ///
        // /// <typeparam name="ElementType"> The type of vector elements being serialized. </typeparam>
        // /// <param name="value"> The vector being serialized. </param>
        // template<typename ElementType>
        // void Serialize(const std::vector<ElementType>& value);

        // /// <summary> Serialize an unnamed std::string. </summary>
        // ///
        // /// <param name="value"> The string being serialized. </param>
        // void Serialize(const std::string& value);

        // /// <summary> Serialize unnamed class types. </summary>
        // ///
        // /// <typeparam name="ValueType"> Type being serialized. </typeparam>
        // /// <param name="value"> The variable being serialized. </param>
        // template <typename ValueType, IsClass<ValueType> concept = 0>
        // void Serialize(const ValueType& value);
    
        /// <summary> Serialize unnamed values of any serializable type. </summary>
        template <typename ValueType>
        void Serialize(ValueType&& value)
        {
            Serialize("", value);
        }
    };

    /// <summary> A deserializer. </summary>
    class Deserializer
    {
    public:
        /// <summary> Deserialize fundamental types. </summary>
        ///
        /// <typeparam name="ValueType"> The type being deserialized. </typeparam>
        /// <param name="name"> Name of the variable being deserialized, which is compared to the serialized version. </param>
        /// <param name="value"> [in,out] Reference to the variable being deserialized. </param>
        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void Deserialize(const char* name, ValueType& value);

        /// <summary> Deserialize a unique pointer to a polymorphic class using the default type factory. </summary>
        ///
        /// <typeparam name="ValueType"> The type pointed to, must be a polymorphic class. </typeparam>
        /// <param name="name"> Name of the variable being deserialized, which is compared to the serialized version. </param>
        /// <param name="value"> [in,out] Reference to the variable being deserialized. </param>
        template<typename BaseType>
        void Deserialize(const char* name, std::unique_ptr<BaseType>& value); 

        /// <summary> Deserialize a unique pointer to a polymorphic class using the supplied type factory. </summary>
        ///
        /// <typeparam name="ValueType"> The type pointed to, must be a polymorphic class. </typeparam>
        /// <param name="name"> Name of the variable being deserialized, which is compared to the serialized version. </param>
        /// <param name="value"> [in,out] Reference to the variable being deserialized. </param>
        /// <param name="factory"> Const reference to the type factory to use to construct the object being deserialized. </param>
        template<typename BaseType>
        void Deserialize(const char* name, std::unique_ptr<BaseType>& value, const TypeFactory<BaseType>& factory);

        /// <summary> Deserialize vector of unique_ptr to polymorphic class. </summary>
        ///
        /// <typeparam name="BaseType"> Type of the polymorphic base class. </typeparam>
        /// <param name="name"> The name of the vector. </param>
        /// <param name="value"> [in,out] The vector to deserialize. </param>
        template<typename BaseType>
        void Deserialize(const char* name, std::vector<std::unique_ptr<BaseType>>& value);

        /// <summary> Deserialize vector of unique_ptr to polymorphic class. </summary>
        ///
        /// <typeparam name="BaseType"> Type of the polymorphic base class. </typeparam>
        /// <param name="name"> The name of the vector. </param>
        /// <param name="value"> [in,out] The vector to deserialize. </param>
        /// <param name="factory"> A type factory for the polymorphic base class. </param>
        template<typename BaseType>
        void Deserialize(const char* name, std::vector<std::unique_ptr<BaseType>>& value, const TypeFactory<BaseType>& factory);

        /// <summary> Deserialize vector types. </summary>
        ///
        /// <typeparam name="ElementType"> The type of vector elements being deserialized. </typeparam>
        /// <param name="name"> Name of the variable being deserialized, which is compared to the serialized version. </param>
        /// <param name="value"> [in,out] Reference to the variable being deserialized. </param>
        template<typename ElementType>
        void Deserialize(const char* name, std::vector<ElementType>& value);

        /// <summary> Deserialize a string. </summary>
        ///
        /// <param name="name"> Name of the string being deserialized, which is compared to the serialized version. </param>
        /// <param name="value"> [in,out] Reference to the string being deserialized. </param>
        void Deserialize(const char* name, std::string& value);

        /// <summary> Deserialize class types. </summary>
        ///
        /// <typeparam name="ValueType"> The type being deserialized. </typeparam>
        /// <param name="name"> Name of the variable being deserialized, which is compared to the serialized version. </param>
        /// <param name="value"> [in,out] Reference to the variable being deserialized. </param>
        template <typename ValueType, IsClass<ValueType> concept = 0>
        void Deserialize(const char* name, ValueType& value);

        /// <summary> Deserialize unnamed types </summary>
        ///
        /// <typeparam name="ValueType"> The type being deserialized. </typeparam>
        /// <param name="value"> [out] Reference to the variable being deserialized. </param>
        template <typename ValueType>
        void Deserialize(ValueType&& value)
        {
            Deserialize("", value);
        }

        // /// <summary> Deserialize unnamed fundamental types. </summary>
        // ///
        // /// <typeparam name="ValueType"> The type being deserialized. </typeparam>
        // /// <param name="value"> [in,out] Reference to the variable being deserialized. </param>
        // template <typename ValueType, IsFundamental<ValueType> concept = 0>
        // void Deserialize(ValueType& value);

        // /// <summary> Deserialize an unnamed unique pointer to a polymorphic class. </summary>
        // ///
        // /// <typeparam name="BaseType"> The type pointed to, must be a polymorphic class. </typeparam>
        // /// <param name="value"> [in,out] Reference to the variable being deserialized. </param>
        // /// <param name="factory"> A TypeFactory that constructs instances of this polymorphic class. </param>
        // template<typename BaseType>
        // void Deserialize(std::unique_ptr<BaseType>& value, const TypeFactory<BaseType>& factory); 

        // /// <summary> Deserialize an unnamed vector of unique pointers to a polymorphic class. </summary>
        // ///
        // /// <typeparam name="BaseType"> The type pointed to, must be a polymorphic class. </typeparam>
        // /// <param name="value"> [in,out] Reference to the vector being deserialized. </param>
        // /// <param name="factory"> A TypeFactory that constructs instances of this polymorphic class. </param>
        // template<typename BaseType>
        // void Deserialize(std::vector<std::unique_ptr<BaseType>>& value, const TypeFactory<BaseType>& factory);

        // /// <summary> Deserialize unnamed vector types. </summary>
        // ///
        // /// <typeparam name="ElementType"> The type of vector elements being deserialized. </typeparam>
        // /// <param name="value"> [in,out] Reference to the variable being deserialized. </param>
        // template<typename ElementType>
        // void Deserialize(std::vector<ElementType>& value);

        // /// <summary> Deserialize an unnamed string. </summary>
        // ///
        // /// <param name="value"> [in,out] Reference to the string being deserialized. </param>
        // void Deserialize(std::string& value);

        // /// <summary> Deserialize unnamed class types. </summary>
        // ///
        // /// <typeparam name="ValueType"> The type being deserialized. </typeparam>
        // /// <param name="value"> [in,out] Reference to the variable being deserialized. </param>
        // template <typename ValueType, IsClass<ValueType> concept = 0>
        // void Deserialize(ValueType& value);
    };
}

#include "../tcc/Serialization.tcc"
