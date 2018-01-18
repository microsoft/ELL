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

// stl
#include <numeric>

namespace ell
{
namespace model
{
    PortMemoryLayout::PortMemoryLayout(const Shape& size)
    : _size(size), _stride(size)
    {
        _offset.resize(_size.size(), 0);
    }

    PortMemoryLayout::PortMemoryLayout(const Shape& size, const Shape& padding)
    : _size(size), _offset(padding)
    {
        _stride.resize(_size.size());
        for (size_t index = 0; index < _size.size(); ++index)
        {
            _stride[index] = _size[index] + (2 * padding[index]);
        }
    }

    PortMemoryLayout::PortMemoryLayout(const Shape& size, const Shape& stride, const Shape& offset)
        : _size(size), _stride(stride), _offset(offset)
    {
    }

    size_t PortMemoryLayout::NumEntries() const
    {
        return std::accumulate(_size.begin(), _size.end(), 1, std::multiplies<size_t>());
    }

    size_t PortMemoryLayout::GetMemorySize() const
    {
        return std::accumulate(_stride.begin(), _stride.end(), 1, std::multiplies<size_t>());
    }

    Shape PortMemoryLayout::GetCumulativeIncrement() const
    {
        const auto numDimensions = NumDimensions();
        Shape result(numDimensions);
        int prevScale = 1;
        for (int index = numDimensions - 1; index >= 0; --index)
        {
            result[index] = prevScale;
            prevScale = prevScale * _stride[index];
        }
        return result;
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

    size_t PortMemoryLayout::GetEntryOffset(const Shape& location) const
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

    bool PortMemoryLayout::IsOutOfBounds(const Shape& location) const
    {
        const int numDimensions = NumDimensions();
        for (int index = 0; index < numDimensions; ++index)
        {
            if (static_cast<int>(location[index]+_offset[index]) < 0 || static_cast<int>(location[index])-_offset[index] >= _stride[index])
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
            if(result == nullptr)
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
        archiver["size"] << _size;
        archiver["stride"] << _stride;
        archiver["offset"] << _offset;
    }

    void PortMemoryLayout::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        archiver["size"] >> _size;
        archiver["stride"] >> _stride;
        archiver["offset"] >> _offset;
    }

    size_t NumElements(const Shape& size)
    {
        size_t result = 1;
        for (auto s : size)
        {
            result *= s;
        }
        return result;
    }

    bool ShapesEqual(const Shape& shape1, const Shape& shape2)
    {
        auto size = shape1.size();
        if (size != shape2.size())
        {
            return false;
        }

        for (size_t index = 0; index < size; ++index)
        {
            if (shape1[index] != shape2[index])
            {
                return false;
            }
        }
        return true;
    }

    bool PortMemoryLayoutsEqual(const PortMemoryLayout& layout1, const PortMemoryLayout& layout2)
    {
        return ShapesEqual(layout1.GetStride(), layout2.GetStride()) && ShapesEqual(layout1.GetActiveSize(), layout2.GetActiveSize()) && ShapesEqual(layout1.GetOffset(), layout2.GetOffset());
    }
}
}
