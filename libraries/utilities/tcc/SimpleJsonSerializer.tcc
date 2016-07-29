////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SimpleJsonSerializer.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>

namespace utilities
{
    //
    // SimpleJsonSerializer
    //
    template <typename ValueType, IsFundamental<ValueType> concept>
    void SimpleJsonSerializer::SerializeScalar(const char* name, const ValueType& value)
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

    inline void SimpleJsonSerializer::SerializeScalar(const char* name, std::string value)
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
    void SimpleJsonSerializer::SerializeArray(const char* name, const std::vector<ValueType>& array)
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
