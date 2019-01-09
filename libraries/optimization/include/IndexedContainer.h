////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IndexedContainer.h (optimization)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstddef> // size_t
#include <vector>

namespace ell
{
namespace optimization
{
    /// <summary> An interface for any container that implements Size() and Get() functions </summary>
    template <typename ElementType>
    struct IndexedContainer
    {
        virtual ~IndexedContainer() = default;

        /// <summary> Returns the number of elements in the container </summary>
        virtual size_t Size() const = 0;

        /// <summary> Gets a reference to the elements that corresponds to a given index. </summary>
        virtual ElementType Get(size_t index) const = 0;
    };

    /// <summary> An extension of std::vector that implements the IndexedContainer interface. </summary>
    template <typename VectorElementType, typename IndexedContainerElementType>
    struct VectorIndexedContainer : public std::vector<VectorElementType>
        , public IndexedContainer<IndexedContainerElementType>
    {
        using std::vector<VectorElementType>::vector;

        /// <summary> Returns the number of elements in the container </summary>
        size_t Size() const override { return this->size(); };

        /// <summary> Gets a reference to the elements that corresponds to a given index. </summary>
        IndexedContainerElementType Get(size_t index) const override;
    };
} // namespace optimization
} // namespace ell

#pragma region implementation

namespace ell
{
namespace optimization
{
    template <typename VectorElementType, typename IndexedContainerElementType>
    IndexedContainerElementType VectorIndexedContainer<VectorElementType, IndexedContainerElementType>::Get(size_t index) const
    {
        return IndexedContainerElementType((*this)[index]);
    }
} // namespace optimization
} // namespace ell

#pragma endregion implementation
