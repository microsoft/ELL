////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     AutoDataVector.cpp (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "AutoDataVector.h"
#include "DenseDataVector.h"

namespace emll
{
namespace dataset
{ 
    AutoDataVector::AutoDataVector(std::initializer_list<linear::IndexValue> list)
    {
        DoubleDataVector v(list);
        _pInternal = std::make_unique<FloatDataVector>(v.GetIterator());
    }

    AutoDataVector::AutoDataVector(std::initializer_list<double> list)
    {
        DoubleDataVector v(list);
        _pInternal = std::make_unique<FloatDataVector>(v.GetIterator());
    }

    void AutoDataVector::Print(std::ostream & os) const
    {
        _pInternal->Print(os);
    }

    void AutoDataVector::AddTo(double * p_other, double scalar) const
    {
        _pInternal->AddTo(p_other, scalar);
    }

    double AutoDataVector::Dot(const double * p_other) const
    {
        return _pInternal->Dot(p_other);
    }

    void AutoDataVector::AppendEntry(size_t index, double value)
    {
        _pInternal->AppendEntry(index, value);
    }
}
}