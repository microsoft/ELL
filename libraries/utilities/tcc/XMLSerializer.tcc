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
        auto typeName = SanitizeTypeName(TypeName<ValueType>::GetName());

        _out << indent;
        _out << "<" << typeName;

        if (hasName)
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

        if (hasName)
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

        if (hasName)
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
        auto typeName = SanitizeTypeName(TypeName<ValueType>::GetName());

        _out << indent;
        _out << "<Array";
        if (hasName)
        {
            _out << " name='" << name << "'";
        }
        _out << " type='" << typeName <<  "'>" << std::endl;

        // Indent the next line (the line with the array elements), and then 
        // set the indent to 0 (so there isn't indentation going on inside the line)
        // TODO: find a more principled way to do this
        // TODO: find a way to encode the values that doesn't require putting each one in a tag
        // (say, by having them be a comma-separated list)
        ++_indent;
        _out << GetCurrentIndent();

        auto oldIndent = _indent-1;
        _indent = 0;
        for (const auto& item : array)
        {
            Serialize(item);
            _out << " ";
        }
        _indent = oldIndent;
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
        auto typeName = SanitizeTypeName(TypeName<ValueType>::GetName());
        bool hasName = name != std::string("");

        _tokenizer.MatchNextToken("<");
        _tokenizer.MatchNextToken(typeName);
        if(hasName)
        {
            _tokenizer.MatchNextToken("name");
            _tokenizer.MatchNextToken("=");
            _tokenizer.MatchNextToken("'");
            _tokenizer.MatchNextToken(name);
            _tokenizer.MatchNextToken("'");
        }
        _tokenizer.MatchNextToken("value");
        _tokenizer.MatchNextToken("=");
        _tokenizer.MatchNextToken("'");

        // read value
        auto valueToken = _tokenizer.ReadNextToken();
        std::stringstream valueStream(valueToken);
        valueStream >> value;

        _tokenizer.MatchNextToken("'");
        _tokenizer.MatchNextToken("/");
        _tokenizer.MatchNextToken(">");
    }

    // This function is inline just so it appears next to the other Read* functions
    inline void SimpleXmlDeserializer::ReadScalar(const char* name, std::string& value) 
    {
        auto typeName = "string";
        bool hasName = name != std::string("");

        _tokenizer.MatchNextToken("<");
        _tokenizer.MatchNextToken(typeName);
        if(hasName)
        {
            _tokenizer.MatchNextToken("name");
            _tokenizer.MatchNextToken("=");
            _tokenizer.MatchNextToken("'");
            _tokenizer.MatchNextToken(name);
            _tokenizer.MatchNextToken("'");
        }
        _tokenizer.MatchNextToken("value");
        _tokenizer.MatchNextToken("=");
        _tokenizer.MatchNextToken("'");

        // read value
        auto valueToken = _tokenizer.ReadNextToken();
        value = valueToken;

        _tokenizer.MatchNextToken("'");
        _tokenizer.MatchNextToken("/");
        _tokenizer.MatchNextToken(">");
    }

    template <typename ValueType, IsFundamental<ValueType> concept>
    void SimpleXmlDeserializer::ReadArray(const char* name, std::vector<ValueType>& array, SerializationContext& context)
    {
        auto typeName = SanitizeTypeName(TypeName<ValueType>::GetName());
        bool hasName = name != std::string("");

        _tokenizer.MatchNextToken("<");
        _tokenizer.MatchNextToken("Array");
        if(hasName)
        {
            _tokenizer.MatchNextToken("name");
            _tokenizer.MatchNextToken("=");
            _tokenizer.MatchNextToken("'");
            _tokenizer.MatchNextToken(name);
            _tokenizer.MatchNextToken("'");
        }
                
        _tokenizer.MatchNextToken("type");
        _tokenizer.MatchNextToken("=");
        _tokenizer.MatchNextToken("'");
        _tokenizer.MatchNextToken(typeName);
        _tokenizer.MatchNextToken("'");
        _tokenizer.MatchNextToken(">");

        std::string nextToken = "";
        while(true)
        {
            ValueType obj;
            Deserialize(obj, context);
            array.push_back(obj);
            
            // check for '</'
            auto token1 = _tokenizer.ReadNextToken();
            auto token2 = _tokenizer.ReadNextToken();
            _tokenizer.PutBackToken(token2);
            _tokenizer.PutBackToken(token1);
            if(token1+token2 == "</")
            {
                break;
            }
        }

        _tokenizer.MatchNextToken("<");
        _tokenizer.MatchNextToken("/");
        _tokenizer.MatchNextToken("Array");
        _tokenizer.MatchNextToken(">");
    }
}
