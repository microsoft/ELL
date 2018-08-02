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
#include "Unused.h"

// stl
#include <numeric>

namespace ell
{
namespace utilities
{
    namespace
    {
        MemoryShape Permute(const MemoryShape& shape, const DimensionOrder& order)
        {
            const int numDimensions = shape.NumDimensions();
            std::vector<int> result(numDimensions);
            for(int index = 0; index < numDimensions; ++index)
            {
                result[index] = shape[order[index]];
            }
            return { result };
        }

        MemoryCoordinates Permute(const MemoryCoordinates& coordinates, const DimensionOrder& order)
        {
            const int numDimensions = coordinates.NumDimensions();
            std::vector<int> result(numDimensions);
            for(int index = 0; index < numDimensions; ++index)
            {
                result[index] = coordinates[order[index]];
            }
            return { result };
        }

        MemoryShape ReversePermute(const MemoryShape& shape, const DimensionOrder& order)
        {
            const int numDimensions = shape.NumDimensions();
            std::vector<int> result(numDimensions);
            for(int index = 0; index < numDimensions; ++index)
            {
                result[order[index]] = shape[index];
            }
            return { result };
        }

        MemoryCoordinates ReversePermute(const MemoryCoordinates& coordinates, const DimensionOrder& order)
        {
            const int numDimensions = coordinates.NumDimensions();
            std::vector<int> result(numDimensions);
            for(int index = 0; index < numDimensions; ++index)
            {
                result[order[index]] = coordinates[index];
            }
            return { result };
        }
    }

    //
    // DimensionVector
    //
    DimensionVector::DimensionVector(const std::vector<size_t>& elements) : _data(elements.size())
    {
        std::transform(elements.begin(), elements.end(), _data.begin(), [](size_t x) { return static_cast<int>(x); });
    }

    //
    // DimensionOrder
    //

    DimensionOrder::DimensionOrder(int numDimensions) : DimensionVector(std::vector<int>(numDimensions))
    {
        std::iota(_data.begin(), _data.end(), 0);
    }

    DimensionOrder::DimensionOrder(const std::vector<int>& order) : DimensionVector(order)
    {
        std::vector<int> test(order.size());
        std::iota(test.begin(), test.end(), 0);
        if (!std::is_permutation(order.begin(), order.end(), test.begin()))
        {
            throw InputException(InputExceptionErrors::invalidArgument, "Dimension order must be a valid permutation vector.");
        }
    }

    DimensionOrder::DimensionOrder(const std::initializer_list<int>& order) : DimensionVector(order)
    {
        std::vector<int> test(order.size());
        std::iota(test.begin(), test.end(), 0);
        if (!std::is_permutation(order.begin(), order.end(), test.begin()))
        {
            throw InputException(InputExceptionErrors::invalidArgument, "Dimension order must be a valid permutation vector.");
        }
    }

    int DimensionOrder::operator[](int index) const
    {
        return DimensionVector::operator[](index);
    }

    bool DimensionOrder::IsCanonicalOrder() const
    {
        for(int index = 0; index < NumDimensions(); ++index)
        {
            if (index != (*this)[index])
            {
                return false;
            }
        }
        return true;
    }

    //
    // MemoryShape / Coordinates
    //

    int MemoryShape::NumElements() const
    {
        return std::accumulate(_data.begin(), _data.end(), 1, std::multiplies<int>());
    }

    void MemoryShape::Resize(int numDimensions)
    {
        if (numDimensions > static_cast<int>(_data.size()))
        {
            int extraDimensions = numDimensions - static_cast<int>(_data.size());
            _data.insert(_data.begin(), extraDimensions, 1);
        }
        while (numDimensions < static_cast<int>(_data.size()))
        {
            _data[1] *= _data[0];
            _data.erase(_data.begin());
        }
    }

    //
    // MemoryLayout
    //
    MemoryLayout::MemoryLayout(const MemoryShape& physicalDimensionSize)
        : MemoryLayout(physicalDimensionSize, physicalDimensionSize, MemoryShape(std::vector<int>(physicalDimensionSize.NumDimensions(), 0)))
    {
    }

