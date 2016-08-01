////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SimpleJsonSerializer.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SimpleJsonSerializer.h"
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
        FinishPreviousLine();
        auto indent = GetCurrentIndent();
        if (name != std::string(""))
        {
            _out << indent << name << ": ";
        }
        _out << "{" << std::endl;
        _out << indent << "_type: " << value.GetRuntimeTypeName() << "\n";
    }

    void SimpleJsonSerializer::SerializeObject(const char* name, const ISerializable& value)
    {
        FinishPreviousLine();
        ++_indent;
        value.Serialize(*this); // TODO: need to somehow know if we're in an indenting context or not for the subsequent calls to WriteScalar
        --_indent;
    }

    void SimpleJsonSerializer::EndSerializeObject(const char* name, const ISerializable& value)
    {
//        FinishPreviousLine();
        _out << "\n";
        // TODO: need to skip writing a comma here
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
        FinishPreviousLine();
        auto indent = GetCurrentIndent();
        if (name != std::string(""))
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

    void SimpleJsonSerializer::Indent()
    {
        _out << GetCurrentIndent();
    }

    void SimpleJsonSerializer::FinishPreviousLine()
    {
        _out << _endOfPreviousLine;
        _endOfPreviousLine = "";
    }

    //
    // Deserialization
    //
    SimpleJsonDeserializer::SimpleJsonDeserializer() : _in(std::cin), _tokenizer(std::cin, ",:{}[]'\"") {}
    SimpleJsonDeserializer::SimpleJsonDeserializer(std::istream& inputStream) : _in(inputStream), _tokenizer(inputStream, ",:{}[]'\"") {}

    IMPLEMENT_DESERIALIZE_VALUE(SimpleJsonDeserializer, bool);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleJsonDeserializer, char);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleJsonDeserializer, short);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleJsonDeserializer, int);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleJsonDeserializer, size_t);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleJsonDeserializer, float);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleJsonDeserializer, double);

    // strings
    void SimpleJsonDeserializer::DeserializeValue(const char* name, std::string& value, SerializationContext& context) { ReadScalar(name, value); }

    // ISerializable
    std::string SimpleJsonDeserializer::BeginDeserializeObject(const char* name, ISerializable& value, SerializationContext& context) 
    {
        bool hasName = name != std::string("");
        if(hasName)
        {
            _tokenizer.MatchTokens( {name, ":"} );
        }
        _tokenizer.MatchToken("{");        
        _tokenizer.MatchTokens({"_type", ":"});
        auto typeName = _tokenizer.ReadNextToken();
        std::cout << "Read type: " << typeName << std::endl;    
        return typeName;
    }

    void SimpleJsonDeserializer::DeserializeObject(const char* name, ISerializable& value, SerializationContext& context) 
    {
        std::cout << "Deserializing an object" << std::endl;
        value.Deserialize(*this, context);
        std::cout << "Done" << std::endl;
    }

    void SimpleJsonDeserializer::EndDeserializeObject(const char* name, ISerializable& value, SerializationContext& context) 
    {
        _tokenizer.MatchToken("}");
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

    void SimpleJsonDeserializer::DeserializeArrayValue(const char* name, std::vector<const ISerializable*>& array, SerializationContext& context) {}
}
