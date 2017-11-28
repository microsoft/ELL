////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ArchiveVersion.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ArchiveVersion.h"

namespace ell
{
namespace utilities
{
    bool operator==(const ArchiveVersion& a, const ArchiveVersion& b)
    {
        return a.versionNumber == b.versionNumber;
    }

    bool operator!=(const ArchiveVersion& a, const ArchiveVersion& b)
    {
        return !(a == b);
    }

    bool operator>(const ArchiveVersion& a, const ArchiveVersion& b)
    {
        return a.versionNumber > b.versionNumber;
    }

    bool operator>=(const ArchiveVersion& a, const ArchiveVersion& b)
    {
        return a > b || a == b;
    }

    bool operator<(const ArchiveVersion& a, const ArchiveVersion& b)
    {
        return !(a >= b);
    }

    bool operator<=(const ArchiveVersion& a, const ArchiveVersion& b)
    {
        return !(a > b);
    }
}
}
