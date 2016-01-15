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
using std::cerr;
using std::endl;

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

template<typename DataVectorType>
bool dotProductTest(const DoubleArray& a, const DoubleVector& b, double expectedResult)
{
    DataVectorType dataVector(a.GetIterator());
    double result = dataVector.Dot(b);
    return isEqual(result, expectedResult);
}

void dotProductTest()
{
    DoubleArray a(15);
    a[3] = 1.0;
    a[4] = 1.0;
    a[12] = 1.0;
    a[13] = 1.0;

    DoubleVector b(15);
    b[0] = 0.1;
    b[1] = 1.2;
    b[2] = 2.3;
    b[3] = 3.4;
    b[4] = 4.5;
    b[5] = 5.6;
    b[6] = 6.7;
    b[7] = 7.8;
    b[8] = 8.9;
    b[9] = 9.0;
    b[10] = -0.1;
    b[11] = -1.2;
    b[12] = -2.3;
    b[13] = -3.4;
    b[14] = -4.5;

    const double expectedResult = 2.2;

    // test vectors with content
    if(dotProductTest<DoubleDataVector>(a, b, expectedResult) == false)
    {
        cerr << "failed dot product test with DoubleDataVector" << endl;
        testFailed = true;
    }

    if(dotProductTest<FloatDataVector>(a, b, expectedResult) == false)
    {
        cerr << "failed dot product test with FloatDataVector" << endl;
        testFailed = true;
    }

    if(dotProductTest<SparseDoubleDataVector>(a, b, expectedResult) == false)
    {
        cerr << "failed dot product test with SparseDoubleDataVector" << endl;
        testFailed = true;
    }

    if(dotProductTest<SparseFloatDataVector>(a, b, expectedResult) == false)
    {
        cerr << "failed dot product test with SparseFloatDataVector" << endl;
        testFailed = true;
    }

    if(dotProductTest<SparseShortDataVector>(a, b, expectedResult) == false)
    {
        cerr << "failed dot product test with SparseShortDataVector" << endl;
        testFailed = true;
    }

    if(dotProductTest<SparseBinaryDataVector>(a, b, expectedResult) == false)
    {
        cerr << "failed dot product test with SparseBinaryDataVector" << endl;
        testFailed = true;
    }

    if(dotProductTest<UncompressedSparseBinaryDataVector>(a, b, expectedResult) == false)
    {
        cerr << "failed dot product test with UncompressedSparseBinaryDataVector" << endl;
        testFailed = true;
    }

    ZeroDataVector z;
    if(!isEqual(z.Dot(b), 0.0))
    {
        cerr << "failed dot product test with ZeroDataVector" << endl;
        testFailed = true;
    }

    // test ones vector
    OnesDataVector o(4);
    if(!isEqual(o.Dot(b), 7.0))
    {
        cerr << "failed dot product test with OnesDataVector" << endl;
        testFailed = true;
    }
}

int main()
{
    dotProductTest();

    if(testFailed)
    {
        return 1;
    }

    return 0;
}




