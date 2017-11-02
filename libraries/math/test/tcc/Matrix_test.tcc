////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Matrix_test.tcc (math_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Matrix.h"

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixNumRows()
{
    math::Matrix<ElementType, layout> M{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 7 }
    };

    auto N = M.GetSubMatrix(0, 1, 2, 2);

    testing::ProcessTest("Matrix::Operator", M.NumRows() == 3 && N.NumRows() == 2);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixNumColumns()
{
    math::Matrix<ElementType, layout> M{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 7 }
    };

    auto N = M.GetSubMatrix(0, 1, 2, 2);

    testing::ProcessTest("Matrix::Operator", M.NumColumns() == 4 && N.NumColumns() == 2);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixSize()
{
    math::Matrix<ElementType, layout> M{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 7 }
    };

    auto N = M.GetSubMatrix(0, 1, 2, 2);

    testing::ProcessTest("Matrix::Size", M.Size() == 12 && N.Size() == 4);
}

template <typename ElementType>
void TestMatrixGetIncrement()
{
    math::ColumnMatrix<ElementType> M{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 7 }
    };

    math::RowMatrix<ElementType> N{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 7 }
    };

    testing::ProcessTest("Matrix::GetIncrement", M.GetIncrement() == 3 && N.GetIncrement() == 4);
}

template <typename ElementType>
void TestMatrixGetMinorSize()
{
    math::ColumnMatrix<ElementType> M{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 7 }
    };

    math::RowMatrix<ElementType> N{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 7 }
    };

    testing::ProcessTest("Matrix::GetMinorSize", M.GetMinorSize() == 4 && N.GetMinorSize() == 3);
}

template <typename ElementType>
void TestMatrixGetMajorSize()
{
    math::ColumnMatrix<ElementType> M{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 7 }
    };

    math::RowMatrix<ElementType> N{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 7 }
    };

    testing::ProcessTest("Matrix::GetMajorSize", M.GetMajorSize() == 3 && N.GetMajorSize() == 4);
}

template <typename ElementType>
void TestMatrixGetRowIncrement()
{
    math::ColumnMatrix<ElementType> M{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 7 }
    };

    math::RowMatrix<ElementType> N{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 7 }
    };

    testing::ProcessTest("Matrix::GetRowIncrement", M.GetRowIncrement() == 1 && N.GetRowIncrement() == 4);
}

template <typename ElementType>
void TestMatrixGetColumnIncrement()
{
    math::ColumnMatrix<ElementType> M{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 7 }
    };

    math::RowMatrix<ElementType> N{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 7 }
    };

    testing::ProcessTest("Matrix::GetColumnIncrement", M.GetColumnIncrement() == 3 && N.GetColumnIncrement() == 1);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixIndexer()
{
    math::Matrix<ElementType, layout> M(3, 4);
    M(0, 0) = 1;
    M(0, 2) = 4;
    M(2, 3) = 7;

    auto N = M.GetSubMatrix(0, 1, 2, 2);
    N(1, 0) = 3;

    math::ColumnMatrix<ElementType> R{
        { 1, 0, 4, 0 },
        { 0, 3, 0, 0 },
        { 0, 0, 0, 7 }
    };

    testing::ProcessTest("Matrix::Operator", M == R);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixGetDataPointer()
{
    math::Matrix<ElementType, layout> M{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 7 }
    };

    auto S = M.GetSubMatrix(1, 1, 2, 2);

    testing::ProcessTest("Matrix::GetDataPointer", M.GetDataPointer() == &(M(0,0)) && S.GetDataPointer() == &(M(1,1)));
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixGetLayout()
{
    math::Matrix<ElementType, layout> M{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 7 }
    };

    auto N = M.GetSubMatrix(0, 1, 2, 2);

    testing::ProcessTest("Matrix::GetLayout", M.GetLayout() == layout && N.GetLayout() == layout);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixIsContiguous()
{
    math::Matrix<ElementType, layout> M{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 7 }
    };
    
    auto S = M.GetSubMatrix(1, 1, 1, 2);
    
    testing::ProcessTest("Matrix::IsContiguous", M.IsContiguous() == true && S.IsContiguous() == false);
}

