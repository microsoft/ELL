////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     unique_ptr.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

//
// Wrapper for unique_ptr that returns via std::move
//

namespace std
{
    %feature("novaluewrapper") unique_ptr; // TODO: check if we really need this or not

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
        
        pointer release();
        void reset(pointer ptr = pointer());
        void swap(unique_ptr& _Right);        
    };
}

%define wrap_unique_ptr(name, type)
  %template (name) std::unique_ptr<type>;
  %typemap(out) unique_ptr<type> 
  %{
      $result = std::move($1);
  %};
%enddef