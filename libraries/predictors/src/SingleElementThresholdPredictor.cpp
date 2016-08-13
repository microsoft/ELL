////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SingleElementThresholdPredictor.cpp (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SingleElementThresholdPredictor.h"

namespace predictors
{
    SingleElementThresholdPredictor::SingleElementThresholdPredictor(size_t index, double threshold) : _index(index), _threshold(threshold)
    {}

    void SingleElementThresholdPredictor::PrintLine(std::ostream & os, size_t tabs) const
    {
        os << std::string(tabs * 4, ' ') << "index = " << _index << ", threshold = " << _threshold << "\n";
    }
}