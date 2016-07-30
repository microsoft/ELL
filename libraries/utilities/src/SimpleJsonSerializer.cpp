////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SimpleJsonSerializer.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Serialization.h"
#include "SimpleJsonSerializer.h"
#include "Format.h"
#include "ISerializable.h"
#include "Variant.h"

#include <iostream>
#include <string>
#include <sstream>
#include <cctype>

namespace utilities
{
#define IMPLEMENT_SERIALIZE_VALUE(base, type)          void base::SerializeValue(const char* name, type value, IsFundamental<type> dummy) { WriteScalar(name, value); }
#define IMPLEMENT_SERIALIZE_ARRAY_VALUE(base, type)    void base::SerializeArrayValue(const char* name, const std::vector<type>& value, IsFundamental<type> dummy) { WriteArray(name, value); }

#define IMPLEMENT_DESERIALIZE_VALUE(base, type)        void base::DeserializeValue(const char* name, type& value, IsFundamental<type> dummy) { ReadScalar(name, value); }
#define IMPLEMENT_DESERIALIZE_ARRAY_VALUE(base, type)  void base::DeserializeArrayValue(const char* name, std::vector<type>& value, IsFundamental<type> dummy) { ReadArray(name, value); }

    //
    // Serialization
    //
    SimpleJsonSerializer::SimpleJsonSerializer() : _out(std::cout) {}

    SimpleJsonSerializer::SimpleJsonSerializer(std::ostream& outputStream) : _out(outputStream) {}

    IMPLEMENT_SERIALIZE_VALUE(SimpleJsonSerializer, bool);
    IMPLEMENT_SERIALIZE_VALUE(SimpleJsonSerializer, char);
    IMPLEMENT_SERIALIZE_VALUE(SimpleJsonSerializer, short);
    IMPLEMENT_SERIALIZE_VALUE(SimpleJsonSerializer, int);
    IMPLEMENT_SERIALIZE_VALUE(SimpleJsonSerializer, size_t);
    IMPLEMENT_SERIALIZE_VALUE(SimpleJsonSerializer, float);
    IMPLEMENT_SERIALIZE_VALUE(SimpleJsonSerializer, double);

    // strings
    void SimpleJsonSerializer::SerializeValue(const char* name, const char* value) { WriteScalar(name, value); }

    void SimpleJsonSerializer::SerializeValue(const char* name, const std::string& value) { WriteScalar(name, value); }

    // ISerializable
    void SimpleJsonSerializer::BeginSerializeObject(const char* name, const ISerializable& value)
    {
        auto indent = GetCurrentIndent();
        if (name != std::string(""))
        {
            _out << indent << name << ": ";
        }
        _out << "{" << std::endl;
        _out << indent << "_type: " << value.GetRuntimeTypeName() << std::endl;
    }

    void SimpleJsonSerializer::SerializeObject(const char* name, const ISerializable& value)
    {
        ++_indent;
        value.Serialize(*this); // TODO: need to somehow know if we're in an indenting context or not for the subsequent calls to WriteScalar
        --_indent;
    }

    void SimpleJsonSerializer::EndSerializeObject(const char* name, const ISerializable& value)
    {
        auto indent = GetCurrentIndent();
        _out << indent << "}" << std::endl;
    }

    //
    // Arrays
    //
    IMPLEMENT_SERIALIZE_ARRAY_VALUE(SimpleJsonSerializer, bool);
    IMPLEMENT_SERIALIZE_ARRAY_VALUE(SimpleJsonSerializer, char);
    IMPLEMENT_SERIALIZE_ARRAY_VALUE(SimpleJsonSerializer, short);
    IMPLEMENT_SERIALIZE_ARRAY_VALUE(SimpleJsonSerializer, int);
    IMPLEMENT_SERIALIZE_ARRAY_VALUE(SimpleJsonSerializer, size_t);
    IMPLEMENT_SERIALIZE_ARRAY_VALUE(SimpleJsonSerializer, float);
    IMPLEMENT_SERIALIZE_ARRAY_VALUE(SimpleJsonSerializer, double);

    void SimpleJsonSerializer::SerializeArrayValue(const char* name, const std::vector<const ISerializable*>& array)
    {
        auto indent = GetCurrentIndent();
        if (name != std::string(""))
        {
            _out << name << ": ";
        }

        _out << "[";
        for (const auto& item : array)
        {
            Serialize(*item);
            _out << ", ";
        }
        _out << "]";
    }

