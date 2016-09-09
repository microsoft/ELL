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

%ignore emll::dataset::RowDataset::operator[];

namespace emll::dataset
{
    %ignore IDataVector::Clone;
    %ignore GenericSupervisedExample::GenericSupervisedExample(GenericSupervisedExample&&);
    %ignore GenericSupervisedExample::GetDataVector;
    %ignore GenericSupervisedExample::GenericSupervisedExample;

    %ignore DenseDataVector::operator[];
    %ignore DenseDataVector<double>;
    %ignore DenseDataVector<float>;
    %ignore SparseDataVector<double, emll::utilities::CompressedIntegerList>;
    %ignore SparseDataVector<float, emll::utilities::CompressedIntegerList>;
    %ignore SparseDataVector<short, emll::utilities::CompressedIntegerList>;
}

%ignore emll::dataset::DenseSupervisedExample::DenseSupervisedExample(DenseSupervisedExample&&);
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
wrap_unique_ptr(IDataVectorPtr, emll::dataset::IDataVector)

%template() emll::dataset::RowDataset<emll::dataset::GenericSupervisedExample>;
%template() emll::dataset::DenseDataVector<double>;

// The following template definitions are necessary to eliminate the "warning 315: Nothing known about ..." messages
%template () emll::dataset::DenseDataVector<double>;
%template () emll::dataset::DenseDataVector<float>;
%template () emll::dataset::SparseDataVector<double, emll::utilities::CompressedIntegerList>;
%template () emll::dataset::SparseDataVector<float, emll::utilities::CompressedIntegerList>;
%template () emll::dataset::SparseDataVector<short, emll::utilities::CompressedIntegerList>;
%template () emll::dataset::RowDataset<GenericSupervisedExample>;

// wrap operator[] for python
WRAP_OP_AT(emll::dataset::DoubleDataVector, double)

// wrap "Print" method for python
//    WRAP_PRINT_TO_STR(GenericSupervisedExample)
WRAP_PRINT_TO_STR(emll::dataset::FloatDataVector)
WRAP_PRINT_TO_STR(emll::dataset::DoubleDataVector)

WRAP_PRINT_TO_STR(emll::dataset::SparseDoubleDataVector)
WRAP_PRINT_TO_STR(emll::dataset::SparseFloatDataVector) 
WRAP_PRINT_TO_STR(emll::dataset::SparseShortDataVector)
