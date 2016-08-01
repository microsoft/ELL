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
#include <algorithm>

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
        auto typeName = SanitizeTypeName(value.GetRuntimeTypeName());

        _out << indent;
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
        auto typeName = SanitizeTypeName(value.GetRuntimeTypeName());
        _out << indent;
        _out << "</" << typeName << ">" << std::endl;
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

    // Array of ISerializable
    // TOOD: pass in compile-time type name
    void SimpleXmlSerializer::SerializeArrayValue(const char* name, const std::vector<const ISerializable*>& array)
    {
        bool hasName = name != std::string("");
        auto indent = GetCurrentIndent();
        auto endOfLine = "\n";
        auto size = array.size();
        auto typeName = "ISerializable"; //?

        _out << indent;
        _out << "<Array";
        if (hasName)
        {
            _out << " name='" << name << "'";
        }
        _out << " type='" << typeName <<  "'>" << std::endl;
        ++_indent;
        for (const auto& item : array)
        {
            Serialize(*item);
        }
        --_indent;
        _out << indent;
        _out << "</Array>" << endOfLine;
    }

    //
    // Deserialization
    //
    SimpleXmlDeserializer::SimpleXmlDeserializer() : _in(std::cin), _tokenizer(std::cin, "<>=/'\"") {}
    SimpleXmlDeserializer::SimpleXmlDeserializer(std::istream& inputStream) : _in(inputStream), _tokenizer(inputStream, "<>=/'\"") {}

    IMPLEMENT_DESERIALIZE_VALUE(SimpleXmlDeserializer, bool);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleXmlDeserializer, char);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleXmlDeserializer, short);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleXmlDeserializer, int);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleXmlDeserializer, size_t);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleXmlDeserializer, float);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleXmlDeserializer, double);

    // strings
    void SimpleXmlDeserializer::DeserializeValue(const char* name, std::string& value, SerializationContext& context) { ReadScalar(name, value); }

    // ISerializable
    std::string SimpleXmlDeserializer::BeginDeserializeObject(const char* name, ISerializable& value, SerializationContext& context) 
    {
        bool hasName = name != std::string("");
        auto typeName = SanitizeTypeName(value.GetRuntimeTypeName());

        _tokenizer.MatchTokens({"<", typeName});
        if(hasName)
        {
            _tokenizer.MatchTokens({"name", "=", "'", name, "'"});
        }
        _tokenizer.MatchToken(">");
        return typeName;
    }

    void SimpleXmlDeserializer::DeserializeObject(const char* name, ISerializable& value, SerializationContext& context) 
    {
        // somehow we need to have created the right object type
        value.Deserialize(*this, context);
    }

    void SimpleXmlDeserializer::EndDeserializeObject(const char* name, ISerializable& value, SerializationContext& context) 
    {
        auto typeName = SanitizeTypeName(value.GetRuntimeTypeName());
        _tokenizer.MatchTokens({"<", "/", typeName, ">"});
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

    // TODO: Why does this not do anything???
    void SimpleXmlDeserializer::DeserializeArrayValue(const char* name, std::vector<const ISerializable*>& array, SerializationContext& context) 
    {
        throw "DeserializeArrayValue called";
    }

    // TODO: allow multi-char tokens

    std::string SimpleXmlDeserializer::SanitizeString(const std::string& str)
    {
        return str;
    }

    std::string SimpleXmlDeserializer::UnsanitizeString(const std::string& str)
    {
        return str;
    }

    std::string SimpleXmlSerializer::SanitizeTypeName(const std::string& str)
    {
        // convert '<'->'(' etc.
        auto result = str;
        std::replace(result.begin(), result.end(), '<', '(');
        std::replace(result.begin(), result.end(), '>', ')');
        return result;
    }

    std::string SimpleXmlDeserializer::SanitizeTypeName(const std::string& str)
    {
        // convert '<'->'(' etc.
        auto result = str;
        std::replace(result.begin(), result.end(), '<', '(');
        std::replace(result.begin(), result.end(), '>', ')');
        return result;
    }

    std::string SimpleXmlDeserializer::UnsanitizeTypeName(const std::string& str)
    {
        auto result = str;
        std::replace(result.begin(), result.end(), '(', ')');
        std::replace(result.begin(), result.end(), ')', '>');
        return result;
    }
}
