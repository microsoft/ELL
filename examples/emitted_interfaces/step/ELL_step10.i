// This file will be generated

%module(directors="1") ELL_step10

// Generate docstrings from types and method signatures
%feature("autodoc", "3");

// Common SWIG code
%include "callback.i"
%include "macros.i"

// Model-specific SWIG code
%{
#include "ELL_step10.h"
%}

WRAP_CALLABLES_AS_CALLBACKS(Step10Predictor, DoubleCallbackBase, double)

%include "ELL_step10.h"
