////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     lossFunctions.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%{
#define SWIG_FILE_WITH_INIT
#include "HingeLoss.h"
#include "LogLoss.h"
#include "SquaredLoss.h"
#include "ELL_HingeLoss.h"
#include "ELL_LogLoss.h"
#include "ELL_SquaredLoss.h"
%}

// %include "HingeLoss.h"
// %include "LogLoss.h"
// %include "SquaredLoss.h"
%include "ELL_HingeLoss.h"
%include "ELL_LogLoss.h"
%include "ELL_SquaredLoss.h"
