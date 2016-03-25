////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     main.cpp (lossFunctions_test)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "HingeLoss.h"
#include "LogLoss.h"
#include "SquaredLoss.h"

// testing
#include "testing.h"

// stl
#include <iostream>
#include<string>
#include <cmath>

void hingeLossTest()
{
    lossFunctions::HingeLoss hingeLoss;
    testing::ProcessTest("Testing lossFunctions::HingeLoss::Evaluate(2,1)", testing::IsEqual(hingeLoss.Evaluate(2, 1), 0));
    testing::ProcessTest("Testing lossFunctions::HingeLoss::Evaluate(-2,-1)", testing::IsEqual(hingeLoss.Evaluate(-2, -1), 0));
    testing::ProcessTest("Testing lossFunctions::HingeLoss::Evaluate(2,-1)", testing::IsEqual(hingeLoss.Evaluate(2, -1), 3));
    testing::ProcessTest("Testing lossFunctions::HingeLoss::Evaluate(-2,1)", testing::IsEqual(hingeLoss.Evaluate(-2, 1), 3));
    testing::ProcessTest("Testing lossFunctions::HingeLoss::Evaluate(0,1)", testing::IsEqual(hingeLoss.Evaluate(0, 1), 1));
    testing::ProcessTest("Testing lossFunctions::HingeLoss::Evaluate(0,-1)", testing::IsEqual(hingeLoss.Evaluate(0, -1), 1));

    testing::ProcessTest("Testing lossFunctions::HingeLoss::GetDerivative(2,1)", testing::IsEqual(hingeLoss.GetDerivative(2, 1), 0));
    testing::ProcessTest("Testing lossFunctions::HingeLoss::GetDerivative(-2,-1)", testing::IsEqual(hingeLoss.GetDerivative(-2, -1), 0));
    testing::ProcessTest("Testing lossFunctions::HingeLoss::GetDerivative(2,-1)", testing::IsEqual(hingeLoss.GetDerivative(2, -1), 1));
    testing::ProcessTest("Testing lossFunctions::HingeLoss::GetDerivative(-2,1)", testing::IsEqual(hingeLoss.GetDerivative(-2, 1), -1));
    testing::ProcessTest("Testing lossFunctions::HingeLoss::GetDerivative(0,1)", testing::IsEqual(hingeLoss.GetDerivative(0, 1), -1));
    testing::ProcessTest("Testing lossFunctions::HingeLoss::GetDerivative(0,-1)", testing::IsEqual(hingeLoss.GetDerivative(0, -1), 1));
}

void logLossTest()
{
    lossFunctions::LogLoss logLoss;
    testing::ProcessTest("Testing lossFunctions::LogLoss::Evaluate(2,1)", testing::IsEqual(logLoss.Evaluate(2, 1), std::log(1 + std::exp(-2))));
    testing::ProcessTest("Testing lossFunctions::LogLoss::Evaluate(-2,-1)", testing::IsEqual(logLoss.Evaluate(-2, -1), std::log(1 + std::exp(-2))));
    testing::ProcessTest("Testing lossFunctions::LogLoss::Evaluate(2,-1)", testing::IsEqual(logLoss.Evaluate(2, -1), std::log(1 + std::exp(2))));
    testing::ProcessTest("Testing lossFunctions::LogLoss::Evaluate(-2,1)", testing::IsEqual(logLoss.Evaluate(-2, 1), std::log(1 + std::exp(2))));
    testing::ProcessTest("Testing lossFunctions::LogLoss::Evaluate(0,1)", testing::IsEqual(logLoss.Evaluate(0, 1), std::log(2)));
    testing::ProcessTest("Testing lossFunctions::LogLoss::Evaluate(0,-1)", testing::IsEqual(logLoss.Evaluate(0, -1), std::log(2)));

    testing::ProcessTest("Testing lossFunctions::LogLoss::GetDerivative(2,1)", testing::IsEqual(logLoss.GetDerivative(2, 1), -1/(1+std::exp(2))));
    testing::ProcessTest("Testing lossFunctions::LogLoss::GetDerivative(-2,-1)", testing::IsEqual(logLoss.GetDerivative(-2, -1), 1/(1 + std::exp(2))));
    testing::ProcessTest("Testing lossFunctions::LogLoss::GetDerivative(2,-1)", testing::IsEqual(logLoss.GetDerivative(2, -1), 1/(1 + std::exp(-2))));
    testing::ProcessTest("Testing lossFunctions::LogLoss::GetDerivative(-2,1)", testing::IsEqual(logLoss.GetDerivative(-2, 1), -1/(1 + std::exp(-2))));
    testing::ProcessTest("Testing lossFunctions::LogLoss::GetDerivative(0,1)", testing::IsEqual(logLoss.GetDerivative(0, 1), -1/2.0));
    testing::ProcessTest("Testing lossFunctions::LogLoss::GetDerivative(0,-1)", testing::IsEqual(logLoss.GetDerivative(0, -1), 1/2.0));
}

void squaredLossTest()
{
    lossFunctions::SquaredLoss squaredLoss;
    testing::ProcessTest("Testing lossFunctions::SquaredLoss::Evaluate(4,4)", testing::IsEqual(squaredLoss.Evaluate(4, 4), 0));
    testing::ProcessTest("Testing lossFunctions::SquaredLoss::Evaluate(4,2)", testing::IsEqual(squaredLoss.Evaluate(4, 2), 2));
    testing::ProcessTest("Testing lossFunctions::SquaredLoss::Evaluate(2,4)", testing::IsEqual(squaredLoss.Evaluate(2, 4), 2));

    testing::ProcessTest("Testing lossFunctions::SquaredLoss::GetDerivative(4,4)", testing::IsEqual(squaredLoss.GetDerivative(4, 4), 0));
    testing::ProcessTest("Testing lossFunctions::SquaredLoss::GetDerivative(4,2)", testing::IsEqual(squaredLoss.GetDerivative(4, 2), 2));
    testing::ProcessTest("Testing lossFunctions::SquaredLoss::GetDerivative(2,4)", testing::IsEqual(squaredLoss.GetDerivative(2, 4), -2));
}

/// Runs all tests
///
int main()
{
    hingeLossTest();
    logLossTest();
    squaredLossTest();

    if(testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}




