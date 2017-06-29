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

%feature("director") ell::api::CallbackBase;
%feature("nodirector") ell::api::CallbackForwarder;

%ignore ell::api::CallbackForwarder::InvokeInput(InputType*);
%ignore ell::api::CallbackForwarder::InvokeOutput(OutputType*);

// C++ code to be wrapped
%include "CallbackInterface.h"

// Template instantiations
#ifndef SWIGJAVASCRIPT
%template(DoubleCallbackBase) ell::api::CallbackBase<double>;
%template(DoubleDoubleCallbackForwarder) ell::api::CallbackForwarder<double, double>;
%template(FloatCallbackBase) ell::api::CallbackBase<float>;
%template(FloatFloatCallbackForwarder) ell::api::CallbackForwarder<float, float>;
#endif

// Include language specific SWIG definitions that must be declared after the
// C++ code has been wrapped by SWIG
#ifdef SWIGPYTHON
    %include "callback_python_post.i"
#elif SWIGJAVASCRIPT
    %include "callback_javascript_post.i"
#endif

// Callback-specific macro for wrapping language-specific callables so that they can act like callbacks
#if defined(SWIGPYTHON)
%define WRAP_CALLABLES_AS_CALLBACKS(ForwarderClass, InputCallbackClass, InputType, OutputCallbackClass, OutputType)
    %pythonprepend ForwarderClass::GetInstance(ell::api::CallbackBase<InputType>&, std::vector<InputType>&, ell::api::CallbackBase<OutputType>&) %{
        class InputCallableWrapper(InputCallbackClass):
            def __init__(self, f):
                super(InputCallableWrapper, self).__init__()
                self.f_ = f
            def Run(self, data):
                return self.f_(data)

        class OutputCallableWrapper(OutputCallbackClass):
            def __init__(self, f):
                super(OutputCallableWrapper, self).__init__()
                self.f_ = f
            def Run(self, data):
                self.f_(data)
                return True

        if not isinstance(inputCallback, InputCallbackClass) and callable(inputCallback):
            wrapper = InputCallableWrapper(inputCallback)
            wrapper.__disown__() # caller to deref wrapper in its dtor
            inputCallback = wrapper
        else:
            inputCallback.__disown__() # caller to deref callback in its dtor

        if not isinstance(outputCallback, OutputCallbackClass) and callable(outputCallback):
            wrapper = OutputCallableWrapper(outputCallback)
            wrapper.__disown__() # caller to deref wrapper in its dtor
            outputCallback = wrapper
        else:
            outputCallback.__disown__() # caller to deref callback in its dtor
    %}
%enddef

#else

%define WRAP_CALLABLES_AS_CALLBACKS(ForwarderClass, InCallbackClass, InputType, OutCallbackClass, OutputType)
%enddef

#endif