////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     DataLoaders.cpp (common)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataLoaders.h"
#include "CoordinateListTools.h"

// utilities
#include "files.h"

// dataset
#include "SequentialLineIterator.h"
#include "SparseEntryParser.h"
#include "MappedParser.h"
#include "ParsingIterator.h"

// stl
#include <memory>

namespace common
{
    std::unique_ptr<dataset::IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments)
    {
       // create line iterator - read line by line sequentially
        dataset::SequentialLineIterator lineIterator(dataLoadArguments.inputDataFile);

        // create parser for sparse vectors (SVMLight format)
        dataset::SparseEntryParser sparseEntryParser;

        // Create iterator
        return dataset::GetParsingIterator(std::move(lineIterator), sparseEntryParser);
    }

    std::unique_ptr<dataset::IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments, const layers::Map& map, const layers::CoordinateList& inputCoordinates)
    {
        // create parser for sparse vectors (SVMLight format)
        dataset::SparseEntryParser sparseEntryParser;

        // create mapped parser
        dataset::MappedParser<dataset::SparseEntryParser> mappedParser(sparseEntryParser, map, inputCoordinates);

        // create line iterator - read line by line sequentially
        dataset::SequentialLineIterator lineIterator(dataLoadArguments.inputDataFile);

        // Create iterator
        return dataset::GetParsingIterator(std::move(lineIterator), mappedParser);
    }

    std::unique_ptr<dataset::IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments, const MapLoadArguments& mapLoadArguments)
    {
        if (mapLoadArguments.inputMapFile == "")
        {
            return GetDataIterator(dataLoadArguments);
        }
        else
        {
            layers::Map map;
            layers::CoordinateList coordinateList;
            return GetDataIteratorMapCoordinates(dataLoadArguments, mapLoadArguments, map, coordinateList);
        }
    }

    std::unique_ptr<dataset::IParsingIterator> GetDataIteratorMapCoordinates(const DataLoadArguments& dataLoadArguments, const MapLoadArguments& mapLoadArguments, /* out */ layers::Map& map, /* out */ layers::CoordinateList& inputCoordinates)
    {
        map = layers::Map::Load(mapLoadArguments.inputMapFile);

        // create list of output coordinates
        inputCoordinates = GetCoordinateList(map, mapLoadArguments.coordinateList);

        // get data iterator
        return GetDataIterator(dataLoadArguments, map, inputCoordinates);
    }

    void DataIteratorToRowDataset(dataset::IParsingIterator& dataIterator, /* out */ dataset::RowDataset& dataset)
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
        /* out */ dataset::RowDataset& rowDataset,
        /* out */ layers::Map& map,
        /* out */ layers::CoordinateList& inputCoordinates)
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
            map = layers::Map(numColumns);

            // create a coordinate list of this map
            inputCoordinates = layers::GetCoordinateList(0, 0, numColumns-1);
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
