////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     DataLoaders.h (common)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "MapLoadArguments.h"
#include "DataLoadArguments.h" 

// dataset
#include "SupervisedExample.h"

// layers
#include "Map.h"
#include "Coordinate.h"

// dataset
#include "ParsingIterator.h"

namespace common
{
    /// <summary> Gets a data iterator from a data file, based on command line parameters. </summary>
    ///
    /// <param name="dataLoadArguments"> The data load arguments. </param>
    ///
    /// <returns> The data iterator. </returns>
    std::unique_ptr<dataset::IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments);

    /// <summary> Gets a (possibly) mapped data iterator, based on command line parameters. </summary>
    ///
    /// <param name="dataLoadArguments"> The data load arguments. </param>
    /// <param name="mapLoadArguments">  The map load arguments. </param>
    ///
    /// <returns> The data iterator. </returns>
    std::unique_ptr<dataset::IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments, const MapLoadArguments& mapLoadArguments);

    // #### document this
    std::shared_ptr<layers::Map> GetMap(const MapLoadArguments& mapLoadArguments);

    // #### document this
    layers::CoordinateList GetInputCoordinates(const layers::Map& map, const MapLoadArguments& mapLoadArguments);

    // #### document this
    dataset::RowDataset LoadDataset(dataset::IParsingIterator& dataIterator);

    /// <summary> Gets a mapped data iterator, based on command line parameters, a map, and a
    /// coordinate list. </summary>
    ///
    /// <param name="dataLoadArguments"> The data load arguments. </param>
    /// <param name="map"> The map. </param>
    /// <param name="inputCoordinates"> The list of input coordinates. </param>
    ///
    /// <returns> The data iterator. </returns>
    std::unique_ptr<dataset::IParsingIterator> GetMappedDataIterator(const DataLoadArguments& dataLoadArguments, const std::shared_ptr<layers::Map>& map, const layers::CoordinateList& inputCoordinates);

// TODO: this
    std::unique_ptr<dataset::IParsingIterator> GetMappedDataIterator(const dataset::IParsingIterator& inputIterator, const std::shared_ptr<layers::Map>& map, const layers::CoordinateList& inputCoordinates);
}

