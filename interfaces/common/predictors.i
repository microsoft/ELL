////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Predictiors.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%ignore emll::predictors::LinearPredictor::GetWeights() const;
%ignore emll::predictors::LinearPredictor::GetBias() const;

%{
#define SWIG_FILE_WITH_INIT
#include "LinearPredictor.h"
%}

#if !defined(SWIGXML) && !defined(SWIGJAVASCRIPT)
%shared_ptr(emll::predictors::LinearPredictor) 
#endif

%include "LinearPredictor.h"
