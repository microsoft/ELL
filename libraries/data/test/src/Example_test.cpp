////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
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

namespace ell
{
template <typename ExampleType>
ExampleType GetExample()
{
    using DataVectorType = typename ExampleType::DataVectorType;
    DataVectorType dataVector{ 1, 0, 1, 0, 1, 0, 1 };
    return ExampleType(std::make_shared<DataVectorType>(std::move(dataVector)), data::WeightLabel{ 1, 1 });
}

template <typename DataVectorType1, typename DataVectorType2>
void ExampleCopyAsTest()
{
    using ExampleType1 = data::Example<DataVectorType1, data::WeightLabel>;
    using ExampleType2 = data::Example<DataVectorType2, data::WeightLabel>;

    auto typeName1 = std::string(typeid(DataVectorType1).name());
    auto typeName2 = std::string(typeid(DataVectorType2).name());

    size_t expectedReferenceCount = (typeName1 == typeName2) ? 2 : 1;
    auto example1 = GetExample<ExampleType1>();
    auto example2 = data::CopyAs<ExampleType1, ExampleType2>(example1);
    bool isExpectedReferenceCount = (example2.GetDataVectorReferenceCount() == expectedReferenceCount);

    std::stringstream ss1, ss2;
    example1.Print(ss1);
    example2.Print(ss2);
    bool isSame = (ss1.str() == ss2.str());

    testing::ProcessTest("Testing " + typeName1 + "::CopyAs<" + typeName2 + ">", isSame && isExpectedReferenceCount);
}

template <typename DataVectorType>
void ExampleCopyAsTestDispatch()
{
    ExampleCopyAsTest<DataVectorType, data::AutoDataVector>();
    ExampleCopyAsTest<DataVectorType, data::DoubleDataVector>();
    ExampleCopyAsTest<DataVectorType, data::FloatDataVector>();
    ExampleCopyAsTest<DataVectorType, data::ShortDataVector>();
    ExampleCopyAsTest<DataVectorType, data::ByteDataVector>();
    ExampleCopyAsTest<DataVectorType, data::SparseDoubleDataVector>();
    ExampleCopyAsTest<DataVectorType, data::SparseFloatDataVector>();
    ExampleCopyAsTest<DataVectorType, data::SparseShortDataVector>();
    ExampleCopyAsTest<DataVectorType, data::SparseByteDataVector>();
    ExampleCopyAsTest<DataVectorType, data::SparseBinaryDataVector>();
}

void ExampleCopyAsTests()
{
    ExampleCopyAsTestDispatch<data::AutoDataVector>();
    ExampleCopyAsTestDispatch<data::DoubleDataVector>();
    ExampleCopyAsTestDispatch<data::FloatDataVector>();
    ExampleCopyAsTestDispatch<data::ShortDataVector>();
    ExampleCopyAsTestDispatch<data::ByteDataVector>();
    ExampleCopyAsTestDispatch<data::SparseDoubleDataVector>();
    ExampleCopyAsTestDispatch<data::SparseFloatDataVector>();
    ExampleCopyAsTestDispatch<data::SparseShortDataVector>();
    ExampleCopyAsTestDispatch<data::SparseByteDataVector>();
    ExampleCopyAsTestDispatch<data::SparseBinaryDataVector>();
}
}