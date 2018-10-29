////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LossFunction_test.h (optimization_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

struct Range { double from, increment, to; };

/// <summary> Test that Loss::Derivative() is consistent with Loss::Value() </summary>
template<typename LossFunctionType>
void TestDerivative(LossFunctionType loss, Range predictionRange, Range outputRange);

/// <summary> Test that Loss::Conjugate() is consistent with Loss::Value() </summary>
template<typename LossFunctionType>
void TestConjugate(LossFunctionType loss, Range vRange, Range outputRange, Range comparatorRange);

/// <summary> Test Loss::ConjugateProx() is consistent with Loss::Conjugate() </summary>
template<typename LossFunctionType>
void TestConjugateProx(LossFunctionType loss, Range thetaRange, Range zRange, Range outputRange, Range comparatorRange);

#include "../tcc/LossFunction_test.tcc"