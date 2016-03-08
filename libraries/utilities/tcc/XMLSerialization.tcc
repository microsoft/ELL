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
#include "Format.h"

namespace
{
    // format strings

    const char* formatOneLine0 = "<^%^>^%^<^/%^>\n";
    const char* formatOneLine1 = "<^% %^=^\"%\"^>^%^<^/%^>\n";

    const char* formatBegin0 = "<^%^>\n";
    const char* formatBegin1 = "<^% %^=^\"%\"^>\n";
    const char* formatBegin2 = "<^% %^=^\"%\" %^=^\"%\"^>\n";

    const char* formatEnd = "<^/%^>\n";
}

namespace utilities
{
    template<typename ValueType>
    void XMLSerializer::Serialize(const char* name, const ValueType& value, typename std::enable_if_t<std::is_fundamental<ValueType>::value>* concept)
    {
        auto typeName = TypeName<ValueType>::GetName();
        Indent();
        if (*name != '\0')
        {
            PrintFormat(_stream, formatOneLine1, typeName, "name", name, value, typeName);
        }
        else
        {
            PrintFormat(_stream, formatOneLine0, typeName, value, typeName);
        }
    }

    template<typename ElementType>
    void XMLSerializer::Serialize(const char* name, const std::vector<ElementType>& value)
    {
        Indent();
        auto size = value.size();
        auto typeName = TypeName<std::vector<ElementType>>::GetName();

        if (*name != '\0')
        {
            PrintFormat(_stream, formatBegin2, typeName, "name", name, "size", size);
        }
        else
        {
            PrintFormat(_stream, formatBegin1, typeName, "size", size);
        }

        ++_indentation;
        for (uint64 i = 0; i < value.size(); ++i)
        {
            Serialize("", value[i]);
        }
        --_indentation;

        Indent();
        PrintFormat(_stream, formatEnd, typeName);
    }

    template<typename ValueType>
    void XMLSerializer::Serialize(const char* name, const ValueType& value, typename std::enable_if_t<std::is_class<ValueType>::value>* concept)
    {
        auto typeName = value.GetRuntimeTypeName();

        Indent();
        if (*name != '\0')
        {
            PrintFormat(_stream, formatBegin1, typeName, "name", name);
        }
        else
        {
            PrintFormat(_stream, formatBegin0, typeName);
        }

        ++_indentation;
        value.Write(*this);
        --_indentation;

        Indent();
        PrintFormat(_stream, formatEnd, typeName);
    }

    template<typename ValueType>
    void XMLSerializer::Serialize(const char* name, const std::shared_ptr<ValueType>& spValue)
    {
        static_assert(std::is_polymorphic<ValueType>::value, "can only serialize shared_ptr to polymorphic classes");
        if (spValue == nullptr)
        {
            throw std::runtime_error("cannot serialize a null pointer");
        }

        auto typeName = TypeName<std::shared_ptr<ValueType>>::GetName();
        auto derivedTypeName = spValue->GetRuntimeTypeName();

        Indent();

        if (*name != '\0')
        {
            PrintFormat(_stream, formatBegin2, typeName, "name", name, "type", derivedTypeName);
        }
        else
        {
            PrintFormat(_stream, formatBegin1, typeName, "type", derivedTypeName);
        }

        ++_indentation;
        Serialize("", *spValue);
        --_indentation;

        Indent();
        PrintFormat(_stream, formatEnd, typeName);
    }

    template<typename ValueType>
    void XMLDeserializer::Deserialize(const char* name, ValueType& value, typename std::enable_if_t<std::is_fundamental<ValueType>::value>* concept)
    {
        auto typeName = TypeName<ValueType>::GetName();
        if (*name != '\0')
        {
            MatchFormatThrowsExceptions(_pStr, formatOneLine1, Match(typeName), Match("name"), Match(name), value, Match(typeName));
        }
        else
        {
            MatchFormatThrowsExceptions(_pStr, formatOneLine0, Match(typeName), value, Match(typeName));
        }
    }

    template<typename ElementType>
    void XMLDeserializer::Deserialize(const char* name, std::vector<ElementType>& value)
    {
        value.clear();

        uint64 size = 0;
        auto typeName = TypeName<std::vector<ElementType>>::GetName();
        if (*name != '\0')
        {
            MatchFormatThrowsExceptions(_pStr, formatBegin2, Match(typeName), Match("name"), Match(name), Match("size"), size);
        }
        else
        {
            MatchFormatThrowsExceptions(_pStr, formatBegin1, Match(typeName), Match("size"), size);
        }

        value.resize(size);

        for (uint64 i = 0; i < value.size(); ++i)
        {
            Deserialize("", value[i]);
        }

        MatchFormatThrowsExceptions(_pStr, formatEnd, Match(typeName));
    }

    template<typename ValueType>
    void XMLDeserializer::Deserialize(const char* name, ValueType& value, typename std::enable_if_t<std::is_class<ValueType>::value>* concept)
    {
        auto typeName = value.GetRuntimeTypeName();
        if (*name != '\0')
        {
            MatchFormatThrowsExceptions(_pStr, formatBegin1, Match(typeName), Match("name"), Match(name));
        }
        else
        {
            MatchFormatThrowsExceptions(_pStr, formatBegin0, Match(typeName));
        }

        value.Read(*this);

        MatchFormatThrowsExceptions(_pStr, formatEnd, Match(typeName));
    }

    template<typename ValueType>
    void XMLDeserializer::Deserialize(const char* name, std::shared_ptr<ValueType>& spValue)
    {
        static_assert(std::is_polymorphic<ValueType>::value, "can only serialize shared_ptr to polymorphic classes");

        auto typeName = TypeName<std::shared_ptr<ValueType>>::GetName();
        std::string derivedTypeName; 

        if (*name != '\0')
        {
            MatchFormatThrowsExceptions(_pStr, formatBegin2, Match(typeName), Match("name"), Match(name), Match("type"), derivedTypeName);
        }
        else
        {
            MatchFormatThrowsExceptions(_pStr, formatBegin1, Match(typeName), Match("type"), derivedTypeName);
        }

        Read(derivedTypeName, spValue);
        Deserialize("", *spValue);
        MatchFormatThrowsExceptions(_pStr, formatEnd, Match(typeName));
    }
}