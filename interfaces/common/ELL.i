////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ELL.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%module "ELL"

#ifdef SWIGJAVASCRIPT
%{
#include "LLVMInclude.h" // LLVM headers must be included before Windows headers (which are included by node.h)

// stl
#include <node.h>
#include <v8.h>
#include <nan.h>
%}
#endif

%{
#ifdef SWIGPYTHON
  #define SWIG_FILE_WITH_INIT
  #define SWIG_PYTHON_EXTRA_NATIVE_CONTAINERS 
#endif

#include <vector>
%}

#ifndef SWIGXML
//%include typemaps.i
%include "std_string.i"
%include "std_vector.i"

%template(DoubleVector) std::vector<double>;
%template(DoubleVectorVector) std::vector<std::vector<double>>;
%template(StringVector) std::vector<std::string>;
#endif

// ELL APIs 
%include "functions.i"
%include "model.i"

#if defined(SWIGJAVASCRIPT)
// Interface for NaN callbacks
%include callback.i

// Interface ELL Load Model Async
// %include loadModelAsync.i
#endif


