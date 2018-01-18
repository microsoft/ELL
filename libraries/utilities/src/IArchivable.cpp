////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IArchivable.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IArchivable.h"
#include "Archiver.h"
#include "ObjectArchiver.h"

namespace ell
{
namespace utilities
{
    ObjectArchive GetDescription(const IArchivable& object)
    {
        utilities::SerializationContext context;
        utilities::ObjectArchiver archiver(context);
        archiver.Archive(object);
        return archiver.GetObjectArchive();
    }

    bool IArchivable::CanReadArchiveVersion(const ArchiveVersion& version) const
    {
        return version == GetArchiveVersion();
    }
}
}
