////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     features.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////


%module "features"

%{
#define SWIG_FILE_WITH_INIT
#define SWIG_PYTHON_EXTRA_NATIVE_CONTAINERS 
#include "DoubleVector.h"
#include "InputFeature.h"
#include "MagnitudeFeature.h"
#include "FeatureSet.h"
#include <sstream>
%}

%ignore Create;
%ignore Allocate;
%ignore Deserialize;

%include "Feature.h"
%include "InputFeature.h"
%include "MagnitudeFeature.h"
%include "FeatureSet.h"

// add __str__ to Coordinate and CoordinateList
%extend features::FeatureSet
{
    std::string __str__() 
    {        
        std::ostringstream oss(std::ostringstream::out);
        ($self)->Serialize(oss);
        return oss.str();
    }
};

%extend features::Feature
{
    std::string __str__() 
    {        
        std::ostringstream oss(std::ostringstream::out);
        auto description = ($self)->GetDescription();
        for(const auto& part: description)
        {
            oss << part << '\t';
        }
        return std::string(oss.str().begin(), oss.str().end()-1);
    }
};

%inline
%{
    void InitializeFeatures()
    {
        features::InputFeature::RegisterFeature();
        features::MagnitudeFeature::RegisterFeature();
    }
%}

#ifdef SWIGPYTHON
%init
%{
    InitializeFeatures();
%}
#endif

%include "unique_ptr.i"
wrap_unique_ptr(FeaturePtr, features::Feature)
wrap_unique_ptr(InputFeaturePtr, features::InputFeature)
wrap_unique_ptr(MagnitudeFeaturePtr, features::MagnitudeFeature)

//%shared_ptr(features::Feature)
//%shared_ptr(features::RegisteredFeature)
//%shared_ptr(features::InputFeature)
