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
    template<typename ValueType>
    void XMLSerializer::WriteSingleLineTags(const std::string& tagName, const ValueType& value)
    {
        auto typeName = TypeName<ValueType>::GetName();
        Indent();
        PrintFormat(_stream, formatOneLine0, typeName, value, typeName);
    }

    template<typename ValueType>
    void XMLSerializer::WriteSingleLineTags(const std::string& tagName, const std::string& name, const ValueType& value)
    {
        auto typeName = TypeName<ValueType>::GetName();
        Indent();
        PrintFormat(_stream, formatOneLine1, typeName, "name", name, value, typeName);
    }

    inline void XMLSerializer::WriteOpenTag(const std::string& tagName)
    {
        Indent();
        PrintFormat(_stream, formatOpenTag0, tagName);
        ++_indentation;
    }

    template <typename ValueType>
    void XMLSerializer::WriteOpenTag(const std::string& tagName, const std::string& attributeName, const ValueType& attributeValue)
    {
        Indent();
        PrintFormat(_stream, formatOpenTag1, tagName, attributeName, attributeValue);
        ++_indentation;
    }

    template <typename ValueType1, typename ValueType2>
    void XMLSerializer::WriteOpenTag(const std::string& tagName, const std::string& attributeName1, const ValueType1& attributeValue1, const std::string& attributeName2, const ValueType2& attributeValue2)
    {
        Indent();
        PrintFormat(_stream, formatOpenTag2, tagName, attributeName1, attributeValue1, attributeName2, attributeValue2);
        ++_indentation;
    }

    inline void XMLSerializer::WriteCloseTag(const std::string& tagName)
    {
        --_indentation;
        Indent();
        PrintFormat(_stream, formatCloseTag, tagName);
    }

    // serialize fundamental types
    template<typename ValueType>
    void XMLSerializer::Serialize(const char* name, const ValueType& value, typename std::enable_if_t<std::is_fundamental<ValueType>::value>* concept)
    {
        auto typeName = TypeName<ValueType>::GetName();
        WriteSingleLineTags(typeName, name, value);
    }

    // serialize std::vector
    template<typename ElementType>
    void XMLSerializer::Serialize(const char* name, const std::vector<ElementType>& value)
    {
        auto size = value.size();
        auto typeName = TypeName<std::vector<ElementType>>::GetName();

        WriteOpenTag(typeName, "name", name, "size", size);
        for (uint64 i = 0; i < value.size(); ++i)
        {
            SerializeUnnamed(value[i]);
        }
        WriteCloseTag(typeName);
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

        WriteOpenTag(typeName, "name", name);
        WriteOpenTag(runtimeTypeName);
        value->Write(*this);
        WriteCloseTag(runtimeTypeName);
        WriteCloseTag(typeName);
    }

    // serialize classes
    template<typename ValueType>
    void XMLSerializer::Serialize(const char* name, const ValueType& value, typename std::enable_if_t<std::is_class<ValueType>::value>* concept)
    {
        auto typeName = ValueType::GetTypeName();

        WriteOpenTag(typeName, "name", name);
        value.Write(*this);
        WriteCloseTag(typeName);
    }

    // serialize fundamental types
    template<typename ValueType>
    void XMLSerializer::SerializeUnnamed(const ValueType& value, typename std::enable_if_t<std::is_fundamental<ValueType>::value>* concept)
    {
        auto typeName = TypeName<ValueType>::GetName();
        WriteSingleLineTags(typeName, value);
    }

    // serialize std::vector
    template<typename ElementType>
    void XMLSerializer::SerializeUnnamed(const std::vector<ElementType>& value)
    {
        auto size = value.size();
        auto typeName = TypeName<std::vector<ElementType>>::GetName();

        WriteOpenTag(typeName, "size", size);
        for (uint64 i = 0; i < size; ++i)
        {
            SerializeUnnamed(value[i]);
        }
        WriteCloseTag(typeName);
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

        WriteOpenTag(typeName);
        WriteOpenTag(runtimeTypeName);
        value->Write(*this);
        WriteCloseTag(runtimeTypeName);
        WriteCloseTag(typeName);
    }

    // serialize classes
    template<typename ValueType>
    void XMLSerializer::SerializeUnnamed(const ValueType& value, typename std::enable_if_t<std::is_class<ValueType>::value>* concept)
    {
        auto typeName = ValueType::GetTypeName();

        WriteOpenTag(typeName);
        value.Write(*this);
        WriteCloseTag(typeName);
    }

    //
    // XMLDeserializer
    //

    template <typename TagType>
    void XMLDeserializer::ReadOpenTag(TagType&& tagName)
    {
        MatchFormatThrowsExceptions(_pStr, formatOpenTag0, tagName);
    }

    template <typename TagType, typename NameType, typename ValueType>
    void XMLDeserializer::ReadOpenTag(TagType&& tagName, NameType&& attributeName, ValueType&& attributeValue)
    {
        MatchFormatThrowsExceptions(_pStr, formatOpenTag1, tagName, attributeName, attributeValue);
    }

    template <typename TagType, typename NameType1, typename ValueType1, typename NameType2, typename ValueType2>
    void XMLDeserializer::ReadOpenTag(TagType&& tagName, NameType1&& attributeName1, ValueType1&& attributeValue1, NameType2&& attributeName2, ValueType2&& attributeValue2)
    {
        MatchFormatThrowsExceptions(_pStr, formatOpenTag2, tagName, attributeName1, attributeValue1, attributeName2, attributeValue2);
    }

    template <typename TagType>
    void XMLDeserializer::ReadCloseTag(TagType&& tagName)
    {
        MatchFormatThrowsExceptions(_pStr, formatCloseTag, Match(tagName));
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
        ReadOpenTag(Match(typeName), Match("name"), Match(name), Match("size"), size);

        value.resize(size);

        for (uint64 i = 0; i < value.size(); ++i)
        {
            DeserializeUnnamed(value[i]);
        }
        ReadCloseTag(Match(typeName));
    }

    // deserialize pointers to polymorphic classes
    template<typename ValueType>
    void XMLDeserializer::Deserialize(const char* name, std::unique_ptr<ValueType>& value)
    {
        static_assert(std::is_polymorphic<ValueType>::value, "can only serialize unique_ptr to polymorphic classes");

        auto typeName = TypeName<std::unique_ptr<ValueType>>::GetName();
        std::string runtimeTypeName;

        ReadOpenTag(Match(typeName), Match("name"), Match(name));
        ReadOpenTag(runtimeTypeName);

        Read(runtimeTypeName, value); 
        value->Read(*this);

        ReadCloseTag(Match(runtimeTypeName));
        ReadCloseTag(Match(typeName));
    }

    // deserialize classes
    template<typename ValueType>
    void XMLDeserializer::Deserialize(const char* name, ValueType& value, typename std::enable_if_t<std::is_class<ValueType>::value>* concept)
    {
        auto typeName = ValueType::GetTypeName();
        ReadOpenTag(Match(typeName), Match("name"), Match(name));

        value.Read(*this);

        ReadCloseTag(Match(typeName));
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
        ReadOpenTag(Match(typeName), Match("size"), size);

        value.resize(size);

        for (uint64 i = 0; i < value.size(); ++i)
        {
            DeserializeUnnamed(value[i]);
        }

        ReadCloseTag(Match(typeName));
    }

    // deserialize pointers to polymorphic classes
    template<typename ValueType>
    void XMLDeserializer::DeserializeUnnamed(std::unique_ptr<ValueType>& value)
    {
        static_assert(std::is_polymorphic<ValueType>::value, "can only serialize unique_ptr to polymorphic classes");

        auto typeName = TypeName<std::unique_ptr<ValueType>>::GetName();
        std::string runtimeTypeName;

        ReadOpenTag(Match(typeName));
        ReadOpenTag(runtimeTypeName); 

        Read(runtimeTypeName, value); 
        value->Read(*this);

        ReadCloseTag(Match(runtimeTypeName));
        ReadCloseTag(Match(typeName));
    }

    // deserialize classes
    template<typename ValueType>
    void XMLDeserializer::DeserializeUnnamed(ValueType& value, typename std::enable_if_t<std::is_class<ValueType>::value>* concept)
    {
        auto typeName = ValueType::GetTypeName();
        ReadOpenTag(Match(typeName));

        value.Read(*this);

        ReadCloseTag(Match(typeName));
    }
}