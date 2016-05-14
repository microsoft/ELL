////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IStatefulTrainer.h (trainers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// dataset
#include "RowDataset.h"

namespace trainers
{
    template <typename PredictorType>
    class IStatefulTrainer
    {
    public:

        virtual ~IStatefulTrainer() = default;

        virtual void Update(dataset::GenericRowDataset::Iterator exampleIterator) = 0;
        
        virtual const PredictorType& GetPredictor() const = 0;
        
        virtual PredictorType Reset() = 0;
    };
}