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

/// Generates a linear::DoubleVector
///
linear::DoubleVector getVector()
{
    linear::DoubleVector a(15);
    a[0] = 0.1;
    a[1] = 1.2;
    a[2] = 2.3;
    a[3] = 3.4;
    a[4] = 4.5;
    a[5] = 5.6;
    a[6] = 6.7;
    a[7] = 7.8;
    a[8] = 8.9;
    a[9] = 9.0;
    a[10] = -0.1;
    a[11] = -1.2;
    a[12] = -2.3;
    a[13] = -3.4;
    a[14] = -4.5;

    return a;
}

/// Generates a binary valued linear::DoubleVector
///
linear::DoubleVector getBinaryVector()
{
    linear::DoubleVector a(15);
    a[3] = 1;
    a[4] = 1;
    a[12] = 1;
    a[13] = 1;

    return a;
}

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

template <typename DataVectorType1, typename DataVectorType2>
void ToDataVectorTest(std::initializer_list<double> list)
{
    const DataVectorType1 v(list);
    auto u = v.ToDataVector<DataVectorType2>();
    auto w = v.ToArray();
    auto z = u.ToArray();

    std::string name1 = typeid(DataVectorType1).name();
    std::string name2 = typeid(DataVectorType2).name();

    testing::ProcessTest(name1 + "::ToDataVector<" + name2 + ">", testing::IsEqual(w,z, 1.0e-6));
}

void AutoDataVectorTest()
{
    dataset::AutoDataVector v1{ 0.123456789, 1.12345678901, 2.3456789012, 3.4567890123 };
    testing::ProcessTest("AutoDataVector ctor", v1.GetInternalType() == dataset::IDataVector::Type::DoubleDataVector);

    dataset::AutoDataVector v2{ 0.1f, 1.2f, 2.3f, 3.4f, 4.5f, 5.6f };
    testing::ProcessTest("AutoDataVector ctor", v2.GetInternalType() == dataset::IDataVector::Type::FloatDataVector);

    dataset::AutoDataVector v3{ 1234, 2345, 3456, 4567, 5678, 6789 };
    testing::ProcessTest("AutoDataVector ctor", v3.GetInternalType() == dataset::IDataVector::Type::ShortDataVector);

    dataset::AutoDataVector v4{ 10, 20, 30, 40, 50, 60, 70 };
    testing::ProcessTest("AutoDataVector ctor", v4.GetInternalType() == dataset::IDataVector::Type::ByteDataVector);

    dataset::AutoDataVector v5{ 0, 0, 0, 0, 0, 1.2345678901, 0, 0, 0 };
    testing::ProcessTest("AutoDataVector ctor", v5.GetInternalType() == dataset::IDataVector::Type::SparseDoubleDataVector);

    dataset::AutoDataVector v6{ 0, 0, 0, 0, 0, 1.2f, 0, 0, 0 };
    testing::ProcessTest("AutoDataVector ctor", v6.GetInternalType() == dataset::IDataVector::Type::SparseFloatDataVector);

    dataset::AutoDataVector v7{ 0, 0, 0, 0, 0, 1234, 0, 0, 0 };
    testing::ProcessTest("AutoDataVector ctor", v7.GetInternalType() == dataset::IDataVector::Type::SparseShortDataVector);

    dataset::AutoDataVector v8{ 0, 0, 0, 0, 0, 10, 0, 0, 0 };
    testing::ProcessTest("AutoDataVector ctor", v8.GetInternalType() == dataset::IDataVector::Type::SparseByteDataVector);

    dataset::AutoDataVector v9{ 0, 0, 0, 0, 0, 1, 0, 0, 0 };
    testing::ProcessTest("AutoDataVector ctor", v9.GetInternalType() == dataset::IDataVector::Type::SparseBinaryDataVector);
}


