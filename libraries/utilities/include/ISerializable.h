////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ISerializable.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ObjectDescription.h"
#include "Variant.h"
#include "TypeName.h"

// stl
#include <string>
#include <ostream>

namespace utilities
{
    class ISerializable
    {
    public:
        virtual ~ISerializable() = default;

        /// <summary> Get an ObjectDescription describing how to serialize this object </summary>
        virtual ObjectDescription GetDescription() const = 0;

//        static std::string GetTypeName() { return "ISerializable"; }
    };

    // helper function
    template <typename T>
    ObjectDescription GetDescription(T&& obj);

    // TODO: put these someplace that makes sense
    template <typename ValueType>
    using IsFundamental = typename std::enable_if_t<std::is_fundamental<ValueType>::value, int>;

    template <typename ValueType>
    using IsClass = typename std::enable_if_t<std::is_class<ValueType>::value, int>;

    //
    // Serializer class
    //
    class Serializer
    {
    public:
        template <typename T>
        void Serialize(T&& obj);

/*
        /// <summary> Serialize fundamental types. </summary>
        ///
        /// <typeparam name="ValueType"> The type being serialized. </typeparam>
        /// <param name="name"> Name of the variable being serialized. </param>
        /// <param name="value"> The variable being serialized. </param>
        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void Serialize(const char* name, const ValueType& value);

        /// <summary> Serialize a unique pointer to a polymorphic class. </summary>
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

        /// <summary> Serialize unnamed fundamental types. </summary>
        ///
        /// <typeparam name="ValueType"> The type being serialized. </typeparam>
        /// <param name="value"> The variable being serialized. </param>
        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void Serialize(const ValueType& value);

        /// <summary> Serialize an unnamed unique pointer to a polymorphic class. </summary>
        ///
        /// <typeparam name="ValueType"> The type pointed to, must be a polymorphic class. </typeparam>
        /// <param name="value"> The variable being serialized. </param>
        template<typename ValueType>
        void Serialize(const std::unique_ptr<ValueType>& value);

        /// <summary> Serialize an unnamed vector. </summary>
        ///
        /// <typeparam name="ElementType"> The type of vector elements being serialized. </typeparam>
        /// <param name="value"> The vector being serialized. </param>
        template<typename ElementType>
        void Serialize(const std::vector<ElementType>& value);

        /// <summary> Serialize an unnamed std::string. </summary>
        ///
        /// <param name="value"> The string being serialized. </param>
        void Serialize(const std::string& value);

        /// <summary> Serialize unnamed class types. </summary>
        ///
        /// <typeparam name="ValueType"> Type being serialized. </typeparam>
        /// <param name="value"> The variable being serialized. </param>
        template <typename ValueType, IsClass<ValueType> concept = 0>
        void Serialize(const ValueType& value);
        */

        protected:
        virtual void SerializeType(std::string typeName) = 0;
        virtual void SerializeField(std::string name, const Variant& variant) = 0;
    };

    class SimpleSerializer : public Serializer
    {
    protected:
        virtual void SerializeType(std::string typeName) override;
        virtual void SerializeField(std::string name, const Variant& variant) override;

    private:
        int _indent = 0;
    };
}

#include "../tcc/ISerializable.tcc"
