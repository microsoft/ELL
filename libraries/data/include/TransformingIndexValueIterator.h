////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     TransformingIndexValueIterator.h (data)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IndexValue.h"

// stl
#include <memory>

namespace emll
{
namespace data
{
    /// <summary> An index-value iterator that wraps another index-value iterator and applies a
    /// transformation to each of its non-zero elements. </summary>
    ///
    /// <typeparam name="WrappedIndexValueIteratorType"> Type of the wrapped index value iterator. </typeparam>
    /// <typeparam name="TransformType"> A functor type that takes an IndexValue and returns a double. </typeparam>
    template <typename WrappedIndexValueIteratorType, typename TransformType>
    class TransformingIndexValueIterator : public IIndexValueIterator
    {
    public:
        /// <summary> Constructs an instance of TransformingIndexValueIterator. </summary>
        ///
        /// <param name="wrappedIterator"> The index value iterator whose non-zero values are to be transformed. </param>
        /// <param name="transform"> A transform that takes an IndexValue and returns the transformed double value. </param>
        TransformingIndexValueIterator(WrappedIndexValueIteratorType wrappedIterator, TransformType transform) : _wrappedIterator(std::move(wrappedIterator)), _transform(std::move(transform)) {}

        /// <summary> Returns True if the iterator is currently pointing to a valid iterate. </summary>
        ///
        /// <returns> True if the iterator is currently pointing to a valid iterate. </returns>
        bool IsValid() const { return _wrappedIterator.IsValid(); }

        /// <summary> Proceeds to the Next iterate </summary>
        void Next() { _wrappedIterator.Next(); }

        /// <summary> Returns The current index-value pair </summary>
        ///
        /// <returns> The current index-value pair </returns>
        IndexValue Get() const 
        {
            auto indexValue = _wrappedIterator.Get();
            return{ indexValue.index, _transform(indexValue) };
        }

    protected:
        WrappedIndexValueIteratorType _wrappedIterator;
        TransformType _transform;
    };

    /// <summary> Creates an TransformingIndexValueIterator.</summary>
    ///
    /// <param name="wrappedIterator"> STL iterator pointing at beginning of range to iterate over. </param>
    /// <param name="transform"> STL iterator pointing at end of range to iterate over. </param>
    template <typename WrappedIndexValueIteratorType, typename TransformType>
    TransformingIndexValueIterator<WrappedIndexValueIteratorType, TransformType> MakeTransformingIndexValueIterator(WrappedIndexValueIteratorType wrappedIterator, TransformType transform)
    {
        return TransformingIndexValueIterator<WrappedIndexValueIteratorType, TransformType>(std::move(wrappedIterator), std::move(transform));
    }
}
}
