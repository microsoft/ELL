////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (functions_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "HingeLoss.h"
#include "LogLoss.h"
#include "SquaredLoss.h"
#include "SmoothHingeLoss.h"
#include "L2Regularizer.h"
#include "ElasticNetRegularizer.h"

// testing
#include "testing.h"

// math
#include "Vector.h"
#include "VectorOperations.h"

// stl
#include <cmath>
#include <iostream>
#include <string>
#include <array>
#include <random>
#include <functional>

using namespace ell;
using Range = std::array<double, 3>;

void HingeLossTest()
{
    functions::HingeLoss hingeLoss;
    testing::ProcessTest("Testing functions::HingeLoss(2,1)", testing::IsEqual(hingeLoss(2, 1), 0.0));
    testing::ProcessTest("Testing functions::HingeLoss(-2,-1)", testing::IsEqual(hingeLoss(-2, -1), 0.0));
    testing::ProcessTest("Testing functions::HingeLoss(2,-1)", testing::IsEqual(hingeLoss(2, -1), 3.0));
    testing::ProcessTest("Testing functions::HingeLoss(-2,1)", testing::IsEqual(hingeLoss(-2, 1), 3.0));
    testing::ProcessTest("Testing functions::HingeLoss(0,1)", testing::IsEqual(hingeLoss(0, 1), 1.0));
    testing::ProcessTest("Testing functions::HingeLoss(0,-1)", testing::IsEqual(hingeLoss(0, -1), 1.0));

    testing::ProcessTest("Testing functions::HingeLoss::GetDerivative(2,1)", testing::IsEqual(hingeLoss.GetDerivative(2, 1), 0.0));
    testing::ProcessTest("Testing functions::HingeLoss::GetDerivative(-2,-1)", testing::IsEqual(hingeLoss.GetDerivative(-2, -1), 0.0));
    testing::ProcessTest("Testing functions::HingeLoss::GetDerivative(2,-1)", testing::IsEqual(hingeLoss.GetDerivative(2, -1), 1.0));
    testing::ProcessTest("Testing functions::HingeLoss::GetDerivative(-2,1)", testing::IsEqual(hingeLoss.GetDerivative(-2, 1), -1.0));
    testing::ProcessTest("Testing functions::HingeLoss::GetDerivative(0,1)", testing::IsEqual(hingeLoss.GetDerivative(0, 1), -1.0));
    testing::ProcessTest("Testing functions::HingeLoss::GetDerivative(0,-1)", testing::IsEqual(hingeLoss.GetDerivative(0, -1), 1.0));
}

void LogLossTest()
{
    functions::LogLoss logLoss;
    testing::ProcessTest("Testing functions::LogLoss(2,1)", testing::IsEqual(logLoss(2, 1), std::log(1 + std::exp(-2))));
    testing::ProcessTest("Testing functions::LogLoss(-2,-1)", testing::IsEqual(logLoss(-2, -1), std::log(1 + std::exp(-2))));
    testing::ProcessTest("Testing functions::LogLoss(2,-1)", testing::IsEqual(logLoss(2, -1), std::log(1 + std::exp(2))));
    testing::ProcessTest("Testing functions::LogLoss(-2,1)", testing::IsEqual(logLoss(-2, 1), std::log(1 + std::exp(2))));
    testing::ProcessTest("Testing functions::LogLoss(0,1)", testing::IsEqual(logLoss(0, 1), std::log(2)));
    testing::ProcessTest("Testing functions::LogLoss(0,-1)", testing::IsEqual(logLoss(0, -1), std::log(2)));

    testing::ProcessTest("Testing functions::LogLoss::GetDerivative(2,1)", testing::IsEqual(logLoss.GetDerivative(2, 1), -1 / (1 + std::exp(2))));
    testing::ProcessTest("Testing functions::LogLoss::GetDerivative(-2,-1)", testing::IsEqual(logLoss.GetDerivative(-2, -1), 1 / (1 + std::exp(2))));
    testing::ProcessTest("Testing functions::LogLoss::GetDerivative(2,-1)", testing::IsEqual(logLoss.GetDerivative(2, -1), 1 / (1 + std::exp(-2))));
    testing::ProcessTest("Testing functions::LogLoss::GetDerivative(-2,1)", testing::IsEqual(logLoss.GetDerivative(-2, 1), -1 / (1 + std::exp(-2))));
    testing::ProcessTest("Testing functions::LogLoss::GetDerivative(0,1)", testing::IsEqual(logLoss.GetDerivative(0, 1), -1 / 2.0));
    testing::ProcessTest("Testing functions::LogLoss::GetDerivative(0,-1)", testing::IsEqual(logLoss.GetDerivative(0, -1), 1 / 2.0));
}

