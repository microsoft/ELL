////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Math.i (interfaces)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%{
#include "MathInterface.h"
%}

// Include language specific SWIG definitions that must be declared before the
// C++ code to be wrapped
#ifdef SWIGPYTHON
    %include "math_python_pre.i"
#elif SWIGJAVASCRIPT
    %include "math_javascript_pre.i"
#endif

// Include the C++ code to be wrapped
%include "MathInterface.h"

// Template instaniations
%template(FloatTensor) ell::api::math::Tensor<float>;
%template(DoubleTensor) ell::api::math::Tensor<double>;

// Include language specific SWIG definitions that must be declared after the
// C++ code has been wrapped by SWIG
#ifdef SWIGPYTHON
    %include "math_python_post.i"
#elif SWIGJAVASCRIPT
    %include "math_javascript_post.i"
#endif
