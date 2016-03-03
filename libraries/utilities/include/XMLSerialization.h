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
    class XMLSerializer
    {
    public:
        void Serialize(const char* name, const int& value);

        void Serialize(const char* name, const uint64& value);

        void Serialize(const char* name, const double& value);

        template<typename Type>
        void Serialize(const char* name, const Type& value);

        void WriteToStream(std::ostream& os) const;
        
    private:

        void Indent();

        uint64 _indentation = 0;
        std::stringstream _stream;
    };

    class XMLDeserializer
    {
    public:
        XMLDeserializer(std::istream& is);

        void Deserialize(const char* name, int& value);

        void Deserialize(const char* name, uint64& value);

        void Deserialize(const char* name, double& value);

        template<typename Type>
        void Deserialize(const char* name, Type& value);

    private:
        std::string _string;
        const char* _pStr;
    };
}

#include "../tcc/XMLSerialization.tcc"
