////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     vector.i (interfaces)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%{
#ifdef SWIGPYTHON
  #define SWIG_FILE_WITH_INIT
  #define SWIG_PYTHON_EXTRA_NATIVE_CONTAINERS 
#endif

#include <vector>
%}

#ifndef SWIGXML
%include "std_string.i"
%include "std_vector.i"

%template(DoubleVector) std::vector<double>;
%template(DoubleVectorVector) std::vector<std::vector<double>>;
%template(FloatVector) std::vector<float>;
%template(FloatVectorVector) std::vector<std::vector<float>>;
%template(StringVector) std::vector<std::string>;

#endif


