////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     DoubleVector.tcc (linear)
//  Authors:  Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <functional>

namespace linear
{
    inline DoubleVector::DoubleVector(const std::vector<double>& v) : _data(v) {};

    inline DoubleVector::DoubleVector(std::vector<double>&& v) : _data(std::forward<std::vector<double>>(v)) {};

    inline DoubleVector::operator std::vector<double>() const &
    {
        return _data;
    }
    
    inline DoubleVector::operator std::vector<double> & () &
    {
        return std::ref(_data);
    };

    inline DoubleVector::operator std::vector<double> const & () const &
    {
        return std::cref(_data);
    };

    inline DoubleVector::operator std::vector<double> && () &&
    {
        return std::move(_data);
    };

    template<typename IndexValueIteratorType, IsIndexValueIterator<IndexValueIteratorType> concept>
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


    inline double& DoubleVector::operator[](uint64 index)
    {
        return _data[index]; 
    }
    
    inline double DoubleVector::operator[](uint64 index) const
    {
        return _data[index];
    }
    
    inline double* DoubleVector::GetDataPointer()
    {
        return _data.data();
    }
    
    inline const double* DoubleVector::GetDataPointer() const
    {
        return _data.data();
    }

    inline uint64 DoubleVector::Size() const
    {
        return _data.size();
    }

}

