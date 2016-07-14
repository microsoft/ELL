////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     WeightLabel.h (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <ostream>

namespace dataset
{
    class WeightLabel
    {
    public:
        WeightLabel(double weight, double label);

        WeightLabel(const WeightLabel&) = default;

        double GetWeight() const { return _weight; }

        double GetLabel() const { return _label; }

        void Print(std::ostream& os) const;

    private:

        double _weight;
        double _label;
    };
}