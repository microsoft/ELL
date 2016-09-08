////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IArchivable.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IArchivable.h"
#include "Archiver.h"
#include "ObjectArchiver.h"

// utiliites
#include "Exception.h"

// stl
#include <memory>

namespace utilities
{
    ObjectArchive IArchivable::GetDescription() const
    {
        utilities::SerializationContext context;
        utilities::ObjectArchiver archiver(context);
        archiver.Archive(*this);
        return archiver.GetObjectArchive();
    }
}