/// Runs all tests
///
int main()
{
    IDataVectorTest<dataset::DoubleDataVector>();
    IDataVectorTest<dataset::FloatDataVector>();
    IDataVectorTest<dataset::ShortDataVector>();
    IDataVectorTest<dataset::ByteDataVector>();
    IDataVectorTest<dataset::SparseDoubleDataVector>();
    IDataVectorTest<dataset::SparseFloatDataVector>();
    IDataVectorTest<dataset::SparseShortDataVector>();
    IDataVectorTest<dataset::SparseByteDataVector>();
    IDataVectorTest<dataset::AutoDataVector>();

    IDataVectorBinaryTest<dataset::DoubleDataVector>();
    IDataVectorBinaryTest<dataset::FloatDataVector>();
    IDataVectorBinaryTest<dataset::ShortDataVector>();
    IDataVectorBinaryTest<dataset::ByteDataVector>();
    IDataVectorBinaryTest<dataset::SparseDoubleDataVector>();
    IDataVectorBinaryTest<dataset::SparseFloatDataVector>();
    IDataVectorBinaryTest<dataset::SparseShortDataVector>();
    IDataVectorBinaryTest<dataset::SparseByteDataVector>();
    IDataVectorBinaryTest<dataset::AutoDataVector>();
    IDataVectorBinaryTest<dataset::SparseBinaryDataVector>();

    ToDataVectorTest<dataset::DoubleDataVector, dataset::DoubleDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<dataset::DoubleDataVector, dataset::FloatDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<dataset::DoubleDataVector, dataset::ShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::DoubleDataVector, dataset::ByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::DoubleDataVector, dataset::SparseDoubleDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<dataset::DoubleDataVector, dataset::SparseFloatDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<dataset::DoubleDataVector, dataset::SparseShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::DoubleDataVector, dataset::SparseByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::DoubleDataVector, dataset::SparseBinaryDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });

    ToDataVectorTest<dataset::FloatDataVector, dataset::DoubleDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<dataset::FloatDataVector, dataset::FloatDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<dataset::FloatDataVector, dataset::ShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::FloatDataVector, dataset::ByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::FloatDataVector, dataset::SparseDoubleDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<dataset::FloatDataVector, dataset::SparseFloatDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<dataset::FloatDataVector, dataset::SparseShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::FloatDataVector, dataset::SparseByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::FloatDataVector, dataset::SparseBinaryDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });

    ToDataVectorTest<dataset::ShortDataVector, dataset::DoubleDataVector>({ 1, 0, 11, 0, 0, 0, 0, 3, 0, 42 });
    ToDataVectorTest<dataset::ShortDataVector, dataset::FloatDataVector>({ 1, 0, 11, 0, 0, 0, 0, 3, 0, 42 });
    ToDataVectorTest<dataset::ShortDataVector, dataset::ShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::ShortDataVector, dataset::ByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::ShortDataVector, dataset::SparseDoubleDataVector>({ 1, 0, 11, 0, 0, 0, 0, 3, 0, 42 });
    ToDataVectorTest<dataset::ShortDataVector, dataset::SparseFloatDataVector>({ 1, 0, 11, 0, 0, 0, 0, 3, 0, 42 });
    ToDataVectorTest<dataset::ShortDataVector, dataset::SparseShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::ShortDataVector, dataset::SparseByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::ShortDataVector, dataset::SparseBinaryDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });

    ToDataVectorTest<dataset::ByteDataVector, dataset::DoubleDataVector>({ 1, 0, 11, 0, 0, 0, 0, 3, 0, 42 });
    ToDataVectorTest<dataset::ByteDataVector, dataset::FloatDataVector>({ 1, 0, 11, 0, 0, 0, 0, 3, 0, 42 });
    ToDataVectorTest<dataset::ByteDataVector, dataset::ShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::ByteDataVector, dataset::ByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::ByteDataVector, dataset::SparseDoubleDataVector>({ 1, 0, 11, 0, 0, 0, 0, 3, 0, 42 });
    ToDataVectorTest<dataset::ByteDataVector, dataset::SparseFloatDataVector>({ 1, 0, 11, 0, 0, 0, 0, 3, 0, 42 });
    ToDataVectorTest<dataset::ByteDataVector, dataset::SparseShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::ByteDataVector, dataset::SparseByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::ByteDataVector, dataset::SparseBinaryDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });

    ToDataVectorTest<dataset::SparseDoubleDataVector, dataset::DoubleDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<dataset::SparseDoubleDataVector, dataset::FloatDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<dataset::SparseDoubleDataVector, dataset::ShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::SparseDoubleDataVector, dataset::ByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::SparseDoubleDataVector, dataset::SparseDoubleDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<dataset::SparseDoubleDataVector, dataset::SparseFloatDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<dataset::SparseDoubleDataVector, dataset::SparseShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::SparseDoubleDataVector, dataset::SparseByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::SparseDoubleDataVector, dataset::SparseBinaryDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });

    ToDataVectorTest<dataset::SparseFloatDataVector, dataset::DoubleDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<dataset::SparseFloatDataVector, dataset::FloatDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<dataset::SparseFloatDataVector, dataset::ShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::SparseFloatDataVector, dataset::ByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::SparseFloatDataVector, dataset::SparseDoubleDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<dataset::SparseFloatDataVector, dataset::SparseFloatDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<dataset::SparseFloatDataVector, dataset::SparseShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::SparseFloatDataVector, dataset::SparseByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::SparseFloatDataVector, dataset::SparseBinaryDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });

    ToDataVectorTest<dataset::SparseShortDataVector, dataset::DoubleDataVector>({ 1, 0, 11, 0, 0, 0, 0, 3, 0, 42 });
    ToDataVectorTest<dataset::SparseShortDataVector, dataset::FloatDataVector>({ 1, 0, 11, 0, 0, 0, 0, 3, 0, 42 });
    ToDataVectorTest<dataset::SparseShortDataVector, dataset::ShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::SparseShortDataVector, dataset::ByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::SparseShortDataVector, dataset::SparseDoubleDataVector>({ 1, 0, 11, 0, 0, 0, 0, 3, 0, 42 });
    ToDataVectorTest<dataset::SparseShortDataVector, dataset::SparseFloatDataVector>({ 1, 0, 11, 0, 0, 0, 0, 3, 0, 42 });
    ToDataVectorTest<dataset::SparseShortDataVector, dataset::SparseShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::SparseShortDataVector, dataset::SparseByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::SparseShortDataVector, dataset::SparseBinaryDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });

    ToDataVectorTest<dataset::SparseByteDataVector, dataset::DoubleDataVector>({ 1, 0, 11, 0, 0, 0, 0, 3, 0, 42 });
    ToDataVectorTest<dataset::SparseByteDataVector, dataset::FloatDataVector>({ 1, 0, 11, 0, 0, 0, 0, 3, 0, 42 });
    ToDataVectorTest<dataset::SparseByteDataVector, dataset::ShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::SparseByteDataVector, dataset::ByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::SparseByteDataVector, dataset::SparseDoubleDataVector>({ 1, 0, 11, 0, 0, 0, 0, 3, 0, 42 });
    ToDataVectorTest<dataset::SparseByteDataVector, dataset::SparseFloatDataVector>({ 1, 0, 11, 0, 0, 0, 0, 3, 0, 42 });
    ToDataVectorTest<dataset::SparseByteDataVector, dataset::SparseShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::SparseByteDataVector, dataset::SparseByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::SparseByteDataVector, dataset::SparseBinaryDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });

    ToDataVectorTest<dataset::SparseBinaryDataVector, dataset::DoubleDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });
    ToDataVectorTest<dataset::SparseBinaryDataVector, dataset::FloatDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });
    ToDataVectorTest<dataset::SparseBinaryDataVector, dataset::ShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });
    ToDataVectorTest<dataset::SparseBinaryDataVector, dataset::ByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });
    ToDataVectorTest<dataset::SparseBinaryDataVector, dataset::SparseDoubleDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });
    ToDataVectorTest<dataset::SparseBinaryDataVector, dataset::SparseFloatDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });
    ToDataVectorTest<dataset::SparseBinaryDataVector, dataset::SparseShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });
    ToDataVectorTest<dataset::SparseBinaryDataVector, dataset::SparseByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });
    ToDataVectorTest<dataset::SparseBinaryDataVector, dataset::SparseBinaryDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });

    ToDataVectorTest<dataset::AutoDataVector, dataset::DoubleDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<dataset::AutoDataVector, dataset::FloatDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<dataset::AutoDataVector, dataset::ShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::AutoDataVector, dataset::ByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::AutoDataVector, dataset::SparseDoubleDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<dataset::AutoDataVector, dataset::SparseFloatDataVector>({ 1, 0, 1.1, 0, 0, 0, 0, 3, 0, 4.2 });
    ToDataVectorTest<dataset::AutoDataVector, dataset::SparseShortDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::AutoDataVector, dataset::SparseByteDataVector>({ 1, 0, 1, 0, 0, 0, 0, 3, 0, 4 });
    ToDataVectorTest<dataset::AutoDataVector, dataset::SparseBinaryDataVector>({ 1, 0, 1, 0, 0, 0, 0, 1, 0, 1 });

    AutoDataVectorTest();

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}
