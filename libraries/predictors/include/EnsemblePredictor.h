////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     EnsemblePredictor.h (predictors)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// dataset
#include "IDataVector.h"

// layers
#include "Model.h"
#include "CoordinateList.h"

// stl
#include <vector>

namespace predictors
{
    /// <summary> Implements an ensemble (weighted sum) of a base predictor type. </summary>
    ///
    /// <typeparam name="BasePredictorType"> The base predictor type. </typeparam>
    template <typename BasePredictorType>
    class EnsemblePredictor
    {
    public:

        /// <summary> Appends a predictor to the ensemble </summary>
        ///
        /// <param name="basePredictor"> The predictor. </param>
        /// <param name="weight"> The weight of the predictor. </param>
        void AppendPredictor(BasePredictorType basePredictor, double weight=1.0);

        /// <summary> Returns the output of the predictor for a given example. </summary>
        ///
        /// <param name="example"> The data vector. </param>
        ///
        /// <returns> A double. </returns>
        double Predict(const dataset::IDataVector& dataVector) const;

        /// <summary> Adds the ensemble to a model. </summary>
        ///
        /// <param name="model"> [in,out] The model. </param>
        /// <param name="inputCoordinates"> The input coordinates. </param>
        ///
        ///  <returns> The predictor's otuput coordinates in the model. </returns>
        layers::CoordinateList AddToModel(layers::Model& model, layers::CoordinateList inputCoordinates) const;

    private:
        std::vector<BasePredictorType> _basePredictors;
        std::vector<double> _weights;
    };
}

#include "../tcc/EnsemblePredictor.tcc"
