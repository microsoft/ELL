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

namespace emll
{
namespace interfaces
{
    //
    // RowDataset
    //
    GenericRowDataset::GenericRowDataset(const GenericRowDataset& other)
        : _dataset(other._dataset)
    {
    }

    GenericRowDataset::GenericRowDataset(dataset::GenericRowDataset&& dataset)
        : _dataset(std::move(dataset))
    {
    }

    uint64_t GenericRowDataset::NumExamples() const
    {
        return _dataset.NumExamples();
    }

    uint64_t GenericRowDataset::GetMaxDataVectorSize() const
    {
        return _dataset.GetMaxDataVectorSize();
    }

    dataset::GenericSupervisedExample GenericRowDataset::GetExample(uint64_t index) const
    {
        if (index >= NumExamples())
        {
            throw std::runtime_error("Out of bounds");
        }

        std::cout << "Dataset size: " << NumExamples() << std::endl;
        std::cout << "Dataset[" << index << "]: ";
        auto result = static_cast<dataset::GenericSupervisedExample>(_dataset.GetExample(index));
        result.Print(std::cout);
        std::cout << std::endl;
        return static_cast<dataset::GenericSupervisedExample>(_dataset.GetExample(index));
    }

    dataset::DenseSupervisedExample GenericRowDataset::GetDenseSupervisedExample(uint64_t index) const
    {
        if (index >= NumExamples())
        {
            throw std::runtime_error("Out of bounds");
        }

        const auto& example = _dataset.GetExample(index); // GenericSupervisedExample
        const auto& exampleData = example.GetDataVector();
        auto exampleDataArray = exampleData.ToArray();
        auto resultData = std::make_shared<dataset::DoubleDataVector>(exampleDataArray);
        dataset::DenseSupervisedExample result(resultData, example.GetMetadata());
        return result;
    }

    dataset::GenericRowIterator GenericRowDataset::GetIterator(uint64_t firstExample, uint64_t numExamples) const
    {
        return _dataset.GetIterator(firstExample, numExamples);
    }

    void GenericRowDataset::AddExample(dataset::GenericSupervisedExample&& example)
    {
        _dataset.AddExample(std::move(example));
    }

    void GenericRowDataset::RandomPermute(std::default_random_engine& rng)
    {
        _dataset.RandomPermute(rng);
    }

    void GenericRowDataset::RandomPermute(std::default_random_engine& rng, uint64_t count)
    {
        _dataset.RandomPermute(rng, count);
    }
}
}
