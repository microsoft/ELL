// Loaders.cpp

#include "Loaders.h"

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

#include "ParsingIterator.h"
using dataset::GetParsingIterator;

namespace common
{
    unique_ptr<IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments)
    {
        // open data file
        ifstream dataFStream = OpenIfstream(dataLoadArguments.inputDataFile);

        // create line iterator - read line by line sequentially
        SequentialLineIterator lineIterator(dataFStream);

        // create parser for sparse vectors (SVMLight format)
        SparseEntryParser sparseEntryParser;

        // Create iterator
        return GetParsingIterator(lineIterator, sparseEntryParser);
    }

    unique_ptr<IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments, const Map& map, const CoordinateList& coordinateList)
    {
        // open data file
        ifstream dataFStream = OpenIfstream(dataLoadArguments.inputDataFile);

        // create line iterator - read line by line sequentially
        SequentialLineIterator lineIterator(dataFStream);

        // create parser for sparse vectors (SVMLight format)
        SparseEntryParser sparseEntryParser;

        // create mapped parser
        MappedParser<SparseEntryParser> mappedParser(sparseEntryParser, map, coordinateList);

        // Create iterator
        return GetParsingIterator(lineIterator, mappedParser);
    }

    void LoadDataset(IParsingIterator& parsingIterator, RowDataset& dataset)
    {
        // Load row by row
        while (parsingIterator.IsValid())
        {
            dataset.PushBackRow(parsingIterator.Get());
            parsingIterator.Next();
        }
    }

    void LoadDatasetMapCoordinates(
        const DataLoadArguments& dataLoadArguments,
        const MapLoadArguments& mapLoadArguments,
        RowDataset& rowDataset,
        Map& map,
        CoordinateList& coordinateList)
    {
        // handle two cases - input map specified or unspecified
        if (mapLoadArguments.inputMapFile == "")
        {
            // get data iterator 
            auto upDataIterator = GetDataIterator(dataLoadArguments);

            // load dataset
            LoadDataset(*upDataIterator, rowDataset);

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
            ifstream mapFStream = OpenIfstream(mapLoadArguments.inputMapFile);
            map = JsonSerializer::Load<Map>(mapFStream, "Base");

            // create list of output coordinates
            coordinateList = CoordinateListFactory::IgnoreSuffix(map, mapLoadArguments.inputMapIgnoreSuffix);

            // get data iterator
            auto upDataIterator = GetDataIterator(dataLoadArguments, map, coordinateList);

            // load dataset
            LoadDataset(*upDataIterator, rowDataset);
        }
    }
}
