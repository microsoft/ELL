////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     RowDataset.cpp (dataset)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "RowDataset.h"

// stl
#include <stdexcept>
#include <random>
#include <cassert>

namespace dataset
{
    uint64 RowDataset::NumExamples() const
    {
        return _examples.size();
    }

    uint64 RowDataset::GetMaxExampleSize() const
    {
        return _maxExampleSize;
    }

    const SupervisedExample& RowDataset::GetExample(uint64 index) const
    {
        return _examples[index];
    }

    RowDataset::Iterator RowDataset::GetIterator(uint64 firstExample, uint64 numExamples) const
    {
        if (firstExample >= NumExamples())
        {
            return utilities::MakeStlIterator(_examples.cend(), _examples.cend());
        }

        uint64 lastExample = firstExample + numExamples;
        if(lastExample > NumExamples() || numExamples == 0)
        {
            lastExample = NumExamples();
        }

        return utilities::MakeStlIterator(_examples.cbegin() + firstExample, _examples.cbegin() + lastExample);
    }

    void RowDataset::AddExample(SupervisedExample&& example)
    {
        uint64 size = example.Size();
        _examples.push_back(std::move(example));

        if(_maxExampleSize < size)
        {
            _maxExampleSize = size;
        }
    }

    void RowDataset::RandPerm(std::default_random_engine& rng)
    {
        RandPerm(rng, NumExamples());
    }

    void RowDataset::RandPerm(std::default_random_engine& rng, uint64 count)
    {
        uint64 lastIndex = NumExamples()-1;

        for(uint64 i = 0; i < count; ++i)
        {
            std::uniform_int_distribution<uint64> dist(i, lastIndex);
            uint64 j = dist(rng);
            std::swap(_examples[i], _examples[j]);
        }
    }
}

