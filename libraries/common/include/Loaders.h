// Loaders.h

#pragma once

// dataset
#include "SupervisedExample.h"
using dataset::RowDataset;

// layers
#include "Map.h"
using layers::Map;

#include "Coordinate.h"
using layers::CoordinateList;

// utilities
#include "MapLoadArguments.h"
using utilities::MapLoadArguments;

#include "DataLoadArguments.h" 
using utilities::DataLoadArguments;

#include "ParsingIterator.h"
using dataset::IParsingIterator;


namespace common
{
    /// Gets a data iterator from a data file, based on command line parameters
    ///
    unique_ptr<IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments);

    /// Gets a mapped data iterator, based on command line parameters, a map, and a coordinate list
    ///
    unique_ptr<IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments, const Map& map, const CoordinateList& coordinateList);

    /// Converts a data iterator (one that gets supervised examples) into a RowDataset
    ///
    void DataIteratorToRowDataset(IParsingIterator& parsingIterator, RowDataset& dataset);

    /// Loads a dataset, a map, and a coordinate list based on command line parameters
    ///
    void LoadDatasetMapCoordinates(
        const DataLoadArguments& dataLoadArguments,
        const MapLoadArguments& mapLoadArguments,
        RowDataset& rowDataset,
        Map& map,
        CoordinateList& coordinateList);
}

