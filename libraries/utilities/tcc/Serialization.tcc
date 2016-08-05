////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Serialization.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>

namespace utilities
{
    //
    // Serializer class
    //

    // TODO: split out scalar vs. array here (or is it fundamental vs. ISerializable?)
    template <typename ValueType>
    void Serializer::Serialize(ValueType&& value)
    {
        Serialize("", value);
    }

    template <typename ValueType, IsNotVector<ValueType> concept>
    void Serializer::Serialize(const char* name, ValueType&& value)
    {
        SerializeValue(name, value);
    }

    // Pointers
    template <typename ValueType>
    void Serializer::Serialize(const char* name, ValueType* value)
    {
        Serialize(name, *value);
    }

    // Vector of fundamental types
    template <typename ValueType, IsFundamental<ValueType> concept>
    void Serializer::Serialize(const char* name, const std::vector<ValueType>& array)
    {
        SerializeArrayValue(name, array);
    }

    // Vector of serializable objects
    template <typename ValueType, IsSerializable<ValueType> concept>
    void Serializer::Serialize(const char* name, const std::vector<ValueType>& array)
    {
        auto baseTypeName = ValueType::GetTypeName();
        std::vector<const utilities::ISerializable*> tmpArray;
        for (const auto& item : array)
        {
            tmpArray.push_back(&item);
        }
        SerializeArrayValue(name, baseTypeName, tmpArray);
    }

    // Vector of serializable pointers
    template <typename ValueType, IsSerializable<ValueType> concept>
    void Serializer::Serialize(const char* name, const std::vector<const ValueType*>& array)
    {
        auto baseTypeName = ValueType::GetTypeName();
        std::vector<const utilities::ISerializable*> tmpArray;
        for (const auto& item : array)
        {
            tmpArray.push_back(item);
        }
        SerializeArrayValue(name, baseTypeName, tmpArray);
    }

    //
    // Deserialization
    //
    template <typename ValueType>
    void Deserializer::Deserialize(ValueType&& value, SerializationContext& context)
    {
        Deserialize("", value, context);
    }

    template <typename ValueType, IsNotVector<ValueType> concept>
    void Deserializer::Deserialize(const char* name, ValueType&& value, SerializationContext& context)
    {
        DeserializeValue(name, value, context);
    }

    // pointer to non-serializable type
    template <typename ValueType, IsNotSerializable<ValueType> concept>
    void Deserializer::Deserialize(const char* name, std::unique_ptr<ValueType>& value, SerializationContext& context)
    {
        std::cout << "Deserializing non-serializable pointer" << std::endl;
        auto ptr = std::make_unique<ValueType>();
        DeserializeValue(name, *ptr, context);
        value = std::move(ptr);
    }

    // pointer to serializable type
    template <typename ValueType, IsSerializable<ValueType> concept>
    void Deserializer::Deserialize(const char* name, std::unique_ptr<ValueType>& value, SerializationContext& context)
    {
        auto baseTypeName = ValueType::GetTypeName();
        auto encodedTypeName = BeginDeserializeObject(name, baseTypeName, context);
        std::cout << "Deserializing serializable pointer to type " << baseTypeName << ", encoded type: " << encodedTypeName << std::endl;

        // TODO: create new typeName thing
        std::unique_ptr<ValueType> newPtr;

        // Need to do some SFINAE thing to ensure ValueType isn't an abstract base class 
        if(false) // baseTypeName == encodedTypeName
        {
//            newPtr = std::make_unique<ValueType>();
        }
        else
        {
            newPtr = context.GetTypeFactory().Construct<ValueType>(encodedTypeName);
        }

        DeserializeObject(name, *newPtr, context);
        EndDeserializeObject(name, *newPtr, context);
        value = std::move(newPtr);
    }

    template <typename ValueType, IsFundamental<ValueType> concept>
    void Deserializer::Deserialize(const char* name, std::vector<ValueType>& array, SerializationContext& context)
    {
        DeserializeArray(name, array, context);
    }

    // Vector of serializable objects
    template <typename ValueType, IsSerializable<ValueType> concept>
    void Deserializer::Deserialize(const char* name, std::vector<ValueType>& array, SerializationContext& context)
    {
        array.clear();
        auto typeName = ValueType::GetTypeName();
        BeginDeserializeArray(name, typeName, context);
        while(true)
        {
            ValueType value;
            auto good = DeserializeArrayItem(value, context);
            if(!good)
            {
                break;
            }
            array.push_back(value);
        }
        EndDeserializeArray(name, typeName, context);
    }

    // Vector of serializable objects
    template <typename ValueType, IsSerializable<ValueType> concept>
    void Deserializer::Deserialize(const char* name, std::vector<std::unique_ptr<ValueType>>& array, SerializationContext& context)
    {
        array.clear();
        auto typeName = ValueType::GetTypeName();
        BeginDeserializeArray(name, typeName, context);
        while(true)
        {
            auto newPtr = context.GetTypeFactory().Construct<ValueType>(typeName);
            auto good = DeserializeArrayItem(*newPtr, context);
            if(!good)
            {
                break;
            }
            array.push_back(std::move(newPtr));
        }
        EndDeserializeArray(name, typeName, context);
    }

}
