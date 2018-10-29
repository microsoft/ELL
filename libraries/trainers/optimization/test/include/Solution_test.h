////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Solution_test.h (optimization_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

/// <summary> Tests that biased and unbiased VectorSolution and biased and unbiased MatrixSolution all behave identically when given equivalent SGD optimization problems. </summary>
template <typename RealType, typename LossFunctionType, typename RegularizerType>
void TestSolutionEquivalenceSGD(double regularizationParameter);

/// <summary> Tests that biased and unbiased VectorSolution and biased and unbiased MatrixSolution all behave identically when given equivalent SDCA optimization problems. </summary>
template <typename RealType, typename LossFunctionType, typename RegularizerType>
void TestSolutionEquivalenceSDCA(double regularizationParameter);

#include "../tcc/Solution_test.tcc"