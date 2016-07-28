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

    template <typename ValueType, IsSerializable<ValueType> concept>
    void Serializer::Serialize(const char* name, const std::vector<ValueType>& array)
    {
        std::cout << "Serialize of vector<serializable>" << std::endl;
        std::vector<const utilities::ISerializable*> tmpArray;
        for(const auto& item: array)
        {
            tmpArray.push_back(&item);
        }
        SerializeArrayValue(name, tmpArray);
    }
//     template <typename ValueType, IsVector<ValueType> concept=0>
//     void Serializer::Serialize(const char* name, const std::vector<ValueType&&>& value)
//     {
//         // :(
//         std::cout << "std::vector overload" << std::endl;
// //        SerializeValue(name, value);        
//     }

    //
    // SimpleSerializer
    //
    template <typename ValueType, IsFundamental<ValueType> concept>
    void SimpleSerializer::SerializeScalar(const char* name, const ValueType& value)
    {
        using std::to_string;
        if (name == std::string(""))
        {
            std::cout << to_string(value) << std::endl;
        }
        else
        {
            std::cout << name << ": " << to_string(value) << std::endl;
        }
    }

    template <typename ValueType, IsFundamental<ValueType> concept>
    void SimpleSerializer::SerializeArray(const char* name, const std::vector<ValueType>& array)
    {
        std::cout << "[";
        for (const auto& item : array)
        {
            Serialize(item);
            std::cout << " ";
        }
        std::cout << "]" << std::endl;
    }

    template <typename ValueType, IsSerializable<ValueType> concept>
    void SimpleSerializer::SerializeArray(const char* name, const std::vector<ValueType>& array)
    {
        std::cout << "[";
        for (const auto& item : array)
        {
//            Serialize(item);
            std::cout << " ";
        }
        std::cout << "]" << std::endl;
    }


    // Need a thing for vector<T> where T: fundamental
    // Need a thing for vector<T> where T: ISerializable*
}
