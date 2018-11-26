////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     evaluators.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%{
//#define SWIG_FILE_WITH_INIT
#include <evaluators/include/AUCAggregator.h>
#include <evaluators/include/BinaryErrorAggregator.h>
#include <evaluators/include/Evaluator.h>
#include <evaluators/include/IncrementalEvaluator.h>
#include <evaluators/include/LossAggregator.h>
%}

// Wrap some things with SWIG

%include <evaluators/include/AUCAggregator.h>
%include <evaluators/include/BinaryErrorAggregator.h>
