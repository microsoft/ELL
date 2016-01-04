// DatasetMapLoader.h

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

namespace common
{
    /// Container that holds a static function that loads and parses a dataset and a map 
    ///
    struct DatasetMapLoader
    {
        static void Load(
            const DataLoadArguments& dataLoadArguments,
            const MapLoadArguments& mapLoadArguments,
            /* out */ RowDataset& rowDataset,
            /* out */ Map& map,
            /* out */ CoordinateList& coordinateList);
    };
}

