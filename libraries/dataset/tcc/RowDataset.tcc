////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     RowDataset.tcc (dataset)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// stl
#include <stdexcept>
#include <random>
#include <cassert>
#include "..\include\RowDataset.h"

namespace dataset
{
    template<typename RowType>
    uint64_t RowDataset<RowType>::NumExamples() const
    {
        return _examples.size();
    }

    template<typename RowType>
    uint64_t RowDataset<RowType>::GetMaxExampleSize() const
    {
        return _maxExampleSize;
    }

    template<typename RowType>
    const RowType& RowDataset<RowType>::GetExample(uint64_t index) const
    {
        return _examples[index];
    }

    template<typename RowType>
    const RowType & RowDataset<RowType>::operator[](uint64_t index) const
    {
        return _examples[index];
    }

    template<typename RowType>
    typename RowDataset<RowType>::Iterator RowDataset<RowType>::GetIterator(uint64_t firstExample, uint64_t numExamples) const
    {
        if (firstExample >= NumExamples())
        {
            return utilities::MakeStlIterator(_examples.cend(), _examples.cend());
        }

        uint64_t lastExample = firstExample + numExamples;
        if(lastExample > NumExamples() || numExamples == 0)
        {
            lastExample = NumExamples();
        }

        return utilities::MakeStlIterator(_examples.cbegin() + firstExample, _examples.cbegin() + lastExample);
    }

    template<typename RowType>
    void RowDataset<RowType>::AddExample(RowType example)
    {
        uint64_t size = example.GetDataVector().Size();
        _examples.push_back(std::move(example));

        if(_maxExampleSize < size)
        {
            _maxExampleSize = size;
        }
    }

    template<typename RowType>
    void RowDataset<RowType>::RandPerm(std::default_random_engine& rng)
    {
        RandPerm(rng, NumExamples());
    }

    template<typename RowType>
    void RowDataset<RowType>::RandPerm(std::default_random_engine& rng, uint64_t count)
    {
        uint64_t lastIndex = NumExamples()-1;

        for(uint64_t i = 0; i < count; ++i)
        {
            std::uniform_int_distribution<uint64_t> dist(i, lastIndex);
            uint64_t j = dist(rng);
            std::swap(_examples[i], _examples[j]);
        }
    }
}

