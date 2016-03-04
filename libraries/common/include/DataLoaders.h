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

    /// <summary> Gets a mapped data iterator, based on command line parameters, a map, and a
    /// coordinate list. </summary>
    ///
    /// <param name="dataLoadArguments"> The data load arguments. </param>
    /// <param name="map"> The map. </param>
    /// <param name="inputCoordinates"> The list of input coordinates. </param>
    ///
    /// <returns> The data iterator. </returns>
    std::unique_ptr<dataset::IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments, const layers::Map& map, const layers::CoordinateList& inputCoordinates);

    /// <summary> Gets a mapped data iterator, based on command line parameters. </summary>
    ///
    /// <param name="dataLoadArguments"> The data load arguments. </param>
    /// <param name="mapLoadArguments">  The map load arguments. </param>
    ///
    /// <returns> The data iterator. </returns>
    std::unique_ptr<dataset::IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments, const MapLoadArguments& mapLoadArguments);

    /// <summary> Gets a mapped data iterator, a map and a coordinate list, based on command line
    /// parameters. </summary>
    ///
    /// <param name="dataLoadArguments"> The data load arguments. </param>
    /// <param name="mapLoadArguments"> The map load arguments. </param>
    /// <param name="map"> [in,out] The map. </param>
    /// <param name="inputCoordinates"> [in,out] The list of input coordinates. </param>
    ///
    /// <returns> The data iterator. </returns>
    std::unique_ptr<dataset::IParsingIterator> GetDataIteratorMapCoordinates(const DataLoadArguments& dataLoadArguments, const MapLoadArguments& mapLoadArguments, /* out */ layers::Map& map, /* out */ layers::CoordinateList& inputCoordinates);

    /// <summary> Converts a data iterator (one that gets supervised examples) into a
    /// dataset::RowDataset. </summary>
    ///
    /// <param name="parsingIterator"> [in,out] The parsing iterator. </param>
    /// <param name="dataset"> [in,out] The dataset. </param>
    void DataIteratorToRowDataset(dataset::IParsingIterator& parsingIterator, /* out */ dataset::RowDataset& dataset);

    /// <summary> Loads a dataset, a map, and a coordinate list based on command line parameters. </summary>
    ///
    /// <param name="dataLoadArguments"> The data load arguments. </param>
    /// <param name="mapLoadArguments"> The map load arguments. </param>
    /// <param name="rowDataset"> [in,out] The row dataset. </param>
    /// <param name="map"> [in,out] The map. </param>
    /// <param name="inputCoordinates"> [in,out] The input coordinates. </param>
    void GetRowDatasetMapCoordinates(
        const DataLoadArguments& dataLoadArguments,
        const MapLoadArguments& mapLoadArguments,
        /* out */ dataset::RowDataset& rowDataset,
        /* out */ layers::Map& map,
        /* out */ layers::CoordinateList& inputCoordinates);
}

