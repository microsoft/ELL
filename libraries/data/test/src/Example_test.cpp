////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Example_test.cpp (data_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Example_test.h"

#include "Example.h"

// testing 
#include "testing.h"

// stl
#include <sstream>

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
    void ToExampleTest()
    {
        using ExampleType1 = data::Example<DataVectorType1, data::WeightLabel>;
        using ExampleType2 = data::Example<DataVectorType2, data::WeightLabel>;

        auto typeName1 = std::string(typeid(DataVectorType1).name());
        auto typeName2 = std::string(typeid(DataVectorType2).name());

        size_t expectedReferenceCount = (typeName1 == typeName2) ? 2 : 1;
        auto example1 = GetExample<ExampleType1>();
        auto example2 = example1.ToExample<ExampleType2>();
        bool isExpectedReferenceCount = (example2.GetDataVectorReferenceCount() == expectedReferenceCount);

        std::stringstream ss1, ss2;
        example1.Print(ss1);
        example2.Print(ss2);
        bool isSame = (ss1.str() == ss2.str());

        testing::ProcessTest("Testing " + typeName1 + "::ToExample<" + typeName2 + ">", isSame && isExpectedReferenceCount);
    }

    template<typename DataVectorType>
    void ToExampleTestDispatch()
    {
        ToExampleTest<DataVectorType, data::AutoDataVector>();
        ToExampleTest<DataVectorType, data::DoubleDataVector>();
        ToExampleTest<DataVectorType, data::FloatDataVector>();
        ToExampleTest<DataVectorType, data::ShortDataVector>();
        ToExampleTest<DataVectorType, data::ByteDataVector>();
        ToExampleTest<DataVectorType, data::SparseDoubleDataVector>();
        ToExampleTest<DataVectorType, data::SparseFloatDataVector>();
        ToExampleTest<DataVectorType, data::SparseShortDataVector>();
        ToExampleTest<DataVectorType, data::SparseByteDataVector>();
        ToExampleTest<DataVectorType, data::SparseBinaryDataVector>();
    }

    void ToExampleTests()
    {
        ToExampleTestDispatch<data::AutoDataVector>();
        ToExampleTestDispatch<data::DoubleDataVector>();
        ToExampleTestDispatch<data::FloatDataVector>();
        ToExampleTestDispatch<data::ShortDataVector>();
        ToExampleTestDispatch<data::ByteDataVector>();
        ToExampleTestDispatch<data::SparseDoubleDataVector>();
        ToExampleTestDispatch<data::SparseFloatDataVector>();
        ToExampleTestDispatch<data::SparseShortDataVector>();
        ToExampleTestDispatch<data::SparseByteDataVector>();
        ToExampleTestDispatch<data::SparseBinaryDataVector>();
    }
}