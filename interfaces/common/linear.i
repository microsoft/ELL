////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     linear.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

//%module linear

%{
#define SWIG_FILE_WITH_INIT
#include "DoubleVector.h"
#include "DoubleSubvector.h"
#include "DoubleMatrix.h"
#include "IVector.h"
#include "IMatrix.h"
%}

%ignore linear::DoubleVector::GetIterator;
%ignore linear::DoubleSubvector::GetIterator;
%ignore linear::DoubleVector::GetDataPointer;
%ignore linear::DoubleVector::DoubleVector(DoubleVector &&);
%ignore linear::DoubleVector::DoubleVector(std::vector<double> &&);

%ignore linear::DoubleVector::operator[];
%rename (GetValue) linear::DoubleVector::operator[](uint64_t) const;
%rename (GetValue) linear::DoubleSubvector::operator[](uint64_t) const;
%ignore linear::DoubleMatrixBase::operator()const;


%include "DoubleVector.h"
%include "DoubleSubvector.h"
%include "DoubleMatrix.h"
%include "IVector.h"
%include "IMatrix.h"

WRAP_OP_AT(linear::DoubleVector, double)
// WRAP_OP_AT(linear::DoubleSubvector, double)

%extend linear::DoubleMatrix
{
    const double& __getitem__(size_t i, size_t j)
    {
        return (*$self)(i,j);
    }
};

WRAP_PRINT_TO_STR(linear::DoubleVector)
WRAP_PRINT_TO_STR(linear::DoubleSubvector)
WRAP_PRINT_TO_STR(linear::DoubleMatrix)
