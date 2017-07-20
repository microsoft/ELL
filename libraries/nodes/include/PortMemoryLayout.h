////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PortMemoryLayout.h (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "IArchivable.h"

// stl
#include <vector>

namespace ell
{
namespace nodes
{

    /// <summary> A vector of numbers representing shape information. </summary>
    using Shape = std::vector<size_t>;

    /// <summary> Checks if two shapes are equal. </summary>
    ///
    /// <param name="shape1"> The first shape. </param>
    /// <param name="shape2"> The other shape. </param>
    bool ShapesEqual(const Shape& shape1, const Shape& shape2);

    /// <summary> A struct representing the memory layout of port data. </summary>
    struct PortMemoryLayout : public utilities::IArchivable
    {
        Shape size; // the "active" area of the memory
        Shape stride; // the allocated size along each dimension
        Shape offset; // the offset to the active area for each dimension

        PortMemoryLayout() = default;
        
        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <param name="size"> The extent of the active are of the memory region. </param>
        /// <param name="stride"> The extent of the allocated memory of the memory region. </param>
        /// <param name="offset"> The offset into memory to the active area of the memory region. </param>
        PortMemoryLayout(const Shape& size, const Shape& stride, const Shape& offset);

        /// <summary> Returns the number of dimensions in this memory layout </summary>
        ///
        /// <returns> The number of dimensions </summary>
        size_t NumDimensions() const { return size.size(); }
         
        /// <summary>
        /// Gets the cumulative increments for each dimension. This is the distance in memory
        /// between two entries that are adjacent in that dimension.
        /// </summary>
        ///
        /// <returns> A `Shape` object containing the cumulative increment in each dimension </returns>
        Shape GetCumulativeIncrement() const;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "PortMemoryLayout"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;
    };

    /// <summary> Checks if two memory layouts are equal. </summary>
    ///
    /// <param name="layout1"> The first memory layout. </param>
    /// <param name="layout2"> The other memory layout. </param>
    bool PortMemoryLayoutsEqual(const PortMemoryLayout& layout1, const PortMemoryLayout& layout2);
}
}
