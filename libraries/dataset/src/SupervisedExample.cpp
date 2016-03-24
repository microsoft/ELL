////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     SupervisedExample.cpp (dataset)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SupervisedExample.h"

namespace dataset
{
    SupervisedExample::SupervisedExample(std::unique_ptr<IDataVector> instance, double label, double weight) : _upInstance(std::move(instance)), _label(label), _weight(weight)
    {}

    double SupervisedExample::GetWeight() const
    {
        return _weight;
    }

    double SupervisedExample::GetLabel() const
    {
        return _label;
    }
    
    void SupervisedExample::Reset()
    {
        _upInstance->Reset();
    }

    void SupervisedExample::PushBack(uint64 index, double value)
    {
        _upInstance->PushBack(index, value);
    }

    uint64 SupervisedExample::Size() const
    {
        return _upInstance->Size();
    }

    uint64 SupervisedExample::NumNonzeros() const
    {
        return _upInstance->NumNonzeros();
    }

    double SupervisedExample::Norm2() const
    {
        return _upInstance->Norm2();
    }

    void SupervisedExample::AddTo(double * p_other, double scalar) const
    {
        _upInstance->AddTo(p_other, scalar);
    }

    void SupervisedExample::AddTo(std::vector<double>& other, double scalar) const
    {
        AddTo(other.data(), scalar);
    }

    double SupervisedExample::Dot(const std::vector<double>& other) const
    {
        return _upInstance->Dot(other.data());
    }

    double SupervisedExample::Dot(const double * p_other) const
    {
        return _upInstance->Dot(p_other);
    }

    void SupervisedExample::Print(std::ostream & os) const
    {
        os << _label << '\t';
        _upInstance->Print(os);
    }

    std::ostream & operator<<(std::ostream & ostream, const SupervisedExample & example)
    {
        example.Print(ostream);
        return ostream;
    }
}
