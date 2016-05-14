////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     utilities.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%module utilities

%include "stl.i"

%ignore utilities::operator<<;
%ignore utilities::MakeAnyIterator;
%ignore utilities::IteratorBase;
%ignore utilities::IteratorWrapper;

%{
#define SWIG_FILE_WITH_INIT
#include "BinaryClassificationEvaluator.h"
#include "IIterator.h"
#include "AnyIterator.h"
#include "RandomEngines.h"
#include "StlIterator.h"
#include "StlIndexValueIterator.h"
#include "LogLoss.h"
#include "HingeLoss.h"
#include "SquaredLoss.h"
%}

%include "BinaryClassificationEvaluator.h"
%include "IIterator.h"
%include "AnyIterator.h"
%include "RandomEngines.h"

// This is necessary for us to avoid leaking memory:
%template (SupervisedExampleIterator) utilities::AnyIterator<dataset::SupervisedExample<dataset::IDataVector>>;

template <typename IteratorType, typename ValueType>
class StlIterator {};

%template (LinearLogLossClassificationEvaluator) utilities::BinaryClassificationEvaluator<predictors::LinearPredictor, lossFunctions::LogLoss>;
%template (LinearHingeLossClassificationEvaluator) utilities::BinaryClassificationEvaluator<predictors::LinearPredictor, lossFunctions::HingeLoss>;
%template (LinearSquaredLossClassificationEvaluator) utilities::BinaryClassificationEvaluator<predictors::LinearPredictor, lossFunctions::SquaredLoss>;
