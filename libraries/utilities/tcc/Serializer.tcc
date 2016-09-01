////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Serializer.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

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

    template <typename ValueType>
    void Serializer::Serialize(const char* name, ValueType&& value)
    {
        SerializeItem(name, value);
    }

    //
    // Implementations
    //
    template <typename ValueType, IsNotVector<ValueType> concept>
    void Serializer::SerializeItem(const char* name, ValueType&& value)
    {
        SerializeValue(name, value);
    }

    // Pointers
    template <typename ValueType>
    void Serializer::SerializeItem(const char* name, ValueType* value)
    {
        Serialize(name, *value);
    }

    // Vector of fundamental types
    template <typename ValueType, IsFundamental<ValueType> concept>
    void Serializer::SerializeItem(const char* name, const std::vector<ValueType>& array)
    {
        SerializeArray(name, array);
    }

    // Vector of serializable objects
    template <typename ValueType, IsSerializable<ValueType> concept>
    void Serializer::SerializeItem(const char* name, const std::vector<ValueType>& array)
    {
        auto baseTypeName = ValueType::GetTypeName();
        std::vector<const utilities::ISerializable*> tmpArray;
        for (const auto& item : array)
        {
            tmpArray.push_back(&item);
        }
        SerializeArray(name, baseTypeName, tmpArray);
    }

    // Vector of serializable pointers
    template <typename ValueType, IsSerializable<ValueType> concept>
    void Serializer::SerializeItem(const char* name, const std::vector<const ValueType*>& array)
    {
        auto baseTypeName = ValueType::GetTypeName();
        std::vector<const utilities::ISerializable*> tmpArray;
        for (const auto& item : array)
        {
            tmpArray.push_back(item);
        }
        SerializeArray(name, baseTypeName, tmpArray);
    }

    //
    // Deserialization
    //
    template <typename ValueType>
    void Deserializer::Deserialize(ValueType&& value)
    {
        Deserialize("", value);
    }

    template <typename ValueType>
    void Deserializer::Deserialize(const char* name, ValueType&& value)
    {
        DeserializeItem(name, value, GetContext());
    }

    template <typename ValueType, IsNotVector<ValueType> concept>
    void Deserializer::DeserializeItem(const char* name, ValueType&& value, SerializationContext& context)
    {
        DeserializeValue(name, value, context);
    }

    // pointer to non-serializable type
    template <typename ValueType, IsNotSerializable<ValueType> concept>
    void Deserializer::DeserializeItem(const char* name, std::unique_ptr<ValueType>& value, SerializationContext& context)
    {
        auto ptr = std::make_unique<ValueType>();
        DeserializeValue(name, *ptr, context);
        value = std::move(ptr);
    }

    // pointer to serializable type
    template <typename ValueType, IsSerializable<ValueType> concept>
    void Deserializer::DeserializeItem(const char* name, std::unique_ptr<ValueType>& value, SerializationContext& context)
    {
        auto baseTypeName = ValueType::GetTypeName();
        auto encodedTypeName = BeginDeserializeObject(name, baseTypeName, context);

        std::unique_ptr<ValueType> newPtr = context.GetTypeFactory().Construct<ValueType>(encodedTypeName);
        DeserializeObject(name, *newPtr, context);
        EndDeserializeObject(name, encodedTypeName, context);
        value = std::move(newPtr);
    }

    template <typename ValueType, IsFundamental<ValueType> concept>
    void Deserializer::DeserializeItem(const char* name, std::vector<ValueType>& arr, SerializationContext& context)
    {
        arr.clear();
        DeserializeArray(name, arr, context);
    }

    // Vector of serializable objects
    template <typename ValueType, IsSerializable<ValueType> concept>
    void Deserializer::DeserializeItem(const char* name, std::vector<ValueType>& arr, SerializationContext& context)
    {
        arr.clear();
        auto typeName = ValueType::GetTypeName();
        BeginDeserializeArray(name, typeName, context);
        while(true)
        {
            auto good = BeginDeserializeArrayItem(typeName, context);
            if(!good)
            {
                break;
            }
            ValueType value;
            Deserialize(value);
            arr.push_back(value);
            EndDeserializeArrayItem(typeName, context);            
        }
        EndDeserializeArray(name, typeName, context);
    }

    // Vector of serializable objects
    template <typename ValueType, IsSerializable<ValueType> concept>
    void Deserializer::DeserializeItem(const char* name, std::vector<std::unique_ptr<ValueType>>& arr, SerializationContext& context)
    {
        arr.clear();
        auto typeName = ValueType::GetTypeName();
        BeginDeserializeArray(name, typeName, context);
        while(true)
        {
            auto good = BeginDeserializeArrayItem(typeName, context);            
            if(!good)
            {
                break;
            }
            std::unique_ptr<ValueType> newPtr;
            Deserialize(newPtr);
            arr.push_back(std::move(newPtr));
            EndDeserializeArrayItem(typeName, context);            
        }
        EndDeserializeArray(name, typeName, context);
    }

    // Vector of serializable objects
    template <typename ValueType, IsSerializable<ValueType> concept>
    void Deserializer::DeserializeItem(const char* name, std::vector<const ValueType*>& arr, SerializationContext& context)
    {
        arr.clear();
        auto typeName = ValueType::GetTypeName();
        BeginDeserializeArray(name, typeName, context);
        while(true)
        {
            auto good = BeginDeserializeArrayItem(typeName, context);            
            if(!good)
            {
                break;
            }
            std::unique_ptr<ValueType> newPtr;
            Deserialize(newPtr);
            arr.push_back(newPtr.release());
            EndDeserializeArrayItem(typeName, context);            
        }
        EndDeserializeArray(name, typeName, context);
    }
}
