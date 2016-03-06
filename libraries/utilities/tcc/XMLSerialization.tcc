////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     XMLSerialization.tcc (utilities)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TypeName.h"

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
        auto typeName = TypeName<ValueType>::GetSerializationName();
        Indent();
        if (*name != '\0')
        {
            PrintFormat(_stream, typeFormatOneLine, typeName, name, value, typeName);
        }
        else
        {
            PrintFormat(_stream, unnamedTypeFormatOneLine, typeName, value, typeName);
        }
    }

    template<typename ElementType>
    void XMLSerializer::Serialize(const char* name, const std::vector<ElementType>& value)
    {
        if (value.size() == 0)
        {
            throw std::runtime_error("cannot serialize a zero-size vector");
        }

        Indent();
        auto size = value.size();
        auto typeName = TypeName<std::vector<ElementType>>::GetSerializationName();

        if (*name != '\0')
        {
            PrintFormat(_stream, vectorFormatBegin, typeName, name, size);
        }
        else
        {
            PrintFormat(_stream, unnamedVectorFormatBegin, typeName, size);
        }

        ++_indentation;
        for (uint64 i = 0; i < value.size(); ++i)
        {
            Serialize("", value[i]);
        }
        --_indentation;

        Indent();
        PrintFormat(_stream, typeFormatEnd, typeName);
    }

    template<typename ValueType>
    void XMLSerializer::Serialize(const char* name, const ValueType& value, typename std::enable_if_t<std::is_class<ValueType>::value>* concept)
    {
        auto typeName = TypeName<ValueType>::GetSerializationName();

        Indent();
        if (*name != '\0')
        {
            PrintFormat(_stream, typeFormatBegin, typeName, name);
        }
        else
        {
            PrintFormat(_stream, unnamedTypeFormatBegin, typeName);
        }

        ++_indentation;
        value.Write(*this);
        --_indentation;

        Indent();
        PrintFormat(_stream, typeFormatEnd, typeName);
    }

    template<typename ValueType>
    void XMLSerializer::Serialize(const char* name, const std::shared_ptr<ValueType>& spValue)
    {
        static_assert(std::is_polymorphic<ValueType>::value, "can only serialize shared_ptr to polymorphic classes");
        if (spValue == nullptr)
        {
            throw std::runtime_error("cannot serialize a null pointer");
        }

        auto typeName = TypeName<std::shared_ptr<ValueType>>::GetSerializationName();
        Indent();

        if (*name != '\0')
        {
            PrintFormat(_stream, typeFormatBegin, typeName, name);
        }
        else
        {
            PrintFormat(_stream, unnamedTypeFormatBegin, typeName);
        }

        ++_indentation;
        Serialize("", *spValue);
        --_indentation;

        Indent();
        PrintFormat(_stream, typeFormatEnd, typeName);
    }

    template<typename ValueType>
    void XMLDeserializer::Deserialize(const char* name, ValueType& value, typename std::enable_if_t<std::is_fundamental<ValueType>::value>* concept)
    {
        auto typeName = TypeName<ValueType>::GetSerializationName();
        if (*name != '\0')
        {
            MatchFormatThrowsExceptions(_pStr, typeFormatOneLine, Match(typeName), Match(name), value, Match(typeName));
        }
        else
        {
            MatchFormatThrowsExceptions(_pStr, unnamedTypeFormatOneLine, Match(typeName), value, Match(typeName));
        }
    }

    template<typename ElementType>
    void XMLDeserializer::Deserialize(const char* name, std::vector<ElementType>& value)
    {
        value.clear();

        uint64 size = 0;
        auto typeName = TypeName<std::vector<ElementType>>::GetSerializationName();
        if (*name != '\0')
        {
            MatchFormatThrowsExceptions(_pStr, vectorFormatBegin, Match(typeName), Match(name), size);
        }
        else
        {
            MatchFormatThrowsExceptions(_pStr, unnamedVectorFormatBegin, Match(typeName), size);
        }

        value.resize(size);

        for (uint64 i = 0; i < value.size(); ++i)
        {
            Deserialize("", value[i]);
        }

        MatchFormatThrowsExceptions(_pStr, typeFormatEnd, Match(typeName));
    }

    template<typename ValueType>
    void XMLDeserializer::Deserialize(const char* name, ValueType& value, typename std::enable_if_t<std::is_class<ValueType>::value>* concept)
    {
        auto typeName = TypeName<ValueType>::GetSerializationName();
        if (*name != '\0')
        {
            MatchFormatThrowsExceptions(_pStr, typeFormatBegin, Match(typeName), Match(name));
        }
        else
        {
            MatchFormatThrowsExceptions(_pStr, unnamedTypeFormatBegin, Match(typeName));
        }

        value.Read(*this);

        MatchFormatThrowsExceptions(_pStr, typeFormatEnd, Match(typeName));
    }

    template<typename ValueType>
    void XMLDeserializer::Deserialize(const char* name, std::shared_ptr<ValueType>& spValue)
    {
        static_assert(std::is_polymorphic<ValueType>::value, "can only serialize shared_ptr to polymorphic classes");

        auto typeName = TypeName<std::shared_ptr<ValueType>>::GetSerializationName();
        
        if (*name != '\0')
        {
            MatchFormatThrowsExceptions(_pStr, typeFormatBegin, Match(typeName), Match(name));
        }
        else
        {
            MatchFormatThrowsExceptions(_pStr, unnamedTypeFormatBegin, Match(typeName));
        }

        spValue = std::make_shared<ValueType>();
        Deserialize("", *spValue);
        MatchFormatThrowsExceptions(_pStr, typeFormatEnd, Match(typeName));
    }
}