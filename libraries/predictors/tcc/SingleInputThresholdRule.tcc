////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SingleInputThresholdRule.tcc (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Exception.h"

namespace predictors
{
    template<typename RandomAccessVectorType>
    int predictors::SingleInputThresholdRule::Compute(const RandomAccessVectorType& inputVector) const
    {
        if (inputVector.size() <= _index)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange);
        }

        return inputVector[_index] > _threshold ? 1 : 0;
    }
}