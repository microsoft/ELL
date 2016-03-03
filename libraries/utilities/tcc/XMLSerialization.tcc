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
    const char* typeFormatOneLine = "<^% name^=^\"%\"^> % <^/%^>\n";

    const char* typeFormatBegin = "<^% name^=^\"%\"^>\n";
    const char* typeFormatEnd = "<^/%^>\n";

    const char* pointerFormatBegin = "<^* type=\"%\" name^=^\"%\"^>\n";
    const char* pointerFormatEnd = "<^/*^>\n";

    const char* vectorFormatBegin = "<^Vector name^=^\"%\" type=\"%\" size=\"%\"^>\n";
    const char* vectorFormatEnd = "<^/Vector^>\n";
}

namespace utilities
{
    template<typename ValueType>
    void XMLSerializer::Serialize(const char* name, const ValueType& value, typename std::enable_if_t<std::is_fundamental<ValueType>::value>* concept)
    {
        auto className = TypeInfo<ValueType>::GetSerializationName();
        Indent();
        Format::Printf(_stream, typeFormatOneLine, className, name, value, className);
    }

    template<typename ElementType>
    void XMLSerializer::Serialize(const char* name, const std::vector<ElementType>& value)
    {
        Indent();
        uint64 size = value.size();
        auto typeName = TypeInfo<ElementType>::GetSerializationName();
        Format::Printf(_stream, vectorFormatBegin, name, typeName, size);

        ++_indentation;

        for (uint64 i = 0; i < value.size(); ++i)
        {
            Serialize("", value[i]);
        }

        --_indentation;
        Indent();
        Format::Printf(_stream, vectorFormatEnd);
    }

    template<typename ValueType>
    void XMLSerializer::Serialize(const char* name, const ValueType& value, typename std::enable_if_t<std::is_class<ValueType>::value>* concept)
    {
        auto className = TypeInfo<ValueType>::GetSerializationName();

        Indent();
        Format::Printf(_stream, typeFormatBegin, className.c_str(), name);

        ++_indentation;

        value.Write(*this);

        --_indentation;
        Indent();
        Format::Printf(_stream, typeFormatEnd, className);
    }

    template<typename ValueType>
    void XMLSerializer::Serialize(const char* name, const std::shared_ptr<ValueType>& spValue)
    {
        auto className = TypeInfo<ValueType>::GetSerializationName();
        Indent();

        Format::Printf(_stream, pointerFormatBegin, className, name);

        ++_indentation;

        Serialize("", *spValue);

        --_indentation;
        Indent();
        Format::Printf(_stream, pointerFormatEnd, className);
    }

    template<typename ValueType>
    void XMLDeserializer::Deserialize(const char* name, ValueType& value, typename std::enable_if_t<std::is_fundamental<ValueType>::value>* concept)
    {
        auto className = TypeInfo<ValueType>::GetSerializationName();
        Format::MatchScanfThrowsExceptions(_pStr, typeFormatOneLine, Format::Match(className.c_str()), Format::Match(name), value, Format::Match(className.c_str()));
    }

    template<typename ElementType>
    void XMLDeserializer::Deserialize(const char* name, std::vector<ElementType>& value)
    {
        value.clear();

        uint64 size = 0;
        auto typeName = TypeInfo<ElementType>::GetSerializationName();
        Format::MatchScanfThrowsExceptions(_pStr, vectorFormatBegin, Format::Match(name), Format::Match(typeName.c_str()), size);
        value.resize(size);

        for (uint64 i = 0; i < value.size(); ++i)
        {
            Deserialize("", value[i]);
        }

        Format::MatchScanfThrowsExceptions(_pStr, vectorFormatEnd);
    }

    template<typename ValueType>
    void XMLDeserializer::Deserialize(const char* name, ValueType& value, typename std::enable_if_t<std::is_class<ValueType>::value>* concept)
    {
        auto className = TypeInfo<ValueType>::GetSerializationName();
        Format::MatchScanfThrowsExceptions(_pStr, typeFormatBegin, Format::Match(className.c_str()), Format::Match(name));

        value.Read(*this);

        Format::MatchScanfThrowsExceptions(_pStr, typeFormatEnd, Format::Match(className.c_str()));
    }

    template<typename ValueType>
    void XMLDeserializer::Deserialize(const char* name, std::shared_ptr<ValueType>& spValue)
    {
        auto className = TypeInfo<ValueType>::GetSerializationName();
        Format::MatchScanfThrowsExceptions(_pStr, pointerFormatBegin, Format::Match(className.c_str()), Format::Match(name));
        spValue = std::make_shared<ValueType>();
        Deserialize("", *spValue);
        Format::MatchScanfThrowsExceptions(_pStr, pointerFormatEnd, Format::Match(className.c_str()));
    }
}