void SquaredLossTest()
{
    functions::SquaredLoss squaredLoss;
    testing::ProcessTest("Testing functions::SquaredLoss(4,4)", testing::IsEqual(squaredLoss(4, 4), 0.0));
    testing::ProcessTest("Testing functions::SquaredLoss(4,2)", testing::IsEqual(squaredLoss(4, 2), 2.0));
    testing::ProcessTest("Testing functions::SquaredLoss(2,4)", testing::IsEqual(squaredLoss(2, 4), 2.0));

    testing::ProcessTest("Testing functions::SquaredLoss::GetDerivative(4,4)", testing::IsEqual(squaredLoss.GetDerivative(4, 4), 0.0));
    testing::ProcessTest("Testing functions::SquaredLoss::GetDerivative(4,2)", testing::IsEqual(squaredLoss.GetDerivative(4, 2), 2.0));
    testing::ProcessTest("Testing functions::SquaredLoss::GetDerivative(2,4)", testing::IsEqual(squaredLoss.GetDerivative(2, 4), -2.0));
}

void SmoothHingeLossTest()
{
    functions::SmoothHingeLoss smoothHingeLoss;
    testing::ProcessTest("Testing functions::SmoothHingeLoss(2,1)", testing::IsEqual(smoothHingeLoss(2, 1), 0.0));
    testing::ProcessTest("Testing functions::SmoothHingeLoss(-2,-1)", testing::IsEqual(smoothHingeLoss(-2, -1), 0.0));
    testing::ProcessTest("Testing functions::SmoothHingeLoss(2,-1)", testing::IsEqual(smoothHingeLoss(2, -1), 2.5));
    testing::ProcessTest("Testing functions::SmoothHingeLoss(-2,1)", testing::IsEqual(smoothHingeLoss(-2, 1), 2.5));
    testing::ProcessTest("Testing functions::SmoothHingeLoss(0,1)", testing::IsEqual(smoothHingeLoss(0, 1), 0.5));
    testing::ProcessTest("Testing functions::SmoothHingeLoss(0,-1)", testing::IsEqual(smoothHingeLoss(0, -1), 0.5));

    testing::ProcessTest("Testing functions::SmoothHingeLoss::GetDerivative(2,1)", testing::IsEqual(smoothHingeLoss.GetDerivative(2, 1), 0.0));
    testing::ProcessTest("Testing functions::SmoothHingeLoss::GetDerivative(-2,-1)", testing::IsEqual(smoothHingeLoss.GetDerivative(-2, -1), 0.0));
    testing::ProcessTest("Testing functions::SmoothHingeLoss::GetDerivative(2,-1)", testing::IsEqual(smoothHingeLoss.GetDerivative(2, -1), 1.0));
    testing::ProcessTest("Testing functions::SmoothHingeLoss::GetDerivative(-2,1)", testing::IsEqual(smoothHingeLoss.GetDerivative(-2, 1), -1.0));
    testing::ProcessTest("Testing functions::SmoothHingeLoss::GetDerivative(0,1)", testing::IsEqual(smoothHingeLoss.GetDerivative(0, 1), -1.0));
    testing::ProcessTest("Testing functions::SmoothHingeLoss::GetDerivative(0,-1)", testing::IsEqual(smoothHingeLoss.GetDerivative(0, -1), 1.0));
}

