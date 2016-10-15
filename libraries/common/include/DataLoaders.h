////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DataLoaders.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DataLoadArguments.h"

// data
#include "Dataset.h"
#include "ParsingExampleIterator.h"

// model
#include "DynamicMap.h"

// stl
#include <string>

namespace emll
{
namespace common
{
    /// <summary> Gets a data iterator from a data file, based on data load arguments. </summary>
    ///
    /// <param name="dataLoadArguments"> The data load arguments. </param>
    ///
    /// <returns> The data iterator. </returns>
    std::unique_ptr<data::IParsingExampleIterator> GetDataIterator(const DataLoadArguments& dataLoadArguments);

    /// <summary> Gets a dataset from data load arguments. </summary>
    ///
    /// <typeparam name="DatasetType"> Dataset type. </typeparam>
    /// <param name="dataLoadArguments"> The data load arguments. </param>
    ///
    /// <returns> The dataset. </returns>
    template <typename DatasetType = data::AutoSupervisedDataset>
    DatasetType GetDataset(const DataLoadArguments& dataLoadArguments);

    // ####
    // #### TODO: make this work:
    template <typename DatasetType = data::AutoSupervisedDataset>
    DatasetType GetMappedDataset(const DataLoadArguments& dataLoadArguments, const model::DynamicMap& map)
    {
        auto dataIterator = GetDataIterator(dataLoadArguments);
        DatasetType dataset;
        using OutDataVectorType = DatasetType::ExampleType::DataVectorType;

        // generate mapped dataset
        while (dataIterator->IsValid())
        {
            auto row = dataIterator->Get(); // AutoSupervisedExample
            auto dataVec = row.GetDataVector();
            map.SetInputValue(0, dataVec);
            auto output = map.ComputeOutput<OutDataVectorType>(0);
            auto mappedRow = typename DatasetType::ExampleType( output, row.GetMetadata() );
            dataset.AddExample(mappedRow);
            dataIterator->Next();
        }
        return dataset;
    }

}
}

#include "../tcc/DataLoaders.tcc"
