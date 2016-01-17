// test.cpp

#include "DoubleMatrix.h"
using linear::DoubleMatrix;
using linear::MatrixStructure;

#include "DoubleVector.h"
using linear::DoubleVector;

// types
#include "types.h"

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
void fillMatrix(MatrixType& M, int seed, bool lowerTriangle, bool upperTriangle)
{
    default_random_engine rng(seed);
    normal_distribution<> dist(0,1);
    for (uint64 row = 0; row < M.NumRows(); ++row)
    {
        uint64 fromColumn = lowerTriangle ? 0 : row;
        uint64 toColumn = upperTriangle ? M.NumColumns() : row;
        for (uint64 col = fromColumn; col < toColumn; ++col)
        {
            M(row, col) = dist(rng);
        }
    }
}

DoubleVector getVector(uint64 size, int seed)
{
    default_random_engine rng(seed);
    normal_distribution<> dist(0, 1);
    DoubleVector v(size);
    for (uint64 i = 0; i < size; ++i)
    {
        v[i] = dist(rng);
    }
    return v;
}

/// Tests Gemv()
///
template<typename MatrixType1, typename MatrixType2>
void testGemv(const MatrixType1& M1, const MatrixType2& M2, double alpha, double beta)
{
    auto v = getVector(M1.NumColumns(), 345);
    auto output1 = getVector(M1.NumRows(), 567);
    auto output2 = getVector(M1.NumRows(), 567);

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
    DoubleMatrix<MatrixStructure::column> M1(13, 17);
    DoubleMatrix<MatrixStructure::row> M2(13, 17);
    fillMatrix(M1, 123, true, true);
    fillMatrix(M2, 123, true, true);

    testGemv(M1, M2, 0, 0);
    testGemv(M1, M2, 1, 0);
    testGemv(M1, M2, 0, 1);
    testGemv(M1, M2, 1, 1);

    DoubleMatrix<MatrixStructure::column> M3(17, 17);
    DoubleMatrix<MatrixStructure::columnSquare> M4(17);
    fillMatrix(M3, 123, true, true);
    fillMatrix(M4, 123, true, true);

    testGemv(M3, M4, 0, 0);
    testGemv(M3, M4, 1, 0);
    testGemv(M3, M4, 0, 1);
    testGemv(M3, M4, 1, 1);

    DoubleMatrix<MatrixStructure::rowSquare> M5(17);
    fillMatrix(M5, 123, true, true);

    testGemv(M3, M5, 0, 0);
    testGemv(M3, M5, 1, 0);
    testGemv(M3, M5, 0, 1);
    testGemv(M3, M5, 1, 1);

    DoubleMatrix<MatrixStructure::columnSquare> M6(17);
    DoubleMatrix<MatrixStructure::rowSquareUptriangular> M7(17);
    fillMatrix(M6, 123, false, true);
    fillMatrix(M7, 123, false, true);

    testGemv(M6, M7, 0, 0);
    testGemv(M6, M7, 1, 0);
    testGemv(M6, M7, 0, 1);
    testGemv(M6, M7, 1, 1);

    DoubleMatrix<MatrixStructure::columnSquare> M8(17);
    DoubleMatrix<MatrixStructure::diagonal> M9(17);
    fillMatrix(M8, 123, false, false);
    fillMatrix(M9, 123, false, false);

    testGemv(M8, M9, 0, 0);
    testGemv(M8, M9, 1, 0);
    testGemv(M8, M9, 0, 1);
    testGemv(M8, M9, 1, 1);
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




