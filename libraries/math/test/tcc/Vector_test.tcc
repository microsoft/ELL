////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Vector_test.tcc (math_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// testing
#include "testing.h"

// stl
#include <random>

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
