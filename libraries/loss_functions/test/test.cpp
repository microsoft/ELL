// test.cpp

#include "HingeLoss.h"
using loss_functions::HingeLoss;

#include "LogLoss.h"
using loss_functions::LogLoss;

#include "SquaredLoss.h"
using loss_functions::SquaredLoss;

// testing
#include "testing.h"
using testing::isEqual;
using testing::processTest;
using testing::testFailed;

// stl
#include <iostream>
using std::cout;
using std::endl;

#include<string>
using std::string;

#include <cmath>
using std::log;
using std::exp;

void hingeLossTest()
{
    HingeLoss hingeLoss;
    processTest("Testing HingeLoss::Evaluate(2,1)", isEqual(hingeLoss.Evaluate(2, 1), 0));
    processTest("Testing HingeLoss::Evaluate(-2,-1)", isEqual(hingeLoss.Evaluate(-2, -1), 0));
    processTest("Testing HingeLoss::Evaluate(2,-1)", isEqual(hingeLoss.Evaluate(2, -1), 3));
    processTest("Testing HingeLoss::Evaluate(-2,1)", isEqual(hingeLoss.Evaluate(-2, 1), 3));
    processTest("Testing HingeLoss::Evaluate(0,1)", isEqual(hingeLoss.Evaluate(0, 1), 1));
    processTest("Testing HingeLoss::Evaluate(0,-1)", isEqual(hingeLoss.Evaluate(0, -1), 1));

    processTest("Testing HingeLoss::GetDerivative(2,1)", isEqual(hingeLoss.GetDerivative(2, 1), 0));
    processTest("Testing HingeLoss::GetDerivative(-2,-1)", isEqual(hingeLoss.GetDerivative(-2, -1), 0));
    processTest("Testing HingeLoss::GetDerivative(2,-1)", isEqual(hingeLoss.GetDerivative(2, -1), 1));
    processTest("Testing HingeLoss::GetDerivative(-2,1)", isEqual(hingeLoss.GetDerivative(-2, 1), -1));
    processTest("Testing HingeLoss::GetDerivative(0,1)", isEqual(hingeLoss.GetDerivative(0, 1), -1));
    processTest("Testing HingeLoss::GetDerivative(0,-1)", isEqual(hingeLoss.GetDerivative(0, -1), 1));
}

void logLossTest()
{
    LogLoss logLoss;
    processTest("Testing LogLoss::Evaluate(2,1)", isEqual(logLoss.Evaluate(2, 1), log(1 + exp(-2))));
    processTest("Testing LogLoss::Evaluate(-2,-1)", isEqual(logLoss.Evaluate(-2, -1), log(1 + exp(-2))));
    processTest("Testing LogLoss::Evaluate(2,-1)", isEqual(logLoss.Evaluate(2, -1), log(1 + exp(2))));
    processTest("Testing LogLoss::Evaluate(-2,1)", isEqual(logLoss.Evaluate(-2, 1), log(1 + exp(2))));
    processTest("Testing LogLoss::Evaluate(0,1)", isEqual(logLoss.Evaluate(0, 1), log(2)));
    processTest("Testing LogLoss::Evaluate(0,-1)", isEqual(logLoss.Evaluate(0, -1), log(2)));

    processTest("Testing LogLoss::GetDerivative(2,1)", isEqual(logLoss.GetDerivative(2, 1), -1/(1+exp(2))));
    processTest("Testing LogLoss::GetDerivative(-2,-1)", isEqual(logLoss.GetDerivative(-2, -1), 1/(1 + exp(2))));
    processTest("Testing LogLoss::GetDerivative(2,-1)", isEqual(logLoss.GetDerivative(2, -1), 1/(1 + exp(-2))));
    processTest("Testing LogLoss::GetDerivative(-2,1)", isEqual(logLoss.GetDerivative(-2, 1), -1/(1 + exp(-2))));
    processTest("Testing LogLoss::GetDerivative(0,1)", isEqual(logLoss.GetDerivative(0, 1), -1/2.0));
    processTest("Testing LogLoss::GetDerivative(0,-1)", isEqual(logLoss.GetDerivative(0, -1), 1/2.0));
}

void squaredLossTest()
{
    SquaredLoss squaredLoss;
    processTest("Testing SquaredLoss::Evaluate(4,4)", isEqual(squaredLoss.Evaluate(4, 4), 0));
    processTest("Testing SquaredLoss::Evaluate(4,2)", isEqual(squaredLoss.Evaluate(4, 2), 2));
    processTest("Testing SquaredLoss::Evaluate(2,4)", isEqual(squaredLoss.Evaluate(2, 4), 2));

    processTest("Testing SquaredLoss::GetDerivative(4,4)", isEqual(squaredLoss.GetDerivative(4, 4), 0));
    processTest("Testing SquaredLoss::GetDerivative(4,2)", isEqual(squaredLoss.GetDerivative(4, 2), 2));
    processTest("Testing SquaredLoss::GetDerivative(2,4)", isEqual(squaredLoss.GetDerivative(2, 4), -2));
}

/// Runs all tests
///
int main()
{
    hingeLossTest();
    logLossTest();
    squaredLossTest();

    if(testFailed())
    {
        return 1;
    }

    return 0;
}




