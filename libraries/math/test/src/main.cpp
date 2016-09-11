////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     main.cpp (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Vector.h"
#include "Matrix.h"
#include "Operations.h"
#include "Print.h"

// testing
#include "testing.h"

// stl
#include <random>
#include <iostream>

template<typename ElementType>
void TestVector()
{
    math::ColumnVector<ElementType> v(10);
    v.Fill(2);
    math::ColumnVector<ElementType> u{ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 };
    testing::ProcessTest("DoubleColumnVector construction and Fill", v == u);

    v.Reset();
    math::ColumnVector<ElementType> z{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    testing::ProcessTest("DoubleColumnVector::Reset", v == z);

    v[3] = 7;
    v[7] = 9;
    math::ColumnVector<ElementType> a{ 0, 0, 0, 7, 0, 0, 0, 9, 0, 0 };
    testing::ProcessTest("DoubleColumnVector::operator[]", v == a);

    testing::ProcessTest("DoubleColumnVector::Norm2", testing::IsEqual(a.Norm2(), static_cast<ElementType>(std::sqrt(7*7+9*9))));
    testing::ProcessTest("DoubleColumnVector::Norm1", a.Norm1() == 7+9);
    testing::ProcessTest("DoubleColumnVector::Norm0", a.Norm0() == 2);
    testing::ProcessTest("DoubleColumnVector::Min", a.Min() == 0);
    testing::ProcessTest("DoubleColumnVector::Max", a.Max() == 9);

    std::default_random_engine rng;
    std::normal_distribution<ElementType> normal(0, 1.0);
    auto generator = [&]() { return normal(rng); };
    v.Generate(generator);

    v *= 0;
    testing::ProcessTest("DoubleColumnVector::operator *=", v == z);
}

template<typename ElementType>
void TestVectorReference()
{
    math::ColumnVector<ElementType> u(10);
    auto v = u.GetReference();
    auto w = v.GetSubVector(1, 3);
    w.Fill(1);
    math::ColumnVector<ElementType> y{ 0, 1, 1, 1, 0, 0, 0, 0, 0, 0 };
    testing::ProcessTest("ColumnVectorReference::Fill", u == y);

    w.Reset();
    math::ColumnVector<ElementType> z{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    testing::ProcessTest("ColumnVectorReference::Reset", u == z);

    w[0] = 7;
    w[2] = 9;
    math::ColumnVector<ElementType> a{ 0, 7, 0, 9, 0, 0, 0, 0, 0, 0 };
    testing::ProcessTest("ColumnVectorReference::operator[]", u == a);

    auto b = w.GetSubVector(0, 2);
    testing::ProcessTest("ColumnVectorReference::Norm2", testing::IsEqual(b.Norm2(), static_cast<ElementType>(std::sqrt(7*7))));
    testing::ProcessTest("ColumnVectorReference::Norm1", b.Norm1() == 7);
    testing::ProcessTest("ColumnVectorReference::Norm0", b.Norm0() == 1);
    testing::ProcessTest("ColumnVectorReference::Min", b.Min() == 0);
    testing::ProcessTest("ColumnVectorReference::Max", b.Max() == 7);

    std::default_random_engine rng;
    std::normal_distribution<ElementType> normal(0, 1.0);
    auto generator = [&]() { return normal(rng); };
    w.Generate(generator);

    v.Fill(3);
    math::RowVector<ElementType> c{ 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 };
    auto t = v.Transpose();
    testing::ProcessTest("ColumnVectorReference::Transpose()", c == t);

    v *= 0;
    testing::ProcessTest("ColumnVectorReference::operator *=", v == z);
}

template<typename ElementType>
void TestVectorProduct()
{
    math::RowVector<ElementType> u{ 0, 1, 0, 1, 0 };
    math::ColumnVector<ElementType> v{ 1, 2, 3, 4, 5 };
    auto dot = math::Operations::Dot(u, v);
    testing::ProcessTest("Operations::Dot(Vector, Vector)", dot == 6);

    ElementType result;
    math::Operations::Product(u, v, result);
    testing::ProcessTest("Operations::Product(Vector, Vector)", result == 6);
}

template<typename ElementType>
void TestAddTo()
{
    math::RowVector<ElementType> u{ 1, 2, 3, 4, 5 };
    math::RowVector<ElementType> v{ 0, 1, 0, 1, 0 };
    math::Operations::AddTo(static_cast<ElementType>(2.0), v, u);

    math::RowVector<ElementType> z{ 1, 4, 3, 6, 5 };
    testing::ProcessTest("Operations::AddTo(scalar, Vector, Vector)", u == z);
}

void TestConstDoubleVector()
{
    const math::RowVector<double> u{ 0, 1, 0, 1, 0 };
    auto v = u.GetReference();
    //v += 3; // should not compile
}

template<typename ElementType, math::MatrixLayout Layout>
void TestMatrix()
{
    math::Matrix<ElementType, Layout> M(3, 4);
    M(0, 0) = 1;
    M(0, 2) = 4;
    M(2, 3) = 7;

    math::ColumnMatrix<ElementType> S1
    { 
        { 1, 0, 4, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 7 } 
    };
    testing::ProcessTest("Matrix::Operator()", M == S1);

    auto N = M.GetBlock(1, 1, 2, 3);
    N.Fill(3);
    N(0, 1) = 4;

    math::ColumnMatrix<ElementType> S2
    {
        { 1, 0, 4, 0 },
        { 0, 3, 4, 3 },
        { 0, 3, 3, 3 }
    };
    testing::ProcessTest("Matrix::GetBlock()", M == S2);

    auto v = M.GetRow(2);
    v[2] = 5;
    v[3] = 6;

    math::ColumnMatrix<ElementType> S3
    {
        { 1, 0, 4, 0 },
        { 0, 3, 4, 3 },
        { 0, 3, 5, 6 }
    };
    testing::ProcessTest("Matrix::GetRow()", M == S3);

    auto u = M.GetColumn(1);
    u[0] = 2;
    u[1] = 2;
    u[2] = 8;

    math::ColumnMatrix<ElementType> S4
    {
        { 1, 2, 4, 0 },
        { 0, 2, 4, 3 },
        { 0, 8, 5, 6 }
    };
    testing::ProcessTest("Matrix::GetColumn()", M == S4);

    auto w = M.GetDiagonal<math::VectorOrientation::column>();
    w.Fill(9);

    math::ColumnMatrix<ElementType> S5
    {
        { 9, 2, 4, 0 },
        { 0, 9, 4, 3 },
        { 0, 8, 9, 6 }
    };
    testing::ProcessTest("Matrix::GetDiagonal()", M == S5);

    auto T = M.Transpose();
    math::ColumnMatrix<ElementType> S6
    {
        { 9, 0, 0 },
        { 2, 9, 8 },
        { 4, 4, 9 },
        { 0, 3, 6 }
    };
    testing::ProcessTest("Matrix::GetDiagonal()", T == S6);
}

template<typename ElementType, math::MatrixLayout Layout>
void TestMatrix2()
{
    math::Matrix<ElementType, Layout> M(7, 7);

    auto N = M.GetBlock(1, 1, 5, 5);
    N.GetRow(0).Fill(1);
    N.GetRow(4).Fill(1);
    N.GetDiagonal<math::VectorOrientation::column>().Fill(1);

    auto T = N.Transpose();
    T.GetRow(0).Fill(1);
    T.GetRow(4).Fill(1);

    math::ColumnMatrix<ElementType> C
    {
        { 0, 0, 0, 0, 0, 0, 0 },
        { 0, 1, 1, 1, 1, 1, 0 },
        { 0, 1, 1, 0, 0, 1, 0 },
        { 0, 1, 0, 1, 0, 1, 0 },
        { 0, 1, 0, 0, 1, 1, 0 },
        { 0, 1, 1, 1, 1, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 0 }
    };
    testing::ProcessTest("Matrix::GetDiagonal()", M == C);
}

/// Runs all tests
///
int main()
{
    TestVector<float>();
    TestVector<double>();
    TestVectorReference<float>();
    TestVectorReference<double>();
    TestVectorProduct<float>();
    TestVectorProduct<double>();
    TestAddTo<float>();
    TestAddTo<double>();
    TestConstDoubleVector();

    TestMatrix<float, math::MatrixLayout::rowMajor>();
    TestMatrix<float, math::MatrixLayout::columnMajor>();
    TestMatrix<double, math::MatrixLayout::rowMajor>();
    TestMatrix<double, math::MatrixLayout::columnMajor>();

    TestMatrix2<double, math::MatrixLayout::rowMajor>();
    TestMatrix2<double, math::MatrixLayout::columnMajor>();

    if(testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}
