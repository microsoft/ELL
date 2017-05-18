////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DataLoaders.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DataLoadArguments.h"

// data
#include "Dataset.h"
#include "ExampleIterator.h"

// model
#include "DynamicMap.h"

// utilities
#include "StlContainerIterator.h"

// stl
#include <string>

namespace ell
{
namespace common
{
    /// <summary> Gets a data iterator from a data file, based on data load arguments. </summary>
    ///
    /// <param name="dataLoadArguments"> The data load arguments. </param>
    ///
    /// <returns> The data iterator. </returns>
    data::AutoSupervisedExampleIterator GetExampleIterator(const DataLoadArguments& dataLoadArguments);

    /// <summary> Creates a dataset from an example iterator. </summary>
    ///
    /// <param name="exampleIterator"> The example iterator. </param>
    ///
    /// <returns> The dataset. </returns>
    data::AutoSupervisedDataset GetDataset(data::AutoSupervisedExampleIterator exampleIterator);

    /// <summary> Gets a dataset from data load arguments. </summary>
    ///
    /// <param name="dataLoadArguments"> The data load arguments. </param>
    ///
    /// <returns> The dataset. </returns>
    data::AutoSupervisedDataset GetDataset(const DataLoadArguments& dataLoadArguments);

    /// <summary>
    /// Gets a dataset by loading it from an example iterator and running it through a map.
    /// </summary>
    ///
    /// <typeparam name="MapType"> Map type. </typeparam>
    /// <param name="exampleIterator"> The example iterator. </param>
    /// <param name="map"> The map. </param>
    ///
    /// <returns> The mapped dataset. </returns>
    template <typename MapType>
    data::AutoSupervisedDataset GetMappedDataset(data::AutoSupervisedExampleIterator exampleIterator, const MapType& map);

    /// <summary>
    /// Gets a dataset by loading it according to data load arguments and then running it through a
    /// map.
    /// </summary>
    ///
    /// <param name="dataLoadArguments"> The data load arguments. </param>
    /// <param name="map"> The map. </param>
    ///
    /// <returns> The dataset. </returns>
    template <typename MapType>
    data::AutoSupervisedDataset GetMappedDataset(const DataLoadArguments& dataLoadArguments, const MapType& map);
}
}

#include "../tcc/DataLoaders.tcc"
