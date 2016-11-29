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

// model
#include "DynamicMap.h"

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

    /// <summary>
    /// Gets a dataset by loading it according to data load arguments and then running it through a
    /// map.
    /// </summary>
    ///
    /// <typeparam name="DatasetType"> The Dataset type. </typeparam>
    /// <param name="dataLoadArguments"> The data load arguments. </param>
    /// <param name="map"> The map. </param>
    ///
    /// <returns> The dataset. </returns>
    template <typename DatasetType = data::AutoSupervisedDataset>
    DatasetType GetMappedDataset(const DataLoadArguments& dataLoadArguments, const model::DynamicMap& map);
}
}

#include "../tcc/DataLoaders.tcc"
