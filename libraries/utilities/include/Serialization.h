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
#include "TypeTraits.h"

// stl
#include <cstdint>
#include <string>
#include <vector>
#include <memory>

namespace utilities
{
    /// <summary> A context object used during deserialization. Contains a GenericTypeFactory. </summary>
    class SerializationContext
    {
    public:
        virtual ~SerializationContext() = default;
        
        /// <summary> Gets the type factory associated with this context. </summary>
        ///
        /// <returns> The type factory associated with this context. </returns>
        virtual GenericTypeFactory& GetTypeFactory() { return _typeFactory; }
    
    private:
        GenericTypeFactory _typeFactory;
    };

#define DECLARE_SERIALIZE_VALUE_BASE(type) virtual void SerializeValue(const char* name, type value, IsFundamental<type> dummy = 0) = 0;
#define DECLARE_SERIALIZE_ARRAY_BASE(type) virtual void SerializeArray(const char* name, const std::vector<type>& value, IsFundamental<type> dummy = 0) = 0;

#define DECLARE_SERIALIZE_VALUE_OVERRIDE(type) virtual void SerializeValue(const char* name, type value, IsFundamental<type> dummy = 0) override;
#define DECLARE_SERIALIZE_ARRAY_OVERRIDE(type) virtual void SerializeArray(const char* name, const std::vector<type>& value, IsFundamental<type> dummy = 0) override;

    /// <summary>
    /// The Serializer and Deserializer abstract base classes facilitate serialization and
    /// deserialization of some fundamental types, `std::string`s, `std::vector`s, and
    /// classes that derive from the ISerializable abstract base class, as well as implementing a
    /// static method called `GetTypeName`. Serializing a couple of
    /// variables to a string stream is as simple as
    /// 
    ///     double x = 5.3;
    ///     uint64_t y = 12;
    ///     std::stringstream stream;
    ///     SerializerType serializer(stream);
    ///     serializer.Serialize(x);
    ///     serializer.Serialize(y);
    /// 
    /// Deserialization must occur in the same order.
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
    /// Serialization of `std::string`s and `std::vector`s of fundamental types is similar.
    /// 
    /// To make a class serializable, the ISerializable class must be inherited from, and a default constructor
    /// needs to be implemented. Additionally, the static method `GetTypeName` needs to be implemented.
    /// 
    ///     class Bar: public ISerializable
    ///     {
    ///     public: 
    ///         Bar();
    ///         void Serialize(utilities::Deserializer&amp; deserializer) const
    ///         void Deserialize(utilities::Serializer&amp; serializer, utilities::SerializationContext& context);
    ///         virtual std::string GetRuntimeTypeName() const;
    ///
    ///         static std::string GetTypeName();
    ///     }
    /// 
    /// A typical implementation of Serialize will include a sequence of
    /// calls to serializer.Serialize(), in the same order. To serialize the class, call:
    ///
    ///     Bar z;
    ///     serializer.Serialize("z", z);
    ///
    /// A typical implementation of Deserialize() will include a similar sequence of calls to
    /// deserializer.Deserialize(). 
    /// 
    /// 
    /// This class also supports serializing and deserialization of std::unique_pointers to serializable
    /// classes (that is, classes that derive from ISerializable, have a default constructor, and implement
    /// the static `GetTypeName` function).
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

        DECLARE_SERIALIZE_ARRAY_BASE(bool);
        DECLARE_SERIALIZE_ARRAY_BASE(char);
        DECLARE_SERIALIZE_ARRAY_BASE(short);
        DECLARE_SERIALIZE_ARRAY_BASE(int);
        DECLARE_SERIALIZE_ARRAY_BASE(size_t);
        DECLARE_SERIALIZE_ARRAY_BASE(float);
        DECLARE_SERIALIZE_ARRAY_BASE(double);
        virtual void SerializeArray(const char* name, const std::string& baseTypeName, const std::vector<const ISerializable*>& array) = 0;

        virtual void BeginSerializeObject(const char* name, const ISerializable& value);
        virtual void SerializeObject(const char* name, const ISerializable& value) = 0;
        virtual void EndSerializeObject(const char* name, const ISerializable& value);

        virtual void EndSerialization() {}
    };

