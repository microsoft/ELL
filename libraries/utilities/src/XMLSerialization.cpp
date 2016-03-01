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

namespace utilities
{
    void XMLSerializer::Serialize(const char * name, const double & value)
    {
    }

    void XMLSerializer::WriteToStream(std::ostream & os) const
    {
    }

    XMLDeserializer::XMLDeserializer(std::istream & is)
    {
    }

    void XMLDeserializer::Deserialize(const char * name, double & value)
    {
    }
}
