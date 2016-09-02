////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     dataset.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%{
//#define SWIG_FILE_WITH_INIT
#include "IVector.h"
#include "DenseDataVector.h"
#include "IDataVector.h"
#include "SparseDataVector.h"
#include "Example.h"
#include "RowDatasetInterface.h"
%}

%ignore dataset::RowDataset::operator[];

namespace dataset
{
    %ignore IDataVector::Clone;
    %ignore GenericSupervisedExample::GenericSupervisedExample(GenericSupervisedExample&&);
    %ignore GenericSupervisedExample::GetDataVector;
    %ignore GenericSupervisedExample::GenericSupervisedExample;

    %ignore DenseDataVector::operator[];
    %ignore DenseDataVector<double>;
    %ignore DenseDataVector<float>;
    %ignore SparseDataVector<double, utilities::CompressedIntegerList>;
    %ignore SparseDataVector<float, utilities::CompressedIntegerList>;
    %ignore SparseDataVector<short, utilities::CompressedIntegerList>;
}

%ignore dataset::DenseSupervisedExample::DenseSupervisedExample(DenseSupervisedExample&&);
%ignore interfaces::GenericRowDataset::GenericRowDataset(GenericRowDataset &&);

%include "noncopyable.i"

%include "IVector.h"
%include "IDataVector.h"
%include "DenseDataVector.h"
%include "SparseDataVector.h"
%include "Example.h"
%include "RowDataset.h"

%include "RowDatasetInterface.h"
%import "RowDataset.h"

%include "unique_ptr.i"
wrap_unique_ptr(IDataVectorPtr, dataset::IDataVector)

%template() dataset::RowDataset<dataset::GenericSupervisedExample>;
%template() dataset::DenseDataVector<double>;

// The following template definitions are necessary to eliminate the "warning 315: Nothing known about ..." messages
%template () dataset::DenseDataVector<double>;
%template () dataset::DenseDataVector<float>;
%template () dataset::SparseDataVector<double, utilities::CompressedIntegerList>;
%template () dataset::SparseDataVector<float, utilities::CompressedIntegerList>;
%template () dataset::SparseDataVector<short, utilities::CompressedIntegerList>;
%template () dataset::RowDataset<GenericSupervisedExample>;

// wrap operator[] for python
WRAP_OP_AT(dataset::DoubleDataVector, double)

// wrap "Print" method for python
//    WRAP_PRINT_TO_STR(GenericSupervisedExample)
WRAP_PRINT_TO_STR(dataset::FloatDataVector)
WRAP_PRINT_TO_STR(dataset::DoubleDataVector)

WRAP_PRINT_TO_STR(dataset::SparseDoubleDataVector)
WRAP_PRINT_TO_STR(dataset::SparseFloatDataVector) 
WRAP_PRINT_TO_STR(dataset::SparseShortDataVector)
