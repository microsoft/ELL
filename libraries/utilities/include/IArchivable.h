////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IArchivable.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Archiver.h"
#include "Exception.h"
#include "ObjectArchive.h"
#include "TypeName.h"

// stl
#include <ostream>
#include <string>

namespace ell
{
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
        /// <param name="archive"> The `ObjectArchive` to get the object from </param>
        /// <returns> The new object </returns>
        template <typename ValueType>
        static ValueType CreateObject(const ObjectArchive& archive);

        /// <summary> Gets the name of this type. </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "IArchivable"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const { return GetTypeName(); }

        virtual bool ArchiveAsPrimitive() const { return false; }

    protected:
        friend class Archiver;
        friend class Unarchiver;

        /// <summary> Gets the archive version of the object. </summary>
        ///
        /// <returns> The archive version of the object. </summary>
        virtual ArchiveVersion GetArchiveVersion() const { return { 0 }; }

        /// <summary> Writes the object to an archiver. </summary>
        ///
        /// <param name="archiver"> The archiver. </param>
        virtual void WriteToArchive(Archiver& archiver) const = 0;

        /// <summary> Reads the object from an unarchiver. </summary>
        ///
        /// <param name="archiver"> The unarchiver. </param>
        virtual void ReadFromArchive(Unarchiver& archiver) = 0;
    };

    class ArchivedAsPrimitive : public IArchivable
    {
        virtual bool ArchiveAsPrimitive() const { return true; }
    };
}
}

#include "../tcc/IArchivable.tcc"
