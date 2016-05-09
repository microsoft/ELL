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
#include "IirFilterFeature.h"
#include "FeatureSet.h"
#include <sstream>
%}

%include "Feature.h"
%include "InputFeature.h"
%include "MagnitudeFeature.h"
%include "IirFilterFeature.h"
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
