////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Matrix.h"
#include "Operations.h"
#include "Print.h"
#include "Vector.h"
#include "Tensor.h"

// testing
#include "testing.h"

// stl
#include <iostream>
#include <random>

using namespace ell;

template <typename ElementType, math::VectorOrientation Orientation>
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

template <typename ElementType, math::ImplementationType Implementation>
void TestVectorOperations()
{
    auto implementationName = math::OperationsImplementation<Implementation>::GetImplementationName();
    using Ops = math::OperationsImplementation<Implementation>;

    math::RowVector<ElementType> u{ 0, 1, 0, 2, 0 };
    math::ColumnVector<ElementType> v{ 1, 2, 3, 4, 5 };

    testing::ProcessTest(implementationName + "Operations::Norm0(Vector)", math::Operations::Norm0(u) == 2);

    testing::ProcessTest(implementationName + "Operations::Norm1(Vector)", math::Operations::Norm1(u) == 3);

    testing::ProcessTest(implementationName + "Operations::Norm2(Vector)", testing::IsEqual(math::Operations::Norm2(u), static_cast<ElementType>(std::sqrt(5))));

    auto dot = Ops::Dot(u, v);
    testing::ProcessTest(implementationName + "Operations::Dot(Vector, Vector)", dot == 10);

    dot = Ops::Dot(v.Transpose(), u);
    testing::ProcessTest(implementationName + "Operations::Dot(VectorReference, Vector)", dot == 10);

    ElementType r;
    Ops::Multiply(u, v, r);
    testing::ProcessTest(implementationName + "Operations::Multiply(Vector, Vector, scalar)", r == 10);

    Ops::Multiply(v.Transpose(), u.Transpose(), r);
    testing::ProcessTest(implementationName + "Operations::Multiply(Vector, Vector, scalar)", r == 10);

    Ops::Add(static_cast<ElementType>(1), v);
    math::ColumnVector<ElementType> r0{ 2, 3, 4, 5, 6 };
    testing::ProcessTest(implementationName + "Operations::Add(scalar, Vector)", v == r0);

    Ops::Multiply(static_cast<ElementType>(0), v);
    math::ColumnVector<ElementType> r1{ 0, 0, 0, 0, 0 };
    testing::ProcessTest(implementationName + "Operations::Multiply(scalar, Vector)", v == r1);

    math::ColumnMatrix<ElementType> M{
        { 1, 2, 4, 0 },
        { 0, 2, 4, 3 },
        { 0, 8, 5, 6 }
    };
    auto N = M.GetSubMatrix(1, 0, 2, 3);
    auto w = N.GetRow(0);
    auto z = N.GetRow(1);

    dot = Ops::Dot(w, z);
    testing::ProcessTest(implementationName + "Operations::Dot(VectorReference, VectorReference)", dot == 36);

    Ops::Add(static_cast<ElementType>(1), w);
    math::ColumnMatrix<ElementType> R0{
        { 1, 2, 4, 0 },
        { 1, 3, 5, 3 },
        { 0, 8, 5, 6 }
    };
    testing::ProcessTest(implementationName + "Operations::Add(scalar, VectorReference)", M == R0);

    Ops::Add(static_cast<ElementType>(2), r0.GetSubVector(0, 3), w.Transpose());
    math::ColumnMatrix<ElementType> R1{
        { 1, 2, 4, 0 },
        { 5, 9, 13, 3 },
        { 0, 8, 5, 6 }
    };
    testing::ProcessTest(implementationName + "Operations::Add(scalar, VectorReference, VectorReference)", M == R1);

    math::Operations::Multiply(static_cast<ElementType>(2), z);
    math::ColumnMatrix<ElementType> R2{
        { 1, 2, 4, 0 },
        { 5, 9, 13, 3 },
        { 0, 16, 10, 6 }
    };
    testing::ProcessTest(implementationName + "Operations::Multiply(VectorReference, scalar)", M == R2);

    testing::ProcessTest(implementationName + "Operations::Norm0(VectorReference)", math::Operations::Norm0(M.GetColumn(1)) == 3);

    testing::ProcessTest(implementationName + "Operations::Norm1(VectorReference)", math::Operations::Norm1(M.GetColumn(1)) == 2 + 9 + 16);

    testing::ProcessTest(implementationName + "Operations::Norm2(VectorReference)", testing::IsEqual(math::Operations::Norm2(M.GetColumn(1)), static_cast<ElementType>(std::sqrt(2 * 2 + 9 * 9 + 16 * 16))));

    Ops::Copy(math::RowVector<ElementType>{ 1, 1, 1, 1 }, M.GetRow(1));
    Ops::Copy(math::ColumnVector<ElementType>{ 1, 1, 1 }, M.GetColumn(2));
    math::ColumnMatrix<ElementType> R3{
        { 1, 2, 1, 0 },
        { 1, 1, 1, 1 },
        { 0, 16, 1, 6 }
    };
    testing::ProcessTest(implementationName + "Operations::Copy(VectorReference, VectorReference)", M == R3);
}

