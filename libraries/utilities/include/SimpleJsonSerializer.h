////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SimpleJsonSerializer.h (utilities)
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
    class SimpleJsonSerializer : public Serializer
    {
    public:
    protected:
        // virtual void SerializeVariant(std::string name, const Variant& variant) override;

        DECLARE_FUNDAMENTAL_SERIALIZE_OVERRIDE(bool);
        DECLARE_FUNDAMENTAL_SERIALIZE_OVERRIDE(char);
        DECLARE_FUNDAMENTAL_SERIALIZE_OVERRIDE(short);
        DECLARE_FUNDAMENTAL_SERIALIZE_OVERRIDE(int);
        DECLARE_FUNDAMENTAL_SERIALIZE_OVERRIDE(size_t);
        DECLARE_FUNDAMENTAL_SERIALIZE_OVERRIDE(float);
        DECLARE_FUNDAMENTAL_SERIALIZE_OVERRIDE(double);

        DECLARE_FUNDAMENTAL_ARRAY_SERIALIZE_OVERRIDE(bool);
        DECLARE_FUNDAMENTAL_ARRAY_SERIALIZE_OVERRIDE(char);
        DECLARE_FUNDAMENTAL_ARRAY_SERIALIZE_OVERRIDE(short);
        DECLARE_FUNDAMENTAL_ARRAY_SERIALIZE_OVERRIDE(int);
        DECLARE_FUNDAMENTAL_ARRAY_SERIALIZE_OVERRIDE(size_t);
        DECLARE_FUNDAMENTAL_ARRAY_SERIALIZE_OVERRIDE(float);
        DECLARE_FUNDAMENTAL_ARRAY_SERIALIZE_OVERRIDE(double);

        virtual void SerializeValue(const char* name, std::string value) override;

        virtual void SerializeObject(const char* name, const ISerializable& value) override;

        virtual void SerializeArrayValue(const char* name, const std::vector<const ISerializable*>& array) override;

        // template <typename ValueType, IsClass<ValueType> concept = 0>
        // void Serialize(const char* name, const ValueType& value);

        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void Deserialize(const char* name, ValueType& value);

        virtual void Deserialize(const char* name, ISerializable& value) override;

        // template<typename BaseType>
        // void Deserialize(const char* name, std::unique_ptr<BaseType>& value);

        // template<typename BaseType>
        // void Deserialize(const char* name, std::unique_ptr<BaseType>& value, const TypeFactory<BaseType>& factory);

        // template<typename BaseType>
        // void Deserialize(const char* name, std::vector<std::unique_ptr<BaseType>>& value);

        // template<typename BaseType>
        // void Deserialize(const char* name, std::vector<std::unique_ptr<BaseType>>& value, const TypeFactory<BaseType>& factory);

        // template<typename ElementType>
        // void Deserialize(const char* name, std::vector<ElementType>& value);

        // void Deserialize(const char* name, std::string& value);

        // template <typename ValueType, IsClass<ValueType> concept = 0>
        // void Deserialize(const char* name, ValueType& value);

    private:
        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void SerializeScalar(const char* name, const ValueType& value);

        void SerializeScalar(const char* name, std::string value);

        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void SerializeArray(const char* name, const std::vector<ValueType>& array);
    };
}

#include "../tcc/SimpleJsonSerializer.tcc"
