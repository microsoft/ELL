////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     JsonArchiver.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "JsonArchiver.h"
#include "Archiver.h"
#include "IArchivable.h"
#include "Unused.h"

#include <cassert>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>

namespace ell
{
namespace utilities
{
    //
    // Serialization
    //
    JsonArchiver::JsonArchiver()
        : _out(std::cout)
    {
    }

    JsonArchiver::JsonArchiver(std::ostream& outputStream)
        : _out(outputStream)
    {
    }

    #define ARCHIVE_TYPE_OP(t) IMPLEMENT_ARCHIVE_VALUE(JsonArchiver, t);
    ARCHIVABLE_TYPES_LIST
    #undef ARCHIVE_TYPE_OP

    // strings
    void JsonArchiver::ArchiveValue(const char* name, const std::string& value)
    {
        WriteScalar(name, value);
    }

    // IArchivable
    void JsonArchiver::BeginArchiveObject(const char* name, const IArchivable& value)
    {
        FinishPreviousLine();

        auto indent = GetCurrentIndent();
        _out << indent;
        bool hasName = name != std::string("");
        if (hasName)
        {
            _out << "\"" << name << "\": ";
        }

        if (value.ArchiveAsPrimitive())
        {
            return;
        }

        _out << "{\n";
        WriteObjectType(value);
        WriteObjectVersion(value);
        IncrementIndent();
    }

    void JsonArchiver::EndArchiveObject(const char* name, const IArchivable& value)
    {
        bool hasName = name != std::string("");
        if (!value.ArchiveAsPrimitive())
        {
            _out << "\n"; // Output newline instead of calling "FinishPreviousLine"
            DecrementIndent();
            auto indent = GetCurrentIndent();
            _out << indent << "}";
        }

        // need to output a comma if we're serializing a field (that is, if name != "")
        SetEndOfLine(hasName ? ",\n" : "\n");
    }

    void JsonArchiver::EndArchiving()
    {
        FinishPreviousLine();
    }

    //
    // Arrays
    //
    #define ARCHIVE_TYPE_OP(t) IMPLEMENT_ARCHIVE_ARRAY(JsonArchiver, t);
    ARCHIVABLE_TYPES_LIST
    #undef ARCHIVE_TYPE_OP

    void JsonArchiver::ArchiveArray(const char* name, const std::vector<std::string>& array)
    {
        WriteArray(name, array);
    }

    void JsonArchiver::ArchiveArray(const char* name, const std::string& baseTypeName, const std::vector<const IArchivable*>& array)
    {
        UNUSED(baseTypeName);
        FinishPreviousLine();
        auto indent = GetCurrentIndent();
        bool hasName = name != std::string("");

        _out << indent;
        if (hasName)
        {
            _out << "\"" << name << "\": ";
        }

        _out << "[\n";

        auto numItems = array.size();
        for (size_t index = 0; index < numItems; ++index)
        {
            Archive(*array[index]);
            if (index != numItems - 1)
            {
                _out << ", ";
            }
        }
        _out << "]";
        SetEndOfLine(",\n");
    }

    void JsonArchiver::WriteObjectType(const IArchivable& value)
    {
        auto indent = GetCurrentIndent();
        _out << indent << "  \"_type\": \"" << GetArchivedTypeName(value) << "\",\n";
    }

