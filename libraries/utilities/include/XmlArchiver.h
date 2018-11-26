////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     XmlArchiver.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Archiver.h"
#include "Exception.h"
#include "Tokenizer.h"
#include "TypeFactory.h"
#include "TypeName.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

namespace ell
{
namespace utilities
{
    /// <summary> An archiver that encodes data in an XML format. </summary>
    class XmlArchiver : public Archiver
    {
    public:
        /// <summary> Default Constructor --- writes to standard output. </summary>
        XmlArchiver();

        /// <summary> Constructor </summary>
        ///
        /// <param name="outputStream"> The stream to write data to. </summary>
        XmlArchiver(std::ostream& outputStream);

    protected:
#define ARCHIVE_TYPE_OP(t) DECLARE_ARCHIVE_VALUE_OVERRIDE(t);
        ARCHIVABLE_TYPES_LIST
#undef ARCHIVE_TYPE_OP
        void ArchiveValue(const char* name, const std::string& value) override;

#define ARCHIVE_TYPE_OP(t) DECLARE_ARCHIVE_ARRAY_OVERRIDE(t);
        ARCHIVABLE_TYPES_LIST
#undef ARCHIVE_TYPE_OP

        void ArchiveNull(const char* name) override;

        void ArchiveArray(const char* name, const std::vector<std::string>& array) override;
        void ArchiveArray(const char* name, const std::string& baseTypeName, const std::vector<const IArchivable*>& array) override;

        void BeginArchiveObject(const char* name, const IArchivable& value) override;
        void EndArchiveObject(const char* name, const IArchivable& value) override;

        void EndArchiving() override;

    private:
        // Serialization
        void WriteFileHeader();
        void WriteFileFooter();

        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void WriteScalar(const char* name, const ValueType& value);

        void WriteScalar(const char* name, const char* value);
        void WriteScalar(const char* name, const std::string& value);

        template <typename ValueType>
        void WriteArray(const char* name, const std::vector<ValueType>& array);

        void CloseStream();

        std::string GetCurrentIndent() { return std::string(2 * _indent, ' '); }
        void IncrementIndent() { ++_indent; }
        void DecrementIndent() { --_indent; }
        void SetIndent(int indent);

        std::ostream& _out;
        bool _ready = false;
        int _indent = 0;
    };

    /// <summary> An unarchiver that reads data encoded in an XML format. </summary>
    class XmlUnarchiver : public Unarchiver
    {
    public:
        /// <summary> Default Constructor --- reads from standard input. </summary>
        XmlUnarchiver(SerializationContext context);

        /// <summary> Constructor </summary>
        ///
        /// <param name="inputStream"> The stream to read data from. </summary>
        XmlUnarchiver(std::istream& inputStream, SerializationContext context);

        /// <summary> Indicates if a property with the given name is available to be read next </summary>
        ///
        /// <param name="name"> The name of the property </param>
        ///
        /// <returns> true if a property with the given name can be read next </returns>
        bool HasNextPropertyName(const std::string& name) override;

    protected:
#define ARCHIVE_TYPE_OP(t) DECLARE_UNARCHIVE_VALUE_OVERRIDE(t);
        ARCHIVABLE_TYPES_LIST
#undef ARCHIVE_TYPE_OP

        void UnarchiveValue(const char* name, std::string& value) override;

        bool UnarchiveNull(const char* name) override;

#define ARCHIVE_TYPE_OP(t) DECLARE_UNARCHIVE_ARRAY_OVERRIDE(t);
        ARCHIVABLE_TYPES_LIST
#undef ARCHIVE_TYPE_OP

        void UnarchiveArray(const char* name, std::vector<std::string>& array) override;
        void BeginUnarchiveArray(const char* name, const std::string& typeName) override;
        bool BeginUnarchiveArrayItem(const std::string& typeName) override;
        void EndUnarchiveArrayItem(const std::string& typeName) override;
        void EndUnarchiveArray(const char* name, const std::string& typeName) override;

        ArchivedObjectInfo BeginUnarchiveObject(const char* name, const std::string& typeName) override;
        void EndUnarchiveObject(const char* name, const std::string& typeName) override;

        void EndUnarchiving() override;

    private:
        // Deserialization
        void ReadFileHeader();
        void ReadFileFooter();

        template <typename ValueType, IsIntegral<ValueType> concept = 0>
        void ReadScalar(const char* name, ValueType& value);
        template <typename ValueType, IsFloatingPoint<ValueType> concept = 0>
        void ReadScalar(const char* name, ValueType& value);
        void ReadScalar(const char* name, std::string& value);
        template <typename ValueType, IsFundamental<ValueType> concept = 0>
        void ReadArray(const char* name, std::vector<ValueType>& array);
        void ReadArray(const char* name, std::vector<std::string>& array);

