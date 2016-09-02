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
    // PropertySerializer class
    //

    template <typename ValueType>
    void Serializer::PropertySerializer::operator<<(ValueType&& value)
    {
        _serializer.Serialize(_propertyName.c_str(), value);
    }


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
    void Serializer::operator<<(ValueType&& value)
    {
        Serialize(std::forward<ValueType>(value));
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

    // Vector of strings
    inline void Serializer::SerializeItem(const char* name, const std::vector<std::string>& array)
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
    // PropertyDeserializer class
    //
    template <typename ValueType>
    void Deserializer::PropertyDeserializer::operator>>(ValueType&& value)
    {
        _deserializer.Deserialize(_propertyName.c_str(), value);
    }

    //
    // Deserializer class
    //
    template <typename ValueType>
    void Deserializer::Deserialize(ValueType&& value)
    {
        Deserialize("", value);
    }

    template <typename ValueType>
    void Deserializer::operator>>(ValueType&& value)
    {
        Deserialize(std::forward<ValueType>(value));
    }

    template <typename ValueType>
    void Deserializer::Deserialize(const char* name, ValueType&& value)
    {
        DeserializeItem(name, value);
    }

    template <typename ValueType, IsNotVector<ValueType> concept>
    void Deserializer::DeserializeItem(const char* name, ValueType&& value)
    {
        DeserializeValue(name, value);
    }

    // pointer to non-serializable type
    template <typename ValueType, IsNotSerializable<ValueType> concept>
    void Deserializer::DeserializeItem(const char* name, std::unique_ptr<ValueType>& value)
    {
        auto ptr = std::make_unique<ValueType>();
        DeserializeValue(name, *ptr);
        value = std::move(ptr);
    }

    // pointer to serializable type
    template <typename ValueType, IsSerializable<ValueType> concept>
    void Deserializer::DeserializeItem(const char* name, std::unique_ptr<ValueType>& value)
    {
        auto baseTypeName = ValueType::GetTypeName();
        auto encodedTypeName = BeginDeserializeObject(name, baseTypeName);

        std::unique_ptr<ValueType> newPtr = GetContext().GetTypeFactory().Construct<ValueType>(encodedTypeName);
        DeserializeObject(name, *newPtr);
        EndDeserializeObject(name, encodedTypeName);
        value = std::move(newPtr);
    }

    // Vector of fundamental types
    template <typename ValueType, IsFundamental<ValueType> concept>
    void Deserializer::DeserializeItem(const char* name, std::vector<ValueType>& arr)
    {
        arr.clear();
        DeserializeArray(name, arr);
    }

    // Vector of strings
    inline void Deserializer::DeserializeItem(const char* name, std::vector<std::string>& arr)
    {
        arr.clear();
        DeserializeArray(name, arr);
    }

    // Vector of serializable objects
    template <typename ValueType, IsSerializable<ValueType> concept>
    void Deserializer::DeserializeItem(const char* name, std::vector<ValueType>& arr)
    {
        arr.clear();
        auto typeName = ValueType::GetTypeName();
        BeginDeserializeArray(name, typeName);
        while(true)
        {
            auto good = BeginDeserializeArrayItem(typeName);
            if(!good)
            {
                break;
            }
            ValueType value;
            Deserialize(value);
            arr.push_back(value);
            EndDeserializeArrayItem(typeName);            
        }
        EndDeserializeArray(name, typeName);
    }

    // Vector of unique pointers to serializable objects
    template <typename ValueType, IsSerializable<ValueType> concept>
    void Deserializer::DeserializeItem(const char* name, std::vector<std::unique_ptr<ValueType>>& arr)
    {
        arr.clear();
        auto typeName = ValueType::GetTypeName();
        BeginDeserializeArray(name, typeName);
        while(true)
        {
            auto good = BeginDeserializeArrayItem(typeName);            
            if(!good)
            {
                break;
            }
            std::unique_ptr<ValueType> newPtr;
            Deserialize(newPtr);
            arr.push_back(std::move(newPtr));
            EndDeserializeArrayItem(typeName);            
        }
        EndDeserializeArray(name, typeName);
    }

    // Vector of raw pointers to serializable objects
    template <typename ValueType, IsSerializable<ValueType> concept>
    void Deserializer::DeserializeItem(const char* name, std::vector<const ValueType*>& arr)
    {
        arr.clear();
        auto typeName = ValueType::GetTypeName();
        BeginDeserializeArray(name, typeName);
        while(true)
        {
            auto good = BeginDeserializeArrayItem(typeName);            
            if(!good)
            {
                break;
            }
            std::unique_ptr<ValueType> newPtr;
            Deserialize(newPtr);
            arr.push_back(newPtr.release());
            EndDeserializeArrayItem(typeName);            
        }
        EndDeserializeArray(name, typeName);
    }
}
