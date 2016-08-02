////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Serialization.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TypeFactory.h"
#include "TypeName.h"
#include "Exception.h"

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
    class SerializationContext
    {
    public:
        virtual ~SerializationContext() = default;
    };

    // TODO: put all this stuff somewhere else
    template <typename ValueType>
    class is_vector
    {
        template <typename VectorType>
        static constexpr bool is_vector_checker(typename VectorType::value_type*, typename std::enable_if_t<std::is_base_of<VectorType, typename std::vector<typename VectorType::value_type>>::value, int> = 0)
        {
            return true;
        }

        template <typename VectorType>
        static constexpr bool is_vector_checker(...)
        {
            return false;
        }

    public:
        static const bool value = is_vector_checker<ValueType>(0);
    };

    class ISerializable;

    template <typename ValueType>
    using IsFundamental = typename std::enable_if_t<std::is_fundamental<typename std::decay<ValueType>::type>::value, int>;

    template <typename ValueType>
    using IsNotFundamental = typename std::enable_if_t<!std::is_fundamental<typename std::decay<ValueType>::type>::value, int>;

    template <typename ValueType>
    using IsClass = typename std::enable_if_t<std::is_class<ValueType>::value, int>;

    template <typename ValueType>
    using IsSerializable = typename std::enable_if_t<std::is_base_of<ISerializable, typename std::decay<ValueType>::type>::value, int>;

    template <typename ValueType>
    using IsNotSerializable = typename std::enable_if_t<(!std::is_base_of<ISerializable, typename std::decay<ValueType>::type>::value) && (!std::is_fundamental<typename std::decay<ValueType>::type>::value), int>;

    template <typename ValueType>
    using IsVector = typename std::enable_if_t<is_vector<typename std::decay<ValueType>::type>::value, int>;

    template <typename ValueType>
    using IsNotVector = typename std::enable_if_t<!is_vector<typename std::decay<ValueType>::type>::value, int>;

#define DECLARE_SERIALIZE_VALUE_BASE(type) virtual void SerializeValue(const char* name, type value, IsFundamental<type> dummy = 0) = 0;
#define DECLARE_SERIALIZE_ARRAY_VALUE_BASE(type) virtual void SerializeArrayValue(const char* name, const std::vector<type>& value, IsFundamental<type> dummy = 0) = 0;
#define DECLARE_SERIALIZE_VALUE_OVERRIDE(type) virtual void SerializeValue(const char* name, type value, IsFundamental<type> dummy = 0) override;
#define DECLARE_SERIALIZE_ARRAY_VALUE_OVERRIDE(type) virtual void SerializeArrayValue(const char* name, const std::vector<type>& value, IsFundamental<type> dummy = 0) override;

    /// <summary>
    /// The Serializer and Deserializer abstract base classes facilitate serialization and
    /// deserialization of some fundamental types, `std::string`s, `std::vector`s, and
    /// classes that implement the required functions. Serializing a couple of variables to a
    /// string stream is as simple as
    /// 
    ///     double x = 5.3;
    ///     uint64_t y = 12;
    ///     std::stringstream stream;
    ///     SerializerType serializer(stream);
    ///     serializer.Serialize(x);
    ///     serializer.Serialize(y);
    /// 
    /// Note: Deserialization must occur in the same order.
    /// 
    ///     DeserializerType deserializer(stream);
    ///     double xx;
    ///     uint64_t yy;
    ///     deserializer.deserialize(xx);
    ///     deserializer.deserialize(yy);
    ///     assert(x == xx &amp;&amp; y == yy);
    /// 
    /// The Serializer subclasses also support serialization of named variables, in which case the
    /// deserialization must specify the correct variable name.
    /// 
    ///     x = 0.4;
    ///     serializer.Serialize("x", x);
    ///     deserializer.deserialize("x", xx);
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
    ///         void Read(utilities::Deserializer&amp; deserializer, utilities::SerializationContext& context);
    ///         void Write(utilities::Serializer&amp; serializer) const;
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
    /// </summary>
    class Serializer
    {
    public:
        /// <summary> Serialize unnamed values of any serializable type. </summary>
        template <typename ValueType>
        void Serialize(ValueType&& value);

        // TODO: call these something else, and just call them from the generic Serialize(name, ValueType&& value)
        /// <summary> Serialize named values of any serializable type. </summary>
        template <typename ValueType, IsNotVector<ValueType> concept = 0>
        void Serialize(const char* name, ValueType&& value);

        template <typename ValueType>
        void Serialize(const char* name, ValueType* value);

        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void Serialize(const char* name, const std::vector<ValueType>& value);

        template <typename ValueType, IsSerializable<ValueType> concept = 0>
        void Serialize(const char* name, const std::vector<ValueType>& value);

        template <typename ValueType, IsSerializable<ValueType> concept = 0>
        void Serialize(const char* name, const std::vector<const ValueType*>& value);

    protected:
        // These are all the virtual function that need to be implemented by serializers
        DECLARE_SERIALIZE_VALUE_BASE(bool);
        DECLARE_SERIALIZE_VALUE_BASE(char);
        DECLARE_SERIALIZE_VALUE_BASE(short);
        DECLARE_SERIALIZE_VALUE_BASE(int);
        DECLARE_SERIALIZE_VALUE_BASE(size_t);
        DECLARE_SERIALIZE_VALUE_BASE(float);
        DECLARE_SERIALIZE_VALUE_BASE(double);
        virtual void SerializeValue(const char* name, const std::string& value) = 0;
        virtual void SerializeValue(const char* name, const ISerializable& value);

        DECLARE_SERIALIZE_ARRAY_VALUE_BASE(bool);
        DECLARE_SERIALIZE_ARRAY_VALUE_BASE(char);
        DECLARE_SERIALIZE_ARRAY_VALUE_BASE(short);
        DECLARE_SERIALIZE_ARRAY_VALUE_BASE(int);
        DECLARE_SERIALIZE_ARRAY_VALUE_BASE(size_t);
        DECLARE_SERIALIZE_ARRAY_VALUE_BASE(float);
        DECLARE_SERIALIZE_ARRAY_VALUE_BASE(double);
        virtual void SerializeArrayValue(const char* name, const std::vector<const ISerializable*>& array) = 0;

        virtual void BeginSerializeObject(const char* name, const ISerializable& value);
        virtual void SerializeObject(const char* name, const ISerializable& value) = 0;
        virtual void EndSerializeObject(const char* name, const ISerializable& value);

        virtual void EndSerialization() {}
    };

