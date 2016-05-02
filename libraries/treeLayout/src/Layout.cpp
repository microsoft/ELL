////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     Layout.cpp (treeLayout)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Layout.h"

// stl
#include <stdexcept>

namespace treeLayout
{
    VertexPosition::VertexPosition()
    {}

    VertexPosition::VertexPosition(double depth, double offset) :
        _depth(depth), _offset(offset)
    {}

    double VertexPosition::GetDepth() const
    {
        return _depth;
    }

    double VertexPosition::GetOffset() const
    {
        return _offset;
    }

    void VertexPosition::SetDepth(double value)
    {
        _depth = value;
    }

    void VertexPosition::SetOffset(double value)
    {
        _offset = value;
    }

    Layout::Layout(uint64_t size, double min_offset, double max_offset, double min_depth, double max_depth) :
        _positions(size), _minOffset(min_offset), _maxOffset(max_offset), _minDepth(min_depth), _maxDepth(max_depth)
    {}

    VertexPosition& Layout::operator[](uint64_t index)
    {
        return _positions[index];
    }

    const VertexPosition& Layout::operator[](uint64_t index) const
    {
        return _positions[index];
    }

    uint64_t Layout::Size() const
    {
        return (uint64_t)_positions.size();
    }

    double Layout::GetMinOffset() const
    {
        return _minOffset;
    }

    double Layout::GetMaxOffset() const
    {
        return _maxOffset;
    }

    double Layout::GetMinDepth() const
    {
        return _minDepth;
    }

    double Layout::GetMaxDepth() const
    {
        return _maxDepth;
    }

    const std::vector<VertexPosition>& Layout::GetVertices() const
    {
        return _positions;
    }
}
