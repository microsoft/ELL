////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Layout.h (treeLayout)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <cstdint>
#include <vector>

/// <summary> treeLayout namespace </summary>
namespace treeLayout
{
    /// <summary>
    /// Contains the (depth, offset) position of one vertex in the tree. If the root of the tree is
    /// to be on top, positive depth is down and positive offset is right.
    /// </summary>
    class VertexPosition
    {
    public:
        
        /// <summary> Constructs a default vertex position. </summary>
        VertexPosition();

        /// <summary> Constructs a vertex position with specific depth and offset. </summary>
        ///
        /// <param name="depth"> The depth. </param>
        /// <param name="offset"> The offset. </param>
        VertexPosition(double depth, double offset);

        /// <summary> Returns the depth of the vertex. </summary>
        ///
        /// <returns> The depth. </returns>
        double GetDepth() const;

        /// <summary> Returns the offset of the vertex. </summary>
        ///
        /// <returns> The offset. </returns>
        double GetOffset() const;

        /// <summary> Sets the depth. </summary>
        ///
        /// <param name="value"> The depth. </param>
        void SetDepth(double value);

        /// <summary> Sets the offset. </summary>
        ///
        /// <param name="value"> The offset. </param>
        void SetOffset(double value);

    private:
        double _depth;
        double _offset;
    };

    /// <summary>
    /// Contains the Layout of an entire tree (the return value of a Layout generating algorithm)
    /// </summary>
    class Layout {

    public:

        /// <summary> Constructs a layout. </summary>
        Layout(){};

        /// <summary> Constructs a layout with given parameters. </summary>
        ///
        /// <param name="size"> The size. </param>
        /// <param name="min_offset"> The minimum offset. </param>
        /// <param name="max_offset"> The maximum offset. </param>
        /// <param name="min_depth"> The minimum depth. </param>
        /// <param name="max_depth"> The maximum depth. </param>
        Layout(uint64_t size, double min_offset, double max_offset, double min_depth, double max_depth);

        /// <summary> Returns a reference to the position of a specified vertex. </summary>
        ///
        /// <param name="index"> Zero-based index of the vertex. </param>
        ///
        /// <returns> The vertex position. </returns>
        VertexPosition& operator[](uint64_t index);

        /// <summary> Returns a reference to the position of a specified vertex. </summary>
        ///
        /// <param name="index"> Zero-based index of the vertex. </param>
        ///
        /// <returns> The vertex position. </returns>
        const VertexPosition& operator[](uint64_t index) const;

        /// <summary> Returns The number of vertices. </summary>
        ///
        /// <returns> The number of vertices. </returns>
        uint64_t Size() const;

        /// <summary>
        /// Returns the min offset (if the tree is top down, this is the left of its bounding box)
        /// </summary>
        ///
        /// <returns> The minimum offset. </returns>
        double GetMinOffset() const;

        /// <summary>
        /// Returns the max offset (if the tree is top down, this is the right of its bounding box)
        /// </summary>
        ///
        /// <returns> The maximum offset. </returns>
        double GetMaxOffset() const;

        /// <summary>
        /// Returns the min depth (if the tree is top down, this is the top of its bounding box)
        /// </summary>
        ///
        /// <returns> The minimum depth. </returns>
        double GetMinDepth() const;

        /// <summary> Returns the max depth (if the tree is top down, this is the bottom of its bounding
        /// box) </summary>
        ///
        /// <returns> The maximum depth. </returns>
        double GetMaxDepth() const;

        /// <summary> Gets the vector of vertex positions. </summary>
        ///
        /// <returns> The vertices. </returns>
        const std::vector<VertexPosition>& GetVertices() const;

    private:
        std::vector<VertexPosition> _positions;
        double _minOffset = 0;
        double _maxOffset = 0;
        double _minDepth = 0;
        double _maxDepth = 0;
    };
}
