////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ConstantPredictor.cpp (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ConstantPredictor.h"

// stl
#include <string>

namespace predictors
{
    ConstantPredictor::ConstantPredictor(double value) : _value(value)
    {}

    void ConstantPredictor::PrintLine(std::ostream& os, size_t tabs) const
    {
        os << std::string(tabs * 4, ' ') << "constant = " << _value << "\n";
    }
}