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
}
}
