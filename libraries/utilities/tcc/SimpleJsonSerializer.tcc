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

        _out << indent;
        if (hasName)
        {
            _out << name << ": ";
        }
        _out << to_string(value) << endOfLine;
    }

    // This function is inline just so it appears next to the other Write* functions
    inline void SimpleJsonSerializer::WriteScalar(const char* name, const char* value)
    {
        auto indent = GetCurrentIndent();
        bool hasName = name != std::string("");
        auto endOfLine = hasName ? "\n" : "";

        _out << indent;
        if (hasName)
        {
            _out << name << ": ";
        }
        _out << "\"" << value << "\"" << endOfLine;
    }

    inline void SimpleJsonSerializer::WriteScalar(const char* name, const std::string& value)
    {
        auto indent = GetCurrentIndent();
        bool hasName = name != std::string("");
        auto endOfLine = hasName ? "\n" : "";

        _out << indent;
        if (hasName)
        {
            _out << name << ": ";
        }
        _out << "\"" << value << "\"" << endOfLine;
    }

    template <typename ValueType, IsFundamental<ValueType> concept>
    void SimpleJsonSerializer::WriteArray(const char* name, const std::vector<ValueType>& array)
    {
        bool hasName = name != std::string("");
        auto indent = GetCurrentIndent();
        auto endOfLine = "\n";

        _out << indent;
        if (hasName)
        {
            _out << name << ": ";
        }

        _out << "[";
        // reset indent
        for (const auto& item : array)
        {
            Serialize(item);
            _out << ", ";
        }
        // reset indent
        _out << "]" << endOfLine;
    }

    //
    // Deserialization
    //

    // TODO: make simple simple tokenizer that splits based on simple patterns
    // split on: whitepace, comma, colon, square brackets, curly brackets, quotes

    template <typename ValueType, IsFundamental<ValueType> concept>
    void SimpleJsonDeserializer::ReadScalar(const char* name, ValueType& value)
    {
        bool hasName = name != std::string("");
        if(hasName)
        {
            MatchNextToken(name);
            MatchNextToken(":");
        }

        // read string
        auto valueToken = ReadNextToken();
        std::stringstream valueStream(valueToken);
        valueStream >> value;
    }

    // This function is inline just so it appears next to the other Read* functions
    inline void SimpleJsonDeserializer::ReadScalar(const char* name, std::string& value) 
    {
        bool hasName = name != std::string("");
        if(hasName)
        {
            MatchNextToken(name);
            MatchNextToken(":");
        }

        MatchNextToken("\"");
        auto valueToken = ReadNextToken();
        value = valueToken;
        MatchNextToken("\"");
    }

    template <typename ValueType, IsFundamental<ValueType> concept>
    void SimpleJsonDeserializer::ReadArray(const char* name, std::vector<ValueType>& array)
    {
        bool hasName = name != std::string("");
        if(hasName)
        {
            MatchNextToken(name);
            MatchNextToken(":");
        }
                
        MatchNextToken("[");
        std::string nextToken = "";
        while(nextToken != "]")
        {
            ValueType obj;
            Deserialize(obj);
            array.push_back(obj);

            MatchNextToken(",");

            // Want to peek at the next token here to see if it's a ']'
            nextToken = PeekNextToken();
        }
        MatchNextToken("]");
    }
}
