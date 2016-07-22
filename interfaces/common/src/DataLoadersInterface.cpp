////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DataLoadersInterface.cpp (interfaces)
//  Authors:  Chuck Jacobs
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
#include "WeightLabel.h"

// layers
#include "Map.h"
#include "Coordinate.h"
#include "CoordinateListTools.h"

// utilties
#include "Files.h"

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

    utilities::AnyIterator<dataset::GenericSupervisedExample> GetMappedDataIterator(const common::DataLoadArguments& dataLoadArguments, const interfaces::Map& map)
    {
        // create mapped parser for sparse vectors (SVMLight format)
        dataset::MappedParser<dataset::SparseEntryParser> mappedParser(dataset::SparseEntryParser(), map.GetMap());

        // create line iterator - read line by line sequentially
        dataset::SequentialLineIterator lineIterator(dataLoadArguments.inputDataFile);

        // Create iterator
        auto parsingIterator = MakeParsingIterator(std::move(lineIterator), std::move(mappedParser));
        return utilities::MakeAnyIterator(parsingIterator); // TODO: investigate why this works, but using std::move(parsingIterator) doesn't (shouldn't a ParsingIterator with a SequentialLineIterator uncopyable?)
    }
    
    // (Temporary) dense file I/O
    void ReadTsvStream(std::istream& stream, dataset::GenericRowDataset& dataset)
    {
        std::string current_label = "";
        std::string line_buf;
        while (std::getline(stream, line_buf))
        {
            // read instance
            std::stringstream line_stream(line_buf);

            // for each field
            std::string field;
            int field_index = 0;
            std::string label;
            int timestamp = 0;
            std::vector<double> features;

            while (std::getline(line_stream, field, '\t'))
            {
                // 0: label
                // 1: timestamp
                // 2-end: feature data (unless comment)

                if (field_index == 0) // label / comment type
                {
                    label = field == "" ? "Other" : field;
                }
                else if (field_index == 1) // timestamp
                {
                    timestamp = std::stoi(field, nullptr);
                }
                else // feature data
                {
                    double val = std::stod(field, nullptr);
                    features.push_back(val);
                }
                field_index++;
            }

            // done reading line, add row to database
            unsigned int min_num_features = 5;
            if (features.size() >= min_num_features)
            {
                auto labelValue = label == "Other" ? 0.0 : 1.0;
                auto dataVector = std::static_pointer_cast<dataset::IDataVector>(std::make_shared<dataset::DoubleDataVector>(features));
                dataset::GenericSupervisedExample example(dataVector, dataset::WeightLabel{1, labelValue});
                dataset.AddExample(std::move(example));
//                _rows.emplace_back(label, timestamp, features);
            }
        }
    }
    
    bool EndsWith(const std::string& str, const std::string& suffix)
    {
        return(str.rfind(suffix) == str.length() - suffix.length());
    }
}

namespace interfaces
{
    utilities::AnyIterator<dataset::GenericSupervisedExample> GetDataIterator(const std::string& dataFilename)
    {
        // create parser for sparse vectors (SVMLight format)
        dataset::SparseEntryParser sparseEntryParser;

        // create line iterator - read line by line sequentially
        dataset::SequentialLineIterator lineIterator(dataFilename);

        // Create iterator
        auto parsingIterator = MakeParsingIterator(std::move(lineIterator), std::move(sparseEntryParser));
        return utilities::MakeAnyIterator(parsingIterator); // Why does this work?
    }

    utilities::AnyIterator<dataset::GenericSupervisedExample> GetDataIterator(const std::string& dataFilename, int dimension, const std::string coordinateListString, const std::string& modelFilename)
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

    interfaces::GenericRowDataset GetDataset(const std::string& dataFilename)
    {
        dataset::GenericRowDataset rowDataset;
        // load dense TSV file if filename ends in .tsv
        if(EndsWith(dataFilename, ".tsv"))
        {
            auto inputStream = utilities::OpenIfstream(dataFilename);
            ReadTsvStream(inputStream, rowDataset);
        }
        else
        {
            common::DataLoadArguments dataLoadArguments;
            dataLoadArguments.inputDataFile = dataFilename;
            auto dataIterator = common::GetDataIterator(dataLoadArguments);
            while (dataIterator->IsValid())
            {
                rowDataset.AddExample(dataIterator->Get());
                dataIterator->Next();
            }
        }
        

        return interfaces::GenericRowDataset(std::move(rowDataset));
    }

    interfaces::GenericRowDataset GetDataset(const common::DataLoadArguments& dataLoadArguments, const interfaces::Map& map)
    {
        dataset::GenericRowDataset rowDataset;

        auto dataIterator = GetMappedDataIterator(dataLoadArguments, map.GetMap());
        while (dataIterator->IsValid())
        {
            rowDataset.AddExample(dataIterator->Get());
            dataIterator->Next();
        }

        return interfaces::GenericRowDataset(std::move(rowDataset));
    }

    interfaces::GenericRowDataset GetMappedDataset(const std::string& dataFilename, const interfaces::Map& map)
    {
       common::DataLoadArguments dataLoadArguments;
       dataLoadArguments.inputDataFile = dataFilename;
       return interfaces::GetDataset(dataLoadArguments, map);
    }
    
    interfaces::GenericRowDataset GetDataset(const std::string& dataFilename, const std::string& mapFilename)
    {
       common::DataLoadArguments dataLoadArguments;
       common::MapLoadArguments mapLoadArguments;
    
       dataLoadArguments.inputDataFile = dataFilename;
       mapLoadArguments.modelLoadArguments.inputModelFile = mapFilename;

       auto dataIterator = common::GetDataIterator(dataLoadArguments, mapLoadArguments);

        dataset::GenericRowDataset rowDataset;

        while (dataIterator->IsValid())
        {
            rowDataset.AddExample(dataIterator->Get());
            dataIterator->Next();
        }

        return interfaces::GenericRowDataset(std::move(rowDataset));
    }
}
