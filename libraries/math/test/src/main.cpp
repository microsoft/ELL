////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     main.cpp (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Tensor.h"

// testing
#include "testing.h"

// stl
#include <random>

void TestVector()
{
    math::DoubleColumnVector v(10);
    v.Fill(2);
    math::DoubleColumnVector u{ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 };
    testing::ProcessTest("DoubleColumnVector construction and Fill", v == u);

    v.Reset();
    math::DoubleColumnVector z{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    testing::ProcessTest("DoubleColumnVector::Reset", v == z);

    v[3] = 7;
    v[7] = 9;
    math::DoubleColumnVector a{ 0, 0, 0, 7, 0, 0, 0, 9, 0, 0 };
    testing::ProcessTest("DoubleColumnVector::operator[]", v == a);

    std::default_random_engine rng;
    std::normal_distribution<double> normal(0, 1.0);
    auto generator = [&]() { return normal(rng); };
    v.Generate(generator);
}

void TestVectorReference()
{
    math::DoubleColumnVector u(10);
    auto v = u.GetReference();
    auto w = v.GetSubVector(1, 3, 2);
    w.Fill(1);
    math::DoubleColumnVector y{ 0, 1, 0, 1, 0, 1, 0, 0, 0, 0 };
    testing::ProcessTest("DoubleColumnVectorReference::Fill", u == y);

    w.Reset();
    math::DoubleColumnVector z{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    testing::ProcessTest("DoubleColumnVectorReference::Reset", u == z);

    w[0] = 7;
    w[2] = 9;
    math::DoubleColumnVector a{ 0, 7, 0, 0, 0, 9, 0, 0, 0, 0 };
    testing::ProcessTest("DoubleColumnVectorReference::operator[]", u == a);

    std::default_random_engine rng;
    std::normal_distribution<double> normal(0, 1.0);
    auto generator = [&]() { return normal(rng); };
    w.Generate(generator);
}

void TestVectorProduct()
{
    math::DoubleRowVector u{ 0, 1, 0, 1, 0 };
    math::DoubleColumnVector v{ 1, 2, 3, 4, 5 };
    auto dot = math::TensorOperations::Dot(u, v);
    testing::ProcessTest("TensorOperations::Dot(Vector, Vector)", dot == 6);

    double result;
    math::TensorOperations::Product(u, v, result);
    testing::ProcessTest("TensorOperations::Product(Vector, Vector)", result == 6);

    dot = math::TensorOperations::Dot(u.GetConstReference(), v.GetConstReference());
    testing::ProcessTest("TensorOperations::Dot(VectorConstReference, VectorConstReference)", dot == 6);

    math::TensorOperations::Product(u.GetConstReference(), v.GetConstReference(), result);
    testing::ProcessTest("TensorOperations::Product(VectorConstReference, VectorConstReference)", result == 6);

}

/// Runs all tests
///
int main()
{
    TestVector();
    TestVectorReference();
    TestVectorProduct();

    if(testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}
