////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DataLoaders.cpp (common)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataLoaders.h"

// utilities
#include "Files.h"

// dataset
#include "ParsingIterator.h"
#include "RowDataset.h"
#include "SequentialLineIterator.h"
#include "SparseEntryParser.h"

// stl
#include <cstdint>
#include <memory>
#include <stdexcept>

namespace emll
{
namespace common
{
    //
    // Public functions
    //
    std::unique_ptr<dataset::IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments)
    {
        // create parser for sparse vectors (SVMLight format)
        dataset::SparseEntryParser sparseEntryParser;

        // create line iterator - read line by line sequentially
        dataset::SequentialLineIterator lineIterator(dataLoadArguments.inputDataFilename);

        // Create iterator
        return dataset::GetParsingIterator(std::move(lineIterator), std::move(sparseEntryParser));
    }

    dataset::GenericRowDataset GetRowDataset(const DataLoadArguments& dataLoadArguments)
    {
        auto dataIterator = GetDataIterator(dataLoadArguments);
        dataset::GenericRowDataset rowDataset;
        while (dataIterator->IsValid())
        {
            rowDataset.AddExample(dataIterator->Get());
            dataIterator->Next();
        }

        return rowDataset;
    }
}
}
