////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DenseDataVector.cpp (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DenseDataVector.h"

// stl
#include <cassert>

namespace emll
{
namespace dataset
{

    template <typename ValueType>
    DenseDataVector<ValueType>::DenseDataVector()
        : _numNonzeros(0)
    {
        _data.reserve(DEFAULT_DENSE_VECTOR_CAPACITY);
    }

    template <typename ValueType>
    DenseDataVector<ValueType>::DenseDataVector(const IDataVector& dataVector)
        : DenseDataVector(dataVector.ToArray())
        {
        }

    // template <typename ValueType>
    // DenseDataVector<ValueType>::DenseDataVector(std::vector<ValueType> data)
    //     : _numNonzeros(0), _data(std::move(data))
    // {
    //     for (auto value : _data)
    //     {
    //         if (value != 0)
    //         {
    //             ++_numNonzeros;
    //         }
    //     }
    // }

    template <typename ValueType>
    DenseDataVector<ValueType>::DenseDataVector(std::vector<double> data)
        : _numNonzeros(0)
    {
        auto size = data.size();
        _data.resize(size);
        for(size_t index = 0; index < size; ++index)
        {
            auto value = static_cast<ValueType>(data[index]);
            _data[index++] = value;
            if (value != 0)
            {
                ++_numNonzeros;
            }
        }
    }

    template <typename ValueType>
    double DenseDataVector<ValueType>::operator[](uint64_t index) const
    {
        if (index >= _data.size())
        {
            return 0.0;
        }
        return (double)_data[index];
    }

    template <typename ValueType>
    void DenseDataVector<ValueType>::AppendEntry(uint64_t index, double value)
    {
        if (value == 0)
        {
            return;
        }

        assert(index >= Size());

        _data.resize(index + 1);
        _data[index] = (ValueType)value;
        ++_numNonzeros;
    }

    template <typename ValueType>
    void DenseDataVector<ValueType>::Reset()
    {
        _data.resize(0);
        _numNonzeros = 0;
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
    void DenseDataVector<ValueType>::AddTo(double* p_other, double scalar) const
    {
        for (uint64_t i = 0; i < Size(); ++i)
        {
            p_other[i] += (double)(scalar * _data[i]);
        }
    }

    template <typename ValueType>
    double DenseDataVector<ValueType>::Dot(const double* p_other) const
    {
        double result = 0.0;
        for (uint64_t i = 0; i < Size(); ++i)
        {
            result += _data[i] * p_other[i];
        }

        return result;
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

    template class DenseDataVector<float>;
    template class DenseDataVector<double>;
}
}
