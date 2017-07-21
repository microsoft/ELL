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

    IMPLEMENT_ARCHIVE_VALUE(XmlArchiver, bool);
    IMPLEMENT_ARCHIVE_VALUE(XmlArchiver, char);
    IMPLEMENT_ARCHIVE_VALUE(XmlArchiver, short);
    IMPLEMENT_ARCHIVE_VALUE(XmlArchiver, int);
    IMPLEMENT_ARCHIVE_VALUE(XmlArchiver, size_t);
    IMPLEMENT_ARCHIVE_VALUE(XmlArchiver, int64_t);
#ifdef __APPLE__
    IMPLEMENT_ARCHIVE_VALUE(XmlArchiver, uint64_t);
#endif
    IMPLEMENT_ARCHIVE_VALUE(XmlArchiver, float);
    IMPLEMENT_ARCHIVE_VALUE(XmlArchiver, double);

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
        DecrementIndent();
        auto indent = GetCurrentIndent();
        auto typeName = XmlUtilities::EncodeTypeName(GetArchivedTypeName(value));
        _out << indent;
        _out << "</" << typeName << ">" << std::endl;
    }

    //
    // Arrays
    //
    IMPLEMENT_ARCHIVE_ARRAY(XmlArchiver, bool);
    IMPLEMENT_ARCHIVE_ARRAY(XmlArchiver, char);
    IMPLEMENT_ARCHIVE_ARRAY(XmlArchiver, short);
    IMPLEMENT_ARCHIVE_ARRAY(XmlArchiver, int);
    IMPLEMENT_ARCHIVE_ARRAY(XmlArchiver, size_t);
    IMPLEMENT_ARCHIVE_ARRAY(XmlArchiver, int64_t);
#ifdef __APPLE__
    IMPLEMENT_ARCHIVE_ARRAY(XmlArchiver, uint64_t);
#endif
    IMPLEMENT_ARCHIVE_ARRAY(XmlArchiver, float);
    IMPLEMENT_ARCHIVE_ARRAY(XmlArchiver, double);

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

    IMPLEMENT_UNARCHIVE_VALUE(XmlUnarchiver, bool);
    IMPLEMENT_UNARCHIVE_VALUE(XmlUnarchiver, char);
    IMPLEMENT_UNARCHIVE_VALUE(XmlUnarchiver, short);
    IMPLEMENT_UNARCHIVE_VALUE(XmlUnarchiver, int);
    IMPLEMENT_UNARCHIVE_VALUE(XmlUnarchiver, size_t);
    IMPLEMENT_UNARCHIVE_VALUE(XmlUnarchiver, int64_t);
#ifdef __APPLE__
    IMPLEMENT_UNARCHIVE_VALUE(XmlUnarchiver, uint64_t);
#endif
    IMPLEMENT_UNARCHIVE_VALUE(XmlUnarchiver, float);
    IMPLEMENT_UNARCHIVE_VALUE(XmlUnarchiver, double);

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
        auto EncodedTypeName = XmlUtilities::EncodeTypeName(typeName);
        _tokenizer.MatchTokens({ "<", "/", EncodedTypeName, ">" });
    }

    //
    // Arrays
    //
    IMPLEMENT_UNARCHIVE_ARRAY(XmlUnarchiver, bool);
    IMPLEMENT_UNARCHIVE_ARRAY(XmlUnarchiver, char);
    IMPLEMENT_UNARCHIVE_ARRAY(XmlUnarchiver, short);
    IMPLEMENT_UNARCHIVE_ARRAY(XmlUnarchiver, int);
    IMPLEMENT_UNARCHIVE_ARRAY(XmlUnarchiver, size_t);
    IMPLEMENT_UNARCHIVE_ARRAY(XmlUnarchiver, int64_t);
#ifdef __APPLE__
    IMPLEMENT_UNARCHIVE_ARRAY(XmlUnarchiver, uint64_t);
#endif
    IMPLEMENT_UNARCHIVE_ARRAY(XmlUnarchiver, float);
    IMPLEMENT_UNARCHIVE_ARRAY(XmlUnarchiver, double);

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
    }

    void XmlUnarchiver::EndUnarchiveArray(const char* name, const std::string& typeName)
    {
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
