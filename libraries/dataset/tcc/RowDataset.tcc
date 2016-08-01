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
#include "..\include\RowDataset.h"

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
    ExampleType& RowDataset<ExampleType>::GetExample(uint64_t index)
    {
        return _examples[index];
    }

    template<typename ExampleType>
    const ExampleType& RowDataset<ExampleType>::GetExample(uint64_t index) const
    {
        return _examples[index];
    }

    template<typename ExampleType>
    ExampleType& RowDataset<ExampleType>::operator[](uint64_t index)
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
    void RowDataset<ExampleType>::RandomPermute(std::default_random_engine& rng, uint64_t prefixSize, uint64_t rangeFirstIndex, uint64_t rangeSize)
    {
        using std::swap;
        if(rangeSize == 0) rangeSize = NumExamples();
        uint64_t lastIndex = rangeFirstIndex + rangeSize - 1;

        for (uint64_t i = rangeFirstIndex; i < rangeFirstIndex + prefixSize; ++i)
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

        std::sort(_examples.begin() + fromRowIndex, 
        _examples.begin() + fromRowIndex + size, 
        [&](const ExampleType& a, const ExampleType& b) -> bool 
        {
            return sortKey(a) < sortKey(b);
        });
    }

    template<typename ExampleType>
    template<typename PartitionKeyType>
    void RowDataset<ExampleType>::Partition(PartitionKeyType partitionKey, uint64_t fromRowIndex, uint64_t size)
    {
        size = CorrectRangeSize(fromRowIndex, size);
        std::partition(_examples.begin() + fromRowIndex, _examples.begin() + fromRowIndex + size, partitionKey);
    }

    template<typename ExampleType>
    void RowDataset<ExampleType>::Print(std::ostream& os, size_t tabs, uint64_t fromRowIndex, uint64_t size) const
    {
        size = CorrectRangeSize(fromRowIndex, size);

        for(uint64_t index = fromRowIndex; index < fromRowIndex + size; ++index)
        {
            os << std::string(tabs * 4, ' ');
            _examples[index].Print(os);
            os << "\n";
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

