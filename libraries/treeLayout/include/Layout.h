////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Layout.h (treeLayout)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// types
#include "types.h"

// stl
#include <vector>
#include <cstring> // for size_t

namespace treeLayout
{
    /// Contains the (depth, offset) position of one vertex in the tree. If the root of the tree is to be on top, positive depth is down and positive offset is right.
    ///
    class VertexPosition
    {
    public:
        
        /// Constructs a default vertex position
        ///
        VertexPosition();        
        
        /// Constructs a vertex position with specific depth and offset
        ///
        VertexPosition(double depth, double offset);

        /// \returns The depth of the vertex
        ///
        double GetDepth() const;

        /// \returns The offset of the vertex
        ///
        double GetOffset() const;

        /// Sets the depth
        ///
        void SetDepth(double value);

        /// Sets the offset
        ///
        void SetOffset(double value);

    private:
        double _depth;
        double _offset;
    };

    /// Contains the Layout of an entire tree (the return value of a Layout generating algorithm)
    ///
    class Layout {

    public:
        /// default constructor
        Layout(){};

        /// Constructs a layout with given parameters
        ///
        Layout(size_t size, double min_offset, double max_offset, double min_depth, double max_depth);

        ///
        /// \returns A reference to the position of a specified vertex
        ///
        VertexPosition& operator[](uint64 index);

        /// \returns A reference to the position of a specified vertex
        ///
        const VertexPosition& operator[](uint64 index) const;

        /// \return The number of vertices
        ///
        uint64 size() const;

        /// \returns The min offset (if the tree is top down, this is the left of its bounding box)
        ///
        double GetMinOffset() const;

        /// \returns The max offset (if the tree is top down, this is the right of its bounding box)
        ///
        double GetMaxOffset() const;
        
        /// \returns The min depth (if the tree is top down, this is the top of its bounding box)
        ///
        double GetMinDepth() const;
        
        /// \returns The max depth (if the tree is top down, this is the bottom of its bounding box)
        ///
        double GetMaxDepth() const;

        const std::vector<VertexPosition>& GetVertices() const;

    private:
        std::vector<VertexPosition> _positions;
        double _minOffset = 0;
        double _maxOffset = 0;
        double _minDepth = 0;
        double _maxDepth = 0;
    };
}
