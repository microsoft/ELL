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

// data
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
    std::unique_ptr<data::IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments)
    {
        // create parser for sparse vectors (SVMLight format)
        data::SparseEntryParser sparseEntryParser;

        // create line iterator - read line by line sequentially
        data::SequentialLineIterator lineIterator(dataLoadArguments.inputDataFilename);

        // Create iterator
        return data::GetParsingIterator(std::move(lineIterator), std::move(sparseEntryParser));
    }

    data::AutoSupervisedDataset GetRowDataset(const DataLoadArguments& dataLoadArguments)
    {
        auto dataIterator = GetDataIterator(dataLoadArguments);
        data::AutoSupervisedDataset rowDataset;
        while (dataIterator->IsValid())
        {
            rowDataset.AddExample(dataIterator->Get());
            dataIterator->Next();
        }

        return rowDataset;
    }
}
}
