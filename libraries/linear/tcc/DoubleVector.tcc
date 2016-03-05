////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     DoubleVector.tcc (linear)
//  Authors:  Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

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
    
    inline DoubleVector::operator std::vector<double> && () &&
    {
        return std::move(_data);
    };

    template<typename IndexValueIteratorType, typename concept>
    DoubleVector::DoubleVector(IndexValueIteratorType indexValueIterator)
    {
        while (indexValueIterator.IsValid())
        {
            auto entry = indexValueIterator.Get();
            _data.resize(entry.index + 1);
            _data[entry.index] = entry.value;
            indexValueIterator.Next();
        }
    }
}

