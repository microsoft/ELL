////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     utilities.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

//%module utilities

%include "stl.i"

%ignore utilities::operator<<;
%ignore utilities::MakeAnyIterator;
%ignore utilities::IteratorWrapper;

%{
#define SWIG_FILE_WITH_INIT
#include "BinaryClassificationEvaluator.h"
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

%include "BinaryClassificationEvaluator.h"
%include "AnyIterator.h"
%include "RandomEngines.h"
%include "RowDataset.h"

%include "SGDIncrementalTrainer.h"

// This is necessary for us to avoid leaking memory:
// %template (SupervisedExampleIterator) utilities::AnyIterator<dataset::SupervisedExample<dataset::IDataVector>>;

//%template () utilities::StlIterator<typename std::vector<dataset::SupervisedExample<dataset::IDataVector>>::const_iterator, dataset::SupervisedExample<dataset::IDataVector>>;

// TODO: need to make SWIG aware of utilities::IBinaryClassificationEvaluator<predictors::LinearPredictor>::ExampleIteratorType
// and that it's the same as dataset::GenericRowDataset::Iterator
// ... which is the same as utilities::VectorIterator<ExampleType>;
// ... which is the same as utilities::StlIterator<typename std::vector<ExampleType>::const_iterator>;

%include "LogLoss.h"
%include "HingeLoss.h"
%include "SquaredLoss.h"

%template () utilities::IBinaryClassificationEvaluator<predictors::LinearPredictor>;
%template (LinearLogLossClassificationEvaluator) utilities::BinaryClassificationEvaluator<predictors::LinearPredictor, lossFunctions::LogLoss>;
%template (LinearHingeLossClassificationEvaluator) utilities::BinaryClassificationEvaluator<predictors::LinearPredictor, lossFunctions::HingeLoss>;
%template (LinearSquaredLossClassificationEvaluator) utilities::BinaryClassificationEvaluator<predictors::LinearPredictor, lossFunctions::SquaredLoss>;

%template () trainers::SGDIncrementalTrainer<lossFunctions::SquaredLoss>;
typedef predictors::LinearPredictor trainers::SGDIncrementalTrainer<lossFunctions::SquaredLoss>::Predictor;

%ignore utilities::BinaryClassificationEvaluator<predictors::LinearPredictor, lossFunctions::LogLoss>::Evaluate;

%extend utilities::BinaryClassificationEvaluator<predictors::LinearPredictor, lossFunctions::LogLoss>
{
    void Eval(dataset::GenericRowIterator& dataIterator, std::shared_ptr<const predictors::LinearPredictor> predictor)
    {
        ($self)->Evaluate(dataIterator, *predictor);
    }
}

