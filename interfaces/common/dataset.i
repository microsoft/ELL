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


namespace dataset
{
    %ignore RowDataset;
    %ignore IDataVector::Clone;

    %ignore GenericSupervisedExample::GetDataVector;
    %ignore GenericSupervisedExample::GenericSupervisedExample;

    %ignore DataRow<void>;
    %ignore DenseDataVector::operator[];
    %ignore DenseDataVector<double>;
    %ignore DenseDataVector<float>;
    %ignore SparseDataVector<double, utilities::CompressedIntegerList>;
    %ignore SparseDataVector<float, utilities::CompressedIntegerList>;
    %ignore SparseDataVector<short, utilities::CompressedIntegerList>;
}

namespace interfaces
{
    %ignore GenericRowDataset::GenericRowDataset(GenericRowDataset &&);
}

%{
#define SWIG_FILE_WITH_INIT
#include "IVector.h"
#include "DenseDataVector.h"
#include "IDataVector.h"
#include "SparseDataVector.h"
#include "SupervisedExample.h"
#include "RowDatasetInterface.h"
%}

namespace utilities
{
    class IIterator;
}

%include "noncopyable.i"

%include "IVector.h"
%include "DenseDataVector.h"
%include "IDataVector.h"
%include "SparseDataVector.h"
%include "SupervisedExample.h"

namespace dataset
{
    wrap_noncopyable(SupervisedExample<IDataVector>);
    %template() SupervisedExample<IDataVector>;
}

%include "RowDatasetInterface.h"


%include "unique_ptr.i"
wrap_unique_ptr(IDataVectorPtr, dataset::IDataVector)

namespace dataset
{
    // The following template definitions are necessary to eliminate the "warning 315: Nothing known about ..." messages
    %template () DenseDataVector<double>;
    %template () DenseDataVector<float>;
    %template () SparseDataVector<double, utilities::CompressedIntegerList>;
    %template () SparseDataVector<float, utilities::CompressedIntegerList>;
    %template () SparseDataVector<short, utilities::CompressedIntegerList>;

    // Bafflingly, the below causes SWIG to give an error about no default constructor for SparseDataVector<>
    // %template (SparseDoubleDataVectorBase) SparseDataVector<double, utilities::CompressedIntegerList>;
    // %template (SparseFloatDataVectorBase) SparseDataVector<float, utilities::CompressedIntegerList>;
    // %template (SparseShortDataVectorBase) SparseDataVector<short, utilities::CompressedIntegerList>;

    // wrap operator[] for python
    WRAP_OP_AT(DenseDataVector, double)

    // wrap "Print" method for python
    //WRAP_PRINT_TO_STR(GenericSupervisedExample)
    WRAP_PRINT_TO_STR(SupervisedExample<dataset::IDataVector>)
    WRAP_PRINT_TO_STR(FloatDataVector)
    WRAP_PRINT_TO_STR(DoubleDataVector)

    //WRAP_PRINT_TO_STR(SparseDoubleDataVector)
    //WRAP_PRINT_TO_STR(SparseFloatDataVector)
    //WRAP_PRINT_TO_STR(SparseShortDataVector)
}
