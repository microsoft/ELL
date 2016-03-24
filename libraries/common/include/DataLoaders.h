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
#include "RowDataset.h"

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
    /// <param name="mapLoadArguments"> The map load arguments. </param>
    ///
    /// <returns> The data iterator. </returns>
    std::unique_ptr<dataset::IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments, const MapLoadArguments& mapLoadArguments);

    /// <summary> Gets the map specified by the command line parameters </summary>
    ///
    /// <param name="mapLoadArguments"> The map load arguments. </param>
    ///
    /// <returns> A shared pointer to the map. </returns>
    std::unique_ptr<layers::Map> GetMap(const MapLoadArguments& mapLoadArguments);

    /// <summary> Gets the input coordinates for the given map, as specified by the command line parameters </summary>
    ///
    /// <param name="map">  The map. </param>
    /// <param name="mapLoadArguments"> The map load arguments. </param>
    ///
    /// <returns> The input coordinates to use for the map. </returns>
    layers::CoordinateList GetInputCoordinates(const layers::Map& map, const MapLoadArguments& mapLoadArguments);

    /// <summary> Gets a dataset from a data file, based on command line parameters. </summary>
    ///
    /// <param name="dataLoadArguments"> The data load arguments. </param>
    ///
    /// <returns> The dataset read in from the file. </returns>
    std::unique_ptr<dataset::RowDataset> GetDataset(const DataLoadArguments& dataLoadArguments);

    /// <summary> Gets a dataset by passing a dataset through a map, based on command line parameters. </summary>
    ///
    /// <param name="dataLoadArguments"> The data load arguments. </param>
    /// <param name="mapLoadArguments"> The map load arguments. </param>
    ///
    /// <returns> The dataset produced by running the input dataset through the map. </returns>
    std::unique_ptr<dataset::RowDataset> GetDataset(const DataLoadArguments& dataLoadArguments, const std::shared_ptr<layers::Map>& map, const layers::CoordinateList& inputCoordinates);
}

