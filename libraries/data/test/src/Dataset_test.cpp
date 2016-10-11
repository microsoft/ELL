////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Dataset_test.cpp (data_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Dataset_test.h"

#include "Dataset.h"

namespace emll
{
    template<typename ExampleType>
    ExampleType GetExample()
    {
        using DataVectorType = ExampleType::DataVectorType;
        DataVectorType dataVector{ 1,0,1,0,1,0,1 };
        return ExampleType(std::make_shared<DataVectorType>(std::move(dataVector)), data::WeightLabel{ 1,1 });
    }

    template<typename DataVectorType1, typename DataVectorType2>
    void DatasetCastingTest()
    {
        using ExampleType1 = data::Example<DataVectorType1, data::WeightLabel>;
        using ExampleType2 = data::Example<DataVectorType2, data::WeightLabel>;
        
        data::Dataset<ExampleType1> dataset1;
        dataset1.AddExample(GetExample<ExampleType1>());
        dataset1.AddExample(GetExample<ExampleType1>());
        dataset1.AddExample(GetExample<ExampleType1>());

//        data::Dataset<ExampleType2> dataset2(dataset1.GetExampleIterator<ExampleType2>());
    }

    void DatasetCastingTests()
    {
        DatasetCastingTest<data::DoubleDataVector, data::DoubleDataVector>();
        DatasetCastingTest<data::DoubleDataVector, data::FloatDataVector>();
    }
}