////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DataLoaders.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DataLoadArguments.h"

// data
#include "Dataset.h"
#include "ParsingExampleIterator.h"

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
    std::unique_ptr<data::IParsingExampleIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments);

    /// <summary> Gets a dataset from data load arguments. </summary>
    ///
    /// <typeparam name="DatasetType"> Dataset type. </typeparam>
    /// <param name="dataLoadArguments"> The data load arguments. </param>
    ///
    /// <returns> The dataset. </returns>
    template <typename DatasetType = data::AutoSupervisedDataset>
    DatasetType GetDataset(const DataLoadArguments& dataLoadArguments);
}
}

#include "../tcc/DataLoaders.tcc"