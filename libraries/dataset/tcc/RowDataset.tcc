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
    template<typename DataVectorType>
    RowDataset<DataVectorType>::RowDataset(Iterator exampleIterator)
    {
        while(exampleIterator.IsValid())
        {
            AddExample(SupervisedExample<DataVectorType>(exampleIterator.Get()));
            exampleIterator.Next();
        }
    }

    template<typename DataVectorType>
    const SupervisedExample<DataVectorType>& RowDataset<DataVectorType>::GetExample(uint64_t index) const
    {
        return _examples[index];
    }

    template<typename DataVectorType>
    const SupervisedExample<DataVectorType>& RowDataset<DataVectorType>::operator[](uint64_t index) const
    {
        return _examples[index];
    }

    template<typename DataVectorType>
    typename RowDataset<DataVectorType>::Iterator RowDataset<DataVectorType>::GetIterator(uint64_t fromRowIndex, uint64_t size) const
    {
        assert(fromRowIndex + size <= _examples.size());

        if (size == 0)
        {
            size = _examples.size() - fromRowIndex;
        }

        return utilities::MakeStlIterator(_examples.cbegin() + fromRowIndex, _examples.cbegin() + fromRowIndex + size);
    }

    template<typename DataVectorType>
    void RowDataset<DataVectorType>::AddExample(ExampleType&& example)
    {
        uint64_t size = example.GetDataVector().Size();
        _examples.push_back(std::move(example));

        if (_maxExampleSize < size)
        {
            _maxExampleSize = size;
        }
    }

    template<typename DataVectorType>
    void RowDataset<DataVectorType>::Reset()
    {
        _examples.clear();
        _maxExampleSize = 0;
    }

    template<typename DataVectorType>
    void RowDataset<DataVectorType>::RandomPermute(std::default_random_engine& rng)
    {
        RandomPermute(rng, NumExamples());
    }

    template<typename DataVectorType>
    void RowDataset<DataVectorType>::RandomPermute(std::default_random_engine& rng, uint64_t count)
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

    template<typename DataVectorType>
    template<typename SortKeyType>
    void RowDataset<DataVectorType>::Sort(SortKeyType sortKey, uint64_t fromRowIndex, uint64_t size)
    {

        assert(fromRowIndex + size <= _examples.size());

        if (size == 0)
        {
            size = _examples.size() - fromRowIndex;
        }
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

    template<typename DataVectorType>
    void RowDataset<DataVectorType>::Print(std::ostream& os) const
    {
        for(const auto& example : _examples)
        {
            os << example << std::endl;
        }
    }

    template<typename DataVectorType>
    std::ostream& operator<<(std::ostream& os, RowDataset<DataVectorType>& dataset)
    {
        dataset.Print(os);
        return os;
    }
}

