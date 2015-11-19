// SupervisedExample.cpp

#include "SupervisedExample.h"

using std::move;

namespace dataset
{
    SupervisedExample::SupervisedExample(unique_ptr<IDataVector> instance, double label, double weight) : _upInstance(move(instance)), _label(label), _weight(weight)
    {}

    double SupervisedExample::GetWeight() const
    {
        return _weight;
    }

    double SupervisedExample::GetLabel() const
    {
        return _label;
    }
    
    IDataVector::type SupervisedExample::GetType() const
    {
        return type();
    }

    void SupervisedExample::Reset()
    {
        _upInstance->Reset();
    }

    void SupervisedExample::PushBack(uint64 index, double value)
    {
        _upInstance->PushBack(index, value);
    }

    //void SupervisedExample::foreach_nonzero(std::function<void(uint64, double)> func, uint64 index_offset) const
    //{
    //    _upInstance->foreach_nonzero(func, index_offset);
    //}

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

    double SupervisedExample::Dot(const double * p_other) const
    {
        return _upInstance->Dot(p_other);
    }

    void SupervisedExample::Print(ostream & os) const
    {
        _upInstance->Print(os);
    }
}