////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     utilities.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%include "stl.i"

%ignore utilities::operator<<;
%ignore utilities::MakeAnyIterator;
%ignore utilities::IteratorWrapper;

%{
#define SWIG_FILE_WITH_INIT
#include "AnyIterator.h"
#include "RandomEngines.h"
#include "StlIterator.h"

#include "LogLoss.h"
#include "HingeLoss.h"
#include "SquaredLoss.h"
#include "LinearPredictor.h"
#include "SGDIncrementalTrainer.h"
%}

template <typename IteratorType, typename ValueType> class StlIterator {};


%include "AnyIterator.h"
%include "RandomEngines.h"
%include "RowDataset.h"

%include "SGDIncrementalTrainer_wrap.h"

// This is necessary for us to avoid leaking memory:
// %template (SupervisedExampleIterator) utilities::AnyIterator<dataset::SupervisedExample<dataset::IDataVector>>;

%template () dataset::SupervisedExample<dataset::IDataVector>;
%template () std::vector<dataset::SupervisedExample<dataset::IDataVector>>;
%template () utilities::StlIterator<typename std::vector<dataset::SupervisedExample<dataset::IDataVector>>::const_iterator, dataset::SupervisedExample<dataset::IDataVector>>;

%include "LogLoss.h"
%include "HingeLoss.h"
%include "SquaredLoss.h"

%template () trainers::SGDIncrementalTrainer<lossFunctions::LogLoss>;
%template () trainers::SGDIncrementalTrainer<lossFunctions::HingeLoss>;
%template () trainers::SGDIncrementalTrainer<lossFunctions::SquaredLoss>;

typedef predictors::LinearPredictor trainers::SGDIncrementalTrainer<lossFunctions::SquaredLoss>::Predictor;

// TODO: wrap print