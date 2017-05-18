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
    template <typename WrappedIndexValueIteratorType, typename TransformationType>
    TransformingIndexValueIterator<WrappedIndexValueIteratorType, TransformationType>::TransformingIndexValueIterator(WrappedIndexValueIteratorType wrappedIterator, TransformationType transform)
        : _wrappedIterator(std::move(wrappedIterator)), _transform(std::move(transform))
    {
    }

    template <typename WrappedIndexValueIteratorType, typename TransformationType>
    IndexValue TransformingIndexValueIterator<WrappedIndexValueIteratorType, TransformationType>::Get() const
    {
        auto indexValue = _wrappedIterator.Get();
        return { indexValue.index, _transform(indexValue) };
    }
}
}