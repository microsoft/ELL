////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IArchivable.h (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ArchiveVersion.h"
#include "Archiver.h"
#include "Exception.h"
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

        /// <summary> Gets the name of this type. </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "IArchivable"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const { return GetTypeName(); }

        /// <summary></summary>
        virtual bool ArchiveAsPrimitive() const { return false; }

    protected:
        friend class Archiver;
        friend class Unarchiver;
        friend ObjectArchive GetDescription(const IArchivable& object);
        template <typename ValueType>
        friend ValueType CreateObject(const ObjectArchive& archive);

        /// <summary> Gets the archive version of the object. </summary>
        ///
        /// <returns> The archive version of the object. </summary>
        virtual ArchiveVersion GetArchiveVersion() const { return { 0 }; }

        /// <summary> Indicates if this object can unarchive an object with the given version number. </summary>
        ///
        /// <returns> The archive version of the object. </summary>
        virtual bool CanReadArchiveVersion(const ArchiveVersion& version) const;

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
        bool ArchiveAsPrimitive() const override { return true; }
    };
}
}
