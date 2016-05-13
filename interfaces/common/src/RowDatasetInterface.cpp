////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     RowDatasetInterface.cpp (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "RowDatasetInterface.h"

#include "RowDataset.h"
#include <string>

namespace interfaces
{
    //
    // RowDataset
    //
    RowDataset::RowDataset(dataset::RowDataset<dataset::IDataVector> dataset) : _dataset(std::move(dataset))
    {
    }

    uint64_t RowDataset::NumExamples() const
    {
        return _dataset.NumExamples();
    }

    uint64_t RowDataset::GetMaxDataVectorSize() const
    {
        return _dataset.GetMaxDataVectorSize();
    }

    dataset::GenericSupervisedExample RowDataset::GetExample(uint64_t index) const
    {
        return dataset::GenericSupervisedExample(_dataset.GetExample(index));
    }

    RowDataset::Iterator RowDataset::GetIterator(uint64_t firstExample, uint64_t numExamples) const
    {
        return _dataset.GetIterator(firstExample, numExamples);
    }

    void RowDataset::AddExample(dataset::GenericSupervisedExample&& example)
    {
        _dataset.AddExample(std::move(example));
    }

    void RowDataset::RandomPermute(std::default_random_engine& rng)
    {
        _dataset.RandomPermute(rng);
    }

    void RowDataset::RandomPermute(std::default_random_engine& rng, uint64_t count)
    {
        _dataset.RandomPermute(rng, count);
    }
}