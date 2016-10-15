////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DataLoaders.tcc (common)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace emll
{
namespace common
{
    template <typename DatasetType>
    DatasetType GetMappedDataset(common::DataLoadArguments& dataLoadArguments, const model::DynamicMap& map)
    {
        auto dataIterator = GetDataIterator(dataLoadArguments);
        DatasetType dataset;

        // generate mapped dataset
        auto mapInputSize = map.GetInputSize(0);
        while (dataIterator->IsValid())
        {
            auto row = dataIterator->Get();
            // truncate to size of map input
            dataset::DoubleDataVector dataVec = row.GetDataVector();
            dataVec.Resize(mapInputSize);
            map.SetInputValue<double>(0, dataVec);
            auto output = map.ComputeOutput<dataset::DoubleDataVector>(0);
            auto mappedRow = dataset::DenseSupervisedExample{ output, row.GetMetadata() };
            mappedDataset.AddExample(mappedRow);
            dataIterator->Next();
        }
        return dataset;
    }

    template <typename DatasetType>
    DatasetType GetDataset(const DataLoadArguments& dataLoadArguments)
    {
        auto dataIterator = GetDataIterator(dataLoadArguments);
        DatasetType dataset;
        while (dataIterator->IsValid())
        {
            dataset.AddExample(dataIterator->Get());
            dataIterator->Next();
        }

        return dataset;
    }
 
}
}
