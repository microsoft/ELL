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

using namespace emll;

// template structure to make it easier to test both native and blas implementations 
enum class Implementation { native, blas };

template <Implementation Implementation>
struct ImplementedOperations;

template <>
struct ImplementedOperations<Implementation::blas> : public math::BlasOperations {};

template <>
struct ImplementedOperations<Implementation::native> : public math::NativeOperations {};

template<typename ElementType, math::VectorOrientation Orientation>
void TestVector()
{
    math::Vector<ElementType, Orientation> v(10);
    v.Fill(2);
    math::Vector<ElementType, Orientation> r0{ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 };
    testing::ProcessTest("Vector::Fill", v == r0);

    v.Reset();
    math::Vector<ElementType, Orientation> r1{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    testing::ProcessTest("Vector::Reset", v == r1);

    v[3] = 7;
    v[7] = 9;
    math::Vector<ElementType, Orientation> r2{ 0, 0, 0, 7, 0, 0, 0, 9, 0, 0 };
    testing::ProcessTest("Vector::operator[]", v == r2);

    auto w = v.GetSubVector(1, 3);
    w.Fill(3);
    math::Vector<ElementType, Orientation> r3{ 0, 3, 3, 3, 0, 0, 0, 9, 0, 0 };
    testing::ProcessTest("VectorReference::Fill", v == r3);

    auto u = v.GetSubVector(3, 2);
    u.Reset();
    math::Vector<ElementType, Orientation> r4{ 0, 3, 3, 0, 0, 0, 0, 9, 0, 0 };
    testing::ProcessTest("VectorReference::Reset", v == r4);

    // just checking compilation 
    std::default_random_engine rng;
    std::normal_distribution<ElementType> normal(0, 1.0);
    auto generator = [&]() { return normal(rng); };
    v.Generate(generator);
    u.Generate(generator);
}

template<typename ElementType, Implementation Implementation>
void TestVectorOperations()
{
    auto implementationName = ImplementedOperations<Implementation>::GetImplementationName();

    math::RowVector<ElementType> u{ 0, 1, 0, 2, 0 };
    math::ColumnVector<ElementType> v{ 1, 2, 3, 4, 5 };

    testing::ProcessTest(implementationName + "Operations::Norm0(Vector)", math::Operations::Norm0(u) == 2);

    testing::ProcessTest(implementationName + "Operations::Norm1(Vector)", math::Operations::Norm1(u) == 3);

    testing::ProcessTest(implementationName + "Operations::Norm2(Vector)", testing::IsEqual(math::Operations::Norm2(u), static_cast<ElementType>(std::sqrt(5))));

    auto dot = ImplementedOperations<Implementation>::Dot(u, v);
    testing::ProcessTest(implementationName + "Operations::Dot(Vector, Vector)", dot == 10);

    dot = ImplementedOperations<Implementation>::Dot(v.Transpose(), u);
    testing::ProcessTest(implementationName + "Operations::Dot(VectorReference, Vector)", dot == 10);

    ElementType r;
    ImplementedOperations<Implementation>::Multiply(u, v, r);
    testing::ProcessTest(implementationName + "Operations::Multiply(Vector, Vector, scalar)", r == 10);

    ImplementedOperations<Implementation>::Multiply(v.Transpose(), u.Transpose(), r);
    testing::ProcessTest(implementationName + "Operations::Multiply(Vector, Vector, scalar)", r == 10);

    ImplementedOperations<Implementation>::Add(static_cast<ElementType>(1), v);
    math::ColumnVector<ElementType> r0{ 2, 3, 4, 5, 6 };
    testing::ProcessTest(implementationName + "Operations::Add(scalar, Vector)", v == r0);

    ImplementedOperations<Implementation>::Multiply(static_cast<ElementType>(0), v);
    math::ColumnVector<ElementType> r1{ 0, 0, 0, 0, 0 };
    testing::ProcessTest(implementationName + "Operations::Multiply(scalar, Vector)", v == r1);

    math::ColumnMatrix<ElementType> M
    {
        { 1, 2, 4, 0 },
        { 0, 2, 4, 3 },
        { 0, 8, 5, 6 }
    };
    auto N = M.GetBlock(1, 0, 2, 3);
    auto w = N.GetRow(0);
    auto z = N.GetRow(1);

    dot = ImplementedOperations<Implementation>::Dot(w, z);
    testing::ProcessTest(implementationName + "Operations::Dot(VectorReference, VectorReference)", dot == 36);

    ImplementedOperations<Implementation>::Add(static_cast<ElementType>(1), w);
    math::ColumnMatrix<ElementType> R0
    {
        { 1, 2, 4, 0 },
        { 1, 3, 5, 3 },
        { 0, 8, 5, 6 }
    };
    testing::ProcessTest(implementationName + "Operations::Add(scalar, VectorReference)", M == R0);

    ImplementedOperations<Implementation>::Add(static_cast<ElementType>(2), r0.GetSubVector(0,3), w.Transpose());
    math::ColumnMatrix<ElementType> R1
    { 
        { 1, 2, 4, 0 },
        { 5, 9, 13, 3 },
        { 0, 8, 5, 6 }
    };
    testing::ProcessTest(implementationName + "Operations::Add(scalar, VectorReference, VectorReference)", M == R1);

    math::Operations::Multiply(static_cast<ElementType>(2), z);
    math::ColumnMatrix<ElementType> R2
    {
        { 1, 2, 4, 0 },
        { 5, 9, 13, 3 },
        { 0, 16, 10, 6 }
    };
    testing::ProcessTest(implementationName + "Operations::Multiply(VectorReference, scalar)", M == R2);

    testing::ProcessTest(implementationName + "Operations::Norm0(VectorReference)", math::Operations::Norm0(M.GetColumn(1)) == 3);

    testing::ProcessTest(implementationName + "Operations::Norm1(VectorReference)", math::Operations::Norm1(M.GetColumn(1)) == 2+9+16);

    testing::ProcessTest(implementationName + "Operations::Norm2(VectorReference)", testing::IsEqual(math::Operations::Norm2(M.GetColumn(1)), static_cast<ElementType>(std::sqrt(2*2+9*9+16*16))));
}

template<typename ElementType, math::MatrixLayout Layout>
void TestMatrix1()
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

    math::ColumnMatrix<ElementType> R1
    {
        { 0, 0, 0, 0, 0, 0, 0 },
        { 0, 1, 1, 1, 1, 1, 0 },
        { 0, 1, 1, 0, 0, 1, 0 },
        { 0, 1, 0, 1, 0, 1, 0 },
        { 0, 1, 0, 0, 1, 1, 0 },
        { 0, 1, 1, 1, 1, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 0 }
    };
    testing::ProcessTest("Matrix::GetDiagonal()", M == R1);

    auto S = N.GetBlock(1, 1, 3, 3);
    S.Fill(8);

    math::ColumnMatrix<ElementType> R2
    {
        { 0, 0, 0, 0, 0, 0, 0 },
        { 0, 1, 1, 1, 1, 1, 0 },
        { 0, 1, 8, 8, 8, 1, 0 },
        { 0, 1, 8, 8, 8, 1, 0 },
        { 0, 1, 8, 8, 8, 1, 0 },
        { 0, 1, 1, 1, 1, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 0 }
    };
    testing::ProcessTest("Matrix::Fill()", M == R2);
}

