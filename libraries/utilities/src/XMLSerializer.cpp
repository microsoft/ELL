////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SimpleXmlSerializer.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "XMLSerializer.h"
#include "Serializer.h"
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
    SimpleXmlSerializer::SimpleXmlSerializer() : _out(std::cout) 
    {
        WriteFileHeader();
    }

    SimpleXmlSerializer::SimpleXmlSerializer(std::ostream& outputStream) : _out(outputStream) 
    {
        WriteFileHeader();
    }

    SimpleXmlSerializer::~SimpleXmlSerializer()
    {
        WriteFileFooter();
    }

    void SimpleXmlSerializer::WriteFileHeader()
    {   
        // Write XML declaration
        _out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";     
        _out << "<emll version=\"1.0\">\n";
    }

    void SimpleXmlSerializer::WriteFileFooter()
    {
        _out << "</emll>\n";
    }

    IMPLEMENT_SERIALIZE_VALUE(SimpleXmlSerializer, bool);
    IMPLEMENT_SERIALIZE_VALUE(SimpleXmlSerializer, char);
    IMPLEMENT_SERIALIZE_VALUE(SimpleXmlSerializer, short);
    IMPLEMENT_SERIALIZE_VALUE(SimpleXmlSerializer, int);
    IMPLEMENT_SERIALIZE_VALUE(SimpleXmlSerializer, size_t);
    IMPLEMENT_SERIALIZE_VALUE(SimpleXmlSerializer, float);
    IMPLEMENT_SERIALIZE_VALUE(SimpleXmlSerializer, double);

    // strings
    void SimpleXmlSerializer::SerializeValue(const char* name, const std::string& value) { WriteScalar(name, value); }

    // ISerializable
    void SimpleXmlSerializer::BeginSerializeObject(const char* name, const ISerializable& value)
    {
        auto indent = GetCurrentIndent();
        auto typeName = XmlUtilities::EncodeTypeName(value.GetRuntimeTypeName());

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
        auto typeName = XmlUtilities::EncodeTypeName(value.GetRuntimeTypeName());
        _out << indent;
        _out << "</" << typeName << ">" << std::endl;
    }

    //
    // Arrays
    //
    IMPLEMENT_SERIALIZE_ARRAY(SimpleXmlSerializer, bool);
    IMPLEMENT_SERIALIZE_ARRAY(SimpleXmlSerializer, char);
    IMPLEMENT_SERIALIZE_ARRAY(SimpleXmlSerializer, short);
    IMPLEMENT_SERIALIZE_ARRAY(SimpleXmlSerializer, int);
    IMPLEMENT_SERIALIZE_ARRAY(SimpleXmlSerializer, size_t);
    IMPLEMENT_SERIALIZE_ARRAY(SimpleXmlSerializer, float);
    IMPLEMENT_SERIALIZE_ARRAY(SimpleXmlSerializer, double);

    // Array of pointers-to-ISerializable
    // TOOD: pass in compile-time type name
    void SimpleXmlSerializer::SerializeArray(const char* name, const std::string& baseTypeName, const std::vector<const ISerializable*>& array)
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
        _out << " type='" << baseTypeName <<  "'>" << std::endl;
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
    SimpleXmlDeserializer::SimpleXmlDeserializer() : _in(std::cin), _tokenizer(std::cin, "<>=/'\"") 
    {
        ReadFileHeader();
    }

    SimpleXmlDeserializer::SimpleXmlDeserializer(std::istream& inputStream) : _in(inputStream), _tokenizer(inputStream, "<>?=/'\"") 
    {
        ReadFileHeader(); 
    }

    SimpleXmlDeserializer::~SimpleXmlDeserializer() 
    {
        ReadFileFooter(); 
    }

    IMPLEMENT_DESERIALIZE_VALUE(SimpleXmlDeserializer, bool);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleXmlDeserializer, char);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleXmlDeserializer, short);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleXmlDeserializer, int);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleXmlDeserializer, size_t);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleXmlDeserializer, float);
    IMPLEMENT_DESERIALIZE_VALUE(SimpleXmlDeserializer, double);

    // TODO: add a "read tag"-type function
    void SimpleXmlDeserializer::ReadFileHeader()
    {   
        _tokenizer.MatchTokens({"<", "?", "xml"});
        while(_tokenizer.PeekNextToken() != "?")
        {
            _tokenizer.ReadNextToken();
        }
        _tokenizer.MatchTokens({"?", ">"});
        _tokenizer.MatchTokens({"<", "emll", "version", "=", "\"", "1.0", "\"", ">"});
    }

    void SimpleXmlDeserializer::ReadFileFooter()
    {
        _tokenizer.MatchTokens({"<", "/", "emll", ">"});
    }

    // strings
    void SimpleXmlDeserializer::DeserializeValue(const char* name, std::string& value, SerializationContext& context) { ReadScalar(name, value); }

    // ISerializable
    std::string SimpleXmlDeserializer::BeginDeserializeObject(const char* name, const std::string& typeName, SerializationContext& context) 
    {
        bool hasName = name != std::string("");
        auto rawTypeName = typeName;

        _tokenizer.MatchToken("<");
        auto readTypeName = XmlUtilities::DecodeTypeName(_tokenizer.ReadNextToken());
        if(hasName)
        {
            _tokenizer.MatchTokens({"name", "=", "'", name, "'"});
        }
        _tokenizer.MatchToken(">");
        return readTypeName;
    }

    void SimpleXmlDeserializer::DeserializeObject(const char* name, ISerializable& value, SerializationContext& context) 
    {
        value.Deserialize(*this, context);
    }

    void SimpleXmlDeserializer::EndDeserializeObject(const char* name, const std::string& typeName, SerializationContext& context) 
    {
        auto EncodedTypeName = XmlUtilities::EncodeTypeName(typeName);
        _tokenizer.MatchTokens({"<", "/", EncodedTypeName, ">"});
    }

    //
    // Arrays
    //
    IMPLEMENT_DESERIALIZE_ARRAY(SimpleXmlDeserializer, bool);
    IMPLEMENT_DESERIALIZE_ARRAY(SimpleXmlDeserializer, char);
    IMPLEMENT_DESERIALIZE_ARRAY(SimpleXmlDeserializer, short);
    IMPLEMENT_DESERIALIZE_ARRAY(SimpleXmlDeserializer, int);
    IMPLEMENT_DESERIALIZE_ARRAY(SimpleXmlDeserializer, size_t);
    IMPLEMENT_DESERIALIZE_ARRAY(SimpleXmlDeserializer, float);
    IMPLEMENT_DESERIALIZE_ARRAY(SimpleXmlDeserializer, double);

    void SimpleXmlDeserializer::DeserializeArray(const char* name, std::vector<std::string>& array, SerializationContext& context)
    {
        ReadArray(name, array, context);
    }

    void SimpleXmlDeserializer::BeginDeserializeArray(const char* name, const std::string& typeName, SerializationContext& context)
    {
        bool hasName = name != std::string("");

        _tokenizer.MatchTokens({"<", "Array"});
        if(hasName)
        {
            _tokenizer.MatchTokens({"name", "=", "'", name, "'"});
        }

        _tokenizer.MatchTokens({"type", "=", "'", typeName, "'", ">"});
    }

    bool SimpleXmlDeserializer::BeginDeserializeArrayItem(const std::string& typeName, SerializationContext& context)
    {
        // check for '</'
        auto token1 = _tokenizer.ReadNextToken();
        auto token2 = _tokenizer.ReadNextToken();
        _tokenizer.PutBackToken(token2);
        _tokenizer.PutBackToken(token1);
        if(token1+token2 == "</")
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    void SimpleXmlDeserializer::EndDeserializeArrayItem(const std::string& typeName, SerializationContext& context)
    {
    }

    void SimpleXmlDeserializer::EndDeserializeArray(const char* name, const std::string& typeName, SerializationContext& context)
    {
        _tokenizer.MatchTokens({"<", "/", "Array", ">"});
    }

    //
    // XmlUtilities
    //
    std::string XmlUtilities::EncodeAttributeString(const std::string& str)
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

    std::string XmlUtilities::DecodeAttributeString(const std::string& str)
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
        std::string XmlUtilities::EncodeTypeName(const std::string& str)
    {
        // convert '<' into '(' etc.
        auto result = str;
        std::replace(result.begin(), result.end(), '<', '(');
        std::replace(result.begin(), result.end(), '>', ')');
        return result;
    }

    std::string XmlUtilities::DecodeTypeName(const std::string& str)
    {
        // convert '(' into '<' etc.
        auto result = str;
        std::replace(result.begin(), result.end(), '(', '<');
        std::replace(result.begin(), result.end(), ')', '>');
        return result;
    }
}
