////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Archiver.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace utilities
{
    //
    // PropertyArchiver class
    //

    template <typename ValueType>
    void Archiver::PropertyArchiver::operator<<(ValueType&& value)
    {
        _serializer.Serialize(_propertyName.c_str(), value);
    }

    //
    // Archiver class
    //

    template <typename ValueType>
    void Archiver::Serialize(ValueType&& value)
    {
        Serialize("", value);
    }

    template <typename ValueType>
    void Archiver::operator<<(ValueType&& value)
    {
        Serialize(std::forward<ValueType>(value));
    }

    template <typename ValueType>
    void Archiver::Serialize(const char* name, ValueType&& value)
    {
        SerializeItem(name, value);
    }

    //
    // Implementations
    //

    // Non-vectors
    template <typename ValueType, IsNotVector<ValueType> concept>
    void Archiver::SerializeItem(const char* name, ValueType&& value)
    {
        SerializeValue(name, value);
    }

    // Pointers
    template <typename ValueType>
    void Archiver::SerializeItem(const char* name, ValueType* value)
    {
        Serialize(name, *value);
    }

    // Vector of fundamental types
    template <typename ValueType, IsFundamental<ValueType> concept>
    void Archiver::SerializeItem(const char* name, const std::vector<ValueType>& array)
    {
        SerializeArray(name, array);
    }

    // Vector of strings
    inline void Archiver::SerializeItem(const char* name, const std::vector<std::string>& array)
    {
        SerializeArray(name, array);
    }

    // Vector of serializable objects
    template <typename ValueType, IsSerializable<ValueType> concept>
    void Archiver::SerializeItem(const char* name, const std::vector<ValueType>& array)
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
    void Archiver::SerializeItem(const char* name, const std::vector<const ValueType*>& array)
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
    // PropertyUnarchiver class
    //
    template <typename ValueType>
    void Unarchiver::PropertyUnarchiver::operator>>(ValueType&& value)
    {
        _deserializer.Deserialize(_propertyName.c_str(), value);
    }

    //
    // Unarchiver class
    //
    template <typename ValueType>
    void Unarchiver::Deserialize(ValueType&& value)
    {
        Deserialize("", value);
    }

    template <typename ValueType>
    void Unarchiver::operator>>(ValueType&& value)
    {
        Deserialize(std::forward<ValueType>(value));
    }

    template <typename ValueType>
    void Unarchiver::Deserialize(const char* name, ValueType&& value)
    {
        DeserializeItem(name, value);
    }

    template <typename ValueType, IsNotVector<ValueType> concept>
    void Unarchiver::DeserializeItem(const char* name, ValueType&& value)
    {
        DeserializeValue(name, value);
    }

    // pointer to non-serializable type
    template <typename ValueType, IsNotSerializable<ValueType> concept>
    void Unarchiver::DeserializeItem(const char* name, std::unique_ptr<ValueType>& value)
    {
        auto ptr = std::make_unique<ValueType>();
        DeserializeValue(name, *ptr);
        value = std::move(ptr);
    }

    // pointer to serializable type
    template <typename ValueType, IsSerializable<ValueType> concept>
    void Unarchiver::DeserializeItem(const char* name, std::unique_ptr<ValueType>& value)
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
    void Unarchiver::DeserializeItem(const char* name, std::vector<ValueType>& arr)
    {
        arr.clear();
        DeserializeArray(name, arr);
    }

    // Vector of strings
    inline void Unarchiver::DeserializeItem(const char* name, std::vector<std::string>& arr)
    {
        arr.clear();
        DeserializeArray(name, arr);
    }

    // Vector of serializable objects
    template <typename ValueType, IsSerializable<ValueType> concept>
    void Unarchiver::DeserializeItem(const char* name, std::vector<ValueType>& arr)
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
    void Unarchiver::DeserializeItem(const char* name, std::vector<std::unique_ptr<ValueType>>& arr)
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
    void Unarchiver::DeserializeItem(const char* name, std::vector<const ValueType*>& arr)
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
