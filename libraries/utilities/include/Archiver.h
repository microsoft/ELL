////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Archiver.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Serializer.h"

namespace utilities
{
    /// <summary> Archiver class. Superclass to Serializer and Deserializer. </summary>
    class Archiver : public Serializer, public Deserializer`
    {
    public:
        virtual ~Archiver() = default;
    };
}
