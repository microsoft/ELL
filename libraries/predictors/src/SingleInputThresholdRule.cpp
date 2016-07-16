////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SingleInputThresholdRule.cpp (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SingleInputThresholdRule.h"

namespace predictors
{
    SingleInputThresholdRule::SingleInputThresholdRule(size_t index, double threshold) : _index(index), _threshold(threshold)
    {}

    void SingleInputThresholdRule::PrintLine(std::ostream & os, size_t tabs) const
    {
        os << std::string(tabs * 4, ' ') << "index = " << _index << ", threshold = " << _threshold << "\n";
    }
}