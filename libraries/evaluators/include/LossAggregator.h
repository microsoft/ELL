////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     LossAggregator.h (evaluators)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <cstdint>

namespace evaluators
{
    template <typename LossFunctionType>
    class LossAggregator
    {
    public:

        LossAggregator(const LossFunctionType& lossFunction);

        void Update(double prediction, double label, double weight);

    private:
        LossFunctionType _lossFunction;
        double _sumWeights = 0.0;
        double _sumWeightedLoss = 0.0;
    };
}

#include "../tcc/LossAggregator.tcc"
