////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     trainers.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%{
#define SWIG_FILE_WITH_INIT
#include "LogLoss.h"
#include "HingeLoss.h"
#include "SquaredLoss.h"
#include "SGDIncrementalTrainer.h"
%}

%include "SGDIncrementalTrainer.h"

%inline 
%{
    class LinearPredictorProxy
    {
    public:
        LinearPredictorProxy(const std::shared_ptr<const emll::predictors::LinearPredictor>& pred) : _pred(*pred) {}
        const emll::predictors::LinearPredictor GetPred() { return _pred; }
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
