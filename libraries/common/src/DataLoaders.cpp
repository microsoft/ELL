////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DataLoaders.cpp (common)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

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
