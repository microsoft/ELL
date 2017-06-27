// Macros for exposing operator[] to python / javascript
#if defined(SWIGPYTHON)
%define WRAP_OP_AT(Class, ValueType)
  %extend Class 
  {
    ValueType __getitem__(size_t index)
    {
      return (*$self)[index];
    }
  };
%enddef

#elif defined(SWIGJAVASCRIPT)

%define WRAP_OP_AT(Class, ValueType)
  %extend Class 
  {
    ValueType get(size_t index)
    {
      return (*$self)[index];
    }
  };
%enddef

#else

%define WRAP_OP_AT(Class, ValueType)
%enddef

#endif

// Macro for exposing Print(ostream) into __str__ / toString in python / javascript
#if defined(SWIGPYTHON)
%define WRAP_PRINT_TO_STR(Class)
    %extend Class
    {
        std::string __str__() 
        {        
            std::ostringstream oss(std::ostringstream::out);
            ($self)->Print(oss);
            return oss.str();
        }
    };
%enddef

#elif defined(SWIGJAVASCRIPT)

%define WRAP_PRINT_TO_STR(Class)
    %extend Class
    {
        std::string toString() 
        {        
            std::ostringstream oss(std::ostringstream::out);
            ($self)->Print(oss);
            return oss.str();
        }
    };
%enddef

#else

%define WRAP_PRINT_TO_STR(Class)
%enddef

#endif

// Macro for exposing ostream.operator<<() into __str__ / toString in python / javascript
#if defined(SWIGPYTHON)
%define WRAP_OSTREAM_OUT_TO_STR(Class)
    %extend Class
    {
        std::string __str__() 
        {
            std::ostringstream oss(std::ostringstream::out);
            oss << *($self);
            return oss.str();
        }
    };
%enddef

#elif defined(SWIGJAVASCRIPT)

%define WRAP_OSTREAM_OUT_TO_STR(Class)
    %extend Class
    {
        std::string toString() 
        {        
            std::ostringstream oss(std::ostringstream::out);
            oss << *($self);
            return oss.str();
        }
    };
%enddef

#else

%define WRAP_OSTREAM_OUT_TO_STR(Class)
%enddef

#endif

// Macro for enabling types to be constructed from numpy arrays
#if defined(SWIGPYTHON)
%define CONSTRUCTABLE_WITH_NUMPY(TypeName)
%pythoncode %{
    class TypeName(TypeName):
        def __init__(self, numpyArray):
            # str(type) avoids requiring import numpy (more robust check?)
            if ('numpy.ndarray' in str(type(numpyArray))):
                if (len(numpyArray.shape) == 1):
                    super(TypeName, self).__init__(numpyArray)
                elif (len(numpyArray.shape) == 3):
                    super(TypeName, self).__init__(numpyArray.ravel(), numpyArray.shape[0], numpyArray.shape[1], numpyArray.shape[2])
                elif (len(numpyArray.shape) == 4):
                    # Create a stacked 3 dimensional tensor 
                    numpyArrayStacked = numpyArray.reshape(numpyArray.shape[0] * numpyArray.shape[1], numpyArray.shape[2], numpyArray.shape[3])
                    super(TypeName, self).__init__(numpyArrayStacked.ravel(), numpyArrayStacked.shape[0], numpyArrayStacked.shape[1], numpyArrayStacked.shape[2])
                else:
                    raise ValueError('Invalid number of dimensions!')
            else:
                # fallback to non-numpy construction
                super(TypeName, self).__init__(numpyArray)
%}
%enddef

#else

// Intentionally undefined because numpy is python-specific

#endif

