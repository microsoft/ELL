////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     EnsemblePredictor.tcc (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// layers
#include "Coordinatewise.h"
#include "Sum.h"

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

    template<typename BasePredictorType>
    layers::CoordinateList predictors::EnsemblePredictor<BasePredictorType>::AddToModel(layers::Model& model, layers::CoordinateList inputCoordinates) const 
    {
        layers::CoordinateList basePredictorOutputCoordinateList;

        for(uint64_t i = 0; i<_basePredictors.size(); ++i)
        {
            auto outputCoordinates = _basePredictors[i].AddToModel(model, inputCoordinates);
            basePredictorOutputCoordinateList.AddCoordinate(outputCoordinates[0]);
        }

        auto weightsLayer = std::make_unique<layers::Coordinatewise>(std::vector<double>(_weights), std::move(basePredictorOutputCoordinateList), layers::Coordinatewise::OperationType::multiply);
        auto weightsLayerCoordinates = model.AddLayer(std::move(weightsLayer));

        auto sumLayer = std::make_unique<layers::Sum>(std::move(weightsLayerCoordinates));
        auto sumLayerCoordinates = model.AddLayer(std::move(sumLayer));

        return sumLayerCoordinates;
    }
}