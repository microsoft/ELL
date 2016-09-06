////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ObjectArchiver.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Archiver.h"
#include "TypeFactory.h"
#include "TypeName.h"
#include "Exception.h"
#include "ObjectArchive.h"

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
    /// <summary> An archiver that encodes data in an ObjectArchive </summary>
    class ObjectArchiver : public Archiver, public Unarchiver
    {
    public:
        /// <summary> Constructor for writing </summary>
        /// <param name="context"> The `SerializationContext` to use </param>
        ObjectArchiver(SerializationContext context);
 
        /// <summary> Constructor for reading </summary>
        ///
        /// <param name="objectDescription"> The description to deserialize data from. </summary>
        /// <param name="context"> The `SerializationContext` to use </param>
        ObjectArchiver(const ObjectArchive& objectDescription, SerializationContext context);

        /// <summary> Gets the `ObjectArchive` containing the information for the archived object </summary>
        ///
        /// <returns> The `ObjectArchive` containing the information  for the archived object </returns>
        ObjectArchive GetObjectArchive() { return _objectDescription; }

    protected:
        // Serialization
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
        virtual void SerializeArray(const char* name, const std::vector<std::string>& array) override;
        virtual void SerializeArray(const char* name, const std::string& baseTypeName, const std::vector<const ISerializable*>& array) override;

        virtual void SerializeObject(const char* name, const ISerializable& value) override;

        // Deserialization
        DECLARE_DESERIALIZE_VALUE_OVERRIDE(bool);
        DECLARE_DESERIALIZE_VALUE_OVERRIDE(char);
        DECLARE_DESERIALIZE_VALUE_OVERRIDE(short);
        DECLARE_DESERIALIZE_VALUE_OVERRIDE(int);
        DECLARE_DESERIALIZE_VALUE_OVERRIDE(size_t);
        DECLARE_DESERIALIZE_VALUE_OVERRIDE(float);
        DECLARE_DESERIALIZE_VALUE_OVERRIDE(double);
        virtual void DeserializeValue(const char* name, std::string& value) override;

        DECLARE_DESERIALIZE_ARRAY_OVERRIDE(bool);
        DECLARE_DESERIALIZE_ARRAY_OVERRIDE(char);
        DECLARE_DESERIALIZE_ARRAY_OVERRIDE(short);
        DECLARE_DESERIALIZE_ARRAY_OVERRIDE(int);
        DECLARE_DESERIALIZE_ARRAY_OVERRIDE(size_t);
        DECLARE_DESERIALIZE_ARRAY_OVERRIDE(float);
        DECLARE_DESERIALIZE_ARRAY_OVERRIDE(double);
        virtual void DeserializeArray(const char* name, std::vector<std::string>& array) override;
        
        virtual void DeserializeObject(const char* name, ISerializable& value) override;
        virtual bool BeginDeserializeArrayItem(const std::string& typeName) override;
        virtual void EndDeserializeArrayItem(const std::string& typeName) override;

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

        // Deserialization
        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void ReadScalar(const char* name, ValueType& value);

        void ReadScalar(const char* name, std::string& value);

        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void ReadArray(const char* name, std::vector<ValueType>& array);

        void ReadArray(const char* name, std::vector<std::string>& array);

        // The object description
        ObjectArchive _objectDescription;
    };
}

#include "../tcc/ObjectArchiver.tcc"
