////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SupervisedExample.cpp (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SupervisedExample.h"

namespace dataset
{
    SupervisedExample::SupervisedExample(const SupervisedExample& other) : _dataVector(other._dataVector->Clone()), _label(other._label), _weight(other._weight) 
    {
    }

    SupervisedExample::SupervisedExample(std::unique_ptr<IDataVector> instance, double label, double weight) : _dataVector(std::move(instance)), _label(label), _weight(weight)
    {
    }

    SupervisedExample& SupervisedExample::operator=(SupervisedExample other)
    {
        swap(*this, other);
        return *this;
    }

    const IDataVector& SupervisedExample::GetDataVector() const
    {
        return *_dataVector.get();
    }

    double SupervisedExample::GetWeight() const
    {
        return _weight;
    }

    double SupervisedExample::GetLabel() const
    {
        return _label;
    }

    void SupervisedExample::Print(std::ostream & os) const
    {
        os << _label << '\t';
        GetDataVector().Print(os);
    }

    std::ostream & operator<<(std::ostream & ostream, const SupervisedExample & example)
    {
        example.Print(ostream);
        return ostream;
    }

    void swap(SupervisedExample& a, SupervisedExample &b)
    {
        using std::swap;
        swap(a._dataVector, b._dataVector);
        swap(a._label, b._label);
        swap(a._weight, b._weight);
    }

}
