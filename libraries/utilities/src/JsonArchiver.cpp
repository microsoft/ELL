////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     JsonArchiver.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "JsonArchiver.h"
#include "Archiver.h"
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
    JsonArchiver::JsonArchiver() : _out(std::cout) {}

    JsonArchiver::JsonArchiver(std::ostream& outputStream) : _out(outputStream) {}

    IMPLEMENT_SERIALIZE_VALUE(JsonArchiver, bool);
    IMPLEMENT_SERIALIZE_VALUE(JsonArchiver, char);
    IMPLEMENT_SERIALIZE_VALUE(JsonArchiver, short);
    IMPLEMENT_SERIALIZE_VALUE(JsonArchiver, int);
    IMPLEMENT_SERIALIZE_VALUE(JsonArchiver, size_t);
    IMPLEMENT_SERIALIZE_VALUE(JsonArchiver, float);
    IMPLEMENT_SERIALIZE_VALUE(JsonArchiver, double);

    // strings
    void JsonArchiver::SerializeValue(const char* name, const std::string& value) { WriteScalar(name, value); }

    // ISerializable
    void JsonArchiver::BeginSerializeObject(const char* name, const ISerializable& value)
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

    void JsonArchiver::SerializeObject(const char* name, const ISerializable& value)
    {
        FinishPreviousLine();
        ++_indent;
        value.Serialize(*this); // TODO: need to somehow know if we're in an indenting context or not for the subsequent calls to WriteScalar
        --_indent;
    }

    void JsonArchiver::EndSerializeObject(const char* name, const ISerializable& value)
    {
        bool hasName = name != std::string("");
        _out << "\n"; // Output newline instead of calling "FinishPreviousLine"
        auto indent = GetCurrentIndent();
        _out << indent << "}";
        // need to output a comma if we're serializing a field (that is, if name != "")
        SetEndOfLine(hasName ? ",\n" : "\n");
    }

    void JsonArchiver::EndSerialization()
    {
        FinishPreviousLine();
    }

    //
    // Arrays
    //
    IMPLEMENT_SERIALIZE_ARRAY(JsonArchiver, bool);
    IMPLEMENT_SERIALIZE_ARRAY(JsonArchiver, char);
    IMPLEMENT_SERIALIZE_ARRAY(JsonArchiver, short);
    IMPLEMENT_SERIALIZE_ARRAY(JsonArchiver, int);
    IMPLEMENT_SERIALIZE_ARRAY(JsonArchiver, size_t);
    IMPLEMENT_SERIALIZE_ARRAY(JsonArchiver, float);
    IMPLEMENT_SERIALIZE_ARRAY(JsonArchiver, double);

    void JsonArchiver::SerializeArray(const char* name, const std::vector<std::string>& array)
    {
        WriteArray(name, array);
    }

    void JsonArchiver::SerializeArray(const char* name, const std::string& baseTypeName, const std::vector<const ISerializable*>& array)
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

    void JsonArchiver::Indent()
    {
        _out << GetCurrentIndent();
    }

    void JsonArchiver::FinishPreviousLine()
    {
        _out << _endOfPreviousLine;
        _endOfPreviousLine = "";
    }

    void JsonArchiver::SetEndOfLine(std::string endOfLine)
    {
        _endOfPreviousLine = endOfLine;
    }

    //
    // Deserialization
    //
    JsonUnarchiver::JsonUnarchiver(SerializationContext context) : Unarchiver(std::move(context)), _tokenizer(std::cin, ",:{}[]'\"") {}
    JsonUnarchiver::JsonUnarchiver(std::istream& inputStream, SerializationContext context) : Unarchiver(std::move(context)), _tokenizer(inputStream, ",:{}[]'\"") {}

    IMPLEMENT_DESERIALIZE_VALUE(JsonUnarchiver, bool);
    IMPLEMENT_DESERIALIZE_VALUE(JsonUnarchiver, char);
    IMPLEMENT_DESERIALIZE_VALUE(JsonUnarchiver, short);
    IMPLEMENT_DESERIALIZE_VALUE(JsonUnarchiver, int);
    IMPLEMENT_DESERIALIZE_VALUE(JsonUnarchiver, size_t);
    IMPLEMENT_DESERIALIZE_VALUE(JsonUnarchiver, float);
    IMPLEMENT_DESERIALIZE_VALUE(JsonUnarchiver, double);

    // strings
    void JsonUnarchiver::DeserializeValue(const char* name, std::string& value) 
    { 
        ReadScalar(name, value); 
    }

    // ISerializable
    std::string JsonUnarchiver::BeginDeserializeObject(const char* name, const std::string& typeName) 
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

    void JsonUnarchiver::DeserializeObject(const char* name, ISerializable& value) 
    {
        value.Deserialize(*this);
    }

    void JsonUnarchiver::EndDeserializeObject(const char* name, const std::string& typeName) 
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
    IMPLEMENT_DESERIALIZE_ARRAY(JsonUnarchiver, bool);
    IMPLEMENT_DESERIALIZE_ARRAY(JsonUnarchiver, char);
    IMPLEMENT_DESERIALIZE_ARRAY(JsonUnarchiver, short);
    IMPLEMENT_DESERIALIZE_ARRAY(JsonUnarchiver, int);
    IMPLEMENT_DESERIALIZE_ARRAY(JsonUnarchiver, size_t);
    IMPLEMENT_DESERIALIZE_ARRAY(JsonUnarchiver, float);
    IMPLEMENT_DESERIALIZE_ARRAY(JsonUnarchiver, double);

    void JsonUnarchiver::DeserializeArray(const char* name, std::vector<std::string>& array)
    {
        ReadArray(name, array);
    }

    void JsonUnarchiver::BeginDeserializeArray(const char* name, const std::string& typeName)
    {
        bool hasName = name != std::string("");
        if(hasName)
        {
            MatchFieldName(name);
        }
                
        _tokenizer.MatchToken("[");
    }

    bool JsonUnarchiver::BeginDeserializeArrayItem(const std::string& typeName)
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

    void JsonUnarchiver::EndDeserializeArrayItem(const std::string& typeName)
    {
        if(_tokenizer.PeekNextToken() == ",")
        {
            _tokenizer.ReadNextToken();
        }
    }
    
    void JsonUnarchiver::EndDeserializeArray(const char* name, const std::string& typeName)
    {
        _tokenizer.MatchToken("]");
    }

    void JsonUnarchiver::MatchFieldName(const char* key)
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
