////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Archiver.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl

namespace utilities
{
    /// <summary> Archiver class. Superclass to Serializer and ObjectDescription. </summary>
    class ArchiverX
    {
    public:
        virtual ~ArchiverX() = default;

        // crud. we need to expose op>> and op<<, but they're templated. Rats.
    };
}
