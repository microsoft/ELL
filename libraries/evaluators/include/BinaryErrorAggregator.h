////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BinaryErrorAggregator.h (evaluators)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <cstdint>
#include <iostream>

namespace evaluators
{
    class BinaryErrorAggregator
    {
    public:
        void Update(double prediction, double label, double weight);

        void Print(std::ostream& os) const;

    private:
        double _sumWeightedTruePositives = 0.0;
        double _sumWeightedTrueNegatives = 0.0;
        double _sumWeightedFalsePositives = 0.0;
        double _sumWeightedFalseNegatives = 0.0;
    };
}