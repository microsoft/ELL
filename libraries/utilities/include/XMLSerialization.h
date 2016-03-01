////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     XMLSerialization.h (utilities)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "types.h"

// stl
#include <string>
#include <sstream>
#include <iostream>

namespace utilities
{
    class XMLBase
    {
    protected:
        const char* _doubleFormat = "<Double name=\"%s\"> %e </Double>";
    };

    class XMLSerializer
    {
    public:
        void Serialize(const char* name, const double& value);

        void WriteToStream(std::ostream& os) const;
        
    private:
        uint64 _indentation = 0; 
        std::stringstream _stream;
    };

    class XMLDeserializer
    {
    public:
        XMLDeserializer(std::istream& is);

        void Deserialize(const char* name, double& value);

    private:
        std::stringstream _stream;
        const char* _pStr;
    };

}