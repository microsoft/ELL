// test.cpp

#include "SparseDataVector.h"
#include "DenseDataVector.h"
#include "SparseBinaryDataVector.h"
#include "ZeroDataVector.h"
#include "OnesDataVector.h"
using namespace dataset;

// testing
#include "testing.h"
using testing::isEqual;
using testing::processTest;
using testing::testFailed;

// types
#include "RealArray.h"
using types::DoubleArray;

// linear
#include "DoubleVector.h"
using linear::DoubleVector;

// stl
#include <iostream>
using std::cout;
using std::endl;

#include<string>
using std::string;

#include <sstream>
using std::stringstream;

/// Generates a DoubleVector
///
DoubleVector getVector()
{
    DoubleVector a(15);
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

/// Generates a binary valued DoubleVector
///
DoubleVector getBinaryVector()
{
    DoubleVector a(15);
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

    processTest("Testing " + string(typeid(DataVectorType).name()) + "::Dot()", isEqual(result, expectedResult));
}

/// Tests the Dot() member of ZeroDataVector
///
void dotTestZeroDataVector()
{
    auto a = getVector();
    ZeroDataVector z;
    double result = z.Dot(a);
    processTest("Testing dataset::ZeroDataVector::Dot()", isEqual(result, 0.0));
}

/// Tests the Dot() member of OnesDataVector
///
void dotTestOnesDataVector()
{
    auto a = getVector();
    OnesDataVector o(4);
    double result = o.Dot(a);
    processTest("Testing dataset::OnesDataVector::Dot()", isEqual(result, 7.0));
}

/// Tests the Dot() member of DataVectors
///
void dotTest()
{
    dotTest<DoubleDataVector>();
    dotTest<FloatDataVector>();
    dotTest<SparseDoubleDataVector>();
    dotTest<SparseFloatDataVector>();
    dotTest<SparseShortDataVector>();
    dotTest<SparseBinaryDataVector>();
    dotTest<UncompressedSparseBinaryDataVector>();
    dotTestZeroDataVector();
    dotTestOnesDataVector();
}

/// Adds a DataVector to a DoubleVector in two different ways and checks that the result is the same
///
template<typename DataVectorType1, typename DataVectorType2>
void addToTest()
{
    auto a1 = getVector();
    auto a2 = getVector();

    DoubleArray b(15);
    b[3] = 1.0;
    b[4] = 1.0;
    b[12] = 1.0;
    b[13] = 1.0;

    DataVectorType1 c1(b.GetIterator());
    DataVectorType2 c2(b.GetIterator());

    c1.AddTo(a1);
    c2.AddTo(a2);

    string name1 = typeid(DataVectorType1).name();
    string name2 = typeid(DataVectorType2).name();

    processTest("Comparing AddTo() in " + name1 + " and " + name2, isEqual(a1, a2));
}

/// Tests the AddTo() member of ZeroDataVector
///
void addToTestZeroDataVector()
{
    auto a1 = getVector();
    auto a2 = getVector();

    ZeroDataVector z;

    z.AddTo(a1);

    processTest("Testing dataset::ZeroDataVector::AddTo()", isEqual(a1, a2));
}

/// Tests the AddTo() member of OnesDataVector
///
void addToTestOnesDataVector()
{
    auto a1 = getVector();
    auto a2 = getVector();

    OnesDataVector o(a1.Size());

    o.AddTo(a1);

    double norm1 = a1.Norm2();
    double norm2 = a2.Norm2() + 2 * o.Dot(a2) + a2.Size();

    processTest("Testing dataset::OnesDataVector::AddTo()", isEqual(norm1, norm2));
}

/// Tests the AddTo() member of DataVectors
///
void addToTest()
{
    addToTest<DoubleDataVector, FloatDataVector>();
    addToTest<DoubleDataVector, SparseDoubleDataVector>();
    addToTest<DoubleDataVector, SparseFloatDataVector>();
    addToTest<DoubleDataVector, SparseShortDataVector>();
    addToTest<DoubleDataVector, SparseBinaryDataVector>();
    addToTest<DoubleDataVector, UncompressedSparseBinaryDataVector>();
    addToTestZeroDataVector();
    addToTestOnesDataVector();
}

/// Casts one DataVector type into another and checks that the result is the same
///
template<typename DataVectorType1, typename DataVectorType2>
void iteratorConstructorTest(const DoubleVector& a)
{
    DataVectorType1 b(a.GetIterator());
    DataVectorType2 c(b.GetIterator());

    DoubleVector d(c.GetIterator());

    string name1 = typeid(DataVectorType1).name();
    string name2 = typeid(DataVectorType2).name();

    processTest("Casting " + name1 + " to " + name2, isEqual(a, d, 1.0e-6));
}

/// Tests the GetIterator() and Constructors
///
void iteratorConstructorTest()
{
    auto a = getVector();

    iteratorConstructorTest<DoubleDataVector, DoubleDataVector>(a);
    iteratorConstructorTest<DoubleDataVector, FloatDataVector>(a);
    iteratorConstructorTest<DoubleDataVector, SparseDoubleDataVector>(a);
    iteratorConstructorTest<DoubleDataVector, SparseFloatDataVector>(a);
    iteratorConstructorTest<FloatDataVector, DoubleDataVector>(a);
    iteratorConstructorTest<FloatDataVector, FloatDataVector>(a);
    iteratorConstructorTest<FloatDataVector, SparseDoubleDataVector>(a);
    iteratorConstructorTest<FloatDataVector, SparseFloatDataVector>(a);
    iteratorConstructorTest<SparseDoubleDataVector, DoubleDataVector>(a);
    iteratorConstructorTest<SparseDoubleDataVector, FloatDataVector>(a);
    iteratorConstructorTest<SparseDoubleDataVector, SparseDoubleDataVector>(a);
    iteratorConstructorTest<SparseDoubleDataVector, SparseFloatDataVector>(a);
    iteratorConstructorTest<SparseFloatDataVector, DoubleDataVector>(a);
    iteratorConstructorTest<SparseFloatDataVector, FloatDataVector>(a);
    iteratorConstructorTest<SparseFloatDataVector, SparseDoubleDataVector>(a);
    iteratorConstructorTest<SparseFloatDataVector, SparseFloatDataVector>(a);

    auto b = getBinaryVector();

    iteratorConstructorTest<DoubleDataVector, DoubleDataVector>(b);
    iteratorConstructorTest<DoubleDataVector, FloatDataVector>(b);
    iteratorConstructorTest<DoubleDataVector, SparseDoubleDataVector>(b);
    iteratorConstructorTest<DoubleDataVector, SparseFloatDataVector>(b);
    iteratorConstructorTest<DoubleDataVector, SparseShortDataVector>(b);
    iteratorConstructorTest<DoubleDataVector, SparseBinaryDataVector>(b);
    iteratorConstructorTest<DoubleDataVector, UncompressedSparseBinaryDataVector>(b);

    iteratorConstructorTest<FloatDataVector, DoubleDataVector>(b);
    iteratorConstructorTest<FloatDataVector, FloatDataVector>(b);
    iteratorConstructorTest<FloatDataVector, SparseFloatDataVector>(b);
    iteratorConstructorTest<FloatDataVector, SparseFloatDataVector>(b);
    iteratorConstructorTest<FloatDataVector, SparseShortDataVector>(b);
    iteratorConstructorTest<FloatDataVector, SparseBinaryDataVector>(b);
    iteratorConstructorTest<FloatDataVector, UncompressedSparseBinaryDataVector>(b);

    iteratorConstructorTest<SparseDoubleDataVector, DoubleDataVector>(b);
    iteratorConstructorTest<SparseDoubleDataVector, FloatDataVector>(b);
    iteratorConstructorTest<SparseDoubleDataVector, SparseDoubleDataVector>(b);
    iteratorConstructorTest<SparseDoubleDataVector, SparseFloatDataVector>(b);
    iteratorConstructorTest<SparseDoubleDataVector, SparseShortDataVector>(b);
    iteratorConstructorTest<SparseDoubleDataVector, SparseBinaryDataVector>(b);
    iteratorConstructorTest<SparseDoubleDataVector, UncompressedSparseBinaryDataVector>(b);

    iteratorConstructorTest<SparseFloatDataVector, DoubleDataVector>(b);
    iteratorConstructorTest<SparseFloatDataVector, FloatDataVector>(b);
    iteratorConstructorTest<SparseFloatDataVector, SparseFloatDataVector>(b);
    iteratorConstructorTest<SparseFloatDataVector, SparseFloatDataVector>(b);
    iteratorConstructorTest<SparseFloatDataVector, SparseShortDataVector>(b);
    iteratorConstructorTest<SparseFloatDataVector, SparseBinaryDataVector>(b);
    iteratorConstructorTest<SparseFloatDataVector, UncompressedSparseBinaryDataVector>(b);

    iteratorConstructorTest<SparseShortDataVector, DoubleDataVector>(b);
    iteratorConstructorTest<SparseShortDataVector, FloatDataVector>(b);
    iteratorConstructorTest<SparseShortDataVector, SparseFloatDataVector>(b);
    iteratorConstructorTest<SparseShortDataVector, SparseFloatDataVector>(b);
    iteratorConstructorTest<SparseShortDataVector, SparseShortDataVector>(b);
    iteratorConstructorTest<SparseShortDataVector, SparseBinaryDataVector>(b);
    iteratorConstructorTest<SparseShortDataVector, UncompressedSparseBinaryDataVector>(b);

    iteratorConstructorTest<SparseBinaryDataVector, DoubleDataVector>(b);
    iteratorConstructorTest<SparseBinaryDataVector, FloatDataVector>(b);
    iteratorConstructorTest<SparseBinaryDataVector, SparseFloatDataVector>(b);
    iteratorConstructorTest<SparseBinaryDataVector, SparseFloatDataVector>(b);
    iteratorConstructorTest<SparseBinaryDataVector, SparseShortDataVector>(b);
    iteratorConstructorTest<SparseBinaryDataVector, SparseBinaryDataVector>(b);
    iteratorConstructorTest<SparseBinaryDataVector, UncompressedSparseBinaryDataVector>(b);
}

/// Tests that two DataVector types print identically
///
template<typename DataVectorType1, typename DataVectorType2>
void printTest(const DoubleVector& a)
{
    DataVectorType1 b1(a.GetIterator());
    DataVectorType2 b2(a.GetIterator());

    stringstream ss1;
    stringstream ss2;

    b1.Print(ss1);
    b2.Print(ss2);

    string s1 = ss1.str();
    string s2 = ss2.str();

    string name1 = typeid(DataVectorType1).name();
    string name2 = typeid(DataVectorType2).name();

    processTest("Comparing " + name1 + "::Print() and " + name2 + "::Print()", s1 == s2);
}

void printTestOnesDataVector()
{
    DoubleDataVector a;
    for(int i = 0; i<15; ++i)
    {
        a.PushBack(i, 1);
    }

    OnesDataVector b(15);

    stringstream ass;
    stringstream bss;

    a.Print(ass);
    b.Print(bss);

    string as = ass.str();
    string bs = bss.str();

    processTest("Comparing class dataset::DoubleDataVector::Print() and class dataset::OnesDataVector::Print()", as == bs);
}

void printTestZeroDataVector()
{
    DoubleDataVector a;
    ZeroDataVector b;

    stringstream ass;
    stringstream bss;

    a.Print(ass);
    b.Print(bss);

    string as = ass.str();
    string bs = bss.str();

    processTest("Comparing class dataset::DoubleDataVector::Print() and class dataset::ZeroDataVector::Print()", as == bs);
}

void printTest()
{
    auto a = getVector();

    printTest<DoubleDataVector, FloatDataVector>(a);
    printTest<DoubleDataVector, SparseDoubleDataVector>(a);
    printTest<DoubleDataVector, SparseFloatDataVector>(a);

    auto b = getBinaryVector();

    printTest<DoubleDataVector, FloatDataVector>(b);
    printTest<DoubleDataVector, SparseDoubleDataVector>(b);
    printTest<DoubleDataVector, SparseFloatDataVector>(b);
    printTest<DoubleDataVector, SparseBinaryDataVector>(b);
    printTest<DoubleDataVector, UncompressedSparseBinaryDataVector>(b);

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

    // TODO - test DataVector pushback and NumNonzeros
    // TODO - test integer list and compressed integer list
    // TODO - test data vector builder and supervised example builder
    // TODO - test supervised example
    // TODO - test parsing iterator, sequential line iterator, and sparse entry parser 

    if(testFailed())
    {
        return 1;
    }

    return 0;
}