#define DECLARE_DESERIALIZE_VALUE_BASE(type) virtual void DeserializeValue(const char* name, type& value, SerializationContext& context, IsFundamental<type> dummy = 0) = 0;
#define DECLARE_DESERIALIZE_ARRAY_VALUE_BASE(type) virtual void DeserializeArrayValue(const char* name, std::vector<type>& value, SerializationContext& context, IsFundamental<type> dummy = 0) = 0;
#define DECLARE_DESERIALIZE_VALUE_OVERRIDE(type) virtual void DeserializeValue(const char* name, type& value, SerializationContext& context, IsFundamental<type> dummy = 0) override;
#define DECLARE_DESERIALIZE_ARRAY_VALUE_OVERRIDE(type) virtual void DeserializeArrayValue(const char* name, std::vector<type>& value, SerializationContext& context, IsFundamental<type> dummy = 0) override;

    class Deserializer
    {
    public:
        /// <summary> Serialize unnamed values of any serializable type. </summary>
        template <typename ValueType>
        void Deserialize(ValueType&& value, SerializationContext& context);

        /// <summary> Serialize named values of various serializable types. </summary>
        template <typename ValueType, IsNotVector<ValueType> concept = 0>
        void Deserialize(const char* name, ValueType&& value, SerializationContext& context);

        template <typename ValueType, IsNotSerializable<ValueType> concept = 0>
        void Deserialize(const char* name, std::unique_ptr<ValueType>& value, SerializationContext& context);

        template <typename ValueType, IsSerializable<ValueType> concept = 0>
        void Deserialize(const char* name, std::unique_ptr<ValueType>& value, SerializationContext& context);

        // vector of fundamental values
        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void Deserialize(const char* name, std::vector<ValueType>& value, SerializationContext& context);

        // vector of ISerializable values
        template <typename ValueType, IsSerializable<ValueType> concept = 0>
        void Deserialize(const char* name, std::vector<ValueType>& value, SerializationContext& context);

        // Vector of pointers to serializable things
        template <typename ValueType, IsSerializable<ValueType> concept = 0>
        void Deserialize(const char* name, std::vector<const ValueType*>& value, SerializationContext& context);

    protected:
        DECLARE_DESERIALIZE_VALUE_BASE(bool);
        DECLARE_DESERIALIZE_VALUE_BASE(char);
        DECLARE_DESERIALIZE_VALUE_BASE(short);
        DECLARE_DESERIALIZE_VALUE_BASE(int);
        DECLARE_DESERIALIZE_VALUE_BASE(size_t);
        DECLARE_DESERIALIZE_VALUE_BASE(float);
        DECLARE_DESERIALIZE_VALUE_BASE(double);
        virtual void DeserializeValue(const char* name, std::string& value, SerializationContext& context) = 0;
        virtual void DeserializeValue(const char* name, ISerializable& value, SerializationContext& context);

        DECLARE_DESERIALIZE_ARRAY_VALUE_BASE(bool);
        DECLARE_DESERIALIZE_ARRAY_VALUE_BASE(char);
        DECLARE_DESERIALIZE_ARRAY_VALUE_BASE(short);
        DECLARE_DESERIALIZE_ARRAY_VALUE_BASE(int);
        DECLARE_DESERIALIZE_ARRAY_VALUE_BASE(size_t);
        DECLARE_DESERIALIZE_ARRAY_VALUE_BASE(float);
        DECLARE_DESERIALIZE_ARRAY_VALUE_BASE(double);
        virtual void DeserializeArrayValue(const char* name, std::vector<ISerializable*>& array, SerializationContext& context) = 0;

        virtual std::string BeginDeserializeObject(const char* name, ISerializable& value, SerializationContext& context); // returns typename
        virtual void DeserializeObject(const char* name, ISerializable& value, SerializationContext& context) = 0;
        virtual void EndDeserializeObject(const char* name, ISerializable& value, SerializationContext& context);
    };
}

#define IMPLEMENT_SERIALIZE_VALUE(base, type)          void base::SerializeValue(const char* name, type value, IsFundamental<type> dummy) { WriteScalar(name, value); }
#define IMPLEMENT_SERIALIZE_ARRAY_VALUE(base, type)    void base::SerializeArrayValue(const char* name, const std::vector<type>& value, IsFundamental<type> dummy) { WriteArray(name, value); }

#define IMPLEMENT_DESERIALIZE_VALUE(base, type)        void base::DeserializeValue(const char* name, type& value, SerializationContext& context, IsFundamental<type> dummy) { ReadScalar(name, value); }
#define IMPLEMENT_DESERIALIZE_ARRAY_VALUE(base, type)  void base::DeserializeArrayValue(const char* name, std::vector<type>& value, SerializationContext& context, IsFundamental<type> dummy) { ReadArray(name, value, context); }

#include "../tcc/Serialization.tcc"
