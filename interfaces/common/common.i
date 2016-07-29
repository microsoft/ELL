////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     common.i (interfaces)
//  Authors:  Chuck Jacobs, Piali Choudhury (pialic)
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%{
#include "DataLoadArguments.h"
#include "ModelLoadArguments.h"
#include "MapLoadArguments.h"
#include "DataLoadersInterface.h"
#include "LoadModelInterface.h"
#include "LoadModelGraph.h"
%}

%ignore common::GetDataset;

%include "DataLoadArguments.h"
%include "ModelLoadArguments.h"
%include "MapLoadArguments.h"
%include "LoadModelGraph.h"

// Modified versions of interfaces
%include "DataLoadersInterface.h"
%include "LoadModelInterface.h" 

#if defined(SWIGJAVASCRIPT)
%include "loadModelAsync.i"
%include "loadDatasetAsync.i"
#endif
