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
    template<typename IndexValueIteratorType, typename concept>
    DoubleVector::DoubleVector(IndexValueIteratorType indexValueIterator) : types::DoubleArray()
    {
        while (indexValueIterator.IsValid())
        {
            auto entry = indexValueIterator.Get();
            this->resize(entry.index + 1);
            (*this)[entry.index] = entry.value;
            indexValueIterator.Next();
        }
    }
}

