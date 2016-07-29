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

    // template <typename ValueType, IsVector<ValueType> concept>
    // void Serializer::Serialize(const char* name, ValueType&& value)
    // {
    //     std::cout << "Vector" << std::endl;
    //     SerializeValue(name, value);
    // 

    template <typename ValueType, IsFundamental<ValueType> concept>
    void Serializer::Serialize(const char* name, const std::vector<ValueType>& array)
    {
        SerializeArrayValue(name, array);
    }

    // Vector of serializable objects
    template <typename ValueType, IsSerializable<ValueType> concept>
    void Serializer::Serialize(const char* name, const std::vector<ValueType>& array)
    {
        std::vector<const utilities::ISerializable*> tmpArray;
        for(const auto& item: array)
        {
            tmpArray.push_back(&item);
        }
        SerializeArrayValue(name, tmpArray);
    }

    // Vector of serializable pointers
    template <typename ValueType, IsSerializable<ValueType> concept>
    void Serializer::Serialize(const char* name, const std::vector<const ValueType*>& array)
    {
        std::vector<const utilities::ISerializable*> tmpArray;
        for (const auto& item : array)
        {
            tmpArray.push_back(item);
        }
        SerializeArrayValue(name, tmpArray);
    }

    //
    // SimpleSerializer
    //
    template <typename ValueType, IsFundamental<ValueType> concept>
    void SimpleSerializer::SerializeScalar(const char* name, const ValueType& value)
    {
        using std::to_string;
        if (name == std::string(""))
        {
            std::cout << to_string(value);
        }
        else
        {
            std::cout << name << ": " << to_string(value) << std::endl;
        }
    }

    inline void SimpleSerializer::SerializeScalar(const char* name, std::string value)
    {
        using std::to_string;
        if (name == std::string(""))
        {
            std::cout << "\"" << value << "\"";
        }
        else
        {
            std::cout << name << ": \"" << value << "\"" << std::endl;
        }
    }

    template <typename ValueType, IsFundamental<ValueType> concept>
    void SimpleSerializer::SerializeArray(const char* name, const std::vector<ValueType>& array)
    {
        if (name != std::string(""))
        {
            std::cout << name << ": ";
        }

        std::cout << "[";
        for (const auto& item : array)
        {
            Serialize(item);
            std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }
}
