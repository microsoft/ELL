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

    const char* formatOpenTag0 = "<^%^>\n";
    const char* formatOpenTag1 = "<^% %^=^\"%\"^>\n";
    const char* formatOpenTag2 = "<^% %^=^\"%\" %^=^\"%\"^>\n";

    const char* formatCloseTag = "<^/%^>\n";
}

namespace utilities
{
    // serialize fundamental types
    template<typename ValueType>
    void XMLSerializer::Serialize(const char* name, const ValueType& value, typename std::enable_if_t<std::is_fundamental<ValueType>::value>* concept)
    {
        auto typeName = TypeName<ValueType>::GetName();
        Indent();
        PrintFormat(_stream, formatOneLine1, typeName, "name", name, value, typeName);
    }

    // serialize std::vector
    template<typename ElementType>
    void XMLSerializer::Serialize(const char* name, const std::vector<ElementType>& value)
    {
        Indent();
        auto size = value.size();
        auto typeName = TypeName<std::vector<ElementType>>::GetName();
        PrintFormat(_stream, formatOpenTag2, typeName, "name", name, "size", size);

        ++_indentation;
        for (uint64 i = 0; i < value.size(); ++i)
        {
            SerializeUnnamed(value[i]);
        }
        --_indentation;

        Indent();
        PrintFormat(_stream, formatCloseTag, typeName);
    }

    // serialize pointers to polymorphic classes
    template<typename ValueType>
    void XMLSerializer::Serialize(const char* name, const std::unique_ptr<ValueType>& value)
    {
        static_assert(std::is_polymorphic<ValueType>::value, "can only serialize unique_ptr to polymorphic classes");
        if (value == nullptr)
        {
            throw std::runtime_error("cannot serialize a null pointer");
        }

        auto typeName = TypeName<std::unique_ptr<ValueType>>::GetName();
        auto runtimeTypeName = value->GetRuntimeTypeName();

        Indent();
        PrintFormat(_stream, formatOpenTag1, typeName, "name", name);

        ++_indentation;
        Indent();
        PrintFormat(_stream, formatOpenTag0, runtimeTypeName);

        ++_indentation;
        value->Write(*this);

        --_indentation;
        Indent();
        PrintFormat(_stream, formatCloseTag, runtimeTypeName);

        --_indentation;
        Indent();
        PrintFormat(_stream, formatCloseTag, typeName);
    }

    // serialize classes
    template<typename ValueType>
    void XMLSerializer::Serialize(const char* name, const ValueType& value, typename std::enable_if_t<std::is_class<ValueType>::value>* concept)
    {
        auto typeName = ValueType::GetTypeName();

        Indent();
        PrintFormat(_stream, formatOpenTag1, typeName, "name", name);

        ++_indentation;
        value.Write(*this);
        --_indentation;

        Indent();
        PrintFormat(_stream, formatCloseTag, typeName);
    }

    // serialize fundamental types
    template<typename ValueType>
    void XMLSerializer::SerializeUnnamed(const ValueType& value, typename std::enable_if_t<std::is_fundamental<ValueType>::value>* concept)
    {
        auto typeName = TypeName<ValueType>::GetName();
        Indent();
        PrintFormat(_stream, formatOneLine0, typeName, value, typeName);
    }

    // serialize std::vector
    template<typename ElementType>
    void XMLSerializer::SerializeUnnamed(const std::vector<ElementType>& value)
    {
        Indent();
        auto size = value.size();
        auto typeName = TypeName<std::vector<ElementType>>::GetName();
        PrintFormat(_stream, formatOpenTag1, typeName, "size", size);

        ++_indentation;
        for (uint64 i = 0; i < value.size(); ++i)
        {
            SerializeUnnamed(value[i]);
        }
        --_indentation;

        Indent();
        PrintFormat(_stream, formatCloseTag, typeName);
    }

    // serialize pointers to polymorphic classes
    template<typename ValueType>
    void XMLSerializer::SerializeUnnamed(const std::unique_ptr<ValueType>& value)
    {
        static_assert(std::is_polymorphic<ValueType>::value, "can only serialize unique_ptr to polymorphic classes");
        if (value == nullptr)
        {
            throw std::runtime_error("cannot serialize a null pointer");
        }

        auto typeName = TypeName<std::unique_ptr<ValueType>>::GetName();
        auto runtimeTypeName = value->GetRuntimeTypeName();

        Indent();
        PrintFormat(_stream, formatOpenTag0, typeName);

        ++_indentation;
        Indent();
        PrintFormat(_stream, formatOpenTag0, runtimeTypeName);

        ++_indentation;
        value->Write(*this);

        --_indentation;
        Indent();
        PrintFormat(_stream, formatCloseTag, runtimeTypeName);

        --_indentation;
        Indent();
        PrintFormat(_stream, formatCloseTag, typeName);
    }

    // serialize classes
    template<typename ValueType>
    void XMLSerializer::SerializeUnnamed(const ValueType& value, typename std::enable_if_t<std::is_class<ValueType>::value>* concept)
    {
        auto typeName = ValueType::GetTypeName();

        Indent();
        PrintFormat(_stream, formatOpenTag0, typeName);

        ++_indentation;
        value.Write(*this);
        --_indentation;

        Indent();
        PrintFormat(_stream, formatCloseTag, typeName);
    }

