////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Matrix_test.tcc (math_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Matrix.h"

template <typename ElementType, math::MatrixLayout layout>
void TestMatrix1()
{
    math::Matrix<ElementType, layout> M(3, 4);
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

template <typename ElementType, math::MatrixLayout layout>
void TestMatrix2()
{
    math::Matrix<ElementType, layout> M(7, 7);

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

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixArchiver()
{
    math::Matrix<ElementType, layout> M(3, 4);
    M(0, 0) = 1;
    M(0, 2) = 4;
    M(2, 3) = 7;

    utilities::SerializationContext context;
    std::stringstream strstream;
    utilities::JsonArchiver archiver(strstream);

    math::MatrixArchiver::Write(M, "test", archiver);
    utilities::JsonUnarchiver unarchiver(strstream, context);

    math::Matrix<ElementType, layout> Ma(0, 0);
    math::MatrixArchiver::Read(Ma, "test", unarchiver);
    testing::ProcessTest("MatrixArchiver, write and read matrix", Ma == M);

}

template <typename ElementType, math::MatrixLayout layout1, math::MatrixLayout layout2>
void TestMatrixCopy()
{
    math::Matrix<ElementType, layout1> M1{
        { 1, 2, 3, 4, 5, 6, 7, 8, 9 },
        { 1, 2, 3, 4, 5, 6, 7, 8, 9 },
        { 1, 2, 3, 4, 5, 6, 7, 8, 9 },
        { 1, 2, 3, 4, 5, 6, 7, 8, 9 }
    };

    math::Matrix<ElementType, layout2> M2(M1);

    testing::ProcessTest("Matrix copy ctor", M1 == M2);
}

template <typename ElementType>
void TestMatrixReference()
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

template <typename ElementType, math::MatrixLayout layout, math::ImplementationType Implementation>
void TestMatrixOperations()
{
    auto implementationName = math::OperationsImplementation<Implementation>::GetImplementationName();
    using Ops = math::OperationsImplementation<Implementation>;

    math::Matrix<ElementType, layout> M{
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

    math::Matrix<ElementType, layout> A1{
        { 1, 2 },
        { 3, 1 },
        { 2, 0 }
    };
    math::Matrix<ElementType, layout> B1{
        { 3, 4,  5,  6 },
        { 8, 9, 10, 11 }
    };
    math::Matrix<ElementType, layout> C1{
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
    testing::ProcessTest(implementationName + "Operations::Add(scalar, MatrixReference)", M == R0);

    Ops::Multiply(static_cast<ElementType>(2), M);
    math::Matrix<ElementType, math::MatrixLayout::columnMajor> R1{
        { 4, 2 },
        { 2, 4 },
        { 6, 6 }
    };
    testing::ProcessTest(implementationName + "Operations::Add(scalar, MatrixReference)", M == R1);

    math::Matrix<ElementType, layout> R2{
        { 0, 2 },
        { 4, 8 },
        { 1, 3 }
    };
    M.CopyFrom(R2);
    testing::ProcessTest(implementationName + "CopyFrom", M == R2);

    math::Matrix<ElementType, layout> D{
        { 10, 1, 9, 1 },
        { 2, 11, 2, 13 },
        { 1, 3, 1, 31 }
    };

    math::RowVector<ElementType> p(D.NumColumns());
    math::RowVector<ElementType> q(D.NumRows());
    math::RowVector<ElementType> a{ 13, 15, 12, 45 };
    math::RowVector<ElementType> b{ 21, 28, 36 };

    Ops::ColumnWiseSum(D, p);
    testing::ProcessTest(implementationName + "Operations::ColumnWiseSum(MatrixReference)", p == a);

    Ops::ColumnWiseSum(D.Transpose(), q);
    testing::ProcessTest(implementationName + "Operations::ColumnWiseSum(MatrixReference)", q == b);
}


template <typename ElementType, math::MatrixLayout layout, math::ImplementationType Implementation>
void TestContiguousMatrixOperations()
{
    auto implementationName = math::OperationsImplementation<Implementation>::GetImplementationName();
    using Ops = math::OperationsImplementation<Implementation>;

    math::Matrix<ElementType, layout> M{
        { 1, 0 },
        { 0, 1 },
        { 2, 2 }
    };

    // Add to entire matrix (contiguous)
    Ops::Add(static_cast<ElementType>(1), M);
    math::Matrix<ElementType, math::MatrixLayout::columnMajor> R1{
        { 2, 1 },
        { 1, 2 },
        { 3, 3 }
    };
    testing::ProcessTest(implementationName + "Operations::Add(scalar, MatrixReference)", M == R1);

    // Add to block (contiguous in one orientation and noncontiguous in another)
    Ops::Add(static_cast<ElementType>(1), M.GetSubMatrix(0, 0, 2, 2));
    math::Matrix<ElementType, math::MatrixLayout::columnMajor> R2{
        { 3, 2 },
        { 2, 3 },
        { 3, 3 }
    };
    testing::ProcessTest(implementationName + "Operations::Add(scalar, MatrixReference)", M == R2);

    // Add to block (contiguous in both orientaitons)
    Ops::Add(static_cast<ElementType>(1), M.GetSubMatrix(1, 0, 2, 1));
    math::Matrix<ElementType, math::MatrixLayout::columnMajor> R3{
        { 3, 2 },
        { 3, 3 },
        { 4, 3 }
    };
    testing::ProcessTest(implementationName + "Operations::Add(scalar, MatrixReference)", M == R3);

    math::Matrix<ElementType, layout> A1{
        { 0, 1 },
        { 2, 3 },
        { 4, 5 }
    };
    M.CopyFrom(A1);
    testing::ProcessTest(implementationName + "CopyFrom", M == A1);

    math::Matrix<ElementType, layout> A2{
        { 1 },
        { 3 }
    };

    M.GetSubMatrix(1, 0, 2, 1).CopyFrom(A2);
    math::Matrix<ElementType, math::MatrixLayout::columnMajor> R4{
        { 0, 1 },
        { 1, 3 },
        { 3, 5 }
    };
    testing::ProcessTest(implementationName + "CopyFrom", M == R4);

    Ops::Multiply(static_cast<ElementType>(2.0), M);
    math::Matrix<ElementType, math::MatrixLayout::columnMajor> R5{
        { 0, 2 },
        { 2, 6 },
        { 6, 10 }
    };
    testing::ProcessTest(implementationName + "Operations::Multiply(scalar, MatrixReference)", M == R5);

    Ops::Multiply(static_cast<ElementType>(2.0), M.GetSubMatrix(1, 0, 2, 1));
    math::Matrix<ElementType, math::MatrixLayout::columnMajor> R6{
        { 0, 2 },
        { 4, 6 },
        { 12, 10 }
    };
    testing::ProcessTest(implementationName + "Operations::Multiply(scalar, MatrixReference)", M == R6);

    Ops::MultiplyAdd(static_cast<ElementType>(0.5), static_cast<ElementType>(1), M);
    math::Matrix<ElementType, math::MatrixLayout::columnMajor> R7{
        { 1, 2 },
        { 3, 4 },
        { 7, 6 }
    };
    testing::ProcessTest(implementationName + "Operations::MultiplyAdd(scalar, scalar, MatrixReference)", M == R7);

    Ops::MultiplyAdd(static_cast<ElementType>(2), static_cast<ElementType>(-1), M.GetSubMatrix(1, 0, 2, 1));
    math::Matrix<ElementType, math::MatrixLayout::columnMajor> R8{
        { 1, 2 },
        { 5, 4 },
        { 13, 6 }
    };
    testing::ProcessTest(implementationName + "Operations::MultiplyAdd(scalar, scalar, MatrixReference)", M == R8);
}

template <typename ElementType, math::MatrixLayout layout>
void TestConstMatrixReference()
{
    math::Matrix<ElementType, layout> M{
        { 1, 2, 4, 0 },
        { 1, 3, 5, 3 },
        { 0, 8, 1, 6 },
        { 1, 2, 4, 3 }
    };

    math::ConstMatrixReference<ElementType, layout> N(M);
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
    testing::ProcessTest("ConstMatrixReference testing GetRow", S.GetRow(0).Norm1() == 3 + 8 + 2);
    testing::ProcessTest("ConstMatrixReference testing GetRow", S.GetRow(1).Norm1() == 5 + 1 + 4);
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

template <typename ElementType, math::MatrixLayout layoutA, math::MatrixLayout layoutB, math::ImplementationType Implementation>
void TestMatrixMatrixMultiply()
{
    auto implementationName = math::OperationsImplementation<Implementation>::GetImplementationName();
    using Ops = math::OperationsImplementation<Implementation>;

    math::Matrix<ElementType, layoutA> A{
        { 1, 2 },
        { 3, 1 },
        { 2, 0 }
    };

    math::Matrix<ElementType, layoutB> B{
        { 3, 4,  5,  6 },
        { 8, 9, 10, 11 }
    };

    math::Matrix<ElementType, layoutA> R{
        { 19, 22, 25, 28 },
        { 17, 21, 25, 29 },
        { 6, 8, 10, 12 }
    };

    math::Matrix<ElementType, layoutA> C(A.NumRows(), B.NumColumns());
    Ops::Multiply(1.0, A, B, 0.0, C);

    testing::ProcessTest(implementationName + "Operations::Multiply(Matrix, Matrix)", C == R);
}
