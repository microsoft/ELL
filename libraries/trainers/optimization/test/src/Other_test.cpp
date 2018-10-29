////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Other_test.cpp (optimization_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Other_test.h"

// optimization
#include "L2Regularizer.h"
#include "MatrixSolution.h"
#include "MatrixExampleSet.h"
#include "MultivariateLoss.h"
#include "NormProx.h"
#include "SGDOptimizer.h"
#include "SquareLoss.h"

// testing
#include "testing.h"

// math
#include "Matrix.h"
#include "Vector.h"

// stl
#include <memory>
#include <random>
#include <vector>

using namespace ell;
using namespace ell::trainers::optimization;

void TestL1Prox()
{
    math::ColumnVector<double> v { 1, 2 ,3, -1, -2, -3, 0.5, -0.5 };
    L1Prox(v, 1.0);

    math::ColumnVector<double> v1{ 1, 2 ,3 , -1, -2, -3, 0.5, -0.5 };
    math::ColumnVector<double> v2{ 1, 2 ,3 , -1, -2, -3, 0.5, -0.5 };
    math::ColumnVector<double> v3{ 1, 2 ,3 , -1, -2, -3, 0.5, -0.5 };
    math::ColumnVector<double> v4{ 1, 2 ,3 , -1, -2, -3, 0.5, -0.5 };
    math::ColumnVector<double> v5{ 1, 2 ,3 , -1, -2, -3, 0.5, -0.5 };

    L1Prox(v1, 0.4);
    math::ColumnVector<double> r1{ 0.6, 1.6 ,2.6 , -0.6, -1.6, -2.6, 0.1, -0.1 };

    L1Prox(v2, 1.0);
    math::ColumnVector<double> r2{ 0, 1 ,2 , 0, -1, -2, 0, 0 };

    L1Prox(v3, 1.2);
    math::ColumnVector<double> r3{ 0, 0.8 ,1.8 , 0, -0.8, -1.8, 0, 0 };

    L1Prox(v4, 2.5);
    math::ColumnVector<double> r4{ 0, 0 ,0.5 , 0, 0, -0.5, 0, 0 };

    L1Prox(v5, 3.0);
    math::ColumnVector<double> r5(8);

    const double tolerance = 1.0e-9;

    bool test1 = v1.IsEqual(r1, tolerance);
    bool test2 = v2.IsEqual(r2, tolerance);
    bool test3 = v3.IsEqual(r3, tolerance);
    bool test4 = v4.IsEqual(r4, tolerance);
    bool test5 = v5.IsEqual(r5, tolerance);

    testing::ProcessTest("TestL1Prox", test1 && test2 && test3 && test4 && test5);
}

void TestLInfinityProx()
{
    std::vector<size_t> scratch;

    math::ColumnVector<double> v1{ 1, 2 ,3 , -1, -2, -3, 0.5, -0.5 };
    LInfinityProx(v1, scratch, 1.0);
    math::ColumnVector<double> r1{ 1, 2 ,2.5, -1, -2, -2.5, 0.5, -0.5 };

    math::ColumnVector<double> v2{ 1, 2 ,3 , -1, -2, -3, 0.5, -0.5 };
    LInfinityProx(v2, scratch, 2.0);
    math::ColumnVector<double> r2{ 1, 2, 2 , -1, -2, -2, 0.5, -0.5 };

    math::ColumnVector<double> v3{ 1, 2 ,3 , -1, -2, -3, 0.5, -0.5 };
    LInfinityProx(v3, scratch, 3.0);
    math::ColumnVector<double> r3{ 1, 1.75, 1.75, -1, -1.75, -1.75, 0.5, -0.5 };

    math::ColumnVector<double> v4{ 1, 2 ,3 , -1, -2, -3, 0.5, -0.5 };
    LInfinityProx(v4, scratch, v4.Norm1() - 0.1 * v4.Size());
    math::ColumnVector<double> r4{ 0.1, 0.1, 0.1, -0.1, -0.1, -0.1, 0.1, -0.1 };

    math::ColumnVector<double> v5{ 1, 2 ,3 , -1, -2, -3, 0.5, -0.5 };
    LInfinityProx(v5, scratch, v5.Norm1());
    math::ColumnVector<double> r5(8);

    math::ColumnVector<double> v6{ -0.5 };
    LInfinityProx(v6, scratch, 0.1);
    math::ColumnVector<double> r6{ -0.4 };

    math::ColumnVector<double> v7{};
    LInfinityProx(v7, scratch, 0.1);

    const double tolerance = 1.0e-9;

    bool test1 = v1.IsEqual(r1, tolerance);
    bool test2 = v2.IsEqual(r2, tolerance);
    bool test3 = v3.IsEqual(r3, tolerance);
    bool test4 = v4.IsEqual(r4, tolerance);
    bool test5 = v5.IsEqual(r5, tolerance);
    bool test6 = v6.IsEqual(r6, tolerance);

    testing::ProcessTest("TestLInfinityProx", test1 && test2 && test3 && test4 && test5 && test6);
}

void TestMatrixExampleSet()
{
    size_t numRows = 10;
    size_t numInputColumns = 7;
    size_t numOutputColumns = 4;

    // generate two random matrices
    std::default_random_engine randomEngine;
    std::normal_distribution<double> normal;

    math::RowMatrix<double> input(numRows, numInputColumns);
    input.Generate([&]() { return normal(randomEngine); });

    math::RowMatrix<double> output(numRows, numOutputColumns);
    output.Generate([&]() { return normal(randomEngine); });

    // use the two matrices to create an example set
    auto examples = std::make_shared<MatrixExampleSet<double>>( std::move(input), std::move(output) );
    
    // train an SGD optimizer on the example set
    auto optimizer = MakeSGDOptimizer<MatrixSolution<double>>(examples, MultivariateLoss<SquareLoss>{}, { 0.0001 });
    optimizer.PerformEpochs();
}
