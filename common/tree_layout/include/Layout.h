// Layout.h

#pragma once

#include <vector>

using std::vector;

namespace tree_layout
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

        ///
        ///
        Layout(size_t size, double min_offset, double max_offset, double min_depth, double max_depth);

        Layout(const Layout& l) : Positions(l.Positions), MinOffset(l.MinOffset), MaxOffset(l.MaxOffset), MinDepth(l.MinDepth), MaxDepth(l.MaxDepth) {}

        ///
        /// \returns A reference to the position of a specified vertex
        ///
        VertexPosition& operator[](int index);

        /// \returns A reference to the position of a specified vertex
        ///
        const VertexPosition& operator[](int index) const;

        /// \return The number of vertices
        ///
        int size() const;

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

        const vector<VertexPosition>& GetVertices() const { return Positions; }

    private:
        vector<VertexPosition> Positions;
        double MinOffset = 0;
        double MaxOffset = 0;
        double MinDepth = 0;
        double MaxDepth = 0;
    };
}