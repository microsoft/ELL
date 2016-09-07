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

void TestDoubleVector()
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

    testing::ProcessTest("DoubleColumnVector::Norm2", testing::IsEqual(a.Norm2(), std::sqrt(7*7+9*9)));
    testing::ProcessTest("DoubleColumnVector::Norm1", a.Norm1() == 7+9);
    testing::ProcessTest("DoubleColumnVector::Norm0", a.Norm0() == 2);
    testing::ProcessTest("DoubleColumnVector::Min", a.Min() == 0);
    testing::ProcessTest("DoubleColumnVector::Max", a.Max() == 9);

    std::default_random_engine rng;
    std::normal_distribution<double> normal(0, 1.0);
    auto generator = [&]() { return normal(rng); };
    v.Generate(generator);

    v *= 0;
    testing::ProcessTest("DoubleColumnVector::operator *=", v == z);
}

void TestSingleVector()
{
    math::SingleColumnVector v(10);
    v.Fill(2);
    math::SingleColumnVector u{ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 };
    testing::ProcessTest("SingleColumnVector construction and Fill", v == u);

    v.Reset();
    math::SingleColumnVector z{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    testing::ProcessTest("SingleColumnVector::Reset", v == z);

    v[3] = 7;
    v[7] = 9;
    math::SingleColumnVector a{ 0, 0, 0, 7, 0, 0, 0, 9, 0, 0 };
    testing::ProcessTest("SingleColumnVector::operator[]", v == a);

    testing::ProcessTest("SingleColumnVector::Norm2", testing::IsEqual(a.Norm2(), std::sqrtf(7*7+9*9)));
    testing::ProcessTest("SingleColumnVector::Norm1", a.Norm1() == 7+9);
    testing::ProcessTest("SingleColumnVector::Norm0", a.Norm0() == 2);
    testing::ProcessTest("SingleColumnVector::Min", a.Min() == 0);
    testing::ProcessTest("SingleColumnVector::Max", a.Max() == 9);

    std::default_random_engine rng;
    std::normal_distribution<float> normal(0, 1.0);
    auto generator = [&]() { return normal(rng); };
    v.Generate(generator);

    v *= 0;
    testing::ProcessTest("SingleColumnVector::operator *=", v == z);
}

void TestDoubleVectorReference()
{
    math::DoubleColumnVector u(10);
    const auto v = u.GetReference();
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

    auto b = w.GetSubVector(0, 2);
    testing::ProcessTest("DoubleColumnVectorReference::Norm2", testing::IsEqual(b.Norm2(), std::sqrt(7*7)));
    testing::ProcessTest("DoubleColumnVectorReference::Norm1", b.Norm1() == 7);
    testing::ProcessTest("DoubleColumnVectorReference::Norm0", b.Norm0() == 1);
    testing::ProcessTest("DoubleColumnVectorReference::Min", b.Min() == 0);
    testing::ProcessTest("DoubleColumnVectorReference::Max", b.Max() == 7);

    std::default_random_engine rng;
    std::normal_distribution<double> normal(0, 1.0);
    auto generator = [&]() { return normal(rng); };
    w.Generate(generator);

    //    v *= 0;
    //    testing::ProcessTest("DoubleColumnVectorReference::operator *=", v == z);
}

void TestSingleVectorReference()
{
    math::SingleColumnVector u(10);
    auto v = u.GetReference();
    auto w = v.GetSubVector(1, 3, 2);
    w.Fill(1);
    math::SingleColumnVector y{ 0, 1, 0, 1, 0, 1, 0, 0, 0, 0 };
    testing::ProcessTest("SingleColumnVectorReference::Fill", u == y);

    w.Reset();
    math::SingleColumnVector z{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    testing::ProcessTest("SingleColumnVectorReference::Reset", u == z);

    w[0] = 7;
    w[2] = 9;
    math::SingleColumnVector a{ 0, 7, 0, 0, 0, 9, 0, 0, 0, 0 };
    testing::ProcessTest("SingleColumnVectorReference::operator[]", u == a);

    auto b = w.GetSubVector(0, 2);
    testing::ProcessTest("SingleColumnVectorReference::Norm2", testing::IsEqual(b.Norm2(), std::sqrtf(7*7)));
    testing::ProcessTest("SingleColumnVectorReference::Norm1", b.Norm1() == 7);
    testing::ProcessTest("SingleColumnVectorReference::Norm0", b.Norm0() == 1);
    testing::ProcessTest("SingleColumnVectorReference::Min", b.Min() == 0);
    testing::ProcessTest("SingleColumnVectorReference::Max", b.Max() == 7);

    std::default_random_engine rng;
    std::normal_distribution<float> normal(0, 1.0);
    auto generator = [&]() { return normal(rng); };
    w.Generate(generator);

    v *= 0;
    testing::ProcessTest("SingleColumnVectorReference::operator *=", v == z);
}

void TestDoubleVectorProduct()
{
    math::DoubleRowVector u{ 0, 1, 0, 1, 0 };
    math::DoubleColumnVector v{ 1, 2, 3, 4, 5 };
    auto dot = math::TensorOperations::Dot(u, v);
    testing::ProcessTest("TensorOperations::Dot(Vector, Vector)", dot == 6);

    double result;
    math::TensorOperations::Product(u, v, result);
    testing::ProcessTest("TensorOperations::Product(Vector, Vector)", result == 6);
}

void TestSingleVectorProduct()
{
    math::SingleRowVector u{ 0, 1, 0, 1, 0 };
    math::SingleColumnVector v{ 1, 2, 3, 4, 5 };
    auto dot = math::TensorOperations::Dot(u, v);
    testing::ProcessTest("TensorOperations::Dot(Vector, Vector)", dot == 6);

    float result;
    math::TensorOperations::Product(u, v, result);
    testing::ProcessTest("TensorOperations::Product(Vector, Vector)", result == 6);
}

void TestConstDoubleVector()
{
    const math::DoubleRowVector u{ 0, 1, 0, 1, 0 };
    auto v = u.GetReference();
    // decltype(u.GetReference()) v = u.GetReference(); // won't compile
    std::cout << "typeof(v): " << typeid(v).name() << std::endl;
    v += 3; // how does this compile and work?
}


/// Runs all tests
///
int main()
{
    TestDoubleVector();
    TestDoubleVectorReference();
    TestDoubleVectorProduct();
    TestSingleVector();
    TestSingleVectorReference();
    TestSingleVectorProduct();
    TestConstDoubleVector();

    if(testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}
