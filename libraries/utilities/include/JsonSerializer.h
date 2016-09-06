////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     JsonSerializer.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Serializer.h"
#include "TypeFactory.h"
#include "TypeName.h"
#include "Exception.h"
#include "Tokenizer.h"

// stl
#include <cstdint>
#include <string>
#include <sstream>
#include <ostream>
#include <vector>
#include <type_traits>
#include <memory>

namespace utilities
{
    /// <summary> A serializer that encodes data as JSON-formatted text </summary>
    class JsonSerializer : public Serializer
    {
    public:
        /// <summary> Default Constructor --- writes to standard output. </summary>
        JsonSerializer();

        /// <summary> Constructor </summary>
        ///
        /// <param name="outputStream"> The stream to serialize data to. </summary>
        JsonSerializer(std::ostream& outputStream);

    protected:
        DECLARE_SERIALIZE_VALUE_OVERRIDE(bool);
        DECLARE_SERIALIZE_VALUE_OVERRIDE(char);
        DECLARE_SERIALIZE_VALUE_OVERRIDE(short);
        DECLARE_SERIALIZE_VALUE_OVERRIDE(int);
        DECLARE_SERIALIZE_VALUE_OVERRIDE(size_t);
        DECLARE_SERIALIZE_VALUE_OVERRIDE(float);
        DECLARE_SERIALIZE_VALUE_OVERRIDE(double);
        virtual void SerializeValue(const char* name, const std::string& value) override;

        DECLARE_SERIALIZE_ARRAY_OVERRIDE(bool);
        DECLARE_SERIALIZE_ARRAY_OVERRIDE(char);
        DECLARE_SERIALIZE_ARRAY_OVERRIDE(short);
        DECLARE_SERIALIZE_ARRAY_OVERRIDE(int);
        DECLARE_SERIALIZE_ARRAY_OVERRIDE(size_t);
        DECLARE_SERIALIZE_ARRAY_OVERRIDE(float);
        DECLARE_SERIALIZE_ARRAY_OVERRIDE(double);
        virtual void SerializeArray(const char* name, const std::string& baseTypeName, const std::vector<const ISerializable*>& array) override;

        virtual void BeginSerializeObject(const char* name, const ISerializable& value) override;
        virtual void SerializeObject(const char* name, const ISerializable& value) override;
        virtual void EndSerializeObject(const char* name, const ISerializable& value) override;

        virtual void EndSerialization() override;

    private:
        // Serialization
        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void WriteScalar(const char* name, const ValueType& value);

        void WriteScalar(const char* name, const char* value);
        void WriteScalar(const char* name, const std::string& value);

        template <typename ValueType>
        void WriteArray(const char* name, const std::vector<ValueType>& array);

        template <typename ValueType, IsSerializable<ValueType> concept = 0>
        void WriteArray(const char* name, const std::vector<ValueType>& array);

        std::ostream& _out;
        int _indent = 0;
        std::string _endOfPreviousLine;
        std::string GetCurrentIndent() { return std::string(2 * _indent, ' '); }
        void Indent();
        void FinishPreviousLine();
        void SetEndOfLine(std::string endOfLine);
    };

    /// <summary> A deserializer that reads data encoded in JSON-formatted text. </summary>
    class JsonDeserializer : public Deserializer
    {
    public:
        /// <summary> Default Constructor --- reads from standard input. </summary>
        JsonDeserializer();

        /// <summary> Constructor </summary>
        ///
        /// <param name="inputStream"> The stream to deserialize data from. </summary>
        JsonDeserializer(std::istream& inputStream);

    protected:
        DECLARE_DESERIALIZE_VALUE_OVERRIDE(bool);
        DECLARE_DESERIALIZE_VALUE_OVERRIDE(char);
        DECLARE_DESERIALIZE_VALUE_OVERRIDE(short);
        DECLARE_DESERIALIZE_VALUE_OVERRIDE(int);
        DECLARE_DESERIALIZE_VALUE_OVERRIDE(size_t);
        DECLARE_DESERIALIZE_VALUE_OVERRIDE(float);
        DECLARE_DESERIALIZE_VALUE_OVERRIDE(double);
        virtual void DeserializeValue(const char* name, std::string& value, SerializationContext& context) override;

        DECLARE_DESERIALIZE_ARRAY_OVERRIDE(bool);
        DECLARE_DESERIALIZE_ARRAY_OVERRIDE(char);
        DECLARE_DESERIALIZE_ARRAY_OVERRIDE(short);
        DECLARE_DESERIALIZE_ARRAY_OVERRIDE(int);
        DECLARE_DESERIALIZE_ARRAY_OVERRIDE(size_t);
        DECLARE_DESERIALIZE_ARRAY_OVERRIDE(float);
        DECLARE_DESERIALIZE_ARRAY_OVERRIDE(double);
        virtual void DeserializeArray(const char* name, std::vector<std::string>& array, SerializationContext& context) override;
        virtual void BeginDeserializeArray(const char* name, const std::string& typeName, SerializationContext& context) override;
        virtual bool BeginDeserializeArrayItem(const std::string& typeName, SerializationContext& context) override;
        virtual void EndDeserializeArrayItem(const std::string& typeName, SerializationContext& context) override;
        virtual void EndDeserializeArray(const char* name, const std::string& typeName, SerializationContext& context) override;

        virtual std::string BeginDeserializeObject(const char* name, const std::string& typeName, SerializationContext& context) override;
        virtual void DeserializeObject(const char* name, ISerializable& value, SerializationContext& context) override;
        virtual void EndDeserializeObject(const char* name, const std::string& typeName, SerializationContext& context) override;


    private:
        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void ReadScalar(const char* name, ValueType& value);

        void ReadScalar(const char* name, std::string& value);

        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void ReadArray(const char* name, std::vector<ValueType>& array, SerializationContext& context);

        void ReadArray(const char* name, std::vector<std::string>& array, SerializationContext& context);

        void MatchFieldName(const char* name);

        std::istream& _in;
        std::string _endOfPreviousLine;
        Tokenizer _tokenizer;
    };

    // Json utility functions
    class JsonUtilities
    {
    public:
        static std::string EncodeString(const std::string& str);
        static std::string DecodeString(const std::string& str);
        static std::string EncodeTypeName(const std::string& str);
        static std::string DecodeTypeName(const std::string& str);
    };
}

#include "../tcc/JsonSerializer.tcc"
