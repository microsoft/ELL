////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     XmlArchiver.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "XmlArchiver.h"
#include "Archiver.h"
#include "IArchivable.h"
#include "Unused.h"

// stl
#include <algorithm>
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
    XmlArchiver::XmlArchiver()
        : _out(std::cout), _ready(true)
    {
        WriteFileHeader();
    }

    XmlArchiver::XmlArchiver(std::ostream& outputStream)
        : _out(outputStream), _ready(true)
    {
        WriteFileHeader();
    }

    void XmlArchiver::WriteFileHeader()
    {
        // Write XML declaration
        _out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
        _out << "<ell version=\"1.0\">\n";
    }

    void XmlArchiver::WriteFileFooter()
    {
        _out << "</ell>\n";
    }

    #define ARCHIVE_TYPE_OP(t) IMPLEMENT_ARCHIVE_VALUE(XmlArchiver, t);
    ARCHIVABLE_TYPES_LIST
    #undef ARCHIVE_TYPE_OP

    // strings
    void XmlArchiver::ArchiveValue(const char* name, const std::string& value)
    {
        WriteScalar(name, value);
    }

    // IArchivable
    void XmlArchiver::BeginArchiveObject(const char* name, const IArchivable& value)
    {
        auto indent = GetCurrentIndent();
        auto typeName = XmlUtilities::EncodeTypeName(GetArchivedTypeName(value));

        _out << indent;
        _out << "<" << typeName;

        if (name != std::string(""))
        {
            _out << " name='" << name << "'";
        }
        _out << ">" << std::endl;
        IncrementIndent();
    }

    void XmlArchiver::EndArchiveObject(const char* name, const IArchivable& value)
    {
        UNUSED(name);
        DecrementIndent();
        auto indent = GetCurrentIndent();
        auto typeName = XmlUtilities::EncodeTypeName(GetArchivedTypeName(value));
        _out << indent;
        _out << "</" << typeName << ">" << std::endl;
    }

    //
    // Arrays
    //
    #define ARCHIVE_TYPE_OP(t) IMPLEMENT_ARCHIVE_ARRAY(XmlArchiver, t);
    ARCHIVABLE_TYPES_LIST
    #undef ARCHIVE_TYPE_OP

    void XmlArchiver::ArchiveArray(const char* name, const std::vector<std::string>& array)
    {
        WriteArray(name, array);
    }

    // Array of pointers-to-IArchivable
    // TOOD: pass in compile-time type name
    void XmlArchiver::ArchiveArray(const char* name, const std::string& baseTypeName, const std::vector<const IArchivable*>& array)
    {
        bool hasName = name != std::string("");
        auto indent = GetCurrentIndent();
        auto endOfLine = "\n";

        _out << indent;
        _out << "<Array";
        if (hasName)
        {
            _out << " name='" << name << "'";
        }
        _out << " type='" << baseTypeName << "'>" << std::endl;
        IncrementIndent();
        for (const auto& item : array)
        {
            Archive(*item);
        }
        DecrementIndent();
        _out << indent;
        _out << "</Array>" << endOfLine;
    }

    void XmlArchiver::SetIndent(int indent)
    {
        _indent = indent;
    }

    void XmlArchiver::EndArchiving()
    {
        WriteFileFooter();
        _out.flush();
    }

    //
    // Deserialization
    //
    XmlUnarchiver::XmlUnarchiver(SerializationContext context)
        : Unarchiver(std::move(context)), _tokenizer(std::cin, "<>=/'\"")
    {
        ReadFileHeader();
    }

    XmlUnarchiver::XmlUnarchiver(std::istream& inputStream, SerializationContext context)
        : Unarchiver(std::move(context)), _tokenizer(inputStream, "<>?=/'\"")
    {
        ReadFileHeader();
    }

    #define ARCHIVE_TYPE_OP(t) IMPLEMENT_UNARCHIVE_VALUE(XmlUnarchiver, t);
    ARCHIVABLE_TYPES_LIST
    #undef ARCHIVE_TYPE_OP

    void XmlUnarchiver::ReadFileHeader()
    {
        _tokenizer.MatchTokens({ "<", "?", "xml" });
        while (_tokenizer.PeekNextToken() != "?")
        {
            _tokenizer.ReadNextToken();
        }
        _tokenizer.MatchTokens({ "?", ">" });
        _tokenizer.MatchTokens({ "<", "ell", "version", "=", "\"", "1.0", "\"", ">" });
    }

    void XmlUnarchiver::ReadFileFooter()
    {
        _tokenizer.MatchTokens({ "<", "/", "ell", ">" });
    }

    // strings
    void XmlUnarchiver::UnarchiveValue(const char* name, std::string& value)
    {
        ReadScalar(name, value);
    }

    // IArchivable
    ArchivedObjectInfo XmlUnarchiver::BeginUnarchiveObject(const char* name, const std::string& typeName)
    {
        bool hasName = name != std::string("");
        auto rawTypeName = typeName;

        _tokenizer.MatchToken("<");
        auto readTypeName = XmlUtilities::DecodeTypeName(_tokenizer.ReadNextToken());
        assert(readTypeName != "");
        if (hasName)
        {
            _tokenizer.MatchTokens({ "name", "=", "'", name, "'" });
        }
        _tokenizer.MatchToken(">");
        return {readTypeName, 0};
    }

    void XmlUnarchiver::EndUnarchiveObject(const char* name, const std::string& typeName)
    {
        UNUSED(name);
        auto EncodedTypeName = XmlUtilities::EncodeTypeName(typeName);
        _tokenizer.MatchTokens({ "<", "/", EncodedTypeName, ">" });
    }

    bool XmlUnarchiver::HasNextPropertyName(const std::string&)
    {
        assert(false && "Unimplemented");
        return false;
    }

    //
    // Arrays
    //
    #define ARCHIVE_TYPE_OP(t) IMPLEMENT_UNARCHIVE_ARRAY(XmlUnarchiver, t);
    ARCHIVABLE_TYPES_LIST
    #undef ARCHIVE_TYPE_OP

    void XmlUnarchiver::UnarchiveArray(const char* name, std::vector<std::string>& array)
    {
        ReadArray(name, array);
    }

    void XmlUnarchiver::BeginUnarchiveArray(const char* name, const std::string& typeName)
    {
        bool hasName = name != std::string("");

        _tokenizer.MatchTokens({ "<", "Array" });
        if (hasName)
        {
            _tokenizer.MatchTokens({ "name", "=", "'", name, "'" });
        }

        _tokenizer.MatchTokens({ "type", "=", "'", typeName, "'", ">" });
    }

    bool XmlUnarchiver::BeginUnarchiveArrayItem(const std::string& typeName)
    {
        UNUSED(typeName);
        // check for '</'
        auto token1 = _tokenizer.ReadNextToken();
        auto token2 = _tokenizer.ReadNextToken();
        _tokenizer.PutBackToken(token2);
        _tokenizer.PutBackToken(token1);
        if (token1 + token2 == "</")
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    void XmlUnarchiver::EndUnarchiveArrayItem(const std::string& typeName)
    {
        UNUSED(typeName);
    }

    void XmlUnarchiver::EndUnarchiveArray(const char* name, const std::string& typeName)
    {
        UNUSED(name, typeName);
        _tokenizer.MatchTokens({ "<", "/", "Array", ">" });
    }

    void XmlUnarchiver::EndUnarchiving()
    {
        ReadFileFooter();
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
}
