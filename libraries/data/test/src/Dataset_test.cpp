////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Dataset_test.cpp (data_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Dataset_test.h"
#include "Dataset.h"

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
}