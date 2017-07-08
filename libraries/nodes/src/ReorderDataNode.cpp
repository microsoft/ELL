////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ReorderDataNode.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ReorderDataNode.h"

// model
#include "CompilableNodeUtilities.h"
#include "IRMapCompiler.h"
#include "OutputNode.h"

// stl
#include <algorithm>

namespace ell
{
namespace nodes
{
    DataShape::DataShape(const std::array<size_t, Dimension>& extent)
        : _extent(extent), _offset{ 0, 0, 0 }
    {
        ComputeExtents({ 0, 1, 2 });
    }

    DataShape::DataShape(const std::array<size_t, Dimension>& extent, const std::array<size_t, Dimension>& padding)
        : _extent(extent), _offset(padding)
    {
        ComputeExtents({ 0, 1, 2 });
    }

    DataShape::DataShape(const std::array<size_t, Dimension>& extent, const std::array<size_t, Dimension>& padding, const std::array<size_t, Dimension>& order)
        : _extent(extent), _offset(padding)
    {
        ComputeExtents(order);
    }

    void DataShape::ComputeExtents(const std::array<size_t, Dimension>& order)
    {
        int dimension = _extent.size();
        _stride[order[0]] = 1; // min_stride
        for (int index = 1; index < dimension; ++index)
        {
            _stride[order[index]] = _stride[order[index - 1]] * (_extent[order[index - 1]] + 2 * _offset[order[index - 1]]);
        }
        _totalSize = _stride[order[dimension - 1]] * (_extent[order[dimension - 1]] + 2 * _offset[order[dimension - 1]]);
    }

    size_t DataShape::NumEntries() const
    {
        return std::accumulate(_extent.begin(), _extent.end(), 1, std::multiplies<size_t>());
    }

    size_t DataShape::GetMemorySize() const
    {
        return _totalSize;
    }

    size_t DataShape::GetDataOffset() const
    {
        size_t result = 0;
        for (int index = 0; index < Dimension; ++index)
        {
            result += _offset[index] * _stride[index];
        }
        return result;
    }

    size_t DataShape::GetEntryOffset(const std::array<int, Dimension>& location) const
    {
        size_t result = 0;
        for (int index = 0; index < Dimension; ++index)
        {
            result += (location[index] + _offset[index]) * _stride[index];
        }
        return result;
    }

    bool DataShape::IsOutOfBounds(const std::array<int, Dimension>& location) const
    {
        for (int index = 0; index < Dimension; ++index)
        {
            int minBounds = -_offset[index]; // needs to be signed -- can be negative
            int maxBounds = _extent[index] + _offset[index];
            if (location[index] < minBounds || location[index] >= maxBounds)
                return true;
        }
        return false;
    }

    llvm::Value* DataShape::EmitGetEntryOffset(emitters::IRFunctionEmitter& function, const std::array<llvm::Value*, Dimension>& location) const
    {
        llvm::Value* result = nullptr;
        for (int index = 0; index < Dimension; ++index)
        {
            auto offsetLocation = function.Operator(emitters::TypedOperator::add, function.Literal<int>(_offset[index]), location[index]);
            auto increment = function.Operator(emitters::TypedOperator::multiply, offsetLocation, function.Literal<int>(_stride[index]));
            if (result == nullptr)
            {
                result = increment;
            }
            else
            {
                result = function.Operator(emitters::TypedOperator::add, result, increment);
            }
        }
        return result;
    }

    llvm::Value* DataShape::EmitIsOutOfBounds(emitters::IRFunctionEmitter& function, const std::array<llvm::Value*, Dimension>& location) const
    {
        llvm::Value* result = function.FalseBit();
        llvm::Value* trueBit = function.TrueBit();
        for (int index = 0; index < Dimension; ++index)
        {
            int minBounds = -_offset[index]; // needs to be signed -- can be negative
            int maxBounds = _extent[index] + _offset[index];
            result = function.Operator(emitters::TypedOperator::logicalOr, result, function.Comparison(emitters::TypedComparison::lessThan, location[index], function.Literal<int>(minBounds)));
            result = function.Operator(emitters::TypedOperator::logicalOr, result, function.Comparison(emitters::TypedComparison::greaterThanOrEquals, location[index], function.Literal<int>(maxBounds)));
        }
        return result;
    }

    void DataShape::WriteToArchive(utilities::Archiver& archiver) const
    {
        archiver["extent"] << std::vector<size_t>{ _extent.begin(), _extent.end() };
        archiver["stride"] << std::vector<size_t>{ _stride.begin(), _stride.end() };
        archiver["offset"] << std::vector<size_t>{ _offset.begin(), _offset.end() };
        archiver["size"] << _totalSize;
    }

    void DataShape::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        std::vector<size_t> temp;

        archiver["extent"] >> temp;
        std::copy(temp.begin(), temp.end(), _extent.begin());
        temp.clear();

        archiver["stride"] >> temp;
        std::copy(temp.begin(), temp.end(), _stride.begin());
        temp.clear();

        archiver["offset"] >> temp;
        std::copy(temp.begin(), temp.end(), _offset.begin());
        temp.clear();

        archiver["size"] >> _totalSize;
    }
} // nodes
} // ell
