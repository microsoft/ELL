////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     XMLSerialization.tcc (utilities)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TypeInfo.h"

namespace
{
    // format strings

    const char* typeFormatOneLine = "<^% name^=^\"%\"^>^%^<^/%^>\n";
    const char* unnamedTypeFormatOneLine = "<^%^>^%^<^/%^>\n";

    const char* typeFormatBegin = "<^% name^=^\"%\"^>\n";
    const char* unnamedTypeFormatBegin = "<^%^>\n";
    const char* typeFormatEnd = "<^/%^>\n";

    const char* vectorFormatBegin = "<^% name^=^\"%\" size^=^\"%\"^>\n";
    const char* unnamedVectorFormatBegin = "<^% size^=^\"%\"^>\n";
}

namespace utilities
{
    template<typename ValueType>
    void XMLSerializer::Serialize(const char* name, const ValueType& value, typename std::enable_if_t<std::is_fundamental<ValueType>::value>* concept)
    {
        auto typeName = TypeInfo<ValueType>::GetSerializationName();
        Indent();
        if (*name != '\0')
        {
            Format::Printf(_stream, typeFormatOneLine, typeName, name, value, typeName);
        }
        else
        {
            Format::Printf(_stream, unnamedTypeFormatOneLine, typeName, value, typeName);
        }
    }

    template<typename ElementType>
    void XMLSerializer::Serialize(const char* name, const std::vector<ElementType>& value)
    {
        Indent();
        auto size = value.size();
        auto typeName = TypeInfo<std::vector<ElementType>>::GetSerializationName();

        if (*name != '\0')
        {
            Format::Printf(_stream, vectorFormatBegin, typeName, name, size);
        }
        else
        {
            Format::Printf(_stream, unnamedVectorFormatBegin, typeName, size);
        }

        ++_indentation;
        for (uint64 i = 0; i < value.size(); ++i)
        {
            Serialize("", value[i]);
        }
        --_indentation;

        Indent();
        Format::Printf(_stream, typeFormatEnd, typeName);
    }

    template<typename ValueType>
    void XMLSerializer::Serialize(const char* name, const ValueType& value, typename std::enable_if_t<std::is_class<ValueType>::value>* concept)
    {
        auto typeName = TypeInfo<ValueType>::GetSerializationName();

        Indent();
        if (*name != '\0')
        {
            Format::Printf(_stream, typeFormatBegin, typeName, name);
        }
        else
        {
            Format::Printf(_stream, unnamedTypeFormatBegin, typeName);
        }

        ++_indentation;
        value.Write(*this);
        --_indentation;

        Indent();
        Format::Printf(_stream, typeFormatEnd, typeName);
    }

    template<typename ValueType>
    void XMLSerializer::Serialize(const char* name, const std::shared_ptr<ValueType>& spValue)
    {
        auto typeName = TypeInfo<std::shared_ptr<ValueType>>::GetSerializationName();
        Indent();

        if (*name != '\0')
        {
            Format::Printf(_stream, typeFormatBegin, typeName, name);
        }
        else
        {
            Format::Printf(_stream, unnamedTypeFormatBegin, typeName);
        }

        ++_indentation;
        Serialize("", *spValue);
        --_indentation;

        Indent();
        Format::Printf(_stream, typeFormatEnd, typeName);
    }

    template<typename ValueType>
    void XMLDeserializer::Deserialize(const char* name, ValueType& value, typename std::enable_if_t<std::is_fundamental<ValueType>::value>* concept)
    {
        auto typeName = TypeInfo<ValueType>::GetSerializationName();
        if (*name != '\0')
        {
            Format::MatchScanfThrowsExceptions(_pStr, typeFormatOneLine, Format::Match(typeName), Format::Match(name), value, Format::Match(typeName));
        }
        else
        {
            Format::MatchScanfThrowsExceptions(_pStr, unnamedTypeFormatOneLine, Format::Match(typeName), value, Format::Match(typeName));
        }
    }

    template<typename ElementType>
    void XMLDeserializer::Deserialize(const char* name, std::vector<ElementType>& value)
    {
        value.clear();

        uint64 size = 0;
        auto typeName = TypeInfo<std::vector<ElementType>>::GetSerializationName();
        if (*name != '\0')
        {
            Format::MatchScanfThrowsExceptions(_pStr, vectorFormatBegin, Format::Match(typeName), Format::Match(name), size);
        }
        else
        {
            Format::MatchScanfThrowsExceptions(_pStr, unnamedVectorFormatBegin, Format::Match(typeName), size);
        }

        value.resize(size);

        for (uint64 i = 0; i < value.size(); ++i)
        {
            Deserialize("", value[i]);
        }

        Format::MatchScanfThrowsExceptions(_pStr, typeFormatEnd, Format::Match(typeName));
    }

    template<typename ValueType>
    void XMLDeserializer::Deserialize(const char* name, ValueType& value, typename std::enable_if_t<std::is_class<ValueType>::value>* concept)
    {
        auto typeName = TypeInfo<ValueType>::GetSerializationName();
        if (*name != '\0')
        {
            Format::MatchScanfThrowsExceptions(_pStr, typeFormatBegin, Format::Match(typeName), Format::Match(name));
        }
        else
        {
            Format::MatchScanfThrowsExceptions(_pStr, unnamedTypeFormatBegin, Format::Match(typeName));
        }

        value.Read(*this);

        Format::MatchScanfThrowsExceptions(_pStr, typeFormatEnd, Format::Match(typeName));
    }

    template<typename ValueType>
    void XMLDeserializer::Deserialize(const char* name, std::shared_ptr<ValueType>& spValue)
    {
        auto typeName = TypeInfo<std::shared_ptr<ValueType>>::GetSerializationName();
        
        if (*name != '\0')
        {
            Format::MatchScanfThrowsExceptions(_pStr, typeFormatBegin, Format::Match(typeName), Format::Match(name));
        }
        else
        {
            Format::MatchScanfThrowsExceptions(_pStr, unnamedTypeFormatBegin, Format::Match(typeName));
        }

        spValue = std::make_shared<ValueType>();
        Deserialize("", *spValue);
        Format::MatchScanfThrowsExceptions(_pStr, typeFormatEnd, Format::Match(typeName));
    }
}