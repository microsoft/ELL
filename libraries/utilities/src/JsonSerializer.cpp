////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     JsonSerializer.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "JsonSerializer.h"
#include "Serialization.h"
#include "ISerializable.h"

#include <iostream>
#include <string>
#include <sstream>
#include <cctype>

namespace utilities
{
    //
    // Serialization
    //
    JsonSerializer::JsonSerializer() : _out(std::cout) {}

    JsonSerializer::JsonSerializer(std::ostream& outputStream) : _out(outputStream) {}

    IMPLEMENT_SERIALIZE_VALUE(JsonSerializer, bool);
    IMPLEMENT_SERIALIZE_VALUE(JsonSerializer, char);
    IMPLEMENT_SERIALIZE_VALUE(JsonSerializer, short);
    IMPLEMENT_SERIALIZE_VALUE(JsonSerializer, int);
    IMPLEMENT_SERIALIZE_VALUE(JsonSerializer, size_t);
    IMPLEMENT_SERIALIZE_VALUE(JsonSerializer, float);
    IMPLEMENT_SERIALIZE_VALUE(JsonSerializer, double);

    // strings
    void JsonSerializer::SerializeValue(const char* name, const std::string& value) { WriteScalar(name, value); }

    // ISerializable
    void JsonSerializer::BeginSerializeObject(const char* name, const ISerializable& value)
    {
        FinishPreviousLine();
        auto indent = GetCurrentIndent();
        if (name != std::string(""))
        {
            _out << indent << name << ": ";
        }
        _out << "{\n";
        _out << indent << "_type: " << value.GetRuntimeTypeName();
        SetEndOfLine(",\n");
        ++_nestedObjectCount;
    }

    void JsonSerializer::SerializeObject(const char* name, const ISerializable& value)
    {
        FinishPreviousLine();
        ++_indent;
        value.Serialize(*this); // TODO: need to somehow know if we're in an indenting context or not for the subsequent calls to WriteScalar
        --_indent;
    }

    void JsonSerializer::EndSerializeObject(const char* name, const ISerializable& value)
    {
//        FinishPreviousLine();
        _out << "\n";
        auto indent = GetCurrentIndent();
        _out << indent << "}";
        // need to output a comma if we're serializing a field. How?
        --_nestedObjectCount;
        SetEndOfLine(_nestedObjectCount > 0 ? ",\n" : "\n");
    }

    void JsonSerializer::EndSerialization()
    {
        FinishPreviousLine();
    }

    //
    // Arrays
    //
    IMPLEMENT_SERIALIZE_ARRAY_VALUE(JsonSerializer, bool);
    IMPLEMENT_SERIALIZE_ARRAY_VALUE(JsonSerializer, char);
    IMPLEMENT_SERIALIZE_ARRAY_VALUE(JsonSerializer, short);
    IMPLEMENT_SERIALIZE_ARRAY_VALUE(JsonSerializer, int);
    IMPLEMENT_SERIALIZE_ARRAY_VALUE(JsonSerializer, size_t);
    IMPLEMENT_SERIALIZE_ARRAY_VALUE(JsonSerializer, float);
    IMPLEMENT_SERIALIZE_ARRAY_VALUE(JsonSerializer, double);

    void JsonSerializer::SerializeArrayValue(const char* name, const std::vector<const ISerializable*>& array)
    {
        FinishPreviousLine();
        auto indent = GetCurrentIndent();
        bool hasName = name != std::string("");

        _out << indent;
        if (hasName)
        {
            _out << name << ": ";
        }

        _out << "[";


        auto numItems = array.size();
        for(size_t index = 0; index < numItems; ++index)
        {
            Serialize(*array[index]);
            if(index != numItems-1)
            {
                _out << ", ";
            }
        }
        _out << "]";
    }

    void JsonSerializer::Indent()
    {
        _out << GetCurrentIndent();
    }

    void JsonSerializer::FinishPreviousLine()
    {
        _out << _endOfPreviousLine;
        _endOfPreviousLine = "";
    }

    void JsonSerializer::SetEndOfLine(std::string endOfLine)
    {
        _endOfPreviousLine = endOfLine;
    }

    //
    // Deserialization
    //
    JsonDeserializer::JsonDeserializer() : _in(std::cin), _tokenizer(std::cin, ",:{}[]'\"") {}
    JsonDeserializer::JsonDeserializer(std::istream& inputStream) : _in(inputStream), _tokenizer(inputStream, ",:{}[]'\"") {}

    IMPLEMENT_DESERIALIZE_VALUE(JsonDeserializer, bool);
    IMPLEMENT_DESERIALIZE_VALUE(JsonDeserializer, char);
    IMPLEMENT_DESERIALIZE_VALUE(JsonDeserializer, short);
    IMPLEMENT_DESERIALIZE_VALUE(JsonDeserializer, int);
    IMPLEMENT_DESERIALIZE_VALUE(JsonDeserializer, size_t);
    IMPLEMENT_DESERIALIZE_VALUE(JsonDeserializer, float);
    IMPLEMENT_DESERIALIZE_VALUE(JsonDeserializer, double);

    // strings
    void JsonDeserializer::DeserializeValue(const char* name, std::string& value, SerializationContext& context) 
    { 
        ReadScalar(name, value); 
    }

    // ISerializable
    std::string JsonDeserializer::BeginDeserializeObject(const char* name, ISerializable& value, SerializationContext& context) 
    {
        bool hasName = name != std::string("");
        if(hasName)
        {
            _tokenizer.MatchTokens( {name, ":"} );
        }
        _tokenizer.MatchToken("{");        
        _tokenizer.MatchTokens({"_type", ":"});
        auto typeName = _tokenizer.ReadNextToken();

        // eat a comma if it exists
        if(hasName)
        {
            if(_tokenizer.PeekNextToken() == ",")
            {
                _tokenizer.ReadNextToken();
            }
        }
        return typeName;
    }

    void JsonDeserializer::DeserializeObject(const char* name, ISerializable& value, SerializationContext& context) 
    {
        value.Deserialize(*this, context);
    }

    void JsonDeserializer::EndDeserializeObject(const char* name, ISerializable& value, SerializationContext& context) 
    {
        _tokenizer.MatchToken("}");

        // eat a comma if it exists
        if(_tokenizer.PeekNextToken() == ",")
        {
            _tokenizer.ReadNextToken();
        }
    }

    //
    // Arrays
    //
    IMPLEMENT_DESERIALIZE_ARRAY(JsonDeserializer, bool);
    IMPLEMENT_DESERIALIZE_ARRAY(JsonDeserializer, char);
    IMPLEMENT_DESERIALIZE_ARRAY(JsonDeserializer, short);
    IMPLEMENT_DESERIALIZE_ARRAY(JsonDeserializer, int);
    IMPLEMENT_DESERIALIZE_ARRAY(JsonDeserializer, size_t);
    IMPLEMENT_DESERIALIZE_ARRAY(JsonDeserializer, float);
    IMPLEMENT_DESERIALIZE_ARRAY(JsonDeserializer, double);

    void JsonDeserializer::DeserializeArray(const char* name, std::vector<std::string>& array, SerializationContext& context)
    {
        ReadArray(name, array, context);
    }

    void JsonDeserializer::DeserializeArray(const char* name, std::vector<ISerializable*>& array, SerializationContext& context) 
    {}
}
