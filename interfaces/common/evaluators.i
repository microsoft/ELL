////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     evaluators.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%{
//#define SWIG_FILE_WITH_INIT
#include "AUCAggregator.h"
#include "BinaryErrorAggregator.h"
#include "Evaluator.h"
#include "IncrementalEvaluator.h"
#include "LossAggregator.h"
%}

// Wrap some things with SWIG

%include "AUCAggregator.h"
%include "BinaryErrorAggregator.h"