    MemoryLayout::MemoryLayout(const MemoryShape& physicalDimensionSize, const MemoryShape& physicalDimensionPadding)
        : _size(physicalDimensionSize), _stride(physicalDimensionSize), _offset(physicalDimensionPadding), _increment({}), _dimensionOrder(physicalDimensionSize.NumDimensions())
    {
        for (int index = 0; index < _size.NumDimensions(); ++index)
        {
            if (physicalDimensionPadding[index] < 0)
            {
                throw InputException(InputExceptionErrors::invalidArgument, "Padding must be positive or zero.");
            }
            _stride[index] = _size[index] + (2 * physicalDimensionPadding[index]);
            if (_size[index] + _offset[index] > _stride[index])
            {
                throw InputException(InputExceptionErrors::invalidArgument, "Stride must be larger or equal to the size plus offset.");
            }
        }
        _increment = ComputeCumulativeIncrement();
    }

    MemoryLayout::MemoryLayout(const MemoryShape& physicalDimensionSize, const MemoryShape& physicalDimensionStride, const MemoryShape& physicalDimensionOffset)
        : _size(physicalDimensionSize), _stride(physicalDimensionStride), _offset(physicalDimensionOffset), _increment({}), _dimensionOrder(physicalDimensionSize.NumDimensions())
    {
        for (int index = 0; index < _size.NumDimensions(); ++index)
        {
            if (_size[index] + _offset[index] > _stride[index])
            {
                throw InputException(InputExceptionErrors::invalidArgument, "Stride must be larger or equal to the size plus offset.");
            }
        }
        _increment = ComputeCumulativeIncrement();
    }

    MemoryLayout::MemoryLayout(const MemoryShape& physicalDimensionSize, const DimensionOrder& order)
        : MemoryLayout(physicalDimensionSize, physicalDimensionSize, MemoryShape(std::vector<int>(physicalDimensionSize.NumDimensions(), 0)), order)
    {
    }

    MemoryLayout::MemoryLayout(const MemoryShape& physicalDimensionSize, const MemoryShape& physicalDimensionPadding, const DimensionOrder& order)
        : _size(physicalDimensionSize), _stride(physicalDimensionSize), _offset(physicalDimensionPadding), _increment({}), _dimensionOrder(order)
    {
        for (int index = 0; index < _size.NumDimensions(); ++index)
        {
            if (physicalDimensionPadding[index] < 0)
            {
                throw InputException(InputExceptionErrors::invalidArgument, "Padding must be positive or zero.");
            }
            _stride[index] = _size[index] + (2 * physicalDimensionPadding[index]);
            if (_size[index] + _offset[index] > _stride[index])
            {
                throw InputException(InputExceptionErrors::invalidArgument, "Stride must be larger or equal to the size plus offset.");
            }
        }
        _increment = ComputeCumulativeIncrement();
    }

