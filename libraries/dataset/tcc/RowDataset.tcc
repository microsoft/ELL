////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     RowDataset.tcc (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// stl
#include <stdexcept>
#include <random>
#include <cassert>
#include <algorithm>

namespace dataset
{
    template<typename ExampleType>
    RowDataset<ExampleType>::RowDataset(Iterator exampleIterator)
    {
        while(exampleIterator.IsValid())
        {
            AddExample(ExampleType(exampleIterator.Get()));
            exampleIterator.Next();
        }
    }

    template<typename ExampleType>
    const ExampleType& RowDataset<ExampleType>::GetExample(uint64_t index) const
    {
        return _examples[index];
    }

    template<typename ExampleType>
    const ExampleType& RowDataset<ExampleType>::operator[](uint64_t index) const
    {
        return _examples[index];
    }

    template<typename ExampleType>
    typename RowDataset<ExampleType>::Iterator RowDataset<ExampleType>::GetIterator(uint64_t fromRowIndex, uint64_t size) const
    {
        size = CorrectRangeSize(fromRowIndex, size);
        return utilities::MakeStlIterator(_examples.cbegin() + fromRowIndex, _examples.cbegin() + fromRowIndex + size);
    }

    template<typename ExampleType>
    void RowDataset<ExampleType>::AddExample(ExampleType example)
    {
        uint64_t size = example.GetDataVector().Size();
        _examples.push_back(std::move(example));

        if (_maxExampleSize < size)
        {
            _maxExampleSize = size;
        }
    }

    template<typename ExampleType>
    void RowDataset<ExampleType>::Reset()
    {
        _examples.clear();
        _maxExampleSize = 0;
    }

    template<typename ExampleType>
    void RowDataset<ExampleType>::RandomPermute(std::default_random_engine& rng)
    {
        RandomPermute(rng, NumExamples());
    }

    template<typename ExampleType>
    void RowDataset<ExampleType>::RandomPermute(std::default_random_engine& rng, uint64_t count)
    {
        using std::swap;
        uint64_t lastIndex = NumExamples() - 1;

        for (uint64_t i = 0; i < count; ++i)
        {
            std::uniform_int_distribution<uint64_t> dist(i, lastIndex);
            uint64_t j = dist(rng);
            swap(_examples[i], _examples[j]);
        }
    }

    template<typename ExampleType>
    template<typename SortKeyType>
    void RowDataset<ExampleType>::Sort(SortKeyType sortKey, uint64_t fromRowIndex, uint64_t size)
    {
        size = CorrectRangeSize(fromRowIndex, size);

        if(size <= 1)
        {
            return;
        }

        std::sort(_examples.begin() + fromRowIndex, 
        _examples.begin() + fromRowIndex + size, 
        [&](const ExampleType& a, const ExampleType& b) -> bool 
        {
            return sortKey(a) < sortKey(b);
        });
    }

    template<typename ExampleType>
    void RowDataset<ExampleType>::Print(std::ostream& os, uint64_t fromRowIndex, uint64_t size) const
    {
        size = CorrectRangeSize(fromRowIndex, size);

        for(uint64_t index = fromRowIndex; index < fromRowIndex + size; ++index)
        {
            os << _examples[index] << std::endl;
        }
    }

    template<typename ExampleType>
    std::ostream& operator<<(std::ostream& os, RowDataset<ExampleType>& dataset)
    {
        dataset.Print(os);
        return os;
    }

    template<typename ExampleType>
    uint64_t RowDataset<ExampleType>::CorrectRangeSize(uint64_t fromRowIndex, uint64_t size) const {
        if(size == 0 || fromRowIndex + size > _examples.size())
        {
            return _examples.size() - fromRowIndex;
        }
        return size;
    }
}

