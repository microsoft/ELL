////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IArchivable.h (utilities)
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
    class ObjectArchive;

    /// <summary> IArchivable interface </summary>
    class IArchivable
    {
    public:
        virtual ~IArchivable() = default;

        /// <summary> Create an `ObjectArchive` from an object </summary>
        ///
        /// <returns> The `ObjectArchive` describing the object </returns>
        ObjectArchive GetDescription() const;

        /// <summary> Creates an object from an `ObjectArchive` </summary>
        ///
        /// <typeparam name="ValueType"> The type of the object to retrieve </typeparam>
        /// <param name="archiver"> The `Archiver` to get the object from </param>
        /// <returns> The new object </returns>
        template <typename ValueType>
        static ValueType CreateObject(const ObjectArchive& archiver);

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const { return "IArchivable"; }

        /// <summary> Serializes the object. </summary>
        ///
        /// <param name="archiver">  The archiver. </param>
        virtual void WriteToArchive(Archiver& archiver) const = 0;

        /// <summary> Deserializes the object. </summary>
        ///
        /// <param name="archiver"> The unarchiver. </param>
        virtual void ReadFromArchive(Unarchiver& archiver) = 0;

        /// <summary> Gets the name of this type. </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "IArchivable"; }
    };
}

#include "../tcc/IArchivable.tcc"

