////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PortMemoryLayout.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PortMemoryLayout.h"

// emitters
#include "IRFunctionEmitter.h"

// utilities
#include "Exception.h"

// stl
#include <numeric>

namespace ell
{
namespace model
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
    // PortMemoryLayout
    //
    PortMemoryLayout::PortMemoryLayout(const MemoryShape& size)
        : _size(size), _stride(size), _offset(std::vector<int>(size.NumDimensions(), 0)), _increment({})
    {
        _increment = ComputeCumulativeIncrement();
    }

    PortMemoryLayout::PortMemoryLayout(const MemoryShape& size, const MemoryShape& padding)
        : _size(size), _stride({}), _offset(padding), _increment({})
    {
        _stride.Resize(_size.NumDimensions());
        for (int index = 0; index < _size.NumDimensions(); ++index)
        {
            _stride[index] = _size[index] + (2 * padding[index]);
        }
        _increment = ComputeCumulativeIncrement();
    }

    PortMemoryLayout::PortMemoryLayout(const MemoryShape& size, const MemoryShape& stride, const MemoryShape& offset)
        : _size(size), _stride(stride), _offset(offset), _increment({})
    {
        _increment = ComputeCumulativeIncrement();
    }

    bool PortMemoryLayout::HasPadding() const
    {
        return _size != _stride;
    }

    size_t PortMemoryLayout::NumElements() const
    {
        return static_cast<size_t>(_size.NumElements());
    }

    size_t PortMemoryLayout::GetMemorySize() const
    {
        return static_cast<size_t>(_stride.NumElements());
    }

    MemoryShape PortMemoryLayout::ComputeCumulativeIncrement() const
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

    size_t PortMemoryLayout::GetDataOffset() const
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

    size_t PortMemoryLayout::GetEntryOffset(const MemoryShape& location) const
    {
        auto increment = GetCumulativeIncrement();
        auto numDimensions = NumDimensions();
        size_t result = 0;
        for (size_t index = 0; index < numDimensions; ++index)
        {
            result += increment[index] * (location[index] + _offset[index]);
        }
        return result;
    }

    bool PortMemoryLayout::IsOutOfBounds(const MemoryShape& location) const
    {
        const int numDimensions = NumDimensions();
        for (int index = 0; index < numDimensions; ++index)
        {
            if (location[index] + _offset[index] < 0 || location[index] - _offset[index] >= _stride[index])
            {
                return true;
            }
        }
        return false;
    }

    llvm::Value* PortMemoryLayout::EmitGetEntryOffset(emitters::IRFunctionEmitter& function, const std::vector<llvm::Value*>& location) const
    {
        auto increment = GetCumulativeIncrement();
        const int numDimensions = NumDimensions();
        llvm::Value* result = nullptr;
        for (int index = 0; index < numDimensions; ++index)
        {
            auto offsetLocation = function.Operator(emitters::TypedOperator::add, function.Literal<int>(_offset[index]), location[index]);
            auto dimensionOffset = function.Operator(emitters::TypedOperator::multiply, offsetLocation, function.Literal<int>(increment[index]));
            if (result == nullptr)
            {
                result = dimensionOffset;
            }
            else
            {
                result = function.Operator(emitters::TypedOperator::add, result, dimensionOffset);
            }
        }
        return result;
    }

    llvm::Value* PortMemoryLayout::EmitIsOutOfBounds(emitters::IRFunctionEmitter& function, const std::vector<llvm::Value*>& location) const
    {
        llvm::Value* result = nullptr;
        const int numDimensions = NumDimensions();
        for (int index = 0; index < numDimensions; ++index)
        {
            auto test1 = function.Comparison(emitters::TypedComparison::lessThan, function.Operator(emitters::TypedOperator::add, location[index], function.Literal<int>(_offset[index])), function.Literal<int>(0));
            if (result == nullptr)
            {
                result = test1;
            }
            else
            {
                result = function.Operator(emitters::TypedOperator::logicalOr, result, test1);
            }

            auto test2 = function.Comparison(emitters::TypedComparison::lessThan, function.Operator(emitters::TypedOperator::subtract, location[index], function.Literal<int>(_offset[index])), function.Literal<int>(_stride[index]));
            result = function.Operator(emitters::TypedOperator::logicalOr, result, test2);
        }
        return result;
    }

    void PortMemoryLayout::WriteToArchive(utilities::Archiver& archiver) const
    {
        archiver["size"] << _size.ToVector();
        archiver["stride"] << _stride.ToVector();
        archiver["offset"] << _offset.ToVector();
    }

    void PortMemoryLayout::ReadFromArchive(utilities::Unarchiver& archiver)
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

    bool PortMemoryLayoutsEqual(const PortMemoryLayout& layout1, const PortMemoryLayout& layout2)
    {
        return ShapesEqual(layout1.GetStride(), layout2.GetStride()) && ShapesEqual(layout1.GetActiveSize(), layout2.GetActiveSize()) && ShapesEqual(layout1.GetOffset(), layout2.GetOffset());
    }
}
}
