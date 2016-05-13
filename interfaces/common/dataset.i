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

%ignore dataset::operator<<;
%ignore dataset::RowDataset;
%ignore dataset::IDataVector::Clone;

%ignore dataset::GenericSupervisedExample::GetDataVector;
%ignore dataset::GenericSupervisedExample::GenericSupervisedExample;

%ignore dataset::DataRow<void>;
%ignore dataset::DenseDataVector::operator[];
%ignore dataset::DenseDataVector<double>;
%ignore dataset::DenseDataVector<float>;
%ignore dataset::SparseDataVector<double, utilities::CompressedIntegerList>;
%ignore dataset::SparseDataVector<float, utilities::CompressedIntegerList>;
%ignore dataset::SparseDataVector<short, utilities::CompressedIntegerList>;

%{
#define SWIG_FILE_WITH_INIT
#include "IVector.h"
#include "DenseDataVector.h"
#include "IDataVector.h"
#include "SparseDataVector.h"
#include "SupervisedExample.h"
#include "RowDatasetInterface.h"
%}

%include "IVector.h"
%include "DenseDataVector.h"
%include "IDataVector.h"
%include "SparseDataVector.h"
%include "SupervisedExample.h"
%include "RowDatasetInterface.h"

%include "unique_ptr.i"
wrap_unique_ptr(IDataVectorPtr, dataset::IDataVector)

%template (DoubleDataVector2) dataset::DenseDataVector<double>;
%template (FloatDataVector2) dataset::DenseDataVector<float>;
%template (SparseDoubleDataVector2) dataset::SparseDataVector<double, utilities::CompressedIntegerList>;
%template (SparseFloatDataVector2) dataset::SparseDataVector<float, utilities::CompressedIntegerList>;
%template (SparseShortDataVector2) dataset::SparseDataVector<short, utilities::CompressedIntegerList>;

// Bafflingly, the below causes SWIG to give an error about no default constructor for SparseDataVector<>
// %template (SparseDoubleDataVectorBase) dataset::SparseDataVector<double, utilities::CompressedIntegerList>;
// %template (SparseFloatDataVectorBase) dataset::SparseDataVector<float, utilities::CompressedIntegerList>;
// %template (SparseShortDataVectorBase) dataset::SparseDataVector<short, utilities::CompressedIntegerList>;

WRAP_OP_AT(dataset::DenseDataVector, double)

//WRAP_PRINT_TO_STR(dataset::GenericSupervisedExample)
WRAP_PRINT_TO_STR(dataset::SupervisedExample<dataset::IDataVector>)
WRAP_PRINT_TO_STR(dataset::FloatDataVector)
WRAP_PRINT_TO_STR(dataset::DoubleDataVector)

//WRAP_PRINT_TO_STR(dataset::SparseDoubleDataVector)
//WRAP_PRINT_TO_STR(dataset::SparseFloatDataVector)
//WRAP_PRINT_TO_STR(dataset::SparseShortDataVector)
