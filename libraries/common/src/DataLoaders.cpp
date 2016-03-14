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
#include <stdexcept>

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
            auto map = std::shared_ptr<layers::Map>(GetMap(mapLoadArguments));
            if(map == nullptr)
            {
                throw std::runtime_error("Error: couldn't load map");
            }
            auto inputCoordinates = GetInputCoordinates(*map, mapLoadArguments);
            return GetMappedDataIterator(dataLoadArguments, map, inputCoordinates);
        }
    }

    std::unique_ptr<layers::Map> GetMap(const MapLoadArguments& mapLoadArguments)
    {
        if (mapLoadArguments.inputMapFile != "")
        {
            return std::make_unique<layers::Map>(layers::Map::Load(mapLoadArguments.inputMapFile));
        }
        else
        {
            return nullptr;
        }
    }

    std::unique_ptr<dataset::IParsingIterator> GetMappedDataIterator(const DataLoadArguments& dataLoadArguments, const std::shared_ptr<layers::Map>& map, const layers::CoordinateList& inputCoordinates)
    {
        if(map == nullptr)
        {
            return GetDataIterator(dataLoadArguments);
        }

        // create parser for sparse vectors (SVMLight format)
        dataset::SparseEntryParser sparseEntryParser;

        // create mapped parser
        dataset::MappedParser<dataset::SparseEntryParser> mappedParser(sparseEntryParser, map, inputCoordinates);

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

    void GetRowDatasetMapCoordinates(
        const DataLoadArguments& dataLoadArguments,
        const MapLoadArguments& mapLoadArguments,
        dataset::RowDataset& rowDataset,
        std::shared_ptr<layers::Map>& map,
        layers::CoordinateList& inputCoordinates)
    {
        // handle two cases - input map specified or unspecified
        if (mapLoadArguments.inputMapFile == "")
        {
            // get data iterator 
            auto upDataIterator = GetDataIterator(dataLoadArguments);

            // load dataset
            rowDataset = LoadDataset(*upDataIterator);

            // number of columns
            uint64 numColumns = rowDataset.NumColumns();

            // create default map with single input layer
            map = std::make_shared<layers::Map>(numColumns);

            // create a coordinate list of this map
            inputCoordinates = layers::GetCoordinateList(0, 0, numColumns-1);
        }
        else
        {
            // read map from file
            map = GetMap(mapLoadArguments);

            // read input coordinates
            inputCoordinates = GetInputCoordinates(*map, mapLoadArguments);

            // fill in dataset
            auto upDataIterator = GetMappedDataIterator(dataLoadArguments, map, inputCoordinates);
            rowDataset = LoadDataset(*upDataIterator);
        }
    }
}
