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

%ignore common::GetDataset;

%include "DataLoadArguments.h"
%include "LoadModel.h"

// Modified versions of interfaces
%include "DataLoadersInterface.h"

%template (GenericSupervisedExampleIterator) utilities::AnyIterator<dataset::GenericSupervisedExample>;

#if defined(SWIGJAVASCRIPT)
%include "loadModelAsync.i"
%include "loadDatasetAsync.i"
#endif
