////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     test.cpp (dataset_test)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SparseDataVector.h"
#include "DenseDataVector.h"
#include "SparseBinaryDataVector.h"
#include "ZeroDataVector.h"
#include "OnesDataVector.h"

// testing
#include "testing.h"

// linear
#include "DoubleVector.h"

// utilities
#include "StlIndexValueIterator.h"

// stl
#include <iostream>
#include <string>
#include <sstream>

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

/// Tests the Dot() member of DataVectors
///
template<typename DataVectorType>
void dotTest()
{
    auto a = getVector();
    auto b = getBinaryVector();

    DataVectorType c(b.GetIterator());

    double result = c.Dot(a);
    const double expectedResult = 2.2;

    testing::ProcessTest("Testing " + std::string(typeid(DataVectorType).name()) + "::Dot()", testing::IsEqual(result, expectedResult));
}

/// Tests the Dot() member of ZeroDataVector
///
void dotTestZeroDataVector()
{
    auto a = getVector();
    dataset::ZeroDataVector z;
    double result = z.Dot(a);
    testing::ProcessTest("Testing dataset::ZeroDataVector::Dot()", testing::IsEqual(result, 0.0));
}

/// Tests the Dot() member of OnesDataVector
///
void dotTestOnesDataVector()
{
    auto a = getVector();
    dataset::OnesDataVector o(4);
    double result = o.Dot(a);
    testing::ProcessTest("Testing dataset::OnesDataVector::Dot()", testing::IsEqual(result, 7.0));
}

/// Tests the Dot() member of DataVectors
///
void dotTest()
{
    dotTest<dataset::DoubleDataVector>();
    dotTest<dataset::FloatDataVector>();
    dotTest<dataset::SparseDoubleDataVector>();
    dotTest<dataset::SparseFloatDataVector>();
    dotTest<dataset::SparseShortDataVector>();
    dotTest<dataset::SparseBinaryDataVector>();
    dotTest<dataset::UncompressedSparseBinaryDataVector>();
    dotTestZeroDataVector();
    dotTestOnesDataVector();
}

/// Adds a DataVector to a linear::DoubleVector in two different ways and checks that the result is the same
///
template<typename DataVectorType1, typename DataVectorType2>
void addToTest()
{
    auto a1 = getVector();
    auto a2 = getVector();

    std::vector<double> b(15);
    b[3] = 1.0;
    b[4] = 1.0;
    b[12] = 1.0;
    b[13] = 1.0;

    DataVectorType1 c1(utilities::MakeStlIndexValueIterator(b));
    DataVectorType2 c2(utilities::MakeStlIndexValueIterator(b));

    c1.AddTo(a1);
    c2.AddTo(a2);

    std::string name1 = typeid(DataVectorType1).name();
    std::string name2 = typeid(DataVectorType2).name();
    
    testing::ProcessTest("Comparing AddTo() in " + name1 + " and " + name2, testing::IsEqual(a1, a2));
}

/// Tests the AddTo() member of ZeroDataVector
///
void addToTestZeroDataVector()
{
    auto a1 = getVector();
    auto a2 = getVector();

    dataset::ZeroDataVector z;

    z.AddTo(a1);

    testing::ProcessTest("Testing dataset::ZeroDataVector::AddTo()", testing::IsEqual(a1, a2));
}

/// Tests the AddTo() member of OnesDataVector
///
void addToTestOnesDataVector()
{
    auto a1 = getVector();
    auto a2 = getVector();

    dataset::OnesDataVector o(a1.Size());

    o.AddTo(a1);

    double norm1 = a1.Norm2();
    double norm2 = a2.Norm2() + 2 * o.Dot(a2) + a2.Size();

    testing::ProcessTest("Testing dataset::OnesDataVector::AddTo()", testing::IsEqual(norm1, norm2));
}

/// Tests the AddTo() member of DataVectors
///
void addToTest()
{
    addToTest<dataset::DoubleDataVector, dataset::FloatDataVector>();
    addToTest<dataset::DoubleDataVector, dataset::SparseDoubleDataVector>();
    addToTest<dataset::DoubleDataVector, dataset::SparseFloatDataVector>();
    addToTest<dataset::DoubleDataVector, dataset::SparseShortDataVector>();
    addToTest<dataset::DoubleDataVector, dataset::SparseBinaryDataVector>();
    addToTest<dataset::DoubleDataVector, dataset::UncompressedSparseBinaryDataVector>();
    addToTestZeroDataVector();
    addToTestOnesDataVector();
}

/// Casts one DataVector type into another and checks that the result is the same
///
template<typename DataVectorType1, typename DataVectorType2>
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
template<typename DataVectorType1, typename DataVectorType2>
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

void printTestOnesDataVector()
{
    dataset::DoubleDataVector a;
    for(int i = 0; i<15; ++i)
    {
        a.PushBack(i, 1);
    }

    dataset::OnesDataVector b(15);

    std::stringstream ass;
    std::stringstream bss;

    a.Print(ass);
    b.Print(bss);

    std::string as = ass.str();
    std::string bs = bss.str();

    testing::ProcessTest("Comparing class dataset::DoubleDataVector::Print() and class dataset::OnesDataVector::Print()", as == bs);
}

void printTestZeroDataVector()
{
    dataset::DoubleDataVector a;
    dataset::ZeroDataVector b;

    std::stringstream ass;
    std::stringstream bss;

    a.Print(ass);
    b.Print(bss);

    std::string as = ass.str();
    std::string bs = bss.str();

    testing::ProcessTest("Comparing class dataset::DoubleDataVector::Print() and class dataset::ZeroDataVector::Print()", as == bs);
}

void printTest()
{
    auto a = getVector();

    printTest<dataset::DoubleDataVector, dataset::FloatDataVector>(a);
    printTest<dataset::DoubleDataVector, dataset::SparseDoubleDataVector>(a);
    printTest<dataset::DoubleDataVector, dataset::SparseFloatDataVector>(a);

    auto b = getBinaryVector();

    printTest<dataset::DoubleDataVector, dataset::FloatDataVector>(b);
    printTest<dataset::DoubleDataVector, dataset::SparseDoubleDataVector>(b);
    printTest<dataset::DoubleDataVector, dataset::SparseFloatDataVector>(b);
    printTest<dataset::DoubleDataVector, dataset::SparseBinaryDataVector>(b);
    printTest<dataset::DoubleDataVector, dataset::UncompressedSparseBinaryDataVector>(b);

    printTestOnesDataVector();
    printTestZeroDataVector();
}

/// Runs all tests
///
int main()
{
    dotTest();
    addToTest();
    iteratorConstructorTest();
    printTest();

    if(testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}




