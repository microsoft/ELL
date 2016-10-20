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

    template <typename DatasetType>
    DatasetType GetMappedDataset(const DataLoadArguments& dataLoadArguments, const model::DynamicMap& map)
    {
        auto dataIterator = GetDataIterator(dataLoadArguments);
        DatasetType dataset;

        // generate mapped dataset
        while (dataIterator->IsValid())
        {
            auto row = dataIterator->Get(); // this is an AutoSupervisedExample
            auto dataVec = row.GetDataVector().DeepCopyAs<data::DoubleDataVector>();
            map.SetInputValue(0, dataVec);
            auto output = map.ComputeOutput<data::DoubleDataVector>(0);
            // convert output vector to OutDataVectorType
            auto mappedRow = typename DatasetType::DatasetExampleType(output, row.GetMetadata());
            dataset.AddExample(mappedRow);
            dataIterator->Next();
        }
        return dataset;
    }
}
}
