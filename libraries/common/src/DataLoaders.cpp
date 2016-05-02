////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     DataLoaders.cpp (common)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataLoaders.h"
#include "CoordinateListTools.h"
#include "LoadModel.h"

// utilities
#include "Files.h"

// dataset
#include "SequentialLineIterator.h"
#include "SparseEntryParser.h"
#include "MappedParser.h"
#include "ParsingIterator.h"
#include "RowDataset.h"

// stl
#include <cstdint>
#include <memory>
#include <stdexcept>

namespace
{
    std::unique_ptr<dataset::IParsingIterator> GetMappedDataIterator(const common::DataLoadArguments& dataLoadArguments, layers::Map map)
    {
        // create mapped parser for sparse vectors (SVMLight format)
        dataset::MappedParser<dataset::SparseEntryParser> mappedParser(dataset::SparseEntryParser(), std::move(map));

        // create line iterator - read line by line sequentially
        dataset::SequentialLineIterator lineIterator(dataLoadArguments.inputDataFile);

        // Create iterator
        return dataset::GetParsingIterator(std::move(lineIterator), std::move(mappedParser));
    }
}

namespace common
{
    //
    // Public functions
    //
    std::unique_ptr<dataset::IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments)
    {
        // create parser for sparse vectors (SVMLight format)
        dataset::SparseEntryParser sparseEntryParser;

        // create line iterator - read line by line sequentially
        dataset::SequentialLineIterator lineIterator(dataLoadArguments.inputDataFile);

        // Create iterator
        return dataset::GetParsingIterator(std::move(lineIterator), std::move(sparseEntryParser));
    }

    std::unique_ptr<dataset::IParsingIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments, const MapLoadArguments& mapLoadArguments)
    {
        // read model file
        auto model = common::LoadModel(mapLoadArguments);

        // get map output coordinate list
        auto mapOutputCoordinates = layers::BuildCoordinateList(model, dataLoadArguments.parsedDataDimension, mapLoadArguments.coordinateListString);

        // get a data iterator
        return GetMappedDataIterator(dataLoadArguments, layers::Map(model, mapOutputCoordinates));
    }

    dataset::RowDataset<> GetRowDataset(const DataLoadArguments& dataLoadArguments, layers::Map map)
    {
        auto dataIterator = GetMappedDataIterator(dataLoadArguments, map);

        dataset::RowDataset<> rowDataset;

        while (dataIterator->IsValid())
        {
            rowDataset.AddExample(dataIterator->Get());
            dataIterator->Next();
        }

        return rowDataset;
    }
}
