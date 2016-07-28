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
//#include "Variant.h"

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
    class ISerializable;

    template <typename ValueType>
    using IsFundamental = typename std::enable_if_t<std::is_fundamental<typename std::decay<ValueType>::type>::value, int>;

    template <typename ValueType>
    using IsNotFundamental = typename std::enable_if_t<!std::is_fundamental<typename std::decay<ValueType>::type>::value, int>;

    template <typename ValueType>
    using IsClass = typename std::enable_if_t<std::is_class<ValueType>::value, int>;

    class Serializer
    {
    public:
        /// <summary> Serialize unnamed values of any serializable type. </summary>
        template <typename ValueType>
        void Serialize(ValueType&& value)
        {
            SerializeValue("", value);
        }

        template <typename ValueType>
        void Serialize(const char* name, ValueType&& value)
        {
            SerializeValue(name, value);
        }

        // template <typename ValueType>
        // void Serialize2(const char* name, ValueType&& value)
        // {
        //     Variant var = MakeVariant<ValueType>(value);
        //     SerializeVariant(name, var);
        // }

    protected:

        // #### Instead of having all these different overloads, we could possibly have SerializeVariant(const Variant& value) be the
        //      virtual interface, and let the subclass deal with extracting the value from the variant

        //virtual void SerializeVariant(std::string name, const Variant& variant) = 0;

        // fundamental types
        virtual void SerializeValue(const char* name, bool value) = 0;

        virtual void SerializeValue(const char* name, char value) = 0;
        virtual void SerializeValue(const char* name, short value) = 0;
        virtual void SerializeValue(const char* name, int value) = 0;
        virtual void SerializeValue(const char* name, size_t value) = 0;

        virtual void SerializeValue(const char* name, float value) = 0;
        virtual void SerializeValue(const char* name, double value) = 0;

        virtual void SerializeValue(const char* name, const ISerializable& value) = 0;
    
        // Repeat above for arrays
        virtual void SerializeValue(const char* name, const std::vector<int>& array) = 0;
//        virtual void SerializeValue(const char* name, const std::vector<const ISerializable&>& array) = 0;

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

        virtual void SerializeValue(const char* name, bool value) override;
        virtual void SerializeValue(const char* name, char value) override;
        virtual void SerializeValue(const char* name, short value) override;
        virtual void SerializeValue(const char* name, int value) override;
        virtual void SerializeValue(const char* name, size_t value) override;
        
        virtual void SerializeValue(const char* name, float value) override;
        virtual void SerializeValue(const char* name, double value) override;

        virtual void SerializeValue(const char* name, const ISerializable& value) override;

        virtual void SerializeValue(const char* name, const std::vector<int>& array) override;
        //virtual void SerializeValue(const char* name, const std::vector<const ISerializable&>& array) override;

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
        void SerializeFundamental(const char* name, const ValueType& value);


    };

}

#include "../tcc/Serialization.tcc"
