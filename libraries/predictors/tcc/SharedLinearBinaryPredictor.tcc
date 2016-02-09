////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     SharedLinearBinaryPredictor.tcc (predictors)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace predictors
{
    template<typename DataVectorType>
    double SharedLinearBinaryPredictor::Predict(const DataVectorType& example) const
    {
        return example.Dot(_sp_predictor->w) + _sp_predictor->b;
    }
}
