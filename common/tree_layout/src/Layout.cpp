// Layout.cpp

#include "Layout.h"
#include <stdexcept>
#include <string>

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
        Positions(size), MinOffset(min_offset), MaxOffset(max_offset), MinDepth(min_depth), MaxDepth(max_depth)
    {}

    VertexPosition& Layout::operator[](int index)
    {
        return Positions[index];
    }

    const VertexPosition& Layout::operator[](int index) const
    {
        return Positions[index];
    }

    int Layout::size() const
    {
        return (int)Positions.size();
    }

    double Layout::GetMinOffset() const
    {
        return MinOffset;
    }

    double Layout::GetMaxOffset() const
    {
        return MaxOffset;
    }

    double Layout::GetMinDepth() const
    {
        return MinDepth;
    }

    double Layout::GetMaxDepth() const
    {
        return MaxDepth;
    }

}
