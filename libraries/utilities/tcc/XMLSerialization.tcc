////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     XMLSerialization.tcc (utilities)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <type_traits>

namespace
{
    const char* typeFormatBegin = "<^% name^=^\"%\"^>\n";
    const char* typeFormatEnd = "<^/%^>\n";
}

namespace utilities
{
    template<typename Type>
    inline void XMLSerializer::Serialize(const char* name, const Type& value)
    {
        auto className = value.GetSerializationName();
        Indent();
        
        Format::Printf(_stream, typeFormatBegin, className, name);
        ++_indentation;

        value.Write(*this);

        --_indentation;
        Indent();
        Format::Printf(_stream, typeFormatEnd, className);
    }

    template<typename Type>
    inline void XMLDeserializer::Deserialize(const char* name, Type& value)
    {

        auto className = value.GetSerializationName();
        Format::MatchScanf(_pStr, typeFormatBegin, Format::Match(className), Format::Match(name));

        value.Read(*this);

        Format::MatchScanf(_pStr, typeFormatEnd, Format::Match(className));
    }
}