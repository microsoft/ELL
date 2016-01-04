// Layout.cpp

#include "Layout.h"

#include <stdexcept>
using std::runtime_error;

namespace tree_layout
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

    Layout::Layout(size_t size, double min_offset, double max_offset, double min_depth, double max_depth) :
        _positions(size), _minOffset(min_offset), _maxOffset(max_offset), _minDepth(min_depth), _maxDepth(max_depth)
    {}

    VertexPosition& Layout::operator[](uint64 index)
    {
        return _positions[index];
    }

    const VertexPosition& Layout::operator[](uint64 index) const
    {
        return _positions[index];
    }

    uint64 Layout::size() const
    {
        return (uint64)_positions.size();
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

    const vector<VertexPosition>& Layout::GetVertices() const
    {
        return _positions;
    }
}
