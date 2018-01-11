////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     shape.i (common)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include language specific SWIG definitions that must be declared before the
// C++ code to be wrapped

%inline %{

namespace ell {
namespace api {
namespace math {
    typedef struct TensorShape 
    {
        int rows;
        int columns;
        int channels;
        int Size() const { return rows * columns * channels; }
    } TensorShape;
}
}
}

%}