template <typename ElementType>
void TestElementWiseOperations()
{
    using Ops = math::DerivedOperations<ElementType>;

    math::RowVector<ElementType> u{ 0, 1, 2, 2, 10 };
    math::ColumnVector<ElementType> v{ 1, 2, 3, 4, 5 };
    math::ColumnVector<ElementType> r{ 0, 2, 6, 8, 50 };

    math::ColumnVector<ElementType> t(u.Size());
    Ops::ElementWiseMultiply(u, v, t);
    testing::ProcessTest("Operations::VectorElementwise(VectorReference, VectorReference)", t == r);

    math::ColumnMatrix<ElementType> A{
        { 1, 2, 4 },
        { 3, 1, 5 },
        { 8, 2, 3 }
    };

    math::RowMatrix<ElementType> B{
        { 2, 7, 4 },
        { 1, 9, 3 },
        { 3, 10, 2 }
    };

    math::ColumnMatrix<ElementType> R{
        { 2, 14, 16 },
        { 3, 9, 15 },
        { 24, 20, 6 }
    };

    math::ColumnMatrix<ElementType> C(A.NumRows(), B.NumColumns());
    Ops::ElementWiseMultiply(A, B, C);

    testing::ProcessTest("Operations::ElementWiseMultiply(MatrixReference, MatrixReference)", C == R);
}

template <typename ElementType, math::MatrixLayout Layout>
void TestMatrix1()
{
    math::Matrix<ElementType, Layout> M(3, 4);
    M(0, 0) = 1;
    M(0, 2) = 4;
    M(2, 3) = 7;

    math::ColumnMatrix<ElementType> S1{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 7 }
    };
    testing::ProcessTest("Matrix::Operator()", M == S1);

    auto N = M.GetSubMatrix(1, 1, 2, 3);
    N.Fill(3);
    N(0, 1) = 4;

    math::ColumnMatrix<ElementType> S2{
        { 1, 0, 4, 0 },
        { 0, 3, 4, 3 },
        { 0, 3, 3, 3 }
    };
    testing::ProcessTest("Matrix::GetSubMatrix()", M == S2);

    auto v = M.GetRow(2);
    v[2] = 5;
    v[3] = 6;

    math::ColumnMatrix<ElementType> S3{
        { 1, 0, 4, 0 },
        { 0, 3, 4, 3 },
        { 0, 3, 5, 6 }
    };
    testing::ProcessTest("Matrix::GetRow()", M == S3);

    auto u = M.GetColumn(1);
    u[0] = 2;
    u[1] = 2;
    u[2] = 8;

    math::ColumnMatrix<ElementType> S4{
        { 1, 2, 4, 0 },
        { 0, 2, 4, 3 },
        { 0, 8, 5, 6 }
    };
    testing::ProcessTest("Matrix::GetColumn()", M == S4);

    auto w = M.GetDiagonal();
    w.Fill(9);

    math::ColumnMatrix<ElementType> S5{
        { 9, 2, 4, 0 },
        { 0, 9, 4, 3 },
        { 0, 8, 9, 6 }
    };
    testing::ProcessTest("Matrix::GetDiagonal()", M == S5);

    auto T = M.Transpose();
    math::ColumnMatrix<ElementType> S6{
        { 9, 0, 0 },
        { 2, 9, 8 },
        { 4, 4, 9 },
        { 0, 3, 6 }
    };
    testing::ProcessTest("Matrix::GetDiagonal()", T == S6);

    std::default_random_engine rng;
    std::normal_distribution<ElementType> normal(0, 1.0);
    auto generator = [&]() { return normal(rng); };
    M.Generate(generator);
}

