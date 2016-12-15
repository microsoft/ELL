////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     unique_ptr.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

//
// Wrapper for unique_ptr that returns via std::move
//

namespace std
{
    %feature("novaluewrapper") unique_ptr; 

    // Skeleton of unique_ptr definition
    template< class T>
        class unique_ptr {
    public:
        typedef T element_type;
        typedef T* pointer;
        
        unique_ptr();
        unique_ptr( std::nullptr_t Nptr );
        explicit unique_ptr( pointer Ptr );
        unique_ptr (unique_ptr&& Right);
        template<class T2, Class Del2> unique_ptr( unique_ptr<T2, Del2>&& Right );
        ~unique_ptr();
        
		pointer get () const;
        pointer release();
        void reset(pointer ptr = pointer());
        void swap(unique_ptr& _Right);        
    };
}

%define wrap_unique_ptr(name, type)
  %template (name) std::unique_ptr<type>;
  %typemap(out) std::unique_ptr<type>
  %{
#if SWIGPYTHON
	$result = SWIG_NewPointerObj(new $1_ltype(std::move($1)), $1_descriptor, SWIG_POINTER_OWN);
#endif
#if SWIGCSHARP
    $result = new $1_ltype(std::move($1));
#endif
#if SWIGJAVASCRIPT
	$result = SWIG_NewPointerObj(new $1_ltype(std::move($1)), $1_descriptor, SWIG_POINTER_OWN);
#endif
  %};
%enddef
