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


// stl
#include <string>
#include <iostream>

namespace ell
{
namespace common
{
    /// <summary> Gets a data iterator from an input stream. </summary>
    ///
    /// <param name="stream"> Input stream to load data from. </param>
    ///
    /// <returns> The data iterator. </returns>
    data::AutoSupervisedExampleIterator GetExampleIterator(std::istream& stream);

    /// <summary> Gets a dataset from data load arguments. </summary>
    ///
    /// <typeparam name="DatasetType"> Dataset type. </typeparam>
    /// <param name="stream"> Input stream to load data from. </param>
    ///
    /// <returns> The dataset. </returns>
    data::AutoSupervisedDataset GetDataset(std::istream& stream);

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
    /// Gets a dataset by loading it from an input stream and then running it through a map.
    /// </summary>
    ///
    /// <typeparam name="DatasetType"> The Dataset type. </typeparam>
    /// <param name="stream"> Input stream to load data from. </param>
    /// <param name="map"> The map. </param>
    ///
    /// <returns> The dataset. </returns>
    template <typename MapType>
    data::AutoSupervisedDataset GetMappedDataset(std::istream& stream, const MapType& map);
}
}

#include "../tcc/DataLoaders.tcc"
