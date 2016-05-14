////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     trainers.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%module trainers

%{
#define SWIG_FILE_WITH_INIT
#include "StochasticGradientDescentLearner.h"
#include "LogLoss.h"
#include "HingeLoss.h"
#include "SquaredLoss.h"
%}

%include "StochasticGradientDescentLearner.h"

%template (LogLossOptimizer) trainers::StochasticGradientDescentLearner<lossFunctions::LogLoss>;
%template (HingeLossOptimizer) trainers::StochasticGradientDescentLearner<lossFunctions::HingeLoss>;
%template (SquaredLossOptimizer) trainers::StochasticGradientDescentLearner<lossFunctions::SquaredLoss>;
