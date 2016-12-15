////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TransformingIndexValueIterator.tcc (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace data
{
    template <typename WrappedIndexValueIteratorType, typename TransformType>
    TransformingIndexValueIterator<WrappedIndexValueIteratorType, TransformType>::TransformingIndexValueIterator(WrappedIndexValueIteratorType wrappedIterator, TransformType transform)
        : _wrappedIterator(std::move(wrappedIterator)), _transform(std::move(transform))
    {
    }

    template <typename WrappedIndexValueIteratorType, typename TransformType>
    IndexValue TransformingIndexValueIterator<WrappedIndexValueIteratorType, TransformType>::Get() const
    {
        auto indexValue = _wrappedIterator.Get();
        return { indexValue.index, _transform(indexValue) };
    }
}
}