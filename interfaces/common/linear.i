////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     linear.i (interfaces)
//  Authors:  Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%module linear

%{
#define SWIG_FILE_WITH_INIT
#include "DoubleVector.h"
#include "DoubleSubvector.h"
#include "DoubleMatrix.h"
%}

%ignore linear::DoubleVector::GetIterator;
%ignore linear::DoubleSubvector::GetIterator;

%rename (GetValue) linear::DoubleVector::operator[](uint64_t) const;
%rename (GetValue) linear::DoubleSubvector::operator[](uint64_t) const;

%include "DoubleVector.h"
%include "DoubleSubvector.h"
%include "DoubleMatrix.h"

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
