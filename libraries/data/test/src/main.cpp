////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     main.cpp (dataset_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DenseDataVector.h"
#include "SparseBinaryDataVector.h"
#include "SparseDataVector.h"
#include "AutoDataVector.h"
#include "DataVector.h"

// testing
#include "testing.h"

// linear
#include "DoubleVector.h"

// utilities
#include "StlIndexValueIterator.h"

// stl
#include <iostream>
#include <sstream>
#include <string>
#include <cmath>

using namespace emll;

template<typename DataVectorType>
void IDataVectorTest()
{
    DataVectorType u{{0,12}, {3,-7}, {4,1}};
    testing::ProcessTest("Testing " + std::string(typeid(DataVectorType).name()) + "::Norm2()", testing::IsEqual(u.Norm2(), std::sqrt(12*12+7*7+1*1)));

    std::vector<double> w{1, 1, 1, 1, 1, 1};
    testing::ProcessTest("Testing " + std::string(typeid(DataVectorType).name()) + "::Dot()", testing::IsEqual(u.Dot(w.data()), 12.0-7.0+1.0));

    u.AddTo(w.data(), 2);
    std::vector<double> z{25, 1, 1, -13, 3, 1};
    testing::ProcessTest("Testing " + std::string(typeid(DataVectorType).name()) + "::AddTo()", testing::IsEqual(w, z));

    std::stringstream ss;
    u.Print(ss);
    auto sss = ss.str();
    testing::ProcessTest("Testing " + std::string(typeid(DataVectorType).name()) + "::Print()", sss == "0:12\t3:-7\t4:1");
}

template<typename DataVectorType>
void IDataVectorBinaryTest()
{
    DataVectorType u{{0,1}, {3,1}, {4,1}};
    testing::ProcessTest("Testing " + std::string(typeid(DataVectorType).name()) + "::Norm2()", testing::IsEqual(u.Norm2(), std::sqrt(3)));

    std::vector<double> w{1, 2, 3, 4, 5, 6};
    testing::ProcessTest("Testing " + std::string(typeid(DataVectorType).name()) + "::Dot()", testing::IsEqual(u.Dot(w.data()), 1.0+4.0+5.0));

    u.AddTo(w.data(), 2);
    std::vector<double> z{3, 2, 3, 6, 7, 6};
    testing::ProcessTest("Testing " + std::string(typeid(DataVectorType).name()) + "::AddTo()", testing::IsEqual(w, z));

    std::stringstream ss;
    u.Print(ss);
    auto sss = ss.str();
    testing::ProcessTest("Testing " + std::string(typeid(DataVectorType).name()) + "::Print()", sss == "0:1\t3:1\t4:1");
}

void IDataVectorTests()
{
    IDataVectorTest<data::DoubleDataVector>();
    IDataVectorTest<data::FloatDataVector>();
    IDataVectorTest<data::ShortDataVector>();
    IDataVectorTest<data::ByteDataVector>();
    IDataVectorTest<data::SparseDoubleDataVector>();
    IDataVectorTest<data::SparseFloatDataVector>();
    IDataVectorTest<data::SparseShortDataVector>();
    IDataVectorTest<data::SparseByteDataVector>();
    IDataVectorTest<data::AutoDataVector>();

    IDataVectorBinaryTest<data::DoubleDataVector>();
    IDataVectorBinaryTest<data::FloatDataVector>();
    IDataVectorBinaryTest<data::ShortDataVector>();
    IDataVectorBinaryTest<data::ByteDataVector>();
    IDataVectorBinaryTest<data::SparseDoubleDataVector>();
    IDataVectorBinaryTest<data::SparseFloatDataVector>();
    IDataVectorBinaryTest<data::SparseShortDataVector>();
    IDataVectorBinaryTest<data::SparseByteDataVector>();
    IDataVectorBinaryTest<data::AutoDataVector>();
    IDataVectorBinaryTest<data::SparseBinaryDataVector>();
}

template <typename DataVectorType1, typename DataVectorType2>
void ToDataVectorTest(std::initializer_list<double> list)
{
    const DataVectorType1 v(list);
    auto u = v.Duplicate<DataVectorType2>();
    auto w = v.ToArray();
    auto z = u.ToArray();

    std::string name1 = typeid(DataVectorType1).name();
    std::string name2 = typeid(DataVectorType2).name();

    testing::ProcessTest(name1 + "::Duplicate<" + name2 + ">", testing::IsEqual(w,z, 1.0e-6));
}

