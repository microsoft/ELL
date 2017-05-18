////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DataLoaders.cpp (common)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// #include "stdafx.h"

#include "DataLoaders.h"

// utilities
#include "Files.h"

// data
#include "Dataset.h"
#include "ParsingExampleIterator.h"
#include "SequentialLineIterator.h"
#include "SparseEntryParser.h"

// model
#include "DynamicMap.h"

// stl
#include <memory>
#include <stdexcept>

namespace ell
{
namespace common
{
    data::AutoSupervisedExampleIterator GetExampleIterator(const DataLoadArguments& dataLoadArguments)
    {
        // create parser for sparse vectors (SVMLight format)
        data::SparseEntryParser sparseEntryParser;

        // create line iterator - read line by line sequentially
        data::SequentialLineIterator lineIterator(dataLoadArguments.inputDataFilename);

        // Create iterator
        return data::GetParsingExampleIterator(std::move(lineIterator), std::move(sparseEntryParser));
    }

    data::AutoSupervisedDataset GetDataset(data::AutoSupervisedExampleIterator exampleIterator)
    {
        data::AutoSupervisedDataset dataset;
        while (exampleIterator.IsValid())
        {
            dataset.AddExample(exampleIterator.Get());
            exampleIterator.Next();
        }

        return dataset;
    }

    data::AutoSupervisedDataset GetDataset(const DataLoadArguments& dataLoadArguments)
    {
        return GetDataset(GetExampleIterator(dataLoadArguments));
    }
}
}
