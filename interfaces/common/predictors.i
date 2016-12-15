////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Predictiors.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%ignore ell::predictors::LinearPredictor::GetWeights() const;
%ignore ell::predictors::LinearPredictor::GetBias() const;

%{
#define SWIG_FILE_WITH_INIT
#include "LinearPredictor.h"
%}

#if !defined(SWIGXML) && !defined(SWIGR) && !defined(SWIGJAVASCRIPT)
%shared_ptr(ell::predictors::LinearPredictor) 
#endif

%include "LinearPredictor.h"
