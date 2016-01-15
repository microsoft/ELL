// dataVectorTest.cpp

#include "SparseDataVector.h"
#include "DenseDataVector.h"
#include "SparseBinaryDataVector.h"
#include "ZeroDataVector.h"
#include "OnesDataVector.h"
using namespace dataset;

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

bool testFailed = false;

/// checks if two doubles are equal, up to a small numerical error
///
bool isEqual(double a, double b)
{
    const double epsilon = 1.0e-8;
    if(a-b < epsilon && b-a < epsilon)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool isEqual(const DoubleVector& a, const DoubleVector& b)
{
    for(int i = 0; i < a.Size(); ++i)
    {
        if(isEqual(a[i], b[i]) == false)
        {
            return false;
        }
    }
    return true;
}

/// Generates a DoubleVector, for the purpose of testing
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

void processTest(const string& testName, const string& typeName, bool success)
{
    cout << "Running " << testName << " test with " << typeName << " ... ";

    if (success)
    {
        cout << "Passed\n";
    }
    else
    {
        cout << "Failed\n";
        testFailed = true;
    }
}

template<typename DataVectorType>
void dotTest()
{
    auto a = getVector();

    DoubleArray b(15);
    b[3] = 1.0;
    b[4] = 1.0;
    b[12] = 1.0;
    b[13] = 1.0;

    DataVectorType c(b.GetIterator());

    double result = c.Dot(a);
    const double expectedResult = 2.2;

    processTest("Dot()", typeid(DataVectorType).name(), isEqual(result, expectedResult));
}

void dotTestZeroDataVector()
{
    auto a = getVector();
    ZeroDataVector z;
    double result = z.Dot(a);
    processTest("Dot()", "ZeroDataVector", isEqual(result, 0.0));
}

void dotTestOnesDataVector()
{
    auto a = getVector();
    OnesDataVector o(4);
    double result = o.Dot(a);
    processTest("Dot()", "OnesDataVector", isEqual(result, 7.0));
}

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

    processTest("AddTo()", name1 + " and " + name2, isEqual(a1, a2));
}

void addToTest()
{
    addToTest<DoubleDataVector, FloatDataVector>();
    addToTest<DoubleDataVector, SparseDoubleDataVector>();
    addToTest<DoubleDataVector, SparseFloatDataVector>();
    addToTest<DoubleDataVector, SparseShortDataVector>();
    addToTest<DoubleDataVector, SparseBinaryDataVector>();
    addToTest<DoubleDataVector, UncompressedSparseBinaryDataVector>();

    // TODO Zero and Ones DataVectors
}

int main()
{
    dotTest();
    addToTest();

    if(testFailed)
    {
        return 1;
    }

    return 0;
}




