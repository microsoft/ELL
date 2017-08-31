////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ELL.i (interfaces)
//  Authors:  Chuck Jacobs, Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include language specific SWIG definitions that must be declared before the
// C++ code to be wrapped
#ifdef SWIGPYTHON
    %include "ELL_python_pre.i"
#elif SWIGJAVASCRIPT
    %include "ELL_javascript_pre.i"
#endif

%module "ELL"
// Generate decent docstrings from types and method signatures
%feature("autodoc", "3");

#ifdef SWIGJAVASCRIPT
%{
#include "LLVMInclude.h" // LLVM headers must be included before Windows headers (which are included by node.h)

// stl
#include <node.h>
#include <v8.h>
#include <nan.h>
%}
#endif

%include "vector.i"

#ifndef SWIGXML
//%include typemaps.i
%include "unique_ptr.i"
%include "exception.i"
#endif
 
// Add ELL exception handling
%exception 
{
    try 
    {
	    $action
    } 
    catch(const ell::utilities::LogicException& e) 
    {
	    SWIG_exception(SWIG_RuntimeError, e.GetMessage().c_str());
    }
    catch(const ell::utilities::SystemException& e) 
    {
	    SWIG_exception(SWIG_SystemError, e.GetMessage().c_str());
    } 
    catch(const ell::utilities::NumericException& e) 
    {
	    SWIG_exception(SWIG_ValueError, e.GetMessage().c_str());
    } 
    catch(const ell::utilities::InputException& e) 
    {
	    SWIG_exception(SWIG_ValueError, e.GetMessage().c_str());
    }
    catch(const ell::utilities::Exception& e) 
    {
	    SWIG_exception(SWIG_RuntimeError, e.GetMessage().c_str());
    }
    catch(const std::exception& e)
    {
	    SWIG_exception(SWIG_RuntimeError, e.what());
    }
}

// ELL APIs 
%include "functions.i"
%include "math.i"
%include "predictors.i"
%include "model.i"

#if defined(SWIGJAVASCRIPT)
// Interface for NaN callbacks
%include callback.i

// Interface ELL Load Model Async
// %include loadModelAsync.i
#endif

// Include language specific SWIG definitions that must be declared after the
// C++ code has been wrapped by SWIG
#ifdef SWIGPYTHON
    %include "ELL_python_post.i"
#elif SWIGJAVASCRIPT
    %include "ELL_javascript_post.i"
#endif
