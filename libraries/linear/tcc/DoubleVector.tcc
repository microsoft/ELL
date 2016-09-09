////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DoubleVector.tcc (linear)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <functional>

namespace emll
{
namespace linear
{
    inline DoubleVector::DoubleVector(const std::vector<double>& v)
        : _data(v){};

    inline DoubleVector::DoubleVector(std::vector<double>&& v)
        : _data(std::forward<std::vector<double>>(v)){};

    template <typename IndexValueIteratorType, linear::IsIndexValueIterator<IndexValueIteratorType> concept>
    inline DoubleVector::DoubleVector(IndexValueIteratorType indexValueIterator)
    {
        while (indexValueIterator.IsValid())
        {
            auto entry = indexValueIterator.Get();
            _data.resize(entry.index + 1);
            _data[entry.index] = entry.value;
            indexValueIterator.Next();
        }
    }

    inline double& DoubleVector::operator[](uint64_t index)
    {
        return _data[index];
    }

    inline double DoubleVector::operator[](uint64_t index) const
    {
        return _data[index];
    }
}
}
