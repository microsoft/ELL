////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     layers.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%module "layers"

%ignore Model::GetTypeName;

%{
#define SWIG_FILE_WITH_INIT
#define SWIG_PYTHON_EXTRA_NATIVE_CONTAINERS 

#include "Coordinate.h"
#include "CoordinateList.h"
#include "CoordinateListTools.h"
#include "Layer.h"
#include "Model.h"
#include "Map.h"
#include "MapInterface.h"
#include "ModelInterface.h"
#include "LoadModel.h"
%}


%include "Coordinate.h"
%include "CoordinateList.h"
%include "CoordinateListTools.h"
%include "Layer.h"
// TODO: include the various layer types?
%include "MapInterface.h"
%include "ModelInterface.h"

// Add operator[] to CoordinateList objects
WRAP_OP_AT(layers::CoordinateList, layers::Coordinate);

// add __str__ to Coordinate and CoordinateList
%extend layers::Coordinate
{
    std::string __str__() 
    {        
        std::ostringstream oss(std::ostringstream::out);
        oss << "(" << ($self)->GetLayerIndex() << ", " << ($self)->GetElementIndex() << ")";
        return oss.str();
    }
};

WRAP_PRINT_TO_STR(layers::CoordinateList)
