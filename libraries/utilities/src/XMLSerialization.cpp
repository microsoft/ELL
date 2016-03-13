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
    XMLDeserializer::XMLDeserializer(std::istream& stream)
    {
        std::stringstream stringStream;
        stringStream << stream.rdbuf();
        _string = std::move(stringStream.str());
        _pStr = _string.c_str();
    }

    XMLSerializer::XMLSerializer(std::ostream & stream) : _stream(stream)
    {}

    void XMLSerializer::Indent()
    {
        for (uint64 i = 0; i < _indentation; ++i)
        {
            _stream << "    ";
        }
    }
}