    //
    // Deserialization
    //
    SimpleJsonDeserializer::SimpleJsonDeserializer() : _in(std::cin) {}
    SimpleJsonDeserializer::SimpleJsonDeserializer(std::istream& inStream) : _in(inStream) {}

    IMPLEMENT_DESERIALIZE_VALUE(SimpleJsonDeserializer, bool);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleJsonDeserializer, char);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleJsonDeserializer, short);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleJsonDeserializer, int);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleJsonDeserializer, size_t);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleJsonDeserializer, float);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleJsonDeserializer, double);

    // strings
    void SimpleJsonDeserializer::DeserializeValue(const char* name, std::string& value) { ReadScalar(name, value); }

    // ISerializable
    std::string SimpleJsonDeserializer::BeginDeserializeObject(const char* name, ISerializable& value) 
    {
        bool hasName = name != std::string("");
        if(hasName)
        {
            MatchNextToken(name);
            MatchNextToken(":");
        }
        MatchNextToken("{");
        
        MatchNextToken("_type");
        MatchNextToken(":");
        auto typeName = ReadNextToken();
        std::cout << "Read type: " << typeName << std::endl;    
        return typeName;
    }

    void SimpleJsonDeserializer::DeserializeObject(const char* name, ISerializable& value) 
    {
        std::cout << "Deserializing an object" << std::endl;
        // somehow we need to have created the right object type
        value.Deserialize(*this);
        std::cout << "Done" << std::endl;
    }

    void SimpleJsonDeserializer::EndDeserializeObject(const char* name, ISerializable& value) 
    {
        MatchNextToken("}");
    }

    //
    // Arrays
    //
    IMPLEMENT_DESERIALIZE_ARRAY_VALUE(SimpleJsonDeserializer, bool);
    IMPLEMENT_DESERIALIZE_ARRAY_VALUE(SimpleJsonDeserializer, char);
    IMPLEMENT_DESERIALIZE_ARRAY_VALUE(SimpleJsonDeserializer, short);
    IMPLEMENT_DESERIALIZE_ARRAY_VALUE(SimpleJsonDeserializer, int);
    IMPLEMENT_DESERIALIZE_ARRAY_VALUE(SimpleJsonDeserializer, size_t);
    IMPLEMENT_DESERIALIZE_ARRAY_VALUE(SimpleJsonDeserializer, float);
    IMPLEMENT_DESERIALIZE_ARRAY_VALUE(SimpleJsonDeserializer, double);

    void SimpleJsonDeserializer::DeserializeArrayValue(const char* name, std::vector<const ISerializable*>& array) {}

    // Tokenizer
    std::string SimpleJsonDeserializer::ReadNextToken()
    {
        if (_peekedToken != "")
        {
            auto temp = _peekedToken;
            _peekedToken = "";
            return temp;
        }

        std::string whitespace = " \r\t\n";
        std::string tokenStopChars = " \t\r\n,:{}[]'\"";
        std::stringstream tokenStream;

        // eat whitespace and add first char
        while (_in)
        {
            auto ch = _in.get();
            if (ch == EOF)
                return "";
            if (!std::isspace(ch))
            {
                tokenStream << (char)ch;
                if (tokenStopChars.find(ch) == std::string::npos)
                    break;
                else
                    return tokenStream.str();
            }
        }

        while (_in)
        {
            auto ch = _in.get();
            if (ch == EOF)
            {
                break;
            }

            if (tokenStopChars.find(ch) != std::string::npos)
            {
                _in.unget();
                break;
            }
            tokenStream << (char)ch;
        }

        return tokenStream.str();
    }

    std::string SimpleJsonDeserializer::PeekNextToken()
    {
        if (_peekedToken == "")
        {
            _peekedToken = ReadNextToken();
        }
        return _peekedToken;
    }

    void SimpleJsonDeserializer::PrintTokens()
    {
        while (true)
        {
            auto token = ReadNextToken();
            if (token == "")
                break;
            std::cout << "Token: " << token << std::endl;
        }
    }

    void SimpleJsonDeserializer::MatchNextToken(std::string value)
    {
        auto token = ReadNextToken();
        if (token != value)
        {
            throw InputException(InputExceptionErrors::badStringFormat, std::string{"Failed to match token "} + value);
        }
    }
}
