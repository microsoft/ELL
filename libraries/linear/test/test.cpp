// test.cpp

#include "DoubleMatrix.h"
using linear::DoubleMatrix;
using linear::MatrixStructure;

#include "DoubleVector.h"
using linear::DoubleVector;

// stl
#include <iostream>
using std::cout;
using std::endl;

#include<string>
using std::string;
using std::to_string;

#include <random>
using std::default_random_engine;
using std::normal_distribution;

bool testFailed = false;

/// checks if two doubles are equal, up to a small numerical error
///
bool isEqual(double a, double b, double tolerance = 1.0e-8)
{
    if (a - b < tolerance && b - a < tolerance)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/// checks if two vectors are equal, up to a small numerical error in each coordinate
///
bool isEqual(const DoubleVector& a, const DoubleVector& b, double tolerance = 1.0e-8)
{
    for (int i = 0; i < a.Size(); ++i)
    {
        if (isEqual(a[i], b[i], tolerance) == false)
        {
            return false;
        }
    }
    return true;
}

/// Prints the results of a test and sets the global testFailed flag if appropriate
///
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

/// Fills a matrix with binary numbers (using a random generator with fixed seed)
///
template<typename MatrixType>
MatrixType getMatrix(int numRows, int numCols, int seed)
{
    default_random_engine rng(seed);
    normal_distribution<> dist(0,1);
    MatrixType M(numRows, numCols);
    for (int row = 0; row < numRows; ++row)
    {
        for (int col = 0; col < numCols; ++col)
        {
            M(row, col) = dist(rng);
        }
    }
    return M;
}

DoubleVector getVector(int size, int seed)
{
    default_random_engine rng(seed);
    normal_distribution<> dist(0, 1);
    DoubleVector v(size);
    for (int i = 0; i < size; ++i)
    {
        v[i] = dist(rng);
    }
    return v;
}

/// Tests Gemv()
///
template<typename MatrixType1, typename MatrixType2>
void testGemv(int numRows, int numColumns, double alpha, double beta)
{
    auto M1 = getMatrix<MatrixType1>(numRows, numColumns, 123);
    auto M2 = getMatrix<MatrixType2>(numRows, numColumns, 123);
    auto v = getVector(numColumns, 345);
    auto output1 = getVector(numRows, 567);
    auto output2 = getVector(numRows, 567);

    M1.Gemv(v, output1, alpha, beta);
    M2.Gemv(v, output2, alpha, beta);

    string name1 = typeid(MatrixType1).name();
    string name2 = typeid(MatrixType2).name();

    processTest("Gemv(" + to_string(alpha) + "," + to_string(beta) + ")", name1 + " and " + name2, isEqual(output1, output2, 1.0e-8));
}

/// Tests Gemv() for all matrix types
///
void testGemv()
{
    testGemv<DoubleMatrix<MatrixStructure::column>, DoubleMatrix<MatrixStructure::row>>(13, 17, 0, 0);
    testGemv<DoubleMatrix<MatrixStructure::column>, DoubleMatrix<MatrixStructure::row>>(13, 17, 1, 0);
    testGemv<DoubleMatrix<MatrixStructure::column>, DoubleMatrix<MatrixStructure::row>>(13, 17, 0, 1);
    testGemv<DoubleMatrix<MatrixStructure::column>, DoubleMatrix<MatrixStructure::row>>(13, 17, 1, 1);
}

/// Runs all tests
///
int main()
{
    testGemv();

    if(testFailed)
    {
        return 1;
    }

    return 0;
}




