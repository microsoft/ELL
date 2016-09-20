////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     EMLL.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////
 
%module "EMLL"

// IMPORTANT: need to include these before anything else, because they include windows.h or other platform-related things
#if defined(SWIGJAVASCRIPT)
%{
#include <node.h>
#include <v8.h>
#include <nan.h>
%}
#endif

// Common stuff
#ifdef SWIGPYTHON
#pragma SWIG nowarn=325,341,362,401,503
#endif

// Useful defines
%{
#ifdef SWIGPYTHON
  #define SWIG_FILE_WITH_INIT
  #define SWIG_PYTHON_EXTRA_NATIVE_CONTAINERS 
#endif
%}

// Include common STL libraries
%{
#include <cstdint>
#include <sstream>
#include <stdexcept>
%}

#ifndef SWIGXML
%include typemaps.i
%include "std_string.i"
%include "std_vector.i"
#endif 

#if !defined(SWIGJAVASCRIPT) && !defined(SWIGR) && !defined(SWIGXML)
%include "std_shared_ptr.i"
#endif

%include "exception.i" 
%include "unique_ptr.i"

#ifdef SWIGPYTHON
%include "std_iostream.i"  // Sadly, there is no std_iostream.i for C#
#endif

// Make trivial definitions for some things in std:: that SWIG chokes on, so we remember to call their constructors
namespace std 
{
    class default_random_engine {};
}

// Redefine uint64_t to long so we can use it in python
%typemap(in) uint64_t = unsigned long;
%apply unsigned long {uint64_t}

%typemap(in) int64_t = long;
%apply long {int64_t}

#ifndef SWIGXML
%template (DoubleStlVector) std::vector<double>;
%template () std::vector<float>;
%template (StringVector) std::vector<std::string>;
#endif

%{
#include "Exception.h"
%}

// Add some primitive exception handling
%exception {
    try { 
        $action 
    }
    catch(const emll::utilities::Exception& err) {
        std::string errorMessage = std::string("Got exception in EMLL library: ") + err.GetMessage();
        SWIG_exception(SWIG_RuntimeError, errorMessage.c_str());
    }    
    catch(const std::runtime_error& err) {
        SWIG_exception(SWIG_RuntimeError, const_cast<char*>(err.what()));        
    }    
    catch (...) {
        SWIG_exception(SWIG_RuntimeError, "Unknown exception in EMLL library");
    }
}

// Useful macros
%include "macros.i"

// Interface for NaN callbacks
%include callback.i

// Interface includes for lossFunctions library
%include lossFunctions.i

// Interface includes for utilities library
%include utilities.i

// Interface includes for linear library
%include linear.i

// Interface includes for dataset library
%include dataset.i

// Interface for the predictors library
%include predictors.i

// Interface includes for trainers library
%include trainers.i

// Interface for model library
%include model.i

// Interface for common library
%include common.i

// Interface for model library
%include nodes.i

#if !defined(SWIGXML) && !defined(SWIGR) && !defined(SWIGJAVASCRIPT)
%shared_ptr(RowDataset)
#endif
