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
#include "StochasticGradientDescent.h"
#include "LogLoss.h"
#include "HingeLoss.h"
#include "SquaredLoss.h"
%}

%include "StochasticGradientDescent.h"

%template (LogLossOptimizer) trainers::StochasticGradientDescent<lossFunctions::LogLoss>;
%template (HingeLossOptimizer) trainers::StochasticGradientDescent<lossFunctions::HingeLoss>;
%template (SquaredLossOptimizer) trainers::StochasticGradientDescent<lossFunctions::SquaredLoss>;
