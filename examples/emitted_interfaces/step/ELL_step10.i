//
// ELL SWIG interface for module ELL_step10
//

%module(directors="1") ELL_step10
%feature("autodoc", "3");

%include "callback.i"
#ifdef SWIGPYTHON
%rename("%(undercase)s", %$isfunction) "";
#endif

%{
#include "ELL_step10.i.h"
%}

WRAP_CALLABLES_AS_CALLBACKS(ELL_step10Predictor, DoubleCallbackBase, double, DoubleCallbackBase, double)

%include "ELL_step10.i.h"
