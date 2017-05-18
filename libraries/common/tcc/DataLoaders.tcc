////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DataLoaders.tcc (common)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace common
{
    template <typename MapType>
    data::AutoSupervisedDataset GetMappedDataset(data::AutoSupervisedExampleIterator exampleIterator, const MapType& map)
    {
        data::AutoSupervisedDataset dataset;

        // generate mapped dataset
        while (exampleIterator.IsValid())
        {
            auto example = exampleIterator.Get();
            auto mappedDataVector = map.Compute<data::DoubleDataVector>(example.GetDataVector());
            auto mappedExample = data::AutoSupervisedExample(std::move(mappedDataVector), example.GetMetadata());
            dataset.AddExample(mappedExample);

            exampleIterator.Next();
        }
        return dataset;
    }

    template <typename MapType>
    data::AutoSupervisedDataset GetMappedDataset(const DataLoadArguments& dataLoadArguments, const MapType& map)
    {
        return GetMappedDataset(GetExampleIterator(dataLoadArguments), map);
    }
}
}
