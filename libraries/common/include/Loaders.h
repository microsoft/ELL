// Loaders.h

#pragma once

#include "MapLoadArguments.h"
#include "DataLoadArguments.h" 

// dataset
#include "SupervisedExample.h"

// layers
#include "Map.h"

#include "Coordinate.h"

#include "ParsingIterator.h"


namespace common
{
    /// Gets a data iterator from a data file, based on command line parameters
    ///
    std::unique_ptr<dataset::IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments);

    /// Gets a mapped data iterator, based on command line parameters, a map, and a coordinate list
    ///
    std::unique_ptr<dataset::IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments, const layers::Map& map, const layers::CoordinateList& inputCoordinates);

    /// Gets a mapped data iterator, based on command line parameters
    ///
    std::unique_ptr<dataset::IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments, const MapLoadArguments& mapLoadArguments);

    /// Get a map, based on command line parameters
    ///
    layers::Map GetMap(const MapLoadArguments& mapLoadArguments);

    /// Gets a mapped data iterator, a map and a coordinate list, based on command line parameters
    ///
    std::unique_ptr<dataset::IParsingIterator> GetDataIteratorMapCoordinates(const DataLoadArguments& dataLoadArguments, const MapLoadArguments& mapLoadArguments, /* out */ layers::Map& map, /* out */ layers::CoordinateList& inputCoordinates);

    /// Converts a data iterator (one that gets supervised examples) into a dataset::RowDataset
    ///
    void DataIteratorToRowDataset(dataset::IParsingIterator& parsingIterator, /* out */ dataset::RowDataset& dataset);

    /// Loads a dataset, a map, and a coordinate list based on command line parameters
    ///
    void GetRowDatasetMapCoordinates(
        const DataLoadArguments& dataLoadArguments,
        const MapLoadArguments& mapLoadArguments,
        /* out */ dataset::RowDataset& rowDataset,
        /* out */ layers::Map& map,
        /* out */ layers::CoordinateList& inputCoordinates);
}

