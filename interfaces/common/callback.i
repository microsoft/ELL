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
#endif

// stl
%include "vector.i"

// ELL API
%{
#include "CallbackInterface.h"
%}

%feature("director") ell::api::common::CallbackBase;
%feature("nodirector") ell::api::common::CallbackForwarder;

%ignore ell::api::common::CallbackForwarder::Invoke(ElementType*);

// C++ code to be wrapped
%include "CallbackInterface.h"

// Template instantiations
%template(DoubleCallbackBase) ell::api::common::CallbackBase<double>;
%template(DoubleCallbackForwarder) ell::api::common::CallbackForwarder<double>;

// Include language specific SWIG definitions that must be declared after the
// C++ code has been wrapped by SWIG
#ifdef SWIGPYTHON
    %include "callback_python_post.i"
#elif SWIGJAVASCRIPT
    %include "callback_javascript_post.i"
#endif
