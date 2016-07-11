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
