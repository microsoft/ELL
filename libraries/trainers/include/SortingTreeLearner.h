////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     SortingTreeLearner.h (trainers)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// predictors
#include "LinearPredictor.h"

// loss functions
#include "SquaredLoss.h"
#include "LogLoss.h"

// linear
#include "DoubleVector.h"

// dataset
#include "RowDataset.h"
#include "SupervisedExample.h"

// utilities
#include "Iterator.h"

// stl
#include <cstdint>

namespace trainers
{
    template <typename LossFunctionType>
    class SortingTreeLearner
    {
    public:

        SortingTreeLearner(LossFunctionType lossFunction);

        /// <summary> Performs a given number of learning iterations. </summary>
        ///
        /// <typeparam name="ExampleIteratorType"> Type of example iterator to use. </typeparam>
        /// <param name="exampleIterator"> [in,out] The data iterator. </param>
        /// <param name="numExamples"> The number of examples in the iterator. </param>
        template<typename ExampleIteratorType>
        void Update(ExampleIteratorType& exampleIterator);

    private:
        LossFunctionType _lossFunction;
    };
}

#include "../tcc/SortingTreeLearner.tcc"
