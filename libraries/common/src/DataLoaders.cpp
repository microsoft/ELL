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
#include "ParsingExampleIterator.h"
#include "Dataset.h"
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
    std::unique_ptr<data::IParsingExampleIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments)
    {
        // create parser for sparse vectors (SVMLight format)
        data::SparseEntryParser sparseEntryParser;

        // create line iterator - read line by line sequentially
        data::SequentialLineIterator lineIterator(dataLoadArguments.inputDataFilename);

        // Create iterator
        return data::GetParsingExampleIterator(std::move(lineIterator), std::move(sparseEntryParser));
    }
}
}
