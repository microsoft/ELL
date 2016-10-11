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

    template<typename DataVectorType1, typename DataVectorType2>
    void ToExampleTest(size_t expectedReferenceCount)
    {
        using ExampleType1 = data::Example<DataVectorType1, data::WeightLabel>;
        using ExampleType2 = data::Example<DataVectorType2, data::WeightLabel>;

        DataVectorType1 dataVector { 1,0,1,0,1,0,1 };
        auto example1 = ExampleType1(std::make_shared<DataVectorType1>(std::move(dataVector)), data::WeightLabel{ 1,1 });
        auto example2 = example1.ToExample<ExampleType2>();
        bool isExpected = (example2.GetDataVectorReferenceCount() == expectedReferenceCount);        

        std::stringstream ss1, ss2;
        example1.Print(ss1);
        example2.Print(ss2);
        bool isSame = (ss1.str() == ss2.str());

        testing::ProcessTest("Testing " + std::string(typeid(DataVectorType1).name()) + "::ToExample<" + std::string(typeid(DataVectorType2).name()) + ">", isSame && isExpected);
    }

    void ToExampleTests()
    {
        ToExampleTest<data::DoubleDataVector, data::DoubleDataVector>(2);
        ToExampleTest<data::DoubleDataVector, data::FloatDataVector>(1);
        ToExampleTest<data::DoubleDataVector, data::ShortDataVector>(1);
        ToExampleTest<data::DoubleDataVector, data::ByteDataVector>(1);
        ToExampleTest<data::DoubleDataVector, data::SparseDoubleDataVector>(1);
        ToExampleTest<data::DoubleDataVector, data::SparseFloatDataVector>(1);
        ToExampleTest<data::DoubleDataVector, data::SparseShortDataVector>(1);
        ToExampleTest<data::DoubleDataVector, data::SparseByteDataVector>(1);
        ToExampleTest<data::DoubleDataVector, data::SparseBinaryDataVector>(1);

        ToExampleTest<data::FloatDataVector, data::DoubleDataVector>(1);
        ToExampleTest<data::FloatDataVector, data::FloatDataVector>(2);
        ToExampleTest<data::FloatDataVector, data::ShortDataVector>(1);
        ToExampleTest<data::FloatDataVector, data::ByteDataVector>(1);
        ToExampleTest<data::FloatDataVector, data::SparseDoubleDataVector>(1);
        ToExampleTest<data::FloatDataVector, data::SparseFloatDataVector>(1);
        ToExampleTest<data::FloatDataVector, data::SparseShortDataVector>(1);
        ToExampleTest<data::FloatDataVector, data::SparseByteDataVector>(1);
        ToExampleTest<data::FloatDataVector, data::SparseBinaryDataVector>(1);

        ToExampleTest<data::ShortDataVector, data::DoubleDataVector>(1);
        ToExampleTest<data::ShortDataVector, data::FloatDataVector>(1);
        ToExampleTest<data::ShortDataVector, data::ShortDataVector>(2);
        ToExampleTest<data::ShortDataVector, data::ByteDataVector>(1);
        ToExampleTest<data::ShortDataVector, data::SparseDoubleDataVector>(1);
        ToExampleTest<data::ShortDataVector, data::SparseFloatDataVector>(1);
        ToExampleTest<data::ShortDataVector, data::SparseShortDataVector>(1);
        ToExampleTest<data::ShortDataVector, data::SparseByteDataVector>(1);
        ToExampleTest<data::ShortDataVector, data::SparseBinaryDataVector>(1);

        ToExampleTest<data::ByteDataVector, data::DoubleDataVector>(1);
        ToExampleTest<data::ByteDataVector, data::FloatDataVector>(1);
        ToExampleTest<data::ByteDataVector, data::ShortDataVector>(1);
        ToExampleTest<data::ByteDataVector, data::ByteDataVector>(2);
        ToExampleTest<data::ByteDataVector, data::SparseDoubleDataVector>(1);
        ToExampleTest<data::ByteDataVector, data::SparseFloatDataVector>(1);
        ToExampleTest<data::ByteDataVector, data::SparseShortDataVector>(1);
        ToExampleTest<data::ByteDataVector, data::SparseByteDataVector>(1);
        ToExampleTest<data::ByteDataVector, data::SparseBinaryDataVector>(1);

        ToExampleTest<data::SparseDoubleDataVector, data::DoubleDataVector>(1);
        ToExampleTest<data::SparseDoubleDataVector, data::FloatDataVector>(1);
        ToExampleTest<data::SparseDoubleDataVector, data::ShortDataVector>(1);
        ToExampleTest<data::SparseDoubleDataVector, data::ByteDataVector>(1);
        ToExampleTest<data::SparseDoubleDataVector, data::SparseDoubleDataVector>(2);
        ToExampleTest<data::SparseDoubleDataVector, data::SparseFloatDataVector>(1);
        ToExampleTest<data::SparseDoubleDataVector, data::SparseShortDataVector>(1);
        ToExampleTest<data::SparseDoubleDataVector, data::SparseByteDataVector>(1);
        ToExampleTest<data::SparseDoubleDataVector, data::SparseBinaryDataVector>(1);

        ToExampleTest<data::SparseFloatDataVector, data::DoubleDataVector>(1);
        ToExampleTest<data::SparseFloatDataVector, data::FloatDataVector>(1);
        ToExampleTest<data::SparseFloatDataVector, data::ShortDataVector>(1);
        ToExampleTest<data::SparseFloatDataVector, data::ByteDataVector>(1);
        ToExampleTest<data::SparseFloatDataVector, data::SparseDoubleDataVector>(1);
        ToExampleTest<data::SparseFloatDataVector, data::SparseFloatDataVector>(2);
        ToExampleTest<data::SparseFloatDataVector, data::SparseShortDataVector>(1);
        ToExampleTest<data::SparseFloatDataVector, data::SparseByteDataVector>(1);
        ToExampleTest<data::SparseFloatDataVector, data::SparseBinaryDataVector>(1);

        ToExampleTest<data::SparseShortDataVector, data::DoubleDataVector>(1);
        ToExampleTest<data::SparseShortDataVector, data::FloatDataVector>(1);
        ToExampleTest<data::SparseShortDataVector, data::ShortDataVector>(1);
        ToExampleTest<data::SparseShortDataVector, data::ByteDataVector>(1);
        ToExampleTest<data::SparseShortDataVector, data::SparseDoubleDataVector>(1);
        ToExampleTest<data::SparseShortDataVector, data::SparseFloatDataVector>(1);
        ToExampleTest<data::SparseShortDataVector, data::SparseShortDataVector>(2);
        ToExampleTest<data::SparseShortDataVector, data::SparseByteDataVector>(1);
        ToExampleTest<data::SparseShortDataVector, data::SparseBinaryDataVector>(1);

        ToExampleTest<data::SparseByteDataVector, data::DoubleDataVector>(1);
        ToExampleTest<data::SparseByteDataVector, data::FloatDataVector>(1);
        ToExampleTest<data::SparseByteDataVector, data::ShortDataVector>(1);
        ToExampleTest<data::SparseByteDataVector, data::ByteDataVector>(1);
        ToExampleTest<data::SparseByteDataVector, data::SparseDoubleDataVector>(1);
        ToExampleTest<data::SparseByteDataVector, data::SparseFloatDataVector>(1);
        ToExampleTest<data::SparseByteDataVector, data::SparseShortDataVector>(1);
        ToExampleTest<data::SparseByteDataVector, data::SparseByteDataVector>(2);
        ToExampleTest<data::SparseByteDataVector, data::SparseBinaryDataVector>(1);

        ToExampleTest<data::SparseBinaryDataVector, data::DoubleDataVector>(1);
        ToExampleTest<data::SparseBinaryDataVector, data::FloatDataVector>(1);
        ToExampleTest<data::SparseBinaryDataVector, data::ShortDataVector>(1);
        ToExampleTest<data::SparseBinaryDataVector, data::ByteDataVector>(1);
        ToExampleTest<data::SparseBinaryDataVector, data::SparseDoubleDataVector>(1);
        ToExampleTest<data::SparseBinaryDataVector, data::SparseFloatDataVector>(1);
        ToExampleTest<data::SparseBinaryDataVector, data::SparseShortDataVector>(1);
        ToExampleTest<data::SparseBinaryDataVector, data::SparseByteDataVector>(1);
        ToExampleTest<data::SparseBinaryDataVector, data::SparseBinaryDataVector>(2);

    }
}