template <typename ElementType>
void TestMatrixToArray()
{
    math::ColumnMatrix<ElementType> M{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 7 }
    };

    math::RowMatrix<ElementType> N{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 7 }
    };

    auto Sm = M.GetSubMatrix(0, 1, 2, 2);
    auto Sn = N.GetSubMatrix(0, 1, 2, 2);

    std::vector<ElementType> v{ 1,0,0,0,4,0,0,7 };
    std::vector<ElementType> u{ 0,0,4,0 };
    std::vector<ElementType> x{ 1,0,4,0,0,0,0,7 };
    std::vector<ElementType> y{ 0,4,0,0 };

    testing::ProcessTest("Matrix::ToArray", M.ToArray() == v && Sm.ToArray() == u && N.ToArray() == x && Sn.ToArray() == y);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixSwap()
{
    math::Matrix<ElementType, layout> M{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 7 }
    };

    math::Matrix<ElementType, layout> N{
        { 1, 3 },
        { 0, 3 }
    };

    math::Matrix<ElementType, layout> S{
        { 1, 3 },
        { 0, 3 }
    };

    M.Swap(N);

    testing::ProcessTest("Matrix::Swap", M == S);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixIsEqual()
{
    math::Matrix<ElementType, layout> M{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 7 }
    };

    math::Matrix<ElementType, layout> N{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 7 }
    };

    math::Matrix<ElementType, math::TransposeMatrixLayout<layout>::value> S{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 7 }
    };

    auto A = M.GetSubMatrix(0, 1, 2, 2);

    math::Matrix<ElementType, layout> T{
        { 0, 4 },
        { 0, 0 }
    };

    testing::ProcessTest("Matrix::IsEqual", M.IsEqual(N) && M.IsEqual(S) && A.IsEqual(T) && T.IsEqual(A));
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixEqualityOperator()
{
    math::Matrix<ElementType, layout> M{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 7 }
    };

    math::Matrix<ElementType, layout> N{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 7 }
    };

    math::Matrix<ElementType, math::TransposeMatrixLayout<layout>::value> S{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 7 }
    };

    auto A = M.GetSubMatrix(0, 1, 2, 2);

    math::Matrix<ElementType, layout> T{
        { 0, 4 },
        { 0, 0 }
    };

    testing::ProcessTest("Matrix::operator==", M == N && M == S && A == T && T == A);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixInequalityOperator()
{
    math::Matrix<ElementType, layout> M{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 7 }
    };

    math::Matrix<ElementType, layout> N{
        { 1, 0, 4 },
        { 0, 0, 0 }
    };

    math::Matrix<ElementType, layout> S{
        { 1, 3, 4, 0 },
        { 0, 0, 0, 7 }
    };

    auto A = M.GetSubMatrix(0, 1, 2, 2);
    auto B = M.GetSubMatrix(0, 2, 2, 2);

    testing::ProcessTest("Matrix::operator!=", M != N && M != S && A != B);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixGetConstReference()
{
    math::Matrix<ElementType, layout> M{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 7 }
    };

    auto N = M.GetConstReference();
    auto A = M.GetSubMatrix(0, 1, 2, 2);
    auto B = A.GetConstReference();

    testing::ProcessTest("Matrix::GetConstReference", M == N && A == B);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixGetSubMatrix()
{
    math::Matrix<ElementType, layout> M{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 7 }
    };

    auto N = M.GetSubMatrix(1, 1, 2, 3);
    N.Fill(3);
    N(0, 1) = 4;

    auto S = N.GetSubMatrix(0, 1, 2, 2);

    math::Matrix<ElementType, layout> R{
        { 1, 0, 4, 0 },
        { 0, 3, 4, 3 },
        { 0, 3, 3, 3 }
    };

    math::Matrix<ElementType, layout> A{
        { 4, 3 },
        { 3, 3 }
    };

    testing::ProcessTest("Matrix::GetSubMatrix", M == R && S == A);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixGetColumn()
{
    math::Matrix<ElementType, layout> M{
        { 1, 0, 4, 0 },
        { 0, 3, 4, 3 },
        { 0, 3, 5, 6 }
    };

    auto u = M.GetColumn(1);
    u[0] = 2;
    u[1] = 2;
    u[2] = 8;

    auto N = M.GetSubMatrix(1, 1, 2, 3);
    auto v = N.GetColumn(1);

    math::Matrix<ElementType, layout> R{
        { 1, 2, 4, 0 },
        { 0, 2, 4, 3 },
        { 0, 8, 5, 6 }
    };

    math::ColumnVector<ElementType> w{ 4,5 };

    testing::ProcessTest("Matrix::GetColumn", M == R && v == w);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixGetRow()
{
    math::Matrix<ElementType, layout> M{
        { 1, 0, 4, 0 },
        { 0, 3, 4, 3 },
        { 0, 3, 5, 6 }
    };

    auto u = M.GetRow(1);
    u[0] = 2;
    u[1] = 2;
    u[3] = 8;

    auto N = M.GetSubMatrix(1, 1, 2, 3);
    auto v = N.GetRow(1);

    math::Matrix<ElementType, layout> R{
        { 1, 0, 4, 0 },
        { 2, 2, 4, 8 },
        { 0, 3, 5, 6 }
    };

    math::RowVector<ElementType> w{ 3,5,6 };

    testing::ProcessTest("Matrix::GetRow", M == R && w == v);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixGetDiagonal()
{
    math::Matrix<ElementType, layout> M{
        { 1, 2, 4, 0 },
        { 0, 2, 4, 3 },
        { 0, 8, 5, 6 }
    };

    M.GetDiagonal().Fill(9);

    auto N = M.GetSubMatrix(1, 1, 2, 3);
    auto v = N.GetDiagonal();

    math::Matrix<ElementType, layout> R{
        { 9, 2, 4, 0 },
        { 0, 9, 4, 3 },
        { 0, 8, 9, 6 }
    };

    math::ColumnVector<ElementType> u{ 9,9 };

    testing::ProcessTest("Matrix::GetDiagonal", M == R && u == v);
}

template <typename ElementType>
void TestMatrixGetMajorVector()
{
    math::ColumnMatrix<ElementType> M{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 7 }
    };

    math::RowMatrix<ElementType> N{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 7 }
    };

    M.GetMajorVector(1).Fill(8);
    N.GetMajorVector(1).Fill(8);

    math::ColumnMatrix<ElementType> R1{
        { 1, 8, 4, 0 },
        { 0, 8, 0, 7 }
    };

    math::RowMatrix<ElementType> R2{
        { 1, 0, 4, 0 },
        { 8, 8, 8, 8 }
    };

    testing::ProcessTest("Matrix::GetMajorVector", M == R1 && N == R2);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixTranspose()
{
    math::Matrix<ElementType, layout> M{
        { 9, 2, 4, 0 },
        { 0, 9, 4, 3 },
        { 0, 8, 9, 6 }
    };

    auto T = M.Transpose();
    auto N = M.GetSubMatrix(1, 1, 2, 2).Transpose();

    math::Matrix<ElementType, layout> R{
        { 9, 0, 0 },
        { 2, 9, 8 },
        { 4, 4, 9 },
        { 0, 3, 6 }
    };

    math::Matrix<ElementType, layout> S{
        { 9, 8 },
        { 4, 9 }
    };

    testing::ProcessTest("Matrix::Transpose", T == R && N == S);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixCopyFrom()
{

    math::Matrix<ElementType, layout> M(2, 4);

    math::Matrix<ElementType, layout> N{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 7 }
    };

    math::Matrix<ElementType, layout> S{
        { 2, 6 },
        { 3, 9 }
    };

    M.CopyFrom(N);
    M.GetSubMatrix(0,2,2,2).CopyFrom(S);

    math::Matrix<ElementType, layout> R{
        { 1, 0, 2, 6},
        { 0, 0, 3, 9 }
    };

    testing::ProcessTest("Matrix::CopyFrom", M == R);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixReset()
{
    math::Matrix<ElementType, layout> M{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 7 }
    };

    math::Matrix<ElementType, layout> N{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 7 }
    };

    M.Reset();
    N.GetSubMatrix(0, 1, 2, 2).Reset();

    math::Matrix<ElementType, layout> R(2, 4);

    math::Matrix<ElementType, layout> T{
        { 1, 0, 0, 0 },
        { 0, 0, 0, 7 }
    };

    testing::ProcessTest("Matrix::Reset", M == R && N == T);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixFill()
{
    math::Matrix<ElementType, layout> M{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 7 }
    };

    math::Matrix<ElementType, layout> N{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 7 }
    };

    M.Fill(-2);
    N.GetSubMatrix(0, 1, 2, 2).Fill(-2);

    math::Matrix<ElementType, layout> R{
        { -2, -2, -2, -2 },
        { -2, -2, -2, -2 }
    };

    math::Matrix<ElementType, layout> T{
        { 1, -2, -2, 0 },
        { 0, -2, -2, 7 }
    };

    testing::ProcessTest("Matrix::Fill", M == R && N == T);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixGenerate()
{
    math::Matrix<ElementType, layout> M{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 7 }
    };

    math::Matrix<ElementType, layout> N{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 7 }
    };

    M.Generate([]()->ElementType {return -2; });
    N.GetSubMatrix(0, 1, 2, 2).Generate([]()->ElementType {return -2; });

    math::Matrix<ElementType, layout> R{
        { -2, -2, -2, -2 },
        { -2, -2, -2, -2 }
    };

    math::Matrix<ElementType, layout> T{
        { 1, -2, -2, 0 },
        { 0, -2, -2, 7 }
    };

    testing::ProcessTest("Matrix::Fill", M == R && N == T);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixTransform()
{
    math::Matrix<ElementType, layout> M{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 7 }
    };

    math::Matrix<ElementType, layout> N{
        { 1, 0, 4, 0 },
        { 0, 0, 0, 7 }
    };

    M.Transform([](ElementType x) {return 2 * x; });
    N.GetSubMatrix(0, 1, 2, 2).Transform([](ElementType x) {return 2 * x; });

    math::Matrix<ElementType, layout> R{
        { 2, 0, 8, 0 },
        { 0, 0, 0, 14 }
    };

    math::Matrix<ElementType, layout> T{
        { 1, 0, 8, 0 },
        { 0, 0, 0, 7 }
    };

    testing::ProcessTest("Matrix::Transform", M == R && N == T);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixPrint()
{
    math::Matrix<ElementType, layout> M{
        { 1, 2, 0 },
        { 0, 3, 7 }
    };

    std::stringstream stream1;
    math::Print(M, stream1);
    auto x = stream1.str();

    std::stringstream stream2;
    math::Print(M.GetSubMatrix(0,1,2,2), stream2);
    auto y = stream2.str();

    testing::ProcessTest("Print(Matrix)", x == "{ { 1, 2, 0 },\n  { 0, 3, 7 } }\n" && y == "{ { 2, 0 },\n  { 3, 7 } }\n");
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixScalarAdd()
{
    math::Matrix<ElementType, layout> M{
        { 1, 2, 0 },
        { 0, 3, 7 }
    };

    math::Add(static_cast<ElementType>(-2), M);
    math::Add(static_cast<ElementType>(1), M.GetSubMatrix(0,1,2,2));

    math::Matrix<ElementType, layout> R{
        { -1, 1, -1 },
        { -2, 2, 6 }
    };

    testing::ProcessTest("Add(scalar, Matrix)", M == R);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixPlusEqualsOperator()
{
    math::Matrix<ElementType, layout> M{
        { 1, 2, 0 },
        { 0, 3, 7 }
    };

    M += -2;
    M.GetSubMatrix(0, 1, 2, 2) += 1;

    math::Matrix<ElementType, layout> R{
        { -1, 1, -1 },
        { -2, 2, 6 }
    };

    testing::ProcessTest("Matrix::operator+=", M == R);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixMinusEqualsOperator()
{
    math::Matrix<ElementType, layout> M{
        { 1, 2, 0 },
        { 0, 3, 7 }
    };

    M -= 2;
    M.GetSubMatrix(0, 1, 2, 2) -= (-1);

    math::Matrix<ElementType, layout> R{
        { -1, 1, -1 },
        { -2, 2, 6 }
    };

    testing::ProcessTest("Matrix::operator-=", M == R);
}

template <typename ElementType, math::MatrixLayout layout1, math::MatrixLayout layout2, math::ImplementationType implementation>
void TestMatrixGeneralizedMatrixAdd()
{
    auto implementationName = math::Internal::MatrixOperations<implementation>::GetImplementationName();
    using Ops = math::Internal::MatrixOperations<implementation>;

    math::Matrix<ElementType, layout1> M{
        { 1, 0 },
        { 0, 1 },
        { 2, 2 }
    };

    math::Matrix<ElementType, layout1> N{
        { 1, 0 },
        { 0, 1 },
        { 0, 0 }
    };

    math::Matrix<ElementType, layout2> S{
        { 1, 0 },
        { 1, 0 },
        { 0, 1 }
    };

    Ops::Add(static_cast<ElementType>(-1), N, static_cast<ElementType>(2), S, M);

    math::Matrix<ElementType, layout1> R{
        { 2, 0 },
        { 2, 0 },
        { 2, 4 }
    };

    testing::ProcessTest(implementationName + "::GeneralizedMatrixAdd(scalar, Matrix, scalar, Matrix, Matrix)", M == R);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixRowwiseSum()
{
    math::Matrix<ElementType, layout> M{
        { 1, 2, 0 },
        { 0, 3, 7 }
    };

    math::ColumnVector<ElementType> v(2);
    math::RowwiseSum(M, v);

    math::ColumnVector<ElementType> u{ 3, 10 };

    testing::ProcessTest("ColumnwiseSum(Matrix, Vector)", v == u);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixColumnwiseSum()
{
    math::Matrix<ElementType, layout> M{
        { 1, 2, 0 },
        { 0, 3, 7 }
    };

    math::RowVector<ElementType> v(3);
    math::ColumnwiseSum(M, v);

    math::RowVector<ElementType> u{ 1, 5, 7 };

    testing::ProcessTest("ColumnwiseSum(Matrix, Vector)", v == u);
}

template <typename ElementType, math::MatrixLayout layout, math::ImplementationType implementation>
void TestMatrixScalarMultiply()
{
    auto implementationName = math::Internal::MatrixOperations<implementation>::GetImplementationName();
    using Ops = math::Internal::MatrixOperations<implementation>;

    math::Matrix<ElementType, layout> M{
        { 1, 2, 0 },
        { 0, 3, 7 }
    };

    Ops::Multiply(static_cast<ElementType>(-1), M);
    Ops::Multiply(static_cast<ElementType>(2), M.GetSubMatrix(0,1,2,2));

    math::Matrix<ElementType, layout> R{
        { -1, -4, 0 },
        { 0, -6, -14 }
    };

    testing::ProcessTest(implementationName + "::Multiply(scalar, Matrix)", M == R);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixTimesEqualsOperator()
{
    math::Matrix<ElementType, layout> M{
        { 1, 2, 0 },
        { 0, 3, 7 }
    };

    M *= -1;
    M.GetSubMatrix(0, 1, 2, 2) *= 2;

    math::Matrix<ElementType, layout> R{
        { -1, -4, 0 },
        { 0, -6, -14 }
    };

    testing::ProcessTest("Matrix::operator*=", M == R);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixDivideEqualsOperator()
{
    math::Matrix<ElementType, layout> M{
        { 2, 4, 0 },
        { 0, 6, -8 }
    };

    M /= -2;
    M.GetSubMatrix(0, 1, 2, 2) /= 0.5;

    math::Matrix<ElementType, layout> R{
        { -1, -4, 0 },
        { 0, -6, 8 }
    };

    testing::ProcessTest("Matrix::operator/=", M == R);
}

template <typename ElementType, math::MatrixLayout layout, math::ImplementationType implementation>
void TestMatrixVectorMultiply()
{
    auto implementationName = math::Internal::MatrixOperations<implementation>::GetImplementationName();
    using Ops = math::Internal::MatrixOperations<implementation>;

    math::Matrix<ElementType, layout> M{
        { 1, 0 },
        { 0, 1 },
        { 2, 2 }
    };

    math::Matrix<ElementType, layout> N{
        { 1, 0 ,3, 1 },
        { 0, 1, 0, -1 },
        { 2, 0, 1, 3 },
        { 2, 2, 2, 3 }
    };

    math::ColumnVector<ElementType> u{ 1, 1, 0 };
    math::ColumnVector<ElementType> w{ 1, 1, 0 };
    math::ColumnVector<ElementType> v{ 3, 4 };

    ElementType s = 2;
    ElementType t = 3;

    Ops::Multiply(s, M, v, t, u);
    Ops::Multiply(s, N.GetSubMatrix(1,1,3,2), v, t, w);

    math::ColumnVector<ElementType> r{ 9, 11, 28 };

    testing::ProcessTest(implementationName + "::Multiply(scalar, Matrix, Vector, scalar, Vector)", u == r && w == r);
}

template <typename ElementType, math::MatrixLayout layout, math::ImplementationType implementation>
void TestVectorMatrixMultiply()
{
    auto implementationName = math::Internal::MatrixOperations<implementation>::GetImplementationName();
    using Ops = math::Internal::MatrixOperations<implementation>;

    math::Matrix<ElementType, layout> M{
        { 1, 0 },
        { 0, 1 },
        { 2, 2 }
    };

    math::Matrix<ElementType, layout> N{
        { 1, 0 ,3, 1 },
        { 0, 1, 0, -1 },
        { 2, 0, 1, 3 },
        { 2, 2, 2, 3 }
    };

    math::RowVector<ElementType> u{ 1, 1, 0 };
    math::RowVector<ElementType> v{ 3, 4 };
    math::RowVector<ElementType> w{ 3, 4 };

    ElementType s = 2;
    ElementType t = 3;

    Ops::Multiply(s, u, M, t, v);
    Ops::Multiply(s, u, N.GetSubMatrix(1,1,3,2), t, w);
    math::RowVector<ElementType> r{ 11, 14 };

    testing::ProcessTest(implementationName + "::Multiply(scalar, Vector, Matrix, scalar, Vector)", v == r && w == r);
}

template <typename ElementType, math::MatrixLayout layout1, math::MatrixLayout layout2, math::ImplementationType implementation>
void TestMatrixMatrixMultiply()
{
    auto implementationName = math::Internal::MatrixOperations<implementation>::GetImplementationName();
    using Ops = math::Internal::MatrixOperations<implementation>;

    math::Matrix<ElementType, layout1> A{
        { 1, 2 },
        { 3, 1 },
        { 2, 0 }
    };

    // a padded version of A
    math::Matrix<ElementType, layout1> AA{
        { 1, 1, 1, 1},
        { 1, 1, 2, 1},
        { 1, 3, 1, 1},
        { 1, 2, 0, 1},
        { 1, 1, 1, 1}
    };

    math::Matrix<ElementType, layout2> B{
        { 3, 4,  5,  6 },
        { 8, 9, 10, 11 }
    };

    // A padded version of B
    math::Matrix<ElementType, layout2> BB{
        { 1, 1, 1, 1, 1, 1 },
        { 1, 3, 4, 5, 6, 1 },
        { 1, 8, 9, 10, 11, 1 },
        { 1, 1, 1, 1, 1, 1 }
    };

    math::Matrix<ElementType, layout1> C(A.NumRows(), B.NumColumns());
    C.Fill(1);
    Ops::Multiply(static_cast<ElementType>(1), A, B, static_cast<ElementType>(-1), C);

    math::Matrix<ElementType, layout1> CC(A.NumRows()+2, B.NumColumns()+2);
    CC.Fill(1);
    auto CCC = CC.GetSubMatrix(1, 1, 3, 4);
    Ops::Multiply(static_cast<ElementType>(1), AA.GetSubMatrix(1,1,3,2), BB.GetSubMatrix(1,1,2,4), static_cast<ElementType>(-1), CCC);

    math::Matrix<ElementType, layout1> R{
        { 18, 21, 24, 27 },
        { 16, 20, 24, 28 },
        { 5, 7, 9, 11 }
    };

    testing::ProcessTest(implementationName + "::Multiply(scalar, Matrix, Matrix, scalar, Matrix)", C == R && CCC == R);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixScalarMultiplyAdd()
{
    math::Matrix<ElementType, layout> M{
        { 1, 2, 0 },
        { 0, 3, 7 }
    };

    math::MultiplyAdd(static_cast<ElementType>(-1), static_cast<ElementType>(2), M);

    math::Matrix<ElementType, layout> R{
        { 1, 0, 2 },
        { 2, -1, -5 }
    };

    testing::ProcessTest("MultiplyAdd(scalar, scalar, Matrix)", M == R);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixElementwiseMultiply()
{
    math::Matrix<ElementType, layout> M{
        { 1, 2, 0 },
        { 0, 3, 7 }
    };

    math::Matrix<ElementType, layout> N{
        { -1, 1, -1 },
        { 1, 1, 2 }
    };

    math::Matrix<ElementType, layout> C(2, 3);

    math::ElementwiseMultiply(M, N, C);

    math::Matrix<ElementType, layout> R{
        { -1, 2, 0 },
        { 0, 3, 14 }
    };

    testing::ProcessTest("ElementwiseMultiply(Matrix, Matrix, Matrix)", C == R);
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

    testing::ProcessTest("MatrixArchiver", Ma == M);
}

template <typename ElementType, math::MatrixLayout layout1, math::MatrixLayout layout2>
void TestMatrixCopyCtor()
{
    math::Matrix<ElementType, layout1> M1{
        { 1, 2, 3, 4, 5, 6, 7, 8, 9 },
        { 1, 2, 3, 4, 5, 6, 7, 8, 9 },
        { 1, 2, 3, 4, 5, 6, 7, 8, 9 },
        { 1, 2, 3, 4, 5, 6, 7, 8, 9 }
    };

    math::Matrix<ElementType, layout2> M2(M1);

    testing::ProcessTest("Matrix(Matrix)", M1 == M2);
}