    MemoryLayout::MemoryLayout(const MemoryShape& physicalDimensionSize, const MemoryShape& physicalDimensionStride, const MemoryShape& physicalDimensionOffset, const DimensionOrder& order)
        : _size(physicalDimensionSize), _stride(physicalDimensionStride), _offset(physicalDimensionOffset), _increment({}), _dimensionOrder(order)
    {
        for (int index = 0; index < _size.NumDimensions(); ++index)
        {
            if (_size[index] + _offset[index] > _stride[index])
            {
                throw InputException(InputExceptionErrors::invalidArgument, "Stride must be larger or equal to the size plus offset.");
            }
        }
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

    size_t MemoryLayout::GetEntryOffset(const MemoryCoordinates& physicalCoordinates) const
    {
        const auto& offset = GetOffset();
        const auto& increment = GetCumulativeIncrement();
        const auto numDimensions = NumDimensions();
        size_t result = 0;
        for (int index = 0; index < numDimensions; ++index)
        {
            result += increment[index] * (physicalCoordinates[index] + offset[index]);
        }
        return result;
    }

    MemoryCoordinates MemoryLayout::GetLogicalCoordinates(const MemoryCoordinates& physicalCoordinates) const
    {
        if (NumDimensions() != physicalCoordinates.NumDimensions())
        {
            throw InputException(InputExceptionErrors::invalidArgument, "Must have the same number of dimensions.");
        }

        return ReversePermute(physicalCoordinates, _dimensionOrder);
    }

    MemoryShape MemoryLayout::GetLogicalDimensionActiveSize() const
    {
        return ReversePermute(_size, _dimensionOrder);
    }

    MemoryShape MemoryLayout::GetLogicalDimensionStride() const
    {
        return ReversePermute(_stride, _dimensionOrder);
    }

    MemoryShape MemoryLayout::GetLogicalDimensionOffset() const
    {
        return ReversePermute(_offset, _dimensionOrder);
    }

    size_t MemoryLayout::GetLogicalDimensionIncrement(size_t index) const
    {
        return ReversePermute(_increment, _dimensionOrder)[index];
    }

    MemoryCoordinates MemoryLayout::GetPhysicalCoordinates(const MemoryCoordinates& logicalCoordinates) const
    {
        if (NumDimensions() != logicalCoordinates.NumDimensions())
        {
            throw InputException(InputExceptionErrors::invalidArgument, "Must have the same number of dimensions.");
        }

        return Permute(logicalCoordinates, _dimensionOrder);
    }

    bool MemoryLayout::IsOutOfBounds(const MemoryCoordinates& physicalCoordinates) const
    {
        const int numDimensions = NumDimensions();
        for (int index = 0; index < numDimensions; ++index)
        {
            if (physicalCoordinates[index] + _offset[index] < 0 || physicalCoordinates[index] - _offset[index] >= _stride[index])
            {
                return true;
            }
        }
        return false;
    }

    bool MemoryLayout::IsContiguous() const
    {
        return _size == _stride && IsCanonicalOrder();
    }

    bool MemoryLayout::IsCanonicalOrder() const
    {
        return _dimensionOrder.IsCanonicalOrder();
    }

    MemoryLayout MemoryLayout::ReorderedCopy(const DimensionOrder& newOrder) const
    {
        if (!IsCanonicalOrder())
        {
            throw LogicException(LogicExceptionErrors::notImplemented, "Not implemented... yet.");
        }
        MemoryLayout result{ Permute(GetActiveSize(), newOrder), Permute(GetStride(), newOrder), Permute(GetOffset(), newOrder), newOrder };
        return result;
    }

    void MemoryLayout::WriteToArchive(utilities::Archiver& archiver) const
    {
        archiver["size"] << _size.ToVector();
        archiver["stride"] << _stride.ToVector();
        archiver["offset"] << _offset.ToVector();
        archiver["order"] << _dimensionOrder.ToVector();
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
        temp.resize(_size.NumDimensions());
        std::iota(temp.begin(), temp.end(), 0);
        archiver.OptionalProperty("order") >> temp;
        _dimensionOrder = { temp };
        _increment = ComputeCumulativeIncrement();
    }

    bool Equal(const DimensionVector& shape1, const DimensionVector& shape2)
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

    bool operator==(const DimensionOrder& order1, const DimensionOrder& order2)
    {
        return Equal(order1, order2);
    }

    bool operator!=(const DimensionOrder& order1, const DimensionOrder& order2)
    {
        return !Equal(order1, order2);
    }

    bool operator==(const MemoryShape& shape1, const MemoryShape& shape2)
    {
        return Equal(shape1, shape2);
    }

    bool operator!=(const MemoryShape& shape1, const MemoryShape& shape2)
    {
        return !Equal(shape1, shape2);
    }

    bool operator==(const MemoryCoordinates& shape1, const MemoryCoordinates& shape2)
    {
        return Equal(shape1, shape2);
    }

    bool operator!=(const MemoryCoordinates& shape1, const MemoryCoordinates& shape2)
    {
        return !Equal(shape1, shape2);
    }

    bool MemoryLayoutsEqual(const MemoryLayout& layout1, const MemoryLayout& layout2)
    {
        return (layout1.GetStride() == layout2.GetStride()) && (layout1.GetActiveSize() == layout2.GetActiveSize()) && (layout1.GetOffset() == layout2.GetOffset());
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
