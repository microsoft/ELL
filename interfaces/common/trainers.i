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
#include "SGDIncrementalTrainer_wrap.h"
%}

%include "SGDIncrementalTrainer_wrap.h"

%inline 
%{
    class LinearPredictorProxy
    {
    public:
        LinearPredictorProxy(const std::shared_ptr<const predictors::LinearPredictor>& pred) : _pred(*pred) {}
        const predictors::LinearPredictor& GetPred() { return _pred; }
    private:
        predictors::LinearPredictor _pred;
    };
%}

%template (LogLossSGDTrainer) trainers::SGDIncrementalTrainer<lossFunctions::LogLoss>;
%template (HingeLossSGDTrainer) trainers::SGDIncrementalTrainer<lossFunctions::HingeLoss>;
%template (SquaredLossSGDTrainer) trainers::SGDIncrementalTrainer<lossFunctions::SquaredLoss>;

%extend trainers::SGDIncrementalTrainer<lossFunctions::LogLoss>
{
    // TODO: return some other type of thing
    LinearPredictorProxy GetPred() const { return LinearPredictorProxy(($self)->GetPredictor()); }
}
