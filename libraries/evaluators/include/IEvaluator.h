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

namespace evaluators
{
    template <typename PredictorType>
    class IEvaluator 
    {
    public:

        virtual ~IEvaluator() = default;

    };
}