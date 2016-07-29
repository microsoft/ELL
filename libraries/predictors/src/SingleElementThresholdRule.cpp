////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SingleElementThresholdRule.cpp (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SingleElementThresholdRule.h"

namespace predictors
{
    SingleElementThresholdRule::SingleElementThresholdRule(size_t index, double threshold) : _index(index), _threshold(threshold)
    {}

    void SingleElementThresholdRule::PrintLine(std::ostream & os, size_t tabs) const
    {
        os << std::string(tabs * 4, ' ') << "index = " << _index << ", threshold = " << _threshold << "\n";
    }
}