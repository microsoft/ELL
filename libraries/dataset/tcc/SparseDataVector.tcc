////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SparseDataVector.tcc (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace emll
{
namespace dataset
{
    template <typename ElementType, typename IntegerListType>
    template <typename IndexValueIteratorType, typename concept>
    SparseDataVector<ElementType, IntegerListType>::SparseDataVector(IndexValueIteratorType indexValueIterator)
    {
        while (indexValueIterator.IsValid())
        {
            auto indexValue = indexValueIterator.Get();
            SparseDataVector<ElementType, IntegerListType>::AppendEntry(indexValue.index, indexValue.value); // explicit call to SparseDataVector<ElementType>::AppendEntry is given to avoid virtual function call in Ctor
            indexValueIterator.Next();
        }
    }

    template <typename ElementType, typename IntegerListType>
    std::vector<double> SparseDataVector<ElementType, IntegerListType>::ToArray() const
    {
        auto vector = std::vector<double>(Size());
        auto indexValueIterator = GetIterator();

        while (indexValueIterator.IsValid())
        {
            auto indexValue = indexValueIterator.Get();
            vector[indexValue.index] = indexValue.value;
            indexValueIterator.Next();
        }

        return vector;
    }
}
}
