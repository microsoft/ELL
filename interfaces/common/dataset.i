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
#include "SupervisedExample.h"
#include "RowDatasetInterface.h"
%}

%ignore dataset::RowDataset::operator[];
namespace dataset
{
    %ignore IDataVector::Clone;
    %ignore SupervisedExample(SupervisedExample<IDataVector>&& other);
    %ignore GenericSupervisedExample::GenericSupervisedExample(GenericSupervisedExample&& other);
    %ignore GenericSupervisedExample::GetDataVector;
    %ignore GenericSupervisedExample::GenericSupervisedExample;

    %ignore DenseDataVector::operator[];
    %ignore DenseDataVector<double>;
    %ignore DenseDataVector<float>;
    %ignore SparseDataVector<double, utilities::CompressedIntegerList>;
    %ignore SparseDataVector<float, utilities::CompressedIntegerList>;
    %ignore SparseDataVector<short, utilities::CompressedIntegerList>;
}

%ignore dataset::SupervisedExample<dataset::IDataVector>::SupervisedExample(SupervisedExample&&);
%ignore interfaces::GenericRowDataset::GenericRowDataset(GenericRowDataset &&);

%include "noncopyable.i"

%include "IVector.h"
%include "IDataVector.h"
%include "DenseDataVector.h"
%include "SparseDataVector.h"
%include "SupervisedExample.h"
%include "RowDataset.h"

%template() dataset::RowDataset<dataset::IDataVector>;
%template(xx) dataset::DenseDataVector<double>;
%template(DenseSupervisedExample) dataset::SupervisedExample<dataset::DoubleDataVector>;

namespace dataset
{
    wrap_noncopyable(SupervisedExample<IDataVector>);
    %template() SupervisedExample<IDataVector>;
    %template() RowDataset<IDataVector>;
}

%include "RowDatasetInterface.h"
%import "RowDataset.h"

%include "unique_ptr.i"
wrap_unique_ptr(IDataVectorPtr, dataset::IDataVector)


namespace dataset
{
//    %template (GenericSupervisedExample) SupervisedExample<IDataVector>;

    // The following template definitions are necessary to eliminate the "warning 315: Nothing known about ..." messages
    %template () DenseDataVector<double>;
    %template () DenseDataVector<float>;
    %template () SparseDataVector<double, utilities::CompressedIntegerList>;
    %template () SparseDataVector<float, utilities::CompressedIntegerList>;
    %template () SparseDataVector<short, utilities::CompressedIntegerList>;
    %template () RowDataset<IDataVector>;
    
    // Bafflingly, the below causes SWIG to give an error about no default constructor for SparseDataVector<>
    // %template (SparseDoubleDataVectorBase) SparseDataVector<double, utilities::CompressedIntegerList>;
    // %template (SparseFloatDataVectorBase) SparseDataVector<float, utilities::CompressedIntegerList>;
    // %template (SparseShortDataVectorBase) SparseDataVector<short, utilities::CompressedIntegerList>;

    // wrap operator[] for python
    WRAP_OP_AT(DoubleDataVector, double)

    // wrap "Print" method for python
    WRAP_PRINT_TO_STR(SupervisedExample<IDataVector>)
    WRAP_PRINT_TO_STR(FloatDataVector)
    WRAP_PRINT_TO_STR(DoubleDataVector)

    WRAP_PRINT_TO_STR(SparseDoubleDataVector)
    WRAP_PRINT_TO_STR(SparseFloatDataVector) 
    WRAP_PRINT_TO_STR(SparseShortDataVector)
}
