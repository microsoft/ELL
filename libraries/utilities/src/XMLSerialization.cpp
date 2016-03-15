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

    XMLSerializer::XMLSerializer(std::ostream & stream) : _stream(stream)
    {}


    void utilities::XMLSerializer::Serialize(const char* name, const std::string& value)
    {
        WriteSingleLineTags("string", name, value);
    }

    void utilities::XMLSerializer::SerializeUnnamed(const std::string& value)
    {
        WriteSingleLineTags("string", value);
    }

    void XMLSerializer::Indent()
    {
        for(uint64 i = 0; i < _indentation; ++i)
        {
            _stream << "    ";
        }
    }

    XMLDeserializer::XMLDeserializer(std::istream& stream)
    {
        std::stringstream stringStream;
        stringStream << stream.rdbuf();
        _string = std::move(stringStream.str());
        _pStr = _string.c_str();
    }

    void XMLDeserializer::Deserialize(const char * name, std::string& value)
    {
        ReadSingleLineTags(Match("string"), Match("name"), Match(name), value);
    }
    
    void XMLDeserializer::DeserializeUnnamed(std::string& value)
    {
        ReadSingleLineTags(Match("string"), value);
    }
}
