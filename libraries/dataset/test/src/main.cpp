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
    DataVectorType u{0, 3, 4};
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




/// Casts one DataVector type into another and checks that the result is the same
///
template <typename DataVectorType1, typename DataVectorType2>
void iteratorConstructorTest(const linear::DoubleVector& a)
{
    DataVectorType1 b(a.GetIterator());
    DataVectorType2 c(b.GetIterator());

    linear::DoubleVector d(c.GetIterator());

    std::string name1 = typeid(DataVectorType1).name();
    std::string name2 = typeid(DataVectorType2).name();

    testing::ProcessTest("Casting " + name1 + " to " + name2, testing::IsEqual(a, d, 1.0e-6));
}

/// Tests the GetIterator() and Constructors
///
void iteratorConstructorTest()
{
    auto a = getVector();

    iteratorConstructorTest<dataset::DoubleDataVector, dataset::DoubleDataVector>(a);
    iteratorConstructorTest<dataset::DoubleDataVector, dataset::FloatDataVector>(a);
    iteratorConstructorTest<dataset::DoubleDataVector, dataset::SparseDoubleDataVector>(a);
    iteratorConstructorTest<dataset::DoubleDataVector, dataset::SparseFloatDataVector>(a);
    iteratorConstructorTest<dataset::FloatDataVector, dataset::DoubleDataVector>(a);
    iteratorConstructorTest<dataset::FloatDataVector, dataset::FloatDataVector>(a);
    iteratorConstructorTest<dataset::FloatDataVector, dataset::SparseDoubleDataVector>(a);
    iteratorConstructorTest<dataset::FloatDataVector, dataset::SparseFloatDataVector>(a);
    iteratorConstructorTest<dataset::SparseDoubleDataVector, dataset::DoubleDataVector>(a);
    iteratorConstructorTest<dataset::SparseDoubleDataVector, dataset::FloatDataVector>(a);
    iteratorConstructorTest<dataset::SparseDoubleDataVector, dataset::SparseDoubleDataVector>(a);
    iteratorConstructorTest<dataset::SparseDoubleDataVector, dataset::SparseFloatDataVector>(a);
    iteratorConstructorTest<dataset::SparseFloatDataVector, dataset::DoubleDataVector>(a);
    iteratorConstructorTest<dataset::SparseFloatDataVector, dataset::FloatDataVector>(a);
    iteratorConstructorTest<dataset::SparseFloatDataVector, dataset::SparseDoubleDataVector>(a);
    iteratorConstructorTest<dataset::SparseFloatDataVector, dataset::SparseFloatDataVector>(a);

    auto b = getBinaryVector();

    iteratorConstructorTest<dataset::DoubleDataVector, dataset::DoubleDataVector>(b);
    iteratorConstructorTest<dataset::DoubleDataVector, dataset::FloatDataVector>(b);
    iteratorConstructorTest<dataset::DoubleDataVector, dataset::SparseDoubleDataVector>(b);
    iteratorConstructorTest<dataset::DoubleDataVector, dataset::SparseFloatDataVector>(b);
    iteratorConstructorTest<dataset::DoubleDataVector, dataset::SparseShortDataVector>(b);
    iteratorConstructorTest<dataset::DoubleDataVector, dataset::SparseBinaryDataVector>(b);
    iteratorConstructorTest<dataset::DoubleDataVector, dataset::UncompressedSparseBinaryDataVector>(b);

    iteratorConstructorTest<dataset::FloatDataVector, dataset::DoubleDataVector>(b);
    iteratorConstructorTest<dataset::FloatDataVector, dataset::FloatDataVector>(b);
    iteratorConstructorTest<dataset::FloatDataVector, dataset::SparseFloatDataVector>(b);
    iteratorConstructorTest<dataset::FloatDataVector, dataset::SparseFloatDataVector>(b);
    iteratorConstructorTest<dataset::FloatDataVector, dataset::SparseShortDataVector>(b);
    iteratorConstructorTest<dataset::FloatDataVector, dataset::SparseBinaryDataVector>(b);
    iteratorConstructorTest<dataset::FloatDataVector, dataset::UncompressedSparseBinaryDataVector>(b);

    iteratorConstructorTest<dataset::SparseDoubleDataVector, dataset::DoubleDataVector>(b);
    iteratorConstructorTest<dataset::SparseDoubleDataVector, dataset::FloatDataVector>(b);
    iteratorConstructorTest<dataset::SparseDoubleDataVector, dataset::SparseDoubleDataVector>(b);
    iteratorConstructorTest<dataset::SparseDoubleDataVector, dataset::SparseFloatDataVector>(b);
    iteratorConstructorTest<dataset::SparseDoubleDataVector, dataset::SparseShortDataVector>(b);
    iteratorConstructorTest<dataset::SparseDoubleDataVector, dataset::SparseBinaryDataVector>(b);
    iteratorConstructorTest<dataset::SparseDoubleDataVector, dataset::UncompressedSparseBinaryDataVector>(b);

    iteratorConstructorTest<dataset::SparseFloatDataVector, dataset::DoubleDataVector>(b);
    iteratorConstructorTest<dataset::SparseFloatDataVector, dataset::FloatDataVector>(b);
    iteratorConstructorTest<dataset::SparseFloatDataVector, dataset::SparseFloatDataVector>(b);
    iteratorConstructorTest<dataset::SparseFloatDataVector, dataset::SparseFloatDataVector>(b);
    iteratorConstructorTest<dataset::SparseFloatDataVector, dataset::SparseShortDataVector>(b);
    iteratorConstructorTest<dataset::SparseFloatDataVector, dataset::SparseBinaryDataVector>(b);
    iteratorConstructorTest<dataset::SparseFloatDataVector, dataset::UncompressedSparseBinaryDataVector>(b);

    iteratorConstructorTest<dataset::SparseShortDataVector, dataset::DoubleDataVector>(b);
    iteratorConstructorTest<dataset::SparseShortDataVector, dataset::FloatDataVector>(b);
    iteratorConstructorTest<dataset::SparseShortDataVector, dataset::SparseFloatDataVector>(b);
    iteratorConstructorTest<dataset::SparseShortDataVector, dataset::SparseFloatDataVector>(b);
    iteratorConstructorTest<dataset::SparseShortDataVector, dataset::SparseShortDataVector>(b);
    iteratorConstructorTest<dataset::SparseShortDataVector, dataset::SparseBinaryDataVector>(b);
    iteratorConstructorTest<dataset::SparseShortDataVector, dataset::UncompressedSparseBinaryDataVector>(b);

    iteratorConstructorTest<dataset::SparseBinaryDataVector, dataset::DoubleDataVector>(b);
    iteratorConstructorTest<dataset::SparseBinaryDataVector, dataset::FloatDataVector>(b);
    iteratorConstructorTest<dataset::SparseBinaryDataVector, dataset::SparseFloatDataVector>(b);
    iteratorConstructorTest<dataset::SparseBinaryDataVector, dataset::SparseFloatDataVector>(b);
    iteratorConstructorTest<dataset::SparseBinaryDataVector, dataset::SparseShortDataVector>(b);
    iteratorConstructorTest<dataset::SparseBinaryDataVector, dataset::SparseBinaryDataVector>(b);
    iteratorConstructorTest<dataset::SparseBinaryDataVector, dataset::UncompressedSparseBinaryDataVector>(b);
}

