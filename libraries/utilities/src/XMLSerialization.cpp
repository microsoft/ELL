////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     XMLSerialization.cpp (utilities)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "XMLSerialization.h"
#include "Format.h"

namespace utilities
{
    XMLDeserializer::XMLDeserializer(std::istream& is)
    {
        _stream << is.rdbuf();
        _pStr = _stream.str().c_str();
    }

    // int

    const char* intFormat = "<^Int name^=^\"%\"^> % <^/Int^>\n";

    void XMLSerializer::Serialize(const char * name, const int & value)
    {
        Indent();
        Format::Printf(_stream, intFormat, name, value);
    }

    void XMLDeserializer::Deserialize(const char * name, int & value)
    {
        Format::MatchScanf(_pStr, intFormat, Format::Match(name), value);
    }

    // uint64

    const char* uint64Format = "<^UInt64 name^=^\"%\"^> % <^/UInt64^>\n";

    void utilities::XMLSerializer::Serialize(const char * name, const uint64 & value)
    {
        Indent();
        Format::Printf(_stream, uint64Format, name, value);
    }

    void utilities::XMLDeserializer::Deserialize(const char * name, uint64 & value)
    {
        Format::MatchScanf(_pStr, uint64Format, Format::Match(name), value);
    }

    // double

    const char* doubleFormat = "<^Double name^=^\"%\"^> % <^/Double^>\n";

    void XMLSerializer::Serialize(const char* name, const double& value)
    {
        Indent();
        Format::Printf(_stream, doubleFormat, name, value);
    }

    void XMLDeserializer::Deserialize(const char * name, double& value)
    {
        Format::MatchScanf(_pStr, doubleFormat, Format::Match(name), value);
    }

    void XMLSerializer::WriteToStream(std::ostream& os) const
    {
        os << _stream.str();
    }

    void XMLSerializer::Indent()
    {
        for (uint64 i = 0; i < _indentation; ++i)
        {
            _stream << "    ";
        }
    }

}
