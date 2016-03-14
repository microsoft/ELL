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
    /// <param name="mapLoadArguments"> The map load arguments. </param>
    ///
    /// <returns> The data iterator. </returns>
    std::unique_ptr<dataset::IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments, const MapLoadArguments& mapLoadArguments);

    /// <summary> Gets a mapped data iterator, based on command line parameters, a map, and a
    /// coordinate list. </summary>
    ///
    /// <param name="dataLoadArguments"> The data load arguments. </param>
    /// <param name="mapLoadArguments"> The map load arguments. </param>
    /// <param name="rowDataset"> [in,out] The row dataset. </param>
    /// <param name="map"> [in,out] The map. </param>
    /// <param name="inputCoordinates"> [in,out] The input coordinates. </param>
    void GetRowDatasetMapCoordinates(
        const DataLoadArguments& dataLoadArguments,
        const MapLoadArguments& mapLoadArguments,
        dataset::RowDataset& rowDataset,
        std::shared_ptr<layers::Map>& map, // going to have to be a shared_ptr<Map>&
        layers::CoordinateList& inputCoordinates);


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

    /// <summary> Loads a RowDataset from the data iterator returned by GetDataIterator </summary>
    ///
    /// <param name="dataIterator"> The data iterator to use to populate the dataset </param>
    ///
    /// <returns> The dataset. </returns>
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
}