template<typename LossType>
void DerivativeTest(LossType loss, double prediction, double label)
{
    double epsilon = 1.0e-4;
    double lossPlus = loss(prediction + epsilon, label);
    double lossMinus = loss(prediction - epsilon, label);
    double limit = (lossPlus - lossMinus) / (2 * epsilon);
    double error = std::abs(loss.GetDerivative(prediction, label) - limit);
    testing::ProcessTest("Testing " + std::string(typeid(LossType).name()) + "::GetDerivative(" + std::to_string(prediction) + ',' + std::to_string(label) + ")", error < epsilon);
}

template<typename LossType>
void DerivativeTest(LossType loss, Range predictionRange, Range labelRange)
{
    for (double prediction = predictionRange[0]; prediction <= predictionRange[2]; prediction += predictionRange[1])
    {
        for (double label = labelRange[0]; label <= labelRange[2]; label += labelRange[1])
        {
            DerivativeTest(loss, prediction, label);
        }
    }
}

// returns true if the function never takes a value lower than 'bound' in the range 'range'
template<typename FunctionType>
bool IsLowerBound(const FunctionType& function, double bound, Range range)
{
    for (double comparator = range[0]; comparator <= range[2]; comparator += range[1])
    {
        const double epsilon = 1.0e-5;
        double value = function(comparator);
        if (value < bound - epsilon)
        {
            return false;
        }
    }
    return true;
}

template<typename LossType>
void LossConjugateTest(LossType loss, double dual, double label, Range comparatorRange)
{
    auto objective = [&](double x) {return -x*dual + loss(x, label); };
    double negativeConjugate = -loss.Conjugate(dual, label);
    bool isLowerBound = IsLowerBound(objective, negativeConjugate, comparatorRange);
    testing::ProcessTest("Testing " + std::string(typeid(LossType).name()) + "::Conjugate(" + std::to_string(dual) + ',' + std::to_string(label) + ")", isLowerBound);
}

template<typename LossType>
void LossConjugateTest(LossType loss, Range dualRange, Range labelRange, Range comparatorRange)
{
    for (double dual = dualRange[0]; dual <= dualRange[2]; dual += dualRange[1])
    {
        for (double label = labelRange[0]; label <= labelRange[2]; label += labelRange[1])
        {
            LossConjugateTest(loss, dual, label, comparatorRange);
        }
    }
}

// Test if Fenchel-Young holds at given points
template<typename RegularizerType>
void RegularizerConjugateTest(RegularizerType regularizer, math::ConstColumnVectorReference<double> p, math::ConstColumnVectorReference<double> d)
{
    const double epsilon = 1.0e-8;
    double fenchelYoung = regularizer(p) + regularizer.Conjugate(d);
    double dot = math::Dot(p,d);
    testing::ProcessTest("Testing " + std::string(typeid(RegularizerType).name()) + "::Conjugate()", dot < fenchelYoung + epsilon);
}

// Test if Fenchel-Young holds at multiple random points
template<typename RegularizerType>
void RegularizerConjugateTest(RegularizerType regularizer, size_t dimension, size_t repetitions)
{
    math::ColumnVector<double> p(dimension);
    math::ColumnVector<double> d(dimension);

    std::random_device rd;
    std::mt19937 generator(rd());
    std::normal_distribution<> normalDistribution(0, 1);
    auto normalGenerator = [&]() {return normalDistribution(generator); };

    for (size_t t = 0; t > repetitions; ++t)
    {
        p.Generate(normalGenerator);
        d.Generate(normalGenerator);

        RegularizerConjugateTest(regularizer, p, d);
        RegularizerConjugateTest(regularizer, p, p);
    }
}

