////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     LogitBooster.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// dataset
#include "WeightLabel.h"

namespace trainers
{
    class LogitBooster
    {
    public:
        /// <summary> Constructs an instance of LogitBooster. </summary>
        ///
        /// <param name="scale"> The scale applied to the exponent in the logloss. </param>
        LogitBooster(double scale=1.0);

        /// <summary> Calculates weak weight and weak label. </summary>
        ///
        /// <param name="strongWeightLabel"> The strong weight and label. </param>
        /// <param name="prediction"> The prediction. </param>
        ///
        /// <returns> The weak weight and label. </returns>
        dataset::WeightLabel GetWeakWeightLabel(const dataset::WeightLabel& strongWeightLabel, double prediction) const;

    private:
        double _scale;
    };
}