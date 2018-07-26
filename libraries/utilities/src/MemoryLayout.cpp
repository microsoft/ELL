////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MemoryLayout.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MemoryLayout.h"

// utilities
#include "Exception.h"

// stl
#include <numeric>

namespace ell
{
namespace utilities
{
    //
    // MemoryShape
    //

    int MemoryShape::NumElements() const
    {
        return std::accumulate(_shape.begin(), _shape.end(), 1, std::multiplies<int>());
    }

    void MemoryShape::Resize(int numDimensions)
    {
        if (numDimensions > static_cast<int>(_shape.size()))
        {
            int extraDimensions = numDimensions - static_cast<int>(_shape.size());
            _shape.insert(_shape.begin(), extraDimensions, 1);
        }
        while (numDimensions < static_cast<int>(_shape.size()))
        {
            _shape[1] *= _shape[0];
            _shape.erase(_shape.begin());
        }
    }

    //
    // MemoryLayout
    //
    MemoryLayout::MemoryLayout(const MemoryShape& size)
        : _size(size), _stride(size), _offset(std::vector<int>(size.NumDimensions(), 0)), _increment({})
    {
        _increment = ComputeCumulativeIncrement();
    }

    MemoryLayout::MemoryLayout(const MemoryShape& size, const MemoryShape& padding)
        : _size(size), _stride({}), _offset(padding), _increment({})
    {
        _stride.Resize(_size.NumDimensions());
        for (int index = 0; index < _size.NumDimensions(); ++index)
        {
            _stride[index] = _size[index] + (2 * padding[index]);
        }
        _increment = ComputeCumulativeIncrement();
    }

    MemoryLayout::MemoryLayout(const MemoryShape& size, const MemoryShape& stride, const MemoryShape& offset)
        : _size(size), _stride(stride), _offset(offset), _increment({})
    {
        _increment = ComputeCumulativeIncrement();
    }

    bool MemoryLayout::HasPadding() const
    {
        return _size != _stride;
    }

    size_t MemoryLayout::NumElements() const
    {
        return static_cast<size_t>(_size.NumElements());
    }

    size_t MemoryLayout::GetMemorySize() const
    {
        return static_cast<size_t>(_stride.NumElements());
    }

    MemoryShape MemoryLayout::ComputeCumulativeIncrement() const
    {
        const auto numDimensions = NumDimensions();
        std::vector<int> result(numDimensions);
        int prevScale = 1;
        for (int index = numDimensions - 1; index >= 0; --index)
        {
            result[index] = prevScale;
            prevScale = prevScale * _stride[index];
        }
        return { result };
    }

    size_t MemoryLayout::GetDataOffset() const
    {
        size_t result = 0;
        auto increment = GetCumulativeIncrement();
        const int numDimensions = NumDimensions();
        for (int index = 0; index < numDimensions; ++index)
        {
            result += _offset[index] * increment[index];
        }
        return result;
    }

    size_t MemoryLayout::GetEntryOffset(const MemoryShape& location) const
    {
        auto increment = GetCumulativeIncrement();
        auto numDimensions = static_cast<int>(NumDimensions());
        size_t result = 0;
        for (int index = 0; index < numDimensions; ++index)
        {
            result += increment[index] * (location[index] + _offset[index]);
        }
        return result;
    }

    bool MemoryLayout::IsOutOfBounds(const MemoryShape& location) const
    {
        const int numDimensions = NumDimensions();
        for (int index = 0; index < numDimensions; ++index)
        {
            if ((location[index] + _offset[index]) < 0 || (location[index]) - _offset[index] >= _stride[index])
            {
                return true;
            }
        }
        return false;
    }

    void MemoryLayout::WriteToArchive(utilities::Archiver& archiver) const
    {
        archiver["size"] << _size.ToVector();
        archiver["stride"] << _stride.ToVector();
        archiver["offset"] << _offset.ToVector();
    }

    void MemoryLayout::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        std::vector<int> temp;
        archiver["size"] >> temp;
        _size = { temp };
        archiver["stride"] >> temp;
        _stride = { temp };
        archiver["offset"] >> temp;
        _offset = { temp };
        _increment = ComputeCumulativeIncrement();
    }

    size_t NumElements(const MemoryShape& size)
    {
        return size.NumElements();
    }

    bool ShapesEqual(const MemoryShape& shape1, const MemoryShape& shape2)
    {
        auto size = shape1.NumDimensions();
        if (size != shape2.NumDimensions())
        {
            return false;
        }

        for (int index = 0; index < size; ++index)
        {
            if (shape1[index] != shape2[index])
            {
                return false;
            }
        }
        return true;
    }

    bool operator==(const MemoryShape& shape1, const MemoryShape& shape2)
    {
        return ShapesEqual(shape1, shape2);
    }

    bool operator!=(const MemoryShape& shape1, const MemoryShape& shape2)
    {
        return !ShapesEqual(shape1, shape2);
    }

    bool MemoryLayoutsEqual(const MemoryLayout& layout1, const MemoryLayout& layout2)
    {
        return ShapesEqual(layout1.GetStride(), layout2.GetStride()) && ShapesEqual(layout1.GetActiveSize(), layout2.GetActiveSize()) && ShapesEqual(layout1.GetOffset(), layout2.GetOffset());
    }

    bool operator==(const MemoryLayout& layout1, const MemoryLayout& layout2)
    {
        return MemoryLayoutsEqual(layout1, layout2);
    }

    bool operator!=(const MemoryLayout& layout1, const MemoryLayout& layout2)
    {
        return !MemoryLayoutsEqual(layout1, layout2);
    }

}
}
