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

    testing::ProcessTest("Matrix::NumColumns", M.NumColumns() == 4 && N.NumColumns() == 2);
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

    testing::ProcessTest("Matrix::Operator()", M == R);
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

    testing::ProcessTest("Matrix::GetDataPointer", M.GetDataPointer() == &(M(0,0)) && S.GetDataPointer() == &(M(1,1)) && M.GetConstDataPointer() == &(M(0, 0)) && S.GetConstDataPointer() == &(M(1, 1)));
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

    math::RowMatrix<ElementType> T{
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

    math::RowMatrix<ElementType> T{
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

    math::RowMatrix<ElementType> R{
        { 1, 0, 4, 0 },
        { 0, 3, 4, 3 },
        { 0, 3, 3, 3 }
    };

    math::RowMatrix<ElementType> A{
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

    math::RowMatrix<ElementType> R{
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

    math::RowMatrix<ElementType> R{
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

    math::RowMatrix<ElementType> R{
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

    math::RowMatrix<ElementType> R1{
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

    math::RowMatrix<ElementType> R{
        { 9, 0, 0 },
        { 2, 9, 8 },
        { 4, 4, 9 },
        { 0, 3, 6 }
    };

    math::RowMatrix<ElementType> S{
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

    math::RowMatrix<ElementType> R{
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

    math::RowMatrix<ElementType> R(2, 4);

    math::RowMatrix<ElementType> T{
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

    math::RowMatrix<ElementType> R{
        { -2, -2, -2, -2 },
        { -2, -2, -2, -2 }
    };

    math::RowMatrix<ElementType> T{
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

    math::RowMatrix<ElementType> R{
        { -2, -2, -2, -2 },
        { -2, -2, -2, -2 }
    };

    math::RowMatrix<ElementType> T{
        { 1, -2, -2, 0 },
        { 0, -2, -2, 7 }
    };

    testing::ProcessTest("Matrix::Generate", M == R && N == T);
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

    math::RowMatrix<ElementType> R{
        { 2, 0, 8, 0 },
        { 0, 0, 0, 14 }
    };

    math::RowMatrix<ElementType> T{
        { 1, 0, 8, 0 },
        { 0, 0, 0, 7 }
    };

    testing::ProcessTest("Matrix::Transform", M == R && N == T);
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
void TestMatrixPlusEqualsOperatorScalar()
{
    math::Matrix<ElementType, layout> M{
        { 1, 2, 0 },
        { 0, 3, 7 }
    };

    M += -2;
    M.GetSubMatrix(0, 1, 2, 2) += 1;

    math::RowMatrix<ElementType> R{
        { -1, 1, -1 },
        { -2, 2, 6 }
    };

    testing::ProcessTest("Matrix::operator+=(scalar)", M == R);
}

template <typename ElementType, math::MatrixLayout layoutA, math::MatrixLayout layoutB, math::ImplementationType implementation>
void TestMatrixPlusEqualsOperatorMatrix()
{
    math::Matrix<ElementType, layoutA> M{
        { 1, 2, 0 },
        { 0, 3, 7 }
    };

    math::Matrix<ElementType, layoutB> N{
        { 4, 3, 2 },
        { 1, 2, 1 }
    };

    M += N;
    M.GetSubMatrix(0, 1, 2, 2) += N.GetSubMatrix(0, 0, 2, 2);

    math::RowMatrix<ElementType> R{
        { 5, 9, 5 },
        { 1, 6, 10 }
    };

    testing::ProcessTest("Matrix::operator+=(Matrix)", M == R);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixMinusEqualsOperatorScalar()
{
    math::Matrix<ElementType, layout> M{
        { 1, 2, 0 },
        { 0, 3, 7 }
    };

    M -= 2;
    M.GetSubMatrix(0, 1, 2, 2) -= (-1);

    math::RowMatrix<ElementType> R{
        { -1, 1, -1 },
        { -2, 2, 6 }
    };

    testing::ProcessTest("Matrix::operator-=(scalar)", M == R);
}

template <typename ElementType, math::MatrixLayout layoutA, math::MatrixLayout layoutB, math::ImplementationType implementation>
void TestMatrixMinusEqualsOperatorMatrix()
{
    math::Matrix<ElementType, layoutA> M{
        { 1, 2, 0 },
        { 0, 3, 7 }
    };

    math::Matrix<ElementType, layoutB> N{
        { -4, -3, -2 },
        { -1, -2, -1 }
    };

    M -= N;
    M.GetSubMatrix(0, 1, 2, 2) -= N.GetSubMatrix(0, 0, 2, 2);

    math::RowMatrix<ElementType> R{
        { 5, 9, 5 },
        { 1, 6, 10 }
    };
    testing::ProcessTest("Matrix::operator-=(Matrix)", M == R);
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

    math::RowMatrix<ElementType> R{
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

    math::RowMatrix<ElementType> R{
        { -1, -4, 0 },
        { 0, -6, 8 }
    };

    testing::ProcessTest("Matrix::operator/=", M == R);
}

template <typename ElementType, math::MatrixLayout layout, math::ImplementationType implementation>
void TestMatrixAddUpdateScalar()
{
    auto implementationName = math::Internal::MatrixOperations<implementation>::GetImplementationName();

    math::Matrix<ElementType, layout> M{
        { 1, 2, 0 },
        { 0, 3, 7 }
    };

    math::AddUpdate<implementation>(static_cast<ElementType>(-2), M);
    math::AddUpdate<implementation>(static_cast<ElementType>(1), M.GetSubMatrix(0, 1, 2, 2));

    math::RowMatrix<ElementType> R{
        { -1, 1, -1 },
        { -2, 2, 6 }
    };

    testing::ProcessTest(implementationName + "::AddUpdate(scalar, Matrix)", M == R);
}

template <typename ElementType, math::MatrixLayout layout, math::ImplementationType implementation>
void TestMatrixAddUpdateZero()
{
    auto implementationName = math::Internal::MatrixOperations<implementation>::GetImplementationName();

    math::Matrix<ElementType, layout> M{
        { 1, 2, 0 },
        { 0, 3, 7 }
    };

    math::AddUpdate<implementation>(static_cast<ElementType>(0), M);
    math::AddUpdate<implementation>(static_cast<ElementType>(0), M.GetSubMatrix(0, 1, 2, 2));

    math::RowMatrix<ElementType> R{
        { 1, 2, 0 },
        { 0, 3, 7 }
    };

    testing::ProcessTest(implementationName + "::AddUpdate(0, Matrix)", M == R);
}

template <typename ElementType, math::MatrixLayout layoutA, math::MatrixLayout layoutB, math::ImplementationType implementation>
void TestMatrixAddUpdateMatrix()
{
    auto implementationName = math::Internal::MatrixOperations<implementation>::GetImplementationName();

    math::Matrix<ElementType, layoutA> A{
        { 1, 2, 0 },
        { 0, 3, 7 }
    };

    math::Matrix<ElementType, layoutB> B{
        { 4, 3, 2 },
        { 1, 2, 1 }
    };

    math::AddUpdate<implementation>(A, B);
    math::AddUpdate<implementation>(A.GetSubMatrix(0, 1, 2, 2), B.GetSubMatrix(0, 1, 2, 2));

    math::RowMatrix<ElementType> R{
        { 5, 7, 2 },
        { 1, 8, 15 }
    };

    testing::ProcessTest(implementationName + "::AddUpdate(Matrix, Matrix)", B == R);
}

template <typename ElementType, math::MatrixLayout layoutA, math::MatrixLayout layoutB, math::ImplementationType implementation>
void TestMatrixAddSetScalar()
{
    auto implementationName = math::Internal::MatrixOperations<implementation>::GetImplementationName();

    math::Matrix<ElementType, layoutA> M{
        { 1, 2, 0 },
        { 0, 3, 7 }
    };
    math::Matrix<ElementType, layoutB>N(2, 3);

    math::AddSet<implementation>(static_cast<ElementType>(-2), M, N);
    math::AddSet<implementation>(static_cast<ElementType>(1), M.GetSubMatrix(0, 1, 2, 2), N.GetSubMatrix(0, 1, 2, 2));

    math::RowMatrix<ElementType> R{
        { -1, 3, 1 },
        { -2, 4, 8 }
    };

    testing::ProcessTest(implementationName + "::AddSet(scalar, Matrix, Matrix)", N == R);
}

template <typename ElementType, math::MatrixLayout layoutA, math::MatrixLayout layoutB, math::ImplementationType implementation>
void TestMatrixAddSetZero()
{
    auto implementationName = math::Internal::MatrixOperations<implementation>::GetImplementationName();

    math::Matrix<ElementType, layoutA> M{
        { 1, 2, 0 },
        { 0, 3, 7 }
    };
    math::Matrix<ElementType, layoutB>N(2, 3);

    math::AddSet<implementation>(static_cast<ElementType>(0), M, N);
    math::AddSet<implementation>(static_cast<ElementType>(0), M.GetSubMatrix(0, 1, 2, 2), N.GetSubMatrix(0, 1, 2, 2));

    testing::ProcessTest(implementationName + "::AddSet(0, Matrix, Matrix)", M == N);
}

template <typename ElementType, math::MatrixLayout layoutA, math::MatrixLayout layoutB, math::ImplementationType implementation>
void TestMatrixAddSetMatrix()
{
    auto implementationName = math::Internal::MatrixOperations<implementation>::GetImplementationName();

    math::Matrix<ElementType, layoutA> A{
        { 1, 2, 0 },
        { 0, 3, 7 }
    };

    math::Matrix<ElementType, layoutB> B{
        { 1, 2, 1 },
        { 0, 1, 6 }
    };

    math::Matrix<ElementType, layoutB> N(2, 3);

    math::AddSet<implementation>(A, B, N);
    math::AddSet<implementation>(A.GetSubMatrix(0, 1, 2, 2), B.GetSubMatrix(0, 1, 2, 2), N.GetSubMatrix(0, 1, 2, 2));

    math::Matrix<ElementType, layoutB> R{
        { 2, 4, 1 },
        { 0, 4, 13 }
    };

    testing::ProcessTest(implementationName + "::AddSet(Matrix, Matrix, Matrix)", N == R);
}

template <typename ElementType, math::MatrixLayout layout, math::ImplementationType implementation>
void TestMatrixScaleUpdate()
{
    auto implementationName = math::Internal::MatrixOperations<implementation>::GetImplementationName();

    math::Matrix<ElementType, layout> M{
        { 1, 2, 0 },
        { 0, 3, 7 }
    };

    math::ScaleUpdate<implementation>(static_cast<ElementType>(-1), M);
    math::ScaleUpdate<implementation>(static_cast<ElementType>(2), M.GetSubMatrix(0, 1, 2, 2));

    math::RowMatrix<ElementType> R{
        { -1, -4, 0 },
        { 0, -6, -14 }
    };

    testing::ProcessTest(implementationName + "::ScaleUpdate(scalar, Matrix)", M == R);
}

template <typename ElementType, math::MatrixLayout layoutA, math::MatrixLayout layoutB, math::ImplementationType implementation>
void TestMatrixScaleSet()
{
    auto implementationName = math::Internal::MatrixOperations<implementation>::GetImplementationName();

    math::Matrix<ElementType, layoutA> M{
        { 1, 2, 0 },
        { 0, 3, 7 }
    };

    math::Matrix<ElementType, layoutB> N(2, 3);

    math::ScaleSet<implementation>(static_cast<ElementType>(0), M, N);
    math::ScaleSet<implementation>(static_cast<ElementType>(-1), M, N);
    math::ScaleSet<implementation>(static_cast<ElementType>(2), M.GetSubMatrix(0, 1, 2, 2), N.GetSubMatrix(0, 1, 2, 2));

    math::RowMatrix<ElementType> R{
        { -1, 4, 0 },
        { 0, 6, 14 }
    };

    testing::ProcessTest(implementationName + "::ScaleSet(scalar, Matrix, Matrix)", N == R);
}

template <typename ElementType, math::MatrixLayout layoutA, math::MatrixLayout layoutB, math::ImplementationType implementation>
void TestMatrixScaleAddUpdateScalarMatrixOne()
{
    auto implementationName = math::Internal::MatrixOperations<implementation>::GetImplementationName();

    math::Matrix<ElementType, layoutA> M{
        { -1, 0, 0 },
        { -1, 1, 3 }
    };

    math::Matrix<ElementType, layoutB> N{
        { 1, 2, 1 },
        { 0, -3, 4 }
    };

    math::ScaleAddUpdate<implementation>(static_cast<ElementType>(0), M, math::One(), N);
    math::ScaleAddUpdate<implementation>(static_cast<ElementType>(1), M, math::One(), N);
    math::ScaleAddUpdate<implementation>(static_cast<ElementType>(-2), M.GetSubMatrix(0,1,2,2), math::One(), N.GetSubMatrix(0,1,2,2));

    math::RowMatrix<ElementType> R{
        { 0, 2, 1 },
        { -1, -4, 1 }
    };

    testing::ProcessTest(implementationName + "::ScaleAddUpdate(scalar, Matrix, one, Matrix)", N == R);
}

template <typename ElementType, math::MatrixLayout layout, math::ImplementationType implementation>
void TestMatrixScaleAddUpdateScalarOnesMatrix()
{
    auto implementationName = math::Internal::MatrixOperations<implementation>::GetImplementationName();

    math::Matrix<ElementType, layout> M{
        { 1, 2, 0 },
        { 0, 3, 7 }
    };

    math::ScaleAddUpdate<implementation>(static_cast<ElementType>(0), math::OnesMatrix(), static_cast<ElementType>(1), M);
    math::ScaleAddUpdate<implementation>(static_cast<ElementType>(-1), math::OnesMatrix(), static_cast<ElementType>(2), M);
    math::ScaleAddUpdate<implementation>(static_cast<ElementType>(1), math::OnesMatrix(), static_cast<ElementType>(-1), M.GetSubMatrix(0, 1, 2, 2));

    math::RowMatrix<ElementType> R{
        { 1, -2, 2 },
        { -1, -4, -12 }
    };

    testing::ProcessTest(implementationName + "::ScaleAddUpdate(scalar, ones, scalar, Matrix)", M == R);
}

template <typename ElementType, math::MatrixLayout layoutA, math::MatrixLayout layoutB, math::ImplementationType implementation>
void TestMatrixScaleAddUpdateOneMatrixScalar()
{
    auto implementationName = math::Internal::MatrixOperations<implementation>::GetImplementationName();

    math::Matrix<ElementType, layoutA> M{
        { 1, 2, -1 },
        { -1, 3, 7 }
    };

    math::Matrix<ElementType, layoutB> N{
        { 1, 0, 0 },
        { 0, 3, 7 }
    };

    math::ScaleAddUpdate<implementation>(math::One(), M, static_cast<ElementType>(0), N);
    math::ScaleAddUpdate<implementation>(math::One(), M, static_cast<ElementType>(-1), N);
    math::ScaleAddUpdate<implementation>(math::One(), M.GetSubMatrix(0, 1, 2, 2), static_cast<ElementType>(-1), N.GetSubMatrix(0, 1, 2, 2));

    math::RowMatrix<ElementType> R{
        { 0, 2, -1 },
        { 0, 3, 7 }
    };

    testing::ProcessTest(implementationName + "::ScaleAddUpdate(one, Matrix, scale, Matrix)", N == R);
}

template <typename ElementType, math::MatrixLayout layoutA, math::MatrixLayout layoutB, math::ImplementationType implementation>
void TestMatrixScaleAddUpdateScalarMatrixScalar()
{
    auto implementationName = math::Internal::MatrixOperations<implementation>::GetImplementationName();

    math::Matrix<ElementType, layoutA> M{
        { 1, -2, 0 },
        { 0, 3, 2 }
    };

    math::Matrix<ElementType, layoutB> N{
        { -1, 2, 0 },
        { 0, -3, 7 }
    };

    math::ScaleAddUpdate<implementation>(static_cast<ElementType>(0), M, static_cast<ElementType>(1), N);
    math::ScaleAddUpdate<implementation>(static_cast<ElementType>(1), M, static_cast<ElementType>(-1), N);
    math::ScaleAddUpdate<implementation>(static_cast<ElementType>(2), M.GetSubMatrix(0, 1, 2, 2), static_cast<ElementType>(2), N.GetSubMatrix(0, 1, 2, 2));

    math::RowMatrix<ElementType> R{
        { 2, -12, 0 },
        { 0, 18, -6 }
    };

    testing::ProcessTest(implementationName + "::ScaleAddUpdate(scalar, Matrix, scalar, Matrix)", N == R);
}

template <typename ElementType, math::MatrixLayout layoutA, math::MatrixLayout layoutB, math::MatrixLayout outputLayout, math::ImplementationType implementation>
void TestMatrixScaleAddSetScalarMatrixOne()
{
    auto implementationName = math::Internal::MatrixOperations<implementation>::GetImplementationName();
    
    math::Matrix<ElementType, layoutA> M{
        { 1, 2, 0 },
        { 0, 3, 7 }
    };
    
    math::Matrix<ElementType, layoutB> N{
        { -1, 1, 3 },
        { 1, 1, 2 }
    };
    
    math::Matrix<ElementType, outputLayout> O(2, 3);
    
    math::ScaleAddSet<implementation>(static_cast<ElementType>(-1), M, math::One(), N, O);

    math::RowMatrix<ElementType> R{
        { -2, -1, 3 },
        { 1, -2, -5 }
    };

    testing::ProcessTest(implementationName + "::ScaleAddSet(scalar, Matrix, one, Matrix, Matrix, Matrix)", O == R);
}

template <typename ElementType, math::MatrixLayout layoutA, math::MatrixLayout layoutB, math::MatrixLayout outputLayout, math::ImplementationType implementation>
void TestMatrixScaleAddSetOneMatrixScalar()
{
    auto implementationName = math::Internal::MatrixOperations<implementation>::GetImplementationName();

    math::Matrix<ElementType, layoutA> M{
        { 1, 2, 0 },
        { 0, 3, 7 }
    };

    math::Matrix<ElementType, layoutB> N{
        { -2, 0, 1 },
        { 2, 1, 0 }
    };

    math::Matrix<ElementType, outputLayout> O(2, 3);

    math::ScaleAddSet<implementation>(math::One(), M, static_cast<ElementType>(-1), N, O);

    math::RowMatrix<ElementType> R{
        { 3, 2, -1 },
        { -2, 2, 7 }
    };

    testing::ProcessTest(implementationName + "::ScaleAddSet(one, Matrix, scalar, Matrix, Matrix, Matrix)", O == R);
}

template <typename ElementType, math::MatrixLayout layoutA, math::MatrixLayout layoutB, math::MatrixLayout outputLayout, math::ImplementationType implementation>
void TestMatrixScaleAddSetScalarMatrixScalar()
{
    auto implementationName = math::Internal::MatrixOperations<implementation>::GetImplementationName();

    math::Matrix<ElementType, layoutA> M{
        { 1, 2, 0 },
        { 0, 3, 7 }
    };

    math::Matrix<ElementType, layoutB> N{
        { 1, -1, 2 },
        { 2, -1, 0 }
    };

    math::Matrix<ElementType, outputLayout> O(2, 3);

    math::ScaleAddSet<implementation>(static_cast<ElementType>(2), M, static_cast<ElementType>(-1), N, O);

    math::RowMatrix<ElementType> R{
        { 1, 5, -2 },
        { -2, 7, 14 }
    };

    testing::ProcessTest(implementationName + "::ScaleAddSet(scalar, Matrix, scalar, Matrix, Matrix, Matrix)", O == R);
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
void TestMatrixVectorMultiplyScaleAddUpdate()
{
    auto implementationName = math::Internal::MatrixOperations<implementation>::GetImplementationName();

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

    math::MultiplyScaleAddUpdate<implementation>(s, M, v, t, u);
    math::MultiplyScaleAddUpdate<implementation>(s, N.GetSubMatrix(1,1,3,2), v, t, w);

    math::ColumnVector<ElementType> r{ 9, 11, 28 };

    testing::ProcessTest(implementationName + "::MultiplyScaleAddUpdate(scalar, Matrix, Vector, scalar, Vector)", u == r && w == r);
}

template <typename ElementType, math::MatrixLayout layout, math::ImplementationType implementation>
void TestVectorMatrixMultiplyScaleAddUpdate()
{
    auto implementationName = math::Internal::MatrixOperations<implementation>::GetImplementationName();

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

    math::MultiplyScaleAddUpdate<implementation>(s, u, M, t, v);
    math::MultiplyScaleAddUpdate<implementation>(s, u, N.GetSubMatrix(1,1,3,2), t, w);
    
    math::RowVector<ElementType> r{ 11, 14 };

    testing::ProcessTest(implementationName + "::MultiplyScaleAddUpdate(scalar, Vector, Matrix, scalar, Vector)", v == r && w == r);
}

template <typename ElementType, math::MatrixLayout layout1, math::MatrixLayout layout2, math::ImplementationType implementation>
void TestMatrixMatrixMultiplyScaleAddUpdate()
{
    auto implementationName = math::Internal::MatrixOperations<implementation>::GetImplementationName();

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
    math::MultiplyScaleAddUpdate<implementation>(static_cast<ElementType>(1), A, B, static_cast<ElementType>(-1), C);

    math::Matrix<ElementType, layout1> CC(A.NumRows()+2, B.NumColumns()+2);
    CC.Fill(1);
    auto CCC = CC.GetSubMatrix(1, 1, 3, 4);
    math::MultiplyScaleAddUpdate<implementation>(static_cast<ElementType>(1), AA.GetSubMatrix(1,1,3,2), BB.GetSubMatrix(1,1,2,4), static_cast<ElementType>(-1), CCC);

    math::Matrix<ElementType, layout1> R{
        { 18, 21, 24, 27 },
        { 16, 20, 24, 28 },
        { 5, 7, 9, 11 }
    };

    testing::ProcessTest(implementationName + "::MultiplyScaleAddUpdate(scalar, Matrix, Matrix, scalar, Matrix)", C == R && CCC == R);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixElementwiseMultiplySet()
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

    math::ElementwiseMultiplySet(M, N, C);

    math::RowMatrix<ElementType> R{
        { -1, 2, 0 },
        { 0, 3, 14 }
    };

    testing::ProcessTest("ElementwiseMultiplySet(Matrix, Matrix, Matrix)", C == R);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixRowwiseCumulativeSumUpdate()
{
    math::Matrix<ElementType, layout> M{
        { 1, 2, 0 },
        { 0, 3, 7 }
    };

    math::RowwiseCumulativeSumUpdate(M);

    math::Matrix<ElementType, layout> R{
        { 1, 3, 3 },
        { 0, 3, 10 }
    };

    testing::ProcessTest("RowwiseCumulativeSumUpdate(Matrix)", M == R);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixColumnwiseCumulativeSumUpdate()
{
    math::Matrix<ElementType, layout> M{
        { 1, 2, 0 },
        { 0, 3, 7 }
    };

    math::ColumnwiseCumulativeSumUpdate(M);

    math::Matrix<ElementType, layout> R{
        { 1, 2, 0 },
        { 1, 5, 7 }
    };

    testing::ProcessTest("ColumnwiseCumulativeSumUpdate(Matrix)", M == R);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixRowwiseConsecutiveDifferenceUpdate()
{
    math::Matrix<ElementType, layout> M{
        { 1, 2, 0 },
        { 0, 3, 7 }
    };

    math::RowwiseConsecutiveDifferenceUpdate(M);

    math::Matrix<ElementType, layout> R{
        { 1, 1, -2 },
        { 0, 3, 4 }
    };

    testing::ProcessTest("RowwiseConsecutiveDifferenceUpdate(Matrix)", M == R);
}

template <typename ElementType, math::MatrixLayout layout>
void TestMatrixColumnwiseConsecutiveDifferenceUpdate()
{
    math::Matrix<ElementType, layout> M{
        { 1, 2, 0 },
        { 0, 3, 7 }
    };

    math::ColumnwiseConsecutiveDifferenceUpdate(M);

    math::Matrix<ElementType, layout> R{
        { 1, 2, 0 },
        { -1, 1, 7 }
    };

    testing::ProcessTest("ColumnwiseConsecutiveDifferenceUpdate(Matrix)", M == R);
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