template<typename ElementType, math::MatrixLayout Layout, Implementation Implementation>
void TestMatrixOperations()
{
    auto implementationName = ImplementedOperations<Implementation>::GetImplementationName();

    math::Matrix<ElementType, Layout> M
    {
        {1, 0},
        {0, 1},
        {2, 2}
    };

    math::ColumnVector<ElementType> u { 1, 1, 0 };
    math::ColumnVector<ElementType> v { 3, 4 };
    
    ElementType s = 2;
    ElementType t = 3;

    // u = s * M * v + t * u
    ImplementedOperations<Implementation>::Multiply(s, M, v, t, u);

    math::ColumnVector<ElementType> uu{9, 11, 28};
    testing::ProcessTest(implementationName + "Operations::Multiply(Matrix, Vector)", u == uu);

    auto A = M.GetBlock(1, 0, 2, 2);
    auto w = M.GetRow(0).Transpose();
    
    // v = s * a * w + t * v;
    ImplementedOperations<Implementation>::Multiply(s, A, w, t, v);

    math::ColumnVector<ElementType> vv{ 9, 16 };
    testing::ProcessTest("implementationName + Operations::Multiply(MatrixReference, VectorReference)", v == vv);
}

/// Runs all tests
///
int main()
{
    // vector
    TestVector<float, math::VectorOrientation::row>();
    TestVector<double, math::VectorOrientation::row>();
    TestVector<float, math::VectorOrientation::column>();
    TestVector<double, math::VectorOrientation::column>();

    TestVectorOperations<float, Implementation::native>();
    TestVectorOperations<double, Implementation::native>();
    TestVectorOperations<float, Implementation::blas>();
    TestVectorOperations<double, Implementation::blas>();

    TestMatrix1<float, math::MatrixLayout::rowMajor>();
    TestMatrix1<float, math::MatrixLayout::columnMajor>();
    TestMatrix1<double, math::MatrixLayout::rowMajor>();
    TestMatrix1<double, math::MatrixLayout::columnMajor>();

    TestMatrix2<float, math::MatrixLayout::rowMajor>();
    TestMatrix2<float, math::MatrixLayout::columnMajor>();
    TestMatrix2<double, math::MatrixLayout::rowMajor>();
    TestMatrix2<double, math::MatrixLayout::columnMajor>();

    TestMatrixOperations<float, math::MatrixLayout::rowMajor, Implementation::native>();
    TestMatrixOperations<float, math::MatrixLayout::columnMajor, Implementation::native>();
    TestMatrixOperations<double, math::MatrixLayout::rowMajor, Implementation::native>();
    TestMatrixOperations<double, math::MatrixLayout::columnMajor, Implementation::native>();
    TestMatrixOperations<float, math::MatrixLayout::rowMajor, Implementation::blas>();
    TestMatrixOperations<float, math::MatrixLayout::columnMajor, Implementation::blas>();
    TestMatrixOperations<double, math::MatrixLayout::rowMajor, Implementation::blas>();
    TestMatrixOperations<double, math::MatrixLayout::columnMajor, Implementation::blas>();

    if(testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}