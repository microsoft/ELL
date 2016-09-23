////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DenseDataVector.tcc (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Exception.h"

namespace emll
{
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
    DenseDataVector<ValueType>::DenseDataVector()
        : _numNonzeros(0)
    {
        _data.reserve(DEFAULT_DENSE_VECTOR_CAPACITY);
    }

    template <typename ValueType>
    DenseDataVector<ValueType>::DenseDataVector(std::vector<ValueType> data)
        : _numNonzeros(0), _data(std::move(data))
    {
        for (auto value : _data)
        {
            if (value != 0)
            {
                ++_numNonzeros;
            }
        }
    }

    template <typename ValueType>
    double DenseDataVector<ValueType>::operator[](size_t index) const
    {
        if (index >= _data.size())
        {
            return 0.0;
        }
        return (double)_data[index];
    }

    template <typename ValueType>
    void DenseDataVector<ValueType>::AppendEntry(size_t index, double value)
    {
        if (value == 0)
        {
            return;
        }

        if(index < _data.size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "Can only append values to the end of a data vector");
        }

        _data.resize(index + 1);
        _data[index] = (ValueType)value;
        ++_numNonzeros;
    }

    template <typename ValueType>
    double DenseDataVector<ValueType>::Norm2() const
    {
        double result = 0.0;
        for (double element : _data)
        {
            result += (double)(element * element);
        }
        return result;
    }

    template <typename ValueType>
    double DenseDataVector<ValueType>::Dot(const double* p_other) const
    {
        double result = 0.0;
        for (size_t i = 0; i < Size(); ++i)
        {
            result += _data[i] * p_other[i];
        }

        return result;
    }

    template <typename ValueType>
    void DenseDataVector<ValueType>::AddTo(double* p_other, double scalar) const
    {
        for (size_t i = 0; i < Size(); ++i)
        {
            p_other[i] += (double)(scalar * _data[i]);
        }
    }

    template <typename ValueType>
    std::vector<double> DenseDataVector<ValueType>::ToArray() const
    {
        auto vector = std::vector<double>(Size());
        std::copy(_data.cbegin(), _data.cend(), vector.begin());
        return vector;
    }

    template <typename ValueType>
    void DenseDataVector<ValueType>::Print(std::ostream& os) const
    {
        auto iterator = GetIterator();
        while (iterator.IsValid())
        {
            auto indexValue = iterator.Get();
            os << indexValue.index << ':' << indexValue.value << '\t';
            iterator.Next();
        }
    }
}
}
