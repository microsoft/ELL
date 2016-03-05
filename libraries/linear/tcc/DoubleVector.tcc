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

