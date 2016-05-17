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
    void predictors::EnsemblePredictor<BasePredictorType>::AddToModel(layers::Model& model, layers::CoordinateList inputCoordinates) const 
    {
        // TODO
    }
}