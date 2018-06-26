////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Dataset_test.cpp (data_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Dataset_test.h"
#include "Dataset.h"
#include "DataLoaders.h"
#include "Files.h"
#include "StringUtil.h"

// testing
#include "testing.h"

// stl
#include <sstream>

namespace ell
{
template <typename ExampleType>
ExampleType GetExample()
{
    using DataVectorType = typename ExampleType::DataVectorType;
    DataVectorType dataVector{ 1, 0, 1, 0, 1, 0, 1 };
    return ExampleType(std::make_shared<DataVectorType>(std::move(dataVector)), data::WeightLabel{ 1, 1 });
}

template <typename ExampleType1, typename ExampleType2>
void DatasetCastingTest()
{
    data::Dataset<ExampleType1> dataset1;
    dataset1.AddExample(GetExample<ExampleType1>());
    dataset1.AddExample(GetExample<ExampleType1>());
    dataset1.AddExample(GetExample<ExampleType1>());

    data::Dataset<ExampleType2> dataset2(dataset1.GetAnyDataset());

    std::string name1 = typeid(ExampleType1).name();
    std::string name2 = typeid(ExampleType2).name();

    std::stringstream ss1, ss2;
    dataset1.Print(ss1);
    dataset2.Print(ss2);
    bool isSame = (ss1.str() == ss2.str());

    testing::ProcessTest(name1 + "::ctor(" + name2 + ")", isSame);
}

template <typename ExampleType>
void DatasetCastingTestDispatch()
{
    DatasetCastingTest<ExampleType, data::Example<data::AutoDataVector, data::WeightLabel>>();
    DatasetCastingTest<ExampleType, data::Example<data::DoubleDataVector, data::WeightLabel>>();
    DatasetCastingTest<ExampleType, data::Example<data::FloatDataVector, data::WeightLabel>>();
    DatasetCastingTest<ExampleType, data::Example<data::ShortDataVector, data::WeightLabel>>();
    DatasetCastingTest<ExampleType, data::Example<data::ByteDataVector, data::WeightLabel>>();
    DatasetCastingTest<ExampleType, data::Example<data::SparseDoubleDataVector, data::WeightLabel>>();
    DatasetCastingTest<ExampleType, data::Example<data::SparseFloatDataVector, data::WeightLabel>>();
    DatasetCastingTest<ExampleType, data::Example<data::SparseShortDataVector, data::WeightLabel>>();
    DatasetCastingTest<ExampleType, data::Example<data::SparseByteDataVector, data::WeightLabel>>();
    DatasetCastingTest<ExampleType, data::Example<data::SparseBinaryDataVector, data::WeightLabel>>();
}

void DatasetCastingTests()
{
    DatasetCastingTestDispatch<data::AutoSupervisedExample>();
    DatasetCastingTestDispatch<data::DenseSupervisedExample>();
}

void DatasetSerializationTests()
{
    data::Dataset<data::AutoSupervisedExample> dataset1;
    dataset1.AddExample(GetExample<data::AutoSupervisedExample>());
    dataset1.AddExample(GetExample<data::AutoSupervisedExample>());
    dataset1.AddExample(GetExample<data::AutoSupervisedExample>());

    // save the dataset
    const std::string filename("dataset1.txt");
    auto stream = utilities::OpenOfstream(filename);
    dataset1.Print(stream);
    stream.close();

    // load the dataset
    data::Dataset<data::AutoSupervisedExample> dataset2;
    auto stream2 = utilities::OpenIfstream(filename);
    data::AutoSupervisedExampleIterator exampleIterator = ell::common::GetAutoSupervisedExampleIterator(stream2);
    while (exampleIterator.IsValid())
    {
        auto example = exampleIterator.Get();
        auto mappedExample = data::AutoSupervisedExample(example.GetDataVector().ToArray(), example.GetMetadata());
        dataset2.AddExample(mappedExample);
        exampleIterator.Next();
    }

    // make sure they are the same.

    testing::ProcessTest("DatasetSerializationTest size", dataset1.NumExamples() == dataset2.NumExamples());
    testing::ProcessTest("DatasetSerializationTest features", dataset1.NumFeatures() == dataset2.NumFeatures());
    int errors = 0;
    if (dataset1.NumExamples() == dataset2.NumExamples() && dataset1.NumFeatures() == dataset2.NumFeatures())
    {
        for (size_t i = 0; i < dataset1.NumExamples(); i++)
        {
            auto e1 = dataset1.GetExample(i);
            auto e2 = dataset2.GetExample(i);
            
            auto sameVector = testing::IsEqual(e1.GetDataVector().ToArray(), e2.GetDataVector().ToArray());
            auto sameLabel = e1.GetMetadata().label == e2.GetMetadata().label;
            auto sameWeight = e1.GetMetadata().weight == e2.GetMetadata().weight;
            if (!(sameVector && sameLabel && sameWeight))
            {
                errors++;
            }
        }
    }
    testing::ProcessTest(utilities::FormatString("DatasetSerializationTest data %d errors", errors), errors == 0);
}
}