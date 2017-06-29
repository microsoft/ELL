////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PortMemoryLayout.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PortMemoryLayout.h"

namespace ell
{
namespace nodes
{
    PortMemoryLayout::PortMemoryLayout(const Shape& size, const Shape& stride, const Shape& offset)
        : size(size), stride(stride), offset(offset)
    {
    }

    void PortMemoryLayout::WriteToArchive(utilities::Archiver& archiver) const
    {
        archiver["size"] << size;
        archiver["stride"] << stride;
        archiver["offset"] << offset;
    }

    void PortMemoryLayout::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        archiver["size"] >> size;
        archiver["stride"] >> stride;
        archiver["offset"] >> offset;
    }
}
}
