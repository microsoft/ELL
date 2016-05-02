////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     EMLL.i (interfaces)
//  Authors:  Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%module "EMLL"

// Common stuff
#ifdef SWIGPYTHON
#pragma SWIG nowarn=325,341,362,401,503
#endif

%{
#define SWIG_FILE_WITH_INIT
#define SWIG_PYTHON_EXTRA_NATIVE_CONTAINERS
#include <cstdint>
%}

%include "exception.i" 
%include "std_string.i"
#ifdef SWIGPYTHON
%include "std_iostream.i"  // Sadly, there is no std_iostream.i for C#
#endif
%include "std_vector.i"
%include "std_shared_ptr.i"

%template (DoubleVec) std::vector<double>;

// Macro for exposing operator[] to python
%define WRAP_OP_AT(Class, ValueType)
  %extend Class 
  {
    const ValueType& __getitem__(size_t index)
    {
      return (*$self)[index];
    }
  };
%enddef

// Macro for turning Print(ostream) into __str__ 
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

// Make trivial definitions for some things in std:: that SWIG chokes on, so we remember to call their constructors
namespace std 
{
    class default_random_engine {};
}

%ignore utilities::operator<<;
%ignore dataset::operator<<;

%ignore dataset::DataRow<void>;
%ignore layers::Map::GetModel() const;
%ignore layers::Map::GetModel();
%ignore predictors::LinearPredictor::AddToModel(layers::Model&, const layers::CoordinateList& ) const;
%ignore layers::Model::AddLayer;
%ignore utilities::IteratorBase;

// Redefine uint64_t to long so we can use them in python
%typemap(in) uint64_t = unsigned long;
%apply unsigned long {uint64_t}

%typemap(in) int64_t = long;
%apply long {int64_t}

// Add some primitive exception handling
%exception {
    try { 
        $action 
    }
    catch(std::runtime_error err) {
        SWIG_exception(SWIG_RuntimeError, err.what());        
    }    
    catch (...) {
        SWIG_exception(SWIG_RuntimeError, "Exception in EMLL library");
    }
}

// Interface includes for layers library
%include layers.i

// Interface includes for dataset library
%include dataset.i

// Interface for common library
%include common.i

// Interface for the predictors library
%include predictors.i

// Interface includes for optimizer library
%include optimization.i

// Interface includes for lossFunctions library
%include lossFunctions.i

// Interface includes for utilities library
%include utilities.i

// Interface includes for linear library
%include linear.i

%include "unique_ptr.i"
wrap_unique_ptr(LayerPtr, layers::Layer)

%include "std_shared_ptr.i"
%shared_ptr(layers::Map)
%shared_ptr(layers::Model)
%shared_ptr(dataset::RowDataset)
%shared_ptr(RowDataset)

