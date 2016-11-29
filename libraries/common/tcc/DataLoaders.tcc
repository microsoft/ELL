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
            auto example = dataIterator->Get();
            auto mappedDataVector = map.Compute<data::DoubleDataVector>(example.GetDataVector());
            auto mappedExample = typename DatasetType::DatasetExampleType(std::move(mappedDataVector), example.GetMetadata());
            dataset.AddExample(mappedExample);

            dataIterator->Next();
        }
        return dataset;
    }
}
}
