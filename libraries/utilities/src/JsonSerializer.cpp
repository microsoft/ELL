////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     JsonSerializer.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "JsonSerializer.h"
#include "Serializer.h"
#include "ISerializable.h"

#include <iostream>
#include <string>
#include <sstream>
#include <cctype>
#include <cassert>

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
        bool hasName = name != std::string("");
        FinishPreviousLine();
        auto indent = GetCurrentIndent();
        if (hasName)
        {
            _out << indent << "\"" << name << "\": ";
        }
        _out << "{\n";
        _out << indent << "  \"_type\": \"" << value.GetRuntimeTypeName() << "\"";
        SetEndOfLine(",\n");
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
        bool hasName = name != std::string("");
        _out << "\n"; // Output newline instead of calling "FinishPreviousLine"
        auto indent = GetCurrentIndent();
        _out << indent << "}";
        // need to output a comma if we're serializing a field (that is, if name != "")
        SetEndOfLine(hasName ? ",\n" : "\n");
    }

    void JsonSerializer::EndSerialization()
    {
        FinishPreviousLine();
    }

    //
    // Arrays
    //
    IMPLEMENT_SERIALIZE_ARRAY(JsonSerializer, bool);
    IMPLEMENT_SERIALIZE_ARRAY(JsonSerializer, char);
    IMPLEMENT_SERIALIZE_ARRAY(JsonSerializer, short);
    IMPLEMENT_SERIALIZE_ARRAY(JsonSerializer, int);
    IMPLEMENT_SERIALIZE_ARRAY(JsonSerializer, size_t);
    IMPLEMENT_SERIALIZE_ARRAY(JsonSerializer, float);
    IMPLEMENT_SERIALIZE_ARRAY(JsonSerializer, double);

    void JsonSerializer::SerializeArray(const char* name, const std::string& baseTypeName, const std::vector<const ISerializable*>& array)
    {
        FinishPreviousLine();
        auto indent = GetCurrentIndent();
        bool hasName = name != std::string("");

        _out << indent;
        if (hasName)
        {
            _out << "\"" << name << "\": ";
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
    std::string JsonDeserializer::BeginDeserializeObject(const char* name, const std::string& typeName, SerializationContext& context) 
    {
        bool hasName = name != std::string("");
        if(hasName)
        {
            MatchFieldName(name);
        }
        _tokenizer.MatchToken("{");
        MatchFieldName("_type");
        _tokenizer.MatchToken("\"");
        auto encodedTypeName = _tokenizer.ReadNextToken();
        assert(encodedTypeName != "");
        _tokenizer.MatchToken("\"");

        if(_tokenizer.PeekNextToken() == ",")
        {
            _tokenizer.ReadNextToken();
        }
        return encodedTypeName;
    }

    void JsonDeserializer::DeserializeObject(const char* name, ISerializable& value, SerializationContext& context) 
    {
        value.Deserialize(*this, context);
    }

    void JsonDeserializer::EndDeserializeObject(const char* name, const std::string& typeName, SerializationContext& context) 
    {
        bool hasName = name != std::string("");
        _tokenizer.MatchToken("}");

        // eat a comma if it exists
        if(hasName)
        {
            if(_tokenizer.PeekNextToken() == ",")
            {
                _tokenizer.ReadNextToken();
            }
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

    void JsonDeserializer::BeginDeserializeArray(const char* name, const std::string& typeName, SerializationContext& context)
    {
        bool hasName = name != std::string("");
        if(hasName)
        {
            MatchFieldName(name);
        }
                
        _tokenizer.MatchToken("[");
    }

    bool JsonDeserializer::BeginDeserializeArrayItem(const std::string& typeName, SerializationContext& context)
    {
        auto maybeEndArray = _tokenizer.PeekNextToken();
        if(maybeEndArray == "]")
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    void JsonDeserializer::EndDeserializeArrayItem(const std::string& typeName, SerializationContext& context)
    {
        if(_tokenizer.PeekNextToken() == ",")
        {
            _tokenizer.ReadNextToken();
        }
    }
    
    void JsonDeserializer::EndDeserializeArray(const char* name, const std::string& typeName, SerializationContext& context)
    {
        _tokenizer.MatchToken("]");
    }

    void JsonDeserializer::MatchFieldName(const char* key)
    {
        _tokenizer.MatchToken("\"");
        auto s = _tokenizer.ReadNextToken();
        if(s != key)
        {
            throw InputException(InputExceptionErrors::badStringFormat, std::string{"Failed to match name "} + key + ", got: " + s);
        }
        _tokenizer.MatchTokens({"\"", ":"});
    }

    //
    // JsonUtilities
    //

    // Characters that must be escaped in JSON strings: ', ", \, newline (\n), carriage return (\r), tab (\t), backspace (\b), form feed (\f)
    std::string JsonUtilities::EncodeString(const std::string& str)
    {
        std::vector<char> charCodes(127, '\0');
        charCodes['\''] = '\'';
        charCodes['\"'] = '\"';
        charCodes['\\'] = '\\';
        charCodes['\n'] = 'n';
        charCodes['\r'] = 'r';
        charCodes['\t'] = 't';
        charCodes['\b'] = 'b';
        charCodes['\f'] = 'f';

        // copy characters from str until we hit an escaped character, then prepend it with a backslash
        std::stringstream s;
        for(auto ch: str)
        {
            auto encoding = charCodes[ch];
            if(encoding == '\0') // no encoding
            {
                s.put(ch);
            }
            else
            {
                s.put('\\');
                s.put(encoding);
            }
        }
        return s.str();
    }

    std::string JsonUtilities::DecodeString(const std::string& str)
    {
        std::vector<char> charCodes(127, '\0');
        charCodes['\''] = '\'';
        charCodes['\"'] = '\"';
        charCodes['\\'] = '\\';
        charCodes['n'] = '\n';
        charCodes['r'] = '\r';
        charCodes['t'] = '\t';
        charCodes['b'] = '\b';
        charCodes['f'] = '\f';

        std::stringstream s;
        bool prevWasBackslash = false;
        for(auto ch: str)
        {
            if(prevWasBackslash)
            {
                auto encoding = charCodes[ch];
                if(encoding == '\0') // nothing special
                {
                    s.put('\\'); // emit previous backslash
                    s.put(ch); // emit character
                }
                else
                {
                    s.put(encoding);
                }
                prevWasBackslash = false;
            }
            else
            {
                if(ch == '\\')
                {
                    prevWasBackslash = true;
                }
                else
                {
                    prevWasBackslash = false;
                    s.put(ch);
                }
            }
        }

        if(prevWasBackslash)
        {
            s.put('\\');
        }
        return s.str();
    }

    std::string JsonUtilities::EncodeTypeName(const std::string& str)
    {
        return str;
    }

    std::string JsonUtilities::DecodeTypeName(const std::string& str)
    {
        return str;
    }
}