template <typename ElementType, math::MatrixLayout Layout>
void TestMatrix2()
{
    math::Matrix<ElementType, Layout> M(7, 7);

    auto N = M.GetSubMatrix(1, 1, 5, 5);
    N.GetRow(0).Fill(1);
    N.GetRow(4).Fill(1);
    N.GetDiagonal().Fill(1);

    auto T = N.Transpose();
    T.GetRow(0).Fill(1);
    T.GetRow(4).Fill(1);

    math::ColumnMatrix<ElementType> R1{
        { 0, 0, 0, 0, 0, 0, 0 },
        { 0, 1, 1, 1, 1, 1, 0 },
        { 0, 1, 1, 0, 0, 1, 0 },
        { 0, 1, 0, 1, 0, 1, 0 },
        { 0, 1, 0, 0, 1, 1, 0 },
        { 0, 1, 1, 1, 1, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 0 }
    };
    testing::ProcessTest("Matrix::GetDiagonal()", M == R1);

    auto S = N.GetSubMatrix(1, 1, 3, 3);
    S.Fill(8);

    math::ColumnMatrix<ElementType> R2{
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

template <typename ElementType, math::MatrixLayout Layout1, math::MatrixLayout Layout2>
void TestMatrixCopy()
{
    math::Matrix<ElementType, Layout1> M1{
        { 1, 2, 3, 4, 5, 6, 7, 8, 9 },
        { 1, 2, 3, 4, 5, 6, 7, 8, 9 },
        { 1, 2, 3, 4, 5, 6, 7, 8, 9 },
        { 1, 2, 3, 4, 5, 6, 7, 8, 9 }
    };

    math::Matrix<ElementType, Layout2> M2(M1);

    testing::ProcessTest("Matrix copy ctor", M1 == M2);
}

template <typename ElementType>
void TestReferenceMatrix()
{
    std::vector<ElementType> elements = { 1, 2, 3, 4, 5, 6 };
    math::MatrixReference<ElementType, math::MatrixLayout::rowMajor> Mref1(2, 3, elements.data());

    math::Matrix<ElementType, math::MatrixLayout::rowMajor> M1{
        { 1, 2, 3 },
        { 4, 5, 6 }
    };
    testing::ProcessTest("ReferenceMatrix testing initial values", Mref1 == M1);

    elements[2] = 11;
    elements[4] = 10;
    math::Matrix<ElementType, math::MatrixLayout::rowMajor> M2{
        { 1, 2, 11 },
        { 4, 10, 6 }
    };
    testing::ProcessTest("ReferenceMatrix testing modification of values", Mref1 == M2);

    math::MatrixReference<ElementType, math::MatrixLayout::columnMajor> Mref2(2, 3, elements.data());
    math::Matrix<ElementType, math::MatrixLayout::rowMajor> M3{
        { 1, 11, 10 },
        { 2,  4,  6 }
    };
    testing::ProcessTest("ReferenceMatrix testing second round of intial values", Mref2 == M3);

    elements[1] = 12;
    elements[2] = 13;
    math::Matrix<ElementType, math::MatrixLayout::rowMajor> M4{
        { 1, 13, 10 },
        { 12, 4, 6 }
    };
    testing::ProcessTest("ReferenceMatrix testing second round of modified values", Mref2 == M4);
}

template <typename ElementType, math::MatrixLayout Layout, math::ImplementationType Implementation>
void TestMatrixOperations()
{
    auto implementationName = math::OperationsImplementation<Implementation>::GetImplementationName();
    using Ops = math::OperationsImplementation<Implementation>;

    math::Matrix<ElementType, Layout> M{
        { 1, 0 },
        { 0, 1 },
        { 2, 2 }
    };

    math::ColumnVector<ElementType> u{ 1, 1, 0 };
    math::ColumnVector<ElementType> v{ 3, 4 };

    ElementType s = 2;
    ElementType t = 3;

    // u = s * M * v + t * u
    Ops::Multiply(s, M, v, t, u);
    math::ColumnVector<ElementType> r0{ 9, 11, 28 };
    testing::ProcessTest(implementationName + "Operations::Multiply(Matrix, Vector)", u == r0);

    math::Matrix<ElementType, Layout> A1{
        { 1, 2 },
        { 3, 1 },
        { 2, 0 }
    };
    math::Matrix<ElementType, Layout> B1{
        { 3, 4,  5,  6 },
        { 8, 9, 10, 11 }
    };
    math::Matrix<ElementType, Layout> C1{
        { 1, 1, 1, 1 },
        { 2, 1, 2, 1 },
        { 1, 3, 1, 3 }
    };

    auto A = M.GetSubMatrix(1, 0, 2, 2);
    auto w = M.GetRow(0).Transpose();

    // v = s * A * w + t * v;
    Ops::Multiply(s, A, w, t, v);
    math::ColumnVector<ElementType> r1{ 9, 16 };
    testing::ProcessTest(implementationName + "Operations::Multiply(MatrixReference, VectorReference)", v == r1);

    // v = s * M.Transpose * u + t * v;
    Ops::Multiply(s, M.Transpose(), u, t, v);
    math::ColumnVector<ElementType> r2{ 157, 182 };
    testing::ProcessTest(implementationName + "Operations::Multiply(Matrix.Transpose, Vector)", v == r2);

    Ops::Add(static_cast<ElementType>(1), M);
    math::Matrix<ElementType, math::MatrixLayout::columnMajor> R0{
        { 2, 1 },
        { 1, 2 },
        { 3, 3 }
    };
    testing::ProcessTest(implementationName + "Operations::Add(MatrixReference, scalar)", M == R0);

    Ops::Multiply(static_cast<ElementType>(2), M);
    math::Matrix<ElementType, math::MatrixLayout::columnMajor> R1{
        { 4, 2 },
        { 2, 4 },
        { 6, 6 }
    };
    testing::ProcessTest(implementationName + "Operations::Add(MatrixReference, scalar)", M == R1);

    math::Matrix<ElementType, Layout> R2{
        { 0, 2 },
        { 4, 8 },
        { 1, 3 }
    };
    Ops::Copy(R2, M);
    testing::ProcessTest(implementationName + "Operations::Copy(MatrixReference, MatrixReference)", M == R2);
}

template <typename ElementType, math::MatrixLayout Layout>
void TestConstMatrixReference()
{
    math::Matrix<ElementType, Layout> M{
        { 1, 2, 4, 0 },
        { 1, 3, 5, 3 },
        { 0, 8, 1, 6 },
        { 1, 2, 4, 3 }
    };

    math::ConstMatrixReference<ElementType, Layout> N(M);
    auto P = M.GetConstReference();
    testing::ProcessTest("ConstMatrixReference testing operator ==", M == N);
    testing::ProcessTest("ConstMatrixReference testing GetConstReference", N == P);

    std::vector<ElementType> v;
    v.assign(P.GetDataPointer(), P.GetDataPointer() + (size_t)(P.NumRows() * P.NumColumns()));
    ElementType sum = 0;
    std::for_each(v.begin(), v.end(), [&sum](int val) { sum += val; });
    testing::ProcessTest("ConstMatrixReference testing GetDataPointer", sum == 44);

    math::ColumnVector<ElementType> r{ 1, 3, 1, 3 };
    auto u = N.GetDiagonal();
    testing::ProcessTest("ConstMatrixReference testing GetDiagonal", u == r);

    auto R = N.GetSubMatrix(1, 1, 3, 2);
    auto S = R.Transpose();
    testing::ProcessTest("ConstMatrixReference testing GetRow", math::Operations::Norm1(S.GetRow(0)) == 3 + 8 + 2);
    testing::ProcessTest("ConstMatrixReference testing GetRow", math::Operations::Norm1(S.GetRow(1)) == 5 + 1 + 4);
}

template <typename ElementType, math::ImplementationType Implementation>
void TestMatrixMatrixAdd()
{
    auto implementationName = math::OperationsImplementation<Implementation>::GetImplementationName();
    using Ops = math::OperationsImplementation<Implementation>;

    math::Matrix<ElementType, math::MatrixLayout::rowMajor> A{
        { 41, 47, 53, 59 },
        { 40, 45, 56, 61 },
        { 15, 25, 23, 33 },
    };

    math::Matrix<ElementType, math::MatrixLayout::rowMajor> B{
        { 41, 47, 53, 59 },
        { 40, 45, 56, 61 },
        { 15, 25, 23, 33 },
    };

    math::Matrix<ElementType, math::MatrixLayout::columnMajor> C{
        { 41, 47, 53, 59 },
        { 40, 45, 56, 61 },
        { 15, 25, 23, 33 },
    };

    math::Matrix<ElementType, math::MatrixLayout::rowMajor> R{
        { 123, 141, 159, 177 },
        { 120, 135, 168, 183 },
        { 45, 75, 69, 99 },
    };

    math::Matrix<ElementType, math::MatrixLayout::rowMajor> D(A.NumRows(), A.NumColumns());
    Ops::Add(static_cast<ElementType>(1.0), A, static_cast<ElementType>(2.0), B, D);
    testing::ProcessTest(implementationName + "Operations::Add(MatrixReference RowMajor, MatrixReference RowMajor)", D == R);

    math::Matrix<ElementType, math::MatrixLayout::columnMajor> R1(R);
    math::Matrix<ElementType, math::MatrixLayout::columnMajor> E(A.NumRows(), A.NumColumns());
    Ops::Add(static_cast<ElementType>(2.0), C, static_cast<ElementType>(1.0), A, E);
    testing::ProcessTest(implementationName + "Operations::Add(MatrixReference ColumnMajor, MatrixReference RowMajor)", E == R1);
}

template <typename ElementType, math::MatrixLayout LayoutA, math::MatrixLayout LayoutB, math::ImplementationType Implementation>
void TestMatrixMatrixMultiply()
{
    auto implementationName = math::OperationsImplementation<Implementation>::GetImplementationName();
    using Ops = math::OperationsImplementation<Implementation>;

    math::Matrix<ElementType, LayoutA> A{
        { 1, 2 },
        { 3, 1 },
        { 2, 0 }
    };

    math::Matrix<ElementType, LayoutB> B{
        { 3, 4,  5,  6 },
        { 8, 9, 10, 11 }
    };

    math::Matrix<ElementType, LayoutA> R{
        { 19, 22, 25, 28 },
        { 17, 21, 25, 29 },
        { 6, 8, 10, 12 }
    };

    math::Matrix<ElementType, LayoutA> C(A.NumRows(), B.NumColumns());
    Ops::Multiply(1.0, A, B, 0.0, C);

    testing::ProcessTest(implementationName + "Operations::Multiply(Matrix, Matrix)", C == R);
}

template <typename ElementType>
void TestVectorToArray()
{
    std::vector<ElementType> r0{ 41, 47, 53, 59 };
    std::vector<ElementType> r1{ 15, 25, 23, 33 };

    math::RowVector<ElementType> p(r0);
    testing::ProcessTest("Testing vector reference to array for a row vector", p.ToArray() == r0);

    math::ColumnVector<ElementType> q(r1);
    testing::ProcessTest("Testing vector reference to array for a column vector", q.ToArray() == r1);

    math::Matrix<ElementType, math::MatrixLayout::rowMajor> A{
        { 41, 47, 53, 59 },
        { 40, 45, 56, 61 },
        { 15, 25, 23, 33 },
    };

    std::vector<ElementType> r(A.GetRow(0).ToArray());
    testing::ProcessTest("Testing vector reference to array for a row matrix", r == r0);

    std::vector<ElementType> s(A.GetRow(2).ToArray());
    testing::ProcessTest("Testing vector reference to array for a row matrix", s == r1);

    math::Matrix<ElementType, math::MatrixLayout::columnMajor> B(A);

    std::vector<ElementType> t(B.GetRow(0).ToArray());
    testing::ProcessTest("Testing vector reference to array for a column matrix", t == r0);

    std::vector<ElementType> u(B.GetRow(2).ToArray());
    testing::ProcessTest("Testing vector reference to array for a column matrix", u == r1);
}

template<typename ElementType, math::Dimension dimension0, math::Dimension dimension1, math::Dimension dimension2>
void TestTensor()
{
    math::Tensor<ElementType, dimension0, dimension1, dimension2> T(10, 20, 30);
    testing::ProcessTest("Tensor::NumRows()", T.NumRows() == 10);
    testing::ProcessTest("Tensor::NumColumns()", T.NumColumns() == 20);
    testing::ProcessTest("Tensor::NumChannels()", T.NumChannels() == 30);

    T(3, 2, 1) = 2.0;
    T(4, 3, 2) = 3.0;
    T(3, 3, 3) = 4.0;

    auto S = T.GetSubTensor(3, 2, 1, 5, 5, 5);
    testing::ProcessTest("TensorReference::GetSubTensor", S(0, 0, 0) == 2.0 && S(1, 1, 1) == 3.0 && S(0, 1, 2) == 4.0);

    auto Tc = T.GetConstTensorReference();
    auto Sc = Tc.GetSubTensor(3, 2, 1, 5, 5, 5);
    testing::ProcessTest("ConstTensorReference::GetSubTensor", Sc(0, 0, 0) == 2.0 && Sc(1, 1, 1) == 3.0 && Sc(0, 1, 2) == 4.0);

    math::Tensor<ElementType, dimension0, dimension1, dimension2> T1(T);
    math::ChannelColumnRowTensor<ElementType> T2(Tc);
    math::ColumnRowChannelTensor<ElementType> T3(Tc);
}

void TestTensorGetSlice()
{
    math::ColumnRowChannelTensor<double> T1(3, 4, 5);
    T1(0, 0, 0) = 1;
    T1(1, 2, 3) = 2;
    T1(0, 3, 3) = 3;
    T1(2, 2, 4) = 3;

    auto M11 = T1.GetSlice<math::Dimension::column, math::Dimension::row>(3);
    testing::ProcessTest("TensorReference::GetSlice()", M11(2, 1) == 2 && M11(3, 0) == 3);

    auto M12 = T1.GetSlice<math::Dimension::row, math::Dimension::column>(3);
    testing::ProcessTest("TensorReference::GetSlice()", M12(1, 2) == 2 && M12(0, 3) == 3);

    auto M13 = T1.GetSlice<math::Dimension::column, math::Dimension::channel>(0);
    testing::ProcessTest("TensorReference::GetSlice()", M13(0, 0) == 1 && M13(3, 3) == 3);

    auto M14 = T1.GetSlice<math::Dimension::channel, math::Dimension::column>(0);
    testing::ProcessTest("TensorReference::GetSlice()", M14(0, 0) == 1 && M14(3, 3) == 3);

    auto T1c = T1.GetConstTensorReference();

    auto M11c = T1c.GetSlice<math::Dimension::column, math::Dimension::row>(3);
    testing::ProcessTest("ConstTensorReference::GetSlice()", M11c(2, 1) == 2 && M11c(3, 0) == 3);

    auto M12c = T1c.GetSlice<math::Dimension::row, math::Dimension::column>(3);
    testing::ProcessTest("ConstTensorReference::GetSlice()", M12c(1, 2) == 2 && M12c(0, 3) == 3);

    auto M13c = T1c.GetSlice<math::Dimension::column, math::Dimension::channel>(0);
    testing::ProcessTest("ConstTensorReference::GetSlice()", M13c(0, 0) == 1 && M13c(3, 3) == 3);

    auto M14c = T1c.GetSlice<math::Dimension::channel, math::Dimension::column>(0);
    testing::ProcessTest("ConstTensorReference::GetSlice()", M14c(0, 0) == 1 && M14c(3, 3) == 3);

    math::ChannelColumnRowTensor<double> T2(3, 4, 5);
    T2(0, 0, 0) = 1;
    T2(1, 2, 3) = 2;
    T2(0, 3, 3) = 3;
    T2(2, 2, 4) = 4;

    auto M23 = T2.GetSlice<math::Dimension::column, math::Dimension::channel>(0);
    testing::ProcessTest("TensorReference::GetSlice()", M23(0, 0) == 1 && M23(3, 3) == 3);

    auto M24 = T2.GetSlice<math::Dimension::channel, math::Dimension::column>(0);
    testing::ProcessTest("TensorReference::GetSlice()", M24(0, 0) == 1 && M24(3, 3) == 3);

    auto M25 = T2.GetSlice<math::Dimension::row, math::Dimension::channel>(2);
    testing::ProcessTest("TensorReference::GetSlice()", M25(1, 3) == 2 && M25(2, 4) == 4);

    auto M26 = T2.GetSlice<math::Dimension::channel, math::Dimension::row>(2);
    testing::ProcessTest("TensorReference::GetSlice()", M26(3, 1) == 2 && M26(4, 2) == 4);

    auto T2c = T2.GetConstTensorReference();

    auto M23c = T2c.GetSlice<math::Dimension::column, math::Dimension::channel>(0);
    testing::ProcessTest("ConstTensorReference::GetSlice()", M23c(0, 0) == 1 && M23c(3, 3) == 3);

    auto M24c = T2c.GetSlice<math::Dimension::channel, math::Dimension::column>(0);
    testing::ProcessTest("ConstTensorReference::GetSlice()", M24c(0, 0) == 1 && M24c(3, 3) == 3);

    auto M25c = T2c.GetSlice<math::Dimension::row, math::Dimension::channel>(2);
    testing::ProcessTest("ConstTensorReference::GetSlice()", M25c(1, 3) == 2 && M25c(2, 4) == 4);

    auto M26c = T2c.GetSlice<math::Dimension::channel, math::Dimension::row>(2);
    testing::ProcessTest("ConstTensorReference::GetSlice()", M26c(3, 1) == 2 && M26c(4, 2) == 4);
}

void TestTensorFlatten()
{
    math::ChannelColumnRowTensor<double> T(3, 4, 2);
    T(0, 0, 0) = 1;
    T(0, 0, 1) = 2;
    T(0, 1, 0) = 3;
    T(0, 1, 1) = 4;

    auto M = T.FlattenFirstDimension();
    testing::ProcessTest("TensorReference::FlattenFirstDimension()", M(0, 0) == 1 && M(0, 1) == 2 && M(0, 2) == 3 && M(0, 3) == 4);

    auto Tc = T.GetConstTensorReference();
    auto Mc = Tc.FlattenFirstDimension();
    testing::ProcessTest("ConstTensorReference::FlattenFirstDimension()", Mc(0, 0) == 1 && Mc(0, 1) == 2 && Mc(0, 2) == 3 && Mc(0, 3) == 4);
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

    TestVectorOperations<float, math::ImplementationType::native>();
    TestVectorOperations<double, math::ImplementationType::native>();
    TestVectorOperations<float, math::ImplementationType::openBlas>();
    TestVectorOperations<double, math::ImplementationType::openBlas>();

    TestElementWiseOperations<double>();

    TestMatrix1<float, math::MatrixLayout::rowMajor>();
    TestMatrix1<float, math::MatrixLayout::columnMajor>();
    TestMatrix1<double, math::MatrixLayout::rowMajor>();
    TestMatrix1<double, math::MatrixLayout::columnMajor>();

    TestMatrix2<float, math::MatrixLayout::rowMajor>();
    TestMatrix2<float, math::MatrixLayout::columnMajor>();
    TestMatrix2<double, math::MatrixLayout::rowMajor>();
    TestMatrix2<double, math::MatrixLayout::columnMajor>();

    TestReferenceMatrix<int>();

    TestMatrixCopy<float, math::MatrixLayout::rowMajor, math::MatrixLayout::rowMajor>();
    TestMatrixCopy<float, math::MatrixLayout::rowMajor, math::MatrixLayout::columnMajor>();
    TestMatrixCopy<float, math::MatrixLayout::columnMajor, math::MatrixLayout::rowMajor>();
    TestMatrixCopy<float, math::MatrixLayout::columnMajor, math::MatrixLayout::columnMajor>();
    TestMatrixCopy<double, math::MatrixLayout::rowMajor, math::MatrixLayout::rowMajor>();
    TestMatrixCopy<double, math::MatrixLayout::rowMajor, math::MatrixLayout::columnMajor>();
    TestMatrixCopy<double, math::MatrixLayout::columnMajor, math::MatrixLayout::rowMajor>();
    TestMatrixCopy<double, math::MatrixLayout::columnMajor, math::MatrixLayout::columnMajor>();

    TestMatrixOperations<float, math::MatrixLayout::rowMajor, math::ImplementationType::native>();
    TestMatrixOperations<float, math::MatrixLayout::columnMajor, math::ImplementationType::native>();
    TestMatrixOperations<double, math::MatrixLayout::rowMajor, math::ImplementationType::native>();
    TestMatrixOperations<double, math::MatrixLayout::columnMajor, math::ImplementationType::native>();
    TestMatrixOperations<float, math::MatrixLayout::rowMajor, math::ImplementationType::openBlas>();
    TestMatrixOperations<float, math::MatrixLayout::columnMajor, math::ImplementationType::openBlas>();
    TestMatrixOperations<double, math::MatrixLayout::rowMajor, math::ImplementationType::openBlas>();
    TestMatrixOperations<double, math::MatrixLayout::columnMajor, math::ImplementationType::openBlas>();

    TestConstMatrixReference<float, math::MatrixLayout::rowMajor>();
    TestConstMatrixReference<float, math::MatrixLayout::rowMajor>();
    TestConstMatrixReference<double, math::MatrixLayout::columnMajor>();
    TestConstMatrixReference<double, math::MatrixLayout::columnMajor>();

    TestMatrixMatrixAdd<float, math::ImplementationType::native>();
    TestMatrixMatrixAdd<float, math::ImplementationType::openBlas>();
    TestMatrixMatrixAdd<double, math::ImplementationType::native>();
    TestMatrixMatrixAdd<double, math::ImplementationType::openBlas>();

    TestMatrixMatrixMultiply<double, math::MatrixLayout::rowMajor, math::MatrixLayout::rowMajor, math::ImplementationType::native>();
    TestMatrixMatrixMultiply<double, math::MatrixLayout::rowMajor, math::MatrixLayout::columnMajor, math::ImplementationType::native>();
    TestMatrixMatrixMultiply<double, math::MatrixLayout::columnMajor, math::MatrixLayout::rowMajor, math::ImplementationType::native>();
    TestMatrixMatrixMultiply<double, math::MatrixLayout::columnMajor, math::MatrixLayout::columnMajor, math::ImplementationType::native>();
    TestMatrixMatrixMultiply<double, math::MatrixLayout::rowMajor, math::MatrixLayout::rowMajor, math::ImplementationType::openBlas>();
    TestMatrixMatrixMultiply<double, math::MatrixLayout::rowMajor, math::MatrixLayout::columnMajor, math::ImplementationType::openBlas>();
    TestMatrixMatrixMultiply<double, math::MatrixLayout::columnMajor, math::MatrixLayout::rowMajor, math::ImplementationType::openBlas>();
    TestMatrixMatrixMultiply<double, math::MatrixLayout::columnMajor, math::MatrixLayout::columnMajor, math::ImplementationType::openBlas>();

    TestVectorToArray<double>();
    TestVectorToArray<float>();

    TestTensor<double, math::Dimension::column, math::Dimension::row, math::Dimension::channel>();
    TestTensor<double, math::Dimension::channel, math::Dimension::column, math::Dimension::row>();
    TestTensor<float, math::Dimension::column, math::Dimension::row, math::Dimension::channel>();
    TestTensor<float, math::Dimension::channel, math::Dimension::column, math::Dimension::row>();

    TestTensorGetSlice();
    TestTensorFlatten();

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}