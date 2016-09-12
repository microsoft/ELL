////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Layout.cpp (treeLayout)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Layout.h"

// stl
#include <stdexcept>

namespace emll
{
namespace treeLayout
{
    VertexPosition::VertexPosition(double depth, double offset)
        : _depth(depth), _offset(offset)
    {
    }

    void VertexPosition::SetDepth(double value)
    {
        _depth = value;
    }

    void VertexPosition::SetOffset(double value)
    {
        _offset = value;
    }

    Layout::Layout(uint64_t size, double min_offset, double max_offset, double min_depth, double max_depth)
        : _positions(size), _minOffset(min_offset), _maxOffset(max_offset), _minDepth(min_depth), _maxDepth(max_depth)
    {
    }

    VertexPosition& Layout::operator[](uint64_t index)
    {
        return _positions[index];
    }

    const VertexPosition& Layout::operator[](uint64_t index) const
    {
        return _positions[index];
    }
}
}
