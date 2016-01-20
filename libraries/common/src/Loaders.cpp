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

// stl
#include <memory>
using std::move;

namespace common
{
    unique_ptr<IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments)
    {
       // create line iterator - read line by line sequentially
        SequentialLineIterator lineIterator(dataLoadArguments.inputDataFile);

        // create parser for sparse vectors (SVMLight format)
        SparseEntryParser sparseEntryParser;

        // Create iterator
        return GetParsingIterator(move(lineIterator), sparseEntryParser);
    }

    unique_ptr<IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments, const Map& map, const CoordinateList& inputCoordinates)
    {
        // create parser for sparse vectors (SVMLight format)
        SparseEntryParser sparseEntryParser;

        // create mapped parser
        MappedParser<SparseEntryParser> mappedParser(sparseEntryParser, map, inputCoordinates);

        // create line iterator - read line by line sequentially
        SequentialLineIterator lineIterator(dataLoadArguments.inputDataFile);

        // Create iterator
        return GetParsingIterator(move(lineIterator), mappedParser);
    }

    unique_ptr<IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments, const MapLoadArguments& mapLoadArguments)
    {
        if (mapLoadArguments.inputMapFile == "")
        {
            return GetDataIterator(dataLoadArguments);
        }
        else
        {
            Map map;
            CoordinateList coordinateList;
            return GetDataIteratorMapCoordinates(dataLoadArguments, mapLoadArguments, map, coordinateList);
        }
    }

    unique_ptr<IParsingIterator> GetDataIteratorMapCoordinates(const DataLoadArguments& dataLoadArguments, const MapLoadArguments& mapLoadArguments, /* out */ Map& map, /* out */ CoordinateList& inputCoordinates)
    {
        // load map
        auto inputMapFStream = OpenIfstream(mapLoadArguments.inputMapFile);
        map = JsonSerializer::Load<Map>(inputMapFStream, "Base");

        // create list of output coordinates
        inputCoordinates = CoordinateListFactory::IgnoreSuffix(map, mapLoadArguments.inputMapIgnoreSuffix);

        // get data iterator
        return GetDataIterator(dataLoadArguments, map, inputCoordinates);
    }

    void DataIteratorToRowDataset(IParsingIterator& dataIterator, /* out */ RowDataset& dataset)
    {
        // Load row by row
        while (dataIterator.IsValid())
        {
            dataset.PushBackRow(dataIterator.Get());
            dataIterator.Next();
        }
    }

    void GetRowDatasetMapCoordinates(
        const DataLoadArguments& dataLoadArguments,
        const MapLoadArguments& mapLoadArguments,
        /* out */ RowDataset& rowDataset,
        /* out */ Map& map,
        /* out */ CoordinateList& inputCoordinates)
    {
        // handle two cases - input map specified or unspecified
        if (mapLoadArguments.inputMapFile == "")
        {
            // get data iterator 
            auto upDataIterator = GetDataIterator(dataLoadArguments);

            // load dataset
            DataIteratorToRowDataset(*upDataIterator, rowDataset);

            // number of columns
            uint64 numColumns = rowDataset.NumColumns();

            // create default map with single input layer
            map = Map(numColumns);

            // create a coordinate list of this map
            inputCoordinates = CoordinateListFactory::Sequence(0, numColumns);
        }
        else
        {
            // get data iterator. map, coordinates
            auto upDataIterator = GetDataIteratorMapCoordinates(dataLoadArguments, mapLoadArguments, map, inputCoordinates);

            // load dataset
            DataIteratorToRowDataset(*upDataIterator, rowDataset);
        }
    }
}