/// Tests that two DataVector types print identically
///
template <typename DataVectorType1, typename DataVectorType2>
void printTest(const linear::DoubleVector& a)
{
    DataVectorType1 b1(a.GetIterator());
    DataVectorType2 b2(a.GetIterator());

    std::stringstream ss1;
    std::stringstream ss2;

    b1.Print(ss1);
    b2.Print(ss2);

    std::string s1 = ss1.str();
    std::string s2 = ss2.str();

    std::string name1 = typeid(DataVectorType1).name();
    std::string name2 = typeid(DataVectorType2).name();

    testing::ProcessTest("Comparing " + name1 + "::Print() and " + name2 + "::Print()", s1 == s2);
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

    IDataVectorBinaryTest<dataset::DoubleDataVector>();
    IDataVectorBinaryTest<dataset::FloatDataVector>();
    IDataVectorBinaryTest<dataset::ShortDataVector>();
    IDataVectorBinaryTest<dataset::ByteDataVector>();
    IDataVectorBinaryTest<dataset::SparseDoubleDataVector>();
    IDataVectorBinaryTest<dataset::SparseFloatDataVector>();
    IDataVectorBinaryTest<dataset::SparseShortDataVector>();
    IDataVectorBinaryTest<dataset::SparseByteDataVector>();
    IDataVectorBinaryTest<dataset::SparseBinaryDataVector>();

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}
