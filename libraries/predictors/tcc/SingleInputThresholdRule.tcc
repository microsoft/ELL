////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SingleInputThresholdRule.tcc (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace predictors
{
    template<typename RandomAccessVectorType>
    size_t predictors::SingleInputThresholdRule::Compute(const RandomAccessVectorType& inputVector) const
    {
        return inputVector[_index] > _threshold ? 1 : 0;
    }
}