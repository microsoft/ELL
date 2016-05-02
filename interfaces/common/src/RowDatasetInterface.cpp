////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     RowDatasetInterface.cpp (interfaces)
//  Authors:  Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "RowDatasetInterface.h"

#include <string>

namespace interfaces
{
    //
    // RowDataset
    //
    RowDataset::RowDataset(dataset::RowDataset<> dataset) : _dataset(std::move(dataset))
    {
    }


    uint64_t RowDataset::NumExamples() const
    {
        return _dataset.NumExamples();
    }

    uint64_t RowDataset::GetMaxExampleSize() const
    {
        return _dataset.GetMaxExampleSize();
    }

    dataset::SupervisedExample RowDataset::GetExample(uint64_t index) const
    {
        return dataset::SupervisedExample(_dataset.GetExample(index));
    }

    utilities::AnyIterator<dataset::SupervisedExample> RowDataset::GetIterator(uint64_t firstExample, uint64_t numExamples) const
    {
        auto it = _dataset.GetIterator(firstExample, numExamples);
        return utilities::MakeAnyIterator(std::move(it));
    }

    void RowDataset::AddExample(dataset::SupervisedExample&& example)
    {
        _dataset.AddExample(std::move(example));
    }

    void RowDataset::RandPerm(std::default_random_engine& rng)
    {
        _dataset.RandPerm(rng);
    }

    void RowDataset::RandPerm(std::default_random_engine& rng, uint64_t count)
    {
        _dataset.RandPerm(rng, count);
    }
}