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

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "PortMemoryLayout"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Adds an object's properties to an `Archiver` </summary>
        ///
        /// <param name="archiver"> The `Archiver` to add the values from the object to </param>
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;

        /// <summary> Sets the internal state of the object according to the archiver passed in </summary>
        ///
        /// <param name="archiver"> The `Archiver` to get state from </param>
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;
    };

    /// <summary> Checks if two memory layouts are equal. </summary>
    ///
    /// <param name="layout1"> The first memory layout. </param>
    /// <param name="layout2"> The other memory layout. </param>
    bool PortMemoryLayoutsEqual(const PortMemoryLayout& layout1, const PortMemoryLayout& layout2);
}
}
