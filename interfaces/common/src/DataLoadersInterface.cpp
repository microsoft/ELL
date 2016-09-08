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
#include "DataLoaders.h"
#include "RowDatasetInterface.h"

// dataset
#include "Example.h"
#include "ParsingIterator.h"
#include "SequentialLineIterator.h"
#include "SparseEntryParser.h"

// utilties
#include "Files.h"

namespace
{
    template <typename RowIteratorType, typename VectorEntryParserType>
    dataset::ParsingIterator<RowIteratorType, VectorEntryParserType> MakeParsingIterator(RowIteratorType row_iter, VectorEntryParserType parser)
    {
        return dataset::ParsingIterator<RowIteratorType, VectorEntryParserType>(std::move(row_iter), std::move(parser));
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
                dataset::GenericSupervisedExample example(dataVector, dataset::WeightLabel{ 1, labelValue });
                dataset.AddExample(std::move(example));
                //                _rows.emplace_back(label, timestamp, features);
            }
        }
    }

    bool EndsWith(const std::string& str, const std::string& suffix)
    {
        return (str.rfind(suffix) == str.length() - suffix.length());
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

    interfaces::GenericRowDataset GetDataset(const std::string& dataFilename)
    {
        dataset::GenericRowDataset rowDataset;
        // load dense TSV file if filename ends in .tsv
        if (EndsWith(dataFilename, ".tsv"))
        {
            auto inputStream = utilities::OpenIfstream(dataFilename);
            ReadTsvStream(inputStream, rowDataset);
        }
        else
        {
            common::DataLoadArguments dataLoadArguments;
            dataLoadArguments.inputDataFilename = dataFilename;
            auto dataIterator = common::GetDataIterator(dataLoadArguments);
            while (dataIterator->IsValid())
            {
                rowDataset.AddExample(dataIterator->Get());
                dataIterator->Next();
            }
        }

        return interfaces::GenericRowDataset(std::move(rowDataset));
    }
}
