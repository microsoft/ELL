////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SimpleXmlSerializer.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "XMLSerializer.h"
#include "Serialization.h"
#include "ISerializable.h"

// stl
#include <iostream>
#include <string>
#include <sstream>
#include <cctype>

namespace utilities
{
    //
    // Serialization
    //
    SimpleXmlSerializer::SimpleXmlSerializer() : _out(std::cout) {}

    SimpleXmlSerializer::SimpleXmlSerializer(std::ostream& outputStream) : _out(outputStream) {}

    IMPLEMENT_SERIALIZE_VALUE(SimpleXmlSerializer, bool);
    IMPLEMENT_SERIALIZE_VALUE(SimpleXmlSerializer, char);
    IMPLEMENT_SERIALIZE_VALUE(SimpleXmlSerializer, short);
    IMPLEMENT_SERIALIZE_VALUE(SimpleXmlSerializer, int);
    IMPLEMENT_SERIALIZE_VALUE(SimpleXmlSerializer, size_t);
    IMPLEMENT_SERIALIZE_VALUE(SimpleXmlSerializer, float);
    IMPLEMENT_SERIALIZE_VALUE(SimpleXmlSerializer, double);

    // strings
    void SimpleXmlSerializer::SerializeValue(const char* name, const char* value) { WriteScalar(name, value); }

    void SimpleXmlSerializer::SerializeValue(const char* name, const std::string& value) { WriteScalar(name, value); }

    // ISerializable
    void SimpleXmlSerializer::BeginSerializeObject(const char* name, const ISerializable& value)
    {
        auto indent = GetCurrentIndent();
        auto typeName = value.GetRuntimeTypeName();

        _out << "<" << typeName;

        if (name != std::string(""))
        {
            _out << " name='" << name << "'";
        }
        _out << ">" << std::endl;
    }

    void SimpleXmlSerializer::SerializeObject(const char* name, const ISerializable& value)
    {
        ++_indent;
        value.Serialize(*this); // TODO: need to somehow know if we're in an indenting context or not for the subsequent calls to WriteScalar
        --_indent;
    }

    void SimpleXmlSerializer::EndSerializeObject(const char* name, const ISerializable& value)
    {
        auto indent = GetCurrentIndent();
        auto typeName = value.GetRuntimeTypeName();
        _out << indent << "</" << typeName << ">" << std::endl;
    }

    //
    // Arrays
    //
    IMPLEMENT_SERIALIZE_ARRAY_VALUE(SimpleXmlSerializer, bool);
    IMPLEMENT_SERIALIZE_ARRAY_VALUE(SimpleXmlSerializer, char);
    IMPLEMENT_SERIALIZE_ARRAY_VALUE(SimpleXmlSerializer, short);
    IMPLEMENT_SERIALIZE_ARRAY_VALUE(SimpleXmlSerializer, int);
    IMPLEMENT_SERIALIZE_ARRAY_VALUE(SimpleXmlSerializer, size_t);
    IMPLEMENT_SERIALIZE_ARRAY_VALUE(SimpleXmlSerializer, float);
    IMPLEMENT_SERIALIZE_ARRAY_VALUE(SimpleXmlSerializer, double);

    // ???
    void SimpleXmlSerializer::SerializeArrayValue(const char* name, const std::vector<const ISerializable*>& array)
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
    SimpleXmlDeserializer::SimpleXmlDeserializer() : _in(std::cin) {}
    SimpleXmlDeserializer::SimpleXmlDeserializer(std::istream& inStream) : _in(inStream) {}

    IMPLEMENT_DESERIALIZE_VALUE(SimpleXmlDeserializer, bool);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleXmlDeserializer, char);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleXmlDeserializer, short);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleXmlDeserializer, int);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleXmlDeserializer, size_t);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleXmlDeserializer, float);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleXmlDeserializer, double);

    // strings
    void SimpleXmlDeserializer::DeserializeValue(const char* name, std::string& value) { ReadScalar(name, value); }

    // ISerializable
    std::string SimpleXmlDeserializer::BeginDeserializeObject(const char* name, ISerializable& value) 
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

    void SimpleXmlDeserializer::DeserializeObject(const char* name, ISerializable& value) 
    {
        std::cout << "Deserializing an object" << std::endl;
        // somehow we need to have created the right object type
        value.Deserialize(*this);
        std::cout << "Done" << std::endl;
    }

    void SimpleXmlDeserializer::EndDeserializeObject(const char* name, ISerializable& value) 
    {
        MatchNextToken("}");
    }

    //
    // Arrays
    //
    IMPLEMENT_DESERIALIZE_ARRAY_VALUE(SimpleXmlDeserializer, bool);
    IMPLEMENT_DESERIALIZE_ARRAY_VALUE(SimpleXmlDeserializer, char);
    IMPLEMENT_DESERIALIZE_ARRAY_VALUE(SimpleXmlDeserializer, short);
    IMPLEMENT_DESERIALIZE_ARRAY_VALUE(SimpleXmlDeserializer, int);
    IMPLEMENT_DESERIALIZE_ARRAY_VALUE(SimpleXmlDeserializer, size_t);
    IMPLEMENT_DESERIALIZE_ARRAY_VALUE(SimpleXmlDeserializer, float);
    IMPLEMENT_DESERIALIZE_ARRAY_VALUE(SimpleXmlDeserializer, double);

    void SimpleXmlDeserializer::DeserializeArrayValue(const char* name, std::vector<const ISerializable*>& array) {}

    // Tokenizer
    std::string SimpleXmlDeserializer::ReadNextToken()
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

    std::string SimpleXmlDeserializer::PeekNextToken()
    {
        if (_peekedToken == "")
        {
            _peekedToken = ReadNextToken();
        }
        return _peekedToken;
    }

    void SimpleXmlDeserializer::PrintTokens()
    {
        while (true)
        {
            auto token = ReadNextToken();
            if (token == "")
                break;
            std::cout << "Token: " << token << std::endl;
        }
    }

    void SimpleXmlDeserializer::MatchNextToken(std::string value)
    {
        auto token = ReadNextToken();
        if (token != value)
        {
            throw InputException(InputExceptionErrors::badStringFormat, std::string{"Failed to match token "} + value);
        }
    }
}
