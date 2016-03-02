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
        auto className = typeid(Type).name();
        Indent();
        
        Format::Printf(_stream, typeFormatBegin, className, name);
        ++_indentation;

        value.Serialize(*this);

        --_indentation;
        Indent();
        Format::Printf(_stream, typeFormatEnd, className);
    }

    template<typename Type>
    inline void XMLDeserializer::Deserialize(const char* name, Type& value)
    {

        auto className = typeid(Type).name();
        Format::MatchScanf(_pStr, typeFormatBegin, Format::Match(className), Format::Match(name));

        value.Deserialize(*this);

        Format::MatchScanf(_pStr, typeFormatEnd, Format::Match(className));
    }
}