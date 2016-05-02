////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
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
    uint64_t RowDataset::NumExamples() const
    {
        return _examples.size();
    }

    uint64_t RowDataset::GetMaxExampleSize() const
    {
        return _maxExampleSize;
    }

    const SupervisedExample& RowDataset::GetExample(uint64_t index) const
    {
        return _examples[index];
    }

    RowDataset::Iterator RowDataset::GetIterator(uint64_t firstExample, uint64_t numExamples) const
    {
        if (firstExample >= NumExamples())
        {
            return utilities::MakeStlIterator(_examples.cend(), _examples.cend());
        }

        uint64_t lastExample = firstExample + numExamples;
        if (lastExample > NumExamples() || numExamples == 0)
        {
            lastExample = NumExamples();
        }

        return utilities::MakeStlIterator(_examples.cbegin() + firstExample, _examples.cbegin() + lastExample);
    }

    void RowDataset::AddExample(SupervisedExample&& example)
    {
        uint64_t size = example.GetDataVector().Size();
        _examples.push_back(std::move(example));

        if (_maxExampleSize < size)
        {
            _maxExampleSize = size;
        }
    }

    void RowDataset::RandPerm(std::default_random_engine& rng)
    {
        RandPerm(rng, NumExamples());
    }

    void RowDataset::RandPerm(std::default_random_engine& rng, uint64_t count)
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
}

