////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Predictiors.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%ignore predictors::LinearPredictor::GetWeights() const;
%ignore predictors::LinearPredictor::GetBias() const;
//%ignore predictors::DecisionTreePredictor;
%ignore predictors::DecisionTreePredictor::InteriorNode;

%{
#define SWIG_FILE_WITH_INIT
#include "LinearPredictor.h"
#include "DecisionTreePredictor.h"
#include "Coordinatewise.h"
#include "Sum.h"
%}


#if !defined(SWIGXML) && !defined(SWIGJAVASCRIPT)
%shared_ptr(predictors::LinearPredictor) 
#endif

%include "LinearPredictor.h"
%include "DecisionTreePredictor.h"
