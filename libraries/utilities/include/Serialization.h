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
    template <typename ValueType>
    class is_vector
    {
        template <typename VectorType>
        static constexpr bool is_vector_checker(typename VectorType::value_type*) { return true; }

        template <typename VectorType>
        static constexpr bool is_vector_checker(...) { return false; }

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
    using IsSerializable = typename std::enable_if_t<std::is_base_of<ISerializable, ValueType>::value, int>;

    template <typename ValueType>
    using IsNotSerializable = typename std::enable_if_t<(!std::is_base_of<ISerializable, typename std::decay<ValueType>::type>::value) && (!std::is_fundamental<typename std::decay<ValueType>::type>::value), int>;

    template <typename ValueType>
    using IsVector = typename std::enable_if_t<is_vector<typename std::decay<ValueType>::type>::value, int>;

    template <typename ValueType>
    using IsNotVector = typename std::enable_if_t<!is_vector<typename std::decay<ValueType>::type>::value, int>;

#define DECLARE_FUNDAMENTAL_SERIALIZE_BASE(type) virtual void SerializeValue(const char* name, type value, IsFundamental<type> dummy = 0) = 0;
#define DECLARE_FUNDAMENTAL_SERIALIZE_OVERRIDE(type) virtual void SerializeValue(const char* name, type value, IsFundamental<type> dummy = 0) override;

#define DECLARE_FUNDAMENTAL_ARRAY_SERIALIZE_BASE(type) virtual void SerializeArrayValue(const char* name, const std::vector<type>& value, IsFundamental<type> dummy = 0) = 0;
#define DECLARE_FUNDAMENTAL_ARRAY_SERIALIZE_OVERRIDE(type) virtual void SerializeArrayValue(const char* name, const std::vector<type>& value, IsFundamental<type> dummy = 0) override;

    class Serializer
    {
    public:
        /// <summary> Serialize unnamed values of any serializable type. </summary>
        template <typename ValueType>
        void Serialize(ValueType&& value);

        /// <summary> Serialize named values of any serializable type. </summary>

        // TODO: need to use SFINAE to constrain Serialize() here to not accept vectors!!
        template <typename ValueType, IsNotVector<ValueType> concept = 0>
        void Serialize(const char* name, ValueType&& value);

        // template <typename ValueType, IsVector<ValueType> concept=0>
        // void Serialize(const char* name, ValueType&& value);

        // this never gets called -- why?
        template <typename ValueType, IsFundamental<ValueType> concept=0>
        void Serialize(const char* name, const std::vector<ValueType>& value);

        template <typename ValueType, IsSerializable<ValueType> concept=0>
        void Serialize(const char* name, const std::vector<ValueType>& value);

        // template <typename ValueType>
        // void Serialize(const char* name, const std::vector<ValueType&&>& value);

    protected:
        // TODO: Instead of having all these different overloads, we could possibly have SerializeVariant(const Variant& value) be the
        //       virtual interface, and let the subclass deal with extracting the value from the variant

        // virtual void SerializeVariant(std::string name, const Variant& variant) = 0;

        // fundamental types
        // TODO: maybe only require bool, char, int, and have rest use those?
        // Generates "SerializeValue" function definitions
        DECLARE_FUNDAMENTAL_SERIALIZE_BASE(bool);
        DECLARE_FUNDAMENTAL_SERIALIZE_BASE(char);
        DECLARE_FUNDAMENTAL_SERIALIZE_BASE(short);
        DECLARE_FUNDAMENTAL_SERIALIZE_BASE(int);
        DECLARE_FUNDAMENTAL_SERIALIZE_BASE(size_t);
        DECLARE_FUNDAMENTAL_SERIALIZE_BASE(float);
        DECLARE_FUNDAMENTAL_SERIALIZE_BASE(double);

        DECLARE_FUNDAMENTAL_ARRAY_SERIALIZE_BASE(bool);
        DECLARE_FUNDAMENTAL_ARRAY_SERIALIZE_BASE(char);
        DECLARE_FUNDAMENTAL_ARRAY_SERIALIZE_BASE(short);
        DECLARE_FUNDAMENTAL_ARRAY_SERIALIZE_BASE(int);
        DECLARE_FUNDAMENTAL_ARRAY_SERIALIZE_BASE(size_t);
        DECLARE_FUNDAMENTAL_ARRAY_SERIALIZE_BASE(float);
        DECLARE_FUNDAMENTAL_ARRAY_SERIALIZE_BASE(double);

        virtual void SerializeValue(const char* name, const ISerializable& value) = 0;

        virtual void SerializeArrayValue(const char* name, const std::vector<const ISerializable*>& array) = 0;

        //
        // Deserialization
        //
        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void Deserialize(const char* name, ValueType& value);

        // template<typename BaseType>
        // void Deserialize(const char* name, std::unique_ptr<BaseType>& value);

        // template<typename BaseType>
        // void Deserialize(const char* name, std::unique_ptr<BaseType>& value, const TypeFactory<BaseType>& factory);

        virtual void Deserialize(const char* name, ISerializable& value) = 0;

        // template<typename BaseType>
        // void Deserialize(const char* name, std::vector<std::unique_ptr<BaseType>>& value);

        // template<typename BaseType>
        // void Deserialize(const char* name, std::vector<std::unique_ptr<BaseType>>& value, const TypeFactory<BaseType>& factory);

        // template<typename ElementType>
        // void Deserialize(const char* name, std::vector<ElementType>& value);

        // void Deserialize(const char* name, std::string& value);

        // template <typename ValueType, IsClass<ValueType> concept = 0>
        // void Deserialize(const char* name, ValueType& value);

        template <typename ValueType>
        void Deserialize(ValueType&& value)
        {
            Deserialize("", value);
        }
    };

    //
    // SimpleSerializer
    //

    class SimpleSerializer : public Serializer
    {
    public:
    protected:
        // virtual void SerializeVariant(std::string name, const Variant& variant) override;

        // TODO: make macro for defining these
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

        virtual void SerializeValue(const char* name, const ISerializable& value) override;

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

        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void SerializeArray(const char* name, const std::vector<ValueType>& array);

        template <typename ValueType, IsSerializable<ValueType> concept = 0>
        void SerializeArray(const char* name, const std::vector<ValueType>& array);
    };
}

#include "../tcc/Serialization.tcc"
