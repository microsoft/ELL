////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Math.i (interfaces)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////
%{
#include <stddef.h>
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

#ifdef SWIGPYTHON
// bugbug: this isn't working in Javascript.  It results in code that doesn't compile.
// filed as work item #1075 to investigate.
%template(DoubleArgmax) ell::api::math::Argmax<double>;
%template(FloatArgmax) ell::api::math::Argmax<float>;
#endif

// Include language specific SWIG definitions that must be declared after the
// C++ code has been wrapped by SWIG
#ifdef SWIGPYTHON
    %include "math_python_post.i"
#elif SWIGJAVASCRIPT
    %include "math_javascript_post.i"
#endif
