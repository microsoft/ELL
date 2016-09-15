////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     common.i (interfaces)
//  Authors:  Chuck Jacobs, Piali Choudhury (pialic)
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%{
#include "DataLoadArguments.h"
#include "DataLoadersInterface.h"
#include "LoadModel.h"
%}

%ignore emll::common::GetDataset;

// %include "DataLoadArguments.h"
%include "LoadModel.h"

// Modified versions of interfaces
%include "DataLoadersInterface.h"

#if defined(SWIGJAVASCRIPT)
%include "loadModelAsync.i"
%include "loadDatasetAsync.i"
#endif
