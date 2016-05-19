////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IEvaluator.h (evaluators)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// dataset
#include "RowDataset.h"

// stl
#include <iostream>

namespace evaluators
{
    template <typename PredictorType>
    class IEvaluator 
    {
    public:

        virtual ~IEvaluator() = default;

        virtual void Evaluate(const PredictorType& predictor) = 0;

        virtual void Print(std::ostream& os) const = 0;
    };
}