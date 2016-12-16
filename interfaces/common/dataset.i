////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
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
#include "RowDataset.h"
#include "StlIterator.h"
%}

// ignores
%ignore ell::dataset::RowDataset::operator[];
%ignore ell::dataset::IDataVector::Clone;
%ignore ell::dataset::GenericRowDataset::GenericRowDataset(ell::dataset::GenericRowDataset&&);
%ignore ell::dataset::RowDataset<ell::dataset::GenericSupervisedExample>::RowDataset(ell::dataset::RowDataset<ell::dataset::GenericSupervisedExample>&&);
%ignore ell::dataset::GenericSupervisedExample::GenericSupervisedExample;
%ignore ell::dataset::GenericSupervisedExample::GenericSupervisedExample(ell::dataset::GenericSupervisedExample&&);
%ignore ell::dataset::DenseSupervisedExample::DenseSupervisedExample(ell::dataset::DenseSupervisedExample&&);
%ignore ell::dataset::RowDataset< ell::dataset::GenericSupervisedExample >::RowDataset(ell::dataset::RowDataset< ell::dataset::GenericSupervisedExample > &&);
%ignore ell::dataset::Example< ell::dataset::IDataVector,ell::dataset::WeightLabel >::Example(ell::dataset::Example< ell::dataset::IDataVector,ell::dataset::WeightLabel > &&);
%ignore ell::dataset::SparseDataVector<double, ell::utilities::CompressedIntegerList>;
%ignore ell::dataset::SparseDataVector<float, ell::utilities::CompressedIntegerList>;
%ignore ell::dataset::SparseDataVector<short, ell::utilities::CompressedIntegerList>;
%ignore ell::dataset::Example<ell::dataset::IDataVector, ell::dataset::WeightLabel>::Example;

%include "noncopyable.i"
%include "unique_ptr.i"

// linear stuff (should already be included)
%include "IVector.h"

// utilities stuff (should already be included)
%include "CompressedIntegerList.h"

// dataset
%include "IDataVector.h"
%include "DenseDataVector.h"
%include "Example.h"
%include "RowDataset.h"
%include "SparseDataVector.h"

wrap_unique_ptr(IDataVectorPtr, ell::dataset::IDataVector)

%template (GenericSupervisedExample) ell::dataset::Example<ell::dataset::IDataVector, ell::dataset::WeightLabel>;
%template (GenericRowDataset) ell::dataset::RowDataset<ell::dataset::GenericSupervisedExample>;
%template (GenericRowIterator) ell::utilities::StlIterator<typename std::vector<ell::dataset::Example<ell::dataset::IDataVector, ell::dataset::WeightLabel>>::const_iterator, ell::dataset::Example<ell::dataset::IDataVector, ell::dataset::WeightLabel>>;

// The following template definitions are necessary to eliminate the "warning 315: Nothing known about ..." messages
%template () ell::dataset::DenseDataVector<double>;
%template () ell::dataset::DenseDataVector<float>;
%template () ell::dataset::SparseDataVector<double, ell::utilities::CompressedIntegerList>;
%template () ell::dataset::SparseDataVector<float, ell::utilities::CompressedIntegerList>;
%template () ell::dataset::SparseDataVector<short, ell::utilities::CompressedIntegerList>;

// wrap operator[] for python
WRAP_OP_AT(ell::dataset::DoubleDataVector, double)

// wrap "Print" method for python
//    WRAP_PRINT_TO_STR(GenericSupervisedExample)
WRAP_PRINT_TO_STR(ell::dataset::FloatDataVector)
WRAP_PRINT_TO_STR(ell::dataset::DoubleDataVector)

WRAP_PRINT_TO_STR(ell::dataset::SparseDoubleDataVector)
WRAP_PRINT_TO_STR(ell::dataset::SparseFloatDataVector) 
WRAP_PRINT_TO_STR(ell::dataset::SparseShortDataVector)