void ToDataVectorTests()
{
    ToDataVectorTest<data::DoubleDataVector, data::DoubleDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<data::DoubleDataVector, data::FloatDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<data::DoubleDataVector, data::ShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::DoubleDataVector, data::ByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::DoubleDataVector, data::SparseDoubleDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<data::DoubleDataVector, data::SparseFloatDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<data::DoubleDataVector, data::SparseShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::DoubleDataVector, data::SparseByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::DoubleDataVector, data::SparseBinaryDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });
    ToDataVectorTest<data::DoubleDataVector, data::AutoDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });

    ToDataVectorTest<data::FloatDataVector, data::DoubleDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<data::FloatDataVector, data::FloatDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<data::FloatDataVector, data::ShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::FloatDataVector, data::ByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::FloatDataVector, data::SparseDoubleDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<data::FloatDataVector, data::SparseFloatDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<data::FloatDataVector, data::SparseShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::FloatDataVector, data::SparseByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::FloatDataVector, data::SparseBinaryDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });
    ToDataVectorTest<data::FloatDataVector, data::AutoDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });

    ToDataVectorTest<data::ShortDataVector, data::DoubleDataVector>({ 1, 0, 11, 0, 0, 0, 0, 3, 0, 42 });
    ToDataVectorTest<data::ShortDataVector, data::FloatDataVector>({ 1, 0, 11, 0, 0, 0, 0, 3, 0, 42 });
    ToDataVectorTest<data::ShortDataVector, data::ShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::ShortDataVector, data::ByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::ShortDataVector, data::SparseDoubleDataVector>({ 1, 0, 11, 0, 0, 0, 0, 3, 0, 42 });
    ToDataVectorTest<data::ShortDataVector, data::SparseFloatDataVector>({ 1, 0, 11, 0, 0, 0, 0, 3, 0, 42 });
    ToDataVectorTest<data::ShortDataVector, data::SparseShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::ShortDataVector, data::SparseByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::ShortDataVector, data::SparseBinaryDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });
    ToDataVectorTest<data::ShortDataVector, data::AutoDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });

    ToDataVectorTest<data::ByteDataVector, data::DoubleDataVector>({ 1, 0, 11, 0, 0, 0, 0, 3, 0, 42 });
    ToDataVectorTest<data::ByteDataVector, data::FloatDataVector>({ 1, 0, 11, 0, 0, 0, 0, 3, 0, 42 });
    ToDataVectorTest<data::ByteDataVector, data::ShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::ByteDataVector, data::ByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::ByteDataVector, data::SparseDoubleDataVector>({ 1, 0, 11, 0, 0, 0, 0, 3, 0, 42 });
    ToDataVectorTest<data::ByteDataVector, data::SparseFloatDataVector>({ 1, 0, 11, 0, 0, 0, 0, 3, 0, 42 });
    ToDataVectorTest<data::ByteDataVector, data::SparseShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::ByteDataVector, data::SparseByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::ByteDataVector, data::SparseBinaryDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });
    ToDataVectorTest<data::ByteDataVector, data::AutoDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });

    ToDataVectorTest<data::SparseDoubleDataVector, data::DoubleDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<data::SparseDoubleDataVector, data::FloatDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<data::SparseDoubleDataVector, data::ShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::SparseDoubleDataVector, data::ByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::SparseDoubleDataVector, data::SparseDoubleDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<data::SparseDoubleDataVector, data::SparseFloatDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<data::SparseDoubleDataVector, data::SparseShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::SparseDoubleDataVector, data::SparseByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::SparseDoubleDataVector, data::SparseBinaryDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });
    ToDataVectorTest<data::SparseDoubleDataVector, data::AutoDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });

    ToDataVectorTest<data::SparseFloatDataVector, data::DoubleDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<data::SparseFloatDataVector, data::FloatDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<data::SparseFloatDataVector, data::ShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::SparseFloatDataVector, data::ByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::SparseFloatDataVector, data::SparseDoubleDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<data::SparseFloatDataVector, data::SparseFloatDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<data::SparseFloatDataVector, data::SparseShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::SparseFloatDataVector, data::SparseByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::SparseFloatDataVector, data::SparseBinaryDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });
    ToDataVectorTest<data::SparseFloatDataVector, data::AutoDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });

    ToDataVectorTest<data::SparseShortDataVector, data::DoubleDataVector>({ 1, 0, 11, 0, 0, 0, 0, 3, 0, 42 });
    ToDataVectorTest<data::SparseShortDataVector, data::FloatDataVector>({ 1, 0, 11, 0, 0, 0, 0, 3, 0, 42 });
    ToDataVectorTest<data::SparseShortDataVector, data::ShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::SparseShortDataVector, data::ByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::SparseShortDataVector, data::SparseDoubleDataVector>({ 1, 0, 11, 0, 0, 0, 0, 3, 0, 42 });
    ToDataVectorTest<data::SparseShortDataVector, data::SparseFloatDataVector>({ 1, 0, 11, 0, 0, 0, 0, 3, 0, 42 });
    ToDataVectorTest<data::SparseShortDataVector, data::SparseShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::SparseShortDataVector, data::SparseByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::SparseShortDataVector, data::SparseBinaryDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });
    ToDataVectorTest<data::SparseShortDataVector, data::AutoDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });

    ToDataVectorTest<data::SparseByteDataVector, data::DoubleDataVector>({ 1, 0, 11, 0, 0, 0, 0, 3, 0, 42 });
    ToDataVectorTest<data::SparseByteDataVector, data::FloatDataVector>({ 1, 0, 11, 0, 0, 0, 0, 3, 0, 42 });
    ToDataVectorTest<data::SparseByteDataVector, data::ShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::SparseByteDataVector, data::ByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::SparseByteDataVector, data::SparseDoubleDataVector>({ 1, 0, 11, 0, 0, 0, 0, 3, 0, 42 });
    ToDataVectorTest<data::SparseByteDataVector, data::SparseFloatDataVector>({ 1, 0, 11, 0, 0, 0, 0, 3, 0, 42 });
    ToDataVectorTest<data::SparseByteDataVector, data::SparseShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::SparseByteDataVector, data::SparseByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::SparseByteDataVector, data::SparseBinaryDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });
    ToDataVectorTest<data::SparseByteDataVector, data::AutoDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });

    ToDataVectorTest<data::SparseBinaryDataVector, data::DoubleDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });
    ToDataVectorTest<data::SparseBinaryDataVector, data::FloatDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });
    ToDataVectorTest<data::SparseBinaryDataVector, data::ShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });
    ToDataVectorTest<data::SparseBinaryDataVector, data::ByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });
    ToDataVectorTest<data::SparseBinaryDataVector, data::SparseDoubleDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });
    ToDataVectorTest<data::SparseBinaryDataVector, data::SparseFloatDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });
    ToDataVectorTest<data::SparseBinaryDataVector, data::SparseShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });
    ToDataVectorTest<data::SparseBinaryDataVector, data::SparseByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });
    ToDataVectorTest<data::SparseBinaryDataVector, data::SparseBinaryDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });
    ToDataVectorTest<data::SparseBinaryDataVector, data::AutoDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });

    ToDataVectorTest<data::AutoDataVector, data::DoubleDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<data::AutoDataVector, data::FloatDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<data::AutoDataVector, data::ShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::AutoDataVector, data::ByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::AutoDataVector, data::SparseDoubleDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<data::AutoDataVector, data::SparseFloatDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<data::AutoDataVector, data::SparseShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::AutoDataVector, data::SparseByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<data::AutoDataVector, data::SparseBinaryDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });
}

