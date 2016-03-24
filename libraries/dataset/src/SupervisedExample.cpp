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
    SupervisedExample::SupervisedExample(std::unique_ptr<IDataVector> instance, double label, double weight) : _dataVector(std::move(instance)), _label(label), _weight(weight)
    {}

    double SupervisedExample::GetWeight() const
    {
        return _weight;
    }

    double SupervisedExample::GetLabel() const
    {
        return _label;
    }

    const IDataVector & SupervisedExample::GetDataVector() const
    {
        return *_dataVector.get();
    }

    void SupervisedExample::Print(std::ostream & os) const
    {
        os << _label << '\t';
        _dataVector->Print(os);
    }

    std::ostream & operator<<(std::ostream & ostream, const SupervisedExample & example)
    {
        example.Print(ostream);
        return ostream;
    }
}
