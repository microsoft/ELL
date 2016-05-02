////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     DataLoadersInterface.cpp (interfaces)
//  Authors:  Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataLoadersInterface.h"

// common
#include "DataLoadArguments.h"
#include "RowDatasetInterface.h"
#include "DataLoaders.h"
#include "LoadModel.h"

// dataset
#include "SequentialLineIterator.h"
#include "SparseEntryParser.h"
#include "MappedParser.h"
#include "ParsingIterator.h"

// layers
#include "Map.h"
#include "Coordinate.h"
#include "CoordinateListTools.h"

// interface
#include "MapInterface.h"

namespace
{
    template<typename RowIteratorType, typename VectorEntryParserType>
    dataset::ParsingIterator<RowIteratorType, VectorEntryParserType> MakeParsingIterator(RowIteratorType row_iter, VectorEntryParserType parser)
    {
        return dataset::ParsingIterator<RowIteratorType, VectorEntryParserType>(std::move(row_iter), std::move(parser));
    }

    std::unique_ptr<dataset::IParsingIterator> GetMappedDataIterator(const common::DataLoadArguments& dataLoadArguments, layers::Map map)
    {
        // create mapped parser for sparse vectors (SVMLight format)
        dataset::MappedParser<dataset::SparseEntryParser> mappedParser(dataset::SparseEntryParser(), map);

        // create line iterator - read line by line sequentially
        dataset::SequentialLineIterator lineIterator(dataLoadArguments.inputDataFile);

        // Create iterator
        return dataset::GetParsingIterator(std::move(lineIterator), std::move(mappedParser));
    }


    utilities::AnyIterator<dataset::SupervisedExample> GetMappedDataIterator(const common::DataLoadArguments& dataLoadArguments, const interfaces::Map& map) 
    {
        // create mapped parser for sparse vectors (SVMLight format)
        dataset::MappedParser<dataset::SparseEntryParser> mappedParser(dataset::SparseEntryParser(), map.GetMap());

        // create line iterator - read line by line sequentially
        dataset::SequentialLineIterator lineIterator(dataLoadArguments.inputDataFile);

        // Create iterator
        auto parsingIterator = MakeParsingIterator(std::move(lineIterator), std::move(mappedParser));
        return utilities::MakeAnyIterator(parsingIterator); // TODO: investigate why this works, but using std::move(parsingIterator) doesn't (shouldn't a ParsingIterator with a SequentialLineIterator uncopyable?)
    }
}

namespace interfaces
{
    utilities::AnyIterator<dataset::SupervisedExample> GetDataIterator(const std::string& dataFilename)
    {
        // create parser for sparse vectors (SVMLight format)
        dataset::SparseEntryParser sparseEntryParser;

        // create line iterator - read line by line sequentially
        dataset::SequentialLineIterator lineIterator(dataFilename);

        // Create iterator
        auto parsingIterator = MakeParsingIterator(std::move(lineIterator), std::move(sparseEntryParser));
        return utilities::MakeAnyIterator(parsingIterator); // Why does this work?
    }

    utilities::AnyIterator<dataset::SupervisedExample> GetDataIterator(const std::string& dataFilename, int dimension, const std::string coordinateListString, const std::string& modelFilename)
    {
        common::DataLoadArguments dataLoadArguments;
        dataLoadArguments.inputDataFile = dataFilename;
        
        // read model file
        common::ModelLoadArguments modelLoadArguments;
        modelLoadArguments.inputModelFile = modelFilename;
        auto model = common::LoadModel(modelLoadArguments);

        // get map output coordinate list
        auto mapOutputCoordinates = layers::BuildCoordinateList(model, dimension, coordinateListString);

        // get a data iterator
        return GetMappedDataIterator(dataLoadArguments, interfaces::Map(layers::Map(model, mapOutputCoordinates)));
    }

    interfaces::RowDataset GetDataset(const std::string& dataFilename)
    {
       common::DataLoadArguments dataLoadArguments;    
       dataLoadArguments.inputDataFile = dataFilename;
       auto dataIterator = common::GetDataIterator(dataLoadArguments);

        dataset::RowDataset<> rowDataset;

        while (dataIterator->IsValid())
        {
            rowDataset.AddExample(dataIterator->Get());
            dataIterator->Next();
        }

        return interfaces::RowDataset(std::move(rowDataset));
    }

    interfaces::RowDataset GetDataset(const common::DataLoadArguments& dataLoadArguments, const interfaces::Map& map)
    {
        dataset::RowDataset<> rowDataset;

        auto dataIterator = GetMappedDataIterator(dataLoadArguments, map.GetMap());
        while (dataIterator->IsValid())
        {
            rowDataset.AddExample(dataIterator->Get());
            dataIterator->Next();
        }

        return interfaces::RowDataset(std::move(rowDataset));
    }

    interfaces::RowDataset GetDataset(const std::string& dataFilename, const interfaces::Map& map)
    {
       common::DataLoadArguments dataLoadArguments;
       dataLoadArguments.inputDataFile = dataFilename;
       return interfaces::GetDataset(dataLoadArguments, map);
    }
    
    interfaces::RowDataset GetDataset(const std::string& dataFilename, const std::string& mapFilename)
    {
       common::DataLoadArguments dataLoadArguments;
       common::MapLoadArguments mapLoadArguments;
    
       dataLoadArguments.inputDataFile = dataFilename;
       mapLoadArguments.inputModelFile = mapFilename;

       auto dataIterator = common::GetDataIterator(dataLoadArguments, mapLoadArguments);

        dataset::RowDataset<> rowDataset;

        while (dataIterator->IsValid())
        {
            rowDataset.AddExample(dataIterator->Get());
            dataIterator->Next();
        }

        return interfaces::RowDataset(std::move(rowDataset));
    }
}
