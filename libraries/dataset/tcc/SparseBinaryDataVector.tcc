////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SparseBinaryDataVector.tcc (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace emll
{
namespace dataset
{
    template <typename IntegerListType>
    template <typename IndexValueIteratorType, typename concept>
    SparseBinaryDataVectorBase<IntegerListType>::SparseBinaryDataVectorBase(IndexValueIteratorType indexValueIterator)
    {
        while (indexValueIterator.IsValid())
        {
            auto indexValue = indexValueIterator.Get();
            SparseBinaryDataVectorBase<IntegerListType>::AppendEntry(indexValue.index, indexValue.value); // explicit call to SparseBinaryDataVectorBase<ValueType>::AppendEntry is given to avoid virtual function call in Ctor
            indexValueIterator.Next();
        }
    }

    template <typename IntegerListType>
    std::vector<double> SparseBinaryDataVectorBase<IntegerListType>::ToDoubleArray() const
    {
        auto vector = std::vector<double>(Size());
        auto indexIterator = _indices.GetIterator();

        while (indexIterator.IsValid())
        {
            vector[indexIterator.Get()] = 1.0;
            indexIterator.Next();
        }

        return vector;
    }
}
}
