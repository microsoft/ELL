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
#include "RowDataset.h"
#include "StlIterator.h"
%}

// ignores
%ignore emll::dataset::RowDataset::operator[];
%ignore emll::dataset::IDataVector::Clone;
%ignore emll::dataset::GenericRowDataset::GenericRowDataset(emll::dataset::GenericRowDataset&&);
%ignore emll::dataset::RowDataset<emll::dataset::GenericSupervisedExample>::RowDataset(emll::dataset::RowDataset<emll::dataset::GenericSupervisedExample>&&);
%ignore emll::dataset::GenericSupervisedExample::GenericSupervisedExample;
%ignore emll::dataset::GenericSupervisedExample::GenericSupervisedExample(emll::dataset::GenericSupervisedExample&&);
%ignore emll::dataset::DenseSupervisedExample::DenseSupervisedExample(emll::dataset::DenseSupervisedExample&&);
%ignore emll::dataset::RowDataset< emll::dataset::GenericSupervisedExample >::RowDataset(emll::dataset::RowDataset< emll::dataset::GenericSupervisedExample > &&);
%ignore emll::dataset::Example< emll::dataset::IDataVector,emll::dataset::WeightLabel >::Example(emll::dataset::Example< emll::dataset::IDataVector,emll::dataset::WeightLabel > &&);
%ignore emll::dataset::SparseDataVector<double, emll::utilities::CompressedIntegerList>;
%ignore emll::dataset::SparseDataVector<float, emll::utilities::CompressedIntegerList>;
%ignore emll::dataset::SparseDataVector<short, emll::utilities::CompressedIntegerList>;
%ignore emll::dataset::Example<emll::dataset::IDataVector, emll::dataset::WeightLabel>::Example;

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

wrap_unique_ptr(IDataVectorPtr, emll::dataset::IDataVector)

%template (GenericSupervisedExample) emll::dataset::Example<emll::dataset::IDataVector, emll::dataset::WeightLabel>;
%template (GenericRowDataset) emll::dataset::RowDataset<emll::dataset::GenericSupervisedExample>;
%template (GenericRowIterator) emll::utilities::StlIterator<typename std::vector<emll::dataset::Example<emll::dataset::IDataVector, emll::dataset::WeightLabel>>::const_iterator, emll::dataset::Example<emll::dataset::IDataVector, emll::dataset::WeightLabel>>;

// The following template definitions are necessary to eliminate the "warning 315: Nothing known about ..." messages
%template () emll::dataset::DenseDataVector<double>;
%template () emll::dataset::DenseDataVector<float>;
%template () emll::dataset::SparseDataVector<double, emll::utilities::CompressedIntegerList>;
%template () emll::dataset::SparseDataVector<float, emll::utilities::CompressedIntegerList>;
%template () emll::dataset::SparseDataVector<short, emll::utilities::CompressedIntegerList>;

// wrap operator[] for python
WRAP_OP_AT(emll::dataset::DoubleDataVector, double)

// wrap "Print" method for python
//    WRAP_PRINT_TO_STR(GenericSupervisedExample)
WRAP_PRINT_TO_STR(emll::dataset::FloatDataVector)
WRAP_PRINT_TO_STR(emll::dataset::DoubleDataVector)

WRAP_PRINT_TO_STR(emll::dataset::SparseDoubleDataVector)
WRAP_PRINT_TO_STR(emll::dataset::SparseFloatDataVector) 
WRAP_PRINT_TO_STR(emll::dataset::SparseShortDataVector)
