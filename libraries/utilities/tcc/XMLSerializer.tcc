////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SimpleXmlSerializer.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace utilities
{
    //
    // Serialization
    //
    template <typename ValueType, IsFundamental<ValueType> concept>
    void SimpleXmlSerializer::WriteScalar(const char* name, const ValueType& value)
    {
        using std::to_string;
        auto indent = GetCurrentIndent();
        bool hasName = name != std::string("");
        auto endOfLine = hasName ? "\n" : "";

        auto typeName = TypeName<ValueType>::GetName();

        _out << indent;
        _out << "<" << typeName;

        if (name != std::string(""))
        {
            _out << " name='" << name << "'";
        }

        _out << " value='" << to_string(value) << "'/>" << endOfLine;
    }

    inline std::string XmlEncodeString(std::string s)
    {
        return s;
    }

    // This function is inline just so it appears next to the other Write* functions
    inline void SimpleXmlSerializer::WriteScalar(const char* name, const char* value)
    {
        auto indent = GetCurrentIndent();
        bool hasName = name != std::string("");
        auto endOfLine = hasName ? "\n" : "";
        auto typeName = "string";

        _out << indent;
        _out << "<" << typeName;

        if (name != std::string(""))
        {
            _out << " name='" << name << "'";
        }
        _out << " value='" << XmlEncodeString(value) << "'/>" << endOfLine;
    }

    inline void SimpleXmlSerializer::WriteScalar(const char* name, const std::string& value)
    {
        auto indent = GetCurrentIndent();
        bool hasName = name != std::string("");
        auto endOfLine = hasName ? "\n" : "";
        auto typeName = "string";

        _out << indent;
        _out << "<" << typeName;

        if (name != std::string(""))
        {
            _out << " name='" << name << "'";
        }
        _out << " value='" << XmlEncodeString(value) << "'/>" << endOfLine;
    }

    template <typename ValueType, IsFundamental<ValueType> concept>
    void SimpleXmlSerializer::WriteArray(const char* name, const std::vector<ValueType>& array)
    {
        bool hasName = name != std::string("");
        auto indent = GetCurrentIndent();
        auto endOfLine = "\n";
        auto size = array.size();
        auto typeName = TypeName<ValueType>::GetName();

        _out << indent;
        _out << "<Array";
        if (hasName)
        {
            _out << " name='" << name << "'";
        }
        _out << " type='" << typeName <<  "'>" << std::endl;
        ++_indent;
        for (const auto& item : array)
        {
            Serialize(item);
            _out << " "; // ???
        }
        --_indent;
        _out << std::endl;
        _out << indent;
        _out << "</Array>" << std::endl;
    }

    //
    // Deserialization
    //

    // TODO: make simple simple tokenizer that splits based on simple patterns
    // split on: whitepace, comma, colon, square brackets, curly brackets, quotes

    template <typename ValueType, IsFundamental<ValueType> concept>
    void SimpleXmlDeserializer::ReadScalar(const char* name, ValueType& value)
    {
        bool hasName = name != std::string("");
        if(hasName)
        {
            MatchNextToken(name);
            MatchNextToken(":");
        }

        // read string
        auto valueToken = ReadNextToken();
        std::stringstream valueStream(valueToken);
        valueStream >> value;
    }

    // This function is inline just so it appears next to the other Read* functions
    inline void SimpleXmlDeserializer::ReadScalar(const char* name, std::string& value) 
    {
        bool hasName = name != std::string("");
        if(hasName)
        {
            MatchNextToken(name);
            MatchNextToken(":");
        }

        MatchNextToken("\"");
        auto valueToken = ReadNextToken();
        value = valueToken;
        MatchNextToken("\"");
    }

    template <typename ValueType, IsFundamental<ValueType> concept>
    void SimpleXmlDeserializer::ReadArray(const char* name, std::vector<ValueType>& array)
    {
        bool hasName = name != std::string("");
        if(hasName)
        {
            MatchNextToken(name);
            MatchNextToken(":");
        }
                
        MatchNextToken("[");
        std::string nextToken = "";
        while(nextToken != "]")
        {
            ValueType obj;
            Deserialize(obj);
            array.push_back(obj);

            MatchNextToken(",");

            // Want to peek at the next token here to see if it's a ']'
            nextToken = PeekNextToken();
        }
        MatchNextToken("]");
    }
}
