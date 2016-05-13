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
#include "StochasticGradientDescentTrainer.h"
#include "LogLoss.h"
#include "HingeLoss.h"
#include "SquaredLoss.h"
%}

%include "StochasticGradientDescentTrainer.h"

%template (LogLossOptimizer) trainers::StochasticGradientDescentTrainer<lossFunctions::LogLoss>;
%template (HingeLossOptimizer) trainers::StochasticGradientDescentTrainer<lossFunctions::HingeLoss>;
%template (SquaredLossOptimizer) trainers::StochasticGradientDescentTrainer<lossFunctions::SquaredLoss>;
