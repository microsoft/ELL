////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     dataset.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%module dataset

%{
    #include <stdexcept>
%}

%ignore dataset::RowDataset;
%ignore dataset::IDataVector::Clone;

%ignore dataset::SupervisedExample::GetDataVector;
%ignore dataset::SupervisedExample::SupervisedExample;

%ignore dataset::DoubleDataVector::GetType;
%ignore dataset::FloatDataVector::GetType;
%ignore dataset::SparseShortDataVector::GetType;
%ignore dataset::SparseFloatDataVector::GetType;
%ignore dataset::SparseDoubleDataVector::GetType;

%{
#define SWIG_FILE_WITH_INIT
#include "DenseDataVector.h"
#include "IDataVector.h"
#include "RowDataset.h"
#include "SparseDataVector.h"
#include "SupervisedExample.h"

#include "RowDatasetInterface.h"
%}

%include "DenseDataVector.h"
%include "IDataVector.h"
%include "SparseDataVector.h"
%include "SupervisedExample.h"

%include "RowDatasetInterface.h"

%include "unique_ptr.i"
wrap_unique_ptr(IDataVectorPtr, dataset::IDataVector)

%template (DoubleDataVector) dataset::DenseDataVector<double>;
%template (FloatDataVector) dataset::DenseDataVector<float>;

// Bafflingly, the below causes SWIG to give an error about no default constructor for SparseDataVector<>
// %template (SparseDoubleDataVectorBase) dataset::SparseDataVector<double, utilities::CompressedIntegerList>;
// %template (SparseFloatDataVectorBase) dataset::SparseDataVector<float, utilities::CompressedIntegerList>;
// %template (SparseShortDataVectorBase) dataset::SparseDataVector<short, utilities::CompressedIntegerList>;

WRAP_PRINT_TO_STR(dataset::SupervisedExample)
WRAP_PRINT_TO_STR(dataset::FloatDataVector)
WRAP_PRINT_TO_STR(dataset::DoubleDataVector)

//WRAP_PRINT_TO_STR(dataset::SparseDoubleDataVector)
//WRAP_PRINT_TO_STR(dataset::SparseFloatDataVector)
//WRAP_PRINT_TO_STR(dataset::SparseShortDataVector)

