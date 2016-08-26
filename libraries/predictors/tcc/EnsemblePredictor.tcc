////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     EnsemblePredictor.tcc (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace predictors
{
    template <typename BasePredictorType>
    void EnsemblePredictor<BasePredictorType>::AppendPredictor(BasePredictorType basePredictor, double weight)
    {
        _basePredictors.push_back(std::move(basePredictor));
        _weights.push_back(weight);
    }

    template<typename BasePredictorType>
    double predictors::EnsemblePredictor<BasePredictorType>::Predict(const dataset::IDataVector& dataVector) const 
    {
        double output = 0.0;
        for(uint64_t i=0; i<_basePredictors.size(); ++i)
        {
            output += _weights[i] * _basePredictors[i].Predict(dataVector);
        }
        return output;
    }
}