        Tokenizer _tokenizer;
    };

    /// <summary> XML utility functions --- for internal use by `XMLArchiver` and `XMLUnarchiver` </summary>
    class XmlUtilities
    {
    public:
        static std::string EncodeAttributeString(const std::string& str);
        static std::string DecodeAttributeString(const std::string& str);
        static std::string EncodeTypeName(const std::string& str);
        static std::string DecodeTypeName(const std::string& str);
    };
} // namespace utilities
} // namespace ell

#pragma region implementation

namespace ell
{
namespace utilities
{
    //
    // Serialization
    //
    template <typename ValueType, IsFundamental<ValueType> concept>
    void XmlArchiver::WriteScalar(const char* name, const ValueType& value)
    {
        EnsureMaxPrecision<ValueType> precisionScope(_out);
        using std::to_string;
        auto indent = GetCurrentIndent();
        bool hasName = name != std::string("");
        auto endOfLine = hasName ? "\n" : "";
        auto typeName = XmlUtilities::EncodeTypeName(GetArchivedTypeName<ValueType>());

        _out << indent;
        _out << "<" << typeName;

        if (hasName)
        {
            _out << " name='" << name << "'";
        }

        _out << " value='" << to_string(value) << "'/>" << endOfLine;
    }

    // Specialization for bool (though perhaps this should be an overload, not a specialization)
    template <>
    inline void XmlArchiver::WriteScalar(const char* name, const bool& value)
    {
        auto indent = GetCurrentIndent();
        bool hasName = name != std::string("");
        auto endOfLine = hasName ? "\n" : "";
        auto typeName = "bool";

        _out << indent;
        _out << "<" << typeName;

        if (hasName)
        {
            _out << " name='" << name << "'";
        }

        _out << " value='" << (value ? "true" : "false") << "'/>" << endOfLine;
    }

    inline std::string XmlEncodeString(std::string s)
    {
        return s;
    }

    // This function is inline just so it appears next to the other Write* functions
    inline void XmlArchiver::WriteScalar(const char* name, const char* value)
    {
        auto indent = GetCurrentIndent();
        bool hasName = name != std::string("");
        auto endOfLine = hasName ? "\n" : "";
        auto typeName = "string";

        _out << indent;
        _out << "<" << typeName;

        if (hasName)
        {
            _out << " name='" << name << "'";
        }
        _out << " value='" << XmlUtilities::EncodeAttributeString(value) << "'/>" << endOfLine;
    }

    inline void XmlArchiver::WriteScalar(const char* name, const std::string& value)
    {
        auto indent = GetCurrentIndent();
        bool hasName = name != std::string("");
        auto endOfLine = hasName ? "\n" : "";
        auto typeName = "string";

        _out << indent;
        _out << "<" << typeName;

        if (hasName)
        {
            _out << " name='" << name << "'";
        }
        _out << " value='" << XmlUtilities::EncodeAttributeString(value) << "'/>" << endOfLine;
    }

    template <typename ValueType>
    void XmlArchiver::WriteArray(const char* name, const std::vector<ValueType>& array)
    {
        bool hasName = name != std::string("");
        auto indent = GetCurrentIndent();
        auto typeName = XmlUtilities::EncodeTypeName(GetArchivedTypeName<ValueType>());

        _out << indent;
        _out << "<Array";
        if (hasName)
        {
            _out << " name='" << name << "'";
        }
        _out << " type='" << typeName << "'>" << std::endl;

        // Indent the next line (the line with the array elements), and then
        // set the indent to 0 (so there isn't indentation inside the line)
        auto oldIndent = _indent;
        IncrementIndent();
        indent = GetCurrentIndent();
        _out << indent;

        SetIndent(0);
        for (const auto& item : array)
        {
            Archive(item);
            _out << " ";
        }
        SetIndent(oldIndent);
        _out << std::endl;
        _out << indent;
        _out << "</Array>" << std::endl;
    }

    //
    // Deserialization
    //
    template <typename ValueType, IsIntegral<ValueType> concept>
    void XmlUnarchiver::ReadScalar(const char* name, ValueType& value)
    {
        auto typeName = XmlUtilities::EncodeTypeName(GetArchivedTypeName<ValueType>());
        bool hasName = name != std::string("");

        _tokenizer.MatchTokens({ "<", typeName });
        if (hasName)
        {
            _tokenizer.MatchTokens({ "name", "=", "'", name, "'" });
        }
        _tokenizer.MatchTokens({ "value", "=", "'" });

        // read value
        auto valueToken = _tokenizer.ReadNextToken();
        value = static_cast<ValueType>(std::stoll(valueToken));

        _tokenizer.MatchTokens({ "'", "/", ">" });
    }

