////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Optimizer_test.h (optimization_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <trainers/optimization/include/SDCAOptimizer.h>

/// <summary> Tests that the SDCA duality gap tends to zero in a regression setting after a sufficient number of epochs.</summary>
template <typename LossFunctionType, typename RegularizerType>
void TestSDCARegressionConvergence(LossFunctionType lossFunction, RegularizerType regularizer, SDCAOptimizerParameters parameters, double biasVariance, double inputVariance, double outputVariance);

/// <summary> Tests that the SDCA duality gap tends to zero in a classification setting after a sufficient number of epochs.</summary>
template <typename LossFunctionType, typename RegularizerType>
void TestSDCAClassificationConvergence(LossFunctionType lossFunction, RegularizerType regularizer, SDCAOptimizerParameters parameters, double biasVariance, double marginMean, double inputVariance);

#include "../tcc/Optimizer_test.tcc"
