// Loaders.h

#pragma once

#include "MapLoadArguments.h"
#include "DataLoadArguments.h" 

// dataset
#include "SupervisedExample.h"
using dataset::RowDataset;

// layers
#include "Map.h"
using layers::Map;

#include "Coordinate.h"
using layers::CoordinateList;

#include "ParsingIterator.h"
using dataset::IParsingIterator;


namespace common
{
    /// Gets a data iterator from a data file, based on command line parameters
    ///
    unique_ptr<IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments);

    /// Gets a mapped data iterator, based on command line parameters, a map, and a coordinate list
    ///
    unique_ptr<IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments, const Map& map, const CoordinateList& inputCoordinates);

    /// Gets a mapped data iterator, based on command line parameters
    ///
    unique_ptr<IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments, const MapLoadArguments& mapLoadArguments);

    /// Gets a mapped data iterator, a map and a coordinate list, based on command line parameters
    ///
    unique_ptr<IParsingIterator> GetDataIteratorMapCoordinates(const DataLoadArguments& dataLoadArguments, const MapLoadArguments& mapLoadArguments, /* out */ Map& map, /* out */ CoordinateList& inputCoordinates);

    /// Converts a data iterator (one that gets supervised examples) into a RowDataset
    ///
    void DataIteratorToRowDataset(IParsingIterator& parsingIterator, /* out */ RowDataset& dataset);

    /// Loads a dataset, a map, and a coordinate list based on command line parameters
    ///
    void GetRowDatasetMapCoordinates(
        const DataLoadArguments& dataLoadArguments,
        const MapLoadArguments& mapLoadArguments,
        /* out */ RowDataset& rowDataset,
        /* out */ Map& map,
        /* out */ CoordinateList& inputCoordinates);
}