    template <typename ValueType, IsFloatingPoint<ValueType> concept>
    void XmlUnarchiver::ReadScalar(const char* name, ValueType& value)
    {
        auto typeName = XmlUtilities::EncodeTypeName(GetArchivedTypeName<ValueType>());
        bool hasName = name != std::string("");

        _tokenizer.MatchTokens({ "<", typeName });
        if (hasName)
        {
            _tokenizer.MatchTokens({ "name", "=", "'", name, "'" });
        }
        _tokenizer.MatchTokens({ "value", "=", "'" });

        // read value
        auto valueToken = _tokenizer.ReadNextToken();
        value = static_cast<ValueType>(std::stod(valueToken));

        _tokenizer.MatchTokens({ "'", "/", ">" });
    }

    template <>
    inline void XmlUnarchiver::ReadScalar(const char* name, bool& value)
    {
        auto typeName = "bool";
        bool hasName = name != std::string("");

        _tokenizer.MatchTokens({ "<", typeName });
        if (hasName)
        {
            _tokenizer.MatchTokens({ "name", "=", "'", name, "'" });
        }
        _tokenizer.MatchTokens({ "value", "=", "'" });

        // read value
        auto valueToken = _tokenizer.ReadNextToken();
        value = (valueToken == "true");

        _tokenizer.MatchTokens({ "'", "/", ">" });
    }

    // This function is inline just so it appears next to the other Read* functions
    inline void XmlUnarchiver::ReadScalar(const char* name, std::string& value)
    {
        auto typeName = "string";
        bool hasName = name != std::string("");

        _tokenizer.MatchTokens({ "<", typeName });
        if (hasName)
        {
            _tokenizer.MatchTokens({ "name", "=", "'", name, "'" });
        }
        _tokenizer.MatchTokens({ "value", "=", "'" });

        // read value
        auto valueToken = _tokenizer.ReadNextToken();
        value = XmlUtilities::DecodeAttributeString(valueToken);

        _tokenizer.MatchTokens({ "'", "/", ">" });
    }

    template <typename ValueType, IsFundamental<ValueType> concept>
    void XmlUnarchiver::ReadArray(const char* name, std::vector<ValueType>& array)
    {
        auto typeName = XmlUtilities::EncodeTypeName(GetArchivedTypeName<ValueType>());
        bool hasName = name != std::string("");

        _tokenizer.MatchTokens({ "<", "Array" });
        if (hasName)
        {
            _tokenizer.MatchTokens({ "name", "=", "'", name, "'" });
        }

        _tokenizer.MatchTokens({ "type", "=", "'", typeName, "'", ">" });
        while (true)
        {
            ValueType obj;
            Unarchive(obj);
            array.push_back(obj);

            // check for '</'
            auto token1 = _tokenizer.ReadNextToken();
            auto token2 = _tokenizer.ReadNextToken();
            _tokenizer.PutBackToken(token2);
            _tokenizer.PutBackToken(token1);
            if (token1 + token2 == "</")
            {
                break;
            }
        }

        _tokenizer.MatchTokens({ "<", "/", "Array", ">" });
    }

    inline void XmlUnarchiver::ReadArray(const char* name, std::vector<std::string>& array)
    {
        auto typeName = XmlUtilities::EncodeTypeName(TypeName<std::string>::GetName());
        bool hasName = name != std::string("");

        _tokenizer.MatchTokens({ "<", "Array" });
        if (hasName)
        {
            _tokenizer.MatchTokens({ "name", "=", "'", name, "'" });
        }

        _tokenizer.MatchTokens({ "type", "=", "'", typeName, "'", ">" });

        std::string nextToken = "";
        while (true)
        {
            std::string obj;
            Unarchive(obj);
            array.push_back(obj);

            // check for '</'
            auto token1 = _tokenizer.ReadNextToken();
            auto token2 = _tokenizer.ReadNextToken();
            _tokenizer.PutBackToken(token2);
            _tokenizer.PutBackToken(token1);
            if (token1 + token2 == "</")
            {
                break;
            }
        }

        _tokenizer.MatchTokens({ "<", "/", "Array", ">" });
    }
} // namespace utilities
} // namespace ell

#pragma endregion implementation
