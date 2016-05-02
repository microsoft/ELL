////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     SortingTreeLearner.h (trainers)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// predictors
#include "DecisionTree.h"

namespace trainers
{
    template <typename LossFunctionType>
    class SortingTreeLearner
    {
    public:

        /// <summary> Constructs an instance of SortingTreeLearner. </summary>
        ///
        /// <param name="lossFunction"> The loss function. </param>
        SortingTreeLearner(LossFunctionType lossFunction);

        /// <summary> Trains a decision tree. </summary>
        ///
        /// <typeparam name="ExampleIteratorType"> Type of the example iterator. </typeparam>
        /// <param name="exampleIterator"> [in,out] The example iterator. </param>
        ///
        /// <returns> A decision tree. </returns>
        template <typename ExampleIteratorType>
        predictors::DecisionTree Train(ExampleIteratorType& exampleIterator, uint64_t maxDataVectorSize);

    private:
        LossFunctionType _lossFunction;
    };
}

#include "../tcc/SortingTreeLearner.tcc"
