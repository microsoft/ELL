////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LossFunction_test.tcc (optimization_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <testing/include/testing.h>

#include <trainers/optimization/include/GoldenSectionMinimizer.h>

#include <algorithm>
#include <cmath>

using namespace ell;
using namespace ell::trainers::optimization;

template <typename LossFunctionType>
double TestDerivative(LossFunctionType loss, double prediction, double output)
{
    const double epsilon = 1.0e-6;
    double lossPlus = loss.Value(prediction + epsilon, output);
    double lossMinus = loss.Value(prediction - epsilon, output);
    double difference = lossPlus - lossMinus;
    double limit = difference / (2 * epsilon);
    double derivative = loss.Derivative(prediction, output);
    double error = std::abs(derivative - limit);

    return error;
}

template <typename LossFunctionType>
void TestDerivative(LossFunctionType loss, Range predictionRange, Range outputRange)
{
    double errorTolerance = 1.0e-6;
    double maxError = 0;

    for (double prediction = predictionRange.from; prediction <= predictionRange.to; prediction += predictionRange.increment)
    {
        for (double output = outputRange.from; output <= outputRange.to; output += outputRange.increment)
        {
            maxError = std::max(maxError, TestDerivative(loss, prediction, output));
        }
    }

    std::string lossName = typeid(LossFunctionType).name();
    lossName = lossName.substr(lossName.find_last_of(":") + 1);

    testing::ProcessTest("TestDerivative <" + lossName + ">", maxError < errorTolerance);
}

template <typename LossFunctionType>
bool TestConjugate(LossFunctionType loss, double v, double output, double lower, double upper)
{
    const double tolerance = 1.0e-6;

    double conjugate = loss.Conjugate(v, output);
    if (std::isinf(conjugate))
    {
        return true;
    }

    auto objective = [&](double x) { return conjugate - x * v + loss.Value(x, output); };
    auto minimizer = GoldenSectionMinimizer(objective, lower, upper);
    minimizer.MinimizeToPrecision(tolerance);
    if (minimizer.GetMinUpperBound() < tolerance && minimizer.GetMinLowerBound() > -tolerance)
    {
        return true;
    }
    return false;
}

template <typename LossFunctionType>
void TestConjugate(LossFunctionType loss, Range vRange, Range outputRange, double lower, double upper)
{
    bool success = true;
    for (double v = vRange.from; v <= vRange.to; v += vRange.increment)
    {
        for (double output = outputRange.from; output <= outputRange.to; output += outputRange.increment)
        {
            if (!TestConjugate(loss, v, output, lower, upper))
            {
                success = false;
            }
        }
    }

    std::string lossName = typeid(LossFunctionType).name();
    lossName = lossName.substr(lossName.find_last_of(":") + 1);

    testing::ProcessTest("TestConjugate <" + lossName + ">", success);
}

template <typename LossFunctionType>
bool TestConjugateProx(LossFunctionType loss, double theta, double z, double output, double lower, double upper)
{
    const double tolerance = 1.0e-6;

    double conjugateProx = loss.ConjugateProx(theta, z, output);
    double conjugateProxValue = theta * loss.Conjugate(conjugateProx, output) + 0.5 * (conjugateProx - z) * (conjugateProx - z);
    auto objective = [&](double x) { return theta * loss.Conjugate(x, output) + 0.5 * (x - z) * (x - z) - conjugateProxValue; };

    auto minimizer = GoldenSectionMinimizer(objective, lower, upper);
    minimizer.MinimizeToPrecision(tolerance);
    if (minimizer.GetMinUpperBound() < tolerance && minimizer.GetMinLowerBound() > -tolerance)
    {
        return true;
    }
    return false;
}

template <typename LossFunctionType>
void TestConjugateProx(LossFunctionType loss, Range thetaRange, Range zRange, Range outputRange, double lower, double upper)
{
    bool success = true;
    for (double z = zRange.from; z <= zRange.to; z += zRange.increment)
    {
        for (double output = outputRange.from; output <= outputRange.to; output += outputRange.increment)
        {
            for (double theta = thetaRange.from; theta <= thetaRange.to; theta += thetaRange.increment)
            {
                if (!TestConjugateProx(loss, theta, z, output, lower, upper))
                {
                    success = false;
                }
            }
        }
    }

    std::string lossName = typeid(LossFunctionType).name();
    lossName = lossName.substr(lossName.find_last_of(":") + 1);

    testing::ProcessTest("TestConjugateProx <" + lossName + ">", success);
}
