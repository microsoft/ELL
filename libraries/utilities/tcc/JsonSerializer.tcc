////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     JsonSerializer.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace utilities
{
    //
    // Serialization
    //
    template <typename ValueType, IsFundamental<ValueType> concept>
    void JsonSerializer::WriteScalar(const char* name, const ValueType& value)
    {
        auto indent = GetCurrentIndent();
        bool hasName = name != std::string("");        
        auto endOfLine = hasName ? ",\n" : "";

        FinishPreviousLine();
        _out << indent;
        if (hasName)
        {
            _out  << "\"" << name << "\": ";
        }
        _out << value;
        SetEndOfLine(endOfLine);
    }

    // Specialization for bool (though perhaps this should be an overload, not a specialization)
    template <>
    inline void JsonSerializer::WriteScalar(const char* name, const bool& value)
    {
        auto indent = GetCurrentIndent();
        bool hasName = name != std::string("");        
        auto endOfLine = hasName ? ",\n" : "";

        FinishPreviousLine();
        _out << indent;
        if (hasName)
        {
            _out  << "\"" << name << "\": ";
        }
        _out << (value ? "true" : "false");
        SetEndOfLine(endOfLine);
    }

    // This function is inline just so it appears next to the other Write* functions
    inline void JsonSerializer::WriteScalar(const char* name, const char* value)
    {
        auto indent = GetCurrentIndent();
        bool hasName = name != std::string("");
        auto endOfLine = hasName ? ",\n" : "";

        FinishPreviousLine();
        _out << indent;
        if (hasName)
        {
            _out  << "\"" << name << "\": ";
        }
        _out << "\"" << JsonUtilities::EncodeString(value) << "\"";
        SetEndOfLine(endOfLine);
    }

    inline void JsonSerializer::WriteScalar(const char* name, const std::string& value)
    {
        auto indent = GetCurrentIndent();
        bool hasName = name != std::string("");
        auto endOfLine = hasName ? ",\n" : "";

        FinishPreviousLine();
        _out << indent;
        if (hasName)
        {
            _out  << "\"" << name << "\": ";
        }
        _out << "\"" << JsonUtilities::EncodeString(value) << "\"";
        SetEndOfLine(endOfLine);
    }

    template <typename ValueType>
    void JsonSerializer::WriteArray(const char* name, const std::vector<ValueType>& array)
    {
        bool hasName = name != std::string("");
        auto indent = GetCurrentIndent();
        auto endOfLine = "\n";

        FinishPreviousLine();
        _out << indent;
        if (hasName)
        {
            _out  << "\"" << name << "\": ";
        }

        _out << "[";

        // reset indent
        auto numItems = array.size();
        for(size_t index = 0; index < numItems; ++index)
        {
            Serialize(array[index]);
            if(index != numItems-1)
            {
                _out << ", ";
            }
        }
        // reset indent
        _out << "]";
        SetEndOfLine(endOfLine);
    }

    //
    // Deserialization
    //
    template <typename ValueType, IsFundamental<ValueType> concept>
    void JsonDeserializer::ReadScalar(const char* name, ValueType& value)
    {
        bool hasName = name != std::string("");
        if(hasName)
        {
            MatchKey(name);
        }

        // read string
        auto valueToken = _tokenizer.ReadNextToken();
        std::stringstream valueStream(valueToken);
        valueStream >> value;

        // eat a comma if it exists
        if(hasName)
        {
            if(_tokenizer.PeekNextToken() == ",")
            {
                _tokenizer.ReadNextToken();
            }
        }
    }

    template <>
    inline void JsonDeserializer::ReadScalar(const char* name, bool& value)
    {
        bool hasName = name != std::string("");
        if(hasName)
        {
            MatchKey(name);
        }

        // read string
        auto valueToken = _tokenizer.ReadNextToken();
        value = (valueToken == "true");

        // eat a comma if it exists
        if(hasName)
        {
            if(_tokenizer.PeekNextToken() == ",")
            {
                _tokenizer.ReadNextToken();
            }
        }
    }

    // This function is inline just so it appears next to the other Read* functions
    inline void JsonDeserializer::ReadScalar(const char* name, std::string& value) 
    {
        bool hasName = name != std::string("");
        if(hasName)
        {
            MatchKey(name);
        }

        _tokenizer.MatchToken("\"");
        auto valueToken = _tokenizer.ReadNextToken();
        value = JsonUtilities::DecodeString(valueToken);
        _tokenizer.MatchToken("\"");

        // eat a comma if it exists
        if(hasName)
        {
            if(_tokenizer.PeekNextToken() == ",")
            {
                _tokenizer.ReadNextToken();
            }
        }
    }

    template <typename ValueType, IsFundamental<ValueType> concept>
    void JsonDeserializer::ReadArray(const char* name, std::vector<ValueType>& array, SerializationContext& context)
    {
        bool hasName = name != std::string("");
        if(hasName)
        {
            MatchKey(name);
        }
                
        _tokenizer.MatchToken("[");
        while(true)
        {
            auto maybeEndArray = _tokenizer.PeekNextToken();
            if(maybeEndArray == "]")
            {
                break;
            }

            ValueType obj;
            Deserialize(obj, context);
            array.push_back(obj);

            if(_tokenizer.PeekNextToken() == ",")
            {
                _tokenizer.ReadNextToken();
            }
        }
        _tokenizer.MatchToken("]");
    }

    inline void JsonDeserializer::ReadArray(const char* name, std::vector<std::string>& array, SerializationContext& context)
    {
        bool hasName = name != std::string("");
        if(hasName)
        {
            MatchKey(name);
        }
                
        _tokenizer.MatchToken("[");
        while(true)
        {
            auto maybeEndArray = _tokenizer.PeekNextToken();
            if(maybeEndArray == "]")
            {
                break;
            }

            std::string obj;
            Deserialize(obj, context);
            array.push_back(obj);

            if(_tokenizer.PeekNextToken() == ",")
            {
                _tokenizer.ReadNextToken();
            }
        }
        _tokenizer.MatchToken("]");
    }
}
