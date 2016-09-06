////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ISerializable.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TypeName.h"
#include "Archiver.h"
#include "Exception.h"

// stl
#include <string>
#include <ostream>

namespace utilities
{
    /// <summary> ISerializable interface </summary>
    class ISerializable
    {
    public:
        virtual ~ISerializable() = default;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const { return "ISerializable"; }

        /// <summary> Serializes the object. </summary>
        ///
        /// <param name="archiver">  The archiver. </param>
        virtual void Serialize(Archiver& archiver) const = 0;

        /// <summary> Deserializes the object. </summary>
        ///
        /// <param name="archiver"> The unarchiver. </param>
        virtual void Deserialize(Unarchiver& archiver) = 0;

        /// <summary> Gets the name of this type. </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "ISerializable"; }
    };
}