    // deserialize fundamental types
    template<typename ValueType>
    void XMLDeserializer::Deserialize(const char* name, ValueType& value, typename std::enable_if_t<std::is_fundamental<ValueType>::value>* concept)
    {
        auto typeName = TypeName<ValueType>::GetName();
        MatchFormatThrowsExceptions(_pStr, formatOneLine1, Match(typeName), Match("name"), Match(name), value, Match(typeName));
    }

    // deserialize std::vectors
    template<typename ElementType>
    void XMLDeserializer::Deserialize(const char* name, std::vector<ElementType>& value)
    {
        value.clear();

        uint64 size = 0;
        auto typeName = TypeName<std::vector<ElementType>>::GetName();
        MatchFormatThrowsExceptions(_pStr, formatOpenTag2, Match(typeName), Match("name"), Match(name), Match("size"), size);

        value.resize(size);

        for (uint64 i = 0; i < value.size(); ++i)
        {
            DeserializeUnnamed(value[i]);
        }

        MatchFormatThrowsExceptions(_pStr, formatCloseTag, Match(typeName));
    }

    // deserialize pointers to polymorphic classes
    template<typename ValueType>
    void XMLDeserializer::Deserialize(const char* name, std::unique_ptr<ValueType>& value)
    {
        static_assert(std::is_polymorphic<ValueType>::value, "can only serialize unique_ptr to polymorphic classes");

        auto typeName = TypeName<std::unique_ptr<ValueType>>::GetName();
        std::string runtimeTypeName;

        MatchFormatThrowsExceptions(_pStr, formatOpenTag1, Match(typeName), Match("name"), Match(name));
        MatchFormatThrowsExceptions(_pStr, formatOpenTag0, runtimeTypeName);

        Read(runtimeTypeName, value);
        value->Read(*this);

        MatchFormatThrowsExceptions(_pStr, formatCloseTag, Match(runtimeTypeName));
        MatchFormatThrowsExceptions(_pStr, formatCloseTag, Match(typeName));
    }

    // deserialize classes
    template<typename ValueType>
    void XMLDeserializer::Deserialize(const char* name, ValueType& value, typename std::enable_if_t<std::is_class<ValueType>::value>* concept)
    {
        auto typeName = ValueType::GetTypeName();
        MatchFormatThrowsExceptions(_pStr, formatOpenTag1, Match(typeName), Match("name"), Match(name));

        value.Read(*this);

        MatchFormatThrowsExceptions(_pStr, formatCloseTag, Match(typeName));
    }

    // deserialize fundamental types
    template<typename ValueType>
    void XMLDeserializer::DeserializeUnnamed(ValueType& value, typename std::enable_if_t<std::is_fundamental<ValueType>::value>* concept)
    {
        auto typeName = TypeName<ValueType>::GetName();
        MatchFormatThrowsExceptions(_pStr, formatOneLine0, Match(typeName), value, Match(typeName));
    }

    // deserialize std::vectors
    template<typename ElementType>
    void XMLDeserializer::DeserializeUnnamed(std::vector<ElementType>& value)
    {
        value.clear();

        uint64 size = 0;
        auto typeName = TypeName<std::vector<ElementType>>::GetName();
        MatchFormatThrowsExceptions(_pStr, formatOpenTag1, Match(typeName), Match("size"), size);

        value.resize(size);

        for (uint64 i = 0; i < value.size(); ++i)
        {
            DeserializeUnnamed(value[i]);
        }

        MatchFormatThrowsExceptions(_pStr, formatCloseTag, Match(typeName));
    }

    // deserialize pointers to polymorphic classes
    template<typename ValueType>
    void XMLDeserializer::DeserializeUnnamed(std::unique_ptr<ValueType>& value)
    {
        static_assert(std::is_polymorphic<ValueType>::value, "can only serialize unique_ptr to polymorphic classes");

        auto typeName = TypeName<std::unique_ptr<ValueType>>::GetName();
        std::string runtimeTypeName;

        MatchFormatThrowsExceptions(_pStr, formatOpenTag0, Match(typeName));
        MatchFormatThrowsExceptions(_pStr, formatOpenTag0, runtimeTypeName);

        Read(runtimeTypeName, value);
        value->Read(*this);

        MatchFormatThrowsExceptions(_pStr, formatCloseTag, Match(runtimeTypeName));
        MatchFormatThrowsExceptions(_pStr, formatCloseTag, Match(typeName));
    }

    // deserialize classes
    template<typename ValueType>
    void XMLDeserializer::DeserializeUnnamed(ValueType& value, typename std::enable_if_t<std::is_class<ValueType>::value>* concept)
    {
        auto typeName = ValueType::GetTypeName();
        MatchFormatThrowsExceptions(_pStr, formatOpenTag0, Match(typeName));

        value.Read(*this);

        MatchFormatThrowsExceptions(_pStr, formatCloseTag, Match(typeName));
    }
}