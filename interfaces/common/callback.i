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
%include "stdint.i"
%include "vector.i"

// ELL API
%{
#include "CallbackInterface.h"
%}

%feature("director") ell::api::CallbackBase;
%feature("nodirector") ell::api::CallbackForwarder;

%ignore ell::api::CallbackForwarder::InvokeInput(InputType*);
%ignore ell::api::CallbackForwarder::InvokeOutput(OutputType*);
%ignore ell::api::CallbackForwarder::InvokeOutput(OutputType);
%ignore ell::api::CallbackForwarder::InvokeLagNotification(TimeTickType);

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
#endif // SWIGPYTHON

// Macros for wrapping language-specific callables so that they can act like callbacks
#if defined(SWIGPYTHON)

// Macros for Map
%define WRAP_CALLABLE_AS_MAP_INPUT_CALLBACK(CallbackClass, ElementType)
    %pythonprepend ELL_API::Map::SetSourceCallback<ElementType>(ell::api::CallbackBase<ElementType>&, size_t) %{
        class InputCallableWrapper(CallbackClass):
            def __init__(self, f):
                super(InputCallableWrapper, self).__init__()
                self.f_ = f
            def Run(self, data):
                return self.f_(data)

        if not isinstance(callback, CallbackClass) and callable(callback):
            callback = InputCallableWrapper(callback)
        self.input_wrapper = callback # keep it alive
    %}
%enddef

%define WRAP_CALLABLE_AS_MAP_OUTPUT_CALLBACK(CallbackClass, ElementType)
    %pythonprepend ELL_API::Map::SetSinkCallback<ElementType>(ell::api::CallbackBase<ElementType>&, size_t) %{
        class OutputCallableWrapper(CallbackClass):
            def __init__(self, f):
                super(OutputCallableWrapper, self).__init__()
                self.f_ = f
            def Run(self, data):
                self.f_(data)
                return True

        if not isinstance(callback, CallbackClass) and callable(callback):
            callback = OutputCallableWrapper(callback)
        self.output_wrapper = callback # keep it alive
    %}
%enddef

// Macros for CompiledMap
%define WRAP_CALLABLES_AS_COMPILED_MAP_CALLBACKS(InputCallbackClass, OutputCallbackClass, ElementType)
    %pythonprepend ELL_API::CompiledMap::RegisterCallbacks<ElementType>(ell::api::CallbackBase<ElementType>&, ell::api::CallbackBase<ElementType>&) %{
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
            inputCallback = InputCallableWrapper(inputCallback)
        self.input_wrapper = inputCallback # keep it alive

        if not isinstance(outputCallback, OutputCallbackClass) and callable(outputCallback):
            outputCallback = OutputCallableWrapper(outputCallback)
        self.output_wrapper = outputCallback # keep it alive
    %}
%enddef

#else // defined(SWIGPYTHON)

%define WRAP_CALLABLES_AS_COMPILED_MAP_CALLBACKS(InputCallbackClass, OutputCallbackClass, ElementType)
%enddef

#endif // defined(SWIGPYTHON)
