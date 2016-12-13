////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     EMLL.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%module "EMLL"
%{
#include <vector>
%}

#ifndef SWIGXML
//%include typemaps.i
%include "std_string.i"
%include "std_vector.i"
%template(DoubleVector) std::vector<double>;
%template(DoubleVectorVector) std::vector<std::vector<double>>;
#endif 

%include "lossFunctions.i"
%include "model.i"
