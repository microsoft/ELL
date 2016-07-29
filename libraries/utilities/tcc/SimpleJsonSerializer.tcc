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
    // Serialization
    //
    template <typename ValueType, IsFundamental<ValueType> concept>
    void SimpleJsonSerializer::WriteScalar(const char* name, const ValueType& value)
    {
        using std::to_string;
        auto indent = GetCurrentIndent();
        bool hasName = name != std::string("");
        auto endOfLine = hasName ? "\n" : "";

        std::cout << indent;
        if (hasName)
        {
            std::cout << name << ": ";
        }
        std::cout << to_string(value) << endOfLine;
    }

    // This function is inline just so it appears next to the other Write* functions
    inline void SimpleJsonSerializer::WriteScalar(const char* name, const std::string& value)
    {
        auto indent = GetCurrentIndent();
        bool hasName = name != std::string("");
        auto endOfLine = hasName ? "\n" : "";

        std::cout << indent;
        if (hasName)
        {
            std::cout << name << ": ";
        }
        std::cout << "\"" << value << "\"" << endOfLine;
    }

    template <typename ValueType, IsFundamental<ValueType> concept>
    void SimpleJsonSerializer::WriteArray(const char* name, const std::vector<ValueType>& array)
    {
        bool hasName = name != std::string("");
        auto indent = GetCurrentIndent();
        auto endOfLine = "\n";

        std::cout << indent;
        if (hasName)
        {
            std::cout << name << ": ";
        }

        std::cout << "[";
        // reset indent
        for (const auto& item : array)
        {
            Serialize(item);
            std::cout << ", ";
        }
        // reset indent
        std::cout << "]" << endOfLine;
    }

    //
    // Deserialization
    //

    // TODO: make simple simple tokenizer that splits based on simple patterns
    // split on: whitepace, comma, colon, square brackets, curly brackets, quotes

    template <typename ValueType, IsFundamental<ValueType> concept>
    void SimpleJsonSerializer::ReadScalar(const char* name, ValueType& value)
    {
        bool hasName = name != std::string("");
        if(hasName)
        {
            // match name
            // match ':'
        }

        // read string
    }

    // This function is inline just so it appears next to the other Read* functions
    inline void SimpleJsonSerializer::ReadScalar(const char* name, std::string& value) 
    {
        bool hasName = name != std::string("");
        if(hasName)
        {
            // match name
            // match ':'
        }

        // match "
        // read string
        // match "
    }

    template <typename ValueType, IsFundamental<ValueType> concept>
    void SimpleJsonSerializer::ReadArray(const char* name, std::vector<ValueType>& array)
    {
        bool hasName = name != std::string("");
        if(hasName)
        {
            // match name
            // match ':'
        }

        // match [
        // read contents
        // match ]
    }
}