#define DECLARE_DESERIALIZE_VALUE_BASE(type) virtual void DeserializeValue(const char* name, type& value, SerializationContext& context, IsFundamental<type> dummy = 0) = 0;
#define DECLARE_DESERIALIZE_ARRAY_BASE(type) virtual void DeserializeArray(const char* name, std::vector<type>& value, SerializationContext& context, IsFundamental<type> dummy = 0) = 0;
#define DECLARE_DESERIALIZE_VALUE_OVERRIDE(type) virtual void DeserializeValue(const char* name, type& value, SerializationContext& context, IsFundamental<type> dummy = 0) override;
#define DECLARE_DESERIALIZE_ARRAY_OVERRIDE(type) virtual void DeserializeArray(const char* name, std::vector<type>& value, SerializationContext& context, IsFundamental<type> dummy = 0) override;

    class Deserializer
    {
    public:
        /// <summary> Serialize unnamed values of any serializable type. </summary>
        template <typename ValueType>
        void Deserialize(ValueType&& value, SerializationContext& context);

        /// <summary> Serialize named values of various serializable types. </summary>
        template <typename ValueType, IsNotVector<ValueType> concept = 0>
        void Deserialize(const char* name, ValueType&& value, SerializationContext& context);

        // pointer to non-serializable object
        template <typename ValueType, IsNotSerializable<ValueType> concept = 0>
        void Deserialize(const char* name, std::unique_ptr<ValueType>& value, SerializationContext& context);

        // pointer to serializable object
        template <typename ValueType, IsSerializable<ValueType> concept = 0>
        void Deserialize(const char* name, std::unique_ptr<ValueType>& value, SerializationContext& context);

        // vector of fundamental values
        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void Deserialize(const char* name, std::vector<ValueType>& value, SerializationContext& context);

        // vector of ISerializable values
        template <typename ValueType, IsSerializable<ValueType> concept = 0>
        void Deserialize(const char* name, std::vector<ValueType>& value, SerializationContext& context);

        // vector of pointers to ISerializable
        template <typename ValueType, IsSerializable<ValueType> concept = 0>
        void Deserialize(const char* name, std::vector<std::unique_ptr<ValueType>>& value, SerializationContext& context);

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

        DECLARE_DESERIALIZE_ARRAY_BASE(bool);
        DECLARE_DESERIALIZE_ARRAY_BASE(char);
        DECLARE_DESERIALIZE_ARRAY_BASE(short);
        DECLARE_DESERIALIZE_ARRAY_BASE(int);
        DECLARE_DESERIALIZE_ARRAY_BASE(size_t);
        DECLARE_DESERIALIZE_ARRAY_BASE(float);
        DECLARE_DESERIALIZE_ARRAY_BASE(double);

        virtual void DeserializeArray(const char* name, std::vector<std::string>& array, SerializationContext& context) = 0;
        virtual void BeginDeserializeArray(const char* name, const std::string& typeName, SerializationContext& context);
        virtual bool BeginDeserializeArrayItem(const std::string& typeName, SerializationContext& context) = 0;
        virtual void EndDeserializeArrayItem(const std::string& typeName, SerializationContext& context) = 0;
        virtual void EndDeserializeArray(const char* name, const std::string& typeName, SerializationContext& context);

        virtual std::string BeginDeserializeObject(const char* name, const std::string& typeName, SerializationContext& context); // returns typename
        virtual void DeserializeObject(const char* name, ISerializable& value, SerializationContext& context) = 0;
        virtual void EndDeserializeObject(const char* name, const std::string& typeName, SerializationContext& context);
    };
}

#define IMPLEMENT_SERIALIZE_VALUE(base, type)        void base::SerializeValue(const char* name, type value, IsFundamental<type> dummy) { WriteScalar(name, value); }
#define IMPLEMENT_SERIALIZE_ARRAY(base, type)  void base::SerializeArray(const char* name, const std::vector<type>& value, IsFundamental<type> dummy) { WriteArray(name, value); }

#define IMPLEMENT_DESERIALIZE_VALUE(base, type)      void base::DeserializeValue(const char* name, type& value, SerializationContext& context, IsFundamental<type> dummy) { ReadScalar(name, value); }
#define IMPLEMENT_DESERIALIZE_ARRAY(base, type)      void base::DeserializeArray(const char* name, std::vector<type>& value, SerializationContext& context, IsFundamental<type> dummy) { ReadArray(name, value, context); }

#include "../tcc/Serialization.tcc"
