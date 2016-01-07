// DatasetMapLoader.cpp

#include "DatasetMapLoader.h"
#include "DatasetLoader.h"

// utilities
#include "files.h"
using utilities::OpenIfstream;

// layers
#include "CoordinateListFactory.h"
using layers::CoordinateListFactory;

// dataset
#include "SequentialLineIterator.h"
using dataset::SequentialLineIterator;

#include "SparseEntryParser.h"
using dataset::SparseEntryParser;

#include "MappedParser.h"
using dataset::MappedParser;

namespace common
{
    void DatasetMapLoader::Load(
        const DataLoadArguments& dataLoadArguments,
        const MapLoadArguments& mapLoadArguments,
        RowDataset& rowDataset,
        Map& map,
        CoordinateList& coordinateList)
    {
        // open data file
      // ifstream dataFStream = OpenIfstream(dataLoadArguments.inputDataFile);
      ifstream dataFStream = ifstream("foo");
        // create line iterator - read line by line sequentially
        SequentialLineIterator lineIterator(dataFStream);

        // create parser for sparse vectors (SVMLight format)
        SparseEntryParser sparseEntryParser;

        // handle two cases - input map specified or unspecified
        if (mapLoadArguments.inputMapFile == "")
        {
            // load data wihtout applying any map
            rowDataset = DatasetLoader::Load(lineIterator, sparseEntryParser);

            // number of columns
            uint64 numColumns = rowDataset.NumColumns();

            // create default map with single input layer
            map = Map(numColumns);

            // create a coordinate list of this map
            coordinateList = CoordinateListFactory::Sequence(0, numColumns);
        }
        else
        {
            // load map
	  //            ifstream mapFStream = OpenIfstream(mapLoadArguments.inputMapFile);
	  ifstream mapFStream("foo");

            map = JsonSerializer::Load<Map>(mapFStream, "Base");

            // create list of output coordinates
            coordinateList = CoordinateListFactory::IgnoreSuffix(map, mapLoadArguments.inputMapIgnoreSuffix);

            // load data
            MappedParser<SparseEntryParser> mappedParser(sparseEntryParser, map, coordinateList);
            rowDataset = DatasetLoader::Load(lineIterator, mappedParser);
        }
    }
}
