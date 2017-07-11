////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     XmlArchiver.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

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
        auto typeName = XmlUtilities::EncodeTypeName(GetArchivedTypeName<bool>());

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
        auto typeName = XmlUtilities::EncodeTypeName(TypeName<bool>::GetName());
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
}
}
