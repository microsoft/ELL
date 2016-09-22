////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     linear.i (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

%warnfilter (315) std::enable_if_t< std::is_base_of< IIndexValueIterator,IteratorType >::value,int >;

%{
#define SWIG_FILE_WITH_INIT
#include "DoubleVector.h"
#include "DoubleSubvector.h"
#include "DoubleMatrix.h"
#include "IndexValue.h"
#include "IVector.h"
#include "IMatrix.h"
%}

%ignore std::enable_if_t;
%ignore emll::linear::IsIndexValueIterator;
%ignore emll::linear::DoubleVector::GetIterator;
%ignore emll::linear::DoubleSubvector::GetIterator;
%ignore emll::linear::DoubleVector::GetDataPointer;
%ignore emll::linear::DoubleVector::DoubleVector(DoubleVector &&);
%ignore emll::linear::DoubleVector::DoubleVector(std::vector<double> &&);

%ignore emll::linear::DoubleVector::operator[];
%ignore emll::linear::DoubleSubvector::operator[];
%rename (GetValue) emll::linear::DoubleVector::operator[](uint64_t) const;
%rename (GetValue) emll::linear::DoubleSubvector::operator[](uint64_t) const;
%ignore emll::linear::DoubleMatrixBase::operator()const;

%include "IVector.h"
%include "DoubleVector.h"
%include "DoubleSubvector.h"
%include "DoubleMatrix.h"

%include "IndexValue.h"
%include "IMatrix.h"

WRAP_OP_AT(emll::linear::DoubleVector, double)
// WRAP_OP_AT(emll::linear::DoubleSubvector, double)

#ifdef SWIGPYTHON
%extend emll::linear::DoubleMatrix
{
    const double& __getitem__(size_t i, size_t j)
    {
        return (*$self)(i,j);
    }
};

#endif

WRAP_PRINT_TO_STR(emll::linear::DoubleVector)
WRAP_PRINT_TO_STR(emll::linear::DoubleSubvector)
WRAP_PRINT_TO_STR(emll::linear::DoubleMatrix)
