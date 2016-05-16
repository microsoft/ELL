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
#include "SGDIncrementalTrainer.h"
#include "LogLoss.h"
#include "HingeLoss.h"
#include "SquaredLoss.h"
%}

%include "SGDIncrementalTrainer.h"

%template (LogLossOptimizer) trainers::SGDIncrementalTrainer<lossFunctions::LogLoss>;
%template (HingeLossOptimizer) trainers::SGDIncrementalTrainer<lossFunctions::HingeLoss>;
%template (SquaredLossOptimizer) trainers::SGDIncrementalTrainer<lossFunctions::SquaredLoss>;
