////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     XMLSerialization.h (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Format.h"
#include "TypeFactory.h"
#include "Serializer.h"  // Just to get the IsFundamental and IsClass definitions

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
    /// <summary>
    /// The XMLSerializer and XMLDeserializer classes facilitate serialization and
    /// deserialization of some fundamental types, `std::strings`, `std::vectors`, `std::unique_ptrs`, and
    /// classes that implement the required functions. Serializing a couple of variables to the
    /// string stream is as simple as
    /// 
    ///     double x = 5.3;
    ///     uint64_t y = 12;
    ///     std::stringstream stream;
    ///     XMLSerializer serializer(stream);
    ///     serializer.Serialize(x);
    ///     serializer.Serialize(y);
    /// 
    /// Deserialization must occur in the same order.
    /// 
    ///     XMLDeserializer deserializer(stream);
    ///     double xx;
    ///     uint64_t yy;
    ///     deserialize(xx);
    ///     deserialize(yy);
    ///     assert(x == xx &amp;&amp; y == yy);
    /// 
    /// The XMLSerializer also supports serialization of named variables, in which case the
    /// deserialization must specify the correct variable name.
    /// 
    ///     x = 0.4;
    ///     serializer.Serialize("x", x);
    ///     deserialize("x", xx);
    ///     assert(x == xx);
    /// 
    /// Serialization of `std::strings` and `std::vectors` of fundamental types is similar.
    /// 
    /// To make a class serializable, the following public members are required:
    /// 
    ///     class Bar
    ///     {
    ///     public: 
    ///         Bar();
    ///         void Read(utilities::XMLDeserializer&amp; deserializer);
    ///         void Write(utilities::XMLSerializer&amp; serializer) const;
    ///     }
    /// 
    /// A typical implementation of Read() will include a sequence of calls to
    /// deserializer.Deserialize(). A typical implementation of Write will include a sequence of
    /// calls to serializer.Serialize(), in the same order. To serialize the class, call:
    /// 
    ///     Bar z;
    ///     serializer.Serialize("z", z);
    /// 
    /// This class also supports serializing and deserialization of std::unique_pointers to a
    /// polymorphic base class. For this, say that we have a base class named Base. The base class is
    /// required to have the following two public static members, and there pure virtual functions:
    /// 
    ///     class Base
    ///     {
    ///     public: 
    ///         static std::string GetTypeName();
    ///         static const utilities::TypeFactory&lt;Layer&gt; GetTypeFactory();
    ///         virtual std::string GetRuntimeTypeName() const = 0;
    ///         virtual void Read(utilities::XMLDeserializer&amp; deserializer) = 0;
    ///         virtual void Write(utilities::XMLSerializer&amp; serializer) const = 0;
    ///     };
    /// 
    /// `GetTypeFactory()` constructs a factory that maps the names (strings) of classes that derive
    /// from Base to their default constructors. The deserializer relies on this factory to construct
    /// the correct derived type. Now, classes derived from Base must implement the pure virtual
    /// functions defined in Base.
    /// 
    /// Finally, the XMLSerialization classes also support serialization and deserialization of vectors of unique_ptrs.
    /// </summary>
    class XMLSerializer
    {
    public:

        /// <summary> Constructs an instance of XMLSerializer. </summary>
        ///
        /// <param name="stream"> [in,out] An output stream. </param>
        XMLSerializer(std::ostream& stream);

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

    private:
        template<typename ValueType>
        void WriteSingleLineTags(const std::string& tagName, const ValueType& value);

        template<typename ValueType>
        void WriteSingleLineTags(const std::string& tagName, const std::string& name, const ValueType& value);

        void WriteOpenTag(const std::string& tagName);

        template <typename ValueType>
        void WriteOpenTag(const std::string& tagName, const std::string& attributeName, const ValueType& attributeValue);

        template <typename ValueType1, typename ValueType2>
        void WriteOpenTag(const std::string& tagName, const std::string& attributeName1, const ValueType1& attributeValue1, const std::string& attributeName2, const ValueType2& attributeValue2);

        void WriteCloseTag(const std::string& tagName);

        void Indent();

        uint64_t _indentation = 0;
        std::ostream& _stream;
    };

    /// <summary> An XML deserializer. </summary>
    class XMLDeserializer
    {
    public:

        /// <summary> Constructs a XMLDeserializer. </summary>
        ///
        /// <param name="iStream"> [in,out] The input stream. </param>
        XMLDeserializer(std::istream& stream);

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

        /// <summary> Deserialize unnamed fundamental types. </summary>
        ///
        /// <typeparam name="ValueType"> The type being deserialized. </typeparam>
        /// <param name="value"> [in,out] Reference to the variable being deserialized. </param>
        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void Deserialize(ValueType& value);

        /// <summary> Deserialize an unnamed unique pointer to a polymorphic class. </summary>
        ///
        /// <typeparam name="BaseType"> The type pointed to, must be a polymorphic class. </typeparam>
        /// <param name="value"> [in,out] Reference to the variable being deserialized. </param>
        /// <param name="factory"> A TypeFactory that constructs instances of this polymorphic class. </param>
        template<typename BaseType>
        void Deserialize(std::unique_ptr<BaseType>& value, const TypeFactory<BaseType>& factory); 

        /// <summary> Deserialize an unnamed vector of unique pointers to a polymorphic class. </summary>
        ///
        /// <typeparam name="BaseType"> The type pointed to, must be a polymorphic class. </typeparam>
        /// <param name="value"> [in,out] Reference to the vector being deserialized. </param>
        /// <param name="factory"> A TypeFactory that constructs instances of this polymorphic class. </param>
        template<typename BaseType>
        void Deserialize(std::vector<std::unique_ptr<BaseType>>& value, const TypeFactory<BaseType>& factory);

        /// <summary> Deserialize unnamed vector types. </summary>
        ///
        /// <typeparam name="ElementType"> The type of vector elements being deserialized. </typeparam>
        /// <param name="value"> [in,out] Reference to the variable being deserialized. </param>
        template<typename ElementType>
        void Deserialize(std::vector<ElementType>& value);

        /// <summary> Deserialize an unnamed string. </summary>
        ///
        /// <param name="value"> [in,out] Reference to the string being deserialized. </param>
        void Deserialize(std::string& value);

        /// <summary> Deserialize unnamed class types. </summary>
        ///
        /// <typeparam name="ValueType"> The type being deserialized. </typeparam>
        /// <param name="value"> [in,out] Reference to the variable being deserialized. </param>
        template <typename ValueType, IsClass<ValueType> concept = 0>
        void Deserialize(ValueType& value);

    private:

        template <typename TagType>
        void ReadOpenTag(TagType&& tagName);

        template <typename TagType, typename NameType, typename AttributeType>
        void ReadOpenTag(TagType&& tagName, NameType&& attributeName, AttributeType&& attributeValue);

        template <typename TagType, typename NameType1, typename AttributeType1, typename NameType2, typename AttributeType2>
        void ReadOpenTag(TagType&& tagName, NameType1&& attributeName1, AttributeType1&& attributeValue1, NameType2&& attributeName2, AttributeType2&& attributeValue2);

        template <typename TagType>
        void ReadCloseTag(TagType&& tagName);

        template<typename TagType, typename ValueType>
        void ReadSingleLineTags(TagType&& tagName, ValueType&& value);

        template<typename TagType, typename NameType, typename AttributeType, typename ValueType>
        void ReadSingleLineTags(TagType&& tagName, NameType&& attributeName, AttributeType attributeValue, ValueType&& value);

        std::string _string;
        const char* _pStr;
    };
}

#include "../tcc/XMLSerialization.tcc"
