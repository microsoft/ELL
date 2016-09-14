////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     trainers.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%{
#define SWIG_FILE_WITH_INIT
#include "ForestTrainerArguments.h"
#include "MultiEpochIncrementalTrainer.h"
#include "MultiEpochIncrementalTrainerArguments.h"
#include "SGDIncrementalTrainerArguments.h"
#include "TrainerArguments.h"
#include "MakeTrainer.h"
#include "IIncrementalTrainer.h"
#include "SGDIncrementalTrainer_wrap.h"

typedef emll::trainers::IIncrementalTrainer<emll::predictors::LinearPredictor> IncrementalLinearPredictorTrainer;
typedef emll::trainers::IIncrementalTrainer<emll::predictors::SimpleForestPredictor> IncrementalForestPredictorTrainer;
%}

%include "ForestTrainerArguments.h"
%include "MultiEpochIncrementalTrainer.h"
%include "MultiEpochIncrementalTrainerArguments.h"
%include "SGDIncrementalTrainerArguments.h"
%include "TrainerArguments.h"
%include "IIncrementalTrainer.h"

%template () emll::trainers::IIncrementalTrainer<emll::predictors::LinearPredictor>;
%template () emll::trainers::IIncrementalTrainer<emll::predictors::SimpleForestPredictor>;

typedef emll::trainers::IIncrementalTrainer<emll::predictors::LinearPredictor> IncrementalLinearPredictorTrainer;
typedef emll::trainers::IIncrementalTrainer<emll::predictors::SimpleForestPredictor> IncrementalForestPredictorTrainer;

%include "unique_ptr.i"
wrap_unique_ptr(LinearPredictorPtr, IncrementalLinearPredictorTrainer)
wrap_unique_ptr(SimpleForestPredictorPtr, IncrementalForestPredictorTrainer)

%include "MakeTrainer.h"

%include "SGDIncrementalTrainer_wrap.h"

%inline 
%{
    class LinearPredictorProxy
    {
    public:
        LinearPredictorProxy(const std::shared_ptr<const emll::predictors::LinearPredictor>& pred) : _pred(*pred) {}
        const emll::predictors::LinearPredictor& GetPred() { return _pred; }
    private:
        emll::predictors::LinearPredictor _pred;
    };
%}

%template (LogLossSGDTrainer) emll::trainers::SGDIncrementalTrainer<emll::lossFunctions::LogLoss>;
%template (HingeLossSGDTrainer) emll::trainers::SGDIncrementalTrainer<emll::lossFunctions::HingeLoss>;
%template (SquaredLossSGDTrainer) emll::trainers::SGDIncrementalTrainer<emll::lossFunctions::SquaredLoss>;

%extend emll::trainers::SGDIncrementalTrainer<emll::lossFunctions::LogLoss>
{
    LinearPredictorProxy GetPred() const { return LinearPredictorProxy(($self)->GetPredictor()); }
}
