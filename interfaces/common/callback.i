////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     callback.i (common)
//  Authors:  Piali Choudhury (pialic), Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include language specific SWIG definitions that must be declared before the
// C++ code to be wrapped
#ifdef SWIGPYTHON
    %include "callback_python_pre.i"
#elif SWIGJAVASCRIPT
    %include "callback_javascript_pre.i"
#endif // SWIGPYTHON

// stl
%include "integer.i"
%include "vector.i"

// ELL API
%{
#include <vector>
#include <stdint.h>
#include "CallbackInterface.h"
%}

%feature("director") ell::api::CallbackBase;

// C++ code to be wrapped
%include "CallbackInterface.h"

// Template instantiations
#ifndef SWIGJAVASCRIPT
%template(DoubleCallbackBase) ell::api::CallbackBase<double>;
%template(FloatCallbackBase) ell::api::CallbackBase<float>;
%template(IntCallbackBase) ell::api::CallbackBase<int>;
%template(Int64CallbackBase) ell::api::CallbackBase<int64_t>;
%template(Int8CallbackBase) ell::api::CallbackBase<int8_t>;
#endif

// Include language specific SWIG definitions that must be declared after the
// C++ code has been wrapped by SWIG
#ifdef SWIGPYTHON
    %include "callback_python_post.i"
#elif SWIGJAVASCRIPT
    %include "callback_javascript_post.i"
#endif // SWIGPYTHON

// Macros for wrapping language-specific callables so that they can act like callbacks
#if defined(SWIGPYTHON)

#else // defined(SWIGPYTHON)

%define WRAP_CALLABLES_AS_COMPILED_MAP_CALLBACKS(InputCallbackClass, OutputCallbackClass, ElementType)
%enddef

#endif // defined(SWIGPYTHON)
