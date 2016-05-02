////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     optimization.i (interfaces)
//  Authors:  Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%module optimization

%{
#define SWIG_FILE_WITH_INIT
#include "AsgdOptimizer.h"
#include "LogLoss.h"
#include "HingeLoss.h"
#include "SquaredLoss.h"
%}

%include "AsgdOptimizer.h"

%template (LogLossOptimizer) optimization::AsgdOptimizer<lossFunctions::LogLoss>;
%template (HingeLossOptimizer) optimization::AsgdOptimizer<lossFunctions::HingeLoss>;
%template (SquaredLossOptimizer) optimization::AsgdOptimizer<lossFunctions::SquaredLoss>;
