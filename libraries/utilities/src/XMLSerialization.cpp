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
        std::stringstream stream;
        stream << is.rdbuf();
        _string = stream.str();
        _pStr = _string.c_str();
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
