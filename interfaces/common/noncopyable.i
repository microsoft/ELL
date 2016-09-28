//
// noncopyable.i -- wrapper macro to wrap an object with a move constructor but no copy constructor
//

%define wrap_noncopyable(type)
%feature("novaluewrapper") type;

%typemap(out) type 
%{
#if SWIGPYTHON
        $result = SWIG_NewPointerObj(new $1_ltype(std::move($1)), $1_descriptor, SWIG_POINTER_OWN);
#endif
#if SWIGJAVASCRIPT
        $result = SWIG_NewPointerObj(new $1_ltype(std::move($1)), $1_descriptor, SWIG_POINTER_OWN);
#endif
#if SWIGCSHARP
        $result = new $1_ltype(std::move($1));
#endif
%};
%enddef
