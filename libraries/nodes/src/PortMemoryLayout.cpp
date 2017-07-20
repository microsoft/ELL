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

    Shape PortMemoryLayout::GetCumulativeIncrement() const
    {
        const auto numDimensions = NumDimensions();
        Shape result(numDimensions);
        int prevScale = 1;
        for (int index = numDimensions - 1; index >= 0; --index)
        {
            result[index] = prevScale;
            prevScale = prevScale * stride[index];
        }
        return result;
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
