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
#endif

// Macros for wrapping language-specific callables so that they can act like callbacks
#if defined(SWIGPYTHON)

// Macro for emitted code
%define WRAP_CALLABLES_AS_CALLBACKS(ForwarderClass, InputCallbackClass, InputType, OutputCallbackClass, OutputType, LagCallbackClass)
    %pythonprepend ForwarderClass::GetInstance(ell::api::CallbackBase<InputType>&, size_t, ell::api::CallbackBase<OutputType>&, size_t, ell::api::CallbackBase<double>&) %{
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

        class LagCallableWrapper(LagCallbackClass):
            def __init__(self, f):
                super(LagCallableWrapper, self).__init__()
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

        if not isinstance(lagCallback, LagCallbackClass) and callable(lagCallback):
            wrapper =  LagCallableWrapper(lagCallback)
            wrapper.__disown__() # caller to deref wrapper in its dtor
            lagCallback = wrapper
        else:
            lagCallback.__disown__() # caller to deref callback in its dtor

    %}
%enddef

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
            wrapper = InputCallableWrapper(callback)
            wrapper.__disown__() # caller to deref wrapper in its dtor
            callback = wrapper
        else:
            callback.__disown__() # caller to deref callback in its dtor
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
            wrapper = OutputCallableWrapper(callback)
            wrapper.__disown__() # caller to deref wrapper in its dtor
            callback = wrapper
        else:
            callback.__disown__() # caller to deref callback in its dtor
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

%define WRAP_CALLABLES_AS_CALLBACKS(ForwarderClass, Method, InputCallbackClass, InputType, OutputCallbackClass, OutputType, LagCallbackClass)
%enddef
%define WRAP_CALLABLE_AS_MAP_INPUT_CALLBACK(CallbackClass, ElementType)
%enddef
%define WRAP_CALLABLE_AS_MAP_OUTPUT_CALLBACK(CallbackClass, ElementType)
%enddef
%define WRAP_CALLABLES_AS_COMPILED_MAP_CALLBACKS(InputCallbackClass, OutputCallbackClass, ElementType)
%enddef

#endif // defined(SWIGPYTHON)