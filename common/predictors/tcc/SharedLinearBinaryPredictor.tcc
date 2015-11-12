// SharedLinearBinaryPredictor.tcc

namespace predictors
{
    template<typename DatavectorType>
    double SharedLinearBinaryPredictor::Predict(const DatavectorType& example) const
    {
        return example.Dot(_sp_predictor->w) + _sp_predictor->b;
    }
}