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

bool dotProductTest()
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
    if(dotProductTest<DoubleDataVector>(a, b, expectedResult) &&
        dotProductTest<FloatDataVector>(a, b, expectedResult) &&
        dotProductTest<SparseDoubleDataVector>(a, b, expectedResult) &&
        dotProductTest<SparseFloatDataVector>(a, b, expectedResult) &&
        dotProductTest<SparseShortDataVector>(a, b, expectedResult) &&
        dotProductTest<SparseBinaryDataVector>(a, b, expectedResult) &&
        dotProductTest<UncompressedSparseBinaryDataVector>(a, b, expectedResult) == false)
    {
        return false;
    }

    // test zero vector
    ZeroDataVector z;
    if(!isEqual(z.Dot(b), 0.0))
    {
        return false;
    }

    // test ones vector
    OnesDataVector o(4);
    if(!isEqual(o.Dot(b), 7.0))
    {
        return false;
    }

    return true;
}

int main()
{
    if(dotProductTest() == false)
    {
        return 1;
    }

    return 0;
}




