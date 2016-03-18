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
    //
    // XMLSerializer class
    //

    // serialize fundamental types
    template<typename ValueType>
    void XMLSerializer::Serialize(const char* name, const ValueType& value, typename std::enable_if_t<std::is_fundamental<ValueType>::value>* concept)
    {
        auto typeName = TypeName<ValueType>::GetName();
        WriteSingleLineTags(typeName, name, value);
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

    // serialize std::vector
    template<typename ElementType>
    void XMLSerializer::Serialize(const char* name, const std::vector<ElementType>& value)
    {
        auto size = value.size();
        auto typeName = TypeName<std::vector<ElementType>>::GetName();

        WriteOpenTag(typeName, "name", name, "size", size);
        for(uint64 i = 0; i < size; ++i)
        {
            Serialize(value[i]);
        }
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
    void XMLSerializer::Serialize(const ValueType& value, typename std::enable_if_t<std::is_fundamental<ValueType>::value>* concept)
    {
        auto typeName = TypeName<ValueType>::GetName();
        WriteSingleLineTags(typeName, value);
    }

    // serialize pointers to polymorphic classes
    template<typename ValueType>
    void XMLSerializer::Serialize(const std::unique_ptr<ValueType>& value)
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

    // serialize std::vector
    template<typename ElementType>
    void XMLSerializer::Serialize(const std::vector<ElementType>& value)
    {
        auto size = value.size();
        auto typeName = TypeName<std::vector<ElementType>>::GetName();

        WriteOpenTag(typeName, "size", size);
        for(uint64 i = 0; i < size; ++i)
        {
            Serialize(value[i]);
        }
        WriteCloseTag(typeName);
    }

    // serialize classes
    template<typename ValueType>
    void XMLSerializer::Serialize(const ValueType& value, typename std::enable_if_t<std::is_class<ValueType>::value>* concept)
    {
        auto typeName = ValueType::GetTypeName();

        WriteOpenTag(typeName);
        value.Write(*this);
        WriteCloseTag(typeName);
    }

    template<typename ValueType>
    void XMLSerializer::WriteSingleLineTags(const std::string& tagName, const ValueType& value)
    {
        Indent();
        PrintFormat(_stream, formatOneLine0, tagName, value, tagName);
    }

    template<typename ValueType>
    void XMLSerializer::WriteSingleLineTags(const std::string& tagName, const std::string& name, const ValueType& value)
    {
        Indent();
        PrintFormat(_stream, formatOneLine1, tagName, "name", name, value, tagName);
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

    //
    // XMLDeserializer class
    //

    template<typename T>
    void XMLDeserializer::RegisterPolymorphicType()
    {
        _polymorphicTypeFactory.AddType<T>();
    }

    template<typename T>
    void XMLDeserializer::RegisterPolymorphicType(const std::string & typeName)
    {
        _polymorphicTypeFactory.AddType<T>(typeName);
    }

    // deserialize fundamental types
    template<typename ValueType>
    void XMLDeserializer::Deserialize(const char* name, ValueType& value, typename std::enable_if_t<std::is_fundamental<ValueType>::value>* concept)
    {
        auto typeName = TypeName<ValueType>::GetName();
        ReadSingleLineTags(Match(typeName), Match("name"), Match(name), value);
    }

    // deserialize pointers to polymorphic classes
    template<typename BaseType>
    void XMLDeserializer::Deserialize(const char* name, std::unique_ptr<BaseType>& value)
    {
        const auto& factory = BaseType::GetTypeFactory();
        Deserialize(name, value, factory);
    }

    // deserialize pointers to polymorphic classes
    template<typename BaseType>
    void XMLDeserializer::Deserialize(const char* name, std::unique_ptr<BaseType>& value, const TypeFactory<BaseType>& factory)
    {
        auto typeName = TypeName<std::unique_ptr<BaseType>>::GetName();
        std::string runtimeTypeName;

        ReadOpenTag(Match(typeName), Match("name"), Match(name));
        ReadOpenTag(runtimeTypeName);
        value = factory.Construct(runtimeTypeName);
        value->Read(*this);

        ReadCloseTag(Match(runtimeTypeName));
        ReadCloseTag(Match(typeName));
    }

    // deserialize vectors of pointers
    template<typename BaseType>
    void XMLDeserializer::Deserialize(const char * name, std::vector<std::unique_ptr<BaseType>>& value)
    {
        const auto& factory = BaseType::GetTypeFactory();
        Deserialize(name, value, factory);
    }

    // deserialize vectors of pointers
    template<typename BaseType>
    void XMLDeserializer::Deserialize(const char * name, std::vector<std::unique_ptr<BaseType>>& value, const TypeFactory<BaseType>& factory)
    { 
        value.clear();

        uint64 size = 0;
        auto typeName = TypeName<std::vector<std::unique_ptr<BaseType>>>::GetName();
        ReadOpenTag(Match(typeName), Match("name"), Match(name), Match("size"), size);
        value.resize(size);
        for(uint64 i = 0; i < size; ++i)
        {
            Deserialize(value[i], factory);
        }
        ReadCloseTag(Match(typeName));
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
        for(uint64 i = 0; i < size; ++i)
        {
            Deserialize(value[i]);
        }
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
    void XMLDeserializer::Deserialize(ValueType& value, typename std::enable_if_t<std::is_fundamental<ValueType>::value>* concept)
    {
        auto typeName = TypeName<ValueType>::GetName();
        ReadSingleLineTags(Match(typeName), value);
    }

    // deserialize pointers to polymorphic classes
    template<typename BaseType>
    void XMLDeserializer::Deserialize(std::unique_ptr<BaseType>& value, const TypeFactory<BaseType>& factory)
    {
        auto typeName = TypeName<std::unique_ptr<BaseType>>::GetName();
        std::string runtimeTypeName;

        ReadOpenTag(Match(typeName));
        ReadOpenTag(runtimeTypeName);
        value = factory.Construct(runtimeTypeName);
        value->Read(*this);

        ReadCloseTag(Match(runtimeTypeName));
        ReadCloseTag(Match(typeName));
    }

    // deserialize vectors of pointers
    template<typename BaseType>
    void XMLDeserializer::Deserialize(std::vector<std::unique_ptr<BaseType>>& value, const TypeFactory<BaseType>& factory)
    {
        value.clear();

        uint64 size = 0;
        auto typeName = TypeName<std::vector<std::unique_ptr<BaseType>>>::GetName();
        ReadOpenTag(Match(typeName), Match("name"), Match(name), Match("size"), size);
        value.resize(size);
        for(uint64 i = 0; i < size; ++i)
        {
            Deserialize(value[i], factory);
        }
        ReadCloseTag(Match(typeName));
    }

    // deserialize std::vectors
    template<typename ElementType>
    void XMLDeserializer::Deserialize(std::vector<ElementType>& value)
    {
        value.clear();

        uint64 size = 0;
        auto typeName = TypeName<std::vector<ElementType>>::GetName();

        ReadOpenTag(Match(typeName), Match("size"), size);
        value.resize(size);
        for(uint64 i = 0; i < size; ++i)
        {
            Deserialize(value[i]);
        }
        ReadCloseTag(Match(typeName));
    }

    // deserialize classes
    template<typename ValueType>
    void XMLDeserializer::Deserialize(ValueType& value, typename std::enable_if_t<std::is_class<ValueType>::value>* concept)
    {
        auto typeName = ValueType::GetTypeName();
        ReadOpenTag(Match(typeName));
        value.Read(*this);
        ReadCloseTag(Match(typeName));
    }

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
        MatchFormatThrowsExceptions(_pStr, formatCloseTag, tagName);
    }

    template<typename TagType, typename ValueType>
    void XMLDeserializer::ReadSingleLineTags(TagType&& tagName, ValueType&& value)
    {
        MatchFormatThrowsExceptions(_pStr, formatOneLine0, tagName, value, tagName);
    }

    template<typename TagType, typename NameType, typename AttributeType, typename ValueType>
    void XMLDeserializer::ReadSingleLineTags(TagType&& tagName, NameType&& attributeName, AttributeType attributeValue, ValueType&& value)
    {
        MatchFormatThrowsExceptions(_pStr, formatOneLine1, tagName, attributeName, attributeValue, value, tagName);
    }
}