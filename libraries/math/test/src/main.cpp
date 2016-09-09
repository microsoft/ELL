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
    using ColumnVector = math::Vector<ElementType, math::VectorOrientation::column>;

    ColumnVector v(10);
    v.Fill(2);
    ColumnVector u{ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 };
    testing::ProcessTest("DoubleColumnVector construction and Fill", v == u);

    v.Reset();
    ColumnVector z{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    testing::ProcessTest("DoubleColumnVector::Reset", v == z);

    v[3] = 7;
    v[7] = 9;
    ColumnVector a{ 0, 0, 0, 7, 0, 0, 0, 9, 0, 0 };
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
    using ColumnVector = math::Vector<ElementType, math::VectorOrientation::column>;

    ColumnVector u(10);
    auto v = u.GetReference();
    auto w = v.GetSubVector(1, 3);
    w.Fill(1);
    ColumnVector y{ 0, 1, 1, 1, 0, 0, 0, 0, 0, 0 };
    testing::ProcessTest("ColumnVectorReference::Fill", u == y);

    w.Reset();
    ColumnVector z{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    testing::ProcessTest("ColumnVectorReference::Reset", u == z);

    w[0] = 7;
    w[2] = 9;
    ColumnVector a{ 0, 7, 0, 9, 0, 0, 0, 0, 0, 0 };
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

    v *= 0;
    testing::ProcessTest("ColumnVectorReference::operator *=", v == z);
}

template<typename ElementType>
void TestVectorProduct()
{
    using ColumnVector = math::Vector<ElementType, math::VectorOrientation::column>;
    using RowVector = math::Vector<ElementType, math::VectorOrientation::row>;

    RowVector u{ 0, 1, 0, 1, 0 };
    ColumnVector v{ 1, 2, 3, 4, 5 };
    auto dot = math::Operations::Dot(u, v);
    testing::ProcessTest("Operations::Dot(Vector, Vector)", dot == 6);

    ElementType result;
    math::Operations::Product(u, v, result);
    testing::ProcessTest("Operations::Product(Vector, Vector)", result == 6);
}

template<typename ElementType>
void TestAddTo()
{
    using RowVector = math::Vector<ElementType, math::VectorOrientation::row>;

    RowVector u{ 1, 2, 3, 4, 5 };
    RowVector v{ 0, 1, 0, 1, 0 };
    math::Operations::AddTo(static_cast<ElementType>(2.0), v, u);

    RowVector z{ 1, 4, 3, 6, 5 };
    testing::ProcessTest("Operations::AddTo(scalar, Vector, Vector)", u == z);
}

void TestConstDoubleVector()
{
    const math::DoubleRowVector u{ 0, 1, 0, 1, 0 };
    auto v = u.GetReference();
    //v += 3; // should not compile
}

void TestDoubleMatrix()
{
    math::DoubleColumnMatrix M(15, 10);

    auto N = M.GetBlock(2, 5, 3, 3);
    N(0, 0) = 1.0;
    N(1, 1) = 2.0;
    N(2, 2) = 3.0;

    math::Print(N, std::cout);
    std::cout << std::endl;

    math::Print(M, std::cout);
    std::cout << std::endl;

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

    TestDoubleMatrix();

    if(testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}
