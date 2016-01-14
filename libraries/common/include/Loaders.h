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
    /// TODO
    ///
    unique_ptr<IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments);

    /// TODO
    ///
    unique_ptr<IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments, const Map& map, const CoordinateList& coordinateList);

    /// TODO
    ///
    void LoadDataset(IParsingIterator& parsingIterator, RowDataset& dataset);

    /// TODO
    ///
    void LoadDatasetMapCoordinates(
        const DataLoadArguments& dataLoadArguments,
        const MapLoadArguments& mapLoadArguments,
        RowDataset& rowDataset,
        Map& map,
        CoordinateList& coordinateList);
}

