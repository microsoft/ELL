////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     WeightLabel.cpp (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WeightLabel.h"

namespace dataset
{
    dataset::WeightLabel::WeightLabel(double weight, double label) : weight(weight), label(label)
    {}

    void dataset::WeightLabel::Print(std::ostream & os) const
    {
        os << "(" << weight << ", " << label << ")";
    }
}