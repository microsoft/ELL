////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DataLoaders.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DataLoadArguments.h"

// dataset
#include "RowDataset.h"

// dataset
#include "ParsingIterator.h"

// stl
#include <string>

namespace emll
{
namespace common
{
    /// <summary> Gets a data iterator from a data file, based on data load arguments. </summary>
    ///
    /// <param name="dataLoadArguments"> The data load arguments. </param>
    ///
    /// <returns> The data iterator. </returns>
    std::unique_ptr<dataset::IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments);

    /// <summary> Gets a row dataset from data load arguments. </summary>
    ///
    /// <param name="dataLoadArguments"> The data load arguments. </param>
    ///
    /// <returns> The row dataset. </returns>
    dataset::GenericRowDataset GetRowDataset(const DataLoadArguments& dataLoadArguments);
}
}
