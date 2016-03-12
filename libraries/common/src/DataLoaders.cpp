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
#include "Files.h"

// dataset
#include "SequentialLineIterator.h"
#include "SparseEntryParser.h"
#include "MappedParser.h"
#include "ParsingIterator.h"

// stl
#include <memory>

namespace common
{
    dataset::RowDataset LoadDataset(dataset::IParsingIterator& dataIterator)
    {
        dataset::RowDataset dataset;
        while (dataIterator.IsValid())
        {
            dataset.PushBackRow(dataIterator.Get());
            dataIterator.Next();
        }
        return dataset;
    }

    std::unique_ptr<dataset::IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments)
    {
        // create line iterator - read line by line sequentially
        dataset::SequentialLineIterator lineIterator(dataLoadArguments.inputDataFile);

        // create parser for sparse vectors (SVMLight format)
        dataset::SparseEntryParser sparseEntryParser;

        // Create iterator
        return dataset::GetParsingIterator(std::move(lineIterator), sparseEntryParser);
    }

    std::unique_ptr<dataset::IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments, const MapLoadArguments& mapLoadArguments)
    {
        if (mapLoadArguments.inputMapFile == "")
        {
            return GetDataIterator(dataLoadArguments);
        }
        else
        {
            auto map = GetMap(mapLoadArguments);
            auto inputCoordinates = GetInputCoordinates(*map, mapLoadArguments);
            auto dataIterator = GetMappedDataIterator(dataLoadArguments, map, inputCoordinates);
            return dataIterator;
        }
    }

    std::shared_ptr<layers::Map> GetMap(const MapLoadArguments& mapLoadArguments)
    {
        if (mapLoadArguments.inputMapFile != "")
        {
            return std::make_shared<layers::Map>(layers::Map::Load(mapLoadArguments.inputMapFile));
        }
        else
        {
            return nullptr;
        }
    }

    std::unique_ptr<dataset::IParsingIterator> GetMappedDataIterator(const DataLoadArguments& dataLoadArguments, const std::shared_ptr<layers::Map>& map, const layers::CoordinateList& inputCoordinates)
    {
        // create parser for sparse vectors (SVMLight format)
        dataset::SparseEntryParser sparseEntryParser;

        // create mapped parser
        dataset::MappedParser<dataset::SparseEntryParser> mappedParser(sparseEntryParser, map, inputCoordinates);

        // why not just run through the map?

        // create line iterator - read line by line sequentially
        dataset::SequentialLineIterator lineIterator(dataLoadArguments.inputDataFile);

        // Create iterator
        return dataset::GetParsingIterator(std::move(lineIterator), mappedParser);
    }

    layers::CoordinateList GetInputCoordinates(const layers::Map& map, const MapLoadArguments& mapLoadArguments)
    {
        layers::CoordinateList inputCoordinates = layers::GetCoordinateList(map, mapLoadArguments.coordinateList);
        return inputCoordinates;
    }

    // Do this instead of calling GetRowDatasetMapCoordinates
    void MyCodeInMain(const DataLoadArguments& dataLoadArguments, const MapLoadArguments& mapLoadArguments)
    {
        auto inputIterator = GetDataIterator(dataLoadArguments);
        auto map = GetMap(mapLoadArguments);
        dataset::RowDataset rowDataset; // depends on map if there is one, else depends on inputIterator
        layers::CoordinateList inputCoordinates; // depends on dataset if there is one, else depends on map

        // need to set rowDataset and inputCoordinates
        if (map == nullptr)
        {
            rowDataset = LoadDataset(*inputIterator);
            auto numColumns = rowDataset.NumColumns();
            map = std::make_unique<layers::Map>(numColumns); // :(
            inputCoordinates = layers::GetCoordinateList(0, 0, numColumns - 1);
        }
        else
        {
            inputCoordinates = GetInputCoordinates(*map, mapLoadArguments);
            auto dataIterator = GetMappedDataIterator(dataLoadArguments, map, inputCoordinates);
            rowDataset = LoadDataset(*dataIterator);
        }
    }
}
