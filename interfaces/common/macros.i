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
%insert("python") %{
import numpy as np
%}

%define TYPEMAP_COPY_TO_VECTOR(BUFFER_TYPE)
%typemap(in) (BUFFER_TYPE* buffer, size_t length)
             (Py_buffer view_ = {})
{
    static const char* data_type = "BUFFER_TYPE";
    int res = PyObject_GetBuffer($input, &view_, PyBUF_ANY_CONTIGUOUS | PyBUF_WRITABLE | PyBUF_FORMAT);
    if (res < 0)
    {
        PyErr_Clear();
        SWIG_exception_fail(res, "Cannot get a contiguous buffer to write to");
    }
    if (view_.ndim != 1)
    {
        PyErr_Clear();
        SWIG_exception_fail(res, "Expected a 1-dimensional array");
    }

    if (view_.format == nullptr || view_.format[0] != data_type[0])
    {
        PyErr_Clear();
        SWIG_exception_fail(res, "Expected an array of BUFFER_TYPE");
    }
    $1 = ($1_ltype) view_.buf;
    $2 = ($2_ltype) view_.shape[0];
}
%typemap(freearg) (BUFFER_TYPE* buffer, size_t length)
{
    // free the buffer
    PyBuffer_Release(&view_2);
}
%enddef

%{

template<typename VectorType>
void copy_to_buffer(
    const VectorType& field,
    typename VectorType::value_type* buffer,
    typename VectorType::size_type length)
{
    if (length == field.size())
    {
        std::copy(field.begin(), field.end(), buffer);
    }
    else
    {
        throw std::invalid_argument("array sizes don't match");
    }
}

template<typename VectorType>
void copy_from_buffer(
    VectorType& field,
    typename VectorType::value_type* buffer,
    typename VectorType::size_type length)
{
    field.resize(length);
    for (size_t i = 0; i < length; i++)
    {
        field[i] = buffer[i];
    }
}

%}

%define TYPEMAP_VECTOR_TO_ARRAY(ELEMENT_TYPE)

TYPEMAP_COPY_TO_VECTOR(ELEMENT_TYPE)

void copy_to_buffer_ ## ELEMENT_TYPE(const std::vector<ELEMENT_TYPE>& field, ELEMENT_TYPE* buffer, size_t length);
void copy_from_buffer_ ## ELEMENT_TYPE(std::vector<ELEMENT_TYPE>& field, ELEMENT_TYPE* buffer, size_t length);
%{
void copy_to_buffer_## ELEMENT_TYPE(const std::vector<ELEMENT_TYPE>& field, ELEMENT_TYPE* buffer, size_t length)
{
    copy_to_buffer<std::vector<ELEMENT_TYPE>>(field, buffer, length);
}
void copy_from_buffer_## ELEMENT_TYPE(std::vector<ELEMENT_TYPE>& field, ELEMENT_TYPE* buffer, size_t length)
{
    copy_from_buffer<std::vector<ELEMENT_TYPE>>(field, buffer, length);
}
%}

%extend std::vector<ELEMENT_TYPE> {
%insert("python") %{
def __array__(self):
    """Enable access to this data as a numpy array, via np.asarray(v) """
    type_name = "ELEMENT_TYPE"
    s = len(self)
    if type_name == "float":
        a = np.ndarray(shape=(s,), dtype=np.float32)
    elif type_name == "double":
        a = np.ndarray(shape=(s,), dtype=np.float)
    else:
        raise Exception("Expecting float or double type")
    copy_to_buffer_## ELEMENT_TYPE(self, a)
    return a

def copy_from(self, a):
    """Enable buffer copy from given numpy array """
    copy_from_buffer_## ELEMENT_TYPE(self, a)
%}
}
%enddef

%define CONSTRUCT_VECTOR_WITH_NUMPY(TypeName, nptype)
%pythoncode %{
    class TypeName(TypeName):
        def __init__(self, numpyArray = None):
            # str(type) avoids requiring import numpy (more robust check?)
            if ('numpy.ndarray' in str(type(numpyArray))):
                # since this is a vector, we can always ravel it.
                super(TypeName, self).__init__()
                self.copy_from(numpyArray.ravel().astype(nptype))
            elif numpyArray:
                # fallback to non-numpy construction
                super(TypeName, self).__init__(numpyArray)
            else:
                super(TypeName, self).__init__()

%}

%enddef

%define CONSTRUCT_TENSOR_WITH_NUMPY(TypeName)
%pythoncode %{
    class TypeName(TypeName):
        def __init__(self, numpyArray = None):
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
            elif numpyArray:
                # fallback to non-numpy construction
                super(TypeName, self).__init__(numpyArray)
            else:
                super(TypeName, self).__init__()
%}

%enddef

#else

// Intentionally undefined because numpy is python-specific

#endif