void AutoDataVectorTest()
{
    data::AutoDataVector v1{ 0.123456789, 1.12345678901, 2.3456789012, 3.4567890123 };
    testing::ProcessTest("AutoDataVector ctor", v1.GetInternalType() == data::IDataVector::Type::DoubleDataVector);

    data::AutoDataVector v2{ 0.1f, 1.2f, 2.3f, 3.4f, 4.5f, 5.6f };
    testing::ProcessTest("AutoDataVector ctor", v2.GetInternalType() == data::IDataVector::Type::FloatDataVector);

    data::AutoDataVector v3{ 1234, 2345, 3456, 4567, 5678, 6789 };
    testing::ProcessTest("AutoDataVector ctor", v3.GetInternalType() == data::IDataVector::Type::ShortDataVector);

    data::AutoDataVector v4{ 10, 20, 30, 40, 50, 60, 70 };
    testing::ProcessTest("AutoDataVector ctor", v4.GetInternalType() == data::IDataVector::Type::ByteDataVector);

    data::AutoDataVector v5{ 0, 0, 0, 0, 0, 1.2345678901, 0, 0, 0 };
    testing::ProcessTest("AutoDataVector ctor", v5.GetInternalType() == data::IDataVector::Type::SparseDoubleDataVector);

    data::AutoDataVector v6{ 0, 0, 0, 0, 0, 1.2f, 0, 0, 0 };
    testing::ProcessTest("AutoDataVector ctor", v6.GetInternalType() == data::IDataVector::Type::SparseFloatDataVector);

    data::AutoDataVector v7{ 0, 0, 0, 0, 0, 1234, 0, 0, 0 };
    testing::ProcessTest("AutoDataVector ctor", v7.GetInternalType() == data::IDataVector::Type::SparseShortDataVector);

    data::AutoDataVector v8{ 0, 0, 0, 0, 0, 10, 0, 0, 0 };
    testing::ProcessTest("AutoDataVector ctor", v8.GetInternalType() == data::IDataVector::Type::SparseByteDataVector);

    data::AutoDataVector v9{ 0, 0, 0, 0, 0, 1, 0, 0, 0 };
    testing::ProcessTest("AutoDataVector ctor", v9.GetInternalType() == data::IDataVector::Type::SparseBinaryDataVector);
}

/// Runs all tests
///
int main()
{
    IDataVectorTests();
    ToDataVectorTests();
    AutoDataVectorTest();

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}