template<typename LossType>
void ConjugateProxTest(LossType loss, double sigma, double dual, double label, Range comparatorRange)
{
    auto objective = [&](double x) {return sigma * loss.Conjugate(x, label) + 0.5 * std::pow(x - dual, 2); };
    double conjugateProx = loss.ConjugateProx(sigma, dual, label);
    double conjugateProxObjective = objective(conjugateProx);
    bool isLowerBound = IsLowerBound(objective, conjugateProxObjective, comparatorRange);
    testing::ProcessTest("Testing " + std::string(typeid(LossType).name()) + "::ConjugateProx(" + std::to_string(sigma) + ',' + std::to_string(dual) + ',' + std::to_string(label) + ")", isLowerBound);
}

template<typename LossType>
void ConjugateProxTest(LossType loss, double sigma, Range dualRange, Range labelRange, Range comparatorRange)
{
    for (double dual = dualRange[0]; dual <= dualRange[2]; dual += dualRange[1])
    {
        for (double label = labelRange[0]; label <= labelRange[2]; label += labelRange[1])
        {
            ConjugateProxTest<LossType>(loss, sigma, dual, label, comparatorRange);
        }
    }
}

/// Runs all tests
///
int main()
{
    HingeLossTest();
    LogLossTest();
    SquaredLossTest();
    SmoothHingeLossTest();

    DerivativeTest(functions::HingeLoss(), { -2.0, exp(1), 2.0 }, { -1.0, 2.0, 1.0 });
    DerivativeTest(functions::LogLoss(), { -2.0, 0.2, 2.0 }, { -1.0, 2.0, 1.0 });
    DerivativeTest(functions::SquaredLoss(), { -2.0, 0.2, 2.0 }, { -1.0, 0.2, 1.0 });
    DerivativeTest(functions::SmoothHingeLoss(), { -2.0, 0.2, 2.0 }, { -1.0, 2.0, 1.0 });

    LossConjugateTest(functions::SquaredLoss(), { -1.0, 0.1, 1.0 }, { -1.0, 0.1, 1.0 }, { -2.0, 0.1, 2.0 });
    LossConjugateTest(functions::LogLoss(), { -1.0, 0.1, 0.0 }, { 1.0, 0.1, 1.0 }, { -2.0, 0.1, 2.0 });
    LossConjugateTest(functions::LogLoss(), { 0.0, 0.1, 1.0 }, { -1.0, 0.1, -1.0 }, { -2.0, 0.1, 2.0 });
    LossConjugateTest(functions::SmoothHingeLoss(), { -1.0, 0.1, 0.0 }, { 1.0, 0.1, 1.0 }, { -2, 0.1, 2.0 });
    LossConjugateTest(functions::SmoothHingeLoss(), { 0.0, 0.1, 1.0 }, { -1.0, 0.1, -1.0 }, { -2, 0.1, 2.0 });

    RegularizerConjugateTest(functions::L2Regularizer(), 100, 100);
    RegularizerConjugateTest(functions::ElasticNetRegularizer(), 100, 100);

    ConjugateProxTest(functions::SquaredLoss(), 1.0,  { -1.0, 0.1, 1.0 }, { -1.0, 0.1, 1.0 }, { 0, 0.1, 1.0 });
    ConjugateProxTest(functions::LogLoss(), 1.0, { -1.0, 0.1, 1.0 }, { 1.0, 0.1, 1.0 }, { -1.0, 0.1, 0 });
    ConjugateProxTest(functions::LogLoss(), 1.0, { -1.0, 0.1, 1.0 }, { -1.0, 0.1, -1.0 }, { 0, 0.1, 1.0 });
    ConjugateProxTest(functions::SmoothHingeLoss(), 1.0, { -1.0, 0.1, 1.0 }, { -1.0, 1.0, -1.0 }, { 0.0, 0.1, 1.0 });
    ConjugateProxTest(functions::SmoothHingeLoss(), 1.0, { -1.0, 0.1, 1.0 }, { 1.0, 1.0, 1.0 }, { -1.0, 0.1, 0.0 });

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}
