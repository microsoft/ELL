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
        /// <param name="value"> The value. </param>
        void SetDepth(double value);

        /// <summary> Sets the offset. </summary>
        ///
        /// <param name="value"> The value. </param>
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
        /// <summary> default constructor. </summary>
        Layout(){};

        /// <summary> Constructs a layout with given parameters. </summary>
        ///
        /// <param name="size"> The size. </param>
        /// <param name="min_offset"> The minimum offset. </param>
        /// <param name="max_offset"> The maximum offset. </param>
        /// <param name="min_depth"> The minimum depth. </param>
        /// <param name="max_depth"> The maximum depth. </param>
        Layout(size_t size, double min_offset, double max_offset, double min_depth, double max_depth);

        /// <summary> \returns A reference to the position of a specified vertex. </summary>
        ///
        /// <param name="index"> Zero-based index of the. </param>
        ///
        /// <returns> The indexed value. </returns>
        VertexPosition& operator[](uint64 index);

        /// <summary> \returns A reference to the position of a specified vertex. </summary>
        ///
        /// <param name="index"> Zero-based index of the. </param>
        ///
        /// <returns> The indexed value. </returns>
        const VertexPosition& operator[](uint64 index) const;

        /// <summary> \return The number of vertices. </summary>
        ///
        /// <returns> An uint64. </returns>
        uint64 size() const; // TODO capitalize

        /// <summary>
        ///Returns the min offset (if the tree is top down, this is the left of its bounding box)
        /// </summary>
        ///
        /// <returns> The minimum offset. </returns>
        double GetMinOffset() const;

        /// <summary>
        ///Returns the max offset (if the tree is top down, this is the right of its bounding box)
        /// </summary>
        ///
        /// <returns> The maximum offset. </returns>
        double GetMaxOffset() const;

        /// <summary>
        ///Returns the min depth (if the tree is top down, this is the top of its bounding box)
        /// </summary>
        ///
        /// <returns> The minimum depth. </returns>
        double GetMinDepth() const;
        
        //Returns the max depth (if the tree is top down, this is the bottom of its bounding box)
        //
        double GetMaxDepth() const;

        /// <summary> Gets the vertices. </summary>
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
