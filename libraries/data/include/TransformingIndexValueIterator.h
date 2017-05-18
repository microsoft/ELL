////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TransformingIndexValueIterator.h (data)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IndexValue.h"

// stl
#include <memory>

namespace ell
{
namespace data
{
    /// <summary> An index-value iterator that wraps another index-value iterator and applies a
    /// transformation to each of its non-zero elements. </summary>
    ///
    /// <typeparam name="WrappedIndexValueIteratorType"> Type of the wrapped index value iterator. </typeparam>
    /// <typeparam name="TransformationType"> A functor type that takes an IndexValue and returns a double. </typeparam>
    template <typename WrappedIndexValueIteratorType, typename TransformationType>
    class TransformingIndexValueIterator : public IIndexValueIterator
    {
    public:
        /// <summary> Constructs an instance of TransformingIndexValueIterator. </summary>
        ///
        /// <param name="wrappedIterator"> The index value iterator whose non-zero values are to be transformed. </param>
        /// <param name="transform"> A transform that takes an IndexValue and returns the transformed double value. </param>
        TransformingIndexValueIterator(WrappedIndexValueIteratorType wrappedIterator, TransformationType transform);

        /// <summary> Returns True if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> True if the iterator is currently pointing to a valid iterate. </returns>
        bool IsValid() const { return _wrappedIterator.IsValid(); }

        /// <summary> Proceeds to the Next iterate </summary>
        void Next() { _wrappedIterator.Next(); }

        /// <summary> Returns The current index-value pair </summary>
        ///
        /// <returns> The current index-value pair </returns>
        IndexValue Get() const;

    protected:
        WrappedIndexValueIteratorType _wrappedIterator;
        TransformationType _transform;
    };

    /// <summary> Creates an TransformingIndexValueIterator.</summary>
    ///
    /// <param name="wrappedIterator"> STL iterator pointing at beginning of range to iterate over. </param>
    /// <param name="transform"> STL iterator pointing at end of range to iterate over. </param>
    template <typename WrappedIndexValueIteratorType, typename TransformationType>
    TransformingIndexValueIterator<WrappedIndexValueIteratorType, TransformationType> MakeTransformingIndexValueIterator(WrappedIndexValueIteratorType wrappedIterator, TransformationType transform)
    {
        return TransformingIndexValueIterator<WrappedIndexValueIteratorType, TransformationType>(std::move(wrappedIterator), std::move(transform));
    }
}
}

#include "../tcc/TransformingIndexValueIterator.tcc"