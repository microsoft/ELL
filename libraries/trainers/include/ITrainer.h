////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ITrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// dataset
#include "RowDataset.h"

namespace trainers
{
    template <typename PredictorType>
    class ITrainer
    {
    public:

        virtual ~ITrainer() = default;

        virtual PredictorType Train(dataset::GenericRowDataset::Iterator exampleIterator) const = 0;
    };
}