    void JsonArchiver::WriteObjectVersion(const IArchivable& value)
    {
        auto indent = GetCurrentIndent();
        auto version = GetArchiveVersion(value);
        _out << indent << "  \"_version\": \"" << version.versionNumber << "\",\n";
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
    JsonUnarchiver::JsonUnarchiver(SerializationContext context)
        : Unarchiver(std::move(context)), _tokenizer(std::cin, ",:{}[]'\"")
    {
    }

    JsonUnarchiver::JsonUnarchiver(std::istream& inputStream, SerializationContext context)
        : Unarchiver(std::move(context)), _tokenizer(inputStream, ",:{}[]'\"")
    {
    }

    #define ARCHIVE_TYPE_OP(t) IMPLEMENT_UNARCHIVE_VALUE(JsonUnarchiver, t);
    ARCHIVABLE_TYPES_LIST
    #undef ARCHIVE_TYPE_OP

    // strings
    void JsonUnarchiver::UnarchiveValue(const char* name, std::string& value)
    {
        ReadScalar(name, value);
    }

    // IArchivable
    ArchivedObjectInfo JsonUnarchiver::BeginUnarchiveObject(const char* name, const std::string& typeName)
    {
        UNUSED(typeName);
        bool hasName = name != std::string("");
        if (hasName)
        {
            MatchFieldName(name);
        }

        _tokenizer.MatchToken("{");

        MatchFieldName("_type");
        _tokenizer.MatchToken("\"");
        auto encodedTypeName = _tokenizer.ReadNextToken();
        assert(encodedTypeName != "");
        _tokenizer.MatchToken("\"");

        int version = 0;
        if (_tokenizer.PeekNextToken() == ",")
        {
            _tokenizer.ReadNextToken(); // eat the comma

            MatchFieldName("_version");
            _tokenizer.MatchToken("\"");
            auto versionString = _tokenizer.ReadNextToken();
            // parse it
            version = std::stoi(versionString);
            _tokenizer.MatchToken("\"");
            if (_tokenizer.PeekNextToken() == ",")
            {
                _tokenizer.ReadNextToken(); // eat the comma
            }
        }
        return { encodedTypeName, version };
    }

    void JsonUnarchiver::UnarchiveObjectAsPrimitive(const char* name, IArchivable& value)
    {
        bool hasName = name != std::string("");
        if (hasName)
        {
            MatchFieldName(name);
        }

        UnarchiveObject(name, value);

        // eat a comma if it exists
        if (hasName)
        {
            if (_tokenizer.PeekNextToken() == ",")
            {
                _tokenizer.ReadNextToken();
            }
        }
    }

    bool JsonUnarchiver::HasNextPropertyName(const std::string& name)
    {
        bool ok = true;
        std::string nextPropertyName = "";
        std::vector<std::string> readTokens;
        if (_tokenizer.TryMatchToken("\""))
        {
            readTokens.push_back("\"");
            nextPropertyName = _tokenizer.ReadNextToken();
            readTokens.push_back(nextPropertyName);
            if (_tokenizer.TryMatchToken("\""))
            {
                readTokens.push_back("\"");
                if (_tokenizer.TryMatchToken(":"))
                {
                    readTokens.push_back(":");
                }
                else
                {
                    ok = false;
                }
            }
            else
            {
                ok = false;
            }
        }

        // put back all read tokens
        while (!readTokens.empty())
        {
            _tokenizer.PutBackToken(readTokens.back());
            readTokens.pop_back();
        }

        return ok && nextPropertyName == name;
    }

    void JsonUnarchiver::EndUnarchiveObject(const char* name, const std::string& typeName)
    {
        UNUSED(typeName);
        bool hasName = name != std::string("");
        _tokenizer.MatchToken("}");

        // eat a comma if it exists
        if (hasName)
        {
            if (_tokenizer.PeekNextToken() == ",")
            {
                _tokenizer.ReadNextToken();
            }
        }
    }

    //
    // Arrays
    //
    #define ARCHIVE_TYPE_OP(t) IMPLEMENT_UNARCHIVE_ARRAY(JsonUnarchiver, t);
    ARCHIVABLE_TYPES_LIST
    #undef ARCHIVE_TYPE_OP

    void JsonUnarchiver::UnarchiveArray(const char* name, std::vector<std::string>& array)
    {
        ReadArray(name, array);
    }

    void JsonUnarchiver::BeginUnarchiveArray(const char* name, const std::string& typeName)
    {
        UNUSED(typeName);
        bool hasName = name != std::string("");
        if (hasName)
        {
            MatchFieldName(name);
        }

        _tokenizer.MatchToken("[");
    }

    bool JsonUnarchiver::BeginUnarchiveArrayItem(const std::string& typeName)
    {
        UNUSED(typeName);
        auto maybeEndArray = _tokenizer.PeekNextToken();
        if (maybeEndArray == "]")
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    void JsonUnarchiver::EndUnarchiveArrayItem(const std::string& typeName)
    {
        UNUSED(typeName);
        if (_tokenizer.PeekNextToken() == ",")
        {
            _tokenizer.ReadNextToken();
        }
    }

    void JsonUnarchiver::EndUnarchiveArray(const char* name, const std::string& typeName)
    {
        UNUSED(typeName);
        bool hasName = name != std::string("");
        _tokenizer.MatchToken("]");
        // eat a comma if it exists
        if (hasName)
        {
            if (_tokenizer.PeekNextToken() == ",")
            {
                _tokenizer.ReadNextToken();
            }
        }
    }

    bool JsonUnarchiver::TryMatchFieldName(const char* key, std::string& found)
    {
        bool ok = _tokenizer.TryMatchToken("\"");
        if (!ok)
        {
            return false;
        }

        auto s = _tokenizer.PeekNextToken();
        if (s != key)
        {
            found = s;
            return false;
        }
        _tokenizer.ReadNextToken();

        _tokenizer.MatchTokens({ "\"", ":" });
        return true;
    }

    void JsonUnarchiver::MatchFieldName(const char* key)
    {
        std::string found = "non literal";
        if (!TryMatchFieldName(key, found))
        {
            throw InputException(InputExceptionErrors::badStringFormat, std::string{ "Failed to match field " } + key + ", instead found token '" + found + "'");
        }
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
        for (auto ch : str)
        {
            auto encoding = charCodes[ch];
            if (encoding == '\0') // no encoding
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
        for (auto ch : str)
        {
            if (prevWasBackslash)
            {
                auto encoding = charCodes[ch];
                if (encoding == '\0') // nothing special
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
                if (ch == '\\')
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

        if (prevWasBackslash)
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
}
