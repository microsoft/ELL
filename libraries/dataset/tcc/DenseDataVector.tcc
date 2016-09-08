////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DenseDataVector.tcc (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace dataset
{
    template <typename ValueType>
    template <typename IndexValueIteratorType, typename concept>
    DenseDataVector<ValueType>::DenseDataVector(IndexValueIteratorType indexValueIterator)
    {
        while (indexValueIterator.IsValid())
        {
            auto indexValue = indexValueIterator.Get();
            DenseDataVector<ValueType>::AppendEntry(indexValue.index, indexValue.value); // explicit call to DenseDataVector<ValueType>::AppendEntry is given to avoid virtual function call in Ctor
            indexValueIterator.Next();
        }
    }

    template <typename ValueType>
    std::unique_ptr<IDataVector> DenseDataVector<ValueType>::Clone() const
    {
        DenseDataVector<ValueType> result = *this;
        return std::make_unique<DenseDataVector<ValueType>>(std::move(result));
    }

    template <typename ValueType>
    std::vector<double> DenseDataVector<ValueType>::ToArray() const
    {
        auto vector = std::vector<double>(Size());
        std::copy(_data.cbegin(), _data.cend(), vector.begin());
        return vector;
    }
}
